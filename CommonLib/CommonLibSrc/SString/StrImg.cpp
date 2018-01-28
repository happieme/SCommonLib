#include "StrImg.h"
#include "../SCommon/SClib.h"
#include "../SCommon/SIOStd.h"
#include "SymbolConstant.h"

namespace smm
{
	// String 的镜像
	// m_Type 为 TheEndPos 代表类型为独立的 StrImg，该构造函数由String对象调用
	StrImg::StrImg(Uint Type, char* pStr, Uint Size) : m_Type(Type), m_pStr(pStr), m_Size(Size)
	{	}

	// StrImg ( )
	StrImg::StrImg( ):m_Type(TheEndPos)
	{	SetImg(SymbolConstant::imEmpty);	}

	// StrImg (const StrImg& rStr)
	StrImg::StrImg(const StrImg& rStr) : m_Type(TheEndPos) , m_pStr(const_cast<char*>(rStr.ToCStr())) , m_Size(rStr.Size())
	{	}

	// StrImg (const char* pStr,Uint Size)
	StrImg::StrImg(const char* pStr,Uint StrSize) : m_Type(TheEndPos)
	{	SetImg(pStr,StrSize);	}

	// ~StrImg ( )
	StrImg::~StrImg( )
	{	}

	StrImg& StrImg::operator = (const StrImg& imStr)
	{
		SetImg(imStr.ToCStr(),imStr.Size());
		return *this;
	}

	void StrImg::SetImg(const char* pStr,Uint StrSize)
	{
		if(TheEndPos != m_Type) // 只有当前对象是一个独立的对象，而非一个String的引用时，才能赋值
		{	throw "StrImg::Set(const char* pStr,Uint StrSize) Error! You should not modify an exist string object's image!";	}

		if(pStr)
		{
			m_pStr = const_cast<char*>(pStr);
			m_Size = TheMaxCount==StrSize? SClib::StrLen(pStr) : StrSize;
		}
		else
		{
			m_pStr = NULL;
			m_Size = 0;
		}
	}

	void StrImg::SetImg(const StrImg& imStr)
	{	SetImg(imStr.ToCStr(),imStr.Size());	}

	StrImg StrImg::Sub(const StrImg& start, const StrImg& end, Uint pos) const
	{
		StrImg tmp;
		Uint sPos = Find(start,pos);
		if(TheEndPos == sPos)
		{	return SymbolConstant::imEmpty;	}
		else
		{	sPos += start.Size();	}
		Uint tPos = Find(end,sPos);
		if(TheEndPos == tPos)
		{	return SymbolConstant::imEmpty;	}
		if(tPos == sPos)
		{	return SymbolConstant::imEmpty;	}
		return Sub(sPos, tPos-sPos);
	}

	StrImg StrImg::Sub(Uint sPos, Uint nCount) const
	{
		if(sPos < m_Size)
		{
			if(nCount >= m_Size)
			{	nCount = m_Size;	}
			if(sPos+nCount >= m_Size)
			{	nCount = m_Size - sPos;	}
			return StrImg(m_pStr+sPos, nCount);
		}
		return StrImg(NULL,0);
	}

	void StrImg::Clear()
	{	SetImg(NULL,0);	}

	inline const char* StrImg::ToCStr  (  ) const
	{	return m_pStr;	}

	char  StrImg::operator []  (Uint pos) const
	{
		char c = 0;
		if(m_pStr && pos < m_Size)
		{	c = m_pStr[pos];	}
		else
		{	throw "StrImg::operator[](Uint pos) Error! The pos is too large or m_pStr is NULL!";	}
		return c;
	}

	char StrImg::GetAt (Uint pos) const
	{	return this->operator[](pos);	}

#ifdef SMDEBUG
	void StrImg::Show( ) const
	{
		if(!m_pStr)
		{
			cout<<"NULL"<<endl;
			return;
		}
		Uint size = Size();
		for(Uint i=0;i<size;++i)
		{	cout<<m_pStr[i];	}
		cout<<endl;
	}
#endif

	inline Uint StrImg::Size ( ) const
	{	return m_Size;	}

	bool StrImg::IsNull( ) const
	{	return NULL==m_pStr;	}

	bool StrImg::IsEmpty( ) const
	{	return !( m_pStr && 0!=m_Size);	}

	bool StrImg::operator < (const StrImg& imStr) const
	{	return Compare(imStr,false) < 0;	}

	bool StrImg::operator > (const StrImg& imStr) const
	{	return Compare(imStr,false) > 0;	}

	bool StrImg::operator == (const StrImg& imStr) const
	{	return IsEqual(imStr,false);	}

