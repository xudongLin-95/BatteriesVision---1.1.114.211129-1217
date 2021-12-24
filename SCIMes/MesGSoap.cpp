#include "StdAfx.h"
#include "MesGSoap.h"
#include "include/json/cJSON.h"
#include "include/json/CJsonObject.hpp"

CMesGSoap* CMesGSoap::m_instance = NULL;

CMesGSoap::CMesGSoap(void):
	m_bConnected(FALSE),
	m_cstrMesIP("10.36.23.54"),
	m_nMesPort(50899)
{
	loadConfig();
	setupSoaps();
	setMesAddrAndPort();
	InitializeCriticalSection(&m_csSoap);
}


CMesGSoap::~CMesGSoap(void)
{
	DeleteCriticalSection(&m_csSoap);
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

int CMesGSoap::RecipeVersionExamine(const ParamRecipeVersionExamine& paramRecipeVersion, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	//neb::CJsonObject nebCJsonHeader, nebCJsonBody, nebCJsonSFCs, nebCJsonParamList;
	//CLockHelper lockHelp(&m_csSoap);
	//time_t  sendTime;
	//time(&sendTime);
	//cJsonSend.Clear();
	//cJsonRcv.Clear();
	//EquToMesAlarmSoapProxy RecipeVersionExamineSoap;
	//_ns1__RecipeVersionExamine RecipeVersionExamineSend;
	//_ns1__RecipeVersionExamineResponse RecipeVersionExamineRcv;
	//ns1__RecipeVersionExamine	 body;
	//CStringW cstrRecipeCode = CStrA2CStrW(paramRecipeVersion.cstrRecipeCode);
	//CStringW cstrwRecipeVersion = CStrA2CStrW(paramRecipeVersion.cstrVersion);
	//setupSoap(RecipeVersionExamineSoap);
	//body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	//body.ResourceCode = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	//body.RecipeCode = (wchar_t*)reinterpret_cast<const char*>(cstrRecipeCode.GetString());
	//body.Version = (wchar_t*)reinterpret_cast<const char*>(cstrwRecipeVersion.GetString());
	//body.LocalTime = sendTime;
	//{
	//	nebCJsonHeader.Add("EquUserID", CStrW2CStrA(m_EquUserID).GetString());
	//	nebCJsonHeader.Add("EquPassword", CStrW2CStrA(m_EquPassword).GetString());
	//	nebCJsonHeader.Add("OperatorUserID", CStrW2CStrA(m_OperatorUserID).GetString());
	//	nebCJsonHeader.Add("OperatorPassword", CStrW2CStrA(m_OperatorPassword).GetString());
	//	nebCJsonBody.Add("EquipmentCode", CStrW2CStrA(m_EquipmentCode).GetString());
	//	nebCJsonBody.Add("ResourceCode", CStrW2CStrA(m_ResourceCode).GetString());
	//	nebCJsonBody.Add("RecipeCode", paramRecipeVersion.cstrRecipeCode.GetString());
	//	nebCJsonBody.Add("Version", paramRecipeVersion.cstrVersion.GetString());
	//	nebCJsonBody.Add("LocalTime", GetCurrentLocalTime(sendTime).GetString());
	//	cJsonSend.Add("SendTime", GetSystemLocalTime().GetString());
	//	cJsonSend.AddEmptySubArray("header");
	//	cJsonSend.AddEmptySubArray("body");
	//	cJsonSend["header"].Add(nebCJsonHeader);
	//	cJsonSend["body"].Add(nebCJsonBody);
	//}
	//RecipeVersionExamineSend.model = &body;
	//RecipeVersionExamineSoap.soap_header(&getHeader());
	//int res = RecipeVersionExamineSoap.RecipeVersionExamine(getEndpoint(EMESOperate::RecipeVersionExamine), nullptr, &RecipeVersionExamineSend, RecipeVersionExamineRcv);
	//{
	//	GetRcvCJson(cJsonRcv, EMESOperate::RecipeVersionExamine);
	//	cJsonRcv.Replace("Res", res);
	//	cJsonRcv.AddEmptySubArray("Data");
	//	if (RecipeVersionExamineRcv.RecipeVersionExamineResult)
	//	{
	//		CStringW cstrwMsg(RecipeVersionExamineRcv.RecipeVersionExamineResult->Msg);
	//		m_bConnected = TRUE;
	//		//0-异常 1-通过校验 2-配方不存在 3-版本不是当前激活版本 4-当前资源所在线体无激活工单，无法校验。 5-配方与当前资源所在线体的激活工单不匹配
	//		cJsonRcv.Replace("code", RecipeVersionExamineRcv.RecipeVersionExamineResult->Code);
	//		cJsonRcv.Replace("msg", CStrW2CStrA(cstrwMsg).GetString());
	//	}
	//	else
	//	{
	//		m_bConnected = FALSE;
	//	}
	//}
	return 1;
}
inline void CMesGSoap::setUrlArray()
{
	CString cstrUrl;
	m_urlMap.clear();

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesOperatorLogin.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::OperatorLogin] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesHeartbeat.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::Heartbeat] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesState.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::State] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesDownReason.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::DownReason] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesAlarm.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::Alarm] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesRecipeListGet.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::GetRecipeList] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesRecipeGet.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::GetRecipe] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesRecipeVersionExamine.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::RecipeVersionExamine] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesRecipe.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::Recipe] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesToolBind.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::ToolBind] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesToolLife.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::ToolLife] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesEquipmentProcessParam.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::EquipmentProcessParam] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesFeeding.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::Feeding] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesInbound.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::Inbound] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesOutbound.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::Outbound] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesEquipmentProductProcessParam.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::EquipmentProductProcessParam] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesBindSFC.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::BindSFC] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesInboundInContainer.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::InboundInContainer] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesBindContainer.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::BindContainer] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesOutboundInSFCOutputQty.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::OutboundInSFCOutputQty] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesFeedingDJInLaminating.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::FeedingDJInLaminating] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesExamineExists.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::ExamineExists] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesOutboundInLaminating.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::OutboundInLaminating] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesGenerateCellSFC.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::GenerateCellSFC] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesFillingData.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::FillingData] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesInboundInTwoInjection.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::InboundInTwoInjection] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesInboundInSFCContainer.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::InboundInSFCContainer] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesInboundMore.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::InboundMore] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesOutboundMore.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::OutboundMore] = cstrUrl;

	cstrUrl.Format("http://%s:%d/EquToMes/EquToMesCCDFileUploadComplete.asmx",m_cstrMesIP,m_nMesPort);
	m_urlMap[EMESOperate::CCDFileUploadComplete] = cstrUrl;
	
}
inline const char* CMesGSoap::getEndpoint(const EMESOperate& op)
{
	return m_urlMap[op].GetString();

	switch (op)
	{
	case EMESOperate::OperatorLogin:
		return "http://10.36.23.54:50899/EquToMes/EquToMesOperatorLogin.asmx";

	case EMESOperate::Heartbeat:
		return "http://10.36.23.54:50899/EquToMes/EquToMesHeartbeat.asmx";
	
	case EMESOperate::State:
		return "http://10.36.23.54:50899/EquToMes/EquToMesState.asmx";

	case EMESOperate::DownReason:
		return "http://10.36.23.54:50899//EquToMes/EquToMesDownReason.asmx";

	case EMESOperate::Alarm:
		return "http://10.36.23.54:50899/EquToMes/EquToMesAlarm.asmx";

	case EMESOperate::GetRecipeList:
		return "http://10.36.23.54:50899/EquToMes/EquToMesRecipeListGet.asmx";

	case EMESOperate::GetRecipe:
		return "http://10.36.23.54:50899/EquToMes/EquToMesRecipeGet.asmx";

	case EMESOperate::RecipeVersionExamine:
		return "http://10.36.23.54:50899/EquToMes/EquToMesRecipeVersionExamine.asmx";

	case EMESOperate::Recipe:
		return "http://10.36.23.54:50899/EquToMes/EquToMesRecipe.asmx";

	case EMESOperate::ToolBind:
		return "http://10.36.23.54:50899/EquToMes/EquToMesToolBind.asmx";

	case EMESOperate::ToolLife:
		return "http://10.36.23.54:50899/EquToMes/EquToMesToolLife.asmx";

	case EMESOperate::EquipmentProcessParam:
		return "http://10.36.23.54:50899/EquToMes/EquToMesEquipmentProcessParam.asmx";

	case EMESOperate::Feeding:
		return "http://10.36.23.54:50899/EquToMes/EquToMesFeeding.asmx";

	case EMESOperate::Inbound:
		return "http://10.36.23.54:50899/EquToMes/EquToMesInbound.asmx";

	case EMESOperate::Outbound:
		return "http://10.36.23.54:50899/EquToMes/EquToMesOutbound.asmx";

	case EMESOperate::EquipmentProductProcessParam:
		return "http://10.36.23.54:50899/EquToMes/EquToMesEquipmentProductProcessParam.asmx";

	case EMESOperate::BindSFC:
		return "http://10.36.23.54:50899/EquToMes/EquToMesBindSFC.asmx";

	case EMESOperate::InboundInContainer:
		return "http://10.36.23.54:50899/EquToMes/EquToMesInboundInContainer.asmx";

	case EMESOperate::BindContainer:
		return "http://10.36.23.54:50899/EquToMes/EquToMesBindContainer.asmx";

	case EMESOperate::OutboundInSFCOutputQty:
		return "http://10.36.23.54:50899/EquToMes/EquToMesOutboundInSFCOutputQty.asmx";

	case EMESOperate::FeedingDJInLaminating:
		return "http://10.36.23.54:50899/EquToMes/EquToMesFeedingDJInLaminating.asmx";

	case EMESOperate::ExamineExists:
		return "http://10.36.23.54:50899/EquToMes/EquToMesExamineExists.asmx";

	case EMESOperate::OutboundInLaminating:
		return "http://10.36.23.54:50899/EquToMes/EquToMesOutboundInLaminating.asmx";

	case EMESOperate::GenerateCellSFC:
		return "http://10.36.23.54:50899/EquToMes/EquToMesGenerateCellSFC.asmx";

	case EMESOperate::FillingData:
		return "http://10.36.23.54:50899/EquToMes/EquToMesFillingData.asmx";

	case EMESOperate::InboundInTwoInjection:
		return "http://10.36.23.54:50899/EquToMes/EquToMesInboundInTwoInjection.asmx";

	case EMESOperate::InboundInSFCContainer:
		return "http://10.36.23.54:50899/EquToMes/EquToMesInboundInSFCContainer.asmx";

	case EMESOperate::InboundMore:
		return "http://10.36.23.54:50899/EquToMes/EquToMesInboundMore.asmx";

	case EMESOperate::OutboundMore:
		return "http://10.36.23.54:50899/EquToMes/EquToMesOutboundMore.asmx";

	case EMESOperate::CCDFileUploadComplete:
		return "http://10.36.23.54:50899/EquToMes/EquToMesCCDFileUploadComplete.asmx";

	default:
		return "";
	}
}
inline void CMesGSoap::loadConfig()
{
	// 建议从配置文件获取
	m_EquUserID = L"SVOLTTest";
	m_EquPassword = L"SVOLTTest0123";
	m_OperatorUserID = L"OperatorGW00Test";
	m_OperatorPassword = L"OperatorGW00Test0123";
	m_EquipmentCode = L"xx1";
	m_ResourceCode = L"xx2";
}

