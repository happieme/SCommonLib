#include "../SString/String.h"
#include "../SCommon/SClib.h"
#include "SFile.h"
#include "../SCommon/SIOStd.h"
#include "../SString/SymbolConstant.h"

namespace smm
{
	// ---------------------------------------------------------------------------------------------------
	class SFileInner
	{
		friend class SFile;
	private:
		FILE * m_Inner_pFile;
		bool m_Inner_bOpen;
		tagFileInfo m_Inner_FileInfo;
	private:
		SFileInner(const StrImg& FPath)
		: m_Inner_pFile(NULL) , m_Inner_bOpen(false)
		{	m_Inner_FileInfo.FilePath = FPath;	}
		void GetFileInfo_I();
	};

	void SFileInner::GetFileInfo_I()
	{
		struct FILESTATE FInfoStat;
		if(0 == SClib::FileGetState(m_Inner_FileInfo.FilePath.ToCStr(), FInfoStat))
		{
			m_Inner_FileInfo.FileSize = FInfoStat.st_size;
			m_Inner_FileInfo.AccessTime = FInfoStat.st_atime;
			m_Inner_FileInfo.CreateTime = FInfoStat.st_ctime;
			m_Inner_FileInfo.ModifyTime = FInfoStat.st_mtime;
			// 获取文件类型
			m_Inner_FileInfo.TypePos = m_Inner_FileInfo.FilePath.RFind(SymbolConstant::imDot);
/*			if(TheEndPos != DotPos)
			{
				m_Inner_FileInfo.FileType.SetImg( m_Inner_FileInfo.FilePath.ToCStr()+DotPos+1,
											m_Inner_FileInfo.FilePath.Size()-DotPos-1);
			}
*/		}
	}

	// SFile	------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define pFileInner	static_cast<SFileInner*>(m_pFileR)
#define m_pFile		pFileInner->m_Inner_pFile
#define m_bOpen		pFileInner->m_Inner_bOpen
#define m_FileInfo	pFileInner->m_Inner_FileInfo

	SFile::SFile(const StrImg& FPath) : m_pFileR(new SFileInner(FPath))
	{	}

	SFile::~SFile()
	{
		Close();
		delete pFileInner;
	}

	bool SFile::Open(const StrImg& OpenMode)
	{
		if(m_bOpen) // 必须 Close 之后才能再 Open
		{	return m_bOpen;	}
		const char* pFilePath = m_FileInfo.FilePath.ToCStr();
		if(0 == SClib::FileOpen(&m_pFile,pFilePath,OpenMode.ToCStr()))
		{
			m_bOpen = true;
			pFileInner->GetFileInfo_I();
		}
		else
		{	m_bOpen = false;	}
		return m_bOpen;
	}

	bool SFile::IsEnd()
	{
		bool ret = true;
		if(m_bOpen)
		{	ret = 0!=SClib::IsFileEnd(m_pFile);	}
		else
		{	throw "SFile::IsEnd() Error! Try open first!";	}
		return ret;
	}

	bool SFile::IsErr()
	{
		bool ret = true;
		if(m_bOpen)
		{	ret = 0!=SClib::IsFileError(m_pFile);	}
		else
		{	throw "SFile::IsErr() Error! Try open first!";	}
		return ret;
	}

	void SFile::Close()
	{
		if(m_pFile && m_bOpen)
		{
			if(0!=SClib::FileClose(m_pFile))
			{
				// warning throw "SFileInner::Close()\n关闭时出错！";
			}
			m_bOpen = false;
		}
		else
		{
			m_pFile = NULL;	
			m_bOpen = false;
		}
	}

	void SFile::Flush( )
	{
		if(m_pFile)
		{	SClib::FileFlush(m_pFile);	}
	}

	const tagFileInfo& SFile::GetFileInfo( )
	{	return m_FileInfo;	}

	Uint SFile::Size( )
	{	return m_FileInfo.FileSize;	}

