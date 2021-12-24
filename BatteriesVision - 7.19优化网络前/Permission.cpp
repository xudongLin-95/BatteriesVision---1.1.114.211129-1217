#include "StdAfx.h"
#include "Permission.h"

CPermission::CPermission()
{
}

CPermission::~CPermission()
{

}

BOOL CPermission::Init()
{
	ReadParam();
	m_tUserInfo.strJobNumber = "NA";

	POWERLIMITINIT InitParam;
	InitParam.pLog = theGlobal.m_Log.LogCallBack;
	m_pPowerLimit = LoadDHInterFaceDll("PowerLimit");
	if (m_pPowerLimit == NULL || TRUE != m_pPowerLimit->DoSomething("Init", &InitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init PowerLimit.dll,program exists.\r\nPowerLimit.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}
	m_pPowerLimit->DoSomething("GetUserInfo", &m_tUserInfo, NULL);

	return TRUE;
}

BOOL CPermission::ReadParam()
{
	return TRUE;
}

BOOL CPermission::WriteParam()
{
	return TRUE;
}

BOOL CPermission::Login()
{
	if (m_pPowerLimit == NULL) return TRUE;
	if (!m_pPowerLimit->DoSomething("Login", NULL, NULL)) return FALSE;
	m_pPowerLimit->DoSomething("GetUserInfo", &m_tUserInfo, NULL);

	//操作人员登录记录
	RecoredOperatorMsg(TRUE, m_tUserInfo);

	return TRUE;
}

void CPermission::Logout()
{
	if (m_pPowerLimit == NULL) return;

	//操作人员登录记录
	RecoredOperatorMsg(FALSE, m_tUserInfo);

	m_pPowerLimit->DoSomething("Logout", NULL, NULL);
	m_pPowerLimit->DoSomething("GetUserInfo", &m_tUserInfo, NULL);
}

void CPermission::RecoredOperatorMsg(BOOL bLoginOrOut, USERINFO tUserInfo)//操作人员登录记录
{
	try
	{
		//时间
		SYSTEMTIME	tSaveTime;
		::GetLocalTime(&tSaveTime);

		CString sTime, strItem;
		sTime.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay,
			tSaveTime.wHour, tSaveTime.wMinute, tSaveTime.wSecond, tSaveTime.wMilliseconds);

		//获取当前用户
		CString strFileName, pDirectorPath;
		pDirectorPath.Format("%s\\Log\\Operator\\%04d\\%02d\\人员登录记录\\", GetCurrentAppPath(), tSaveTime.wYear, tSaveTime.wMonth);
		strFileName.Format("%s\\%02d_Login.csv", pDirectorPath, tSaveTime.wDay);
		CStdioFile file;

		if (!PathFileExists(pDirectorPath))CreateMultilevelPath(pDirectorPath);
		if (TRUE != file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("CPermission::RecoredOperatorMsg: 创建检查数据文件失败 (%s)", strFileName);
			theGlobal.m_Log.Log(strException, ExcpLog);
		}

		if (bLoginOrOut)
		{
			strItem.Format("%s,%s 成功登录...\n", sTime, tUserInfo.strJobNumber);
		}
		else
		{
			strItem.Format("%s,%s 注销登录...\n", sTime, tUserInfo.strJobNumber);
		}
		file.SeekToEnd();//文件存在，则追加消息
		file.WriteString(strItem);
		file.Close();
	}
	catch (const std::exception& e)
	{
		CString strLog;
		strLog.Format("CPermission_RecoredOperatorMsg异常");
		theGlobal.m_Log.Log(strLog, ExcpLog);
	}
	catch (...)
	{
		CString strLog;
		strLog.Format("CPermission_RecoredOperatorMsg异常");
		theGlobal.m_Log.Log(strLog, ExcpLog);
	}
}


BOOL CPermission::CheckPermission(ePOWERLIMITMODULE eModule)
{
	if (m_pPowerLimit == NULL) return FALSE;
	m_pPowerLimit->DoSomething("GetUserInfo", &m_tUserInfo, NULL);
	return m_tUserInfo.CheckPowerEnable(eModule);
}


BOOL CPermission::ShowUserManage()
{
	if (m_pPowerLimit == NULL) return FALSE;
	return m_pPowerLimit->DoSomething("UserManage", NULL, NULL);
}

BOOL CPermission::ShowUserPower()
{
	if (m_pPowerLimit == NULL) return FALSE;
	return m_pPowerLimit->DoSomething("PowerManage", NULL, NULL);
}