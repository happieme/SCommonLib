#include "../../CommonLib/CommonLibSrc/SString/SConvert.h"
#include "../../CommonLib/CommonLibSrc/SFile/SFile.h"
#include "../../CommonLib/CommonLibSrc/SFile/SLog.h"
#include "../../CommonLib/CommonLibSrc/SFile/SConfig.h"
#include "../../CommonLib/CommonLibSrc/STime/STime.h"
#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"
#include <cassert>

namespace smm
{
	void TestSFile()
	{
		String strC;
		SFile sf("../../../TestFile/TestAnsi.txt");
		sf.Open("w+");
		sf.Write("abc");
		sf.Close();
		if(sf.Open())
		{
			cout<<"FileSize: "<<sf.Size()<<endl;
			sf.Read(strC);
			cout<<"Size: "<<strC.Size()<<endl<<"Content: "<<strC.ToCStr()<<endl;
			sf.SetPos(0);
			char Arr[3];
			sf.Read(Arr,3,1,3);
			cout<<"Content: "<<Arr<<endl;
			sf.SetPos(4);
			sf.Read(strC,10);
			cout<<"strC："<<strC.Size()<<endl<<strC.ToCStr()<<endl;
			strC.Clear();
			CharC c[2] = {0,0};
			cout<<"逐个读取"<<endl;
			while(!sf.IsEnd() || sf.GetPos()<18)
			{
				if(0 == sf.Read(&c,1,1,1))
				{	break;	}
				cout<<c;
			}
			cout<<endl;
			cout<<"读 4 个字符："<<endl;
			strC.Clear();
			sf.SetPos(0);
			sf.Read(strC,4);
			cout<<"大小："<<strC.Size()<<" ,内容："<<endl<<strC.ToCStr()<<endl;
			cout<<"读 倒数 2 个字符："<<endl;
			sf.SetPos(-1, SFile::SFILE_TAIL);
			sf.Read(strC,4);
			cout<<"大小："<<strC.Size()<<" ,内容："<<endl<<strC.ToCStr()<<endl;
			tagFileInfo theFInfo = sf.GetFileInfo();
			cout<<"AccessTime : "<<STime(theFInfo.AccessTime).ToStr().ToCStr()<<endl;
			cout<<"CreateTime : "<<STime(theFInfo.CreateTime).ToStr().ToCStr()<<endl;
			cout<<"ModifyTime : "<<STime(theFInfo.ModifyTime).ToStr().ToCStr()<<endl;
			cout<<"FileSize   : "<<theFInfo.FileSize<<endl;
			cout<<"FilePath   : "<<theFInfo.FilePath.ToCStr()<<endl;
		}
		{
			cout<<"New Adding"<<endl;
			String strC = "\r\nNew Adding ----这是添加的新内容！！！！！！！！！！！！！！！！！！！！！！";
			sf.SetPos(0,SFile::SFILE_TAIL);
			sf.Write(strC);
			sf.SetPos(0);
			sf.Read(strC,32);
			cout<<"strC："<<strC.Size()<<endl<<strC.ToCStr()<<endl;
		}
		{
			// Read line
			String DstStr;
			sf.SetPos(0);
			sf.ReadLine(DstStr);
			cout << "line len=" << DstStr.Size() << ", line content: " << DstStr.ToCStr() << endl;
			sf.ReadLine(DstStr);
			cout << "line len=" << DstStr.Size() << ", line content: " << DstStr.ToCStr() << endl;
			sf.ReadLine(DstStr);
			cout << "line len=" << DstStr.Size() << ", line content: " << DstStr.ToCStr() << endl;
		}
		// utf-8
		cout<<"File : UTF-8"<<endl;
		SFile suf8("../../../TestFile/TestUtf8.txt");
		if(suf8.Open())
		{
			String StrTest;
			cout<<"Current Pos : "<<suf8.GetPos()<<endl;
			suf8.Read(StrTest, 3);
			cout<<StrTest.ToCStr()<<endl;
			cout<<"Current Pos : "<<suf8.GetPos()<<endl;
			suf8.Read(strC,6);
			Uint theSize = strC.Size();
			cout<<"strC："<<theSize<<endl;
			cout<<strC.ToCStr()<<endl;
			for(Uint i=0;i<theSize;i++)
			{	cout<<int(strC.GetAt(i))<<" , ";	}
			cout<<endl;
		}
		{	// 大文件，170 M
			SFile sf("C:\\Documents and Settings\\Administrator\\桌面\\1.txt");
			if(sf.Open())
			{
				cout<<"FileSize: "<<sf.Size()<<endl;
				sf.SetPos(-20,SFile::SFILE_TAIL);
				SARRAY(str, 128);
				Uint ReadSize = sf.Read(str);
				cout<<"ReadSize: "<<ReadSize<<endl;
				cout<<"Read str size: "<<str.Size()<<"Content : |"<<str.ToCStr()<<"|"<<endl;
			}
		}
	}

