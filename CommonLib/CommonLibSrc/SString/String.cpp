#include "String.h"
#include "../SCommon/SClib.h"
#include "../SCommon/SIOStd.h"
#include <stdlib.h> // srand
#include <stdarg.h>

namespace smm
{
	// StringInner 是 String 实现所需要的内部变量或函数 -----------------------------------------
#define m_Capacity	m_Type
	// m_StrR 从左向右4个字节
	// 第3个为 bArray，第4个为 StepSize，其他的待定
#define m_bArray	(0x0000FF00 & m_SrcR)
#define m_StepSize	(0x000000FF & m_SrcR)
#define INITStrR	 0x00000010 // 第三个字节为00 表示 bArray 为 false（01表示bArray为true）, 第四个字节StepSize 为 0x10=16

	inline Uint GetNewCapacity( Uint NewSize , unsigned char StepSize,Uint m_OldCapacity)
	{	// 为了将末（m_sSize）位置 0 ,m_sSize 的位置必须存在,所以 NewSize 要加 1
		// 分配所需的最小空间,当 StepSize 为 8 时：7->8 , 8->8 , 9->16 , 15->16
		Uint NewCapacity = NewSize - NewSize % StepSize + StepSize;
		return NewCapacity;
	}

	// ===== String ======================================================================================================================
	// String
	String::String ( ) : StrImg(16,new char[16],0), m_SrcR(INITStrR)
	{	m_pStr[0] = 0;	}

	// String
	String::String (char Arr[], Uint Capacity, const StrImg& imStr) : StrImg(0,NULL,0)
	{
		if(!Arr || 0==Capacity || imStr.Size() >= Capacity)
		{	throw "String::String(char Arr[], Uint Capacity, const StrImg& imStr) Error! The Arr is an array and it is null or Capacity>0 or Size<Capacity ";	}
		m_SrcR = 0x00000110; // bArray 设置为 true
		m_pStr = Arr;
		m_Capacity = Capacity;
		Append(imStr);
	}

	// String
	String::String(const String& str) : StrImg(0,NULL,0), m_SrcR(INITStrR)
	{
		Append(str);
	}

	// String
	String::String(const StrImg& imStr):StrImg(0,NULL,0), m_SrcR(INITStrR)
	{
		Append(imStr);
	}

	// ~String ( );
	String::~String( )
	{
		if(!m_bArray)
		{	delete [] m_pStr;	}
	}

	String& String::operator  = (const char* pStr) // 针对 Array 的情况需要特殊处理
	{
		if(pStr)
		{
			m_Size = 0;
			const Uint Size = SClib::StrLen(pStr);
			Append(StrImg(pStr, Size));
		}
		return *this;
	}

	String& String::operator  = (const StrImg& imStr)
	{
		if(m_pStr!=imStr.ToCStr()) // 防止自赋值
		{
			if(imStr.IsEmpty())
			{	Clear();	}
			else
			{
				m_Size = 0;
				Append(imStr);
			}
		}
		else	
		{	throw "String::operator= (const StrImg& imStr) Error! A string should not be assigned with a null!";	}
		return *this;
	}

	String& String::operator  = (const String& Str)
	{
		if(m_pStr!=Str.ToCStr() && !Str.IsEmpty())
		{
			m_Size = 0;
			Append(Str);
		}
		return *this;
	}

	Uint String::Capacity( ) const
	{	return m_Capacity;	}

	void String::ReSize(Uint NewSize, bool bShrink)
	{
		if(m_bArray)
		{
			if(NewSize >= m_Capacity)// 相等意味着将没有空间放置末尾的 0
			{	throw "String::ReSize(Uint NewSize, bool bShrink) Error! No more space available. For SArray, The capacity is fixed!";	}
			m_pStr[NewSize] = 0;
			return;
		}
		const Uint NewCapacity = GetNewCapacity(NewSize, m_StepSize, m_Capacity);
		if (NewCapacity != m_Capacity)
		{
			if ( bShrink || NewCapacity>m_Capacity ) // 若不是“实际需求降低且不用收缩”
			{	// 重新分配空间
				char* pTmp = new char[NewCapacity];
				// 对于长度为0的串，下面的if不执行，所以无法设置一个字符为结束符，先在这里设置，如果原长度不为0，则下面的if会执行pTmp的空间会被覆盖而保证正确性
				pTmp[0] = 0;
				if(m_Size>0 && NewSize>0) // 若 NewSize 为 0,则不移动任何数据
				{
					Uint MinSize = NewSize<m_Size ? NewSize : m_Size;
					SClib::MemCpy(pTmp,NewCapacity, m_pStr, MinSize);
					pTmp[MinSize] = 0;
				}
				delete [] m_pStr; // 释放原空间
				m_pStr = pTmp;
				m_Capacity = NewCapacity;
			}
		} // 不收缩空间,则只设置 m_pStr,不分配空间
		if ( NewSize < m_Size) // 只有变小时才设置新的大小 NewSize
		{	m_Size = NewSize;	}
		m_pStr[NewSize] = 0; // 设置字符串结尾符,对字符流亦无影响
	}

