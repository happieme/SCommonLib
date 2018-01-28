// SZlib.cpp : 定义 DLL 应用程序的导出函数。
#include "../ZLib/zlib.h"
#include "SGZip.h"
#include "../SCommon/SIOStd.h"

namespace smm
{
	namespace SCompress
	{
		bool SGZip(const StrImg& imSrc , String& strDst)
		{
			const char* pSrc = imSrc.ToCStr();
			Uint srcLen = imSrc.Size();

			z_stream strm;
			//初始化strm结构中的zalloc, zfree, opaque,要求使用默认的内存分配策略
			strm.zalloc = Z_NULL;
			strm.zfree  = Z_NULL;
			strm.opaque = Z_NULL;

			//设置输入输出缓冲区
			strm.avail_in = srcLen;
			strm.next_in = (Bytef *)pSrc;

			bool bRet = false;
			int Res = -1;
			// 初始化zlib的状态，成功返回 Z_OK
			// deflateInit  : zlib 格式 , 		// err = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
			// deflateInit2 : gzip 格式
			Res = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS+16, 9, Z_DEFAULT_STRATEGY);
			if (Res == Z_OK)
			{	// Z_FINISH表明完成输入，让deflate()完成输出
				const Uint dstLen = deflateBound(&strm,srcLen);
				char * pDst = NULL;
				bool bNew = true;
				if(strDst.Size() >= dstLen)
				{
					pDst = const_cast<char*>(strDst.ToCStr());
					bNew = false;
				}
				else
				{	pDst = new char[dstLen];}
				strm.next_out = (Bytef *)pDst;
				strm.avail_out = dstLen;
				Res = deflate(&strm, Z_FINISH);
				// Z_STREAM_END表明所有的输入都已经读完，所有的输出也都产生
				if (Res == Z_STREAM_END)
				{
				//	cout << "compression succed, before compression size is " << strm.total_in << " after compressione size is " << dstLen - strm.avail_out << endl;
					//strm.avail_out表明输出缓冲区剩余的空闲空间大小
					pDst[strm.total_out] = 0;
					bRet = true;
					if(bNew)
					{	strDst.TakeOver(pDst,strm.total_out,dstLen);	}
				}
				else
				{
					if(bNew)
					{	delete [] pDst;	}
				}
			}
			deflateEnd(&strm);	//deflateEnd释放资源，防止内存泄漏
			return bRet;
		}

		bool IsSGZiped(const StrImg& imSrc)
		{
			if(imSrc.Size() < 10)
			{	return false;	}

			static const char pTag[2] = { char(0x1F) , char(0x8B) };
			if(pTag[0]==imSrc.GetAt(0) && pTag[1]==imSrc.GetAt(1))
			{	return true;	}
			return false;
		}

		bool UnSGZip(const StrImg& imSrc , String& strDst)
		{
			const char* pSrc = imSrc.ToCStr();
			Uint srcLen = imSrc.Size();

			z_stream strm;
			strm.zalloc = NULL;
			strm.zfree = NULL;
			strm.opaque = NULL;

			strm.avail_in = srcLen;
			strm.next_in = (Bytef *)pSrc;

			int Res = -1;
			bool bRet = false;
			Res = inflateInit2(&strm, MAX_WBITS+16);
			if (Res == Z_OK)
			{
				const Uint* pSize = reinterpret_cast<const Uint*>(imSrc.ToCStr()+srcLen-4);
				const Uint dstLen = *pSize+1;

				char * pDst = NULL;
				bool bNew = true;
				if(strDst.Size() >= dstLen)
				{
					pDst = const_cast<char*>(strDst.ToCStr());
					bNew = false;
				}
				else
				{	pDst = new char[dstLen];}

				strm.next_out = (Bytef *)pDst;
				strm.avail_out = dstLen;
				Res = inflate(&strm, Z_FINISH);
				if(Res == Z_STREAM_END)
				{	//	cout << "decompress succeed, before decompress size is " << strm.total_in  << " after decompress size is " << strm.total_out << endl;
					pDst[strm.total_out] = 0;
					bRet = true;
					if(bNew)
					{	strDst.TakeOver(pDst,strm.total_out,dstLen);	}
				}
				else
				{
					if(bNew)
					{	delete [] pDst;	}
				}
			}
			inflateEnd(&strm);
			return bRet;
		}
	}
}; // namespace
