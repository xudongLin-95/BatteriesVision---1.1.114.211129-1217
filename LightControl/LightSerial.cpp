#include "StdAfx.h"
#include "DlgSet.h"
#include "LightSerial.h"

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CLightSerial();
}

CLightSerial* g_This = NULL;

CLightSerial::CLightSerial(void)
{ 
	m_nCurrentStatus = 0;
	g_This = this;
}

CLightSerial::~CLightSerial(void)
{

}

int CLightSerial:: Init(LIGHTINIT &Init)
{
	m_sInit.strProductName = Init.strProductName;//配方
	m_sInit.nProductType = Init.nProductType;
	m_sInit.Log = Init.Log;
	m_strConfigLightSerialFilePath.Format("%s\\Config\\Public\\LightControl", GetCurrentAppPath());
	m_strConfigLightSerialFile.Format("%s\\ComSetting.ini", m_strConfigLightSerialFilePath);
	if (!PathFileExists(m_strConfigLightSerialFilePath))
		CreateMultilevelPath(m_strConfigLightSerialFilePath);

	ReadParams();
	
	return TRUE;
}

BOOL CLightSerial::Log(const CString& strLog,const int& nType,const int&nExcpLevel)
{
	if(m_sInit.Log==NULL) return FALSE;
	return m_sInit.Log(strLog, nType, nExcpLevel);
}

int CLightSerial::DoSomething(void* message,void* wParam,void* lParam)
{
	CString strMessage;
	strMessage.Format("%s",(char*)message);

	if ("INITIALIZE" == strMessage || "Initialize" == strMessage)
	{
		return Init(*(PLIGHTINIT)wParam);
	}
	else if ("OPEN" == strMessage || "Open" == strMessage)
	{
		//return serialPortExt.Open(
	}
	else if("CLOSE" == strMessage || "Close" == strMessage)
	{

	}
	else if("SHOW" == strMessage || "Show" == strMessage)
	{
		return ShowParamDlg();
	}
	else if("SETBRIGHTNESS" == strMessage || "SetBrightness" == strMessage)
	{

	}
	else if("SETCHANNEL" == strMessage || "SetChannel" == strMessage)
	{

	}
	else if ("GETCONNECTION" == strMessage || "GetConnection" == strMessage)
	{
		*(int*)lParam = m_nCurrentStatus;
		return TRUE;
	}
	return TRUE;
}

BOOL CLightSerial::ShowParamDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgSet dlg;
	//CDlgIOSetting dlg;
	dlg.m_pOwner = this;
	dlg.DoModal();
	return TRUE;
}

BOOL CLightSerial::SaveParams()//保存参数接口
{
	try
	{
		//获取选中通道
		WritePrivateProfileInt("COM", "LightType", m_tLightParam.eLightType, m_strConfigLightSerialFile);
		WritePrivateProfileInt("COM", "ComIndex", m_tLightParam.nComIndex, m_strConfigLightSerialFile);
		WritePrivateProfileString("COM", "ComName", m_tLightParam.sComName, m_strConfigLightSerialFile);
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}

BOOL CLightSerial::ReadParams()//获取参数接口
{
	try
	{
		m_tLightParam.eLightType = (eLightControlType)GetPrivateProfileInt("COM", "LightType", 0, m_strConfigLightSerialFile);
		m_tLightParam.nComIndex = GetPrivateProfileInt("COM", "ComIndex", 0, m_strConfigLightSerialFile);
		GetPrivateProfileString("COM", "ComName", "", m_tLightParam.sComName.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigLightSerialFile);
		m_tLightParam.sComName.ReleaseBuffer();//读取产品名称

		return TRUE;

	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}

BOOL CLightSerial:: Open()
{
	if (m_SerialPort.Open(m_tLightParam.sComName, 19200, 8, 0, 1))
	{
		m_nCurrentStatus = true;
		return true;
	}
	else
	{
		m_nCurrentStatus = false;
		return false;
	}
}
BOOL CLightSerial::Close()
{
	if (m_SerialPort.Close())
	{
		m_nCurrentStatus = false;
		return true;
	}
	else
	{
		return false;
	}
}
BOOL CLightSerial::SetBrightness(const int& nChannel, const int& nBrightness)
{
	if (!CheckChannel(nChannel)) return false;

	return m_SerialPort.SetLightBrightness(nChannel, nBrightness);
}
int CLightSerial::GetBrightnes(const int& nChannel)
{
	int nBt = 0;
	if (!CheckChannel(nChannel)) return -1;

	if (!m_SerialPort.GetLightBrightness(nChannel, nBt))
	{
		//日志
		CString logMsg;
		logMsg.Format("获取通道%d的亮度值异常:%d", nChannel,nBt);
		Log(logMsg, ExcpLog);
		return -1;
	}	
	return nBt;
}

BOOL CLightSerial::CheckChannel(const int& nChannel)
{
	switch (m_tLightParam.eLightType)
	{
		case DH_HAPS_48W800_1T:
			return 1 >= nChannel;

		case DH_HAPS_48W800_2T:
			return 2 >= nChannel;
         
		default:

			return false;
	}
}