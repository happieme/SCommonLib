#ifndef HAPPIEME_SJSON
#define HAPPIEME_SJSON
#include "../SString/String.h"

namespace smm
{
	class SMDLLX SJson
	{
	public:
		SJson(const StrImg& strJson="");
		SJson(const SJson& js);
		~SJson(); // 凡是有路径输入的函数都应考虑相对路径 BasePath ？？？
		StrImg BasePath(const StrImg& path="");// ？？？为叶子节点的情况 ？？？
		Uint BaseSize(); // 当前节点下的子节点的个数，[]或{} 下的对象个数 // ？？？叶子节点的情况
		void Init(const StrImg& strJson); // 只能是“{}”或“[]”，初始时调用，无需调用 In()
		void Add(const StrImg& val); // 只作 val 增加
		void Add(const StrImg& Key, const StrImg& val);
		void Add(const SJson& js);// ？？？
		void Add(const StrImg& key, const SJson& js);
		void Delete(const StrImg& path);// ？？？
		void Set(const StrImg& path, const StrImg& value);
		void Set(const SJson& js);//？？？
		StrImg Get(const StrImg& path); // 空表示当前的 []或{} 下，* 表示根节点，this 表示当前节点
		void In();
		void Out();
		void Clear();
		bool IsValid();
		StrImg ToStr(bool bFormatted=false); // 提供输出格式化的形式
		void FromStr(const StrImg& str);
		void Swap(SJson& js);
	private:
		void* m_pJsonR;
	};
}; // namespace

#endif