	void String::SetEndPos(Uint NewSize)
	{
		if(NewSize < m_Capacity)
		{
			m_Size = NewSize;
			m_pStr[m_Size] = 0;
		}
		else
		{	throw "String::SetEndPos(Uint NewSize) Error! The NewSize is larger than or equal to the Capacity!";	}
	}

	char& String::operator []  ( Uint sPos )
	{
		if(sPos < m_Capacity)
		{	return m_pStr[sPos];	}
		else
		{	throw "String::operator [] Error! sPos < m_Capacity";	}
	}

	void String::SetAt(Uint sPos , char c)
	{
		if(sPos < m_Capacity)
		{	m_pStr[sPos] = c;	}
		else
		{	throw "SetAt(Uint sPos , char c) Error! sPos < m_Capacity";	}
	}

	void String::Clear(bool bShrink)
	{
		if(m_bArray)
		{	SetEndPos(0);	}
		else
		{	ReSize(0,bShrink);	}
	}

	String& String::AppendC(char c , Uint nCount)
	{
		Uint OldLastPos = m_Size;
		m_Size += nCount;
		ReSize(m_Size,false);
		for(Uint i=OldLastPos;i<m_Size;++i)
		{	m_pStr[i] = c;	}
		return *this;
	}

	String& String::Append(const StrImg& imStr)
	{
		if(!imStr.IsEmpty())
		{
			const Uint SrcSize = imStr.Size();
			const Uint NewSize = SrcSize + m_Size;
			ReSize(NewSize,false);
			SClib::MemCpy( m_pStr+m_Size, NewSize , imStr.ToCStr() , SrcSize);
			m_Size = NewSize;
		}
		return *this;
	}
/*
	String& String::Append( Uint n)
	{
		char pStr[32];
		int len = sprintf_s(pStr,32,"%u",n);
		if(-1 != len)
		{
			ReSize(m_Size+32);
			Append(StrImg(pStr,len));
		}
		return *this;
	}

	String& String::Append(  int n)
	{
		char pStr[32];
		int len = sprintf_s(pStr,32,"%d",n);
		if(-1 != len)
		{
			ReSize(m_Size+32);
			Append(StrImg(pStr,len));
		}
		return *this;
	}
*/
	// Replace with newStr by searching the oldStr from sPos
	void String::Replace( const StrImg& oldStr , const StrImg& newStr , Uint sPos , Uint nCount , bool IsCase)
	{
		if(oldStr.IsEmpty() || newStr.IsNull())
		{	return;	}
		if(sPos >= m_Size)
		{	return;	}
		const Uint OldPartSize = oldStr.Size();
		const Uint NewPartSize = newStr.Size();
		nCount = nCount<m_Size ? nCount : m_Size;
		Uint tPos = SClib::Min<Uint>(nCount+sPos , m_Size);
		const Uint EachFindMaxCount = 16; // 每次寻找要搜索的个数，然后统一替换，若还有的话再搜再换
		bool bFindAgain = true; // 一次搜索不完就再搜，直到搜完为止
		Uint thePos = 0;
		Uint PosArr[EachFindMaxCount]; // 记录找到的位置
		Uint FoundCount = 0; // 一次所找到的个数
		while(bFindAgain)
		{
			FoundCount = 0;
			while(FoundCount < EachFindMaxCount)
			{
				thePos = Find(oldStr,sPos,nCount,IsCase);
				if(TheEndPos != thePos)
				{
					PosArr[FoundCount] = thePos;
					++ FoundCount;
					sPos = thePos + OldPartSize;
				}
				else
				{
					bFindAgain = false;
					break;
				}
			}
			if(0 == FoundCount)
			{	break;	}
			// need larger space
			if(NewPartSize > OldPartSize)
			{
				Uint NewSize = m_Size + FoundCount * (NewPartSize - OldPartSize);
				ReSize(NewSize,false);
			}
			// replace content
			int PosOffSet = NewPartSize - OldPartSize;; // The old pos has changed after replaced each time
			// 在这里统一替换
			for(Uint j=0;j<FoundCount;++j)
			{	Replace(newStr , PosArr[j] + PosOffSet * j , OldPartSize);	}
		}
	}

