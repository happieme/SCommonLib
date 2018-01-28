// SZlib.cpp : ���� DLL Ӧ�ó���ĵ���������
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
			//��ʼ��strm�ṹ�е�zalloc, zfree, opaque,Ҫ��ʹ��Ĭ�ϵ��ڴ�������
			strm.zalloc = Z_NULL;
			strm.zfree  = Z_NULL;
			strm.opaque = Z_NULL;

			//�����������������
			strm.avail_in = srcLen;
			strm.next_in = (Bytef *)pSrc;

			bool bRet = false;
			int Res = -1;
			// ��ʼ��zlib��״̬���ɹ����� Z_OK
			// deflateInit  : zlib ��ʽ , 		// err = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
			// deflateInit2 : gzip ��ʽ
			Res = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS+16, 9, Z_DEFAULT_STRATEGY);
			if (Res == Z_OK)
			{	// Z_FINISH����������룬��deflate()������
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
				// Z_STREAM_END�������е����붼�Ѿ����꣬���е����Ҳ������
				if (Res == Z_STREAM_END)
				{
				//	cout << "compression succed, before compression size is " << strm.total_in << " after compressione size is " << dstLen - strm.avail_out << endl;
					//strm.avail_out�������������ʣ��Ŀ��пռ��С
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
			deflateEnd(&strm);	//deflateEnd�ͷ���Դ����ֹ�ڴ�й©
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
