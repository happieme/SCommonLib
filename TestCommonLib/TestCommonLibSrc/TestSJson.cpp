#include "../../CommonLib/CommonLibSrc/SJson/SJson.h"
#include "../../CommonLib/CommonLibSrc/SCommon/SIOStd.h"
#include "../../CommonLib/CommonLibSrc/SString/String.h"
#include "../../CommonLib/CommonLibSrc/SString/SConvert.h"
#include "../../CommonLib/CommonLibSrc/SFile/SFile.h"

namespace smm
{
	void TestJsonCopy()
	{
		SARRAY(otherSchool,128);
		otherSchool="{'other':['MIT','North','Canada','China']}";
		otherSchool.Replace("'","\"");
		String str;

		SJson tmpJs(otherSchool);
		str = tmpJs.ToStr(true);
		cout<<str.ToCStr()<<endl;

		SJson js(tmpJs);
		tmpJs.Clear();
		str = js.ToStr(true);
		cout<<str.ToCStr()<<endl;

		SARRAY(car,128);
		car = "['Ford','BMW','BYD','Das Auto']";car.Replace("'","\"");
		tmpJs.Init(car);
		js.Add("car",tmpJs);
		StrImg path("this.car");
		StrImg imVal = js.Get(path);
		if(!imVal.IsNull())
		{
			cout<<"car: "<<imVal.ToCStr()<<endl;
		}
		else
		{	cout<<"path: "<<path.ToCStr()<<" is invalid!"<<endl;	}
		imVal = js.Get("this");

		cout<<"this: "<<imVal.ToCStr()<<endl;
		imVal = js.Get("*");
		cout<<"*: "<<imVal.ToCStr()<<endl;
	}

	void TestJsonCreate()
	{
		cout<<"TestJsonCreate"<<endl;
		try
		{
			SJson js("{}");
			js.In();
				js.Add("name", "Tom");
				js.Add("age", "15");
				js.Set("this.age", "12");
				cout<<"this.name: "<<js.Get("this.name").ToCStr()<<endl;
				js.Add("record","{}");
				js.In();
					js.Add("15","Middle School");
					js.Add("9" ,"Primary School");
					js.Add("College","[]");
					js.In();
						js.Add("Havard");
						js.Add("Yale");
						js.Set("this[0]", "South");
					js.Out();
				js.Out();
				js.Add("friend","[]");
				js.In();
					js.Add("Tom");
					js.Add("Max");
					js.Add("Joy");
				js.Out();
			js.Out();
			SARRAY(str, 1024);
			str = js.ToStr(true);
			cout<<str.Size()<<endl<<str.ToCStr()<<endl;
			cout<<js.Get("*. record .22[0] ").ToCStr()<<endl;
			cout<<js.Get(" *. friend[2] ").ToCStr()<<endl;
			js.BasePath("*.record.College");
			js.Set("[0]","Chicago");
		}
		catch(const char* err)
		{	cout<<err<<endl;	}
	}

	void TestJsonRead()
	{
		cout<<"TestJsonRead"<<endl;
		STextFile sf("../../../TestFile/JsonTest.json");
		if(STextFile::OpenNormal != sf.Open("rb+"))
		{
			cout<<"Open file error!"<<endl;
			return;
		}
		String str;
		sf.Read(str);
		sf.Close();
		SJson Reader;
		try
		{
			Reader.FromStr(str);//"undefined"
			if(!Reader.IsValid())
			{	cout<<"Json read error!"<<endl;	}
			cout<<Reader.ToStr(true).ToCStr()<<endl;
		//	StrImg imAddr = Reader.Get("*.count[0][0].addr");
		//	cout<<SConv::U8_A(imAddr).ToCStr()<<endl;
			Reader.BasePath("*");
			cout<<"GetBaseSize() : "<<Reader.BaseSize()<<endl;
			cout<<"*.count[0][1].major[2] "<<Reader.Get(".count[0][1].major[2]").ToCStr()<<endl;
			Reader.BasePath("*.count[0]");
			cout<<Reader.BaseSize()<<endl;
			cout<<Reader.Get("[1].major[2]").ToCStr()<<endl;
			cout<<Reader.Get("*.count[0][1].2").ToCStr()<<endl;
			cout<<Reader.Get("*.count[1].debt").ToCStr()<<endl;
			cout<<Reader.Get("*.count[1].math").ToCStr()<<endl;
			Reader.BasePath("*.count[1]");
			cout<<Reader.BaseSize()<<endl;
			cout<<"this .math: "<<Reader.Get("this .math").ToCStr()<<endl;
			cout<<"this .debt: "<<Reader.Get("this. debt").ToCStr()<<endl;
		}
		catch(const char* err)
		{	cout<<err<<endl;	}
	}

