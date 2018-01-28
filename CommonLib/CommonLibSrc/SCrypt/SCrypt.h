#ifndef HAPPIEME_SCRIPT
#define HAPPIEME_SCRIPT
#include "../SString/String.h"

namespace smm
{
	class SMDLLX SCrypt
	{
	public:
		static void EnCrypt(const StrImg& Src , String& Dst);
		static void DeCrypt(const StrImg& Src , String& Dst);
		static void Base64_Encode(const StrImg& strSrc , String& strDst);
		static void Base64_Decode(const StrImg& strSrc , String& strDst);
	};
};

#endif