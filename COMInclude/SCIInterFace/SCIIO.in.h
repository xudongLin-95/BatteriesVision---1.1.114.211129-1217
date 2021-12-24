#pragma once
#include "stdafx.h"
#include "Log.in.h"

typedef struct tagTIOINIT//初始化通信类
{
	CString strProductName;	//配方名
	LOGCALLBACK Log;		//写日志回调函数
	int nProductType;		//产品类型
}IOINIT,*PIOINIT;



