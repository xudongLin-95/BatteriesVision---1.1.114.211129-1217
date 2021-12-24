#include "stdafx.h"
#include "resource.h"
#include "PowerLimitManage.h"
#include "DlgUser.h"
#include "DlgUserManage.h"
#include "DlgUserPower.h"

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new PowerLimitManage();
}

CString PowerLimit::m_strStaticFactoryPW = "daheng";

PowerLimit::PowerLimit(void)
{
	m_ePowerType = PT_NONE;
	m_bLogin = FALSE;
}

PowerLimit::~PowerLimit(void)
{
}

void PowerLimit::setPowerType(ePOWERLIMITTYPE type)
{
	m_ePowerType = type;
}

ePOWERLIMITTYPE PowerLimit::getPowerType()
{
	return m_ePowerType;
}

void PowerLimit::setJobNumber(CString strJobNum)
{
	m_strJobNumber = strJobNum;
}

CString PowerLimit::getJobNumber()
{
	return m_strJobNumber;
}
//新加卡号 BY YS
void PowerLimit::setCardNumber(CString strCardNum)
{
	m_strCardNumber = strCardNum;
}
//通过工号获取卡号
CString PowerLimit::getCardNumber()
{
	return m_strCardNumber;
}


void PowerLimit::setPassword(CString strPW)
{
	m_strPassword = strPW;
}

CString PowerLimit::getPassword()
{
	return m_strPassword;
}

void PowerLimit::setLogin(BOOL bLogin)
{
	m_bLogin = bLogin;
}

BOOL PowerLimit::getLogin()
{
	return m_bLogin;
}

void PowerLimit::Clear()
{
	m_ePowerType = PT_NONE;
	m_strJobNumber = "";
	m_strCardNumber = "";
	m_strPassword = "";
	m_bLogin = FALSE;
}

BOOL PowerLimit::match(const PowerLimit & other)
{
	return (m_ePowerType == other.m_ePowerType &&
		    m_strJobNumber == other.m_strJobNumber &&
		    m_strPassword == other.m_strPassword &&
		    m_strCardNumber);
}

PowerLimitList::PowerLimitList(void)
{
}

PowerLimitList::~PowerLimitList(void)
{
	m_vPowerLimit.clear();
}

