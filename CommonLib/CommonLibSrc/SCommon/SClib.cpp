#include "SClib.h"
#ifdef WIN32
	#include <io.h> // _access_s
	#include <direct.h> // _mkdir
#else
	#include <unistd.h>
#endif
#include <string.h> // memcpy_s
#include <ctype.h>	// tolower,toupper
#include <stdarg.h> // va_start va_end
namespace smm
{
	namespace SClib
	{
		// StrLen ------------ Uint StrLen(const CharType * pStr)
		Uint StrLen(const char* pStr)
		{
			if(!pStr)
			{	throw "SClib::StrLen(const char* pStr) Error! The first parameter should not be NULL!";	}
			return	strlen(pStr);//注意，如果pStr未初始化也会出问题，但此处无法判断
		}

		char CharToLower(char c)
		{	return (char)tolower(c);	}

		char CharToUpper(char c)
		{	return (char)toupper(c);	}

#ifdef WIN32
                 int GetVaryFormatLen(const char* pFmt , char* args)
                 {       return _vscprintf(pFmt,args);   } // 是否需要 + 1 ？？？？
#else 
/*                 int GetVaryFormatLen(const char* pFmt , va_list vArgs)
                 {
                         va_start(vArgs, pFmt);
                         int len = vsnprintf(nullptr, 0, pFmt, vArgs);
                         va_end(vArgs);
                         return len+1;
                 }*/
#endif
		int SetVaryFormatBuf(char* pBuf , Uint BufSize , const char* pStrFormat , char* args)
		{
#ifdef WIN32
			return vsnprintf_s(pBuf,BufSize,_TRUNCATE,pStrFormat,args);
#else
			return snprintf (pBuf,BufSize,pStrFormat,args);
#endif
		}

		void MemCpy(void* pDst,Uint DstSize,const void* pSrc,Uint SrcSize)
		{
			if (NULL==pDst || NULL==pSrc)
			{	throw "SClib::MemCpy() Error! Pointer should not be NULL!";	}
			if(DstSize < SrcSize)
			{	throw "SClib::MemCpy() Error! SrcSize is greater than DstSize!";	}
			if(0 == SrcSize)
			{	return;	}
			memmove(pDst,pSrc,SrcSize);
		} // MemCopy =======================================================================

		void MemSet(void* pMem , char c , Uint nCount)
		{
			if(!pMem)
			{	throw "SClib::SClibMemSet() Error!  The first parameter should not be NULL!";	}
			memset(pMem,c,nCount);
		} // MemSet =======================================================================

		void MemClear(void* pMem , Uint nCount)
		{	MemSet(pMem, 0, nCount);	}

		static const double DividerArr[] = 
		{
			1			,
			0.1			,
			0.01		,
			0.001		,
			0.0001		,
			0.00001		,
			0.000001	,
			0.0000001	,
			0.00000001	,
			0.000000001	,
		};

		double StrToDouble(const char* pSrc , Uint SrcSize)
		{
			if(!pSrc || 0==SrcSize)
			{	throw "SClib::ToDouble(const char* pSrc , Uint SrcSize) Error! The Pointer parameter is not valid or pSrc is null.Try IsNumber() first!";	}
			double iPart = StrToInt<long>(pSrc,SrcSize ); // 整数部分
			char TheDot[] = {',',0};
			Uint dPos = PlainFind(pSrc , SrcSize , TheDot , 1 , true ,true);
			if(TheEndPos == dPos)
			{	return iPart;	}

			double dPart = 0.0;
			Uint tPos = dPos;
			// 找到最后一个有效数字
			while(tPos<SrcSize && ' '!=pSrc[tPos] && '\t'!=pSrc[tPos])
			{	++tPos;	}
			// 去除后导“0”
			while(tPos<SrcSize && '0'!=pSrc[tPos])
			{	--tPos;	}

			if(dPos+1 < tPos) // 小数点后至少一位数
			{
				dPart = StrToInt<long>(pSrc+dPos+1 , tPos-dPos-1);
				if(0 != dPart)
				{
					Uint n = tPos-dPos-1;
					if(n < sizeof(DividerArr)) // 太小则不用再计算,跟0差不多
					{	dPart *= DividerArr[n];	}
				}
			}

			// 考虑符号
			if(iPart > 0)
			{	iPart += dPart;	}
			else
			{	iPart -= dPart;	}
			return iPart;
		} // ToDouble =======================================================================

