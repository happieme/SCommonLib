#ifndef HAPPIEME_SCOMMON
#define HAPPIEME_SCOMMON
// 公共类
namespace smm
{
#ifdef WIN32
	#ifndef	SMDLLX
		#define SMDLLX __declspec(dllexport)
	#endif
	#define SMDEBUG	_DEBUG
#else
	#define SMDLLX  
	#define SMDEBUG	DEBUG
#endif

#ifdef WIN32
	typedef __int64 Int64;
	typedef __int64 TimeT;
#else
	typedef long long Int64;
	typedef long TimeT;
#endif
	typedef char    CharC;
	typedef wchar_t CharW;
	typedef unsigned long ULong;
	typedef unsigned int  Uint;
	typedef unsigned char Byte;
	typedef unsigned short Word;
#ifdef UNICODE
	typedef wchar_t SChar;
#else
	typedef char	SChar;
#endif

	class SMDLLX SNonCopy
	{
	private:
		SNonCopy(const SNonCopy&);
		SNonCopy&operator=(const SNonCopy&);
	public:
		SNonCopy(){};
	};

	template<typename T>
	class SMDLLX SNonDerive
	{
	private:
		friend T;
		SNonDerive(){};
	};

/*	// gcc 版本低时上面的做法无法通过编译，采取下面的办法替代
	template<typename T>
	class SNonDerive
	{
	public:
		typedef T me;
	};
	template<typename T>
	class SMDLLX Non_Derive
	{
	private:
		friend class SNonDerive<T>::me;
		Non_Derive(){};
	};
*/
	class SMDLLX SNonCopyDerive:public SNonDerive<SNonCopyDerive>
	{
	private:
		SNonCopyDerive(const SNonCopyDerive&);
		SNonCopyDerive&operator=(const SNonCopyDerive&);
	public:
		SNonCopyDerive(){};
	};

	static const Uint TheEndPos		= static_cast<Uint>(-1);
	static const Uint TheMaxCount	= TheEndPos;
	// 计算数组元素的个数
#define COUNTARR(Arr) sizeof(Arr)/sizeof(Arr[0])
	// 常用的静态初始化函数宏
#define STATIC_INIT_DECLARE		static bool m_Happieme_Static_InitVar;static bool m_Happieme_Static_InitFunc();
#define STATIC_INIT_IMPL(Class)	bool Class::m_Happieme_Static_InitVar=Class::m_Happieme_Static_InitFunc();bool Class::m_Happieme_Static_InitFunc()
/*#define SMax(Type) Type SMax(Type x,Type y) {	return x>y?x:y;	}
		SMax(int);SMax(Uint);SMax(char);SMax(long);SMax(float);SMax(unsigned long);SMax(double);SMax(unsigned char);
*/
};// namespace smm

#endif
