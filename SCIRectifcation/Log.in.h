#pragma once

enum LogType
{
	NoLog=0,	//����¼��־
	StdLog,		//��׼������־
	OperateLog,	//������־
	ExcpLog,	//�쳣��־
	DebugLog,	//������־
};

enum ExcpLevel
{
	NoExcp=0,	//���쳣������ʾ��Ϣ��������
	SlightExcp,	//��΢�쳣����ʾ��Ϣ����ͣ������
	FatalExcp,	//�����쳣����ʾ��Ϣ��ͣ������
};

typedef BOOL (WINAPI *LOGCALLBACK)(const CString& strLog,const int& nLogType,const int& nExcpLevel);		//��־�ص�������dllģ��д��־
//typedef BOOL (WINAPI *LOGCALLBACK)(const CString& strLog,const int& nLogType);		//��־�ص�������dllģ��д��־