template <typename T> 
void CMesGSoap::setupSoap(T& soap)
{
	soap.connect_timeout = 3;
	soap.accept_timeout = 3;
	soap.send_timeout = 30;
	soap.recv_timeout = 100;
	soap.transfer_timeout = 3;

}

void CMesGSoap::setupSoaps()
{
	//setupSoap(loginSoap);
	//
	//setupSoap(stateSoap);
	//setupSoap(alarmSoap);
	//setupSoap(equipmentRrocessParamSoap);
	//setupSoap(m_RecipeSoap);
	//setupSoap(m_RecipeVersionExamineSoap);
	//setupSoap(m_heartSoap);
	//setupSoap(m_equipmentProductRrocessParamSoap);
	//setupSoap(m_getRecipeListSoap);
	//setupSoap(m_getRecipeSoap);

}

CMesGSoap& CMesGSoap::getInstance() 
{
	if (NULL == m_instance)
	{
		m_instance = new CMesGSoap();
	}
	return *m_instance;
}

CString CMesGSoap::GetCurrentLocalTime()
{
	time_t timep;
	time(&timep);
	struct tm *p;
	CString cstrCurrentLocalTime;
	p = localtime(&timep); /*转换为本地的tm结构的时间按*/
	cstrCurrentLocalTime.Format("%04d-%02d-%02dT%02d:%02d:%02d",p->tm_year + 1900,p->tm_mon + 1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	return	cstrCurrentLocalTime;
}
CString CMesGSoap::GetCurrentLocalTime(const time_t & tTime)
{
	struct tm *p;
	CString cstrCurrentLocalTime;
	p = localtime(&tTime); /*转换为本地的tm结构的时间按*/
	cstrCurrentLocalTime.Format("%04d-%02d-%02dT%02d:%02d:%02d",p->tm_year + 1900,p->tm_mon + 1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	return	cstrCurrentLocalTime;
}
CString CMesGSoap::GetSystemLocalTime()
{
	CString cstrCurrent;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	cstrCurrent.Format("%04d/%02d/%02d-%02d:%02d:%02d.%03d",
		sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	return cstrCurrent;
}
ns1__LoginSoapHeader CMesGSoap::getLoginHeader()
{
	ns1__LoginSoapHeader header;
	header.EquUserID = (wchar_t*)(m_EquUserID.GetString());
	header.EquPassword = (wchar_t*)(m_EquPassword.GetString());
	return header;
}

ns1__LoginSoapHeader CMesGSoap::getHeader()
{
	ns1__LoginSoapHeader header;
	header.EquUserID = (wchar_t*)reinterpret_cast<const char*>(m_EquUserID.GetString());
	header.EquPassword = (wchar_t*)reinterpret_cast<const char*>(m_EquPassword.GetString());
	header.OperatorUserID = (wchar_t*)reinterpret_cast<const char*>(m_OperatorUserID.GetString());
	header.OperatorPassword = (wchar_t*)reinterpret_cast<const char*>(m_OperatorPassword.GetString());
	return header;
}
void CMesGSoap::setUserInfo(const CString& vEquUserID, const CString& vEquPassword)
{
	m_EquUserID = CStrA2CStrW(vEquUserID);
	m_EquPassword = CStrA2CStrW(vEquPassword);
}

void CMesGSoap::setOperatorInfo(const CString& vOperatorUserID, const CString& vOperatorPassword)
{
	m_OperatorUserID = CStrA2CStrW(vOperatorUserID);
	m_OperatorPassword = CStrA2CStrW(vOperatorPassword);
}

void CMesGSoap::setEquipmentInfo(const CString& vEquipmentCode, const CString& vResourceCode)
{
	m_EquipmentCode = CStrA2CStrW(vEquipmentCode);
	m_ResourceCode = CStrA2CStrW(vResourceCode);
}

void CMesGSoap::setInfo(const CString& vEquUserID, const CString& vEquPassword, const CString& vOperatorUserID,
	const CString& vOperatorPassword,const CString& vEquipmentCode,const CString& vResourceCode)
{
	CLockHelper lockHelp(&m_csSoap);
	m_EquUserID = CStrA2CStrW(vEquUserID);
	m_EquPassword = CStrA2CStrW(vEquPassword);
	m_OperatorUserID = CStrA2CStrW(vOperatorUserID);
	m_OperatorPassword = CStrA2CStrW(vOperatorPassword);
	m_EquipmentCode = CStrA2CStrW(vEquipmentCode);
	m_ResourceCode = CStrA2CStrW(vResourceCode);
}
void CMesGSoap::setMesAddrAndPort(const CString& strIP, const int& nPort)
{
	m_cstrMesIP = strIP;
	m_nMesPort = nPort;
	setUrlArray();
}
BOOL	CMesGSoap::isConnected()
{
	return m_bConnected;
}

int CMesGSoap::Heartbeat(bool isOnline )
{
	CLockHelper lockHelp(&m_csSoap);
	EquToMesAlarmSoapProxy heartSoap;
	_ns1__Heartbeat heartSend;
	_ns1__HeartbeatResponse heartRcv;
	ns1__Heartbeat body;
	time_t sendTime;
	time(&sendTime);
	body.IsOnline = isOnline;
	body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	body.ResourceCode = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	body.LocalTime = sendTime; 
	heartSend.model = &body;
	heartSoap.soap_header(&getHeader());
	int res = heartSoap.Heartbeat(getEndpoint(EMESOperate::Heartbeat), nullptr, &heartSend, heartRcv);
	if (NULL == heartRcv.HeartbeatResult)
	{
		m_bConnected = FALSE;
	}
	else
	{
		m_bConnected = TRUE;
	}
	return res;
}

int CMesGSoap::Heartbeat(bool isOnline, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader,nebCJsonBody;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();
	EquToMesAlarmSoapProxy heartSoap;
	_ns1__Heartbeat heartSend;
	_ns1__HeartbeatResponse heartRcv;
	ns1__Heartbeat body;
	body.IsOnline = isOnline;
	body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	body.ResourceCode = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	body.LocalTime = sendTime; // 此处时间请百度怎么转化
	heartSend.model = &body;
	heartSoap.soap_header(&getHeader());

	//发送的内容  转为json 传出去，方便记录日志
	{
		nebCJsonHeader.Add("EquUserID",CStrW2CStrA(m_EquUserID).GetString());
		nebCJsonHeader.Add("EquPassword",CStrW2CStrA(m_EquPassword).GetString());
		nebCJsonHeader.Add("OperatorUserID",CStrW2CStrA(m_OperatorUserID).GetString());
		nebCJsonHeader.Add("OperatorPassword",CStrW2CStrA(m_OperatorPassword).GetString());
		nebCJsonBody.Add("IsOnline",isOnline);
		nebCJsonBody.Add("EquipmentCode",CStrW2CStrA(m_EquipmentCode).GetString());
		nebCJsonBody.Add("ResourceCode",CStrW2CStrA(m_ResourceCode).GetString());
		nebCJsonBody.Add("LocalTime",GetCurrentLocalTime(sendTime).GetString());
		cJsonSend.Add("SendTime",GetSystemLocalTime().GetString());
		cJsonSend.AddEmptySubArray("header");
		cJsonSend.AddEmptySubArray("body");
		cJsonSend["header"].Add(nebCJsonHeader);
		cJsonSend["body"].Add(nebCJsonBody);
	}
	int res = heartSoap.Heartbeat(getEndpoint(EMESOperate::Heartbeat), nullptr, &heartSend, heartRcv);
	//接收内容 转为json 传出去，方便记日志
	{
		GetRcvCJson(cJsonRcv,EMESOperate::Heartbeat);
		cJsonRcv.Replace("Res",res);
		if (heartRcv.HeartbeatResult)
		{
			CStringW cstrwMsg(heartRcv.HeartbeatResult->Msg);
			m_bConnected = TRUE;
			cJsonRcv.Replace("code",heartRcv.HeartbeatResult->Code);
			cJsonRcv.Replace("msg",CStrW2CStrA(cstrwMsg).GetString());
		}
		else
		{
			m_bConnected = FALSE;
		}
	}
	return res;
}


int CMesGSoap::CCDFileUploadComplete(const std::vector<ParamCCDUploadFile>& vecParam )
{
	
	CLockHelper lockHelp(&m_csSoap);
	time_t  sendTime ;
	time(&sendTime);
	EquToMesAlarmSoapProxy ccdFileUploadCompleteSoap;
	_ns1__CCDFileUploadComplete ccdFileUploadCompleteSend;
	_ns1__CCDFileUploadCompleteResponse ccdFileUploadCompleteRcv;
	ns1__CCDFileUploadComplete body;
	ns1__ArrayOfCCDFileUploadCompleteSFC sfcArray;
	std::vector<ns1__CCDFileUploadCompleteSFC> vecSfc;
	for (std::vector<ParamCCDUploadFile>::const_iterator itr = vecParam.begin();itr != vecParam.end();itr++)
	{
		ns1__CCDFileUploadCompleteSFC	sfc;
		sfc.SFC =(wchar_t*)reinterpret_cast<const char*>(CStrA2CStrW(itr->cstrSFC).GetString());
		sfc.Passed = (int *)const_cast<const int*>(&itr->nPassed);			//空为合格
		sfc.URI = (wchar_t*)reinterpret_cast<const char*>(CStrA2CStrW(itr->cstrURI).GetString());
		sfc.Timestamp = sendTime;
		vecSfc.push_back(sfc);
	//	sfcArray.CCDFileUploadCompleteSFC.push_back(&sfc);
	}
	for (int i = 0; i < vecSfc.size();i++)
	{
		sfcArray.CCDFileUploadCompleteSFC.push_back(&vecSfc[i]);
	}
	body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	body.ResourceCode  = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	body.LocalTime	   = sendTime;
	body.SFCs = &sfcArray;

	ccdFileUploadCompleteSend.model = &body;
	ccdFileUploadCompleteSoap.soap_header(&getHeader());
	int res = ccdFileUploadCompleteSoap.CCDFileUploadComplete(getEndpoint(EMESOperate::CCDFileUploadComplete),nullptr,&ccdFileUploadCompleteSend,ccdFileUploadCompleteRcv);
	{		
		if (ccdFileUploadCompleteRcv.CCDFileUploadCompleteResult)
		{
			
			m_bConnected = TRUE;
		
		}
		else
		{
			m_bConnected = FALSE;
		}
	}
	return res;
}

int CMesGSoap::CCDFileUploadComplete(const std::vector<ParamCCDUploadFile>& vecParam ,neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader,nebCJsonBody,nebCJsonSFCs;
	CLockHelper lockHelp(&m_csSoap);
	time_t  sendTime ;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();
	EquToMesAlarmSoapProxy ccdFileUploadCompleteSoap;
	_ns1__CCDFileUploadComplete ccdFileUploadCompleteSend;
	_ns1__CCDFileUploadCompleteResponse ccdFileUploadCompleteRcv;
	ns1__CCDFileUploadComplete body;
	ns1__ArrayOfCCDFileUploadCompleteSFC sfcArray;

	std::vector<ns1__CCDFileUploadCompleteSFC> vecSfc;

	std::vector<CStringW> vecStrwSFC;
	std::vector<CStringW> vecStrwURI;
	
	
	for (std::vector<ParamCCDUploadFile>::const_iterator itr = vecParam.begin();itr != vecParam.end();itr++)
	{
		ns1__CCDFileUploadCompleteSFC	sfc;
		neb::CJsonObject	cJsonSFC;

		CStringW cstrwSFC = CStrA2CStrW(itr->cstrSFC);
		CStringW cstrURI = CStrA2CStrW(itr->cstrURI);
		vecStrwSFC.push_back(cstrwSFC);
		vecStrwURI.push_back(cstrURI);

		sfc.SFC =(wchar_t*)reinterpret_cast<const char*>((vecStrwSFC.at(itr - vecParam.begin())).GetString());
		sfc.Passed = (int *)const_cast<const int*>(&itr->nPassed);			//空为合格
		sfc.URI = (wchar_t*)reinterpret_cast<const char*>((vecStrwURI.at(itr - vecParam.begin())).GetString());
		sfc.Timestamp = sendTime;
		vecSfc.push_back(sfc);
	//	sfcArray.CCDFileUploadCompleteSFC.push_back(&sfc);

		cJsonSFC.Add("SFC",itr->cstrSFC.GetString());
		cJsonSFC.Add("Passed",itr->nPassed);
		cJsonSFC.Add("URI",itr->cstrURI.GetString());
		cJsonSFC.Add("Timestamp",GetCurrentLocalTime(sendTime).GetString());
		nebCJsonSFCs.Add(cJsonSFC);
	}
	for (int i = 0; i < vecSfc.size();i++)
	{
		sfcArray.CCDFileUploadCompleteSFC.push_back(&vecSfc[i]);
	}
	body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	body.ResourceCode  = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	body.LocalTime	   = sendTime;
	body.SFCs = &sfcArray;

	{
		nebCJsonHeader.Add("EquUserID",CStrW2CStrA(m_EquUserID).GetString());
		nebCJsonHeader.Add("EquPassword",CStrW2CStrA(m_EquPassword).GetString());
		nebCJsonHeader.Add("OperatorUserID",CStrW2CStrA(m_OperatorUserID).GetString());
		nebCJsonHeader.Add("OperatorPassword",CStrW2CStrA(m_OperatorPassword).GetString());

		nebCJsonBody.Add("EquipmentCode",CStrW2CStrA(m_EquipmentCode).GetString());
		nebCJsonBody.Add("ResourceCode",CStrW2CStrA(m_ResourceCode).GetString());
		nebCJsonBody.Add("LocalTime",GetCurrentLocalTime(sendTime).GetString());

		cJsonSend.Add("SendTime",GetSystemLocalTime().GetString());
		cJsonSend.AddEmptySubArray("header");
		cJsonSend.AddEmptySubArray("body");
	 	cJsonSend.AddEmptySubArray("SFCs");
		cJsonSend["header"].Add(nebCJsonHeader);
		cJsonSend["body"].Add(nebCJsonBody);
		cJsonSend["SFCs"].Add(nebCJsonSFCs);
	//	cJsonSend.Add("SFCs",nebCJsonSFCs);
	}


	ccdFileUploadCompleteSend.model = &body;
	ccdFileUploadCompleteSoap.soap_header(&getHeader());
	int res = ccdFileUploadCompleteSoap.CCDFileUploadComplete(getEndpoint(EMESOperate::CCDFileUploadComplete),nullptr,&ccdFileUploadCompleteSend,ccdFileUploadCompleteRcv);
	
	{	
		GetRcvCJson(cJsonRcv,EMESOperate::CCDFileUploadComplete);
		cJsonRcv.Replace("Res",res);
		if (ccdFileUploadCompleteRcv.CCDFileUploadCompleteResult)
		{
			CStringW cstrwMsg(ccdFileUploadCompleteRcv.CCDFileUploadCompleteResult->Msg);
			m_bConnected = TRUE;
			cJsonRcv.Replace("code",ccdFileUploadCompleteRcv.CCDFileUploadCompleteResult->Code);
			cJsonRcv.Replace("msg",CStrW2CStrA(cstrwMsg).GetString());
		}
		else
		{
			m_bConnected = FALSE;
		}
	}
	return res;
}

int CMesGSoap::GetRecipeList(const CString& strProductCode,neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader,nebCJsonBody,nebCJsonSFCs,nebCJsonParamList;
	CLockHelper lockHelp(&m_csSoap);
	time_t  sendTime ;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();
	EquToMesAlarmSoapProxy getRecipeListSoap;
	_ns1__GetRecipeList getRecipeListSend;
	_ns1__GetRecipeListResponse getRecipeListRcv;
	ns1__RecipeListGet	 body;
	CStringW cstrwProductCode = CStrA2CStrW(strProductCode);
	body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	body.ResourceCode  = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	body.ProductCode = (wchar_t*)reinterpret_cast<const char*>(cstrwProductCode.GetString());
	body.LocalTime	   = sendTime;
	{
		nebCJsonHeader.Add("EquUserID",CStrW2CStrA(m_EquUserID).GetString());
		nebCJsonHeader.Add("EquPassword",CStrW2CStrA(m_EquPassword).GetString());
		nebCJsonHeader.Add("OperatorUserID",CStrW2CStrA(m_OperatorUserID).GetString());
		nebCJsonHeader.Add("OperatorPassword",CStrW2CStrA(m_OperatorPassword).GetString());
		nebCJsonBody.Add("EquipmentCode",CStrW2CStrA(m_EquipmentCode).GetString());
		nebCJsonBody.Add("ResourceCode",CStrW2CStrA(m_ResourceCode).GetString());
		nebCJsonBody.Add("ProductCode",strProductCode.GetString());
		nebCJsonBody.Add("LocalTime",GetCurrentLocalTime(sendTime).GetString());
		cJsonSend.Add("SendTime",GetSystemLocalTime().GetString());
		cJsonSend.AddEmptySubArray("header");
		cJsonSend.AddEmptySubArray("body");
		cJsonSend["header"].Add(nebCJsonHeader);
		cJsonSend["body"].Add(nebCJsonBody);
	}
	getRecipeListSend.model = &body;
	getRecipeListSoap.soap_header(&getHeader());
	int res = getRecipeListSoap.GetRecipeList(getEndpoint(EMESOperate::GetRecipeList),nullptr,&getRecipeListSend,getRecipeListRcv);
	{	
		GetRcvCJson(cJsonRcv,EMESOperate::GetRecipeList);
		cJsonRcv.Replace("Res",res);
		cJsonRcv.AddEmptySubArray("Data");
		if (getRecipeListRcv.GetRecipeListResult)
		{
			CStringW cstrwMsg(getRecipeListRcv.GetRecipeListResult->Msg);
			m_bConnected = TRUE;
			cJsonRcv.Replace("code",getRecipeListRcv.GetRecipeListResult->Code);
			cJsonRcv.Replace("msg",CStrW2CStrA(cstrwMsg).GetString());
			if (getRecipeListRcv.GetRecipeListResult->Data)
			{
				for (std::vector<ns1__RecipeListGetForEqu *>::iterator itr = getRecipeListRcv.GetRecipeListResult->Data->RecipeListGetForEqu.begin();
					itr != getRecipeListRcv.GetRecipeListResult->Data->RecipeListGetForEqu.end();itr++)
				{
					neb::CJsonObject cJson_d;
					CStringW cstrwRecipeCode((*itr)->RecipeCode);
					CStringW cstrwProductCode((*itr)->ProductCode);
					CStringW cstrwVersion((*itr)->Version);
					time_t lastUpdateTime = (*itr)->LastUpdateOnTime;	
					cJson_d.Add("RecipeCode",CStrW2CStrA(cstrwRecipeCode).GetString());
					cJson_d.Add("ProductCode",CStrW2CStrA(cstrwProductCode).GetString());
					cJson_d.Add("Version",CStrW2CStrA(cstrwVersion).GetString());
					cJson_d.Add("LastUpdateOnTime",GetCurrentLocalTime(lastUpdateTime).GetString());
					cJsonRcv["Data"].Add(cJson_d);
				}
				
			}
			
		}
		else
		{
			m_bConnected = FALSE;
		}
	}
	return res;
}
int CMesGSoap::GetRecipe(const CString& strRecipeCode,neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader,nebCJsonBody,nebCJsonSFCs,nebCJsonParamList;
	CLockHelper lockHelp(&m_csSoap);
	time_t  sendTime ;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();
	EquToMesAlarmSoapProxy getRecipeSoap;
	_ns1__GetRecipe getRecipeSend;
	_ns1__GetRecipeResponse getRecipeRcv;
	ns1__RecipeGet	 body;
	CStringW cstrwRecipeCode = CStrA2CStrW(strRecipeCode);
	body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	body.ResourceCode  = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	body.RecipeCode = (wchar_t*)reinterpret_cast<const char*>(cstrwRecipeCode.GetString());
	body.LocalTime	   = sendTime;
	setupSoap(getRecipeSoap);
	{
		nebCJsonHeader.Add("EquUserID",CStrW2CStrA(m_EquUserID).GetString());
		nebCJsonHeader.Add("EquPassword",CStrW2CStrA(m_EquPassword).GetString());
		nebCJsonHeader.Add("OperatorUserID",CStrW2CStrA(m_OperatorUserID).GetString());
		nebCJsonHeader.Add("OperatorPassword",CStrW2CStrA(m_OperatorPassword).GetString());
		nebCJsonBody.Add("EquipmentCode",CStrW2CStrA(m_EquipmentCode).GetString());
		nebCJsonBody.Add("ResourceCode",CStrW2CStrA(m_ResourceCode).GetString());
		nebCJsonBody.Add("RecipeCode",strRecipeCode.GetString());
		nebCJsonBody.Add("LocalTime",GetCurrentLocalTime(sendTime).GetString());
		cJsonSend.Add("SendTime",GetSystemLocalTime().GetString());
		cJsonSend.AddEmptySubArray("header");
		cJsonSend.AddEmptySubArray("body");
		cJsonSend["header"].Add(nebCJsonHeader);
		cJsonSend["body"].Add(nebCJsonBody);
	}
	getRecipeSend.model = &body;
	getRecipeSoap.soap_header(&getHeader());
	int res = getRecipeSoap.GetRecipe(getEndpoint(EMESOperate::GetRecipe),nullptr,&getRecipeSend,getRecipeRcv);
	{	
		GetRcvCJson(cJsonRcv,EMESOperate::GetRecipe);
		cJsonRcv.Replace("Res",res);
		//cJsonRcv.AddEmptySubArray("Data");
		if (getRecipeRcv.GetRecipeResult)
		{
			CStringW cstrwMsg(getRecipeRcv.GetRecipeResult->Msg);
			m_bConnected = TRUE;
			cJsonRcv.Replace("code",getRecipeRcv.GetRecipeResult->Code);
			cJsonRcv.Replace("msg",CStrW2CStrA(cstrwMsg).GetString());
			if (!getRecipeRcv.GetRecipeResult->Data)
			{
				return res;
			}
			CStringW cstrwVersion(getRecipeRcv.GetRecipeResult->Data->Version);
			neb::CJsonObject cJsonData;

			cJsonData.Add("Version",CStrW2CStrA(cstrwVersion).GetString());
			cJsonData.Add("LastUpdateOnTime",GetCurrentLocalTime(getRecipeRcv.GetRecipeResult->Data->LastUpdateOnTime).GetString());

		//	cJsonRcv["Data"].Add("Version",CStrW2CStrA(cstrwVersion).GetString());
		//	cJsonRcv["Data"].Add("LastUpdateOnTime",GetCurrentLocalTime(getRecipeRcv.GetRecipeResult->Data->LastUpdateOnTime).GetString());
			cJsonData.AddEmptySubArray("ParamList");
			if (getRecipeRcv.GetRecipeResult->Data->ParamList)
			{
			//	neb::CJsonObject cJsonParamList;
			//	cJsonRcv["Data"].AddEmptySubArray("ParamList");
				for (std::vector<ns1__RecipeGetParamForEqu *>::iterator itr = getRecipeRcv.GetRecipeResult->Data->ParamList->RecipeGetParamForEqu.begin();
					itr != getRecipeRcv.GetRecipeResult->Data->ParamList->RecipeGetParamForEqu.end();itr++)
				{
					neb::CJsonObject cJson_d;
					CStringW cstrwParamCode((*itr)->ParamCode);
					CStringW cstrwParamValue((*itr)->ParamValue);
					CStringW cstrwParamLower((*itr)->ParamLower);
					CStringW cstrwParamUpper((*itr)->ParamUpper);
					cJson_d.Add("ParamCode",CStrW2CStrA(cstrwParamCode).GetString());
					cJson_d.Add("ParamValue",CStrW2CStrA(cstrwParamValue).GetString());
					cJson_d.Add("ParamLower",CStrW2CStrA(cstrwParamLower).GetString());
					cJson_d.Add("ParamUpper",CStrW2CStrA(cstrwParamUpper).GetString());
					cJsonData["ParamList"].Add(cJson_d);
				}
			//	cJsonRcv["Data"].AddEmptySubArray("ParamList");
				cJsonRcv.Add("Data",cJsonData);
			}
		}
		else
		{
			m_bConnected = FALSE;
		}
	}
	return res;
}

void CMesGSoap::GetRcvCJson(neb::CJsonObject& cJsonRcv, const CString & cstrAPI)
{
	cJsonRcv.Add("RcvTime",GetSystemLocalTime().GetString());
	cJsonRcv.Add("Res",-100);
	cJsonRcv.Add("API",cstrAPI.GetString());
	cJsonRcv.Add("code",-1);						//	1为成功，其他为失败
	cJsonRcv.Add("msg","接收消息为空");
	cJsonRcv.Add("time",GetCurrentLocalTime().GetString());
}
void CMesGSoap::GetRcvCJson(neb::CJsonObject& cJsonRcv, const EMESOperate& op)
{
	cJsonRcv.Add("RcvTime",GetSystemLocalTime().GetString());
	cJsonRcv.Add("Res",-100);
	cJsonRcv.Add("API",getEndpoint(op));
	cJsonRcv.Add("code",-1);						//	1为成功，其他为失败
	cJsonRcv.Add("msg","接收消息为空");
	cJsonRcv.Add("time",GetCurrentLocalTime().GetString());
}

int CMesGSoap::Recipe(const ParamRecipe& paramRecipe, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody, nebCJsonParamList;
	CLockHelper lockHelp(&m_csSoap);
	time_t  sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();
	EquToMesAlarmSoapProxy RecipeSoap;
	_ns1__Recipe RecipeSend;
	_ns1__RecipeResponse RecipeRcv;
	ns1__Recipe	 body;
	ns1__ArrayOfRecipeParam arrayOfRecipeParam;
	std::vector<ns1__RecipeParam> vecRecipeParam;
	std::vector<CStringW> vecCstrwCode;
	std::vector<CStringW> vecCstrwValue;
	std::vector<CStringW> vecCstrwUpper;
	std::vector<CStringW> vecCstrwLower;
	for (std::vector<ParamRecipeBase>::const_iterator itr = paramRecipe.vecParamList.begin(); itr != paramRecipe.vecParamList.end(); itr++)
	{
		ns1__RecipeParam recipeParam;
		neb::CJsonObject cJsonParam;
		CStringW cstrwCode = CStrA2CStrW(itr->cstrParamCode);
		CStringW cstrwValue = CStrA2CStrW(itr->cstrParamValue);
		CStringW cstrwUpper = CStrA2CStrW(itr->cstrParamUpper);
		CStringW cstrwLower = CStrA2CStrW(itr->cstrParamLower);
		vecCstrwCode.push_back(cstrwCode);
		vecCstrwValue.push_back(cstrwValue);
		vecCstrwUpper.push_back(cstrwUpper);
		vecCstrwLower.push_back(cstrwLower);
		recipeParam.ParamCode = (wchar_t*)reinterpret_cast<const char*>(vecCstrwCode.at(itr - paramRecipe.vecParamList.begin()).GetString());
		recipeParam.ParamValue = (wchar_t*)reinterpret_cast<const char*>(vecCstrwValue.at(itr - paramRecipe.vecParamList.begin()).GetString());
		recipeParam.ParamUpper = (wchar_t*)reinterpret_cast<const char*>(vecCstrwUpper.at(itr - paramRecipe.vecParamList.begin()).GetString());
		recipeParam.ParamLower = (wchar_t*)reinterpret_cast<const char*>(vecCstrwLower.at(itr - paramRecipe.vecParamList.begin()).GetString());
		recipeParam.Timestamp = itr->timeStamp;
		vecRecipeParam.push_back(recipeParam);
		//	arrayOfRecipeParam.RecipeParam.push_back(&recipeParam);

		cJsonParam.Add("ParamCode", itr->cstrParamCode.GetString());
		cJsonParam.Add("ParamValue", itr->cstrParamValue.GetString());
		cJsonParam.Add("ParamUpper", itr->cstrParamUpper.GetString());
		cJsonParam.Add("ParamLower", itr->cstrParamLower.GetString());
		cJsonParam.Add("Timestamp", GetCurrentLocalTime(itr->timeStamp).GetString());
		nebCJsonParamList.Add(cJsonParam);
	}
	for (int i = 0; i < vecRecipeParam.size(); i++)
	{
		arrayOfRecipeParam.RecipeParam.push_back(&vecRecipeParam[i]);
	}
	CStringW cstrwRecipeCode = CStrA2CStrW(paramRecipe.cstrRecipeCode);
	CStringW cstrwProductCode = CStrA2CStrW(paramRecipe.cstrProductCode);
	CStringW cstrwVersion = CStrA2CStrW(paramRecipe.cstrVersion);
	body.EquipmentCode = (wchar_t*)reinterpret_cast<const char*>(m_EquipmentCode.GetString());
	body.ResourceCode = (wchar_t*)reinterpret_cast<const char*>(m_ResourceCode.GetString());
	body.RecipeCode = (wchar_t*)reinterpret_cast<const char*>(cstrwRecipeCode.GetString());
	body.ProductCode = (wchar_t*)reinterpret_cast<const char*>(cstrwProductCode.GetString());
	body.Version = (wchar_t*)reinterpret_cast<const char*>(cstrwVersion.GetString());
	body.LocalTime = sendTime;
	{
		nebCJsonHeader.Add("EquUserID", CStrW2CStrA(m_EquUserID).GetString());
		nebCJsonHeader.Add("EquPassword", CStrW2CStrA(m_EquPassword).GetString());
		nebCJsonHeader.Add("OperatorUserID", CStrW2CStrA(m_OperatorUserID).GetString());
		nebCJsonHeader.Add("OperatorPassword", CStrW2CStrA(m_OperatorPassword).GetString());

		nebCJsonBody.Add("EquipmentCode", CStrW2CStrA(m_EquipmentCode).GetString());
		nebCJsonBody.Add("ResourceCode", CStrW2CStrA(m_ResourceCode).GetString());
		nebCJsonBody.Add("RecipeCode", paramRecipe.cstrRecipeCode.GetString());
		nebCJsonBody.Add("ProductCode", paramRecipe.cstrProductCode.GetString());
		nebCJsonBody.Add("Version", paramRecipe.cstrVersion.GetString());
		nebCJsonBody.Add("LocalTime", GetCurrentLocalTime(sendTime).GetString());

		cJsonSend.Add("SendTime", GetSystemLocalTime().GetString());
		cJsonSend.AddEmptySubArray("header");
		cJsonSend.AddEmptySubArray("body");
		cJsonSend["header"].Add(nebCJsonHeader);
		cJsonSend["body"].Add(nebCJsonBody);
		cJsonSend.Add("ParamList", nebCJsonParamList);
	}
	RecipeSend.model = &body;
	RecipeSoap.soap_header(&getHeader());
	int res = RecipeSoap.Recipe(getEndpoint(EMESOperate::Recipe), nullptr, &RecipeSend, RecipeRcv);

	{
		GetRcvCJson(cJsonRcv, EMESOperate::Recipe);
		cJsonRcv.Replace("Res", res);
		cJsonRcv.AddEmptySubArray("Data");
		if (RecipeRcv.RecipeResult)
		{
			CStringW cstrwMsg(RecipeRcv.RecipeResult->Msg);
			m_bConnected = TRUE;
			cJsonRcv.Replace("code", RecipeRcv.RecipeResult->Code);
			cJsonRcv.Replace("msg", CStrW2CStrA(cstrwMsg).GetString());
		}
		else
		{
			m_bConnected = FALSE;
		}
	}
	return res;
}