#pragma once
#include <vector>
#include "Log.in.h"

enum ePOWERLIMITTYPE
{
	PT_NONE = 0,		//δ��¼
	PT_OPERATOR,		//����Ա
	PT_TECHNICIAN, 		//���鳤
	PT_ENGINEER,		//����ʦ
	PT_ADMINISTRATOR,	//����Ա
	PT_SUPER_ADMINISTRATOR,	//��������Ա
	PT_FACTORY,
	PT_COUNT			//��������
};

enum ePOWERLIMITMODULE
{
	PM_ChangeBatch=0,	//������ʹ��Ȩ��
	PM_Product,			//��Ʒ���ͺ����ù���ʹ��Ȩ��
	PM_Storage,			//�洢���ù���ʹ��Ȩ��
	PM_System,			//��Ʒ���ͺ����ù���ʹ��Ȩ��
	PM_Detect,			//��⹦��ʹ��Ȩ��
	PM_Manage,			//Ȩ�޹�����ʹ��Ȩ��
	PM_Exit,			//�˳�����ʹ��Ȩ��
	PM_Camera,			//������ù���ʹ��Ȩ��
	PM_IO,				//IO���ù���ʹ��Ȩ��
	PM_PLC,				//PLC���ù���ʹ��Ȩ��
	PM_LightControl,	//��Դ���ƹ���ʹ��Ȩ��
	PM_Mes,				//MES����ʹ��Ȩ��
	PM_UPS,             //UPS����ʹ��Ȩ��
	PM_Marking,         //���빦��ʹ��Ȩ��
	PM_Temperature,     //�¿�������ʹ��Ȩ��
	PM_BetaRay,			//����ǹ���ʹ��Ȩ��

	PM_COUNT			//��������
};

const CString g_strPowerModuleName[PM_COUNT] = {
	"������",
	"�䷽����",
	"�洢����",
	"ϵͳ����",
	"�������",
	"Ȩ������",
	"�˳�����",
	"�������",
	"IO����",
	"PLC����",
	"��Դ����",
	"MES����",
	"UPS����",
	"��������",
	"�¿�������",
	"���������",
};

typedef struct tagPOWERLIMITINIT
{
	LOGCALLBACK			pLog;				//��־ָ��
}POWERLIMITINIT,*PPOWERLIMITINIT;
typedef struct tagUSERINFO
{
	ePOWERLIMITTYPE		ePowerType;			//�û�Ȩ��
	CString				strJobNumber;		//����
	BOOL				bLogin;				//�Ƿ��¼״̬
	SYSTEMTIME			tLoginTime;			//��¼ʱ��
	HTuple				hv_LoginTimeCount;	//��¼ʱ��ͳ��(s)
	std::vector<BOOL>	vecPowerLimitList;	//Ȩ���б�

	inline BOOL CheckPowerEnable(ePOWERLIMITMODULE eModule)
	{
		if (eModule >= 0 && vecPowerLimitList.size() > eModule && eModule < PM_COUNT)
		{
			return vecPowerLimitList[(int)eModule];
		}
		else
		{
			return FALSE;
		}
	}

}USERINFO, *PUSERINFO;