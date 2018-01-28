#include "SConfig.h"
#include "SFile.h"
#include "../SSystem/SSystem.h"
#include "../SString/SymbolConstant.h"
#include <map>
#include <utility>

namespace smm
{
	using std::map;
	using std::make_pair;

	template<class SInst>
	class SInstanceMap
	{
	private:
		static SLock Locker;
		String  Key;
		SInst* pInst;
		typedef map<String, SInst*> tagMapInst;
		tagMapInst m_MapInst;
	public:
		void Add(const StrImg& Key, SInst* NewInst)
		{
		Locker.Lock();
			m_MapInst.insert(make_pair(Key, NewInst));
		Locker.UnLock();
		}

		void Update(const StrImg& Key, SInst* NewInst)
		{
		Locker.Lock();
			tagMapInst::iterator itr = m_MapInst.find(Key);
			if(m_MapInst.end() != itr)
			{
				delete itr->second;
				itr->second = NewInst;
			}
			else
			{	m_MapInst.insert(make_pair(Key, NewInst));	}
		Locker.UnLock();
		}

		SInst* GetInstance(const StrImg& Key)
		{	//  SInst 只存储不主动创建对象，不存在时由使用者创建实例后添加进来
			SInst* pInst = NULL;
			tagMapInst::iterator itr = m_MapInst.find(Key);
			if(m_MapInst.end() != itr)
			{	pInst = itr->second;	}
			return pInst;
		}

		void Release(const StrImg& Key)
		{
		Locker.Lock();
			pInst = NULL;
			tagMapInst::iterator itr = m_MapInst.find(Key);
			if(m_MapInst.end() != itr)
			{
				delete itr->second;
				m_MapInst.erase(itr);
			}
		Locker.UnLock();
		}

		void ReleaseAll()
		{
		Locker.Lock();
			tagMapInst::iterator itr;
			for(itr=m_MapInst.begin();m_MapInst.end()!=itr;++itr)
			{
				delete itr->second;
				m_MapInst.erase(itr);
			}
		Locker.UnLock();
		}
	};

	template<class SInst>
	SLock SInstanceMap<SInst>::Locker;

	// ----------------------------------------------------------------------------------------------------------------------------

	// 记录子项详细信息
	struct tagItem
	{
		StrImg ItemName;
		StrImg ItemValue;
	};

	typedef map<String, String>		tagMapItem;
	typedef map<String, tagMapItem>	tagMapGroup;

	class SConfigInner
	{
		friend class SConfig;
		static SLock m_Inner_Lock;
		static Uint m_MaxConfigFileSize;
		tagMapGroup m_Inner_GroupMap;
		String m_Inner_FPath;
	private:
		static SInstanceMap<SConfig>* GetInstMap();
	private:
		SConfigInner(const StrImg& FilePath);
		bool Read(const StrImg& FPath);
		void Release(const StrImg& FilePath);
		bool Resolve();
		StrImg Get(const StrImg& GroupName,const StrImg& ItemName);
	};

	// SConfigInner	--------------------------------------------------------------------------------------------------------------------------------
	SLock SConfigInner::m_Inner_Lock;
	Uint SConfigInner::m_MaxConfigFileSize = 1024*1024*8; // 最多 8 M，对于一个配置文件来说这个大小已经足够，限制大小是为了防止恶意使用

	SConfigInner::SConfigInner(const StrImg& FilePath):m_Inner_FPath(FilePath)
	{	}

	SInstanceMap<SConfig>* SConfigInner::GetInstMap()
	{
		static SInstanceMap<SConfig>* pInstMap = NULL;
		if(!pInstMap)
		{	pInstMap = new SInstanceMap<SConfig>();	}
		return pInstMap;
	}

	StrImg SConfigInner::Get(const StrImg& GroupName,const StrImg& ItemName)
	{
		tagMapGroup::iterator itGroup = m_Inner_GroupMap.find(GroupName);
		if(m_Inner_GroupMap.end() != itGroup)
		{
			tagMapItem& theItem = itGroup->second;
			tagMapItem::iterator itItem = theItem.find(ItemName);
			if(theItem.end() != itItem)
			{	return itItem->second;	}
		}
		return SymbolConstant::imEmpty;
	}

