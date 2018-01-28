#ifndef HAPPIEME_SFILE
#define HAPPIEME_SFILE
#include "../SString/String.h"

namespace smm
{
	struct tagFileInfo
	{
		Uint	FileSize;
		TimeT	AccessTime; // 访问时间
		TimeT	ModifyTime; // 修改时间
		TimeT	CreateTime; // 创建时间
		String FilePath;
		Uint	TypePos;
	};

	class SMDLLX SFile : public SNonCopy
	{
	public:
		enum FilePosType { SFILE_HEAD=0 , SFILE_CURRENT , SFILE_TAIL };
		// 一个文件对象只针对一个文件
		SFile(const StrImg& FPath); // EnCoding 不区分大小写
		~SFile( );
		// open
		bool Open(const StrImg& OpenMode = "rb+"); //  默认为二进制读写
		// State
		bool IsEnd( );
		bool IsErr( );
		// Close
		void Close( );
		void Flush( );
		// Position
		void SetPos(int OffSet, FilePosType FromWhere=SFile::SFILE_HEAD);
		Uint GetPos( );
		// BOM 头
		Uint CheckFileType();
		// FileInfo
		const tagFileInfo& GetFileInfo( );
		Uint Size();
		// Read and Write
		Uint Read ( void * pDstBuf,Uint DstSizeInByte,Uint StructCount,Uint StructSize=1);
		Uint Read ( String& DstStr , Uint ReadSize=1024*8); // 缺省为 8K
		Uint ReadLine (String& DstStr); // 读 1 行
		// 在读之后写要先 Close 再 Open
		Uint Write( const void * pSrcStuct,Uint StructSize,Uint StructCount);
		Uint Write( const StrImg& imStr);
	protected:
		void* m_pFileR;
	}; // class SFile

	SCSTRIMG( Utf8_Bom_Mark, "\xEF\xBB\xBF");
	SCSTRIMG(Ucs2L_Bom_Mark, "\xFE\xFF");
	SCSTRIMG(Ucs2B_Bom_Mark, "\xFF\xFE");
	SCSTRIMG(Ucs4L_Bom_Mark, "\xFF\xFE\x00\x00");
	SCSTRIMG(Ucs4B_Bom_Mark, "\x00\x00\xFE\xFF");

	class SMDLLX STextFile : public SFile
	{
	private:
		StrImg m_EnCoding;
	public:	// 文本文件常用的编码：utf8,ucs2L,ucs2B,ucs4L,ucs4B，不区分大小写
		enum OpenStatusType { OpenNormal, OpenFailed, OpenFailEncoded };
		STextFile(const StrImg& FPath , const StrImg& EnCoding="utf8");
		// 返回结果: OpenNormal-打开成功且编码验证通过，OpenFailed-打开失败，OpenFailEncoded-打开但编码验证失败
		OpenStatusType Open(const StrImg& OpenMode = "rb+");
		bool SizeValidate();
	};

	class SMDLLX SFilePath : public SNonCopy
	{
	public:
		SFilePath(const StrImg& path);
		StrImg GetFileName();
		StrImg GetParentDir();
	private:
		void* m_pFilePathR;
	};

	class SMDLLX SFileShell : public SNonCopy
	{
	public:
		static bool MakeDir	( const StrImg& DirPath, int mode=0); // mode is for linux
		// mode, 00 : Existence only; 02 : Write permission; 04 : Read permission; 06 : Read and write permission
		static bool Exist	( const StrImg& Path);
		static bool CanRead ( const StrImg& Path);
		static bool CanWrite( const StrImg& Path);
		static bool CanReadWrite(const StrImg& Path);
	}; // class SFileShell
}; // namespace

#endif
