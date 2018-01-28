#include "STime.h"
#include "../SCommon/SClib.h"
#include <locale.h>

namespace smm
{
	class SMTimeSpan;

	enum Enum_Time_Index 
	{
		STYEAR = 0	,
		STMON		,
		STDAY		,
		STHOUR		,
		STMIN		,
		STSEC
	};

	struct tagMonthPair
	{
		const char* pStrName;
		const char* pStrNum;
	};
	tagMonthPair MonthNameArr[] = 
	{
		{	"Jan" , "01"	} ,
		{	"Feb" , "02"	} ,
		{	"Mar" , "03"	} ,
		{	"Apr" , "04"	} ,
		{	"May" , "05"	} ,
		{	"Jun" , "06"	} ,
		{	"Jul" , "07"	} ,
		{	"Aug" , "08"	} ,
		{	"Sep" , "09"	} ,
		{	"Oct" , "10"	} ,
		{	"Nov" , "11"	} ,
		{	"Dec" , "12"	} 
	};

	const char* WeekNameArr[ ] = 
	{	"Mon" , "Tue" , "Wed" , "Thu" , "Fri" , "Sat" , "Sun" };

	struct tagDateTimeValue
	{
		const char* pStrName;
		int value;
	};
	SCSTRIMG(imNormalFmt	, "%Y-%m-%d %H:%M:%S");
	SCSTRIMG(imSlashFmt		, "%Y/%m/%d %H:%M:%S");
	SCSTRIMG(imDateOnlyFmt	, "%Y-%m-%d");
	SCSTRIMG(imTimeOnlyFmt	, "%H:%M:%S");
	SCSTRIMG(imHttpFmt		, "%a, %d %b %Y %H:%M:%S GMT");
	SCSTRIMG(imYearMonth	, "%Y-%m");
	class STimeInner
	{
		friend class STime;
	private:
		static	TimeT m_TimeZoneDiff; // 时差（以秒为单位）
		STATIC_INIT_DECLARE;
		struct  tm m_Inner_TM;
		TimeT	m_Inner_TimeStamp;
		mutable String m_Inner_TStr;
		TimeT	m_Inner_LastTick;
	private:
		void UpdateTM_I(bool bLocal) // true 表示 local 时间
		{
			if(bLocal)
			{	SClib::SetTM_Local	( m_Inner_TM,m_Inner_TimeStamp );	}
			else
			{	SClib::SetTM_Gmt	( m_Inner_TM,m_Inner_TimeStamp );	}
		}

		void SetHttpTime_I(const StrImg& StrHttpTime)
		{//	Thu, 23 Mar 2006 07:54:53 GMT
			if(29 != StrHttpTime.Size( )) // fixed length
			{	throw "STime::SetHttpTime(const StrImg& StrHttpTime)；HTTP is time string，it's fixed form，length is 29，like“Thu, 23 Mar 2006 07:54:53 GMT”";	}
			// 长度若正确，则不再做特别检查，第8位开始的3个字符为月份
			const char* pBase = StrHttpTime.ToCStr();
			StrImg imStr(pBase+8,3);
			Uint nMon = 0; // 记录月份
			const Uint MonthArrSize = sizeof(MonthNameArr)/sizeof(MonthNameArr[0]);
			while(nMon < MonthArrSize)
			{
				if(imStr.IsEqual(StrImg(MonthNameArr[nMon].pStrName,3))) // 长度固定为 3
				{	break;	}
				++nMon;
			}
			if(MonthArrSize == nMon)
			{	return;	}

			const Uint ArrSize = 24;
			char TArr[ArrSize] = {0}; // “23 Mar 2006”，将"Mar"，替换为" 03"
			SClib::MemCpy(TArr,ArrSize,pBase+5,20);
			TArr[3] = ' ';
			TArr[4] = MonthNameArr[nMon].pStrNum[0];
			TArr[5]= MonthNameArr[nMon].pStrNum[1];
			StrImg imTFmt("dd  mm yyyy hh:mi:ss",20);
			StrImg imTSrc(TArr,20);
			SetTime_I(imTFmt,imTSrc,false);
		}

