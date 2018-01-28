#include "SSystem.h"
#include "../SCommon/SIOStd.h"
#ifdef WIN32
	#include <windows.h>
	#include <direct.h>	 // _getcwd
	#include <process.h> // _beginthreadex
#else
	#include <stdio.h>	// popen,FILE
	#include <unistd.h> // usleep
	#include <semaphore.h> 
	#include <pthread.h>
	#include <dlfcn.h>  // dlopen
#endif

namespace smm
{
	namespace SSystem
	{
#ifdef WIN32
		HANDLE SCreateEvent(bool bManualReset,bool bInitState)
		{	return ::CreateEvent(NULL,bManualReset,bInitState,NULL);	}

		void SWaitForEvent(void* hEvent,Uint mSec)
		{	::WaitForSingleObject(hEvent,mSec);	}

		bool SSetEvent(void* hHandle)
		{	return 0!=::SetEvent(hHandle);	}

		bool SResetEvent(void* hHandle)
		{	return 0!=::ResetEvent(hHandle);	}

		void SCloseHandle(void* hHandle)
		{	::CloseHandle(hHandle);	}

		Uint SGetLastError()
		{	return ::GetLastError();	}

		Uint SShellExecute ( const StrImg& StrFPath	,
							 const StrImg& StrParam	,
							 const StrImg& Operation,
							 const StrImg& StrDir	,
							 int	nShowCmd		,
							 HANDLE hwnd			)
		{
			HINSTANCE res = ::ShellExecuteA( HWND(hwnd) , Operation.ToCStr(), StrFPath.ToCStr() , StrParam.ToCStr(), 
											  StrDir.ToCStr() , nShowCmd);
			return reinterpret_cast<Uint>(res);
		}
#endif

		bool IsCpuBigEndian( )
		{  
			union  
			{  
				unsigned int  a;  
				unsigned char b;  
			}c;  
			c.a = 1;  
			return 1 != c.b;
		}

		void SWait(Uint mSec)
		{
		//	::WaitForSingleObject(SCreateEvent(false,false),mSec);
#ifdef WIN32
			::Sleep(mSec); // 有时不太准确，原因未知
#else
			usleep(mSec*1000);
#endif
		}

		HANDLE SLoadDLL(const StrImg& DllPath)
		{
#ifdef WIN32
			return ::LoadLibraryA(DllPath.ToCStr());
#else
			return dlopen(FilePath.ToCStr(), RTLD_NOW);
#endif
		}

		HANDLE SGetDLLFunc(HANDLE hModule , const StrImg& ProcName)
		{
#ifdef WIN32
			return ::GetProcAddress(reinterpret_cast<HMODULE>(hModule), ProcName.ToCStr());
#else
			return dlsym(hModule, ProcName.ToCStr());
#endif
		}

		String SGetWorkDir( )
		{
#ifdef WIN32
			return String(_getcwd(NULL,0));
#else
			return String(getcwd(NULL,0));
#endif
		}

		// one instance
		bool SCheckAppInstance(const StrImg& strMutex)
		{
			void* p = ::CreateMutex(NULL, FALSE, (LPCWSTR)strMutex.ToCStr());  //创建互斥 L"created by happieme from 2013"
			if(!p)
			{	return true;	}
			else if(::GetLastError() == ERROR_ALREADY_EXISTS)
			{
				CloseHandle(p);
				return true;
			}
			return false;
		}