	// Replace with newStr at sPos
	void String::Replace(const StrImg& imNewPart , Uint sPos , Uint OldPartSize)
	{	// 有效条件：pNewPart!=NULL
		// 1、sPos==m_Size	-> Append （此时 OldSize 无效）
		// 2、OldSize==0	-> Insert
		// 3、sPos<m_Size 且 OldPartSize>0 -> Replace
		if(sPos == m_Size) // Append
		{	Append(imNewPart);	}
		else if(sPos < m_Size)
		{
			if(0 == OldPartSize) // Insert
			{	Insert(sPos,imNewPart);	}
			else // Replace
			{
				const char* pNewPart = imNewPart.ToCStr();
				const Uint NewPartSize= imNewPart.Size();
				OldPartSize = OldPartSize<m_Size ? OldPartSize : m_Size;
				Uint tPos = SClib::Min<Uint>(sPos+OldPartSize,m_Size);
				Uint SrcSize = m_Size-tPos;
				OldPartSize = tPos - sPos; // tPos-sPos 才是有效的 OldPartSize
				const int diff = NewPartSize - OldPartSize;
				m_Size += diff;
				ReSize(m_Size,false);
				if(0 != diff)
				{	SClib::MemCpy( m_pStr+tPos+diff	, m_Capacity , m_pStr+tPos	, SrcSize); } // < 则为前移,否则的后移
				SClib::MemCpy( m_pStr+sPos , m_Capacity , pNewPart , NewPartSize);
				m_pStr[m_Size] = 0;
			}
		}
		else
		{	throw "String::Replace(const StrImg& imNewPart , Uint sPos , Uint OldPartSize) Error! The Pos should not be larger than Size!";	}
	}

	void String::Delete(Uint sPos , Uint nCount)
	{	// 只有 nCount>0 且 sPos+nCount<m_Size 时会引起元素移动,为了防止 nCount 为负数的情况,还要保证
		// nCount< m_Size
		nCount = nCount<m_Size ? nCount : m_Size;
		if(0 == nCount) // 删除 0 个无动作
		{	return;	}

		sPos = sPos<m_Size ? sPos : m_Size;
		Uint tPos = SClib::Min<Uint>(nCount+sPos , m_Size);
		if(tPos < m_Size) // 需要从 tPos 处开始移动（从后向前）
		{
			SClib::MemCpy( m_pStr+sPos, m_Size , m_pStr+tPos, m_Size-tPos);
			m_Size -= nCount;
		}
		else // sPos（含） 之后的都要删除,则 sPos 变为新串的 tPos
		{	m_Size = sPos;	}
		m_pStr[m_Size] = 0;
	}

	void String::Retain(Uint sPos , Uint nCount)
	{	// 要保证 sPos , nCount ,tPos 都在有效范围内
		sPos = sPos<m_Size ? sPos : m_Size;
		nCount = nCount<m_Size ? nCount : m_Size;
		Uint tPos = SClib::Min<Uint>(nCount+sPos , m_Size);

		Delete(tPos,m_Size);
		Delete(0,sPos);
	}

