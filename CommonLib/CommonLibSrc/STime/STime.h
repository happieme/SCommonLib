#ifndef HAPPIEME_STIME
#define HAPPIEME_STIME
#include "../SString/String.h"

namespace smm
{
	enum TFmt_Type
	{
		TFmt_Http = 0 ,		// Wed, 17 Oct 2007 02:15:55 GMT , Http
		TFmt_Normal ,		// 2013-02-05 12:15:33			 , Sql ,常用
		TFmt_Slash	,		// 2013/02/05 12:15:33
		TFmt_Date	,		// 2013-02-05
		TFmt_Time	,		// 12:15:33
		TFmt_YearMonth		// 1998-11
	};

	class SMDLLX STime:virtual private SNonDerive<STime>
	{
	public:
		// t<0 时，返回当地的当前时间（只使用当地时间）
		STime(TimeT t=0);
		STime(const STime&);
		~STime( );
		static TimeT Now( );
		void SetTime(TimeT t);
		void SetTime(const StrImg& StrSrc, TFmt_Type TType); // 支持三种形式，见本页上方的 TType 的定义
		// 设置各种形式的时间，主要用于字符串中的时间提取。需要六个时间参数：年月日时分秒
		// 注意：此处的时间将被视为“当地时间”
		// 写法：("yyyy年 mm月 dd日 hh:mi:ss","2012年 12月 22日 15:45:35")，必须用小写
		// 时间格式要严格对应！所以两者长度必须相等！长度限制为 128，年必须为 4 位
		void SetTime(const StrImg& imStrFmt, const StrImg& imStrSrc);
		STime& ToNow(); // 将对象的时间更新至当前最新
		int Year	() const; // since 1900
		int Month	() const; // [0,11]
		int Day		() const; // [1,31]
		int Hour	() const; // [0,23]
		int Minute	() const; // [0,59]
		int Second	() const; // [0,59]
		int Week	() const; // [0,6]
		int YearDay	() const; // [0,365]
		TimeT TimeStamp()const;
		// strFormat 有长度限制，最长为 128
		StrImg Format(const StrImg& imFmt) const; // 封装了格式化的api
		// language 默认为 0，表示用英语，后面表示是否缩写（在英语时才有用）
		StrImg WeekName (bool abbr=true) const;
		StrImg MonthName(bool abbr=true) const;
		StrImg ToStr(TFmt_Type TType=TFmt_Normal) const;
		// 常用于在程序中计时
		TimeT TickStart( );	// 返回进程启动后的毫秒数
		TimeT TickStop( );	// 返回从上一个 TickStart() 至当前的毫秒数，且设置重新计时，即下次调用Stop获取的是上次调用到本次之间的时差
		// operator
		STime& operator = (const STime&);
		// 时间增加： STime(m_TStamp + NewTStamp)
		// 时间减少： STime(m_TStamp - NewTStamp)
		STime operator - (const STime&) const;
		bool operator == (const STime&) const;
		bool operator != (const STime&) const;
		bool operator <	 (const STime&) const;
		bool operator >	 (const STime&) const;
		bool operator >= (const STime&) const;
		bool operator <= (const STime&) const;
	private:
		void* m_pTimeR;
	};

}; // namespace

#endif

