#ifndef HAPPIEME_SGZIP
#define HAPPIEME_SGZIP
#include "../SString/String.h"

namespace smm
{
	namespace SCompress
	{
		bool SMDLLX		IsSGZiped(const StrImg& imSrc);
		bool SMDLLX		  SGZip	(const StrImg& imSrc , String& strDst);
		bool SMDLLX		UnSGZip	(const StrImg& imSrc , String& strDst);
	}
};
#endif