		void SetTime_I(const StrImg& StrFmt , const StrImg& StrSrc , bool bLocal)
		{
			Uint FmtLen = StrFmt.Size( );
			Uint SrcLen = StrSrc.Size( );
			const Uint LimitSize = 128;
			if(SrcLen!=FmtLen || SrcLen>LimitSize)
			{	throw "STime::SetTime().The two parameter should have ths same length which should not be greater than 128";	}

			const Uint DTArrSize = 6;
			tagDateTimeValue DTArr[DTArrSize] = 
			{
				{ "yyyy", -1 } , { "mm"	, -1 } , { "dd"	, -1 } ,
				{ "hh"	, -1 } , { "mi"	, -1 } , { "ss"	, -1 }
			};

			{	// 填充 DTArr
				Uint pos = 0;
				for(Uint i=0;i<DTArrSize;++i)
				{
					// 从 StrFmt 中寻找 yyyy 这样的标识字段
					pos = StrFmt.Find(DTArr[i].pStrName);
					if(TheEndPos != pos)
					{	// 将 StrSrc 中的对应字段转换为数字
						StrImg imNum(StrSrc.ToCStr()+pos,0==i?4:2);
						if(!imNum.IsEmpty() && IsNumber(imNum))
						{	DTArr[i].value = ToInt<int>(imNum);	}
						else
						{	throw "STime::SetTime(const StrImg& StrFmt , const StrImg& StrSrc , bool bLocal) Error! Parameter error , you should use number to express the time!";	}
					}
				}
			}
			// 至此 DTArr 已填充完毕，下面检查各个字段是否符合要求
			if(!(
					( DTArr[STYEAR].value>=1970) && 
					( 0<=DTArr[STMON].value	&& DTArr[STMON].value<=11 )	&& 
					( 1<=DTArr[STDAY].value	&& DTArr[STDAY].value<=31 )	&& 
					( 0<=DTArr[STHOUR].value&& DTArr[STHOUR].value<=23 )&& 
					( 0<=DTArr[STMIN].value	&& DTArr[STMIN].value<=59 )	&& 
					( 0<=DTArr[STSEC].value	&& DTArr[STSEC].value<=59))
				)
			{	throw "STime::SetTime(const StrImg& StrFmt , const StrImg& StrSrc , bool bLocal) Error! Time number should be in a valid scope!";	}

			tm theTM;
			theTM.tm_year	= DTArr[0].value-1900;
			theTM.tm_mon	= DTArr[1].value-1;
			theTM.tm_mday	= DTArr[2].value;
			theTM.tm_hour	= DTArr[3].value;
			theTM.tm_min	= DTArr[4].value;
			theTM.tm_sec	= DTArr[5].value;
			TimeT LocalT = SClib::TM_2_TStamp(theTM);
			// 这里的 LocalT 实际上是传入的 Gmt 时间，再加一个时差才是真正的 LocalT
			// 如传入 15:02:02，则 LocalT 应为 23:02:02
			if(!bLocal)
			{	// 要转换为真正的 LocalT
				LocalT += m_TimeZoneDiff;
				SClib::SetTM_Local(theTM,LocalT); // 再转换为 tm
			}
			m_Inner_TM = theTM;
			m_Inner_TimeStamp = LocalT;
		}

		StrImg Format_I(const StrImg& imFmt,tm& theTM) const
		{
			const Uint LimitSize = 1024*4;
			if(imFmt.Size( ) > LimitSize)
			{	throw "STime::Format() Error! String should not be longer than 1024*4";	}
			// setlocale( )影响输出结果是英文还是中文
			// 需要把时间转换为 GMT 时间
		//	const char* pLocale = ::setlocale(LC_ALL,NULL);
		//	setlocale(LC_ALL,"");
			char dst[LimitSize] = {0};
			Uint RetLen = SClib::FormatTStr(dst,LimitSize,imFmt.ToCStr(),theTM);
		//	setlocale(LC_ALL,pLocale);
			m_Inner_TStr.Clear();
			if(0 != RetLen)
			{	m_Inner_TStr.Append(StrImg(dst,RetLen));	}
			return m_Inner_TStr;
		}

		StrImg GetHttpTimeStr_I( ) const
		{	// Mon, 06 Mar 2006 12:58:55 GMT
			tm theTM;
			SClib::SetTM_Gmt(theTM,m_Inner_TimeStamp);
			return Format_I(imHttpFmt , theTM);
		}
	};

	TimeT STimeInner::m_TimeZoneDiff = 0; // 时差（以秒为单位）
	STATIC_INIT_IMPL(STimeInner)
	{
		TimeT GmtT;
		tm theTM;
		TimeT LocalT = 24*3600;
		SClib::SetTM_Gmt(theTM,LocalT); // 时差最多不超过 24 小时
		GmtT = SClib::TM_2_TStamp(theTM);
		m_TimeZoneDiff = LocalT-GmtT;
		return true;
	}

#define pInner		static_cast<STimeInner*>(m_pTimeR)
#define m_TM		pInner->m_Inner_TM
#define m_TStr		pInner->m_Inner_TStr
#define m_TimeStamp	pInner->m_Inner_TimeStamp
#define m_LastTick	pInner->m_Inner_LastTick

