#include "SJson.h"
#include <map>
#include <stack>
#include <vector>
#include <utility>
#include <list>
#include "../SCommon/SClib.h"

namespace smm
{
	using std::map;
	using std::stack;
	using std::list;
	using std::make_pair;
	using std::vector;

	SCSTRIMG(imObj,"{}");
	SCSTRIMG(imArr,"[]");
	enum enumJsonType { JSONSTRING, JSONOBJECT, JSONARRAY};

	struct SData
	{
		friend class SJsonInner;
	private:
		void* pVal;
		enumJsonType type;
		SData* pParent;
	public:
		SData(void* p, enumJsonType t, SData* pP):pVal(p),type(t),pParent(pP)
		{}
		~SData()
		{	delete pVal;	}
	};

	typedef map<String,SData*> DObj;
	typedef list<SData*> DArr;

	struct tagKeyArr
	{
		Uint Type;
		StrImg Key;
	};

	class SJsonInner
	{
	private:
		SData* m_pData; // 指向 root下存储的数据
		SData* m_pCurr;
		SData* m_pNext;
		String m_BasePath;
		SData* m_pRoot; // 一个类型为数组的节点
		String m_strJson;
		int    m_nLevel; // 当前的所在层，初始时为0，每经过一个{或[，会加一层，每经过一个}或]，会减一层
	public:
		SJsonInner(const StrImg& strJson)
		{
			Init();
			Resolve(strJson);
		}

		SJsonInner(const SJsonInner& jsonInner)
		{
			Init();
			SData* pDst = Copy(m_pRoot, jsonInner.m_pData);
			DArr* p = (DArr*)(m_pRoot->pVal);
			p->push_back(pDst);
			if(p->size() > 0)
			{	m_pCurr = m_pData = p->front();	}
			else
			{	m_pCurr = m_pData = NULL;	}
		}

		SData* Copy(SData* pDstParent, const SData* pSrc)
		{
			if(JSONSTRING == pSrc->type)
			{	return CreateNewData(*((String*)(pSrc->pVal)),pDstParent);	}
			else if(JSONOBJECT == pSrc->type)
			{
				SData* pObj = CreateNewData(imObj, pDstParent);
				DObj* pS = (DObj*)(pSrc->pVal);
				DObj* pD = (DObj*)(pObj->pVal);
				for(DObj::iterator itr = pS->begin();itr!=pS->end();++itr)
				{
					pD->insert(make_pair(itr->first, Copy(pObj,itr->second)));
				}
				return pObj;
			}
			else if(JSONARRAY == pSrc->type)
			{
				SData* pArr = CreateNewData(imArr, pDstParent);
				DArr* pS = (DArr*)(pSrc->pVal);
				DArr* pD = (DArr*)(pArr->pVal);
				for(DArr::iterator itr = pS->begin();itr!=pS->end();++itr)
				{
					pD->push_back(Copy(pArr, *itr));
				}
				return pArr;
			}
			return NULL;
		}

		~SJsonInner()
		{
			m_pCurr = m_pData;
			Clear();
			delete m_pRoot->pVal;
			delete m_pRoot;
		}

		void Init()
		{
			m_pData = 0;
			m_pCurr = 0;
			m_pNext = 0;
			m_nLevel = 0;
			m_pRoot = new SData(new DArr, JSONARRAY, NULL); // 默认是数组，所有东西都可以装
		}

		StrImg BasePath(const StrImg& Path)
		{
			if(Path.IsEmpty())
			{
				m_BasePath = "*";
				return m_BasePath;
			}
			StrImg tmp(Path);
			tmp.Trim();
			if('*' != tmp.GetAt(0))
			{
				m_BasePath.Clear();
				throw "SJon::SetBasePath(const StrImg& Path) Error! Cannot find \"*\" in Path!";
			}
			m_BasePath = Path;
			m_BasePath.Trim();
			m_pCurr = ResolvePath(m_BasePath);
			return m_BasePath;
		}

