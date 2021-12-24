#pragma once
#include "BaseTmpl.h"
#include "SCIPowerLimit.in.h"

class CPermission : public CBaseTmpl
{
public:
	CPermission();
	~CPermission();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//��¼
	BOOL Login();
	//ע��
	void Logout();
	//��õ�ǰȨ��
	ePOWERLIMITTYPE GetPermission()const { return m_tUserInfo.ePowerType; }
	//����Ƿ�Ȩ��
	BOOL CheckPermission(ePOWERLIMITMODULE eModule);
	//�Ƿ��ѵ�¼
	BOOL IsLogin() { return (m_tUserInfo.bLogin); }
	//��õ�ǰ��ǰ�û���Ϣ
	const tagUSERINFO & GetUserInfo() { return m_tUserInfo; }
	//�û�������
	BOOL ShowUserManage();
	//Ȩ�޹�����
	BOOL ShowUserPower();

	//������Ա��¼��¼
	void RecoredOperatorMsg(BOOL bLoginOrOut, USERINFO tUserInfo);

private:
	
private:
	USERINFO		m_tUserInfo;		//�û���Ϣ
	DHInterface*	m_pPowerLimit;		//Ȩ�޿�
};