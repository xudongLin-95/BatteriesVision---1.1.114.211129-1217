#pragma once
#include "Log.in.h"

typedef struct tagTRECTIFICATIONCALL//回调信息结构
{
	int nType;
	int nErrorCode;
	BOOL bReLoad ; // 是否重新加载纠偏库（更换IP、端口不重启主程序用）
}RECTIFICATIONCALL,*PRECTIFICATIONCALL;

typedef BOOL (WINAPI *RECTIFICATIONCALLBACK)(PRECTIFICATIONCALL tCall);

typedef struct tagTRECTIFICATIONINIT//初始化Rectficaion纠偏类
{
	void* pDlg;				//主界面指针
	CString strProductName; //配方名
	LOGCALLBACK Log;		//写日志回调函数
	int nProductType;		//产品类型
	RECTIFICATIONCALLBACK RectficaionCall;//纠偏反馈
	int nSN;		//纠偏器编号 从1开始
}RECTIFICATIONINIT,*PRECTIFICATIONINIT;