		Uint BaseSize()
		{
			SData* pJson = m_pCurr;
			Uint Size = TheEndPos;
			if(pJson)
			{
				if(JSONOBJECT == pJson->type)
				{	Size = ((DObj*)(pJson->pVal))->size();	}
				else if(JSONARRAY == m_pRoot->type)
				{	Size = ((DArr*)(pJson->pVal))->size();	}
			}
			return Size;
		}

		bool IsValid()
		{	return NULL!=m_pData;	}

		void Clear()
		{
			ReleaseData(m_pData);
			((DArr*)(m_pRoot->pVal))->clear();
			m_pData = 0;
			m_pCurr = 0;
			m_pNext = 0;
			m_BasePath.Clear(true);
			m_strJson.Clear(true);
			m_nLevel = 0;
		}

		void ReleaseData(SData* pData)
		{
			SData* pCurr = pData;
			if(!pCurr)
			{	return;	}

			if(JSONOBJECT == pCurr->type)
			{
				DObj* p = (DObj*)(pCurr->pVal);
				DObj::iterator itr = p->begin();
				for(;itr!=p->end();++itr)
				{	ReleaseData((SData*)(itr->second));	}
				p->clear();
			}
			else if(JSONARRAY == pCurr->type)
			{
				DArr* p = (DArr*)(pCurr->pVal);
				DArr::iterator itr = p->begin();
				for(;itr!=p->end();++itr)
				{	ReleaseData((SData*)(*itr));	}
				p->clear();
			}
			delete pCurr;
		}

		SData* CreateNewData(const StrImg& Val, SData* pParent)
		{
			SData* pNew = NULL;
			if(Val.IsNull())
			{	return NULL;		}
			if(Val.IsEqual(imObj))
			{
				pNew = new SData(new DObj, JSONOBJECT, pParent);
				m_pNext = pNew;
			}
			else if(Val.IsEqual(imArr))
			{
				pNew = new SData(new DArr, JSONARRAY, pParent);
				m_pNext = pNew;
			}
			else
			{
				pNew = new SData(new String(Val), JSONSTRING, pParent);
			}
			return pNew;
		}

		void Add(const SJsonInner& js)
		{
			if(m_pCurr && JSONARRAY == m_pCurr->type)
			{
				SData* pData = new SData(new DArr, JSONARRAY, NULL);
				Copy(pData, js.m_pData);
				DArr* pCurrArr = (DArr*)(m_pCurr->pVal);
				pCurrArr->push_back(pData);
			}
			else
			{	throw "SJon::Add(const SJsonInner& js) Error! You should add this json to an array!";	}
		}

		void Add(const StrImg& key, const SJsonInner& js)
		{
			if(m_pCurr && JSONOBJECT == m_pCurr->type)
			{
				DObj* pCurrObj = (DObj*)(m_pCurr->pVal);
				pCurrObj->insert(make_pair(String(key), js.m_pData));
			}
			else
			{	throw "SJon::Add(const StrImg& Val, const SJsonInner& js) Error! You should add this json to an object!";	}
		}

		void Add(const StrImg& Val)
		{
			if(m_pCurr && JSONARRAY == m_pCurr->type)
			{
				DArr* p = (DArr*)(m_pCurr->pVal);
				p->push_back(CreateNewData(Val, m_pCurr));
			}
			else
			{	throw "SJon::Add(const StrImg& Val) Error! You should add this json to an array!";	}
		}

		void Add(const StrImg& Key, const StrImg& Val)
		{
			if(m_pCurr && JSONOBJECT == m_pCurr->type)
			{
				DObj* p = (DObj*)(m_pCurr->pVal);
				p->insert(make_pair(Key,CreateNewData(Val, m_pCurr)));
			}
			else
			{	throw "SJon::Add(const StrImg& Key, const StrImg& Val) Current container must be an object!";	}
		}

		void In()
		{
			if(m_pNext)
			{	m_pCurr = m_pNext;	}
		}

		void Out()
		{
			if(m_pCurr && m_pCurr->pParent)
			{	m_pCurr = m_pCurr->pParent;	}
		};

