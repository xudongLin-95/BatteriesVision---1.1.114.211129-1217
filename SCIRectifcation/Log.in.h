#pragma once

enum LogType
{
	NoLog=0,	//不记录日志
	StdLog,		//标准运行日志
	OperateLog,	//操作日志
	ExcpLog,	//异常日志
	DebugLog,	//调试日志
};

enum ExcpLevel
{
	NoExcp=0,	//无异常，不显示信息，不报警
	SlightExcp,	//轻微异常，显示信息，不停机报警
	FatalExcp,	//严重异常，显示信息，停机报警
};

typedef BOOL (WINAPI *LOGCALLBACK)(const CString& strLog,const int& nLogType,const int& nExcpLevel);		//日志回调，用于dll模块写日志
//typedef BOOL (WINAPI *LOGCALLBACK)(const CString& strLog,const int& nLogType);		//日志回调，用于dll模块写日志