	int StrImg::Compare(const StrImg& imStr, bool IsCase) const
	{
		const char* pSrc = m_pStr;
		const Uint SrcSize= m_Size;
		const char* pArr = imStr.ToCStr();
		const Uint ArrSize= imStr.Size();

		if(!pArr || !pSrc)
		{	throw "StrImg::Compare(const StrImg& imStr, bool IsCase) Error! The Pointer parameter should not be NULL!";	}

		char LC=0,RC=0;
		if(IsCase) // 区分大小写
		{
			for ( Uint i=0;i<ArrSize && i<SrcSize;++i )
			{
				LC = pSrc[i];	RC = pArr[i];
				if(LC != RC)
				{	break;	}
			}
		}
		else // NoCase ----------------------------------
		{
			for ( Uint i=0;i<ArrSize && i<SrcSize;++i )
			{
				LC = pSrc[i];	RC = pArr[i];
				if(LC != RC) // 相等则继续,不等则换算
				{
					// 若为大写则换算为小写
					(LC>=65 && LC<=90) ? LC+=32 : 0;	(RC>=65 && RC<=90) ? RC+=32 : 0;
				//	LC<RC ? LC+=32 : RC+=32; // 可用在 Find 或 IsEqual 中。
				//	这里不能如上这样处理，如 'A' 和 'B'，LC<RC，LC+=32 变为 97,97>66，导致结果错误
					if(LC != RC)
					{	break;	}
				}
			} // for
		}

		if(LC == RC)
		{
			if(SrcSize != ArrSize)
			{	return  SrcSize<ArrSize ? -1 : 1;	}
			else
			{	return 0;	}
		}
		return LC - RC;
	}

	bool StrImg::IsEqual( const StrImg& imStr, bool IsCase) const
	{
		if(imStr.IsEmpty() || !m_pStr)
		{	return false;	}
		else
		{	
			if(imStr.Size() != m_Size)
			{	return false;	}
			else
			{	return 0 == Compare(imStr , IsCase);	}
		}
	}

	Uint StrImg::Find(const StrImg& findMe, const StrImg& fromMe, const StrImg& untilMe, Uint sPos, bool isCase) const
	{
		Uint fromPos = 0;
		if(!findMe.IsEmpty())
		{
			fromPos = Find(fromMe, sPos, TheMaxCount, isCase);
			if(TheEndPos == fromPos)
			{	fromPos = 0;	}
		}

		Uint untilPos = TheEndPos;
		if(!untilMe.IsEmpty())
		{
			untilPos = Find(untilMe, fromPos, TheMaxCount, isCase);
			if(TheEndPos == untilPos)
			{	untilPos = TheEndPos;	}
		}

		fromPos += fromMe.Size();
		if(fromPos >= untilPos)
		{	return TheEndPos;	}
		return Find(findMe, fromPos, untilPos, isCase);
	}

	Uint StrImg::Find(const StrImg& findMe, Uint sPos, Uint nCount, bool isCase) const
	{
		if(findMe.IsEmpty() || IsEmpty())
		{	return TheEndPos;	}
		Uint SearchSize = findMe.Size();
		if(m_Size<SearchSize || 0==m_Size || 0==SearchSize || sPos>=m_Size)
		{	return TheEndPos;	}

		if(nCount == TheEndPos)
		{	nCount = m_Size;	}
		sPos = SClib::Min<Uint>(sPos , m_Size-SearchSize);
		Uint tPos = sPos + nCount + SearchSize;
		if(tPos > m_Size)
		{	tPos = m_Size;	}

		const char* pSrc  = m_pStr+sPos;
		const Uint SrcSize = tPos - sPos;
		const char* pSearch = findMe.ToCStr();
		Uint pos = TheEndPos;
		if(nCount<32&&SearchSize<5|| SearchSize<3) // 短字符串用普通搜索
		{	pos = SClib::PlainFind(pSrc,SrcSize,pSearch,SearchSize,true,isCase);		}
		else
		{	pos = SClib::SundayFind(pSrc,SrcSize,pSearch,SearchSize,true,isCase);	}
		return TheEndPos==pos ? TheEndPos : sPos+pos;
	}

