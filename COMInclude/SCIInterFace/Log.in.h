#pragma once

enum LogType
{
	NoLog=0,			//����¼��־
	StdLog,				//��׼������־
	AlarmLog,			//�����û��Ĵ�����ʾ��Ϣ
	OperateLog,			//������־�������޸Ĳ���
	CommunicationLog,	//ͨ����־
	ExcpLog,			//���򿪷��͵�����Ա���쳣��Ϣ
	DebugLog,			//������־�����򿪷��͵�����Ա
};

enum ExcpLevel
{
	NoExcp=0,	//���쳣������ʾ��Ϣ��������
	SlightExcp,	//��΢�쳣����ʾ��Ϣ����ͣ������
	FatalExcp,	//�����쳣����ʾ��Ϣ��ͣ������
};

typedef BOOL (WINAPI *LOGCALLBACK)(const CString& strLog,const int& nLogType,const int& nExcpLevel);		//��־�ص�������dllģ��д��־
//typedef BOOL (WINAPI *LOGCALLBACK)(const CString& strLog,const int& nLogType);		//��־�ص�������dllģ��д��־


