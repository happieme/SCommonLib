#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"
#include "../../CommonLib/CommonLibSrc/SString/String.h"
#include "../../CommonLib/CommonLibSrc/SSystem/SSystem.h"
#include "../../CommonLib/CommonLibSrc/STime/STime.h"
#include "../../CommonLib/CommonLibSrc/SCommon/SClib.h"
#include <vector>
#include <set>
#include <string>
#include <string.h>
#include <windows.h> 
using namespace std;

namespace smm
{
	// Test String	-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	void TestCString()
	{
		cout << "file: " << __FILE__ << ", func: " << __FUNCTION__ << ", line: " << __LINE__ << endl;
		SDEBUG("dafe",true);
		StrImg str("1f1d6sa1f3613201");
		SDEBUG(str,true);
		String str2("fffff");
		SDEBUG(str2,true);
		try
		{
			{
				StrImg str1;
				cout<<Uint(str1.ToCStr())<<endl;
				StrImg str2("adsf");
				cout<<Uint(str2.ToCStr())<<endl;
			}
			{
				StrImg imSrc("&&d=12&name=Mike&usr=sam&pwd=45782&&");
				StrSplit Splitter(imSrc,"&");
				StrImg imRes;
				for(Uint i=0;i<Splitter.GetCount();i++)
				{
					imRes = Splitter.GetAt(i);
					if(imRes.IsEmpty())
					{	cout<<"Empty"<<endl; }
					else
					{	cout<<imRes.Size()<<" : "<<imRes.ToCStr()<<endl;	}
				}
			}
			{
				StrImg str("D:\\MyFile\\mysql-src\\mysql-5.7.20\\storage\\innobase\\mtr\\mtr0mtr.cc");
				cout << "StrImg::Sub()" << endl;
				int pos1 = str.RFind("\\");
				int pos2 = str.RFind("\\", pos1-1);
				StrImg s = str.Sub(pos2 + 1);
				s.Show(); 
				str.Sub(2,3).Show();
				str.Sub(-2,3).Show();
				str.Sub(9,3).Show();
				str.Sub(1,9).Show();
			}
			{
				String A("asdf");
				String B("1234");
				String C("uiop");
				String D;
				D = A + B;
				cout<<D.ToCStr()<<endl;
				D.Swap(C);
				cout<<D.ToCStr()<<endl;
				D.Clear();
				D += A += B;
				cout<<D.ToCStr()<<endl;
				D = "A ";
				D += String("B ") += StrImg("C");
			}
			{
				cout<<"与 string 作对比"<<endl;
				Uint i = 0;
				const Uint nCount = 1000*100;
				const Uint nSpace = 1024*640;
				STime st;
				std::string str;
				Uint inpos = 0;
				str.insert(0,"123");
				str.clear();
				str.insert(0,"123");
				str.clear();
				str.insert(0,"123");
				str.clear();
				StrImg imTest("1234567897894561234567");
				const char* pSrc = imTest.ToCStr();
				const Uint SrcSize = imTest.Size();
				TimeT TimeUse = 0;
				String strC("uiop");
				strC.ReSize(36);
				st.TickStart();
			//	char TmpArr[1024];
				for(i=0;i<nCount;i++)
				{
				//	memcpy_s(TmpArr, 1024, pSrc, SrcSize);
				//	memcpy(TmpArr, pSrc, SrcSize);
				//	SClib::MemCpy(TmpArr, 1024, pSrc, SrcSize);
				//	SARRAY(strTmp, nSpace);
					SARRAYSTR(strTmp, 1024, imTest);
				//	String strC2(imTest);
				//	strC = imTest;
				/*	strTmp.ReSize(nSpace);
					strC.Append("12333333333333333333333333");
					strC.Append("156131351");
					strC.Insert(strC.Size()/2,"adsa00000fbc");
					strC.Clear();
				*/}
				TimeUse = st.TickStop();
				cout<<"smm::String : "<<nCount<<" , "<<strC.Size()<<" , "<<TimeUse<<endl;
				st.TickStart();
				for(i=0;i<nCount;i++)
				{
					string strA(pSrc);
				/*	strTmp.reserve(nSpace);
					str.append("12333333333333333333333333");
					str.append("156131351");
					str.insert(str.size()/2,"adsa00000fbc");
					str.clear();
				*/
				}
				TimeUse = st.TickStop();
				cout<<"stl::string : "<<nCount<<" , "<<str.size()<<" , "<<TimeUse<<endl;
			}
			{	// []
				StrImg imStr("asdf132");
				cout<<imStr[3]<<endl;
				String Str(imStr);
				cout<<Str[3]<<endl;
				Str[3] = 'W';
				cout<<Str[3]<<endl;
				cout<<Str.ToCStr()<<endl;
				cout<<imStr.ToCStr()<<endl;
			}
			{
				cout<<"ReSize()"<<endl;
				String str("123456");
				cout<<Uint(str.ToCStr())<<" , "<<str.ToCStr()<<endl;
				str.ReSize(9);
				cout<<Uint(str.ToCStr())<<" , "<<str.ToCStr()<<endl;
				str.ReSize(22);
				cout<<Uint(str.ToCStr())<<" , "<<str.ToCStr()<<endl;
				str.ReSize(14);
				cout<<Uint(str.ToCStr())<<" , "<<str.ToCStr()<<endl;
				str.ReSize(3,true);
				cout<<Uint(str.ToCStr())<<" , "<<str.ToCStr()<<endl;
			}
			{
				//	char arr[1024*991]; // 栈空间 最大 991K
				SARRAY(strbuf,100);
				cout<<"Line : "<<__LINE__<<endl;
			}
			cout<<"测试"<<endl;
			{
				String str;
				str.ReSize(4);
				char* p = const_cast<char*>(str.ToCStr());
				p[0] = 'A';
				p[1] = 'B';
				p[2] = 0;
				cout<<str.ToCStr()<<endl;
				str = "函数";
				cout<<"大小："<<str.Size()<<" , "<<str.ToCStr()<<endl;
			}
			{
				StrImg imAge("12");
				String strName("John");
				//	String str;
				//	str.Format("name: %0,age: %1",strName,imAge);
				//	cout<<str.ToCStr()<<endl;
				//	str.Format("This is 要求： %s,%c,%d,%u","Okelse",'W',-12,456);
				//	cout<<str.ToCStr()<<endl;
			}
			{
				StrImg str1("ABCeew15ed3f");
				cout<<"str1 : "<<str1.ToCStr()<<endl;
				cout<<"str1.Size() : "<<str1.Size()<<endl;
				cout<<"str1.IsEmpty() : "<<str1.IsEmpty()<<endl;
				cout<<"str1.GetAt(0) : "<<str1.GetAt(0)<<endl;
				//	cout<<"str1.GetAt(12) : "<<str1.GetAt(12)<<endl;
				StrImg str2("Bc");
				str2.SetImg("ew15");
				cout<<"str2 : "<<str2.ToCStr()<<endl;
				cout<<"str1.Compare(str2) : "<<str1.Compare(str2)<<endl;
				cout<<"str2.Compare(str1) : "<<str2.Compare(str1)<<endl;
				cout<<"str1<str2 : "<<(str1<str2)<<endl;
				cout<<"str2<str1 : "<<(str2<str1)<<endl;
				cout<<"str1.IsEqual(str2) : "<<str1.IsEqual(str2)<<endl;
				cout<<"str2.IsEqual(str1) : "<<str2.IsEqual(str1)<<endl;
				cout<<endl;
				cout<<"str1.Find(str2)	: "<<str1.Find(str2)  <<endl;
				cout<<"str1.Find(str2,0): "<<str1.Find(str2,0)<<endl;
				cout<<"str1.Find(str2,1): "<<str1.Find(str2,1)<<endl;
				cout<<"str1.Find(str2,2): "<<str1.Find(str2,2)<<endl;
				cout<<"str1.Find(str2,3): "<<str1.Find(str2,3)<<endl;
				cout<<"str1.Find(str2,4): "<<str1.Find(str2,4)<<endl;
				cout<<"str1.Find(str2,5): "<<str1.Find(str2,5)<<endl;
				cout<<endl;
				cout<<"str1.RFind(str2)  : "<<str1.RFind(str2)  <<endl;
				cout<<"str1.RFind(str2,0): "<<str1.RFind(str2,0)<<endl;
				cout<<"str1.RFind(str2,1): "<<str1.RFind(str2,1)<<endl;
				cout<<"str1.RFind(str2,2): "<<str1.RFind(str2,2)<<endl;
				cout<<"str1.RFind(str2,3): "<<str1.RFind(str2,3)<<endl;
				cout<<"str1.RFind(str2,4): "<<str1.RFind(str2,4)<<endl;
				cout<<"str1.RFind(str2,5): "<<str1.RFind(str2,5)<<endl;
				cout<<"str1.RFind(str2,20): "<<str1.RFind(str2,20)<<endl;
				cout<<"str1.RFind(str2,300): "<<str1.RFind(str2,300)<<endl;
				StrImg str6("ulates an analog carrier signal to encode digital");
				StrImg str7("analog");
				cout<<"str6.Find(str7):"<<str6.Find(str7)<<endl;
				cout<<"str6.Size() : "<<str6.Size()<<endl;
			}
			{	// Hash
				STime st;
				st.SetTime(st.Now()+3600);
				st.SetTime("2012-01-01 01:20:33",TFmt_Normal);
				st.SetTime("yyyy年mm月dd日hh点mi分ss秒" , "2012年07月22日05点17分39秒");
				cout<<st.ToStr().ToCStr()<<" , "<<st.TimeStamp()<<endl;
				StrImg str1("1E48fr486egdsafdsgfjdsi0ogjierwohgeruihgioerjt43890ty89y34785t0");
				StrImg str2("1e48fr486egdsafdsgfjdsi0ogjierwohgeruihgioerjt43890ty89y34785t0");
				st.TickStart();
				cout<<"Hash: "<<GetHash(str1)<<" , "<<GetHash(str2)<<endl;
				TimeT tSpan1 = st.TickStop();
				cout<<"Hash 时间："<<tSpan1<<" , Size: "<<str1.Size()<<endl;
				st.TickStart();
				cout<<"IsEqual: "<<str2.IsEqual(str1,true)<<endl;
				TimeT tSpan2 = st.TickStop();
				cout<<tSpan2<<endl;
			}
			{	//
				cout<<"new String to a StrImg"<<endl;
				StrImg* pImg = new String("4567");
				delete pImg;
			}
			{
				String str("123");
				StrImg& imStr = str;
				//	imStr.Clear(); // 这里将抛出异常
				cout<<imStr.Size()<<" , "<<imStr.ToCStr()<<endl;
				str.Append("456");
				cout<<imStr.Size()<<" , "<<imStr.ToCStr()<<endl;
				str.Insert(2,"ABCCD");
				cout<<imStr.Size()<<" , "<<imStr.ToCStr()<<endl;
				String str1("老驴是个猪猪");
				str1.Insert(8,"可爱的");
				cout<<str1.ToCStr()<<endl;
				str1.Append(",小能很喜欢");
				str1.AppendC('!',5);
				str1.Delete(str1.Size()-4,9);
				cout<<str1.ToCStr()<<endl;
				str1.AppendC('!',4);
				str1.Retain(0,str1.Size()-4);
				cout<<str1.ToCStr()<<endl;
				str1.Replace("猪","老师");
				str1.Clear();
				str1.Append("He is a pige,She is a pige,too");
				str1.Replace("[teacher]",7,4);

				cout<<str1.ToCStr()<<endl;
			}
			{
				cout<<"Format( ) ---------------------------------------"<<endl;
				String str;
				str.FormatStr("My name is %s. I'm %d year old. My lucky letter is %c","Happieme",29,'A');
				cout<<str.ToCStr()<<endl;
				str.Clear();
				str.FormatStr("姓名：%s，年龄：%d，学校：%s，成绩：%c","张三丰",120,"武台山职工子弟学校",'D');
				cout<<str.ToCStr()<<endl;
				//	str.Format("insert into %s (姓名,年龄,身高,出生年月,总资产,电话) values(%s,%d,%f,%s,%f,%s)","AdoTestTable","Johnson",21,189,"1989-12-05",35.4,"13198765432");
				str.Clear();
				STime st;
				str.FormatStr("insert into %s (姓名,年龄,身高,出生年月,总资产,电话) values(%s,%d,%.1f,%s,%.1f,%s,%lld,%u)"
					,"AdoTestTable","Johnson",21,189.17305,"1989-12-05",35.4,"13198765432",st.ToNow(),str.Size());
				cout<<str.ToCStr()<<endl;
				// Format
				str.Clear();
				StrImg imFmt("Name:%3 , Age:%1 , Sex:%2 , Addr:%0 , memo:%4");
				StrImg imArr[] = {"ShenZhen","15","SuperMan","Joy","%4"};
				str.FormatArr(imFmt , imArr, 5);
				cout<<str.ToCStr()<<endl;
				str.Clear();
				str.FormatArr(imFmt , imArr, COUNTARR(imArr));
				cout<<str.ToCStr()<<endl;
			}

			// SStrImg 成员函数
			{
				// 从字面值字符串构建一个 SStrImg 对象，由于字面值字符串存储于常量区，故不必担心它会消失
				StrImg imStr1("abce");
				cout<<"imStr1 - value : "<<imStr1.ToCStr()<<" , size : "<<imStr1.Size()<<endl;
				// 从一个已有的 SStrImg 来构建一个新 SStrImg 对象
				StrImg imStr2(imStr1);
				cout<<"imStr2 - value : "<<imStr2.ToCStr()<<" , size : "<<imStr2.Size()<<endl;
				// SStrImg 是已存在的 String 的“像”，所以不会对字符串本身有影响，操作都是“只读”的
				// 读取某个位置的 value，该位置必须合法有效！否则将抛出异常
				cout<<"GetAt(1) : "<<imStr1.GetAt(1)<<endl;
				imStr1 = imStr2;
				cout<<"imStr1 = imStr2"<<endl;
				cout<<"GetAt(1) : "<<imStr1.GetAt(1)<<endl;
				imStr1.Clear();
				cout<<"imStr1.Clear()"<<endl;
				cout<<"imStr1.IsEmpty( ) : "<<imStr1.IsEmpty( )<<endl;
				cout<<"imStr2.IsEmpty( ) : "<<imStr2.IsEmpty( )<<endl;
				//	cout<<imStr1.ToCStr()<<endl; // 无法向控制台输出 NULL 指针
				imStr1.SetImg("abce");
				imStr2.SetImg("Abde");
				cout<<"imStr1 : "<<imStr1.ToCStr()<<endl;
				cout<<"imStr2 : "<<imStr2.ToCStr()<<endl;

				cout<<"Find and RFind (Case): "<<endl;
				cout<<" Find( ) AB : "<<imStr1.Find("AB")<<endl;
				cout<<" Find( ) BDC : "<<imStr1.Find("BDC")<<endl;
				cout<<" Find( ) imStr2 : "<<imStr1.Find(imStr2)<<endl;
				cout<<" RFind( ) ae : "<<imStr1.RFind("ae")<<endl;
				cout<<" str1 , RFind( ) BDE : "<<imStr1.RFind("BDE")<<endl;
				cout<<" str2 , RFind( ) BDE : "<<imStr2.RFind("BDE")<<endl;
			}
			{
				cout<<"配合 STL 容器使用"<<endl;
				set<StrImg> ArrNoCase; // 其顺序与 StrImg 的“<”有关，比较不区分大小写
				StrImg imStr1 = "RBC486";
				StrImg imStr2 = "def56g156es";
				StrImg imStr3 = "1234351456e6";
				ArrNoCase.insert(imStr1);
				ArrNoCase.insert(imStr2);
				ArrNoCase.insert(imStr3);
				cout<<(*(ArrNoCase.find(imStr1))).ToCStr()<<endl;
				// 第一个元素应为 imStr3
				cout<<"不区分大小写"<<endl;
				set<StrImg>::iterator itr = ArrNoCase.begin();
				cout<<"1: "<<(*itr++).ToCStr()<<endl;
				cout<<"2: "<<(*(itr++)).ToCStr()<<endl;
				cout<<"3: "<<(*itr).ToCStr()<<endl; // 因不区分大小，所以 imStr2 ，排在 imStr1 的前面

				cout<<"区分大小写"<<endl;
				set<StrImg,StrCmpCase> ArrCase; // 明确指定使用 StrCmpCase，区分大小写
				ArrCase.insert(imStr1);
				ArrCase.insert(imStr2);
				ArrCase.insert(imStr3);
				set<StrImg,StrCmpCase>::iterator itr2 = ArrCase.begin();
				cout<<"1: "<<(*itr2++).ToCStr()<<endl;
				cout<<"2: "<<(*itr2++).ToCStr()<<endl;
				cout<<"3: "<<(*itr2).ToCStr()<<endl; // 区分大小，所以 imStr2 ，排在 imStr1 的后面
			}

			{	// SString
				// 构造函数与赋值运算符
				String str("asdf");
				cout<<str.ToCStr()<<endl;
				cout<<"Size of String : "<<sizeof(String)<<endl;
				str.Append("uuuuu");
				cout<<"str : "<<str.ToCStr()<<" , Size: "<<str.Size()<<endl;
				StrImg imStr = str;
				StrImg imStr2;
				imStr2 = imStr;
				String str2;
				str2 += str;
				str2 += imStr2;
				str2 += "_156132";
				str2 = StrImg("123") + str2;
				cout<<"str2 : "<<str2.ToCStr()<<endl;
				str2.Clear();
				cout<<"str2 : "<<str2.ToCStr()<<endl;
			}
			{
				//	String delim("[,][b_c][dasf a][ ][&]");
				//	String del("(的)(,)(b_c)(&)");
				StrImg imSrc("汉A__字B__玄C__黄D");
				cout<<"Src addr : "<<&imSrc<<endl;
				cout<<"StrSplit"<<endl;
				StrSplit part(imSrc, "__");
				for(Uint i=0;i<part.GetCount();i++)
				{
					cout<<"Size : "<<part.GetAt(i).Size()<<" , "<<part.GetAt(i).ToCStr()<<" , addr : "<<&(part.GetAt(i))<<endl;
				}
				cout<<endl;
			}

			{
				// Convert
				String str;
				str = "123.45608900+";
				int iret=0;
				double dret=0;
				cout<<"Convert : "<<str.ToCStr()<<endl;
				iret = ToInt<int>(str);
				cout<<"ToInt : "<<iret<<endl;
				// effective bit is 6
				dret = ToDouble(str);
				cout<<"ToDouble : "<<dret<<endl;
				str = "0.0000000001";
				cout<<"Convert : "<<str.ToCStr()<<endl;
				iret = ToInt<int>(str);
				cout<<"ToInt : "<<iret<<endl;
				dret = ToDouble(str);
				cout<<"ToDouble : "<<dret<<endl;
				if(0.0 == dret)
				{	cout<<"Convert double failed!"<<endl;	}

				str = ".1.5.6.41";
				cout<<"Convert : "<<str.ToCStr()<<endl;
				iret = ToInt<int>(str);
				cout<<"ToInt : "<<iret<<endl;
				dret = ToDouble(str);
				cout<<"ToDouble : "<<dret<<endl;
				if(0.0 == dret)
				{	cout<<"Convert double failed!"<<endl;	}
				str = "我";
				cout<<"Convert : "<<str.ToCStr()<<endl;
				try
				{
					iret = ToInt<int>(str);
					cout<<"ToInt : "<<iret<<endl;
				}
				catch(const CharW* pErr)
				{	cout<<pErr<<endl;	}
			}
			{
				String str("123456");
				String str2("about");
				str.Append("789");
				str2.Append("adsf");
				str.Append(str2);
				str.Append(StrImg("uuuu",4));
				cout<<str.ToCStr()<<endl;
				//	cout<<str.Mid(2,3).cstr()<<endl;
			}
			// Delete ----------------------------------------------------------------------------------------------
			{
				cout<<"Delete , Trim ----------------------------------------------------------------"<<endl;
				String str("  d  f dff dsfkuyaf"); // --------------------------------
				cout<<"Src ,Size:"<<str.Size()<<endl<<"|"<<str.ToCStr()<<"|"<<endl;
				str.Trim('L');
				cout<<"TrimLeft\t:"<<str.Size()<<endl<<"|"<<str.ToCStr()<<"|"<<endl;
				str = " fdfdfghfd gfdks \tfd  \t"; // --------------------------------
				cout<<"Src ,Size:"<<str.Size()<<endl<<"|"<<str.ToCStr()<<"|"<<endl;
				str.Trim('R');
				cout<<"TrimRight\t:"<<str.Size()<<endl<<"|"<<str.ToCStr()<<"|"<<endl;
				str = " fdfdks \tfd  \t"; // --------------------------------
				cout<<"Src ,Size:"<<str.Size()<<endl<<"|"<<str.ToCStr()<<"|"<<endl;
				str.Trim();
				cout<<"Trim\t:"<<str.Size()<<endl<<"|"<<str.ToCStr()<<"|"<<endl;
			}
			{
				cout<<"Upper , Lower ------------------------------------------------"<<endl;
				String str("Ab_c+*D &0 Bd");
				cout<<"Src   :"<<str.ToCStr()<<endl;
				str.MakeLower();
				cout<<"Lower :"<<str.ToCStr()<<endl;
				str.MakeUpper();
				cout<<"Upper :"<<str.ToCStr()<<endl;
				cout<<"Reverse ------------------------------------------------"<<endl;
				cout<<"Src     :"<<str.ToCStr()<<endl;
				str.MakeReverse();
				cout<<"Reverse :"<<str.ToCStr()<<endl;
			}
			{
				cout<<"Take over -----------------------------"<<endl;
				String str;
				const Uint NewSize = 16;
				char* pNew = new char[16];
				str.TakeOver(pNew,0,NewSize);
				cout<<str.ToCStr()<<" , "<<int(pNew)<<" , "<<int(str.ToCStr())<<endl;
				str.Append("123");
				cout<<str.ToCStr()<<" , "<<int(pNew)<<" , "<<int(str.ToCStr())<<endl;
				str.Append("456");
				cout<<str.ToCStr()<<" , "<<int(pNew)<<" , "<<int(str.ToCStr())<<endl;
				str.Append("AAAAAAAAAAAAA");
				cout<<str.ToCStr()<<" , "<<int(pNew)<<" , "<<int(str.ToCStr())<<endl;
				pNew = new char(65);
				str.TakeOver(pNew,1,1);
				cout<<int(pNew)<<" , "<<int(str.ToCStr())<<endl;
			}
			{
				cout<<"Match , Compare"<<endl;
				String str("abc");
				cout<<"Src : "<<str.ToCStr()<<endl;
				cout<<"CompareCase  :"<<str.Compare("abc",true)<<endl;
				cout<<"CompareCase  :"<<str.Compare("Abc",true)<<endl;
				cout<<"CompareNoCase:"<<str.Compare("abc")<<endl;
				cout<<"CompareNoCase:"<<str.Compare("aBc")<<endl;
				cout<<"CompareCase  :"<<str.Compare("aabc",true)<<endl;
				cout<<"CompareNoCase:"<<str.Compare("aabc")<<endl;
				cout<<"Match : "<<str.IsEqual("aabc")<<endl;
				cout<<"Match : "<<str.IsEqual("abc")<<endl;
				cout<<"Match : "<<str.IsEqual("ABc")<<endl;
				cout<<"Match : "<<str.IsEqual("ABc",true)<<endl;
				cout<<"Match : "<<str.IsEqual("abc",true)<<endl;
				StrImg Text("fe4dwtf 45eW64er8我们A组T4g6wer841c7gwr89B组+32#%$@#Ref342v 32");
				StrImg Search("我们A组"); //2v
				Uint pos = Text.Find(Search);
				cout<<pos<<endl;
				pos = Text.Find("g6","我们A组","B组");
				cout<<"找A组和B组之间的g6, pos = "<<pos<<endl;
				Search.SetImg("8我们");
				pos = Text.Find(Search);
				cout<<pos<<endl;
				Search.SetImg("45eW6");
				pos = Text.Find(Search);
				cout<<pos<<endl;		
				Search.SetImg("545ew6");
				pos = Text.Find(Search);
				cout<<pos<<endl;
			}
			{
				cout<<"+"<<endl;
				String str("ab");
				str += "uio";
				cout<<str.ToCStr()<<endl;
				String str2("213");
				String str3("wer");
				String str4 = str2 + str3;// + "852";
				cout<<"str4 : "<<str4.ToCStr()<<endl;
				cout<<"str   : "<<str.ToCStr()<<"\t, Obj Addr : "<<int(&str)<<" , Val Addr : "<<int(str.ToCStr())<<endl;
				cout<<"str3  : "<<str3.ToCStr()<<"\t, Obj Addr : "<<int(&str3)<<" , Val Addr : "<<int(str3.ToCStr())<<endl;
				str3.Swap(str);
				cout<<"After swap"<<endl;
				cout<<"str   : "<<str.ToCStr()<<"\t, Obj Addr : "<<int(&str)<<" , Val Addr : "<<int(str.ToCStr())<<endl;
				cout<<"str3  : "<<str3.ToCStr()<<"\t, Obj Addr : "<<int(&str3)<<" , Val Addr : "<<int(str3.ToCStr())<<endl;
			}
			{
				// Replace
				String str("acadsadsefahyaacadsadsefahya");
				cout<<"replace : "<<endl<<str.ToCStr()<<endl;
				str.Replace("ds","wq");
				cout<<str.ToCStr()<<endl;
				str.Replace("a","A");
				cout<<str.ToCStr()<<endl;
				str.Replace("wq","wD");
				cout<<str.ToCStr()<<endl;
				str.Replace("wDA","x");
				cout<<str.ToCStr()<<endl;
				str.Replace("TTTT",2,3);
				cout<<str.ToCStr()<<endl;
			}
			{
				// String 转换为 StrImg
				String str("English");
				cout<<str.ToCStr()<<endl;
				StrImg* pimStr = &str;
				cout<<pimStr->ToCStr()<<endl;
				StrImg iName("Here");
				cout<<iName.ToCStr()<<endl;
			}
			cout<<"Test End!"<<endl;
		}
		catch(const char* pErr)
		{
			cout<<"有函数抛出了异常，信息如下："<<endl;
			cout<<pErr<<endl;
		}
	}