	void String::Insert(Uint sPos,const StrImg& imStr)
	{
		if(imStr.IsEmpty())
		{	return;	}

		if(sPos < m_Size)
		{
			const Uint StrSize = imStr.Size();
			const Uint NewCapacity = GetNewCapacity(m_Size + StrSize, m_StepSize, m_Capacity);
			if(NewCapacity>m_Capacity) // 需要重新分配空间
			{
				if(m_bArray)
				{	throw "String::Insert(Uint sPos,const StrImg& imStr) Error! The string is an array and it has no enough space";	}
				char* pTmp = new char[NewCapacity + m_Capacity/4]; // 做 Insert 操作时要适当增加空间以避免频繁的数据复制
				// 与 ReSize 不同,这里分段移动
				// 先移动源串 0-sPos 的数据至新空间
				SClib::MemCpy(pTmp		, NewCapacity , m_pStr			, sPos);
				// 再移动要插入的串 pStr 至新空间的 sPos 处
				SClib::MemCpy(pTmp+sPos	, NewCapacity , imStr.ToCStr()	, StrSize);
				// 最后移动源串剩余部分
				SClib::MemCpy(pTmp+sPos+StrSize, NewCapacity , m_pStr+sPos , m_Size-sPos);
				delete [] m_pStr; // 释放原空间
				m_pStr = pTmp;
				m_Capacity = NewCapacity;
			}
			else// 在原空间内移动
			{	// caution : 分两部分移动,先插入点后的后移,再将插入串复制到插入点处
				SClib::MemCpy(m_pStr+sPos+StrSize,m_Capacity,m_pStr+sPos,m_Size-sPos); // 后向移动
				SClib::MemCpy(m_pStr+sPos,m_Capacity,imStr.ToCStr(),StrSize);
			}
			m_Size += StrSize;
			m_pStr[m_Size] = 0;
		}
		else if(sPos == m_Size)
		{	Append(imStr);	}
		else
		{	throw "String::Insert(Uint sPos,const StrImg& imStr) Error! The Pos should not be larger than Size!";	}
	}

	// format
	void String::MakeUpper( )
	{
		char c  = 0;
		for ( Uint i=0 ; i<m_Size ; ++i )
		{
			c = m_pStr[i];
			if('a'<=c && c<='z')
			{	m_pStr[i] = c-32 ;	}
		}
	}

	void String::MakeLower( )
	{
		char c  = 0;
		for ( Uint i=0;i<m_Size;++i )
		{
			c = m_pStr[i];
			if('A'<=c && c<='Z')
			{	m_pStr[i] = c+32 ;	}
		}
	}

	void String::MakeReverse( )
	{
		Uint nHalf = m_Size/2; // 4->2,5->2
		char c = 0;
		for(Uint i=0;i<nHalf;++i)
		{
			c					= m_pStr[i];
			m_pStr[i]			= m_pStr[m_Size-1-i];
			m_pStr[m_Size-1-i]	= c;
		}
	}
#ifdef WIN32
	int String::FormatStr(StrImg imFmt, ...)
	{	// 0: 正常;-1:pFmt为 NULL; -2:长度过大;
		if(imFmt.IsEmpty())
		{	throw "String::FormatStr(const char* pFmt , ...) Error! The format parameter should not be empty!" ;	}
		const char* pFmt = imFmt.ToCStr();
		if(pFmt == m_pStr)
		{	return -1;	}
		char* pFirst = reinterpret_cast<char*>(&imFmt)+ sizeof(imFmt);
		int len = SClib::GetVaryFormatLen(pFmt,pFirst) + sizeof(char);
		if(-1 == len)
		{	return -1;	}
		m_Size = 0;
		ReSize(len,false);
		len = SClib::SetVaryFormatBuf(m_pStr,len,pFmt,pFirst);
		if(-1 == len)
		{	return -1;	}
		m_pStr[len] = 0;
		m_Size = len;
		return 0;
	}
#else
	int String::FormatStr(StrImg imFmt, ...)
	{	// 0: 正常;-1:pFmt为 NULL; -2:长度过大;
		if(imFmt.IsEmpty())
		{	throw "String::FormatStr(const char* pFmt , ...) Error! The format parameter should not be empty!" ;	}
		const char* pFmt = imFmt.ToCStr();
		if(pFmt == m_pStr)
		{	return -1;	}
		char* pFirst = reinterpret_cast<char*>(&imFmt)+ sizeof(imFmt);
		va_list vArgs;
		va_start(vArgs, imFmt);
		int len = vsnprintf(nullptr, 0, pFmt, vArgs);
		va_end(vArgs);
		if(-1 == len)
		{	return -1;	}
		m_Size = 0;
		ReSize(len,false);
		len = vsnprintf(m_pStr, len, pFmt, vArgs);		
		if(-1 == len)
		{	return -1;	}
		m_pStr[len] = 0;
		m_Size = len;
		return len;
		return 0;
	}
#endif
/*	int  FormatImg( StrImg imFmt, ...)
	{
		return 0;
	}
*/
	struct tagFormatSeqInfo
	{
		Uint SeqNo;	// 序号
		Uint sPos;	// “%”起始位置
		Uint nCount;// 整个符号所占据的空间大小
	};

