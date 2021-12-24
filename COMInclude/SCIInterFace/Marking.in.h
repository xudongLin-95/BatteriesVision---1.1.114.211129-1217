#pragma once
#include "CallBack.in.h"

typedef struct tagTMARKINGINIT//初始化
{
	CString				strProductName; //配方名
	LOGCALLBACK			Log;			//写日志回调函数
	tagPDInfo			tPdInfo;		//产品信息
	MAINCALLBACK		Main;			//获取采集序号
}MARKINGINIT, *PMARKINGINIT;