// TestCommonLib.cpp : 定义控制台应用程序的入口点。
//
#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"
#include "../../CommonLib/CommonLibSrc/STime/STime.h"

namespace smm
{
	void TestSConv();
	void TestCString();
	void TestSArray();
	void TestSTime();
	void TestSFile();
	void TestReadUcs2();
	void TestReadUtf8();
	void TestGZip();
	void TestSCrypt();
	void TestSJson();
	void TestSBase64();
	void TestSMail(){};
	void TestSConfig();
	void TestSLog();
	void TestJson();
	void TestFileShell();
	void TestSHexdump();
	void TestUtPrintHex();
	void extract_main();
};
	typedef void (*pFunc)();
int main(int argc,char *argv[])
{
using namespace smm;
	struct tagFuncArr
	{
		StrImg name;
		pFunc theFunc;
	};

	tagFuncArr FuncArr[] = {	{ "TestSConv"	,	TestSConv	},
								{ "TestCString"	,	TestCString	},
								{ "TestSArray"	,	TestSArray	},
								{ "TestSTime"	,	TestSTime	},
								{ "TestSFile"	,	TestSFile	},
								{ "TestFileShell", TestFileShell},
								{ "TestReadUcs2",	TestReadUcs2},
								{ "TestReadUtf8",	TestReadUtf8},
								{ "TestGZip"	,	TestGZip	},
								{ "TestSCrypt"	,	TestSCrypt	},
								{ "TestSJson"	,	TestSJson	},
								{ "TestSBase64" ,	TestSBase64	},
								{ "TestSMail"	,	TestSMail	},
								{ "TestSConfig"	,	TestSConfig	},
								{ "TestSLog"	,	TestSLog	},
								{ "TestSJson"	,	TestSJson	},
								{ "TestSHexdump",	TestSHexdump },
								{ "extract_main",	extract_main },
	};

	char ArrIndex = 0;
	char Ascii_a = 'a';
	const Uint ArrLen = COUNTARR(FuncArr);

StartAgain:
	cout<<argv[0]<<endl;
	cout<<endl<<"======================================================================="<<endl;
	for(Uint i=0;i<ArrLen;i++)
	{
		cout<<char(i+Ascii_a)<<'\t'<<FuncArr[i].name.ToCStr()<<"();"<<endl;
	}
	cout<<endl;
	cout<<"Please Select one operation(a number) , or Input "<<char(ArrLen+Ascii_a)<<" to quit."<<endl;
	while(cin>>ArrIndex)
	{
		ArrIndex -= Ascii_a;
		if(ArrIndex == ArrLen)
		{
			cout<<"You will quit!"<<endl;
			return 0;
		}
		else if(ArrIndex < ArrLen)
		{
			FuncArr[ArrIndex].theFunc();
			goto StartAgain;
		}
		else
		{	cout<<"Please Input an correct index number! It should between 0 and "<<ArrLen<<"."<<endl;	}
	}
	return 0;
}