	Uint StrImg::RFind( const StrImg& findMe, Uint sPos, Uint nCount, bool IsCase) const
	{
		if(findMe.IsEmpty() || IsEmpty())
		{	return TheEndPos;	}
		Uint SearchSize = findMe.Size();
		if(m_Size<SearchSize || 0==m_Size || 0==SearchSize)
		{	return TheEndPos;	}

		if(sPos > m_Size)
		{	sPos = m_Size-1;	}
		sPos = SClib::Min<Uint>(m_Size-SearchSize , sPos);
		if(nCount > sPos+1)
		{	nCount = sPos+1;	}

		const Uint nOffSet = sPos-nCount+1;
		const char* pSrc  = m_pStr+nOffSet;
		const Uint SrcSize = nCount+SearchSize-1;	// -1 是因为 nCount 和 SearchSize 都包含了同一个位置(TmpPos)
		const char* pSearch = findMe.ToCStr();
		Uint pos = TheEndPos;
		if(nCount<32&&SearchSize<5|| SearchSize<3) // 短字符串用普通搜索
		{	pos = SClib::PlainFind(pSrc,SrcSize,pSearch,SearchSize,false,IsCase);		}
		else
		{	pos = SClib::SundayFind(pSrc,SrcSize,pSearch,SearchSize,false,IsCase);	}

		return TheEndPos==pos ? TheEndPos : nOffSet+pos;
	}

	StrImg& StrImg::Trim(char side)
	{
		if(' '!=side && 'L'!=side && 'R'!=side)
		{	throw "StrImg::Trim(char side) Error! Param should be 'L','R' or ''.";	}

		Uint sPos=0, tPos=m_Size;
		char c = 0;
		if('L'==side || ' '==side)
		{
			while(sPos < tPos)
			{
				c = m_pStr[sPos];
				if(!(' '==c || '\t'==c || '\n'==c || '\r'==c))
				{	break;	}
				++sPos;
			}
		}

		//  1, trim right
		if('R'==side || ' '==side)
		{
			while(tPos > sPos)
			{
				c = m_pStr[tPos-1];
				if(!(' '==c || '\t'==c || '\n'==c || '\r'==c))
				{	break;	}
				--tPos;
			}
		}

		if(sPos < tPos)
		{
			m_pStr += sPos;
			m_Size = tPos - sPos;
		}
		else
		{	m_Size = 0;	}
		return *this;
	}

	// ----------------------------------------------------------------------------------------------------------------

	// transform , declared in String.h
	template int  SMDLLX ToInt<int >(const StrImg&);
	template long SMDLLX ToInt<long>(const StrImg&);
	template Uint SMDLLX ToInt<Uint>(const StrImg&);
	template Int64 SMDLLX ToInt<Int64>(const StrImg&);
	template<typename NumT>
	NumT ToInt(const StrImg& imStr)
	{	return SClib::StrToInt<NumT>(imStr.ToCStr(),imStr.Size());	}

	double ToDouble(const StrImg& imStr)
	{	return SClib::StrToDouble(imStr.ToCStr(),imStr.Size());	}

	bool IsNumber(const StrImg& imStr)
	{	return SClib::IsNumber(imStr.ToCStr(),imStr.Size());	}

	Uint GetHash(const StrImg& imStr)
	{
		if(imStr.IsEmpty())
		{	return 0;	}
		const Uint ArrSize = 5;
		Uint HashArr[ArrSize] = { 31 , 131 , 1313 , 13131 , 131313 };

		const char* pStr = imStr.ToCStr();
		const Uint StrSize= imStr.Size();
		register Uint Ret = 1;  
		for(Uint i=0;i<StrSize;++i)
		{
			Ret += HashArr[i%ArrSize] * static_cast<Uint>(*pStr++);
		}
		return Ret;
	}
	// 有人说将乘法分解为位运算及加减法可以提高效率，
	// 如将上式表达为：hash = hash << 7 + hash << 1 + hash + ch;
	// 但其实在Intel平台上，CPU内部对二者的处理效率都是差不多的，
	// 我分别进行了100亿次的上述两种运算，发现二者时间差距基本为0
	// 如果是Debug版，分解成位运算后的耗时还要高1/3
	// 在ARM这类RISC系统上没有测试过，由于ARM内部使用Booth's Algorithm来模拟32位整数乘法运算，
	// 它的效率与乘数有关：
	// 当乘数8-31位都为1或0时，需要1个时钟周期
	// 当乘数16-31位都为1或0时，需要2个时钟周期
	// 当乘数24-31位都为1或0时，需要3个时钟周期  
	// 否则，需要4个时钟周期
	// 因此，虽然我没有实际测试，但是我依然认为二者效率上差别不大

	// --------------------------------------------------------------------------------------------------
	// StrImgCmpCase
	bool StrCmpCase::  operator ()(const StrImg& s, const StrImg& t) const
	{ return s.Compare(t,true)<0;	}
	bool StrCmpNoCase::operator ()(const StrImg& s, const StrImg& t) const
	{ return s.Compare(t,false)<0;	}
}; // namespace smm
