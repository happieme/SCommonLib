#include "../../CommonLib/CommonLibSrc/SString/String.h"
#include "../../CommonLib/CommonLibSrc/SSystem/SSystem.h"
#include "../../CommonLib/CommonLibSrc/STime/STime.h"
#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"
#include "../../CommonLib/CommonLibSrc/SCrypt/SCrypt.h"
#include "../../CommonLib/CommonLibSrc/SFile/SFile.h"

namespace smm
{
	void TestSCrypt()
	{
		String Src;
		SFile sf("D:\\cygwin\\home\\image\\jia.jpg");
		String Dst;
		String Res;
		if(sf.Open())
		{
			sf.Read(Src);
			cout<<"SrcSize: "<<Src.Size()<<endl;
			SCrypt::EnCrypt(Src , Dst);
			cout<<"After EnCrypt: "<<Dst.Size()<<endl;
			SCrypt::DeCrypt(Dst , Res);
			cout<<"After DeCrypt: "<<Res.Size()<<endl;
			cout<<"ͼƬ���������"<<Res.IsEqual(Src,true)<<endl;
		}

		Src = "host=localhost;db=devexample;user=root;pwd=1230;charset=utf8;";
		// ����
		cout<<"����ǰ��"<<endl<<Src.Size()<<endl<<Src.ToCStr()<<endl;
		SCrypt::EnCrypt(Src , Dst);
		cout<<"���ܺ�"<<endl<<Dst.Size()<<endl<<Dst.ToCStr()<<endl;
		// ����
		SCrypt::DeCrypt(Dst , Res);
		cout<<"���ܺ�"<<endl<<Res.Size()<<endl<<Res.ToCStr()<<endl;

		if(Src.IsEqual(Res,true))
		{	cout<<"���ܳɹ���"<<endl;	}
		else
		{	cout<<"����ʧ�ܣ�"<<endl;	}
	}

	void TestSBase64( )
	{	// "Provider=SQLOLEDB;Data Source=testdb02,1433;Initial Catalog=r6erp;Uid=hkdb;Pwd=hkdb6688"
		// "Logical negationBitwise complementPre-incrementPre-decrementUnary minusUnary plusDereferenceAddress of12"
		StrImg imSrc("host=localhost;db=devexample;user=root;pwd=1230;charset=utf8;");
		cout<<"Դ���룺"<<endl<<"���ȣ�"<<imSrc.Size()<<endl<<imSrc.ToCStr()<<endl;
		String EncDst;
		SCrypt::Base64_Encode(imSrc , EncDst);
		cout<<"���ܺ�"<<endl<<"���ȣ�"<<EncDst.Size()<<endl<<EncDst.ToCStr()<<endl;
		String DecDst;
		SCrypt::Base64_Decode(EncDst, DecDst);
		cout<<"���ܺ�"<<endl<<"���ȣ�"<<DecDst.Size()<<endl<<DecDst.ToCStr()<<endl;
		if(imSrc.IsEqual(DecDst,true))
		{	cout<<"�ӽ���ǰ��һ�£�"<<endl;	}
		StrImg imUsr("happieme");
		SCrypt::Base64_Encode(imUsr , EncDst);
		cout<<EncDst.ToCStr()<<endl;
		StrImg imPwd("happie810");
		SCrypt::Base64_Encode(imPwd , EncDst);
		cout<<EncDst.ToCStr()<<endl;
	}
};