		void* FindProcess(const StrImg& strName)
		{
			/************************************************************************/
			// 采用直接枚举进程的方法查找指定名称进程
			/* 名    称: FindProcessByName( LPCTSTR szFileName )
			/* 功    能: 查询指定名称的进程是否有实例正在运行
			/* 参　　数: LPCTSTR szFileName, 可执行程序名称, 例如"c:\\notepad.exe"
			/* 返 回 值: BOOL.TRUE, 有同名实例正在运行; FALSE, 没有同名实例正在运行.
			/* 编 写 者: Xianren Li
			/* 创建日期: 2007-08-17
			/* 附加说明: 请注意进程的查找只是依据名称, 无法确保找到的进程调用的是预料
			/*             中的可执行文件.
			/* 修改记录:
			/************************************************************************/

		/*	SARRAY(name, strName);
			name.MakeLower();

			DWORD aProcesses[1024], cbNeeded, cProcesses;

			if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
			{	return TheEndPos;	}

			cProcesses = cbNeeded / sizeof(DWORD);

			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			HANDLE hProcess;
			CString strName;
			BOOL isRunning = FALSE;
			for (unsigned int i = 0; i < cProcesses; i++)
			{
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

				if (NULL != hProcess)
				{
					HMODULE hMod;
					DWORD cbNeeded;

					if (::EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
					{
						::GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
						strName = szProcessName;
						strName.MakeLower();
						if (-1 != strFileName.Find(strName))
						{
							isRunning = TRUE;
							CloseHandle(hProcess);
							break;
						}
					}
					CloseHandle(hProcess);
				}
			}
			return isRunning;
		*/
			return NULL;
		}

		bool KillProcess(Uint ProcessId)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,ProcessId);
			if(hProcess==NULL)
			{	return false;	}
			if(!TerminateProcess(hProcess,0))
			{	return false;	}
			return true;
		}


	}; // SSystem

	StrImg SCmd::GetResult( )
	{	return m_Result;	}

	void SCmd::Run(const StrImg& Cmd)
	{
#ifdef WIN32
		return;
#else
		FILE *pp = popen(Cmd.ToCStr(), "r");
		if(!pp)
		{	return;	}
		char Buf[1024*4] = {0};
		Uint LastPos = 0;
		m_Result.Clear();
		while(fgets(Buf, sizeof(Buf), pp))
		{
			m_Result.Append(Buf);
			LastPos = m_Result.Size()-1;
			if('\n' == m_Result[LastPos])
			{	m_Result[LastPos] = '\t';	}
		}
		m_Result.Replace(SymbolConstant::imLF, SymbolConstant::imSpace);
		pclose(pp);
#endif
	}

	// SMSectionLock ------------------------------------------------------------------------------------
	class SInnerCSLock
	{
		friend SLock;
	private:
#ifdef WIN32
		CRITICAL_SECTION m_Inner_CS;
		bool m_Inner_bLocked;
#else
		pthread_mutex_t  m_Inner_CS;
#endif
	};

#define pInner static_cast<SInnerCSLock*>(m_pLR)
#define m_CS (pInner->m_Inner_CS)
#define m_bLocked pInner->m_Inner_bLocked
	// 	SLock
	SLock::SLock(bool autoLock)
	{
		m_pLR = new SInnerCSLock;
#ifdef WIN32
		if (!InitializeCriticalSectionAndSpinCount(&m_CS, 0x80000400))
		{	throw "SLock::SLock() Error! InitializeCriticalSectionAndSpinCount() Failed!";	}
		if(autoLock)
		{	Lock();	}
#else
		pthread_mutex_init(&m_CS, NULL);
#endif
	}

	SLock::~SLock()
	{
#ifdef WIN32
		::DeleteCriticalSection(&m_CS);
#else
		pthread_mutex_destroy(&m_CS);
#endif
		delete m_pLR;
	}

	void SLock::Lock()
	{
#ifdef WIN32
			::EnterCriticalSection(&m_CS);
#else
			pthread_mutex_lock(&m_CS);
#endif
	}

	void SLock::UnLock()
	{
#ifdef WIN32
			::LeaveCriticalSection(&m_CS);
#else
			pthread_mutex_unlock(&m_CS);
#endif
	}

	// SThread ------------------------------------------------------------------------------------------
	int SThread::CreateThrd(pThreadfun pFun,void * param)
	{
#ifdef WIN32
		HANDLE hRes = (HANDLE)::_beginthreadex(NULL, 0, pFun, param, NULL, NULL);
		if(NULL == hRes)
		{	// exception
			return SSystem::SGetLastError();
		}
		::CloseHandle(hRes);
		return 0;
#else
		pthread_t ntid;
		return pthread_create (&ntid, NULL, pFun, param);
#endif
	}
}; // namespace