		Uint SundayFind(const char* pText , Uint tLen , const char* pSearch , Uint sLen , bool bHeadToTail , bool IsCase)
		{
			if(!pSearch || !pText || sLen>tLen)
			{	return TheEndPos;	}

			// 计算偏移数组
			int OffArr[256]; //  = {0}
			const Uint dLen = tLen-sLen;
			Uint sPos = 0;
			Uint tPos = dLen;
			if(bHeadToTail)
			{	// [0 , tLen-sLen]
				for(Uint i=0;i<256;++i)
				{	OffArr[i] = sLen;	}
				if(IsCase)
				{
					for(Uint i=0;i<sLen;++i)
					{	OffArr[Byte(pSearch[i])] = sLen-i;	}
				}
				else
				{	// 统一按小写记
					char c = 0;
					for(Uint i=0;i<sLen;++i)
					{
						c = pSearch[i];
						int d = sLen-i;
						OffArr[c] = d;
						if(c>=65 && c<=90) // 大写字母 ? c+=32 : 0)
						{	OffArr[c+32] = d;	}
						else if(c>=97 && c<=122) // 小写
						{	OffArr[c-32] = d;	}
					}
				}
			}
			else
			{	// [tLen-sLen , 0]
				sPos = dLen;
				tPos = 0;
				for(Uint i=0;i<256;++i)
				{	OffArr[i] = -1;	}
				if(IsCase)
				{
					for(Uint i=0;i<sLen;++i)
					{	OffArr[Byte(pSearch[i])] = -int(i+1);	}
				}
				else
				{
					char c = 0;
					for(Uint i=0;i<sLen;++i)
					{
						c = pSearch[i];
						int d = -int(i+1);
						OffArr[c] = d;
						if(c>=65 && c<=90) // 大写字母 ? c+=32 : 0)
						{	OffArr[c+32] = d;	}
						else if(c>=97 && c<=122) // 小写
						{	OffArr[c-32] = d;	}
					}
				}
			}

			Uint j = 0;
			Uint nStep = bHeadToTail ? sLen : TheMaxCount; // 反向则为步长为 -1
			if(IsCase)
			{
				for(Uint i=sPos;0<=i&&i<=dLen;)
				{
					for(j=0;j<sLen;++j)
					{
						if(pSearch[j] != pText[i+j])
						{	break;	}
					}
					if(sLen == j)
					{	return i;	}
					else	// i+nStep 为源串中要比较的下一个字符的位置
					{	i += OffArr[Byte(pText[i+nStep])];	}
				}
			}
			else
			{
			//	Uint iTmp = 0;
				char SC=0 , TC=0;
				for(Uint i=sPos;0<=i&&i<=dLen;)
				{
				//	iTmp = i;
					for(j=0;j<sLen;++j)//,++iTmp
					{
						SC = pSearch[j];	TC = pText[i+j]; //iTmp
						if(SC != TC)
						{
							SC>=65&&SC<=90 ? SC+=32 : 0;	TC>=65&&TC<=90 ? TC+=32 : 0;
							if(SC != TC)
							{	break;	}
						}
					}
					if(sLen == j)
					{	return i;	}
					else
					{	i += OffArr[Byte(pText[i+nStep])];	}
				}
			}
			return TheEndPos;
		}

