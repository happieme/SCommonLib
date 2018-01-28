#include "SCrypt.h"

namespace smm
{
	void SCrypt::EnCrypt(const StrImg& Src , String& Dst)
	{
		String Tmp;
		Base64_Encode(Src , Tmp);
		Dst.Clear();
		const Uint TmpSize = Tmp.Size();
		Byte Tar = 0;
		for(Uint i=0;i<TmpSize;++i)
		{
			if(0 == i%2)
			{	Tar = 1;	}
			Dst.AppendC(Tmp[i] ^ Tar);
			Tar = 0;
		}
	}

	// 每两个字节组合为一个字节
	void SCrypt::DeCrypt(const StrImg& Src , String& Dst)
	{
		String Tmp;
		const Uint SrcSize = Src.Size();
		Tmp.ReSize(SrcSize);
		Byte Tar = 0;
		for(Uint i=0;i<SrcSize;++i)
		{
			if(0 == i%2)
			{	Tar = 1;	}
			Tmp.AppendC(Src[i] ^ Tar);
			Tar = 0;
		}
		Base64_Decode(Tmp , Dst);
	}
}; // namespace
