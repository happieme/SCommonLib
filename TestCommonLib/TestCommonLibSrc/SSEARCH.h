#ifndef HAPPIEME_SSEARCH
#define HAPPIEME_SSEARCH
#include <vector>
#include <iostream>
#include "SString/String.h"
#include <regex>

namespace smm
{
	using namespace std;
	using namespace tr1;

	class SValue
	{
		struct tagValue
		{
			int  val;
			int  id;
			tagValue()
			{}
			tagValue(int v,int ID)
			{
				val = v;
				id  = ID;
			}
		};
	private:
		typedef vector<tagValue> tagVec;
		tagVec m_ValArr;
		CString m_ExpStr;
		bool m_bValid;
	public:
		int GetMin( )
		{	return m_ValArr.empty()?TheEndPos:m_ValArr[0].val;	}

		int GetMax( )
		{	return m_ValArr.empty()?TheEndPos:m_ValArr[m_ValArr.size()-1].val;	}

		bool Match(int val)
		{
			UInt Size = m_ValArr.size();
			if(m_ValArr.empty() || val<m_ValArr[0].val || val>m_ValArr[Size-1].val)
			{	return false;	}

			UInt i = 0;
			// find the first element which is greater than val
			// the pos-1 is lowPos
			for(;i<Size;i++)
			{
				if(val > m_ValArr[i].val)
				{	continue;	}
				else
				{	break;	}
			}
			if(val == m_ValArr[i].val)
			{	return true;	}
			// val is surely less than the current value 
			// i is upPos and i-1 is lowPos(both no equal)
			if(m_ValArr[i-1].id==m_ValArr[i].id && 0!=m_ValArr[i].id)
			{	return true;	}
			else
			{	return false;	}
		}

		void Show()
		{
			tagVec::iterator itr = m_ValArr.begin();
			tagVec::iterator iend= m_ValArr.end();
			for(;iend!=itr;++itr)
			{	cout<<itr->id<<" , "<<itr->val<<endl;	}
		}

		bool Resolve( )// Àý£º{3,5,[6,9],12}
		{
			/*{12,[14,19],22,[25,28],32}
			m_ValArr.push_back(tagValue(12,0));
			m_ValArr.push_back(tagValue(14,1));
			m_ValArr.push_back(tagValue(19,1));
			m_ValArr.push_back(tagValue(22,0));
			m_ValArr.push_back(tagValue(25,2));
			m_ValArr.push_back(tagValue(28,2));
			m_ValArr.push_back(tagValue(32,0));
			*/
			UInt i = 1;
			UInt StrSize = m_ExpStr.Size();
			tagValue vOne;
			const CharC* pBase = m_ExpStr.ToStr();
			UInt BaseSize = m_ExpStr.Size();
			while(i < StrSize)
			{
				CharC c = m_ExpStr.GetAt(i);
				if('[' == c)
				{	// i is pos of '['
					UInt k = i;
					do
					{	k++;	}
					while(i<StrSize && ']'!=m_ExpStr.GetAt(k));
					if(StrSize == k)
					{	return false;	}
					else // now k is pos of ']'
					{
						UInt n = i+1;
						while(n<StrSize && '-'!=m_ExpStr.GetAt(n))
						{	n++;	}
						// n is pos of '-'
						if(n>k)
						{	return false;	}
						CStrImg imArr[] =
						{
							CStrImg(pBase+i+1 , n-i-1) ,
							CStrImg(pBase+n+1 , k-n-1)
						};
						for(int m=0;m<2;m++)
						{
							vOne.id = i+1;
							vOne.val= ToInt<CharC,UInt>(imArr[m]);
							if(!imArr[m].ToStr())
							{	return false;	}
							m_ValArr.push_back(vOne);
						}
						i = k+2; // next loop
					}
				} // if
				else
				{
					if(c<0x30 && c>0x39) // i is pos of digit
					{	return false;	}
					UInt n = i+1;
					while(n<StrSize && ','!=m_ExpStr.GetAt(n))
					{	n++;	}
					// n is pos of ','
					CStrImg imT(pBase+i,n-i);
					vOne.id = 0;
					vOne.val= ToInt<CharC,UInt>(imT);
					if(!imT.ToStr())
					{	return false;	}
					m_ValArr.push_back(vOne);
					i = n+1;
				}
			} // for
			return true;
		}

		bool Check(const CStrImg& imStr)
		{
			m_ExpStr = imStr;
			m_ExpStr.Trim();

			UInt StrSize = m_ExpStr.Size();
			if  ( 0 != m_ExpStr.Find("{") 
				|| StrSize-1 != m_ExpStr.Find("}")
				|| StrSize < 2 )
			{	return false;	}

			for(UInt i=1;i<m_ExpStr.Size()-1;)
			{
				CharC c = m_ExpStr.GetAt(i);
				if((c>=0x30&&c<=0x39) || '['==c || ']'==c || ','==c || '-'==c)
				{	i++;	}
				else if(' '==c || '\t'==c)
				{	m_ExpStr.Delete(i);	}
				else
				{	return false;	}				
			}
			return true;
		}

		void Read(const CStrImg& imStr)
		{
			m_bValid = Check(imStr) && Resolve( );
		}
	};

	void TestSValue()
	{
		SValue sv;
		CString strValue("{12 , [14-19] , 22 , [25-28] , 32}");
		sv.Read(strValue);
		cout<<"Show"<<endl;
		sv.Show();
		cout<<sv.Match(15)<<endl;
		cout<<sv.Match(2)<<endl;
		cout<<sv.Match(150)<<endl;
		cout<<sv.Match(28)<<endl;
		cout<<sv.Match(27)<<endl;
		cout<<sv.Match(31)<<endl;
		//
		cout<<"regex"<<endl;
		string str("subject");
		regex xStr("s.*t");
		cout<<regex_match(str, xStr)<<endl;
		xStr = "s.?t";
		cout<<regex_match(str, xStr)<<endl;
		str = "sit";
		xStr= "s.*t";
		cout<<regex_match(str, xStr)<<endl;
		xStr= "s.t";
		cout<<regex_match(str, xStr)<<endl;
	}
};

#endif