		Uint PlainFind(const char* pSrc, Uint SrcSize, const char* pArr, Uint ArrSize, bool bHeadToTail, bool IsCase)
		{
			if(!pSrc || !pArr || ArrSize>SrcSize)
			{	return TheEndPos;	}

			Uint RetPos  = TheEndPos;
			// 始终满足 sPos <= i <= tPos
			Uint dLen = SrcSize - ArrSize;
			Uint nStep = 1;
			Uint sPos  = 0;
			if(!bHeadToTail)
			{
				sPos = dLen;
				nStep = TheMaxCount;
			}
			Uint j = 0;
			if(IsCase) // Case
			{
				for(Uint i=sPos; 0<=i && i<=dLen; i+=nStep)
				{
					for(j=0;j<ArrSize;++j)
					{
						if(pArr[j] != pSrc[i+j])
						{	break;	}
					}
					if(ArrSize == j)
					{	return i;	}
				}
			}
			else // NoCase
			{
				char LC=0 , RC=0;
				for(Uint i=sPos; 0<=i && i<=dLen; i+=nStep)
				{
					for(j=0;j<ArrSize;++j)
					{
						LC = pSrc[i+j];
						RC = pArr[j];
						if(LC != RC)
						{
							LC>=65&&LC<=90 ? LC+=32 : 0;
							RC>=65&&RC<=90 ? RC+=32 : 0;
							if(LC != RC)
							{	break;	}
						}
					}
					if(ArrSize == j)
					{	return i;	}
				}
			}
			return TheEndPos;
		}

		static const long MultiplierArr[] =
		{
			1			,
			10			,
			100			,
			1000		,
			10000		,
			100000		,
			1000000		,
			10000000	,
			100000000	,
			1000000000
		};
		// T1:char或wchar_t , T2:int,long
		template int   StrToInt<int>	(const char* , Uint);
		template long  StrToInt<long>	(const char* , Uint);
		template Uint  StrToInt<Uint>	(const char* , Uint);
		template Int64 StrToInt<Int64>	(const char* , Uint);
		template<typename NumT>
		NumT StrToInt(const char* pSrc , Uint SrcSize)
		{
			if(!pSrc || 0==SrcSize)
			{	throw "SClib::ToInt(const char* pSrc , Uint SrcSize) Error! The Pointer parameter is not valid or pSrc is null.Try IsNumber() first!";	}
			Uint sPos = TheEndPos;	// 起始位置：第一个数字字符
			Uint tPos = TheEndPos;	// 结束位置：第一个数字字符向后遇到的第一个非数字字符或结尾
			NumT val = 0;

			// 确定 sPos
			bool bBreak = false;
			for(Uint i=0;i<SrcSize;++i)
			{
				val = pSrc[i];
				if((0x2F<val && val<0x3A)) // 0-9
				{
					sPos = i;
					break;
				}
			}

			if(TheEndPos == sPos) // 不是数字字符串,没有一个有效字符
			{	return 0;	}

			// 确定符号位置
			bool bSign = true; // 正负号,true为正
			if(sPos > 0) // 可能有符号,也可能有小数点
			{
				val = pSrc[sPos-1];
				if(0x2D == val) // "-"
				{	bSign = false;	}
				else if(0x2E == val) // ".",有小数点,则说明其整数部分为0
				{	return 0;	}
				//	else if(0x2B == val) // "+"
				//	{	}
			}

			// 确定结束位置
			tPos = SrcSize;
			for(Uint i=sPos;i<SrcSize;++i)
			{
				val = pSrc[i];
				if(val<'0' || val>'9')
				{
					tPos = i;
					break;	
				}
			}
			// 若最后一个字符为数字则 tPos 为结尾
			NumT nRet = 0;
			for(Uint i=sPos;i<tPos;++i)
			{	
				val = pSrc[i]-0x30;
				if(0 != val)
				{
					Uint n = tPos-i-1;
					if(n < sizeof(MultiplierArr))
					{
						val *= MultiplierArr[n];
						nRet += val;
					}
					else
					{	return 0;	}
				}
			}
			return bSign ? nRet : (-1)*nRet;
		} // ToInt =======================================================================

