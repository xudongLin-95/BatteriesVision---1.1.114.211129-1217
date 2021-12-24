#pragma once

enum eModuleType
{
	MT_Grabber = 0,		//采集模块
	MT_Measure,			//测量算法模块
	MT_Detect,			//瑕疵算法模块
	MT_Match,			//模板匹配模块
	MT_IO,				//IO模块
	MT_PLC,				//PLC模块
	MT_Rectifcation,	//纠偏模块
	MT_Mes,				//MES模块
	MT_Log,				//日志模块
	MT_LightControl,	//光源控制模块
	MT_CrashRecord,		//捕获崩溃模块
	MT_Marking,			//喷码模块
	MT_UPS,				//UPS模块
};

//模块反馈信息给系统的回调
typedef int(WINAPI *MAINCALLBACK)(eModuleType eType, const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);