#include "StdAfx.h"
#include "Callback.h"


CCallback::CCallback()
{
}

CCallback::~CCallback()
{
}

BOOL CCallback::Init()
{
	ReadParam();

	return TRUE;
}
BOOL CCallback::ReadParam()
{
	return TRUE;
}
BOOL CCallback::WriteParam()
{
	return TRUE;
}

int WINAPI CCallback::MainCallback(eModuleType eType, const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam)
{
	//dll 反馈给主系统消息的回调
	if ("CheckPermission" == strMessage || 
		"GetUserInfo" == strMessage || 
		"GetProductInfo" == strMessage ||
		"Login" == strMessage ||
		"CalColPoint" == strMessage)
	{
		return theGlobal.m_Callback.CommonCall(eType, strMessage, strSubMessage, wParam, lParam);
	}

	switch (eType)
	{
	case MT_Marking:
		return theGlobal.m_Callback.MarkingCall(strMessage, strSubMessage, wParam, lParam);
	case MT_PLC:
		return theGlobal.m_Callback.SCIPLCCall(strMessage, strSubMessage, wParam, lParam);
	case MT_Measure:
		return theGlobal.m_Callback.SCIMeasureCall(strMessage, strSubMessage, wParam, lParam);
	case MT_Mes:
		return theGlobal.m_Callback.SCIMesCall(strMessage, strSubMessage, wParam, lParam);
	default:
		return theGlobal.m_Callback.CommonCall(eType, strMessage, strSubMessage, wParam, lParam);
	}
	return TRUE;
}


int CCallback::MarkingCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam)
{
	try
	{
		if ("ReadCount" == strMessage)
		{
			if ("In0" == strSubMessage)
			{
				int nParam = 28, nCount = 0;

				theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::IO, "READCOUNT", &nParam, &nCount);
				*(int*)wParam = nCount;
				return TRUE;
			}
		}
		else if ("ReadTotalMeter" == strMessage)
		{
			*(double*)wParam = theGlobal.m_Counter.GetCurrentMeterCount();
			return TRUE;
		}
		else if ("ReadSpeed" == strMessage)
		{
			*(double*)wParam = theGlobal.m_Speed.GetSpeed();
			return TRUE;
		}
		else if ("GetSideToCamSNList" == strMessage)
		{
			if ("Double" == strSubMessage)
			{
				for (int i = 0; i < 2; i++)
				{
					HTuple hvCamSNList = theGlobal.m_Project.GetSideToCamSNList(i);
					vector<int> vSN;
					for (int i = 0; i < hvCamSNList.Length(); i++)
					{
						vSN.push_back(hvCamSNList[i].I());
					}

					if (0 == i)
						*(vector<int>*)wParam = vSN;
					else
						*(vector<int>*)lParam = vSN;
				}
			}
			else
			{
				const HTuple hvCamSNList = theGlobal.m_Project.GetSideToCamSNList(_ttoi(strSubMessage));
				vector<int> vSN;
				for (int i = 0; i < hvCamSNList.Length(); i++)
				{
					vSN.push_back(hvCamSNList[i].I());
				}
				*(vector<int>*)wParam = vSN;
			}

			return TRUE;
		}
	}
	catch (const std::exception&)
	{}	
	catch (HException& e)
	{}
	catch (...)
	{}
	return FALSE;
}

int CCallback::SCIPLCCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam)
{
	if ("GetUserInfo" == strMessage)
	{
		*(USERINFO*)wParam = theGlobal.m_Permission.GetUserInfo();
		return true;
	}
}

int CCallback::SCIMeasureCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam)
{
	if ("GetAutoCompensationEnable" == strMessage)
	{
		if (theGlobal.m_Upload.m_bAutoCompensationDisable)
			*(BOOL*)lParam = FALSE;
		else
			*(BOOL*)lParam = theGlobal.m_Upload.m_tParam.bUseCompensationAuto;

		if (theGlobal.m_Upload.m_tParam.bUseCompensationAuto)
		{
			CString strLog;
			strLog.Format("CCallback_SCIMeasureCall: 干湿膜动态补偿禁用%s", theGlobal.m_Upload.m_bAutoCompensationDisable ? "开启" : "关闭");
			theGlobal.m_Log.Std(strLog);
		}
		
		return true;
	}
}

int CCallback::SCIMesCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam)
{
	if ("GetMesProductInfo" == strMessage)
	{
		tagMesProductInfo tInfo;
		tInfo.strBatch = theGlobal.GetCurrentBatch();//批次
		tInfo.strUserID = theGlobal.GetCurrentJobNum();//用户名
		tInfo.strShift = "";	//班次
		tInfo.strProductType = theGlobal.m_ProductTool.GetCurrentProductInfo().strPDName;//产品类型
		tInfo.strPasteCode = "";	//浆料号
		tInfo.dCoatStd = theGlobal.m_ProductTool.GetCurrentProductInfo().tParam.vecCoatWidth[0].dStandard;
		tInfo.dFoilStd = theGlobal.m_ProductTool.GetCurrentProductInfo().tParam.vecFoilWidth[0].dStandard;

		*(tagMesProductInfo*)lParam = tInfo;
		return TRUE;
	}
}

int CCallback::CommonCall(eModuleType eType, const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam)
{
	if ("CheckPermission" == strMessage)
	{
		if (theGlobal.m_Permission.IsLogin())
			return theGlobal.m_Permission.CheckPermission(*(ePOWERLIMITMODULE*)wParam);
		else
			return FALSE;
	}
	else if ("GetUserInfo" == strMessage)
	{
		*(USERINFO*)wParam = theGlobal.m_Permission.GetUserInfo();
		return true;
	}
	else if ("GetProductInfo" == strMessage)
	{
		*(tagPDInfo*)lParam = theGlobal.m_ProductTool.GetCurrentProductInfo();
		return true;
	}
	else if ("Login" == strMessage)
	{
		theGlobal.SendMessageToMainDlg(WM_CHECK_PMSN, NULL, NULL);
		return TRUE;
	}
	else if ("CalColPoint" == strMessage)
	{
		if ("Side0" == strSubMessage)
		{
			return theGlobal.m_AlgTool.DoSomething(0, CAlgorithmTool::AlgMeasure, "CalColPoint", wParam, lParam);
		}
		else
		{
			return theGlobal.m_AlgTool.DoSomething(1, CAlgorithmTool::AlgMeasure, "CalColPoint", wParam, lParam);
		}
	}
}