	// STime		-----------------------------------------------------------------------------------

	STime::STime(TimeT t)
	{
		m_pTimeR = new STimeInner;
		SetTime(t);
	}

	STime::STime(const STime& st)
	{
		m_pTimeR = new STimeInner;
		SetTime(st.TimeStamp());
	}

	STime::~STime( )
	{	delete m_pTimeR;	}

	STime& STime::ToNow()
	{	
		m_TimeStamp = Now( );
		pInner->UpdateTM_I(true);
		return *this;
	}

	TimeT STime::Now( )
	{	return SClib::GetCurrentTimeStamp( );	}

	void STime::SetTime(TimeT t)
	{
		if(t <= 0)
		{	t = SClib::GetCurrentTimeStamp( );	}
		m_TimeStamp = t;
		pInner->UpdateTM_I(true);
	}

	void STime::SetTime(const StrImg& imStr,TFmt_Type TType)
	{
		switch(TType)
		{
		case TFmt_Http:
			pInner->SetHttpTime_I(imStr);
			break;
		case TFmt_Slash:
			SetTime("yyyy/mm/dd hh:mi:ss",imStr);
			break;
		case TFmt_Normal:
			SetTime("yyyy-mm-dd hh:mi:ss",imStr);
			break;
		default:
			break;
		};
	}

	void STime::SetTime(const StrImg& StrFmt , const StrImg& StrSrc)
	{	pInner->SetTime_I(StrFmt,StrSrc,true);	}

	int STime::Year( ) const
	{	return m_TM.tm_year+1900;	}

	int STime::Month( ) const
	{	return m_TM.tm_mon+1;	}

	int STime::Day( ) const
	{	return m_TM.tm_mday;	}

	int STime::Week( ) const
	{	return m_TM.tm_wday;	}

	int STime::YearDay( ) const
	{	return m_TM.tm_yday;	}

	int STime::Hour( ) const
	{	return m_TM.tm_hour;	}

	int STime::Minute( ) const
	{	return m_TM.tm_min;	}

	int STime::Second( ) const
	{	return m_TM.tm_sec;	}

	TimeT STime::TimeStamp( ) const
	{	return m_TimeStamp;	}

	StrImg STime::Format(const StrImg& imFmt) const
	{	return pInner->Format_I(imFmt,m_TM);	}

	StrImg STime::WeekName(bool abbr) const
	{
		if(abbr)
		{	return Format("%a");	}
		else
		{	return Format("%A");	}
	}

	StrImg STime::MonthName(bool abbr) const
	{
		if(abbr)
		{	return Format("%b");	}
		else
		{	return Format("%B");	}
	}

	StrImg STime::ToStr(TFmt_Type TType) const
	{
		switch(TType)
		{
		case TFmt_Normal:
			Format(imNormalFmt);
			break;
		case TFmt_Slash:
			Format(imSlashFmt);
			break;
		case TFmt_Date:
			Format(imDateOnlyFmt);
			break;
		case TFmt_Time:
			Format(imTimeOnlyFmt);
			break;
		case TFmt_Http:
			pInner->GetHttpTimeStr_I( );
			break;
		case TFmt_YearMonth:
			Format(imYearMonth);
			break;
		default:
			break;
		};
		return m_TStr;
	}

	TimeT STime::TickStart()
	{	return m_LastTick = SClib::TickCount();	}

	TimeT STime::TickStop()
	{	return SClib::TickCount() - m_LastTick;	}

	STime& STime::operator = (const STime& st)
	{
		SetTime(st.TimeStamp());
		return *this;
	}

	bool STime::operator ==(const STime& ht ) const
	{	return m_TimeStamp == ht.TimeStamp();	}

	bool STime::operator !=(const STime& ht ) const
	{	return m_TimeStamp != ht.TimeStamp();	}

	bool STime::operator <	(const STime& ht ) const
	{	return m_TimeStamp < ht.TimeStamp();	}

	bool STime::operator >	(const STime& ht ) const
	{	return m_TimeStamp > ht.TimeStamp();	}

	bool STime::operator >=(const STime& ht ) const
	{	return m_TimeStamp >= ht.TimeStamp();	}

	bool STime::operator <=(const STime& ht ) const
	{	return m_TimeStamp <= ht.TimeStamp();	}

	STime STime::operator -(const STime& ht ) const
	{	return STime(m_TimeStamp - ht.TimeStamp());	}

	// STime		---------------------------------------------------------------------------------------------------------------------------------------

}; // namespace