BOOL PowerLimitList::find(PowerLimit& data)
{
	//时间
	SYSTEMTIME	tSaveTime;
	::GetLocalTime(&tSaveTime);
	CString sPassword;
	sPassword.Format("%s%02d%02d", PowerLimit::m_strStaticFactoryPW, tSaveTime.wMonth, tSaveTime.wDay);

	//暂时屏蔽，去掉厂家权限 
	if (data.getPassword() == sPassword)
	{
		data.setPowerType(PT_SUPER_ADMINISTRATOR);//PT_FACTORY
		return TRUE;
	}

	std::vector<PowerLimit>::iterator iter;
	for (iter = m_vPowerLimit.begin(); iter != m_vPowerLimit.end(); iter++)
	{
		if (iter->match(data))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL PowerLimitList::isExit(PowerLimit data)
{
	std::vector<PowerLimit>::iterator iter;
	for (iter = m_vPowerLimit.begin(); iter != m_vPowerLimit.end(); iter++)
	{
		if (iter->getJobNumber() == data.getJobNumber() || iter->getCardNumber() == data.getCardNumber())//工号或卡号一样，即表示用户已存在
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL PowerLimitList::addData(PowerLimit data)
{
	std::vector<PowerLimit>::iterator iter;
	for (iter = m_vPowerLimit.begin(); iter != m_vPowerLimit.end(); iter++)
	{
		if (data.getPowerType() == iter->getPowerType() &&
			data.getJobNumber() == iter->getJobNumber() &&
			data.getCardNumber() == iter->getCardNumber())
		{
			return FALSE;
		}
	}

	m_vPowerLimit.push_back(data);
	return TRUE;
}

BOOL PowerLimitList::delData(PowerLimit data)
{
	std::vector<PowerLimit>::iterator iter;
	for (iter = m_vPowerLimit.begin(); iter != m_vPowerLimit.end(); iter++)
	{
		if (iter->match(data))
		{
			m_vPowerLimit.erase(iter);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL PowerLimitList::editData(PowerLimit data)
{
	std::vector<PowerLimit>::iterator iter;
	for (iter = m_vPowerLimit.begin(); iter != m_vPowerLimit.end(); iter++)
	{
		if (data.getPowerType() == iter->getPowerType() &&
			data.getJobNumber() == iter->getJobNumber() &&
			data.getCardNumber() == iter->getCardNumber())
		{
			iter->setPassword(data.getPassword());
			return TRUE;
		}
	}
	return FALSE;
}

int PowerLimitList::getCount()
{
	return m_vPowerLimit.size();
}

PowerLimit PowerLimitList::getPowerLimit(int nIndex)
{
	if (0 <= nIndex && nIndex < getCount())
	{
		return m_vPowerLimit[nIndex];
	}
	else
	{
		assert(0);
	}

	PowerLimit data;
	return data;
}

PowerLimit* PowerLimitList::getPowerLimitByJN(CString strJN)
{
	// TODO: 在此处插入 return 语句
	std::vector<PowerLimit>::iterator iter;
	for (iter = m_vPowerLimit.begin(); iter != m_vPowerLimit.end(); iter++)
	{
		if (strJN == iter->getJobNumber())
		{
			return &(*iter);
		}
	}

	return NULL;
}

void PowerLimitList::clear()
{
	m_vPowerLimit.clear();
}

PowerLimitManage::PowerLimitManage()
{
	m_hv_TimeStart = 0;
	m_hv_TimeEnd = 0;
}


PowerLimitManage::~PowerLimitManage()
{
}

int PowerLimitManage::DoSomething(void * message, void * wParam, void * lParam)
{
	CString strMessage = (char*)message;
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage || "Init" == strMessage)
	{
		return Init((PPOWERLIMITINIT)wParam);
	}
	else if ("LOGIN" == strMessage || "Login" == strMessage)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		DlgUser dlg;
		dlg.m_pOwner = this;
		if (IDOK == dlg.DoModal())
		{
			//登陆成功后，获取当前用户的权限列表
			m_curPowerType.SetPowerLimitTempList();

			GetLocalTime(&m_tUserInfo.tLoginTime);
			CountSeconds(&m_hv_TimeStart);
			return TRUE;
		}
		return FALSE;
	}
	else if ("USERMANAGE" == strMessage || "UserManage" == strMessage)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		DlgUserManage dlg;
		dlg.m_pOwner = this;
		dlg.DoModal();
		return TRUE;
	}
	else if ("POWERMANAGE" == strMessage || "PowerManage" == strMessage)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		DlgUserPower dlg;
		dlg.m_pOwner = this;
		dlg.DoModal();
		return TRUE;
	}
	else if ("LOGOUT" == strMessage || "Logout" == strMessage)
	{
		m_curPowerLimit.Clear();
		m_curPowerType.Clear();
		m_hv_TimeStart = 0;
		m_hv_TimeEnd = 0;
		ClearStruct();
		return FALSE;
	}
	else if ("ENABLE" == strMessage || "Enable" == strMessage)
	{
		CString strPowerType;
		strPowerType.Format("%s", wParam);
		if ("EXIT" == strPowerType || "Exit" == strPowerType)
		{
			return m_curPowerType.GetModulePower(PM_Exit);
		}
		else if ("CHANGEBATCH" == strPowerType || "ChangeBatch" == strPowerType)
		{
			return m_curPowerType.GetModulePower(PM_ChangeBatch);
		}
		else if ("MANAGE" == strPowerType || "Manage" == strPowerType)
		{
			return m_curPowerType.GetModulePower(PM_Manage);
		}
		else if ("PRODUCT" == strPowerType || "Product" == strPowerType)
		{
			return m_curPowerType.GetModulePower(PM_Product);
		}
		else if ("DETECT" == strPowerType || "Detect" == strPowerType)
		{
			return m_curPowerType.GetModulePower(PM_Detect);
		}
	}
	else if ("GETUSERINFO" == strMessage || "GetUserInfo" == strMessage)
	{
		CString strParam;
		m_tUserInfo.ePowerType = (ePOWERLIMITTYPE)m_curPowerLimit.getPowerType();
		m_tUserInfo.strJobNumber = m_curPowerLimit.getJobNumber();
		m_tUserInfo.bLogin = m_curPowerLimit.getLogin();
		CountSeconds(&m_hv_TimeEnd);
		m_tUserInfo.hv_LoginTimeCount = m_hv_TimeEnd - m_hv_TimeStart;
		m_tUserInfo.vecPowerLimitList = m_curPowerType.GetPowerLimitTempList();

		*(PUSERINFO)wParam = m_tUserInfo;
	}
	else
	{
		CString strMessageBox;
		strMessageBox.Format("PowerLimit模块中 strMessage = %s 有误, 无对应接口", (char*)message);
		AfxMessageBox(strMessageBox);
		return FALSE;
	}

 	return FALSE;
}

int PowerLimitManage::Init(PPOWERLIMITINIT ptInit)
{
	m_tInit = *ptInit;
	try
	{
		SetSystem("border_shape_models", "false");
		m_strConfigPath.Format("%s\\Config\\Public\\PowerLimit", GetCurrentAppPath());
		CreateMultilevelPath(m_strConfigPath);
		m_strConfigFile.Format("%s\\Config\\Public\\PowerLimit\\Config.dat", GetCurrentAppPath());
		Read();


	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("PowerLimitManage_Init: 初始化异常 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strExcp, ExcpLog, SlightExcp);
	}


	return TRUE;
}

BOOL PowerLimitManage::Log(const CString& strLog, const int& nType, const int&nExcpLevel)
{
	if (m_tInit.pLog == NULL) return FALSE;
	return m_tInit.pLog(strLog, nType, nExcpLevel);
}

void PowerLimitManage::Read()
{
	//所有用户信息
	m_PowerLimitList.clear();
	int nPLcount = GetPrivateProfileInt("POWERLIMIT", "COUNT", 0, m_strConfigFile);//用户数目
	for (int nPL = 0; nPL < nPLcount; nPL++)
	{
		PowerLimit data;
		CString strKey, strValue;
		strKey.Format("POWERLIMITTYPE%d", nPL);
		int nPowerType = GetPrivateProfileInt("POWERLIMIT", strKey, 0, m_strConfigFile);//为了测试
		data.setPowerType(ePOWERLIMITTYPE(nPowerType));
		strKey.Format("JOBNUMBER%d", nPL);
		GetPrivateProfileString("POWERLIMIT", strKey, "", strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigFile);
		data.setJobNumber(strValue);
		strKey.Format("PASSWORD%d", nPL);
		GetPrivateProfileString("POWERLIMIT", strKey, "", strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigFile);
		data.setPassword(strValue);
		strKey.Format("CARDNUMBER%d", nPL);
		GetPrivateProfileString("POWERLIMIT", strKey, "", strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigFile);
		data.setCardNumber(strValue);
		m_PowerLimitList.addData(data);
	}

	//所有权限信息
	m_vPowerType.clear();
	int nPTcount = GetPrivateProfileInt("POWERTYPE", "COUNT", 0, m_strConfigFile);
	if (nPTcount > 0)
	{
		for (int nPT = 0; nPT < nPTcount; nPT++)
		{
			PowerType data;
			CString strSection, strKey;
			strSection.Format("POWERTYPE%d", nPT);
			strKey.Format("TYPE");
			data.setPowerType(ePOWERLIMITTYPE(GetPrivateProfileInt(strSection, strKey, PT_NONE, m_strConfigFile)));
			data.GetPowerLimitList().clear();
			for (int nPM = 0; nPM < PM_COUNT; nPM++)
			{
				strKey.Format("POWERMODULE%d", nPM);
				if (nPT == PT_SUPER_ADMINISTRATOR-1)//PT_FACTORY
					data.GetPowerLimitList().push_back(true);
				else
					data.GetPowerLimitList().push_back(GetPrivateProfileInt(strSection, strKey, FALSE, m_strConfigFile));
			}
			m_vPowerType.push_back(data);
		}
	}
	else
	{
		InitDefaultPowerType();
	}
}

void PowerLimitManage::Write()
{
	//所有用户信息
	int nPLcount = m_PowerLimitList.getCount();
	WritePrivateProfileInt("POWERLIMIT", "COUNT", nPLcount, m_strConfigFile);
	for (int nPL = 0; nPL < nPLcount; nPL++)
	{
		CString strSection;
		strSection.Format("POWERLIMITTYPE%d", nPL);
		WritePrivateProfileInt("POWERLIMIT", strSection, m_PowerLimitList.getPowerLimit(nPL).getPowerType(), m_strConfigFile);
		strSection.Format("JOBNUMBER%d", nPL);
		WritePrivateProfileString("POWERLIMIT", strSection, m_PowerLimitList.getPowerLimit(nPL).getJobNumber(), m_strConfigFile);
		strSection.Format("PASSWORD%d", nPL);
		WritePrivateProfileString("POWERLIMIT", strSection, m_PowerLimitList.getPowerLimit(nPL).getPassword(), m_strConfigFile);
		strSection.Format("CARDNUMBER%d", nPL);
		WritePrivateProfileString("POWERLIMIT", strSection, m_PowerLimitList.getPowerLimit(nPL).getCardNumber(), m_strConfigFile);
	}

	//所有权限信息
	int nPTcount = m_vPowerType.size();
	WritePrivateProfileInt("POWERTYPE", "COUNT", nPTcount, m_strConfigFile);
	for (int nPT = 0; nPT < nPTcount; nPT++)
	{
		CString strSection, strKey;
		strSection.Format("POWERTYPE%d", nPT);
		strKey.Format("TYPE");
		WritePrivateProfileInt(strSection, strKey, m_vPowerType[nPT].getPowerType(), m_strConfigFile);
		for (int nPM = 0; nPM < m_vPowerType[nPT].GetPowerLimitList().size(); nPM++)
		{
			strKey.Format("POWERMODULE%d", nPM);
			WritePrivateProfileInt(strSection, strKey, m_vPowerType[nPT].GetPowerLimitList()[nPM], m_strConfigFile);
		}
	}
}

void PowerLimitManage::InitDefaultPowerType()
{
	PowerType data;
	//操作员权限 1
	data.setPowerType(PT_OPERATOR);
	data.GetPowerLimitList().resize(PM_COUNT, FALSE);
	for (int i=0;i<PM_COUNT;i++)
		data.SetModulePower((ePOWERLIMITMODULE)i, FALSE);
	data.SetModulePower(PM_ChangeBatch,TRUE);
	m_vPowerType.push_back(data);
	//技术员权限 2
	data.setPowerType(PT_TECHNICIAN);
	for (int i = 0; i < PM_COUNT; i++)
		data.SetModulePower((ePOWERLIMITMODULE)i, FALSE);
	data.SetModulePower(PM_ChangeBatch, TRUE);
	m_vPowerType.push_back(data);
	//工程师权限 3
	data.setPowerType(PT_ENGINEER);
	for (int i = 0; i < PM_COUNT; i++)
		data.SetModulePower((ePOWERLIMITMODULE)i, FALSE);
	data.SetModulePower(PM_ChangeBatch, TRUE);
	m_vPowerType.push_back(data);
	//管理员权限 4
	data.setPowerType(PT_ADMINISTRATOR);
	for (int i = 0; i < PM_COUNT; i++)
		data.SetModulePower((ePOWERLIMITMODULE)i, FALSE);
	data.SetModulePower(PM_ChangeBatch, TRUE);
	data.SetModulePower(PM_Detect, FALSE);
	m_vPowerType.push_back(data);
	//超级管理员权限 5
	data.setPowerType(PT_SUPER_ADMINISTRATOR);
	for (int i = 0; i < PM_COUNT; i++)
		data.SetModulePower((ePOWERLIMITMODULE)i, TRUE);
	data.SetModulePower(PM_ChangeBatch, TRUE);
	m_vPowerType.push_back(data);
	////厂家权限
	//data.setPowerType(PT_FACTORY);
	//for (int i = 0; i < PM_COUNT; i++)
	//	data.SetModulePower((ePOWERLIMITMODULE)i, TRUE);
	//m_vPowerType.push_back(data);
}

void PowerLimitManage::ClearStruct()
{
	m_tUserInfo.ePowerType = ePOWERLIMITTYPE::PT_NONE;
	m_tUserInfo.strJobNumber = "";
	m_tUserInfo.bLogin = FALSE;

	m_tUserInfo.vecPowerLimitList.clear();
	m_tUserInfo.vecPowerLimitList.resize(PM_COUNT, FALSE);
}

void PowerLimitManage::setCurPowerType(ePOWERLIMITTYPE type)
{
	std::vector<PowerType>::iterator iter;
	for (iter = m_vPowerType.begin(); iter != m_vPowerType.end(); iter++)
	{
		if (type == iter->getPowerType())
		{
			m_curPowerType = *iter;
			return;
		}
	}
}