	int String::FormatArr ( const StrImg& imFmt, StrImg ValArr[],Uint ArrSize )
	{
		if(imFmt.IsEmpty())
		{	return -1;	} // throw "StringCString::FormatArr(const char* pFmt , ...);The first parameter should not be NULL!" ;	}
		const Uint FmtSize = imFmt.Size();
		// 最多支持 32 个参数
		static const Uint MaxParamCount = 32;
		// 先确定参数个数、序号、原括号位置和大小
		Uint ArgCount = 0;
		tagFormatSeqInfo SeqInfoArr[MaxParamCount] = {0};
		static char Persent	= '%';
		static Uint  CZero	= static_cast<Uint>('0');
		static Uint  CNine	= static_cast<Uint>('9');
		Uint C = 0;
		Uint sPosNo = 0;
		Uint tPosNo = 0;
		const char* pFmt = imFmt.ToCStr();
		Uint nMaxNo = 0; // 记录最大的序号
		for(Uint i=0;i<FmtSize;++i) // 遍历strFormat
		{
			if(pFmt[i] == Persent)
			{	// 这中间不能有空白符
				sPosNo = ++i; // 数字的位置，只允许一位数字
				if(i < FmtSize)
				{
					C = pFmt[i];
					if(CZero<=C && C<=CNine)
					{
						C -= 48;
						nMaxNo = SClib::Max<Uint>(nMaxNo,C);
						SeqInfoArr[ArgCount].SeqNo = C;
						SeqInfoArr[ArgCount].sPos = sPosNo-1;
						++ArgCount;
					}
					else
					{	continue;	}
				}
			}
		}

		// 参数过多 或 序号比数量大 或 序号超过最大值
		if(ArgCount>MaxParamCount || nMaxNo>32 || nMaxNo>ArrSize-1)
		{	return -2;	} //throw "StringCString::FormatArr(const char* pFmt , ...);The max count of valid parameter is 16!"
		else if(0 == ArgCount) // 无参数
		{	return 0;	}

		Uint MinNewSize = Size() + imFmt.Size();
		for(Uint i=0;i<ArrSize;++i)
		{
			if(!ValArr[i].IsNull())
			{	MinNewSize += ValArr[i].Size();	}
		}

		if(MinNewSize >= m_Capacity)
		{	ReSize(MinNewSize,false);	}
		int OffSet = Size();	// 此偏移量要记录 Fmt 串之前的长度
		m_Size = 0;
		Append(imFmt); // 总是需要记录 Fmt 串
		StrImg strImg;
		for(Uint i=0;i<ArgCount;++i)
		{
			tagFormatSeqInfo& rInfo = SeqInfoArr[i];
			strImg = ValArr[rInfo.SeqNo];
			if(strImg.IsNull())
			{	continue;	}
			Replace(strImg, rInfo.sPos+OffSet, 2);
			OffSet += strImg.Size()-2;
		}
		return 0;
	}

	// trim
	String& String::Trim(int TrimType)
	{
		if(IsEmpty())
		{	return *this;	}
		//  0, trim both
		Uint sPos=0, tPos=m_Size;
		char c = 0;

		// -1, trim left
		if(-1==TrimType || 0==TrimType)
		{
			c = 0;
			sPos = 0;
			for( ;sPos < m_Size; ++sPos)
			{
				c = m_pStr[sPos];
				if(' '==c || '\t'==c || '\n'==c || '\r'==c)
				{	continue;	}
				else
				{	break;	}
			}

			Delete(0,sPos);
		}

		// -1, trim right
		if( 1==TrimType || 0==TrimType)
		{
			sPos = m_Size-1;

			for( ;0 != sPos; --sPos) // 使用 != 比 <= 要好，因为 Uint 总是 >=0，将陷入无线循环
			{
				c = m_pStr[sPos];
				if(' '==c || '\t'==c || '\n'==c || '\r'==c)
				{	continue;	}
				else
				{	break;	}
			}

			m_Size = sPos + 1;
			m_pStr[m_Size] = 0;
		}
		return *this;
	}