		Uint GetJsonStrSize(SData* pData)
		{
			Uint size = 0;
			if(JSONSTRING == pData->type)
			{
				size++;
				size+= ((String*)(pData->pVal))->Size();
				size++;
			}
			else if(JSONOBJECT == pData->type)
			{
				DObj* p = (DObj*)(pData->pVal);				
				size++;
				for(DObj::iterator itr = p->begin();itr!=p->end();++itr)
				{
					size++;
					size+= itr->first.Size();
					size+=2;
					size+=GetJsonStrSize((SData*)(itr->second));
					size++;
				}
			}
			else if(JSONARRAY == pData->type)
			{
				DArr* p = (DArr*)(pData->pVal);
				size++;
				for(DArr::iterator itr = p->begin();itr!=p->end();++itr)
				{
					size+=GetJsonStrSize((SData*)(*itr));
					size++;
				}
			}
			return size;
		}

		StrImg ToStr(bool bFormatted)
		{
			if(IsValid())
			{
				// throw "SJon::ToStr(bool bFormatted) Error! SJon is invalid!";
				Uint strSize = GetJsonStrSize(m_pCurr);
			//	cout<<"GetJsonStrSize() "<<strSize<<endl;
				m_strJson.Clear();
				m_strJson.ReSize(strSize);
				ToStr(m_pCurr, bFormatted);
			}
		//	cout<<m_strJson.Size()<<endl;
			return m_strJson;
		}

		void FormatHelp(String& str, int nLevel)
		{
			str += "\n";
			for(int i=0;i<nLevel;i++) // 四个空格
			{	str += "    ";	}
		}

		void ToStr(SData* pData, bool bFormatted)
		{
			String& str = m_strJson;
			if(JSONSTRING == pData->type)
			{
				str += "\"";
				str += *((String*)(pData->pVal));
				str += "\"";
			}
			else if(JSONOBJECT == pData->type) // 对象
			{
				if(bFormatted)
				{// { 的前面
					char c = 0;
					for(int i=static_cast<int>((str.Size()))-1; i>=0; i--)
					{
						c = str.GetAt(i);
						if(' ' == c)
						{	continue;	}
						else if('\n'==c || '{'==c || '['==c)
						{	break;	}
						else
						{
							FormatHelp(str, m_nLevel);
							break;
						}
					}
				}
				str += "{";
				if(bFormatted)
				{
					m_nLevel++;
					FormatHelp(str, m_nLevel);
				}
				DObj* p = (DObj*)(pData->pVal);
				DObj::iterator itr = p->begin();
				for(Uint i=0,size=p->size();i<size;++itr,i++)
				{
					str += "\"";
					str += itr->first;
					str += "\":";
					ToStr((SData*)(itr->second), bFormatted);
					if(i < size-1)
					{
						str += ","; // 在对象里逗号的后面只可能是 引号 或 }，前一种情况的后面不会有换行，后一种情况则不会到这 if 里
						if(bFormatted)
						{	FormatHelp(str, m_nLevel);	}
					}
				}
				if(bFormatted)
				{
					m_nLevel--;
					FormatHelp(str, m_nLevel);
				}
				str += "}";
			}
			else if(JSONARRAY == pData->type) // 数组
			{
				if(bFormatted)
				{// [ 的前面
					char c = 0;
					for(int i=static_cast<int>((str.Size()))-1; i>=0; i--)
					{
						c = str.GetAt(i);
						if(' ' == c)
						{	continue;	}
						else if('\n'==c || '{'==c || '['==c)
						{	break;	}
						else
						{
							FormatHelp(str, m_nLevel);
							break;
						}
					}
				}
				str += "[";
				if(bFormatted)
				{
					m_nLevel++;
					FormatHelp(str, m_nLevel);
				}
				DArr* p = (DArr*)(pData->pVal);
				DArr::iterator itr = p->begin();
				for(Uint i=0,size=p->size();i<size;++itr,i++)
				{
					ToStr((SData*)(*itr), bFormatted);
					if(i < size-1)
					{
						char c = str.GetAt(str.Size()-1);
						str += ",";//若后面是 { 或 [ 则会有两个换行，所以若逗号的前一个字符是 } 或 ] 则紧跟即可，不加空格和换行
						if(bFormatted && '}'!=c && ']'!=c)
						{	FormatHelp(str, m_nLevel);	}
					}
				}
				if(bFormatted)
				{
					m_nLevel--;
					FormatHelp(str, m_nLevel);
				}
				str += "]";
			}
		}

