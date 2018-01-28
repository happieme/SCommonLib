#include "SJson.h"

namespace smm
{
/*	SJsonObject::SJsonObject()
	{	pM = new DObj;	}

	void SJsonObject::Add(const StrImg& Key, const StrImg& Val)
	{
		if(!Val.IsNull())
		{
			SData* pData = new SData;
			pData->pVal = new String(Val);
			pData->type = 0;
			pM->insert(make_pair(Key,pData));
		}
	}

	void SJsonObject::Add(const StrImg& Key, SJsonObject* pVal)
	{
		if(pVal)
		{
			SData* pData = new SData();
			pData->pVal = pVal;
			pData->type = 1;
			pM->insert(make_pair(Key,pData));
		}
	}

	void SJsonObject::Add(const StrImg& Key, SJsonArray* pVal)
	{
		if(pVal)
		{
			SData* pData = new SData();
			pData->pVal = pVal;
			pData->type = 2;
			pM->insert(make_pair(Key,pData));
		}
	}

	String SJsonObject::ToString()
	{
		String ret;
		DObj::iterator itr = pM->begin();
		UInt type;
		void* pVal;
		ret +=	"{";
		for(;pM->end()!=itr;++itr)
		{
			ret +=	"\"";
			ret +=	itr->first;
			ret +=	"\":";
			type = itr->second->type;
			pVal = itr->second->pVal;
			switch(type)
			{
			case 0:// string
				ret +=	"\"";
				ret +=	*(reinterpret_cast<String*>(pVal));
				ret +=	"\"";
				break;
			case 1: // object
				ret +=	reinterpret_cast<SJsonObject*>(pVal)->ToString();
				break;
			case 2:
				ret +=	reinterpret_cast<SJsonArray*>(pVal)->ToString();
				break;
			default:
				break;
			}
			ret +=	",";
		}
		ret.SetAt(ret.Size()-1,'}');
		return ret;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------------

	SJsonArray::SJsonArray()
	{	pA = new DArr;	}

	void SJsonArray::Add(const StrImg& Val)
	{
		if(!Val.IsNull())
		{
			SData* pData = new SData();
			pData->pVal = new String(Val);
			pData->type = 0;
			pA->push_back(pData);
		}
	}

	void SJsonArray::Add(SJsonObject* pVal)
	{
		if(pVal)
		{
			SData* pData = new SData();
			pData->pVal = pVal;
			pData->type = 1;
			pA->push_back(pData);
		}
	}

	void SJsonArray::Add(SJsonArray* pVal)
	{
		if(pVal)
		{
			SData* pData = new SData();
			pData->pVal = pVal;
			pData->type = 2;
			pA->push_back(pData);
		}
	}

	String SJsonArray::ToString()
	{
		String ret;
		DArr::iterator itr = pA->begin();
		UInt type;
		void* pVal;
		ret +=	"[";
		for(;pA->end()!=itr;++itr)
		{
			type = (*itr)->type;
			pVal = (*itr)->pVal;
			switch(type)
			{
			case 0:// string
				ret +=	"\"";
				ret +=	*(reinterpret_cast<String*>(pVal));
				ret +=	"\"";
				break;
			case 1: // object
				ret +=	reinterpret_cast<SJsonObject*>(pVal)->ToString();
				break;
			case 2:
				ret +=	reinterpret_cast<SJsonArray*>(pVal)->ToString();
				break;
			default:
				break;
			}
		ret +=	",";
		}
		ret.SetAt(ret.Size()-1,']');
	return ret;
	}
*/
};