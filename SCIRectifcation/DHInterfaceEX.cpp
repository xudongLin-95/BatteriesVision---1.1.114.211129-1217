#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "DHInterfaceEX.h"
#include "DialogUdp.h"

DHInterfaceEX::DHInterfaceEX(void):
	 m_pDlgUdp(NULL)
	, m_CPIDController(NULL)
	, m_isInit(FALSE)
	 , m_strRectifcationIni(_T(""))
	 , m_nConnection(0)
	 {
}


DHInterfaceEX::~DHInterfaceEX(void)
{
	if (m_pDlgUdp)
	{
		delete m_pDlgUdp;
		m_pDlgUdp = NULL;
	}
	if (m_CPIDController)
	{
		delete m_CPIDController;
		m_CPIDController = NULL;
	}
}
int DHInterfaceEX::DoSomething(void* message,void* wParam,void* lParam)
{
	CString  strMessage;
	strMessage.Format("%s",(char*)message);
	RECTIFICATIONINIT * pInitParam;
	if (strMessage == "INITIALIZE" || strMessage == "Initialize")
	{
		if (wParam == NULL)
		{
			pInitParam = NULL;
		}
		else
		{
			pInitParam = (RECTIFICATIONINIT*)wParam;
		}
		
		return Init(pInitParam);
	}
	else if (strMessage == "SHOW" || strMessage == "Show")
	{
		return Show();
	}
	else if (strMessage == "CHANGEOFFSET" || strMessage == "ChangeOffset")
	{
		if (wParam == NULL)
		{
			return TRUE;
		}
		else
		{
			double dOffset = *(double*)wParam;
			int *pSendOk;
			if (lParam == NULL)
			{
				int iTemp;
				pSendOk = &iTemp;
			}
			else
			{
				pSendOk = (int *) lParam;
			}
			return SendMSG(dOffset,pSendOk);
		}	
	}
	else if (strMessage == "UPDATESPEED" || strMessage == "UpadateSpeed")
	{
	//	double dSpeed = *(double*)wParam;

		return Show();
	}
	else if(strMessage == "GETCONNECTION" || strMessage == "GetConnection")
	{
		if (lParam != NULL)
		{
			*(int*)lParam = m_nConnection;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL DHInterfaceEX::Init(PRECTIFICATIONINIT pInit)
{
	SetInit(pInit);
	ReadParam();
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( m_pDlgUdp == NULL && m_isInit == FALSE)
	{
		
		m_pDlgUdp = new DialogUdp();
		m_pDlgUdp->m_pInterfaceEX = this;
		m_pDlgUdp->m_RectifcationSN = m_readParam.nRectifcationSN;
		m_pDlgUdp->m_srcPort = m_readParam.nSrcPort;
		m_pDlgUdp->m_destPort = m_readParam.nDestPort;
		m_pDlgUdp->m_nUse = m_readParam.nUse;
		m_pDlgUdp->m_iDelay = m_readParam.nDelay;
		m_pDlgUdp->m_nThreshValue = m_readParam.nActionThresh;
		m_pDlgUdp->m_nKp = m_readParam.nKp;
		m_pDlgUdp->m_pInitParam = &m_InitParam;
		int strLength =0;
		strLength = m_readParam.strSrcIP.GetLength()+1;
		memset(m_pDlgUdp->m_srcIP,0,MAX_PATH);
		strncpy(m_pDlgUdp->m_srcIP, m_readParam.strSrcIP, strLength); 
		strLength = m_readParam.strDestIP.GetLength()+1;
		memset(m_pDlgUdp->m_destIP,0,MAX_PATH);
		strncpy(m_pDlgUdp->m_destIP, m_readParam.strDestIP, strLength); 
		m_isInit = TRUE;
		m_pDlgUdp->Create(IDD_DIALOGUDP);
		m_pDlgUdp->ShowWindow(SW_HIDE);
		m_CPIDController = new CPIDController(m_readParam.nKp,m_readParam.nActionThresh,m_readParam.nOffset,m_readParam.nDelay);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}


BOOL DHInterfaceEX::Show(void)
{
	if (m_isInit)
	{
		m_pDlgUdp->SetContrl(FALSE);
		m_pDlgUdp->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}


BOOL DHInterfaceEX::ReadParam(void)
{
	try
	{
		CString strFilePath;
		CString strSection;
		strSection.Format(_T("RECTIFICATION%d"),m_InitParam.nSN);
		strFilePath.Format("%s\\Config\\Public\\SCIRectification\\",GetCurrentAppPath());
		CreateMultilevelPath(strFilePath);
		m_strRectifcationIni.Format("%s\\Rectification.ini",strFilePath);
		m_readParam.nUse = GetPrivateProfileInt(strSection,"Use",1,m_strRectifcationIni);
		m_readParam.nSrcPort = GetPrivateProfileInt(strSection,"SrcPort",2200,m_strRectifcationIni);
		m_readParam.nDestPort = GetPrivateProfileInt(strSection,"DestPort",2200,m_strRectifcationIni);
		GetPrivateProfileString(strSection,"SrcIP","192.168.1.180",m_readParam.strSrcIP.GetBuffer(MAX_PATH),MAX_PATH,m_strRectifcationIni);
		m_readParam.strSrcIP.ReleaseBuffer();
		 GetPrivateProfileString(strSection,"DestIP","192.168.1.154",m_readParam.strDestIP.GetBuffer(MAX_PATH),MAX_PATH,m_strRectifcationIni);
		m_readParam.strDestIP.ReleaseBuffer();
		m_readParam.nDelay = GetPrivateProfileInt(strSection,"nDelay",10,m_strRectifcationIni);
		m_readParam.nKp = GetPrivateProfileDouble(strSection,"nKp",0.2,m_strRectifcationIni);
		m_readParam.nOffset = GetPrivateProfileDouble(strSection,"nOffset",0,m_strRectifcationIni);
		m_readParam.nActionThresh =  GetPrivateProfileDouble(strSection,"nActionThresh",0.05,m_strRectifcationIni);
		return TRUE;
	}catch( ...)
	{
		return FALSE;
	}
}


BOOL DHInterfaceEX::WriteParam(void)
{
	try
	{
		CString strFilePath;
		CString strSection;
		strSection.Format(_T("RECTIFICATION%d"),m_InitParam.nSN);
		strFilePath.Format("%s\\Config\\Public\\SCIRectification\\",GetCurrentAppPath());
		CreateMultilevelPath(strFilePath);
		m_strRectifcationIni.Format("%s\\Rectification.ini",strFilePath);
		WritePrivateProfileInt(strSection,"Use",m_readParam.nUse,m_strRectifcationIni);
		WritePrivateProfileInt(strSection,"SrcPort",m_readParam.nSrcPort,m_strRectifcationIni);
		WritePrivateProfileInt(strSection,"DestPort",m_readParam.nDestPort,m_strRectifcationIni);
		WritePrivateProfileString(strSection,"SrcIP",m_readParam.strSrcIP,m_strRectifcationIni);
		WritePrivateProfileString(strSection,"DestIP",m_readParam.strDestIP,m_strRectifcationIni);
		WritePrivateProfileInt(strSection,"nDelay",m_readParam.nDelay,m_strRectifcationIni);
		WritePrivateProfileDouble(strSection,"nKp",m_readParam.nKp,m_strRectifcationIni);
		WritePrivateProfileDouble(strSection,"nOffset",m_readParam.nOffset,m_strRectifcationIni);
		WritePrivateProfileDouble(strSection,"nActionThresh",m_readParam.nActionThresh,m_strRectifcationIni);
		return TRUE;
	}catch( ...)
	{
		return FALSE;
	}
}


BOOL DHInterfaceEX::SendMSG(double dValue,int *pSendOk)
{
	double offsetValue = 0;
	if(NULL != m_CPIDController)
	{
		offsetValue = m_CPIDController->Rectification(dValue);
		if (offsetValue > 0)
		{
			offsetValue = ((int)(offsetValue*10  + 0.5))/10;						//发送数据小数点后1位，小数点后2位四舍五入
		}
		else if(offsetValue < 0)
		{
			offsetValue = ((int)(offsetValue*10  - 0.5))/10;
		}
	}
	m_pDlgUdp->m_valR06 = offsetValue;
	*pSendOk = m_pDlgUdp->m_SendStatus;
	m_nConnection = m_pDlgUdp->m_SendStatus;
	return m_pDlgUdp->SendMSG();
}


BOOL DHInterfaceEX::SetInit(PRECTIFICATIONINIT pInitParam)
{
	if(NULL != pInitParam)
	{
		m_InitParam.Log = pInitParam->Log;
		m_InitParam.nProductType = pInitParam->nProductType;
		m_InitParam.nSN = pInitParam->nSN;
		m_InitParam.pDlg = pInitParam->pDlg;
		m_InitParam.RectficaionCall = pInitParam->RectficaionCall;
		m_InitParam.strProductName = pInitParam->strProductName;
		return TRUE;
	}
	else
	{
		
		m_InitParam.Log = NULL;
		m_InitParam.nProductType = -1;
		m_InitParam.nSN = 1;
		m_InitParam.pDlg = NULL;
		m_InitParam.RectficaionCall = NULL;
		m_InitParam.strProductName = "";
		return TRUE;	
	}	
}
