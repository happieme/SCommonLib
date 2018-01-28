#ifndef HAPPIEME_BASICSTRING
#define HAPPIEME_BASICSTRING
#include "../SCommon/SIOStd.h"
#include "StrImg.h"
#include "SymbolConstant.h"

namespace smm
{
	class SMDLLX String : public StrImg
	{
	public:
		String	( );
		String  ( const StrImg& imStr);
		String  ( const String& str);
		String  ( char Arr[], Uint Capacity, const StrImg& imStr);
		~String ( );
		// size
		Uint Capacity	( ) const;
		// 当 NewSize 所需空间比当前空间小 且 bShrink 为 true 则 Capacity 变小,否则将只改变 Size，若所需空间变大,该选项不起作用
		void ReSize		( Uint NewSize , bool bShrink=false); // 设置为 false 时，若NewSize较小，则是设置 Size 为指定大小
		void SetEndPos	( Uint NewSize); // 设置插入点位置
		// value
		void SetAt		( Uint pos , char c);
		char& operator  []  ( Uint pos ) ;
		// modify
		void Clear		( bool bShrink=false);
		void Delete		( Uint sPos , Uint nCount=1);			// 删除 sPos 后 nCount 个字符,不收缩空间
		void Retain		( Uint sPos , Uint nCount=TheMaxCount); // 保留 sPos 后 nCount 个字符
		String& Append	( const StrImg& imStr );
	//	String& Append	( Uint n);
	//	String& Append	(  int n);
		String& AppendC ( char c , Uint nCount=1);
		void Insert	( Uint sPos , const StrImg& imStr );
		// Replace with newStr by searching the oldStr from sPos
		void Replace	( const StrImg& oldStr , const StrImg& newStr , Uint sPos=0 , Uint nCount=TheMaxCount , bool IsCase=true);
		// Replace with newStr at sPos
		void Replace	( const StrImg& newStr , Uint sPos , Uint oldCount);
		// format
		void MakeUpper	( );
		void MakeLower	( );
		void MakeReverse( );
		// Format 会清除原有内容
		int  FormatStr	( StrImg imFmt, ...); // 似 sprintf ，"name:%s,age:%d"
		int  FormatImg	( StrImg imFmt, ...); // 串的参数类型必须为“StrImg”，尚未实现
		// 似C#的用法,如：("name:%0,age:%1,major:%2","John","15","Math"),参数个数限制为 32，Format 不清除原有内容，将在其后继续添加
		int  FormatArr	( const StrImg& imFmt, StrImg ValArr[],Uint ArrSize);
		// trim
		String& Trim  (int TrimType=0); // 0-both; -1-left; 1-right;
		// take over 接管另一个指针（必须是 new 得到），接管后，pNew已置为空！！！
		void TakeOver(char*& pNew , Uint NewSize , Uint NewCapacity=0);
		// operator
		String& operator = ( const char* pStr);
		String& operator = ( const StrImg& imStr);
		String& operator = ( const String& Str);
		String& operator +=( const StrImg& imStr);
	//	String& operator +=( Uint n);
	//	String& operator +=(  int n);
		bool operator < (const String& str) const; // 使用无符比较
		void Swap(String& Str);
	private:
		Uint m_SrcR;
	};	// class String

	int	 SMDLLX GetRandomNumber(int a , int b , Uint seed=0); // 得到从 a 到 b 的随机数
	void SMDLLX GetRandomString(String& strDst ,Uint nCount , Uint seed=0 , char sC='a' , char tC='z');

	String SMDLLX operator + (const StrImg& strL , const StrImg& strR);
	String SMDLLX operator + (const char* pStrL	 , const StrImg& strR);
	String SMDLLX operator + (const StrImg& pStrL, const char* pStrR);
//	String SMDLLX operator + (const StrImg& pStrL, Uint n);
//	String SMDLLX operator + (const StrImg& pStrL,  int n);

#define SplitterCount 512
	class SMDLLX StrSplit
	{
	private:
		StrImg m_imSrc;
		Uint m_Count;
		Uint m_DelimSize;
		Word m_PosArr[SplitterCount]; // 最多记录 SplitterCount 个 Pos，Pos 本身最大值为 65535
	public:
		StrSplit(const StrImg& imSrc, const StrImg& imSplitter=",");
		StrImg GetAt(Uint Index); // Index 是一个序号，从0开始
		Uint GetCount();
	};

#define	SCSTRING(StrVar,pStr) static const String StrVar(pStr);
#define SARRAYSTR(VarName, Capacity, imStr) char _Happieme_SArray_##VarName[Capacity]; String VarName(_Happieme_SArray_##VarName, Capacity, imStr);
#define SARRAY(VarName, Capacity)	SARRAYSTR(VarName, Capacity, "");
}; // namespace smm

#endif
