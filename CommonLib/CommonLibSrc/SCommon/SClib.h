#ifndef HAPPIEME_SCLIB
#define HAPPIEME_SCLIB
#include "SCommon.h"
#include <time.h>
#include <stdio.h>
#include <sys/stat.h> // _stat,stat

namespace smm
{
	namespace SClib
	{
		// String =====================================================================================
		Uint		StrLen		(const char* pStr);
		Uint		SundayFind	(const char* pText , Uint TextLen , const char* pSearch , 
										 Uint SearchLen, bool bHeadToTail=true , bool IsCase=true);
		Uint		PlainFind	(const char* pSrc , Uint SrcSize , const char* pArr , 
										 Uint ArrSize ,	 bool bHeadToTail=true, bool IsCase=true);
		char	CharToUpper	( char c);
		char	CharToLower ( char c);
		template<typename NumT> // for int , Uint , long , Int64
		NumT	StrToInt	( const char* pSrc , Uint SrcSize);
		double	StrToDouble ( const char* pStr , Uint StrSize);
		bool	IsNumber	( const char* pStr , Uint StrSize);
		// format
		int GetVaryFormatLen(const char* pStrFormat , char* args);
		int SetVaryFormatBuf(char* pBuf , Uint BufSize , const char* pStrFormat , char* args);

		// for memory
		void MemCpy  ( void* pDst , Uint nDstSize , const void* pSrc , Uint nSrcSize);
		void MemSet  ( void* pMem , char c , Uint nCount);
		void MemClear( void* pMem , Uint nCount);

		// for numeric
		template<typename T> // 只针对内部数值类型
		T Max(T lVal,T rVal)
		{	return lVal>rVal ? lVal : rVal;	}

		template<typename T>
		T Min(T lVal,T rVal)
		{	return lVal<rVal ? lVal : rVal;	}

		// for time
		TimeT GetCurrentTimeStamp();
		void SetTM_Local(tm&,TimeT);
		void SetTM_Gmt  (tm&,TimeT);
		TimeT TM_2_TStamp(tm&);
		Uint FormatTStr(char* strDst,Uint DstSize,const char* pStrFmt,tm&);
		TimeT TickCount();

		// for file
		int  FileOpen(FILE** pFile, const char* FileName, const char* OpenMode);
#ifdef WIN32
	#define FILESTATE _stat
#else
	#define FILESTATE  stat
#endif
		int FileGetState(const char* pFilePath, struct FILESTATE& FState);
		int IsFileEnd	(FILE* pFile);
		int IsFileError	(FILE* pFile);
		int FileSeek	(FILE* pFile, long offset, int origin);
		int FileTell	(FILE* pFile);
		int FileClose	(FILE* pFile);
		int FileFlush	(FILE* pFile);
		Uint FileRead	(void *pBuf, int BufSize, int StructSize, int StructCount, FILE* pFile);
		Uint FileWrite	(const void *pSrc, int StructSize, int StructCount, FILE* pFile);
		// for dir
		bool MakeDir(const char* FilePath, int mode);
		int FileAccess( const char* FilePath, int mode);
	} // SClib
} // namespace
#endif
