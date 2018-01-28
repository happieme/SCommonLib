#include "SString/String.h"
#include "SString/Strbuf.h"
#include "SSystem/SSystem.h"
#include "STime/STime.h"
#include <vector>
#include <list>
#include "SCommon/SIOStd.h"

namespace smm
{
	using std::vector;
	using std::list;

	SCSTRIMG(imCodeSrc, "{"
							"int a1;"
							"int d=32;"
							"if(d>40)"
							"{"
								"a1 = d;"
								"a1 = d+3;"
							"}"
							"else"
							"{"
								"d = 40;"
							"}"
							"a1 = d + 13;"
							"for(UInt i=0;i<size;++i)"
							"{"
								"a1 += 23;"
							"}"
						"}"
		);

	CStrImg Constant_TypeArr[] = {"number", "string"};
	CStrImg Inner_TypeArr[] = {"int", "long", "char"};
	CStrImg Symbol_Arr[] = {",", "{", "}", "(", ")", "[", "]", ";", "'", "\""};
	CStrImg Operator_TypeArr[] = {"+", "-", "*", "/", "?", ":", "%", "!", ">", "<", ".", "=", "->"};
	CStrImg Variable_TypeArr[] = {"variable", "function", "class", "struct"};
	CStrImg Control_TypeArr[] = {"if", "else", "continue", "break", "for", "do", "while", "goto"};

	bool IsWhiteSpace(CharC c)
	{	return ' '==c || '\t'==c || '\r'==c || '\n'==c;	}

	bool IsDigit(CharC c)
	{	return '0'<=c&&c<='9';	}

	bool IsLetter(CharC c)
	{	return 'a'<=c&&c<='z' || 'A'<=c&&c<='Z';	}

	bool IsSymbol(CharC c)
	{	return 33<=c&&c<=47 || 58<=c&&c<=64 || 91<=c&&c<=96 || 123<=c&&c<=126;	}

	typedef vector<CStrImg> CStrArr;

	class SWord
	{
	public:
		CStrArr m_WordArr;
		void FormWordArr(const CStrImg&);
		CString m_TmpStr;
	};

	struct tagStateInfo
	{
		UInt sPos;
		UInt tPos;
		UInt	Type; // plain,if,for,while
		UInt	level;// 括号的层次
	};

	class SSyntax
	{
		const CStrArr& m_rWordArr;
	public:
		SSyntax(const CStrArr& rArr):m_rWordArr(rArr){}
		CStrArr m_StatementArr;
		list<tagStateInfo>	m_StructArr;
		void ShowStructArr();
		void AnalyzeStructArr();
		void FormStructArr();
		void SpecialStrcut();
		void Work();
	};

	void SSyntax::Work()
	{
		FormStructArr();
		ShowStructArr();
		AnalyzeStructArr();
	}

	void SSyntax::AnalyzeStructArr()
	{
	}

	void SSyntax::ShowStructArr()
	{
		UInt sPos = 0;
		UInt tPos = 0;
		UInt level= 0;		
		const list<tagStateInfo>& StatementArr = m_StructArr;
		list<tagStateInfo>::iterator itr,iend;
		iend= m_StructArr.end();
		for(itr = m_StructArr.begin();itr!=iend;++itr)
		{
			sPos = itr->sPos;
			tPos = itr->tPos;
			level = itr->level;
			//	cout<<StatementArr[i].level<<" , "<<StatementArr[i].sPos<<" , "<<StatementArr[i].tPos<<" , "<<StatementArr[i].Type<<endl;
			//	cout<<"level : "<<StatementArr[i].level<<endl;
			for(UInt k=0;k<level;++k)
			{	cout<<'\t';}

			for(UInt j=sPos;j<tPos;++j)
			{
				m_rWordArr[j].Show();
				cout<<" ";
			}
			cout<<endl;
		}
	}

