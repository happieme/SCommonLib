#include "../../CommonLib/CommonLibSrc/STime/STime.h"
#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"

namespace smm
{
	void TestSTime()
	{
		STime st;
		st.TickStart( );
		StrImg imStrT("2013-07-12 05:19:15");
		StrImg imFmtT("yyyy-mm-dd hh:mi:ss");
		st.SetTime(imFmtT,imStrT);
		cout<<"st  TimeStamp : "<<st.TimeStamp() <<" , Time : "<<st.ToStr().ToCStr()<<endl;;
		cout<<"st  HttpTime : "<<st.ToStr(TFmt_Http).ToCStr()<<endl;
		cout<<"Week : "<<st.WeekName().ToCStr()<<endl;
		cout<<"Month: "<<st.MonthName().ToCStr()<<endl;
		st.SetTime(0);
		cout<<"TimeStamp : "<<st.TimeStamp()<<endl;
		cout<<"Time : "<<st.ToStr().ToCStr()<<endl;
		st.ToNow();
		cout<<"st  Update : "<<endl;
		cout<<"Local ; "<<st.ToStr().ToCStr()<<endl;
		cout<<"Gmt   ; "<<st.ToStr(TFmt_Http).ToCStr()<<endl;
		cout<<"st  YearDay : "<<st.YearDay()<<endl;
		cout<<"Http Time"<<endl;
		StrImg imHttp("Thu, 23 Mar 2006 07:54:53 GMT");
		cout<<"Set HttpTime : "<<imHttp.ToCStr()<<endl;
		st.SetTime(imHttp,TFmt_Http);
		cout<<"Local Time : "<<st.ToStr().ToCStr()<<endl;
		cout<<"Http  Time : "<<st.ToStr(TFmt_Http).ToCStr()<<endl;
		cout<<"---------------------------------Tick Count : "<<st.TickStop()<<endl;
	}

};
