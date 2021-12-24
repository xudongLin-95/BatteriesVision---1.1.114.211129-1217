#pragma once
#include "SCIPowerLimit.in.h"

class PowerLimit
{
public:
	PowerLimit(void);
	~PowerLimit(void);

public:
	static CString m_strStaticFactoryPW;//厂家密码，唯一
public:
	void			setPowerType(ePOWERLIMITTYPE type);
	ePOWERLIMITTYPE getPowerType();
	void			setJobNumber(CString strJobNum);
	CString			getJobNumber();
	void			setCardNumber(CString strCardNum);
	CString			getCardNumber();
	void			setPassword(CString strPW);
	CString			getPassword();
	void			setLogin(BOOL bLogin);
	BOOL			getLogin();
	void			Clear();
	BOOL			match(const PowerLimit& other);//匹配


private:
	ePOWERLIMITTYPE m_ePowerType;//用户名
	CString   m_strJobNumber;//工号
	CString   m_strCardNumber;//刷卡账户，需要与工号关联 BY YS
	CString   m_strPassword;//密码
	
	BOOL m_bLogin;//是否登录状态
};

class PowerLimitList
{
public:
	PowerLimitList(void);
	~PowerLimitList(void);

public:
	BOOL find(PowerLimit& data);//查找,用于登录
	BOOL isExit(PowerLimit data);//存在，用于添加
	BOOL addData(PowerLimit data);
	BOOL delData(PowerLimit data);
	BOOL editData(PowerLimit data);//编辑
	int getCount();
	PowerLimit getPowerLimit(int nIndex);
	PowerLimit* getPowerLimitByJN(CString strJN);//通过工号获取用户
	void clear();
private:
	std::vector<PowerLimit> m_vPowerLimit;//所有用户信息
};

class PowerType
{
public:
	PowerType(void)
	{
		m_ePowerType = PT_NONE;//PT_OPERATOR;
		m_vecPowerLimitList.clear();
		m_vecPowerLimitTempList.clear();
	};
	~PowerType(void)
	{
	};
	void Clear()
	{
		m_ePowerType = PT_NONE;//PT_OPERATOR;
		m_vecPowerLimitTempList.clear();
		m_vecPowerLimitTempList.resize(PM_COUNT, FALSE);
	}
public:
	void setPowerType(ePOWERLIMITTYPE type) { m_ePowerType = type; }
	ePOWERLIMITTYPE getPowerType() { return m_ePowerType; }
	std::vector<BOOL>& GetPowerLimitList() { return m_vecPowerLimitList; }
	void SetModulePower(ePOWERLIMITMODULE eModule, BOOL bEnable) { m_vecPowerLimitList[eModule] = bEnable; }
	BOOL GetModulePower(ePOWERLIMITMODULE eModule) { return m_vecPowerLimitList[eModule]; }
	//临时权限列表
	std::vector<BOOL>& GetPowerLimitTempList() { return m_vecPowerLimitTempList; }
	//设置临时权限列表
	std::vector<BOOL>& SetPowerLimitTempList() 
	{ 
		//设置临时权限列表
		m_vecPowerLimitTempList.clear();
		m_vecPowerLimitTempList.resize(PM_COUNT, FALSE);
		m_vecPowerLimitTempList = m_vecPowerLimitList;
		return m_vecPowerLimitTempList; 
	}
private:
	ePOWERLIMITTYPE m_ePowerType;//用户类型
	std::vector<BOOL> m_vecPowerLimitList;	    //功能模块访问权限列表
	std::vector<BOOL> m_vecPowerLimitTempList;	//功能模块访问权限列表-临时变量
};

class PowerLimitManage :
	public DHInterface
{
public:
	PowerLimitManage();
	~PowerLimitManage();

	virtual int DoSomething(void* message, void* wParam, void* lParam);
public:
	int  Init(PPOWERLIMITINIT ptInit);
	void Read();
	void Write();
	void InitDefaultPowerType();
	void ClearStruct();
	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);
public:
	void setCurPowerLimit(PowerLimit curPowerLimit) { m_curPowerLimit = curPowerLimit; }
	PowerLimit& getCurPowerLimit() { return m_curPowerLimit; }
	PowerLimitList& getPowerLimitList() { return m_PowerLimitList; }
	void setCurPowerType(ePOWERLIMITTYPE type);//根据当前用户设置当前用户权限
	PowerType& getCurPowerType() { return m_curPowerType; }
	std::vector<PowerType>& GetPowerTypeList() { return m_vPowerType; }
private:
	PowerLimit m_curPowerLimit;//当前用户
    PowerLimitList m_PowerLimitList;//所有用户
	PowerType m_curPowerType;//当前用户权限
	std::vector<PowerType> m_vPowerType;//所有用户权限
private:
	CString m_strConfigPath;
	CString m_strConfigFile;
	POWERLIMITINIT m_tInit;
	USERINFO m_tUserInfo;
	HTuple m_hv_TimeStart;
	HTuple m_hv_TimeEnd;
};