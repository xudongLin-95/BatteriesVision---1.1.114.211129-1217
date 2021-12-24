#include "StdAfx.h"
#include "Record.h"
#include<sys/types.h> 
#include<sys/stat.h>

CRecord::CRecord(void) : m_strFilePath(_T(""))
	,m_nSize(3)
	,m_nLogType(1)
{
	InitializeCriticalSection(&m_csUploadRecord);

	InitializeCriticalSection(&m_csAssignment);

	InitializeCriticalSection(&m_csHttpReport);

	InitializeCriticalSection(&m_csCheck);

	InitializeCriticalSection(&m_csException);

	InitializeCriticalSection(&m_csCommon);

	InitializeCriticalSection(&m_csMSG);

//	log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(".\\Config\\SCIMES\\mesLog.properties")); 

//	m_logger = log4cplus::Logger::getRoot(); 

}

CRecord::~CRecord(void)
{
	DeleteCriticalSection(&m_csUploadRecord);

	DeleteCriticalSection(&m_csAssignment);

	DeleteCriticalSection(&m_csHttpReport);

	DeleteCriticalSection(&m_csCheck);

	DeleteCriticalSection(&m_csException);

	DeleteCriticalSection(&m_csCommon);

	DeleteCriticalSection(&m_csMSG);
}
/*  

BOOL CRecord::writeMSG(const CString &str ,const CString &  path,const int & nSize)
{
	if (m_nLogType < SaveAllLog)
	{
		return FALSE;
	}

	CString cstrMessageWrite,cstrExceptionMessage,cstrTime;

	CString cstrFilePath,cstrName,cstrNewName,cstrExtName,cstrNewFileFullName;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	cstrTime.Format(_T("%04d/%02d/%02d-%02d:%02d:%02d.%03d	"),
		currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds);

	cstrMessageWrite.Format(_T("%s%s\n"),cstrTime,str);

	if ( !GetFilePathParam(path,cstrFilePath,cstrName,cstrExtName))
	{
		cstrExceptionMessage.Format(_T("%swriteMSG文件路径解析失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	if (!PathFileExists(cstrFilePath))
	{
		CreateMultilevelPath(cstrFilePath);
	}

	if (str == _T(""))
	{
		return FALSE;
	}
	if (!IsMaxFileSize(path,nSize))
	{
		cstrExceptionMessage.Format(_T("%swriteMSG检测文件大小失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return	FALSE;
	}	

	EnterCriticalSection(&m_csMSG);

	try
	{
		if(!m_MSGFile.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate))
		{
			LeaveCriticalSection(&m_csMSG);

			cstrExceptionMessage.Format(_T("%swriteMSG文件打开失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

			writeExceptionFile(cstrExceptionMessage);

			return FALSE;
		}

		m_MSGFile.SeekToEnd();

		m_MSGFile.Write(cstrMessageWrite.GetBuffer(cstrMessageWrite.GetLength()),cstrMessageWrite.GetLength());

		cstrMessageWrite.ReleaseBuffer();

		m_MSGFile.Close();
	}
	catch (...)
	{
		m_MSGFile.Close();
		LeaveCriticalSection(&m_csMSG);

		cstrExceptionMessage.Format(_T("%s文件写入失败！\t%s\n写入信息：%s\n"),cstrTime,path,cstrMessageWrite);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	LeaveCriticalSection(&m_csMSG);

	return TRUE;
}

BOOL CRecord::writeAssignmentFile(const CString & str,const CString & path ,const int & nSize)
{
	if (m_nLogType < SaveThreadLog)
	{
		return FALSE;
	}

	CString cstrMessageWrite,cstrExceptionMessage,cstrTime;

	CString cstrFilePath,cstrName,cstrNewName,cstrExtName,cstrNewFileFullName;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	cstrTime.Format(_T("%04d/%02d/%02d-%02d:%02d:%02d.%03d	"),
		currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds);
	
	cstrMessageWrite.Format(_T("%s%s\n"),cstrTime,str);


	if ( !GetFilePathParam(path,cstrFilePath,cstrName,cstrExtName))
	{
		cstrExceptionMessage.Format(_T("%swriteAssignmentFile文件路径解析失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	if (!PathFileExists(cstrFilePath))
	{
		CreateMultilevelPath(cstrFilePath);
	}

	if (str == _T(""))
	{
		return FALSE;
	}
	if (!IsMaxFileSize(path,nSize))
	{
		cstrExceptionMessage.Format(_T("%swriteAssignmentFile检测文件大小失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return	FALSE;
	}	

	EnterCriticalSection(&m_csAssignment);

	try
	{
		if(!m_AssignmentFile.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone))
		{
			LeaveCriticalSection(&m_csAssignment);

			cstrExceptionMessage.Format(_T("%swriteAssignmentFile文件打开失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

			writeExceptionFile(cstrExceptionMessage);

			return FALSE;
		}

		m_AssignmentFile.SeekToEnd();

		m_AssignmentFile.Write(cstrMessageWrite.GetBuffer(cstrMessageWrite.GetLength()),cstrMessageWrite.GetLength());

		cstrMessageWrite.ReleaseBuffer();

		m_AssignmentFile.Close();
	}
	catch (...)
	{
		m_AssignmentFile.Close();
		LeaveCriticalSection(&m_csAssignment);

		cstrExceptionMessage.Format(_T("%swriteAssignmentFile文件写入失败！\t%s\n写入信息：%s\n写入信息：%s\n"),cstrTime,path,cstrMessageWrite,str);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	LeaveCriticalSection(&m_csAssignment);

	return TRUE;
}

BOOL CRecord::writeUploadFile(const CString & str, const CString & path ,const int & nSize)
{
	if (m_nLogType < SaveUpLoadLog)
	{
		return FALSE;
	}

	CString cstrMessageWrite,cstrExceptionMessage,cstrTime;

	CString cstrFilePath,cstrName,cstrNewName,cstrExtName,cstrNewFileFullName;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	cstrTime.Format(_T("%04d/%02d/%02d-%02d:%02d:%02d.%03d	"),
		currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds);

	cstrMessageWrite.Format(_T("%s%s\n"),cstrTime,str);

	if ( !GetFilePathParam(path,cstrFilePath,cstrName,cstrExtName))
	{
		cstrExceptionMessage.Format(_T("%swriteUploadFile文件路径解析失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	if (!PathFileExists(cstrFilePath))
	{
		CreateMultilevelPath(cstrFilePath);
	}

	if (str == _T(""))
	{
		return FALSE;
	}
	if (!IsMaxFileSize(path,nSize))
	{
		cstrExceptionMessage.Format(_T("%swriteUploadFile检测文件大小失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return	FALSE;
	}	

	EnterCriticalSection(&m_csUploadRecord);

	try
	{
		if(!m_UploadFile.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone))
		{
			LeaveCriticalSection(&m_csUploadRecord);

			cstrExceptionMessage.Format(_T("%swriteUploadFile文件打开失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

			writeExceptionFile(cstrExceptionMessage);

			return FALSE;
		}

		m_UploadFile.SeekToEnd();

		m_UploadFile.Write(cstrMessageWrite.GetBuffer(cstrMessageWrite.GetLength()),cstrMessageWrite.GetLength());

		cstrMessageWrite.ReleaseBuffer();

		m_UploadFile.Close();
	}
	catch (...)
	{
		m_UploadFile.Close();
		LeaveCriticalSection(&m_csUploadRecord);

		cstrExceptionMessage.Format(_T("%swriteUploadFile文件写入失败！\t%s\n写入信息：%s\n写入信息：%s\n"),cstrTime,path,cstrMessageWrite,str);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	LeaveCriticalSection(&m_csUploadRecord);

	return TRUE;
}

BOOL CRecord::writeHttpFile(const CString & str, const CString & path ,const int & nSize)	
{
	if (m_nLogType < SaveAllLog)
	{
		return FALSE;
	}

	CString cstrMessageWrite,cstrExceptionMessage,cstrTime;

	CString cstrFilePath,cstrName,cstrNewName,cstrExtName,cstrNewFileFullName;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	cstrTime.Format(_T("%04d/%02d/%02d-%02d:%02d:%02d.%03d	"),
		currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds);

	cstrMessageWrite.Format(_T("%s%s\n"),cstrTime,str);

	if ( !GetFilePathParam(path,cstrFilePath,cstrName,cstrExtName))
	{
		cstrExceptionMessage.Format(_T("%swriteHttpFile文件路径解析失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	if (!PathFileExists(cstrFilePath))
	{
		CreateMultilevelPath(cstrFilePath);
	}

	if (str == _T(""))
	{
		return FALSE;
	}
	if (!IsMaxFileSize(path,nSize))
	{
		cstrExceptionMessage.Format(_T("%swriteHttpFile检测文件大小失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return	FALSE;
	}	

	EnterCriticalSection(&m_csHttpReport);

	try
	{
		if(!m_HttpReportFile.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone))
		{
			LeaveCriticalSection(&m_csHttpReport);

			cstrExceptionMessage.Format(_T("%swriteHttpFile文件打开失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

			writeExceptionFile(cstrExceptionMessage);

			return FALSE;
		}

		m_HttpReportFile.SeekToEnd();

		m_HttpReportFile.Write(cstrMessageWrite.GetBuffer(cstrMessageWrite.GetLength()),cstrMessageWrite.GetLength());

		cstrMessageWrite.ReleaseBuffer();

		m_HttpReportFile.Close();
	}
	catch (...)
	{
		m_HttpReportFile.Close();
		LeaveCriticalSection(&m_csHttpReport);

		cstrExceptionMessage.Format(_T("%swriteHttpFile文件写入失败！\t%s\n写入信息：%s\n"),cstrTime,path,cstrMessageWrite);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	LeaveCriticalSection(&m_csHttpReport);

	return TRUE;
}

BOOL CRecord::writeCommonFile(const CString & str, const CString & path ,const int & nSize)	
{
	if (m_nLogType < SaveAllLog)
	{
		return FALSE;
	}

	CString cstrMessageWrite,cstrExceptionMessage,cstrTime;

	CString cstrFilePath,cstrName,cstrNewName,cstrExtName,cstrNewFileFullName;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	cstrTime.Format(_T("%04d/%02d/%02d-%02d:%02d:%02d.%03d	"),
		currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds);

	cstrMessageWrite.Format(_T("%s%s\n"),cstrTime,str);

	if ( !GetFilePathParam(path,cstrFilePath,cstrName,cstrExtName))
	{
		cstrExceptionMessage.Format(_T("%swriteCommonFile文件路径解析失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	if (!PathFileExists(cstrFilePath))
	{
		CreateMultilevelPath(cstrFilePath);
	}

	if (str == _T(""))
	{
		return FALSE;
	}
	if (!IsMaxFileSize(path,nSize))
	{
		cstrExceptionMessage.Format(_T("%swriteCommonFile检测文件大小失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

		writeExceptionFile(cstrExceptionMessage);

		return	FALSE;
	}	

	EnterCriticalSection(&m_csCommon);

	try
	{
		if(!m_CommonFile.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone))
		{
			LeaveCriticalSection(&m_csCommon);

			cstrExceptionMessage.Format(_T("%swriteCommonFile文件打开失败！\t%s\n写入信息：%s\n"),cstrTime,path,str);

			writeExceptionFile(cstrExceptionMessage);

			return FALSE;
		}

		m_CommonFile.SeekToEnd();

		m_CommonFile.Write(cstrMessageWrite.GetBuffer(cstrMessageWrite.GetLength()),cstrMessageWrite.GetLength());

		cstrMessageWrite.ReleaseBuffer();

		m_CommonFile.Close();
	}
	catch (...)
	{
		m_CommonFile.Close();
		LeaveCriticalSection(&m_csCommon);

		cstrExceptionMessage.Format(_T("%swriteCommonFile文件写入失败！\t%s\n写入信息：%s\n"),cstrTime,path,cstrMessageWrite);

		writeExceptionFile(cstrExceptionMessage);

		return FALSE;
	}

	LeaveCriticalSection(&m_csCommon);

	return TRUE;
}

BOOL CRecord::writeExceptionFile(const CString & str,const CString & path,const int & nSize)
{
	if (m_nLogType < SaveAllLog)
	{
		return FALSE;
	}

	CString cstrMessageWrite;

	CString cstrTime;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	cstrTime.Format(_T("%04d/%02d/%02d-%02d:%02d:%02d.%03d	"),
		currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds);

	cstrMessageWrite.Format(_T("%s%s\n"),cstrTime,str);

	EnterCriticalSection(&m_csException);

	if (str == _T(""))
	{
		LeaveCriticalSection(&m_csException);

		return FALSE;
	}
	if (!IsMaxFileSize(path,nSize))
	{
		LeaveCriticalSection(&m_csException);

		return	FALSE;
	}	
	try
	{
		if(!m_ExceptionFile.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone))
		{

			LeaveCriticalSection(&m_csException);

			return FALSE;
		}

		m_ExceptionFile.SeekToEnd();

		m_ExceptionFile.Write(cstrMessageWrite.GetBuffer(cstrMessageWrite.GetLength()),cstrMessageWrite.GetLength());

		cstrMessageWrite.ReleaseBuffer();

		m_ExceptionFile.Close();
	}
	catch (...)
	{
		m_ExceptionFile.Close();
		LeaveCriticalSection(&m_csException);

		return FALSE;
	}

	LeaveCriticalSection(&m_csException);

	return TRUE;
}

*/











