#pragma once
#include "afx.h"
class CRecord
{
private:
//	log4cplus::Logger m_logger;
private:
	BOOL IsMaxFileSize(const CString & path,int nSize = 3);  //3MB

	BOOL writeExceptionFile(const CString & str,const CString & path = _T("./Log/MyRecord/ExceptionRecord.txt"),const int & nSize = 3);
public:

	BOOL writeAssignmentFile(const CString & str, const CString & path = _T("./Log/MyRecord/AssignmentThreadRecord.txt"),const int & nSize = 3); //记录线程分配信息

	BOOL writeUploadFile(const CString & str, const CString & path = _T("./Log/MyRecord/UploadFileRecord.txt"),const int & nSize = 3);			//记录文件上传信息

	BOOL writeHttpFile(const CString & str, const CString & path = _T("./Log/MyRecord/HttpReportRecord.txt"),const int & nSize = 3);				//记录http上报信息

	BOOL writeCommonFile(const CString & str, const CString & path = _T("./Log/MyRecord/CommonRecord.txt"),const int & nSize = 3);				//记录通用信息

	BOOL writeMSG(const CString & str,const CString const& path = _T("./Log/MyRecord/MSGRecord.txt"),const int & nSize = 3);					//记录信息

	BOOL GetFilePathParam(const CString & cstrFullPath,CString & ctrPath, CString & cstrName,CString & cstrExtName); // 扩展名cstrExtName 为.txt  \ .jpg 带 ‘.’

	~CRecord(void);
	CRecord(void);

private:

	CRITICAL_SECTION m_csAssignment;

	CRITICAL_SECTION m_csUploadRecord;

	CRITICAL_SECTION m_csHttpReport;

	CRITICAL_SECTION m_csCheck;

	CRITICAL_SECTION m_csException;

	CRITICAL_SECTION m_csCommon;

	CRITICAL_SECTION m_csMSG;

	CStdioFile m_UploadFile;			//文件上传

	CStdioFile m_AssignmentFile;		//线程分配

	CStdioFile m_HttpReportFile;		//http上报

	CStdioFile m_CommonFile;			//通用记录

	CStdioFile m_MSGFile;				//MSG 记录

	CStdioFile m_ExceptionFile;			//日志记录过程产生的异常
	
	CStdioFile m_CheckFile;				//检测文件大小

public:
	CString m_strFilePath;

	int m_nSize;

	int m_nLogType;
};