		SData* ResolvePath(const StrImg& keyPath) // 有空格的情况？？？
		{
			tagKeyArr keyArr[512];
			Uint Len = keyPath.Size();
			Uint posD,posL,posT;
			StrImg tmpStr;
			Uint keyType, keyCount=0;
			Uint index = 0;
			SData* pCurr = NULL;
			if('*' == keyPath.GetAt(0))
			{
				index = 1;
				pCurr = m_pData;
			}
			else if(0 == keyPath.Find("this"))
			{
				pCurr = m_pCurr;
				index = 4;
			}
			else
			{
				pCurr = m_pCurr;
				index = 0;
			}

			if(!pCurr)
			{	return NULL;	}
			char tmpC = 0;
			for(;index<Len;)
			{
				tmpC = keyPath.GetAt(index);
				if('.' == tmpC) // 当前为对象
				{
					posL = keyPath.Find("[",index+1);
					posD = keyPath.Find(".",index+1);
					posT = SClib::Min<Uint>(posD,posL);
					if(TheEndPos == posT)
					{	posT = Len;	}
					keyType = TheEndPos;
				}
				else if('[' == tmpC)
				{
					posT = keyPath.Find("]",index+1);
					if(TheEndPos == posT)
					{	keyCount=0;	break;	}
					keyType = 1;
				}
				else if(']' == tmpC)
				{
					index++;
					continue;
				}
				else if(' '==tmpC && '\t'==tmpC)
				{	continue;	}
				else
				{	keyCount=0;	break;	}
				tmpStr.SetImg(keyPath.ToCStr()+index+1, posT-index-1);
				index = posT;
				tmpStr.Trim();
				if(tmpStr.IsEmpty())
				{	keyCount=0;	break;	}
				tagKeyArr oneItem;
				oneItem.Key = tmpStr;
				if(TheEndPos == keyType)
				{	oneItem.Type = keyType;	}
				else
				{	oneItem.Type  = ToInt<Uint>(tmpStr);	}
				keyArr[keyCount] = oneItem;
				++keyCount;
			}

			// 解析路径中每个词，获取最后的 pData ------------------------------------
			Uint Type;
			for(Uint j=0;j<keyCount;++j)
			{
				Type = keyArr[j].Type;
				if(TheEndPos == Type) // object
				{
					DObj*p = (DObj*)(pCurr->pVal);
					DObj::iterator itObj = p->find(keyArr[j].Key);
					if(p->end() != itObj)
					{
						pCurr = itObj->second;
					}
					else
					{	return NULL;	}
				}
				else // array
				{
					DArr* p = (DArr*)(pCurr->pVal);
					DArr::iterator itr = p->begin();
					Uint Size = p->size();
					if(Type < Size)
					{
						for(Uint i=0; i<Type; i++)
						{	advance(itr, 1);	}
					}
					else
					{	return NULL;	}
					pCurr = *itr;
				}
			}
			return pCurr;
		}

		StrImg Get(const StrImg& path)
		{
			SData* pCurr = m_pCurr;
			StrImg tmp(path);
			tmp.Trim();
			StrImg imVal;
			if(!path.IsEmpty() && m_pData)
			{
				pCurr = ResolvePath(tmp);
				if(pCurr)
				{
					if(JSONSTRING == pCurr->type)
					{	imVal = *((String*)(pCurr->pVal));	}
					else
					{
						m_strJson.Clear();
						ToStr(pCurr, false);
						imVal = m_strJson;
					}
				}
			}
			return imVal;
		}

		void Set(const StrImg& path, const StrImg& value)
		{
			StrImg tmp(path);
			tmp.Trim();
			SData* pCurr = m_pCurr;
			if(m_pData && !tmp.IsEmpty())
			{	pCurr = ResolvePath(tmp);	}
			if(pCurr && JSONSTRING==pCurr->type)
			{	*((String*)(pCurr->pVal)) = value;	}
	/*		else if(!pCurr)
			{	throw "SJson::Set(const StrImg& path, const StrImg& value) Error! Path resolved error.";	}
			else if(JSONSTRING != pCurr->type)
			{	throw "SJson::Set(const StrImg& path, const StrImg& value) Error! You should not set a non-string value.";	}
	*/	}