	void TestJsonModify()
	{
		cout<<"TestJsonModify -------------------------------------------------"<<endl;
		SJson js("{}");
		js.In();
			js.Add("name", "Tom");
			js.Add("age", "15");
			js.Add("friend","[]");
			js.In();
				js.Add("Tom");
				js.Add("Max");
				js.Add("Joy");
			js.Out();
		js.Out();
		SARRAY(str, 1024);
		str = js.ToStr(true);
		cout<<str.Size()<<endl<<str.ToCStr()<<endl;
		cout<<js.Get("*. friend[0] ").ToCStr()<<endl;
		js.BasePath("*. friend");
		js.Set("[0] ","Lucy");
		js.Add("Lily");
		cout<<js.ToStr().ToCStr()<<endl;
		cout<<js.Get("[0] ").ToCStr()<<endl;
		js.Delete("[1]");
		cout<<"Delete [1]"<<js.ToStr().ToCStr()<<endl;
		js.Delete("*.name");
		cout<<"Delete *name"<<js.ToStr().ToCStr()<<endl;
	}

	void TestSJson()
	{
		TestJsonCreate();
		TestJsonRead();
		TestJsonModify();
		TestJsonCopy();
/*		SFile sf("../../../TestFile/JsonTest.json");
		if(!sf.Open("rb+"))
		{
			cout<<"Open file error!"<<endl;
			return;
		}
		try
		{
			String strJson;
			String strBom;
			sf.Read(strBom, 3);
			if(!strBom.IsEqual(Utf8_Bom_Mark))
			{	cout<<"Please make sure the file is form in utf8!"<<endl;	}
			sf.SetPos(3);
			sf.Read(strJson);
		//	cout<<strJson.ToCStr()<<endl;
			SJson sJson(strJson,false);

			String Path("this");
			String Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.Size()<<" , "<<Res.ToCStr()<<endl<<endl;

			Path = "this.count";
			Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.Size()<<" , "<<Res.ToCStr()<<endl<<endl;

			Path = "this['count'][0]";
			Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.Size()<<" , "<<Res.ToCStr()<<endl<<endl;

			Path = "this['count'][0][0]";
			Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.Size()<<" , "<<Res.ToCStr()<<endl<<endl;

			Path = "this['count'][0][0].name";
			Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.Size()<<" , "<<Res.ToCStr()<<endl<<endl;

			Path = "this['count'][0][0]['µØÖ·']";
			Res = sJson.Read(SConv::A_U8(Path));
			SConv::U8_A(Res, Res);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.Size()<<" , "<<Res.ToCStr()<<endl<<endl;

			Path = "this.count[0][1].major";
			Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.ToCStr()<<endl<<endl;

			Path = "this.count[0][1].major[2]";
			Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.ToCStr()<<endl<<endl;

			Path = "this.count[1].math";
			Res = sJson.Read(Path);
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.ToCStr()<<endl<<endl;

			Path = "this.count[1]['¸ºÕ®']";
			Res = sJson.Read(SConv::A_U8(Path));
			cout<<Path.Size()<<" , "<<Path.ToCStr()<<endl<<Res.Size()<<" , "<<Res.ToCStr()<<endl<<endl;

			cout<<"Format --------------------------------------"<<endl;
			Res = sJson.Format();
			cout<<SConv::U8_A(Res).ToCStr()<<endl;
			SFile ResFile("D:\\cygwin\\home\\JsonFormat.txt");
			ResFile.Open("rb+");
			ResFile.Write(Utf8_Bom_Mark);
			ResFile.Write(Res);
		}
		catch(const char* pErr)
		{
			cout<<pErr<<endl;
		}
*/	}
};
