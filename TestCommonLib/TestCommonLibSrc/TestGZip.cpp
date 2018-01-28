// SZlib.cpp : 定义 DLL 应用程序的导出函数。
#include "../../CommonLib/CommonLibSrc/SGZip/SGZip.h"
#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"
#include "../../CommonLib/CommonLibSrc/SFile/SFile.h"

namespace smm
{
	using namespace SCompress;
	void TestGZip()
	{
		SFile sf("../../../TestFile/GzipTest.txt");
		if(!sf.Open())
		{	
			cout<<"open file fail!"<<endl;
			return;	
		}
		String FileStr;
		sf.Read(FileStr);
		String strGZip;
		SGZip(FileStr , strGZip);
		cout<<"压缩前："<<FileStr.Size()<<"压缩后："<<strGZip.Size()<<endl;
		sf.Write(strGZip);
		String strUnGZip;
		UnSGZip(strGZip,strUnGZip);
		cout<<"解压后："<<strUnGZip.Size()<<endl;
		cout<<strUnGZip.IsEqual(FileStr)<<endl;
		cout<<"检测是否压缩过？"<<endl;
		cout<<"FileStr  : "<<IsSGZiped(FileStr)<<endl;
		cout<<"strGZip  : "<<IsSGZiped(strGZip)<<endl;
		cout<<"strUnGZip: "<<IsSGZiped(strUnGZip)<<endl;
	}
}; // namespace