		void Delete(const StrImg& path)
		{
			SData* pCurr = ResolvePath(path);
			SData* pParent = pCurr->pParent;
			m_pCurr = pParent;
			if(pParent && JSONARRAY == pParent->type)
			{
				DArr* p = (DArr*)(pParent->pVal);
				for(DArr::iterator b=p->begin(),e=p->end();b!=e;++b)
				{
					if(pCurr == *b)
					{
						p->erase(b);
						break;
					}
				}
			}
			else if(JSONOBJECT == pParent->type) // ResolvePath 返回的 pCurr 是 value，不是key ？？？？
			{
				DObj* p = (DObj*)(pParent->pVal);
				StrImg a = *((String*)(pCurr->pVal));
				DObj::iterator itr = p->find(a);
				if(itr != p->end())
				{	p->erase(itr);	}
			}
			ReleaseData(pCurr);
		}

		SJson Sub(const StrImg& Path)
		{
			SJson js("");
			throw "SJson::Set(const StrImg& path, const StrImg& value) Error! The func is not ready.";
			return js;
		}

		void Resolve(const StrImg& Src)
		{
			StrImg tmp(Src);
			tmp.Trim();
			if(Src.IsEmpty())
			{	return;	}
			Clear();
			const Uint SrcSize = Src.Size();
			const char* pSrc = Src.ToCStr();
			stack<char> markStack;
			StrImg Key, Val;
			Uint ErrNo = 0;
			SARRAYSTR(NextSymbol, 32, "{["); // 下一个应出现的字符
			char firsChar[2] = {""};
			char c = 0;	Uint nextPos = 0;
			tmp.Trim('L');
			SData* pCurr = m_pRoot;
			while(!tmp.IsEmpty())
			{
				tmp.Trim('L');
				c = tmp.GetAt(0);
				firsChar[0] = c;
				if(TheEndPos == NextSymbol.Find(firsChar))
				{	ErrNo = 1;	break;	}
				if(JSONOBJECT == pCurr->type) // " } ,
				{
					if('"' == c)
					{
						nextPos = ExtractString(tmp, Key);
						if(Key.IsNull())
						{	ErrNo = 2;	break;	}
						tmp = tmp.Sub(nextPos).Trim('L');
						if(tmp.IsEmpty() || ':' != tmp.GetAt(0))
						{	ErrNo = 3;	break;	}
						tmp = tmp.Sub(1);
						if(tmp.IsNull())
						{	ErrNo = 4;	break;	}
						tmp.Trim('L');
						Key.Trim();
						c = tmp.GetAt(0);
						DObj* pCurrObj = (DObj*)(pCurr->pVal);
						if('\"' == c)
						{
							nextPos = ExtractString(tmp, Val);
							if(Val.IsNull())
							{	ErrNo = 5;	break;	}
							Val.Trim();
							pCurrObj->insert(make_pair(String(Key), CreateNewData(Val, pCurr)));
							tmp = tmp.Sub(nextPos);
							NextSymbol = ",}";
						}
						else if('{' == c)
						{
							SData* pNew = CreateNewData(imObj, pCurr);
							pCurrObj->insert(make_pair(String(Key), pNew));
							pCurr = pNew;
							tmp = tmp.Sub(1);
							NextSymbol = "\"}";
							markStack.push('{');
						}
						else if('[' == c)
						{
							SData* pNew = CreateNewData(imArr, pCurr);
							pCurrObj->insert(make_pair(String(Key), pNew));
							pCurr = pNew;
							tmp = tmp.Sub(1);
							NextSymbol = "[{\"]0123456789tf-n";
							markStack.push('[');
						}
						else if(('0'<=c && c<='9') || '-'==c)
						{
							nextPos = ExtractNumber(tmp, Val);
							tmp = tmp.Sub(nextPos);
							NextSymbol = ",}";
							Val.Trim();
							pCurrObj->insert(make_pair(String(Key), CreateNewData(Val, pCurr)));
						}
						else if('t'==c || 'f' == c || 'n'==c)
						{
							nextPos = ExtractSpecialValue(tmp, Val);
							tmp = tmp.Sub(nextPos);
							NextSymbol = ",}";
							Val.Trim();
							pCurrObj->insert(make_pair(String(Key), CreateNewData(Val, pCurr)));
						}
						else
						{	ErrNo = 6;	break;	}
					}
					else if(',' == c)
					{
						tmp = tmp.Sub(1);
						NextSymbol = "\"";
					}
					else if('}' == c)
					{
						tmp = tmp.Sub(1);
						if(markStack.top() == '{')
						{
							pCurr = pCurr->pParent;
							markStack.pop();
							NextSymbol = ",}]";
						}
						else
						{	ErrNo = 7;	break;	}
					}
					else
					{	ErrNo = 8;	break;	}
				}
				else if(JSONARRAY == pCurr->type) // 当前已经在 '[' 的里面了，允许出现的字符有 ‘"’、‘,’、‘{’、‘]’、‘[’、number、true、false、null
				{
					DArr* pCurrArr = (DArr*)(pCurr->pVal);
					if('\"' == c)
					{
						nextPos = ExtractString(tmp, Val);
						if(Val.IsNull())
						{	ErrNo = 9;	break;	}
						tmp = tmp.Sub(nextPos);
						NextSymbol = ",]";
						Val.Trim();
						pCurrArr->push_back(CreateNewData(Val, pCurr));
					}
					else if(',' == c)
					{
						tmp = tmp.Sub(1);
						NextSymbol = "\"[{0123456789tf-n";
					}
					else if(('0'<=c && c<='9') || '-'==c) // 数字
					{
						nextPos = ExtractNumber(tmp, Val);
						tmp = tmp.Sub(nextPos);
						NextSymbol = ",]";
						Val.Trim();
						pCurrArr->push_back(CreateNewData(Val, pCurr));
					}
					else if('{' == c)
					{
						tmp = tmp.Sub(1);
						markStack.push('{');
						NextSymbol = "\"}";
						SData* pData = CreateNewData(imObj, pCurr);
						pCurrArr->push_back(pData);
						pCurr = pData;
					}
					else if('[' == c)
					{
						tmp = tmp.Sub(1);
						markStack.push('[');
						NextSymbol = "[{\"]0123456789tf-n";
						SData* pData = CreateNewData(imArr, pCurr);
						pCurrArr->push_back(pData);
						pCurr = pData;
					}
					else if(']' == c)
					{
						tmp = tmp.Sub(1);
						if(markStack.top() == '[')
						{
							pCurr = pCurr->pParent;
							markStack.pop();
							NextSymbol = ",]}";
						}
						else
						{	ErrNo = 10;	break;	}
					}
					else if('t'==c || 'f'==c || 'n' == c)
					{
						nextPos = ExtractSpecialValue(tmp, Val);
						if(Val.IsNull())
						{	ErrNo = 11;	break;	}
						tmp = tmp.Sub(nextPos);
						NextSymbol = ",]";
						Val.Trim();
						pCurrArr->push_back(CreateNewData(Val, pCurr));
					}
					else
					{	ErrNo = 12;	break;	}
				}
			}
			if(ErrNo>0 || pCurr!=m_pRoot)
			{
				DArr* pArr = ((DArr*)(m_pRoot->pVal));
				DArr::iterator itr = pArr->begin();
				DArr::iterator end = pArr->end();
				for(;end!=itr;++itr)
				{
					ReleaseData(*itr);
				}
				m_pCurr = m_pData = NULL;
				throw "SJson::Resolve() Error! maybe the json string is error!";
			}
			DArr* p = (DArr*)(m_pRoot->pVal);
			if(p->size() > 0)
			{	m_pCurr = m_pData = p->front();	}
			else
			{	m_pCurr = m_pData = NULL;	}
		}

