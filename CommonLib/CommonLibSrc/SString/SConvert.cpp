#include "SConvert.h"
#include "../SString/SymbolConstant.h"

#ifdef WIN32
	#include <windows.h>
#else
	#include <stdlib.h>
	#include <locale.h>
#endif

namespace smm
{
	Uint U8StrToWStr( const StrImg& SrcStr, String& DstStr);
	Uint WStrToU8Str( const StrImg& SrcStr, String& DstStr);
	void UCharToU8Char(CharW SrcChar, char *DstU8);
	char BCDCode_2_Char( char cHigh , char cLow );
	char HexChar_2_HexValue(char c);
	void MB_2_UN ( Uint CodeID , const char * pSrcC , Uint cLen , String & DstW );
	void UN_2_MB ( Uint CodeID , const CharW * pStrW , Uint wLen , String & DstC );
	Uint MultiByte_2_Unicode ( Uint CodeID , const char * pStrC , Uint cLen , 
									  CharW * pStrW , Uint wLen , ULong dwFlags=0 );
	Uint Unicode_2_MultiByte ( Uint CodeID , char * pStrC , Uint cLen , 
									  const CharW * pStrW , Uint wLen , ULong dwFlags=0 );

	// --------------------------------------------------------------------------------------------------
	char BCDCode_2_Char( char cHigh , char cLow )
	{
		char dst = 0;
		dst  = HexChar_2_HexValue(cHigh)<<4;
		dst |= HexChar_2_HexValue(cLow);
		return dst;
	}

	char HexChar_2_HexValue(char c)
	{
		char dst = c;
		if(0x41<=c && c<=0x46) // 65->A
		{	dst -= 0x37;	}
		else if(0x30<=c && c<=0x39) // 48->0
		{	dst -= 0x30;	}
		else if(0x61<=c && c<=0x66) // 97->a
		{	dst -= 0x57;	}
		else
		{	dst = 0;	}
		return dst;
	}

	// LINUX下没有下面的两个函数（WINAPI），需要使用函数 mbstowcs和wcstombs
	Uint MultiByte_2_Unicode( Uint CodeID , const char * pStrC , Uint cLen , 
		CharW * pStrW , Uint wLen , ULong dwFlags )
	{
#ifdef WIN32
		return ::MultiByteToWideChar ( CodeID , dwFlags , pStrC , cLen , pStrW , wLen );
#else	// 此时的多字节的具体类别（utf8还是gb2312等由 SetLocale()指定）
		return mbstowcs(pStrW , pStrC , cLen);
#endif
	}

	Uint Unicode_2_MultiByte( Uint CodeID , char * pStrC , Uint cLen , 
		const CharW * pStrW , Uint wLen , ULong dwFlags )
	{
#ifdef WIN32
		return ::WideCharToMultiByte ( CodeID , dwFlags , pStrW , wLen , pStrC , cLen ,NULL,NULL );
#else
		return wcstombs(pStrC , pStrW , cLen);
#endif
	}

	void MB_2_UN ( Uint CodeID , const char * pStrC , Uint cLen , String & DstW )
	{
		Uint wLen = MultiByte_2_Unicode(CodeID , pStrC , cLen , NULL , 0 );
		CharW * pStrW = NULL;
		bool bNew = true;
		if(DstW.Size() >= wLen+1)
		{
			pStrW = (CharW*)(DstW.ToCStr());
			bNew  = false;
		}
		else
		{	pStrW = new CharW[wLen+1];}

		MultiByte_2_Unicode(CodeID , pStrC , cLen , pStrW , wLen );
		pStrW[wLen] = 0;
		if(bNew)
		{
			char* pStrWC = (char*)pStrW;
			DstW.TakeOver(pStrWC, wLen*sizeof(CharW), wLen*sizeof(CharW)+1);
		}
	}

	void UN_2_MB ( Uint CodeID , const CharW * pStrW , Uint wLen , String & strDstC )
	{
		int cLen = Unicode_2_MultiByte(CodeID  , NULL , 0 , pStrW , wLen ); // un to mb
	//	cout<<"探测长度: "<<cLen<<endl;
		char * pStrC = new char[cLen + 1];
		Unicode_2_MultiByte(CodeID , pStrC , cLen , pStrW , wLen , false);
		pStrC[cLen] = 0;
		strDstC.TakeOver(pStrC,cLen,cLen+1);
	}

