// SZlib.cpp : ���� DLL Ӧ�ó���ĵ���������
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
		cout<<"ѹ��ǰ��"<<FileStr.Size()<<"ѹ����"<<strGZip.Size()<<endl;
		sf.Write(strGZip);
		String strUnGZip;
		UnSGZip(strGZip,strUnGZip);
		cout<<"��ѹ��"<<strUnGZip.Size()<<endl;
		cout<<strUnGZip.IsEqual(FileStr)<<endl;
		cout<<"����Ƿ�ѹ������"<<endl;
		cout<<"FileStr  : "<<IsSGZiped(FileStr)<<endl;
		cout<<"strGZip  : "<<IsSGZiped(strGZip)<<endl;
		cout<<"strUnGZip: "<<IsSGZiped(strUnGZip)<<endl;
	}
}; // namespace