		// true, false, null
		Uint ExtractSpecialValue(const StrImg& Src, StrImg& ret)
		{
			ret.Clear();
			char c = Src.GetAt(0);
			Uint len = Src.Size();
			StrImg tmp;
			Uint retLen = 0;
			if('t' == c)
			{
				if(len > 4 && 0 == Src.Find("true"))
				{	ret.SetImg(Src.ToCStr(),4);	retLen = 4;	}
			}
			else if('f' == c)
			{
				if(len > 5 && 0 == Src.Find("false"))
				{	ret.SetImg(Src.ToCStr(),5);	retLen = 5;	}
			}
			else if('n' == c)
			{
				if(len > 4 && 0 == Src.Find("null"))
				{	ret.SetImg(Src.ToCStr(),4);	retLen = 4;	}
			}
			return retLen;
		}

		// 返回 Src 中数字的下一个位置
		Uint ExtractNumber(const StrImg& Src, StrImg& ret)
		{
			char c = 0;
			Uint i = 0;
			Uint size = Src.Size();
			for(;i<size;i++)
			{
				c = Src.GetAt(i);
				if('.'==c || ('0'<=c && c<='9') || ('-'==c && 0==i))
				{	continue;	}
				break;
			}
			ret = Src.Sub(0, i);
			return i;
		}

