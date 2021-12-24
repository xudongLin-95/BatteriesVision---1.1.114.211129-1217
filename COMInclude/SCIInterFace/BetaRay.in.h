#pragma once
#include "Log.in.h"
#include "CallBack.in.h"
#include "Product.in.h"

enum eBetaRayType	//betaray类型
{
	BRT_KFCK = 0,	//开封测控
	BRT_DaCheng,	//大成
};

typedef struct tagBETARAYINIT//初始化
{
	CString				strProductName; //配方名
	LOGCALLBACK			Log;			//写日志回调函数
	tagPDInfo			tPdInfo;		//产品信息
	MAINCALLBACK		Main;			//主系统回调
}BETARAYINIT, *PBETARAYINIT;

typedef struct tagBetaRayPdInfo
{
	float fBaseWeight;
	float fNetWeightA;
	float fNetWeightB;
	float fUpperTol;
	float fLowerTol;
	int	  nReserve;
}BetaRayPdInfo,*PBetaRayPdInfo;