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

	BOOL writeAssignmentFile(const CString & str, const CString & path = _T("./Log/MyRecord/AssignmentThreadRecord.txt"),const int & nSize = 3); //��¼�̷߳�����Ϣ

	BOOL writeUploadFile(const CString & str, const CString & path = _T("./Log/MyRecord/UploadFileRecord.txt"),const int & nSize = 3);			//��¼�ļ��ϴ���Ϣ

	BOOL writeHttpFile(const CString & str, const CString & path = _T("./Log/MyRecord/HttpReportRecord.txt"),const int & nSize = 3);				//��¼http�ϱ���Ϣ

	BOOL writeCommonFile(const CString & str, const CString & path = _T("./Log/MyRecord/CommonRecord.txt"),const int & nSize = 3);				//��¼ͨ����Ϣ

	BOOL writeMSG(const CString & str,const CString const& path = _T("./Log/MyRecord/MSGRecord.txt"),const int & nSize = 3);					//��¼��Ϣ

	BOOL GetFilePathParam(const CString & cstrFullPath,CString & ctrPath, CString & cstrName,CString & cstrExtName); // ��չ��cstrExtName Ϊ.txt  \ .jpg �� ��.��

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

	CStdioFile m_UploadFile;			//�ļ��ϴ�

	CStdioFile m_AssignmentFile;		//�̷߳���

	CStdioFile m_HttpReportFile;		//http�ϱ�

	CStdioFile m_CommonFile;			//ͨ�ü�¼

	CStdioFile m_MSGFile;				//MSG ��¼

	CStdioFile m_ExceptionFile;			//��־��¼���̲������쳣
	
	CStdioFile m_CheckFile;				//����ļ���С

public:
	CString m_strFilePath;

	int m_nSize;

	int m_nLogType;
};