		// 返回 Src 中右引号的下一个字符的位置
		Uint ExtractString(const StrImg& Src, StrImg& ret)
		{
			ret.Clear();
			Uint DQPosL = 0; // Src 的第一个字符已经是引号
			Uint DQPosR = DQPosL;
			while(true)
			{
				DQPosR = Src.Find("\"", DQPosR+1);
				if(TheEndPos == DQPosR)
				{	return TheEndPos;	}
				else
				{// 有反斜线表示是双引号字符本身
					if('\\' != Src.GetAt(DQPosR-1))
					{	break;	}
				}
			}

			ret = Src.Sub(DQPosL+1, DQPosR-DQPosL-1);
			return DQPosR+1;
		}
	};

	// 以上为 SJsonInner ------------------------------------------------------------------------------------------------------------------------

#define pInner	static_cast<SJsonInner*>(m_pJsonR)

	SJson::SJson(const StrImg& strJson)
	{
		m_pJsonR = new SJsonInner(strJson);
	}

	SJson::SJson(const SJson& js)
	{
		m_pJsonR = new SJsonInner(*(SJsonInner*)(js.m_pJsonR));
	}

	void SJson::Swap(SJson& js)
	{
		void* p = m_pJsonR;
		m_pJsonR = js.m_pJsonR;
		js.m_pJsonR = p;
	}

	SJson::~SJson()
	{	delete m_pJsonR;	}

	void SJson::Init(const StrImg& strJson)
	{
		pInner->Clear();
		pInner->Init();
		pInner->Resolve(strJson);
	}

	void SJson::Add(const SJson& js)
	{
		pInner->Add(*(static_cast<SJsonInner*>(js.m_pJsonR)));
	}
	void SJson::Add(const StrImg& key, const SJson& js)
	{
		pInner->Add(key, *(static_cast<SJsonInner*>(js.m_pJsonR)));
	}
	
	void SJson::Add(const StrImg& Val)
	{
		pInner->Add(Val);
	}

	void SJson::Add(const StrImg& Key, const StrImg& Val)
	{
		pInner->Add(Key,Val);
	}

	void SJson::In()
	{
		pInner->In();
	}

	void SJson::Out()
	{
		pInner->Out();
	}

	StrImg SJson::ToStr(bool bFormatted)
	{	return pInner->ToStr(bFormatted);	}

	void SJson::Clear()
	{	pInner->Clear();	}

	bool SJson::IsValid()
	{	return pInner->IsValid();	}

	StrImg SJson::Get(const StrImg& Path)
	{	return pInner->Get(Path);	}

	void SJson::Set(const StrImg& path, const StrImg& value)
	{	pInner->Set(path, value);	}

	StrImg SJson::BasePath(const StrImg& Path)
	{	return pInner->BasePath(Path);	}

	void SJson::FromStr(const StrImg& JsonStr)
	{	pInner->Resolve(JsonStr);	}

	Uint SJson::BaseSize()
	{	return pInner->BaseSize();	}

	void SJson::Delete(const StrImg& Path)
	{	pInner->Delete(Path);	}
};