	void TestReadUtf8()
	{
		cout<<"File : UTF-8"<<endl;
		STextFile su8("../../../TestFile/TestUtf8.txt");
		if(su8.Open())
		{
			String StrTest;
			cout<<"Current Pos : "<<su8.GetPos()<<endl;
			su8.Read(StrTest, 3);
			cout<<StrTest.ToCStr()<<endl;
			cout<<"Current Pos : "<<su8.GetPos()<<endl;
			su8.Read(StrTest,6);
			Uint theSize = StrTest.Size();
			cout<<"strC："<<theSize<<endl;
			cout<<StrTest.ToCStr()<<endl;
			for(Uint i=0;i<theSize;i++)
			{	cout<<int(StrTest.GetAt(i))<<" , ";	}
			cout<<endl;
		}
	}

	void TestReadUcs2()
	{
		/*	SFile sf(L"D:/HPMServer_Git/TestCommonLib/test_ucs2.txt");
		WString strW;
		sf.Open(L"r");
		if(sf.IsOpen())
		{
		sf.Seek(2);
		sf.ReadWString(strW,10);
		wcout<<L"test_ucs2.txt 大小："<<strW.Size()<<endl;
		wcout<<L"内容："<<strW.ToCStr()<<endl;
		strW.Clear();
		sf.Seek(2);
		sf.ReadLine(strW,110);
		wcout<<L"ReadLine："<<endl;
		wcout<<L"大小："<<strW.Size()<<L"内容：\n"<<strW.ToCStr()<<endl;
		strW.Clear();
		sf.ReadLine(strW);
		wcout<<L"大小："<<strW.Size()<<L"内容：\n"<<strW.ToCStr()<<endl;
		SCharW c = 0;
		sf.Seek(2);
		for(UINT i=0;!sf.IsEnd();i++)
		{
		c = sf.ReadCharW();
		wcout<<c;
		}
		wcout<<endl;
		cout<<"读 倒数第 2 个汉字："<<endl;
		sf.SeekEnd();
		sf.Seek(sf.Tell()-4);
		strW.Clear();
		sf.ReadWString(strW,1);
		wcout<<L"大小："<<strW.Size()<<L"内容：\n"<<strW.ToCStr()<<endl;
		sf.Close();
		wcout<<L"写入文件："<<endl;
		sf.Open(L"rb+");
		sf.WriteWString(L"\n新内容");
		sf.SeekEnd();
		sf.Seek(sf.Tell()-8);
		sf.ReadWString(strW,4);
		wcout<<L"大小："<<strW.Size()<<L"内容：\n"<<strW.ToCStr()<<endl;
		}
		*/
	}

	void TestSConfig()
	{
		SConfig* pCfg = NULL;
		{
			pCfg = SConfig::GetInstance("./config.ini");
			String str = pCfg->Get("server","name");
			cout<<str.ToCStr()<<endl;
			str = pCfg->Get("server","ip");
			cout<<"server : ip \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("client","addr");
			cout<<"client : addr \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("server","count");
			cout<<"server : count \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("client","count");
			cout<<"client : count \t "<<str.ToCStr()<<endl;
		}
		{
			pCfg = SConfig::GetInstance("config2.ini");
			String str = pCfg->Get("db","ip");
			cout<<"db,ip \t"<<str.ToCStr()<<endl;
			str = pCfg->Get("db","conn");
			cout<<"db,conn \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("book","addr");
			cout<<"book,addr \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("book","count");
			cout<<"book,count \t "<<str.ToCStr()<<endl;
		}
		{
			pCfg = SConfig::GetInstance("config.ini");
			String str = pCfg->Get("server","name");
			cout<<str.ToCStr()<<endl;
			str = pCfg->Get("server","ip");
			cout<<"server : ip \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("client","addr");
			cout<<"client : addr \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("server","count");
			cout<<"server : count \t "<<str.ToCStr()<<endl;
			str = pCfg->Get("client","count");
			cout<<"client : count \t "<<str.ToCStr()<<endl;
		}
		SConfig::Release("config.ini");
		pCfg = SConfig::GetInstance("config.ini");
	}

	void TestSLog()
	{
		SFile sf("./Log/2016-11.log");//D:\\HPMServer\\MyServer\\TestCommonLib\\Debug\\Log\\2016-11.log
		if(sf.Open("ab+"))//
		{	cout<<"Open Success!"<<endl;	}
		else
		{	cout<<"Open Failed!"<<endl;	}
		SLog* pLog = SLog::GetInstance();
		pLog->Debug("Memery leak!");
	}

	void TestFileShell()
	{
		StrImg strDir("./Log");
		if(!SFileShell::Exist(strDir))
		{
			cout<<strDir.ToCStr()<<" 不存在!"<<endl;
			cout<<"创建之"<<endl;
			if(SFileShell::MakeDir(strDir))
			{
				cout<<"创建成功"<<endl;
				if(!SFileShell::Exist(strDir))
				{	cout<<strDir.ToCStr()<<" 不存在!"<<endl;	}
			}
		}
		else
		{
			cout<<strDir.ToCStr()<<" 已存在!"<<endl;
		}
	
	}
};