	// SConv --------------------------------------------------------------------------------------------
	// public 
	void SConv::U8_U ( const StrImg & iSrcC , String & DstW )
	{
#ifdef WIN32
		MB_2_UN ( 65001 , iSrcC.ToCStr() , iSrcC.Size(), DstW);
#else
		U8StrToWStr(iSrcC, DstW);
#endif
	}

	void SConv::U_U8 ( const StrImg & iSrcW , String & DstC)
	{
#ifdef WIN32
		UN_2_MB(65001 , (const CharW*)(iSrcW.ToCStr()) , iSrcW.Size()/sizeof(CharW), DstC);
#else
		WStrToU8Str(iSrcW, DstC);
#endif
	}

	void SConv::U_A ( const StrImg & iSrcW , String & strDst )
	{
#ifndef WIN32
		char* p = setlocale(LC_ALL,"zh_CN");
	//	if(!p)	{cout<<"setlocale Error!"<<endl;}
	//	else	{cout<<"U_A setlocale()"<<p<<endl;
#endif
		UN_2_MB ( 936 , (const CharW*)(iSrcW.ToCStr()) , iSrcW.Size()/sizeof(CharW) , strDst );	
	}

	void SConv::A_U ( const StrImg & iStrSrc , String & DstW )
	{
#ifndef WIN32
		setlocale(LC_ALL,"zh_CN");
#endif
		MB_2_UN ( 936 , iStrSrc.ToCStr() , iStrSrc.Size() , DstW);
	}

	const Uint tmpBufSize = 1024*16;

	String	SConv::A_U8 ( const StrImg & iStrSrc )
	{
		SARRAY(strDst, tmpBufSize);
		A_U8(iStrSrc , strDst);
		return strDst;
	}

	void SConv::A_U8	( const StrImg & iStrSrc , String & strDst )
	{
		SARRAY(tmp, tmpBufSize);
		A_U(iStrSrc, tmp);
		U_U8(tmp, strDst);
	}

	String	SConv::U8_A( const StrImg & iStrSrc)
	{
		SARRAY(strDst, tmpBufSize);
		U8_A(iStrSrc , strDst);
		return strDst;
	}

	void SConv::U8_A	( const StrImg & iStrSrc , String & strDst )
	{
		SARRAY(tmp, tmpBufSize);
		U8_U(iStrSrc,tmp);
		U_A(tmp,strDst);
	}

	void SConv::UrlDecode( const StrImg& strSrc , String& strDst)
	{
		if(TheEndPos == strSrc.Find(SymbolConstant::imPercent))
		{
			strDst = strSrc;
			return;
		}
		
		char mark = '%' ; // mark 的个数应为单独一个或连续三个，???还有 %2520这样两个的情况需要两次解码！
		const Uint SrcSize = strSrc.Size();
		strDst.ReSize(SrcSize);
		strDst.Clear();
		char c; Uint i = 0;
		while(i < SrcSize)
		{
			c = strSrc.GetAt(i);
			if(mark == c)
			{
				if(i+2<SrcSize) // 确保完整的形式如“%E5”
				{
					c = BCDCode_2_Char(strSrc.GetAt(i+1),strSrc.GetAt(i+2));
					if(c > 0) // ascii 字符 0-9 a-z A-Z
					{
						strDst.AppendC(c);
						i += 3;
					}
					else if(c<0 && i+8<SrcSize) // 汉字,连续的三个编码“%E5%A3%F2”
					{
						if(i+8<SrcSize && mark==strSrc.GetAt(i+3) && mark==strSrc.GetAt(i+6))
						{
							strDst.AppendC(c);
							strDst.AppendC( BCDCode_2_Char(strSrc.GetAt(i+4),strSrc.GetAt(i+5)) );
							strDst.AppendC( BCDCode_2_Char(strSrc.GetAt(i+7),strSrc.GetAt(i+8)) );
							i += 9;
						}
					}
				}
				else
				{	++i;	}
			}
			else
			{	
				strDst.AppendC(c);
				++i;
			}
		} // if
	} // for