	void TestThrow(Uint Size , char** strThrow)
	{
		if(Size>3)
		{
			//	*strThrow = "Size太大";	
		}
		return;
	}

	void TestSArray()
	{
		{
			cout<<sizeof(StrImg)<<" , "<<sizeof(String)<<endl;
			StrImg* pStr = new String("adsf");
			cout<<pStr->ToCStr()<<" , "<<pStr->Size()<<endl;
			const char* pArr = "123";
		//	pStr->SetImg(pArr,3); // 将报错
			delete pStr;
		}
		{
			SARRAY(bf , 120);
			bf.Append("0123456");
			bf.Append("1234567");
			cout<<bf.ToCStr()<<endl;
			bf.Replace("ABC",2,5);
			cout<<bf.ToCStr()<<endl;
			bf.Clear();
			bf.Append("adsfqwer");
			bf.MakeUpper();
			cout<<"MakeUpper : "<<bf.ToCStr()<<endl;
			bf.MakeLower();
			cout<<"MakeLower: "<<bf.ToCStr()<<endl;
			bf.MakeReverse();
			cout<<"MakeReverse : "<<bf.ToCStr()<<endl;
			bf.FormatStr("name: %s,age: %u,letter: %c","John",12,'D');
			cout<<"Format: "<<bf.ToCStr()<<endl;
			bf.Clear();
			// ---------------------------
			SARRAY(bf2 , 16);
			bf2 = StrImg("123");
			cout<<bf2.ToCStr()<<endl;
			StrImg InfoArr[] = {"John","23"};
			bf.FormatArr("name: %0,age: %1,name: %0",InfoArr,2);
			cout<<"Format: "<<bf.ToCStr()<<endl;
		}
	}
}; //