BOOL CRecord::IsMaxFileSize(const CString & path,int nSize ) 
{
	EnterCriticalSection(&m_csCheck);
	
	CString cstrMessageWrite;

	CString cstrTime;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	struct _stat info;

	cstrTime.Format(_T("%04d年%02d月%02d日%02d时%02d分%02d秒%03d毫秒："),
		currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds);

	CString cstrFilePath,cstrName,cstrNewName,cstrExtName,cstrNewFileFullName;

	if ( !GetFilePathParam(path,cstrFilePath,cstrName,cstrExtName))
	{
		cstrMessageWrite.Format(_T("%sIsMaxFileSize文件路径解析失败！\t%s\n"),cstrTime,path);

		writeExceptionFile(cstrMessageWrite);

		LeaveCriticalSection(&m_csCheck);

		return FALSE;
	}
	
	cstrNewName.Format(_T("%s-%d年%d月%d日%d时%d分%d秒"),cstrName,currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond);

	cstrNewFileFullName.Format(_T("%s%s%s"),cstrFilePath,cstrNewName,cstrExtName);

	if (!PathFileExists(cstrFilePath))
	{
		CreateMultilevelPath(cstrFilePath);
	}


	_stat(path, &info);

	double size = info.st_size;

	if ((double)(size/1024/1024) > nSize)		//1500000
	{
		try
		{
			CFile::Rename(path,cstrNewFileFullName);	
		}
		catch (CException* e)
		{
			LeaveCriticalSection(&m_csCheck);

			cstrMessageWrite.Format(_T("%sRename失败！%s\t新文件名：%s\n"),cstrTime,path,cstrNewFileFullName);

			writeExceptionFile(cstrMessageWrite);

			return FALSE;
		}
	}
	LeaveCriticalSection(&m_csCheck);


	/*

	if(!m_CheckFile.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate))
	{
		cstrMessageWrite.Format(_T("%sIsMaxFileSize文件打开失败！\t%s\n"),cstrTime,path);
		
		writeExceptionFile(cstrMessageWrite);

		LeaveCriticalSection(&m_csCheck);

		return FALSE;
	}
	else
	{
		int size = m_CheckFile.GetLength();

		m_CheckFile.Close();

		if (size > nSize)		//1500000
		{
			try
			{
				CFile::Rename(path,cstrNewFileFullName);	
			}
			catch (CException* e)
			{
				LeaveCriticalSection(&m_csCheck);

				cstrMessageWrite.Format(_T("%sRename失败！%s\t新文件名：%s\n"),cstrTime,path,cstrNewFileFullName);

				writeExceptionFile(cstrMessageWrite);

				return FALSE;
			}
		}
		LeaveCriticalSection(&m_csCheck);
	}

	*/

	return TRUE;
}

