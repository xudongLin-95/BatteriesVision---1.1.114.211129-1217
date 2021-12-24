#include "stdafx.h"
#include "UpsSerial.h"
#include "DlgUpsSet.h"

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CUpsSerial();
}
CUpsSerial* g_This = NULL;

CSerialPortExt m_SerialPort;//全局变量

CUpsSerial::CUpsSerial()
{	
	g_This = this;	
	InitializeCriticalSection(&m_csLock);
}

CString CUpsSerial:: UpsAlarmMsg = ""; //静态成员变量，在类外初始化

CRITICAL_SECTION CUpsSerial::m_csLock;

CUpsSerial::~CUpsSerial()
{

}

BOOL CUpsSerial::ShowParamDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgUpsSet dlg;
	dlg.m_pOwner = this;
	dlg.DoModal();
	return TRUE;
}

int CUpsSerial::DoSomething(void* message, void* wParam, void* lParam)
{
	CString LogStr;
	CString strMessage;
	strMessage.Format("%s", (char*)message);

	if ("INITIALIZE" == strMessage || "Initialize" == strMessage)
	{	
		return Init(*(PUPSINIT)wParam);
	}
	else if ("OPEN" == strMessage || "Open" == strMessage)
	{
		return Open();
	}
	else if ("CLOSE" == strMessage || "Close" == strMessage)
	{
		return Close();
	}
	else if ("SHOW" == strMessage || "Show" == strMessage)
	{
		return ShowParamDlg();
	}
	else if ("GETALARMMSG" == strMessage || "GetAlarmMsg" == strMessage)
	{
		CString msg;
		msg = GetUpsAlarmResult();
		if ("%" == msg)
		{
			LogStr.Format("电池电压过低警告");				
			Log(LogStr, ExcpLog);
			*(CString*)wParam = LogStr;
			return false;
		}
		else if ("#" == msg)
		{
			LogStr.Format("UPS 自动测试电池失败返回信息");
			Log(LogStr, ExcpLog);
			*(CString*)wParam = LogStr;
			return false;
		}
		else if ("Ctrl+D" == msg)
		{
			LogStr.Format("UPS 内部温度过高警告");
			Log(LogStr, ExcpLog);
			*(CString*)wParam = LogStr;
			return false;
		}
		else if ("!" == msg)
		{
			LogStr.Format("UPS 使用电池逆变输出");
			Log(LogStr, ExcpLog);
			*(CString*)wParam = LogStr;
			return false;
		}
		else if ("$" == msg)
		{
			LogStr.Format("表示UPS 刚刚从使用的电池切换到使用市电电压逆变");
			Log(LogStr, ExcpLog);
			*(CString*)wParam = LogStr;
			return true;
		}
		LogStr.Format("UPS正常");
		*(CString*)wParam = LogStr;
	}
	
	return TRUE;
}

int CUpsSerial::Init(UPSINIT &Init)
{
	m_sInit = Init;

	CString filePath;
	filePath.Format("%s\\Config\\Public\\UpsControl", GetCurrentAppPath());
	if (!PathFileExists(filePath))CreateMultilevelPath(filePath);
	m_strConfigUpsSerialFile.Format("%s\\Ups.ini", filePath);

	ReadParams();
	GetUpsAlram();//启动UPS状态监听
	
    //Log("CUpsSerial:: Init", StdLog, NoExcp);
	return true;
}

BOOL CUpsSerial:: IsOpen()
{ 
	return m_SerialPort.IsOpen(); 
}

BOOL CUpsSerial::Open() 
{
	return m_SerialPort.Open(m_tUpsParam.nComName, 2400, 8, 0, 1);
}

BOOL CUpsSerial::Close()
{
	return m_SerialPort.Close();
}

BOOL CUpsSerial::SetUpsConnect(CString &recv)//建立连接
{
	return m_SerialPort.SetUpsConnect(recv);
}

BOOL CUpsSerial::GetUpsStates(CString &state)//获取UPS的状态信息
{
	return m_SerialPort.GetUpsStates(state);
}

BOOL CUpsSerial::Log(const CString& strLog, const int& nType, const int&nExcpLevel)
{
	if (m_sInit.Log == NULL) return FALSE;
	return m_sInit.Log(strLog, nType, nExcpLevel);
}

BOOL CUpsSerial::SaveParams()//保存参数接口
{
	try
	{
		//端口号
		WritePrivateProfileString("COM", "ComName", m_tUpsParam.nComName, m_strConfigUpsSerialFile);
		WritePrivateProfileInt("COM", "ComIndex", m_tUpsParam.nComIndex, m_strConfigUpsSerialFile);
		WritePrivateProfileInt("Set", "UseCheck", m_checkUpsStatus, m_strConfigUpsSerialFile);
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CUpsSerial::ReadParams()//获取参数接口
{
	try
	{		
		GetPrivateProfileString("COM", "ComName", "", m_tUpsParam.nComName.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigUpsSerialFile);
		m_tUpsParam.nComName.ReleaseBuffer();
		m_tUpsParam.nComIndex = GetPrivateProfileInt("COM", "ComIndex", -1, m_strConfigUpsSerialFile);
		m_checkUpsStatus = GetPrivateProfileInt("Set", "UseCheck", FALSE, m_strConfigUpsSerialFile);
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

void CUpsSerial::GetUpsAlram()//获取自动报警的状态
{
	//启动线程，循环读取状态信息，
	AfxBeginThread((AFX_THREADPROC)ThreadUpsState, this); // 心跳监测线程
}

UINT CUpsSerial:: ThreadUpsState(LPVOID lpVoid) //UPS循环监测自动异常信息 线程函数
{
	while (1)
	{		
		try
		{
			CLockHelper lk(&m_csLock);

			m_SerialPort.Read(UpsAlarmMsg);		
		}
		catch (const std::exception&)
		{

		}
		catch (...)
		{

		}
		Sleep(1000);
	}	
}