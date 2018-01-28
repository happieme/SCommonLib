#include "SLog.h"
#include "SFile.h"
#include "SConfig.h"
#include "../SSystem/SSystem.h"
#include "../STime/STime.h"
#include "../SString/SymbolConstant.h"
#include <vector>
#include <map>
#include <utility>

namespace smm
{
	using std::map;
	using std::make_pair;
	using std::vector;

	// ------------------------------------------------------------------------------------------------------------------------------
	// SLogInner	-----------------------------------------------------------------------------------------------------------------------------------
	struct tagLogUnit
	{
		bool bFree;
		String content;
	};
	
	const Uint logBufSize = 1024*8; // log 数据缓存，目前只提供 8 K
	class SLogInner
	{
		friend class SLog;
		SLock m_Inner_Lock;
		String m_Inner_LogDir;
		STime  m_Inner_Time;
		String m_logFilePath;
		String m_output,m_input; // Input 表示输入到log对象里，output 表示log输出到文件
		bool   m_Inner_bOutput;	 // 表示log内容是否已经输出到文件
	private:
		SLogInner()
		{	Init();	}

		void Init()
		{
			m_output.ReSize(logBufSize); // 均初始化为 8 k
			m_input .ReSize(logBufSize);
			m_Inner_bOutput = false; // 初始时，没有输出到文件
			SConfig* pCfg = SConfig::GetInstance("Config.ini");
			m_Inner_LogDir = "./Log/";
			if(pCfg)
			{
				StrImg dir = pCfg->Get("server","logdir");
				if(!dir.IsEmpty())
				{
					m_Inner_LogDir = dir;	
					char c = m_Inner_LogDir.GetAt(m_Inner_LogDir.Size()-1);
					if('/'!=c || '\\'!=c)
					{	m_Inner_LogDir.Append("/");	}
				}
			}

			SDEBUG(m_Inner_LogDir,false);
			
			if(!SFileShell::Exist(m_Inner_LogDir))
			{
				if(!SFileShell::MakeDir(m_Inner_LogDir))
				{	throw "SLog.cpp Error! Create log dir failed! You can do it yourself!";	}
			}
		}

		//初始时，
		void Add(const StrImg& LogType, const StrImg& LogStr)
		{
			m_Inner_Lock.Lock();
			{
				if(LogType.IsEmpty())
				{
					m_input.Swap(m_output);
					m_input.Clear();
				}
				else
				{
					m_Inner_Time.ToNow();
					m_input.Append(m_Inner_Time.ToStr());
					m_input.Append("\t");
					m_input.Append(LogType);
					StrImg tmp(LogStr);
					if(m_input.Size()+LogStr.Size()+64 > logBufSize)
					{
						tmp = LogStr.Sub(0,logBufSize-m_input.Size()-64);
					}
					m_input.Append(tmp);
					m_input.Append("\r\n");
					m_Inner_bOutput = false;
				}
			}
			m_Inner_Lock.UnLock();
		}

		void Write()
		{
			if(!m_Inner_bOutput)
			{
				Add("", "");
				if(!m_output.IsEmpty())
				{
					m_Inner_Time.ToNow();
					m_logFilePath = m_Inner_LogDir;
					m_logFilePath.Append(m_Inner_Time.ToStr(TFmt_YearMonth)+".log"); // ../Debug/Log/2016-11.log
					SDEBUG(m_logFilePath,false);
					STextFile sf(m_logFilePath);
					Uint OpenStatus = sf.Open("ab+");
					if(STextFile::OpenFailed != OpenStatus)
					{
						if(STextFile::OpenFailEncoded == OpenStatus)
						{	sf.Write(Utf8_Bom_Mark);	}
						sf.Write(m_output);
					}
					m_output.Clear(); // 为防止log不断增长导致内存错误，即使写文件有问题也要清除log缓存
					m_Inner_bOutput = true;
				}
			}
		}
	};

	// -------------------------------------------------------------------------------------------------------------------------------------------

#define pLogInner	static_cast<SLogInner*>(m_pLogR)

	SLog::SLog()
	{
		m_pLogR = new SLogInner;
	}

	SLog* SLog::GetInstance()
	{
		static SLog* p = NULL;
		if(!p)
		{
			SLock locker(true);
			if(!p)
			{
				p = new SLog;
			}
		}
		return p;
	}

	void SLog::Flush()
	{	pLogInner->Write();	}

	void Release()
	{	delete SLog::GetInstance();	}

#ifdef SMDEBUG
	void SLog::Debug(const StrImg& LogStr)
	{	pLogInner->Add("[Debug] ", LogStr);	}
#endif

	void SLog::Log(const StrImg& LogType, const StrImg& LogStr)
	{	pLogInner->Add(LogType, LogStr);	}

	void SLog::Error(const StrImg& LogStr)
	{	pLogInner->Add("[Error] ", LogStr);	}

	void SLog::Info(const StrImg& LogStr)
	{	pLogInner->Add("[Info] ", LogStr);		}
	
	void SLog::Warn(const StrImg& LogStr)
	{	pLogInner->Add("[Warn] ", LogStr);		}

}; // namespace
