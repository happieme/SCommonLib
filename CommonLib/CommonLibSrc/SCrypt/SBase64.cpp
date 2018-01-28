#include "SCrypt.h"

namespace smm
{	// 64 个字符，128 个数
	static const Byte BaseArr [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const Byte IndexArr[] =
	{
		255,255,255,255,  255,255,255,255,  255,255,255,255,  255,255,255,255,
		255,255,255,255,  255,255,255,255,  255,255,255,255,  255,255,255,255,
		255,255,255,255,  255,255,255,255,  255,255,255, 62,  255,255,255, 63,
		52, 53, 54, 55,   56, 57, 58, 59,   60, 61,255,255,  255,255,255,255,
		255,  0,  1,  2,    3,  4,  5,  6,    7,  8,  9, 10,   11, 12, 13, 14,
		15, 16, 17, 18,   19, 20, 21, 22,   23, 24, 25,255,  255,255,255,255,
		255, 26, 27, 28,   29, 30, 31, 32,   33, 34, 35, 36,   37, 38, 39, 40,
		41, 42 ,43, 44,   45, 46, 47, 48,   49, 50, 51,255,  255,255,255,255
	};

	void SCrypt::Base64_Encode(const StrImg& StrSrc , String& StrDst)
	{
		const char* bytes_to_encode = StrSrc.ToCStr();
		Uint SrcLen = StrSrc.Size();
		Byte cArr3[3]; 
		Byte cArr4[4];
		StrDst.Clear();
		Uint Index = 0;
		while(SrcLen--)
		{
			cArr3[Index++] = *(bytes_to_encode++);  
			if (Index == 3) 
			{
				cArr4[0] = (cArr3[0] & 0xfc) >> 2;  
				cArr4[1] = ((cArr3[0] & 0x03) << 4) + ((cArr3[1] & 0xf0) >> 4);  
				cArr4[2] = ((cArr3[1] & 0x0f) << 2) + ((cArr3[2] & 0xc0) >> 6);  
				cArr4[3] = cArr3[2] & 0x3f;  

				for(Index = 0; Index<4 ; Index++)
				{	StrDst.AppendC(BaseArr[cArr4[Index]]);	}
				Index = 0;  
			}  
		}  

		if(Index)
		{  
			Uint j = 0;  
			for(j = Index; j < 3; j++)  
			{	cArr3[j] = '\0';	}

			cArr4[0] = (cArr3[0] & 0xfc) >> 2;  
			cArr4[1] = ((cArr3[0] & 0x03) << 4) + ((cArr3[1] & 0xf0) >> 4);  
			cArr4[2] = ((cArr3[1] & 0x0f) << 2) + ((cArr3[2] & 0xc0) >> 6);  
			cArr4[3] = cArr3[2] & 0x3f;  

			for (j = 0; (j < Index + 1); j++)  
			{	StrDst.AppendC(BaseArr[cArr4[j]]);	}

			while((Index++ < 3))  
			{	StrDst.Append("=");	}
		}  
	}  


	void SCrypt::Base64_Decode(const StrImg& StrSrc , String& StrDst)
	{
		if(StrSrc.Size() < 4)
		{	return;	}
		const Uint SrcLen = StrSrc.Size();
		const Uint DstLen = SrcLen/4*3+4;
		StrDst.Clear();
		StrDst.ReSize(DstLen);
		StrDst.SetEndPos(DstLen);
		Uint nCount = 0;
		Byte ch = 0;
		Byte ar = 0;
		Uint iDst = 0;
		Uint i = 0;
		for(; i<SrcLen; ++i)
		{
			ar = StrSrc[i];
			if(ar > 0x7F)
			{	continue;	}

			ch = IndexArr[ar];
			if (255 == ch)
			{	continue;	}

			switch(nCount)
			{
			case 0:
				StrDst[iDst] = ch<<2;
				++ nCount;
				break;
			case 1:
				StrDst[iDst++] |= ch>>4;	// *p++ |= ch>> 4;
				StrDst[iDst  ]  = ch<<4;	// *p = ch << 4;
				++ nCount;
				break;
			case 2:
				StrDst[iDst++] |= ch>>2;	// *p++ |= ch >> 2;
				StrDst[iDst  ]  = ch<<6;	// 	*p = a << 6;
				++ nCount;
				break;
			case 3:
				StrDst[iDst++] |= ch;	// *p++ |= ch;
				nCount = 0;
				break;
			} 
		}
		StrDst.SetEndPos(iDst);
	}

}; // namespace smm