	Uint U8StrToWStr( const StrImg& SrcStr, String& DstStr)
	{
		const char* pSrcU8 = SrcStr.ToCStr();
		const Uint SrcLen = SrcStr.Size();
		Uint SrcIndex = 0;
		Uint U8CharByte = 0; // 用 U8 来表示时用几个字节
		unsigned char U8Char = 0;
		// 先进行长度探测
		Uint nDstLenDetect = 0;
		while(SrcIndex < SrcLen)
		{
			U8Char = pSrcU8[SrcIndex];
			if (0 == (U8Char & 0x80))
			{	U8CharByte = 1;	}
			else if(0xE0 == (U8Char & 0xF0))  ///< 1110-xxxx 10xx-xxxx 10xx-xxxx
			{	U8CharByte = 3;	} 
			SrcIndex += U8CharByte;
			++ nDstLenDetect;
		}
		if(SrcLen != SrcIndex)
		{
			nDstLenDetect = TheMaxCount;
			return TheMaxCount;
		}
		DstStr.Clear();
		const Uint NewDstSize = nDstLenDetect*sizeof(CharW);
		DstStr.ReSize(NewDstSize);
		DstStr.SetEndPos(NewDstSize);
		Uint DstIndex = 0;
		CharW WChar = 0;
		CharW* pWStr = (CharW*)DstStr.ToCStr();
		SrcIndex = 0;
		while ( SrcIndex < SrcLen )
		{
			U8Char = pSrcU8[SrcIndex];
			if (0 == (U8Char & 0x80))
			{
				U8CharByte = 1;
				pWStr[DstIndex] = U8Char;
			}
			else if(0xE0 == (U8Char & 0xF0))  ///< 1110-xxxx 10xx-xxxx 10xx-xxxx
			{
				U8CharByte = 3;
				WChar  = (pSrcU8[SrcIndex]	 & 0x1F) << 12;
				WChar |= (pSrcU8[SrcIndex+1] & 0x3F) << 6;
				WChar |= (pSrcU8[SrcIndex+2] & 0x3F);
				pWStr[DstIndex] = WChar;
			}
			SrcIndex += U8CharByte;
			++ DstIndex;
		}
		pWStr[DstIndex] = 0;
		return DstIndex;
	}


	Uint WStrToU8Str(const StrImg& SrcStr , String& DstStr)
	{
		const CharW* pSrcW = (const CharW*)(SrcStr.ToCStr());
		const Uint SrcLen = SrcStr.Size()/sizeof(CharW);
		Uint SrcIndex = 0;
		// 先进行长度探测
		Uint nDstLenDetect = 0;
		CharW WChar = 0;
		Uint U8CharByte = 0;
		while(SrcIndex < SrcLen)
		{
			WChar = pSrcW[SrcIndex];
			if (WChar <= 0x007F)
			{	U8CharByte = 1;	}
			else if (WChar >= 0x0080 && WChar <= 0x07FF)
			{	U8CharByte = 2;	}
			else if (WChar >= 0x0800 && WChar <= 0xFFFF)
			{	U8CharByte = 3;	}
			nDstLenDetect += U8CharByte;
			++ SrcIndex;
		}
		if(SrcIndex != SrcLen)//参数有效性判断
		{
			nDstLenDetect = TheMaxCount;
			return TheMaxCount;
		}

		DstStr.Clear();
		DstStr.ReSize(nDstLenDetect);
		SrcIndex = 0;
		while(SrcIndex < SrcLen)
		{  
			WChar = pSrcW[SrcIndex];
			if (WChar < 0x80)
			{	DstStr.AppendC((char)WChar);	}
			else if (WChar >= 0x0800 && WChar <= 0xFFFF)
			{
				DstStr.AppendC(0xE0 | ( WChar >> 12));
				DstStr.AppendC(0x80 | ((WChar >> 6) & 0x003F));
				DstStr.AppendC(0x80 | ( WChar & 0x3F));
			}
			++ SrcIndex;
		}
		return nDstLenDetect;
	}
}; // namespace
