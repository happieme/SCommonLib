#ifndef HAPPIEME_SSYSTEM
#define HAPPIEME_SSYSTEM
#include "../SString/String.h"

namespace smm
{	// 包装系统函数
	namespace SSystem
	{
		typedef void* HANDLE;
	//	int SMDLLX ShellExec();
		void SMDLLX SWait(Uint mSec);
		void SMDLLX SCloseHandle(void* hHandle);
#ifdef WIN32
		Uint SMDLLX SGetLastError();
		Uint SMDLLX SShellExecute ( const StrImg& pStrFilePath	,
									const StrImg& pStrParameters="",
									const StrImg& pStrOperation="Open",
									const StrImg& pStrDir = 0	,
									int	nShowCmd	= 0, // 表示隐藏窗口
									HANDLE hwnd		= 0
									);
		// event - windows
		HANDLE SMDLLX SCreateEvent(bool bManualReset,bool bInitState);
		void SMDLLX SWaitForEvent(void* hHandle,Uint mSec=TheMaxCount);
		bool SMDLLX SSetEvent(void* hHandle);
		bool SMDLLX SResetEvent(void* hHandle);
// semaphore - linux
#endif
		bool IsCpuBigEndian();
		// for dll
		HANDLE SMDLLX SLoadDLL(const StrImg& FilePath);
		HANDLE SMDLLX SGetDLLFunc(HANDLE hModule , const StrImg& ProcName);
		String SMDLLX SGetWorkDir( );
		bool SMDLLX SCheckAppInstance(const StrImg& strMutex);
	}; // SSystem ----------------------------------------------------------------------------------

	class SMDLLX SCmd
	{
	private:
		String m_Result;
	public:
		StrImg GetResult( );
		void Run(const StrImg& Cmd);
	};

	class SMDLLX SLock
	{
	private:
		void* m_pLR;
	public:
		SLock(bool autoLock=false);
		~SLock();
		void Lock();
		void UnLock();
	};

	class SMDLLX SThread
	{
	public:
#ifdef WIN32
		typedef unsigned __stdcall pThreadfun(void *);
#else
		typedef void* pThreadfun(void *);
#endif
		static int CreateThrd(pThreadfun pFun,void * param);
	};
}; // namespace

#endif