BOOL CRecord ::GetFilePathParam(const CString & cstrFullPath,CString & ctrPath, CString & cstrName,CString & cstrExtName)
{
	CString cstrNameWithExt;

	if (cstrFullPath.Find('.')< 0 && (cstrFullPath.Find('\\') <= 0 || cstrFullPath.Find('/') <= 0 ))		//判断是否为带路径文件名
	{
		return FALSE;
	}
	if (cstrFullPath.Find('\\') >= 0)
	{
		ctrPath = cstrFullPath.Left(cstrFullPath.ReverseFind('\\')+1);

		cstrNameWithExt = cstrFullPath.Right(cstrFullPath.GetLength()-cstrFullPath.ReverseFind('\\')-1);
	}
	else if (cstrFullPath.Find('/') >= 0)
	{
		ctrPath = cstrFullPath.Left(cstrFullPath.ReverseFind('/')+1);

		cstrNameWithExt = cstrFullPath.Right(cstrFullPath.GetLength()-cstrFullPath.ReverseFind('/')-1);
	}
	else
	{
		return FALSE;
	}
	cstrExtName = cstrNameWithExt.Right(cstrNameWithExt.GetLength()- cstrNameWithExt.ReverseFind('.'));

	cstrName = cstrNameWithExt.Left(cstrNameWithExt.ReverseFind('.'));

	return TRUE;
}


