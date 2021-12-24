#pragma once
#include "stdafx.h"
#include "Product.in.h"
#include "Log.in.h"
#include "CallBack.in.h"

typedef struct tagTUPSINIT//初始化
{
	tagPDInfo		tPdInfo;	//产品参数
	LOGCALLBACK		Log;		//写日志回调函数
	MAINCALLBACK	Main;		//主系统回调函数
}UPSINIT,*PUPSINIT;