	void SFile::SetPos(int OffSet , FilePosType FromWhere)
	{
		if(m_bOpen)
		{
			if(0 != SClib::FileSeek(m_pFile,OffSet,FromWhere))
			{	throw "SFile::SetPos(int OffSet , FilePosType FromWhere) Error! fseek error!";	}
		}
		else
		{	throw "SFile::SetPos(int OffSet , FilePosType FromWhere) Error! Try open first!";	}
	}

	Uint SFile::GetPos( )
	{
		Uint ret = TheEndPos;
		if(m_bOpen) // ftell 出错时返回 -1,Uint 为 TheEndPos
		{	ret = SClib::FileTell(m_pFile);	}
		else
		{	throw "SFile::GetPos() Error! Try open first!";	}
		return ret;
	}

	Uint SFile::Read (void *pDstBuf,Uint BufSize,Uint StructCount,Uint StructSize)
	{
		Uint ResSize = 0;
		if(m_bOpen)
		{
			ResSize = StructSize*StructCount;
			if(pDstBuf && BufSize >= ResSize)
			{	ResSize = SClib::FileRead(pDstBuf,BufSize,StructSize,StructCount,m_pFile);	}
			else
			{	throw "SFile::Read (void *pDstBuf,Uint BufSize,Uint StructCount,Uint StructSize) Error! BufSize should >= StructSize*StructCount";	}
		}
		else
		{	throw "SFile::Read (void *pDstBuf,Uint BufSize,Uint StructCount,Uint StructSize) Error! Try open first!";	}
		return ResSize;
	}

	Uint SFile::Read ( String& DstStr , Uint ReadSize)
	{
		if(ReadSize > Size())
		{	ReadSize = Size();	}
		char * pBuf = NULL;
		bool bNew = true;
		if(DstStr.Size() >= ReadSize)
		{
			pBuf = const_cast<char*>(DstStr.ToCStr());
			bNew = false;
		}
		else
		{	pBuf = new char[ReadSize+1];	} // 多申请一个，用于存储结束符
		Uint Res = Read(pBuf , ReadSize , ReadSize);
		pBuf[Res] = 0; // 这里重新设置 m_Size
		if(bNew)
		{
			if(0 != Res)
			{	DstStr.TakeOver(pBuf,Res,ReadSize+1);	}
			else
			{	delete [] pBuf;	}
		}
		return Res;
	}

	Uint SFile::Write(const void* pSrcBuf,Uint StructSize,Uint StructCount)
	{
		Uint ret = TheEndPos;
		if(m_bOpen)
		{
			if(pSrcBuf)
			{
				ret = SClib::FileWrite(pSrcBuf,StructSize,StructCount,m_pFile);
				m_FileInfo.FileSize += ret; // 更新 FileSize
			}
		}
		return ret;
	}

	Uint SFile::ReadLine(String& DstStr) // 读 1 行
	{
		const Uint size = 1024 * 10;
		SARRAY(tmp, size);
		Uint read_count = -1;
		Uint last_pos = 0;
		while (1)
		{
			last_pos = GetPos();
			read_count = Read(tmp, size);
			if (read_count > 0) //GetPos()<Size()
			{
				Uint pos = tmp.Find("\n");
				if (pos != TheEndPos)
				{
					SetPos(last_pos + pos+1);
					DstStr = tmp.Sub(0, pos);
					Uint lastone = DstStr.Size() - 1;
					if ('\r' == DstStr.GetAt(lastone))
					{
						DstStr.Delete(lastone);
					}
				}
				else if(GetPos() == Size())
				{
					DstStr = tmp.Sub(0, pos);
				}
				tmp.Clear();
				break;
			}
			else
			{
				break;
			}
		}
		return DstStr.Size();
	}

	Uint SFile::Write ( const StrImg& SrcStr)
	{	return Write(SrcStr.ToCStr(),1,SrcStr.Size());	}

