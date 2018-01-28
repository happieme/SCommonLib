#ifndef HAPPIEME_SLOG
#define HAPPIEME_SLOG
#include "../SString/String.h"

namespace smm
{
	class SMDLLX SLog:virtual private SNonCopy
	{
	private:
		void * m_pLogR;
		SLog();
	public:
		static SLog* GetInstance();
		static void		Release();
		void Flush();
#ifdef SMDEBUG
		void Debug(const StrImg& LogStr);
#endif
		void Error(const StrImg& LogStr);
		void Info(const StrImg& LogStr);
		void Warn(const StrImg& LogStr);
		void Log(const StrImg& LogType, const StrImg& LogStr);
	};
};
#endif
