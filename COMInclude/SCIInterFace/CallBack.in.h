#pragma once

enum eModuleType
{
	MT_Grabber = 0,		//�ɼ�ģ��
	MT_Measure,			//�����㷨ģ��
	MT_Detect,			//覴��㷨ģ��
	MT_Match,			//ģ��ƥ��ģ��
	MT_IO,				//IOģ��
	MT_PLC,				//PLCģ��
	MT_Rectifcation,	//��ƫģ��
	MT_Mes,				//MESģ��
	MT_Log,				//��־ģ��
	MT_LightControl,	//��Դ����ģ��
	MT_CrashRecord,		//�������ģ��
	MT_Marking,			//����ģ��
	MT_UPS,				//UPSģ��
};

//ģ�鷴����Ϣ��ϵͳ�Ļص�
typedef int(WINAPI *MAINCALLBACK)(eModuleType eType, const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);