#pragma once
#include "stdafx.h"
#include "Log.in.h"

typedef struct tagTLIGHTINIT//初始化光源控制器
{
	CString strProductName; //配方名
	LOGCALLBACK Log;		//写日志回调函数
	int nProductType;		//产品类型
	
}LIGHTINIT,*PLIGHTINIT;

enum eLightControlType	//光源控制器型号
{
	DH_HAPS_48W800_1T = 0,
	DH_HAPS_48W800_2T,
};

