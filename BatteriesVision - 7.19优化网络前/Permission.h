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

	//登录
	BOOL Login();
	//注销
	void Logout();
	//获得当前权限
	ePOWERLIMITTYPE GetPermission()const { return m_tUserInfo.ePowerType; }
	//检查是否够权限
	BOOL CheckPermission(ePOWERLIMITMODULE eModule);
	//是否已登录
	BOOL IsLogin() { return (m_tUserInfo.bLogin); }
	//获得当前当前用户信息
	const tagUSERINFO & GetUserInfo() { return m_tUserInfo; }
	//用户管理弹框
	BOOL ShowUserManage();
	//权限管理弹框
	BOOL ShowUserPower();

	//操作人员登录记录
	void RecoredOperatorMsg(BOOL bLoginOrOut, USERINFO tUserInfo);

private:
	
private:
	USERINFO		m_tUserInfo;		//用户信息
	DHInterface*	m_pPowerLimit;		//权限库
};