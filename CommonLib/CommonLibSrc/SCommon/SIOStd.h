// 用于测试
#ifndef HAPPIME_SIOSTD
#define HAPPIME_SIOSTD
#include "../SCommon/SCommon.h"
#include "../SString/StrImg.h"

#ifdef SMDEBUG
#include <iostream>
using std::cout;
using std::cin;
using std::wcout;
using std::endl;
namespace smm
{
#define	SDEBUG(err,bShow)\
		{\
			if(bShow)\
			{\
				StrImg tmp(err);\
				SARRAY(str,1024*16);\
				cout<<"File: "<<__FILE__<<endl;\
				if(str.Capacity() <= tmp.Size())\
				{	cout<<"The input param of SDEBUG is too large, > 16K !"<<endl;}\
				else\
				{\
					str = tmp;\
					cout<<str.ToCStr()<<endl;\
				}\
			}\
		}
#endif
};
#endif
