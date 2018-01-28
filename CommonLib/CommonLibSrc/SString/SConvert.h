#ifndef HAPPIEME_SCHARCONVERT
#define HAPPIEME_SCHARCONVERT

#include "String.h"

namespace smm
{
	class SMDLLX SConv
	{
	public:
		// Ansi VS unicode VS utf-8，gb2312,gbk 统一视作 ansi
		static void		A_U		( const StrImg & iStrSrc , String & DstW );
		static String	A_U		( const StrImg & iStrSrc );
		static void		A_U8	( const StrImg & iStrSrc , String & strDst );
		static String	A_U8	( const StrImg & iStrSrc );

		static void		U8_U	( const StrImg & iStrSrc , String & DstW );
		static String	U8_U	( const StrImg & iStrSrc );
		static void		U8_A	( const StrImg & iStrSrc , String & strDst );
		static String	U8_A	( const StrImg & iStrSrc );

		static void		U_U8	( const StrImg & iSrcW , String & strDst );
		static String	U_U8	( const StrImg & iSrcW );
		static void		U_A		( const StrImg & iSrcW , String & strDst );
		static String	U_A		( const StrImg & iSrcW );
		// url Utf8 解码，从 Url 编码到 Ansi
		static void UrlDecode( const StrImg& strC , String& strDst); // 去除 %，还原原有的编码
	}; // class SConv
}; // namespace smm

#endif
