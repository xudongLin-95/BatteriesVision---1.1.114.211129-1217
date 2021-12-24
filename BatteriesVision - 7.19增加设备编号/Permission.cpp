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
		strException.Format("Fail to init PowerLimit.dll,program exists.\r\nPowerLimit.dll��ʼ��ʧ�ܣ������˳�");
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

	//������Ա��¼��¼
	RecoredOperatorMsg(TRUE, m_tUserInfo);

	return TRUE;
}

void CPermission::Logout()
{
	if (m_pPowerLimit == NULL) return;

	//������Ա��¼��¼
	RecoredOperatorMsg(FALSE, m_tUserInfo);

	m_pPowerLimit->DoSomething("Logout", NULL, NULL);
	m_pPowerLimit->DoSomething("GetUserInfo", &m_tUserInfo, NULL);
}

void CPermission::RecoredOperatorMsg(BOOL bLoginOrOut, USERINFO tUserInfo)//������Ա��¼��¼
{
	try
	{
		//ʱ��
		SYSTEMTIME	tSaveTime;
		::GetLocalTime(&tSaveTime);

		CString sTime, strItem;
		sTime.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay,
			tSaveTime.wHour, tSaveTime.wMinute, tSaveTime.wSecond, tSaveTime.wMilliseconds);

		//��ȡ��ǰ�û�
		CString strFileName, pDirectorPath;
		pDirectorPath.Format("%s\\Log\\Operator\\%04d\\%02d\\��Ա��¼��¼\\", GetCurrentAppPath(), tSaveTime.wYear, tSaveTime.wMonth);
		strFileName.Format("%s\\%02d_Login.csv", pDirectorPath, tSaveTime.wDay);
		CStdioFile file;

		if (!PathFileExists(pDirectorPath))CreateMultilevelPath(pDirectorPath);
		if (TRUE != file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("CPermission::RecoredOperatorMsg: ������������ļ�ʧ�� (%s)", strFileName);
			theGlobal.m_Log.Log(strException, ExcpLog);
		}

		if (bLoginOrOut)
		{
			strItem.Format("%s,%s �ɹ���¼...\n", sTime, tUserInfo.strJobNumber);
		}
		else
		{
			strItem.Format("%s,%s ע����¼...\n", sTime, tUserInfo.strJobNumber);
		}
		file.SeekToEnd();//�ļ����ڣ���׷����Ϣ
		file.WriteString(strItem);
		file.Close();
	}
	catch (const std::exception& e)
	{
		CString strLog;
		strLog.Format("CPermission_RecoredOperatorMsg�쳣");
		theGlobal.m_Log.Log(strLog, ExcpLog);
	}
	catch (...)
	{
		CString strLog;
		strLog.Format("CPermission_RecoredOperatorMsg�쳣");
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