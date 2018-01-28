#include "../../CommonLib/CommonLibSrc/SString/String.h"
#include "../../CommonLib/CommonLibSrc/SString/SConvert.h"
#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"
#include <locale>
namespace smm
{
	void TestSConv()
	{
#ifdef WIN32
		::setlocale(LC_ALL, "chs");
		String ansi("分数-130");
		cout<<"转换前（Ansi）："<<endl<<"\t\t"<<ansi.ToCStr()<<" , Size : "<<ansi.Size()<<endl;
		String wStr;
		SConv::A_U(ansi,wStr);
		const CharW* pW = (const CharW*)(wStr.ToCStr());
		cout<<"转换后（Unicode）："<<endl;
		for(Uint i=0;i<wStr.Size();i++)
		{	cout<<Uint(wStr.GetAt(i))<<"  ";	}
		cout<<endl;
		wcout<<L"\t\t"<<pW<<L" , Size : "<<wStr.Size()<<endl;
		SConv::U_A(wStr,ansi);
		cout<<"Unicode 转换为 Ansi："<<endl<<"\t\t"<<ansi.ToCStr()<<" , Size : "<<ansi.Size()<<endl;
		String u8Str;
		SConv::A_U8(ansi,u8Str);
		cout<<"ansi 转换为 utf8："<<endl<<"\t\t"<<u8Str.ToCStr()<<" , Size : "<<u8Str.Size()<<endl;
		{
			Uint n = u8Str.GetAt(0) & 0xE5;
			cout<<n<<endl;
			n = u8Str.GetAt(1) & 0x88;
			cout<<n<<endl;
			n = u8Str.GetAt(2) & 0x86;
			cout<<n<<endl;
		}

		SConv::U8_A(u8Str,ansi);
		cout<<"utf8 转换为 ansi："<<endl<<"\t\t"<<ansi.ToCStr()<<" , Size : "<<ansi.Size()<<endl;

		String strURL("http://192.168.1.102/hp/csp/a.htm?a=%E5%95%8A&mark=%2B&c=%E6%B3%95");
		String strDec;	// Utf8
		SConv::UrlDecode( strURL , strDec);
		String strAnsi;
		SConv::U8_A(strDec , strAnsi);
		cout<<"URL_Utf8_Decode："<<strAnsi.ToCStr()<<" , Size : "<<strAnsi.Size()<<endl;
#else
		String strU8("分数-130");
		cout<<"转换前（Utf8）："<<endl<<"\t\t"<<strU8.ToCStr()<<" , Size : "<<strU8.Size()<<endl;
		String wStr;
		SConv::U8_U(strU8,wStr);
		const CharW* pW = (const CharW*)(wStr.ToCStr());
		cout<<wStr.Size()<<endl;
		if(wStr.Size() > 1024)
		{	return ;	}
		cout<<"转换后（Unicode）："<<endl;
		for(int i=0;i<wStr.Size();i++)
		{	cout<<Uint(wStr.GetAt(i))<<"  ";	}
		cout<<endl;
		cout<<"转换回 U8 "<<endl;
		SConv::U_U8(wStr,strU8);
		cout<<"转换后（strU8）："<<endl<<"\t\t"<<strU8.ToCStr()<<" , Size : "<<strU8.Size()<<endl;
		String Ansi;
		cout<<"U 转换为 A"<<endl;
		SConv::U_A(wStr,Ansi);
		cout<<"转换后（Ansi）："<<endl<<"\t\t"<<Ansi.ToCStr()<<" , Size : "<<Ansi.Size()<<endl;

		SConv::U8_A(strU8,Ansi);
		cout<<"utf8 转换为 ansi："<<endl<<"\t\t"<<Ansi.ToCStr()<<" , Size : "<<Ansi.Size()<<endl;
		SConv::A_U8(Ansi,strU8);
		cout<<"ansi 转换为 utf8："<<endl<<"\t\t"<<strU8.ToCStr()<<" , Size : "<<strU8.Size()<<endl;

		String strURL("http://192.168.1.102/hp/csp/a.htm?a=%E5%95%8A&mark=%2B&c=%E6%B3%95");
		String strDec;	// Utf8
		SConv::UrlDecode( strURL , strDec);
		SConv::U8_A(strDec , Ansi);
		cout<<"URL_Utf8_Decode："<<Ansi.ToCStr()<<" , Size : "<<Ansi.Size()<<endl;
#endif
	}
};