	void SSyntax::SpecialStrcut()
	{
		const list<tagStateInfo>& StatementArr = m_StructArr;
		list<tagStateInfo>::iterator itr,iend;
		iend= m_StructArr.end();
		CStrImg iStr;
		UInt sPos,tPos;
		UInt level = 0;
		UInt BracketLevel = 0;
		for(itr = m_StructArr.begin();itr!=iend;++itr)
		{
			sPos = itr->sPos;
			tPos = itr->tPos;
			iStr = m_rWordArr.at(sPos);
			if(iStr.IsEqual("if",true))
			{	// 固定格式 if(...){...}else if(){}...else{}，必须使用大括号！
				// 下面一定是"()"，可能有嵌套
				++sPos;
				if('(' != m_rWordArr.at(sPos).GetAt(0))
				{	throw "There must be a ( after 'if'!";	}
				else
				{
					BracketLevel = 0;
					char c;
					for(;sPos<tPos;++sPos)
					{
						c = m_rWordArr.at(sPos).GetAt(0);
						if('(' == c)
						{	++BracketLevel;	}
						else if(')' == c)
						{
							if(0 == BracketLevel)
							{	break;	}
							else
							{	--BracketLevel;	}
						}
					}
				}
			}
			else if(iStr.IsEqual("while",true))
			{}
			else if(iStr.IsEqual("for",true))
			{}
		}
	}

	void SSyntax::FormStructArr()
	{
		tagStateInfo theInfo;
		int level = 0;
		const UInt size = m_rWordArr.size();
		CStrImg iStr;
		for(UInt i=0;i<size;++i)
		{
			iStr = m_rWordArr[i];
			if(iStr.IsEqual("{"))
			{	++level;	continue;	}
			else if(iStr.IsEqual("}"))
			{	--level;	continue;	}
			theInfo.level = level;
			theInfo.sPos = i;
			UInt j = i;
			for(;j<size;++j)
			{
				iStr = m_rWordArr[j];
				if(iStr.IsEqual(";"))
				{
					i = j;
					break;
				}
				else if(iStr.IsEqual("{"))
				{
					i = j-1;
					break;
				}
			}
			theInfo.tPos = j;
			m_StructArr.push_back(theInfo);
		}
	//	m_Structure
	}

	void SWord::FormWordArr(const CStrImg& imCode)
	{
		UInt i=0;
		CharC c;
		const CharC* pBase = imCode.ToStr();
		CStrImg imWord;
		UInt sPos = 0; // word 的起始位置
		UInt tPos = 0;
		while(i<imCode.Size())
		{// whitespace
			while(i<imCode.Size())
			{
				c = imCode.GetAt(i);
				if(IsWhiteSpace(c))
				{
					++i;
					continue;
				}
				else
				{	break;	}
			}

			sPos = i; // word 的起始位置
			if(IsSymbol(c))
			{	++i;	}
			else
			{
				while(i<imCode.Size())
				{
					c = imCode.GetAt(i);
					if(IsDigit(c) || '_'==c || IsLetter(c))
					{
						++i;
						continue;
					}
					else
					{	break;	}
				}
			}

			imWord.SetImg(pBase+sPos,i-sPos);
			m_WordArr.push_back(imWord);
		}
	}

	/*	const CStrImg& SWord::GetWordType(const CStrImg& imCodeSrc)
	{
	// if is Control_Type
	for(UInt i=0;i<COUNTARR(Control_TypeArr);++i)
	{
	if(imWord.IsEqual(Control_TypeArr[i],true))
	{	return Symbol_TypeArr[i];	}
	}
	}

	CStrImg InnerTypeArr[] = {"int", "long", "char"};
	// int long char

	// if else continue break for do while goto
	bool SWord::IsControlWord(const CStrImg& imWord)
	{
	for(UInt i=0;i<COUNTARR(ControllerArr);++i)
	{
	if(imWord.IsEqual(ControllerArr[i],true))
	{	return true;	}
	}
	return false;
	}
	*/
	void TestSyntax()
	{
		SWord swd;
		const CStrArr& theWordArr = swd.m_WordArr;
		swd.FormWordArr(imCodeSrc);
		for(UInt i=0;i<theWordArr.size();i++)
		{
			theWordArr[i].Show();
			cout<<endl;
		}
		SSyntax stax(theWordArr);
		stax.Work();
	}

	UInt FindMatchBracket(const CStrImg& ToFind,CStrImg Arr[],UInt ArrSize,UInt sPos,UInt tPos)
	{
		return 0;
	}
};