		bool IsNumber(const char* pStr , Uint StrSize)
		{
			if(!pStr)
			{	return false;	}
			if(0 == StrSize)
			{	StrSize = StrLen(pStr);	}
		//	char c = 0;
			// 找到第一个数字
			Uint DigPos = 0;
			Uint index = 0;
			for(;index<StrSize;++index)
			{
				if(isdigit(pStr[index])) // c>=0x30 || c<=0x39
				{
					DigPos = index;
					break;
				}
			}
			if(StrSize == DigPos) // 没找到
			{	return false;	}

			// 从第一个数字起找第一个非数字（不包括点）
			Uint NonDPos = 0;
			char c = 0;
			for(index=DigPos;index<StrSize;++index)
			{
				c = pStr[index];
				if(!isdigit(c)) // c<0x30 || c>0x39
				{
					if(',' != c)
					{
						NonDPos = index;
						break;
					}
				}
			}
			// 至此，满足以下条件：
			// 1、数字前可以有符号或空白符
			if(DigPos > 0)
			{
				for(Uint i=0;i<DigPos;++i)
				{
					if(!isspace(pStr[index]))
					{	
						index = i;
						break;	
					}
				}
				// 只有前面紧跟符号才正常
				char c = pStr[index];
				if(index!=DigPos-1 || ('+'!=c && '-'!=c))
				{	return false;	}
			}
			// 2、点只能出现的中间且只有一个
			if('.' == pStr[NonDPos-1])	// 最后一个不能是点
			{	return false;	}
			Uint DotCount = 0;
			for(index=DigPos;index<NonDPos;++index)
			{
				if('.' == pStr[index])
				{
					if(++DotCount > 1)
					{	return false;	}
				}
			}
			return true;
		}

		TimeT GetCurrentTimeStamp()
		{	return time(NULL);	}

		void SetTM_Local(tm& theTM,TimeT t0)
		{
		time_t t = static_cast<time_t>(t0);
#ifdef WIN32
		localtime_s(&theTM,&t);
#else	// 多线程时有必要使用临界互斥
		theTM = *(localtime(&t));
#endif
		}

		void SetTM_Gmt	(tm& theTM,TimeT t0)
		{
		time_t t = static_cast<time_t>(t0);
#ifdef WIN32
		gmtime_s(&theTM,&t);
#else
		theTM = *(gmtime(&t));
#endif
		}

		TimeT TM_2_TStamp(tm& theTM)
		{	return mktime(&theTM);	}

		Uint FormatTStr(char* strDst,Uint DstSize,const char* pStrFmt,tm& theTM)
		{	return strftime(strDst,DstSize,pStrFmt,&theTM);		}

		TimeT TickCount()
		{	return ::clock();	}

		// File --------------------------------------------------------------------------------------
		int FileOpen(FILE** ppFile, const char* FileName, const char* OpenMode)
		{
#ifdef WIN32
		return fopen_s(ppFile, FileName, OpenMode);
#else
		*ppFile = fopen(FileName, OpenMode);
		return NULL == *ppFile;
#endif
		}

		int FileGetState(const char* pFilePath, struct FILESTATE& FState)
		{
#ifdef WIN32
		return _stat(pFilePath, &FState);
#else
		return  stat(pFilePath,&FState);
#endif
		}

		int IsFileEnd(FILE* pFile)
		{	return feof(pFile);	}

		int  IsFileError(FILE* pFile)
		{	return ferror(pFile);	}

		int FileClose(FILE* pFile)
		{	return fclose(pFile);	}

		int FileFlush(FILE* pFile)
		{	return fflush(pFile);	}

		int FileSeek(FILE* pFile, long offset, int origin)
		{	return fseek(pFile,offset,origin);	}

		int FileTell(FILE* pFile)
		{	return ftell(pFile);	}

		Uint FileRead(void *pBuf, int BufSize, int StructSize, int StructCount, FILE* pFile)
		{
#ifdef WIN32
			return fread_s(pBuf,BufSize, StructSize,StructCount,pFile);	
#else
			return fread  (pBuf			,StructSize,StructCount,pFile);
#endif
		}

		Uint FileWrite(const void *pSrc, int StructSize, int StructCount, FILE* pFile)
		{	return fwrite(pSrc,StructSize,StructCount,pFile);	}

		// make dir
		bool MakeDir(const char* FilePath, int mode)
		{
#ifdef WIN32
			return 0==_mkdir(FilePath);
#else
			return 0==mkdir(FilePath,mode);
#endif
		}

		int FileAccess( const char* FilePath, int mode)
		{
#ifdef WIN32
			return _access_s(FilePath, mode);
#else
			return  access  (FilePath, mode);
#endif
		}
	}; // namespace SClib
}; // namespace smm