	// SFilePath	-------------------------------------------------------------------------------------
#define pFilePathInner	static_cast<SFilePathInner*>(m_pFilePathR)
	class SFilePathInner 
	{
	private:
		String path_;
		friend class SFilePath;
	};
	SFilePath::SFilePath(const StrImg& path)
	{
		m_pFilePathR = new SFilePathInner;
		pFilePathInner->path_ = path;
		pFilePathInner->path_.Replace("\\","/");
	}

	StrImg SFilePath::GetFileName()
	{
		const StrImg& str = pFilePathInner->path_;
		Uint pos = str.RFind("/");
		StrImg res;
		if (TheEndPos != pos)
		{
			res = str.Sub(pos+1);
		}
		return res;
	};

	// STextFile	-------------------------------------------------------------------------------------
	STextFile::STextFile(const StrImg& FPath , const StrImg& EnCoding)
	: SFile(FPath),m_EnCoding(EnCoding)
	{	}

	// 返回结果: OpenNormal-打开成功且编码验证通过，OpenFailed-打开失败，OpenFailEncoded-打开但编码验证失败
	STextFile::OpenStatusType STextFile::Open(const StrImg& OpenMode)
	{
		static const StrImg Arr[] = { "utf8" , "ucs2L" , "ucs2B" , "ucs4L" , "ucs4B" };
		OpenStatusType Ret = OpenFailed;
		if(SFile::Open(OpenMode))
		{
			Ret = OpenFailEncoded;
			char BomHead[5] = {0};
			Uint Res = 0;
			SetPos(SFILE_HEAD);
			if(m_EnCoding.IsEqual(Arr[0])) // utf8
			{
				Res = Read(BomHead,5,3);
				if(3 == Res && Utf8_Bom_Mark.IsEqual(BomHead))	{	Ret = OpenNormal;	SetPos(Res);	}
			}
			else if(m_EnCoding.IsEmpty()) // 空，即未指定，按 Ansi 处理
			{	Ret = OpenNormal;	} // 不用处理
			else if(m_EnCoding.IsEqual(Arr[1])) // ucs2L
			{
				Res = Read(BomHead,5,2);
				if(2 == Res || Ucs2L_Bom_Mark.IsEqual(BomHead)){	Ret = OpenNormal;	SetPos(Res);	}
			}
			else if(m_EnCoding.IsEqual(Arr[2])) // ucs2B
			{
				Res = Read(BomHead,5,2);
				if(2 == Res || Ucs2B_Bom_Mark.IsEqual(BomHead)){	Ret = OpenNormal;	SetPos(Res);	}
			}
			else if(m_EnCoding.IsEqual(Arr[3])) // ucs4L
			{
				Res = Read(BomHead,5,4);
				if(4 == Res || Ucs4L_Bom_Mark.IsEqual(BomHead)){	Ret = OpenNormal;	SetPos(Res);	}
			}
			else if(m_EnCoding.IsEqual(Arr[4])) // ucs4B
			{
				Res = Read(BomHead,5,4);
				if(4 == Res || Ucs4B_Bom_Mark.IsEqual(BomHead)){	Ret = OpenNormal;	SetPos(Res);	}
			}
			else
			{	Ret = OpenFailEncoded;	}
		}
		return Ret;
	}

	// SFileShell	-------------------------------------------------------------------------------------
	bool SFileShell::MakeDir(const StrImg& DirPath,int mode)
	{	return SClib::MakeDir(DirPath.ToCStr(),mode);	}

	bool SFileShell::Exist	 (const StrImg& Path)
	{	return 0 == SClib::FileAccess(Path.ToCStr(),0x0);	}

	bool SFileShell::CanWrite(const StrImg& Path)
	{	return 0 == SClib::FileAccess(Path.ToCStr(),0x02);	}

	bool SFileShell::CanRead (const StrImg& Path)
	{	return 0 == SClib::FileAccess(Path.ToCStr(),0x04);	}

	bool SFileShell::CanReadWrite(const StrImg& Path)
	{	return 0 == SClib::FileAccess(Path.ToCStr(),0x06);	}
}; // namespace