	void String::TakeOver(char*& pNew , Uint NewSize , Uint NewCapacity)
	{
		if(m_bArray) // bArray 改为 0
		{	m_SrcR = m_StepSize;	}
		else
		{	delete [] m_pStr;	}
		m_pStr = pNew;
		m_Size = NewSize;
		pNew = NULL; // 置0，防止使用原资源！
		if(NewCapacity > NewSize)
		{
			m_pStr[NewSize] = 0;
			m_Capacity = NewCapacity;
		}
		else
		{	m_Capacity = NewSize;	}
	}

	String& String::operator += (const StrImg& imStr)
	{
		Append(imStr);
		return *this;
	}

/*	String& String::operator +=( Uint n)
	{	return Append(n);	}

	String& String::operator +=( int n)
	{	return Append(n);	}
*/
	void String::Swap(String& Str)
	{
		unsigned char * pObj1 = reinterpret_cast<unsigned char *>(&Str);
		unsigned char * pObj2 = reinterpret_cast<unsigned char *>(this);
		for (Uint i = 0; i<sizeof(String); ++i)
		{
			pObj1[i] ^= pObj2[i];
			pObj2[i] ^= pObj1[i];
			pObj1[i] ^= pObj2[i];
		}
	}

	bool String::operator < (const String& str) const
	{	return StrImg::Compare(str, false) < 0;	}

	int SMDLLX GetRandomNumber(int a , int b , Uint seed)
	{
		if(0 == seed)
		{	seed = Uint(time(NULL));	}
		srand(seed);
		return static_cast<int>(((double)rand()/RAND_MAX)*(b-a) + a);
	}

	void SMDLLX GetRandomString(String& strDst , Uint nCount , Uint seed , char sC , char tC)
	{
		if(0==nCount || nCount>1024)
		{	throw "String.cpp GetRandomString(String& strDst , Uint nCount , Uint seed , char sC , char tC) Error! The nCount should be 1024 at last!";	}
		if(0 == seed)
		{	seed = Uint(time(NULL));	}
		srand(seed);

		const Uint dstLen = nCount+1;

		char * pDst = NULL;
		bool bNew = true;
		if(strDst.Size() >= dstLen)
		{
			pDst = const_cast<char*>(strDst.ToCStr());
			bNew = false;
		}
		else
		{	pDst = new char[dstLen];}

		pDst[nCount] = 0;
		for(Uint i=0;i<nCount;++i)// 随机可见字符
		{	pDst[i] = static_cast<char>(((double)rand()/RAND_MAX)*(tC-sC) + 97);	}
		if(bNew)
		{	strDst.TakeOver(pDst,nCount,nCount+1);	}
	}
	// ------------------------------------------------------------------------------------------------

	StrSplit::StrSplit(const StrImg& imSrc, const StrImg& imDelim) : m_imSrc(imSrc)
	{
		m_Count = 0;
		if(imSrc.IsEmpty())
		{	return;	}
		const Uint SrcSize = imSrc.Size();
		m_DelimSize = imDelim.Size();
		m_PosArr[0] = 0;
		m_Count = 1;
		for(Uint sPos=0, tPos=0; sPos<SrcSize && m_Count<65536; )
		{
			tPos = imSrc.Find(imDelim, sPos, SrcSize);
			if(TheEndPos != tPos)
			{
				m_PosArr[m_Count++] = tPos;
				sPos = tPos + m_DelimSize;
			}
			else
			{	break;	}
		}
		if(m_Count < 65536)
		{	m_PosArr[m_Count] = SrcSize;	}
	}

	Uint StrSplit::GetCount()
	{	return m_Count;	}

	StrImg StrSplit::GetAt(Uint Index)
	{
		StrImg imRet;
		if(Index < m_Count)
		{
			Word sPos = 0==Index ? 0 : m_PosArr[Index]+m_DelimSize;
			Word tPos = m_PosArr[Index+1];
			if(tPos != sPos)
			{	imRet.SetImg(m_imSrc.ToCStr()+sPos, tPos-sPos);	}
		}
		return imRet;
	}

	// 全局	的“+”
	String operator + (const StrImg& StrL , const StrImg& StrR)
	{
		String str(StrL);
		str.Append(StrR);
		return str;	
	}

	String operator + (const char* pStrL , const StrImg& StrR)
	{
		StrImg imTmp(pStrL);
		String str(imTmp);
		str.Append(StrR);
		return str;
	}

	String SMDLLX operator + (const StrImg& StrL , const char* strR)
	{
		StrImg imTmp(strR);
		String str(StrL);
		str.Append(imTmp);
		return str;
	}
}; // namespace