	bool SConfigInner::Resolve()
	{
		const StrImg& FPath = m_Inner_FPath;
		// 解析文件内容，只解析 utf8 格式的文件
		STextFile sf(FPath, "utf8");
		if(STextFile::OpenNormal != sf.Open())
		{	return false;	}
		const Uint FSize = sf.Size();
		if(FSize > m_MaxConfigFileSize)
		{	return false;	} // throw "The config file's size is larger than 8M!";
		String strContent;
		Uint ReadSize = sf.Read(strContent, FSize);
		if(ReadSize+3 != FSize)
		{	return false;	} // throw "There's sth error in reading file.";
		sf.Close();

		StrImg CurrentGroup;
		StrImg CurrentRow;
		Uint tPos = 0;
		Uint sPos = 0;
		bool bBreak = false;
		StrImg LineMark("\n");
		if(TheEndPos != strContent.Find("\r\n"))
		{	LineMark.SetImg("\r\n");	}
		const Uint LMSize = LineMark.Size();
		char c = 0;
		Uint RowSize = 0;
		Uint EqualPos= 0;
		Uint i = 0, tmp = 0;
		tagMapItem theItemPair;
		const char* pRowHead = NULL;
		StrImg Key,Val;
		bool bRet = true;
		while(!bBreak)
		{	// 寻找换行符
			tPos = strContent.Find(LineMark,sPos);
			if(TheEndPos == tPos)
			{
				bBreak = true;
				tPos = strContent.Size(); // 最后一行无换行符的情况
				if(tPos <= sPos)
				{	break;	}
			}
			
			CurrentRow.SetImg(strContent.ToCStr()+sPos, tPos-sPos);
			CurrentRow.Trim(); // 去除左右的空白符
			if(CurrentRow.IsEmpty()) // 表示本行为空行
			{	goto CONTINUE;	}
			RowSize = CurrentRow.Size();
			c = CurrentRow.GetAt(0); // [] 必须出现在行的开头
			if('[' == c) // Group 在中括号中
			{
				tmp = CurrentRow.Find("]");
				if(TheEndPos == tmp)
				{	bRet = false; break;	} // throw "Lack of ]";
				CurrentGroup.SetImg(CurrentRow.ToCStr()+1, tmp-1);
				CurrentGroup.Trim();
				if(CurrentGroup.IsEmpty())
				{	bRet = false; break;	} // throw "The group name should not be empty.";
			}
			else if('#' == c) // "#"为注释行
			{	goto CONTINUE;	}
			else
			{	// the item
				EqualPos = CurrentRow.Find("="); // 寻找最左侧的等号
				if(TheEndPos == EqualPos)
				{	bRet = false; break;	} // throw "Lack of =";
				pRowHead = CurrentRow.ToCStr();
				Key.SetImg(pRowHead, EqualPos);
				tmp = CurrentRow.Find("#",EqualPos+1);
				if(TheEndPos != tmp)
				{	tmp = tmp-EqualPos-1;	}
				else
				{	tmp = RowSize-EqualPos-1;	}
				Val.SetImg(pRowHead+EqualPos+1, tmp);
				Key.Trim();
				Val.Trim();
				if(!Key.IsEmpty() && !Val.IsEmpty())
				{
					if(!CurrentGroup.IsEmpty())
					{
						tagMapGroup::iterator itr = m_Inner_GroupMap.find(CurrentGroup);
						if(m_Inner_GroupMap.end() != itr) // 已存在
						{	itr->second.insert(make_pair(Key, Val));	}
						else
						{
							theItemPair.clear();
							theItemPair.insert(make_pair(Key, Val));
							m_Inner_GroupMap.insert(make_pair(CurrentGroup,theItemPair));
						}
					}
				}
			}
CONTINUE:
			sPos = tPos + LMSize;
			continue;
		}
		return bRet;
	}


#define pInner		static_cast<SConfigInner*>(m_pCfgR)
#define m_Lock		SConfigInner::m_Inner_Lock
#define m_CurrFPath	pInner->m_Inner_FPath
#define m_ReadSize	pInner->m_Inner_ReadSize

	// SConfig	-----------------------------------------------------------------------------------------

	SConfig::SConfig(const StrImg& FilePath)
	{	m_pCfgR = new SConfigInner(FilePath);	}

	SConfig* SConfig::GetInstance(const StrImg& FilePath)
	{
		static SInstanceMap<SConfig> * pInstMap = SConfigInner::GetInstMap();
		SConfig* pCfg = pInstMap->GetInstance(FilePath);
		if(!pCfg)
		{
		m_Lock.Lock();
			if(!pCfg)
			{
				pCfg = new SConfig(FilePath);
				if(static_cast<SConfigInner*>(pCfg->m_pCfgR)->Resolve())
				{	pInstMap->Add(FilePath, pCfg);	}
				else
				{
					delete pCfg;
					pCfg = NULL;
				}
			}
		m_Lock.UnLock();
		}
		return pCfg;
	}

	void SConfig::Release(const StrImg& FilePath)
	{	SConfigInner::GetInstMap()->Release(FilePath);	}

	StrImg SConfig::Get(const StrImg& GroupName,const StrImg& ItemName)
	{	return pInner->Get(GroupName, ItemName);	}


}; // namespace


/*	void SConfig::Add(const StrImg& GroupName,const StrImg& ItemName,const StrImg& ItemValue)
	{
	m_Lock.Lock();
		MapGroup& rGroup = m_pGroup;
		MapGroup::iterator itr = m_Inner_GroupMap.find(GroupName);
		if(m_Inner_GroupMap.end() != itr)
		{
			MapItem::iterator it = itr->second.find(ItemName);
			if(itr->second.end() != it) // 有此条目
			{	it->second = ItemValue;	}
			else // 无此条目
			{	itr->second.insert(make_pair(ItemName,ItemValue));	}
		}
		else
		{
			MapItem item;
			item.insert(make_pair(ItemName,ItemValue));
			m_Inner_GroupMap.insert(make_pair(GroupName,item));
		}
	m_Lock.UnLock();
	}

	void SConfig::Write()
	{
	m_Lock.Lock();
		// 写带 BOM 的 Utf8 文件
		SARRAY(StrContent, 1024*32);
		StrContent.Append(Utf8_Bom_Mark);
		MapGroup::iterator itr;
		MapItem::iterator it2;
		for(itr=m_Inner_GroupMap.begin();m_Inner_GroupMap.end()!=itr;++itr)
		{
			StrContent.AppendC('[');
			StrContent.Append(itr->first);
			StrContent.AppendC(']');
			StrContent.AppendC('\n');
			for(it2=itr->second.begin();itr->second.end()!=it2;++it2)
			{
				StrContent.Append(it2->first);
				StrContent.AppendC('=');
				StrContent.Append(it2->second);
				StrContent.AppendC('\n');
			}
		}
		SFile sf(m_imFPath);
		if(sf.Open("w+"))// 打开文件并清除内容，没有则创建
		{	// Utf8 格式
			StrContent.Append(Utf8_Bom_Mark);
			sf.Write(StrContent);
		}
	m_Lock.UnLock();
	}
*/