BOOL CRecord::writeExceptionFile(const CString & str,const CString & path ,const int & nSize)
{
	try
	{
		//LOG4CPLUS_FATAL(m_logger,str);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CRecord::writeAssignmentFile(const CString & str, const CString & path ,const int & nSize) //记录线程分配信息
{
	if (m_nLogType < SaveThreadLog)
	{
		return FALSE;
	}
	try
	{
		//LOG4CPLUS_TRACE(m_logger,str);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CRecord::writeUploadFile(const CString & str, const CString & path ,const int & nSize)			//记录文件上传信息
{
	if (m_nLogType < SaveUpLoadLog)
	{
		return FALSE;
	}
	try
	{
		//LOG4CPLUS_DEBUG(m_logger,str);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CRecord::writeHttpFile(const CString & str, const CString & path ,const int & nSize)				//记录http上报信息
{
	if (m_nLogType < SaveUpLoadLog)
	{
		return FALSE;
	}
	try
	{
		//LOG4CPLUS_INFO(m_logger,str);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CRecord::writeCommonFile(const CString & str, const CString & path ,const int & nSize)				//记录通用信息
{
	if (m_nLogType < SaveAllLog)
	{
		return FALSE;
	}
	try
	{
		//LOG4CPLUS_WARN(m_logger,str);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CRecord::writeMSG(const CString & str,const CString const& path ,const int & nSize)
{
	if (m_nLogType < SaveAllLog)
	{
		return FALSE;
	}
	try
	{
		//LOG4CPLUS_ERROR(m_logger,str);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}