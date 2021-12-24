#pragma once
#include "stdafx.h"
#include "Log.in.h"
#include "Product.in.h"
#include "CallBack.in.h"

typedef struct tagTMESINIT//初始化通信类
{
	LOGCALLBACK		Log;		//写日志回调函数
	MAINCALLBACK	Main;		//主系统回调函数
	tagPDInfo		tPdInfo;	//系统参数
}MESINIT,*PMESINIT;

struct tagMesDeviceInfo
{
	//设备信息
	CString strDeviceCode;				//设备编号 Coater_CCD,
	CString strDeviceStatus;            //设备状态 Run    Alarm    Idle,
	CString	strReserve1;				//备注1
	CString	strReserve2;				//备注2
};

struct tagMesProductInfo
{
	CString strBatch;					//母卷批次
	CString strUserID;					//操作员
	CString strShift;					//班次
	CString	strPasteCode;				//浆料号
	CString	strProductType;				//品种
	double	dCoatStd;					//涂布规格
	double	dFoilStd;					//极耳规格
};

struct tagMesHead
{
	//MES数据头部分
	CString m_ProtocolNO;   //:"1110",
	int m_MsgNO;            // : 8999,
	int m_NeedReply;        // : 1,
	CString m_CmdID;        //: "1701B",
	CString m_EID;          // "010920161010000000001"
};

struct tagParametricData
{
	CString name;       //数据项名称
	CString dateType;   //数据类型
	CString value;      //数值
	CString describe;   //描述
};

struct tagNcCode
{
	CString ncCode;     //NG编号
	bool    isNc;       //是否NG
	CString value;      //数值(来自设备)
	CString describe;   //NG描述
};


