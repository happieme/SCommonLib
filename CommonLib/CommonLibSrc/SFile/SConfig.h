#ifndef HAPPIEME_SCONFIG
#define HAPPIEME_SCONFIG
#include "../SString/String.h"

namespace smm
{
	// 文件大小限制 8M
	class SMDLLX SConfig: virtual private SNonDerive<SConfig>
	{
	private:
		void* m_pCfgR;
		SConfig(const StrImg& FilePath);
	public:
		static SConfig* GetInstance(const StrImg& FilePath);
		static void		Release(const StrImg& FilePath);
		StrImg Get(const StrImg& GroupName,const StrImg& ItemName);
	};

};// namespace
#endif
