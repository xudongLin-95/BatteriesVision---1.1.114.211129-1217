#pragma once
#include "SCIPowerLimit.in.h"

class PowerLimit
{
public:
	PowerLimit(void);
	~PowerLimit(void);

public:
	static CString m_strStaticFactoryPW;//�������룬Ψһ
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
	BOOL			match(const PowerLimit& other);//ƥ��


private:
	ePOWERLIMITTYPE m_ePowerType;//�û���
	CString   m_strJobNumber;//����
	CString   m_strCardNumber;//ˢ���˻�����Ҫ�빤�Ź��� BY YS
	CString   m_strPassword;//����
	
	BOOL m_bLogin;//�Ƿ��¼״̬
};

class PowerLimitList
{
public:
	PowerLimitList(void);
	~PowerLimitList(void);

public:
	BOOL find(PowerLimit& data);//����,���ڵ�¼
	BOOL isExit(PowerLimit data);//���ڣ��������
	BOOL addData(PowerLimit data);
	BOOL delData(PowerLimit data);
	BOOL editData(PowerLimit data);//�༭
	int getCount();
	PowerLimit getPowerLimit(int nIndex);
	PowerLimit* getPowerLimitByJN(CString strJN);//ͨ�����Ż�ȡ�û�
	void clear();
private:
	std::vector<PowerLimit> m_vPowerLimit;//�����û���Ϣ
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
	//��ʱȨ���б�
	std::vector<BOOL>& GetPowerLimitTempList() { return m_vecPowerLimitTempList; }
	//������ʱȨ���б�
	std::vector<BOOL>& SetPowerLimitTempList() 
	{ 
		//������ʱȨ���б�
		m_vecPowerLimitTempList.clear();
		m_vecPowerLimitTempList.resize(PM_COUNT, FALSE);
		m_vecPowerLimitTempList = m_vecPowerLimitList;
		return m_vecPowerLimitTempList; 
	}
private:
	ePOWERLIMITTYPE m_ePowerType;//�û�����
	std::vector<BOOL> m_vecPowerLimitList;	    //����ģ�����Ȩ���б�
	std::vector<BOOL> m_vecPowerLimitTempList;	//����ģ�����Ȩ���б�-��ʱ����
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
	void setCurPowerType(ePOWERLIMITTYPE type);//���ݵ�ǰ�û����õ�ǰ�û�Ȩ��
	PowerType& getCurPowerType() { return m_curPowerType; }
	std::vector<PowerType>& GetPowerTypeList() { return m_vPowerType; }
private:
	PowerLimit m_curPowerLimit;//��ǰ�û�
    PowerLimitList m_PowerLimitList;//�����û�
	PowerType m_curPowerType;//��ǰ�û�Ȩ��
	std::vector<PowerType> m_vPowerType;//�����û�Ȩ��
private:
	CString m_strConfigPath;
	CString m_strConfigFile;
	POWERLIMITINIT m_tInit;
	USERINFO m_tUserInfo;
	HTuple m_hv_TimeStart;
	HTuple m_hv_TimeEnd;
};