#ifndef HAPPIEME_BASICSTRIMAGE
#define HAPPIEME_BASICSTRIMAGE
#include "../SCommon/SCommon.h"
#ifdef SMDEBUG
#include <iostream>
using std::ostream;
#endif
namespace smm
{
	class SMDLLX StrImg
	{
	protected:
		Uint m_Type; // 类型有3种：(1)String 的基类部分,(2)独立的 StrImg,(3)SArray 的基类部分
		char* m_pStr;
		Uint m_Size;
		StrImg(Uint Type, char* pStr, Uint Size);
	public:
		// 由用户保证 pStr 不是临时对象且在 StringImage 对象生存期间不能销毁
		StrImg();
		StrImg(const StrImg&);
		StrImg(const char* pStr,Uint StrSize=TheMaxCount);
		~StrImg( );
		StrImg& operator = (const StrImg& imStr);
		void SetImg(const char* pStr,Uint StrSize=TheMaxCount);
		void SetImg(const StrImg&);
		StrImg Sub(Uint sPos, Uint nCount=TheMaxCount) const;
		StrImg Sub(const StrImg& start, const StrImg& end, Uint sPos=0) const;
		void Clear();
		bool operator < (const StrImg& imStr) const; // 使用无符比较
		bool operator > (const StrImg& imStr) const; // 使用无符比较
		bool operator == (const StrImg& imStr) const; // 使用无符比较
		const char* ToCStr  ( ) const;
			  char  GetAt  ( Uint pos ) const;
		char  operator []  ( Uint pos ) const; // throw，推荐使用
		Uint Size	( ) const;
		bool IsEmpty( ) const;
		bool IsNull ( ) const;
		int	 Compare( const StrImg& imStr , bool IsCase=false ) const;
		bool IsEqual( const StrImg& imStr , bool IsCase=false ) const;
		// 默认全范围查找，不区分大小写
		Uint Find	( const StrImg& findMe, Uint sPos=0			, Uint nCount=TheMaxCount, bool isCase=true) const;
		// fromMe 和 untilMe 都必须存在，否则就算找不到，为空时表示开始或末尾
		Uint Find	( const StrImg& findMe, const StrImg& fromMe, const StrImg& untilMe	 , Uint sPos=0, bool isCase=true) const;
  		Uint RFind	( const StrImg& findMe, Uint sPos=TheEndPos	, Uint nCount=TheMaxCount, bool isCase=true) const;
		StrImg& Trim( char side=' '); // L-left,R-right,默认是空格，表示两边
#ifdef SMDEBUG
		void Show() const;
#endif
	};

	template<typename NumT> NumT SMDLLX	ToInt ( const StrImg& );
	double	SMDLLX	ToDouble ( const StrImg& );
	bool	SMDLLX	IsNumber ( const StrImg& );
	Uint	SMDLLX	GetHash	 ( const StrImg& );

	class SMDLLX StrCmpCase
	{	public: bool operator()(const StrImg&,const StrImg&) const;	};
	class SMDLLX StrCmpNoCase
	{	public: bool operator()(const StrImg&,const StrImg&) const;	};

#define	SCSTRIMG(ImgVar,pStr) static const StrImg ImgVar(pStr,TheMaxCount)
}; // namespace 

# endif
