#pragma once
#include <vector>
#include "Log.in.h"

enum ePOWERLIMITTYPE
{
	PT_NONE = 0,		//未登录
	PT_OPERATOR,		//操作员
	PT_TECHNICIAN, 		//班组长
	PT_ENGINEER,		//工程师
	PT_ADMINISTRATOR,	//管理员
	PT_SUPER_ADMINISTRATOR,	//超级管理员
	PT_FACTORY,
	PT_COUNT			//类型总数
};

enum ePOWERLIMITMODULE
{
	PM_ChangeBatch=0,	//换卷功能使用权限
	PM_Product,			//产品换型和设置功能使用权限
	PM_Storage,			//存储设置功能使用权限
	PM_System,			//产品换型和设置功能使用权限
	PM_Detect,			//检测功能使用权限
	PM_Manage,			//权限管理功能使用权限
	PM_Exit,			//退出功能使用权限
	PM_Camera,			//相机设置功能使用权限
	PM_IO,				//IO设置功能使用权限
	PM_PLC,				//PLC设置功能使用权限
	PM_LightControl,	//光源控制功能使用权限
	PM_Mes,				//MES功能使用权限
	PM_UPS,             //UPS功能使用权限
	PM_Marking,         //喷码功能使用权限
	PM_Temperature,     //温控器功能使用权限
	PM_BetaRay,			//测厚仪功能使用权限

	PM_COUNT			//类型总数
};

const CString g_strPowerModuleName[PM_COUNT] = {
	"换卷功能",
	"配方设置",
	"存储设置",
	"系统设置",
	"检测设置",
	"权限设置",
	"退出功能",
	"相机设置",
	"IO设置",
	"PLC设置",
	"光源设置",
	"MES设置",
	"UPS设置",
	"喷码设置",
	"温控器设置",
	"测厚仪设置",
};

typedef struct tagPOWERLIMITINIT
{
	LOGCALLBACK			pLog;				//日志指针
}POWERLIMITINIT,*PPOWERLIMITINIT;
typedef struct tagUSERINFO
{
	ePOWERLIMITTYPE		ePowerType;			//用户权限
	CString				strJobNumber;		//工号
	BOOL				bLogin;				//是否登录状态
	SYSTEMTIME			tLoginTime;			//登录时刻
	HTuple				hv_LoginTimeCount;	//登录时间统计(s)
	std::vector<BOOL>	vecPowerLimitList;	//权限列表

	inline BOOL CheckPowerEnable(ePOWERLIMITMODULE eModule)
	{
		if (eModule >= 0 && vecPowerLimitList.size() > eModule && eModule < PM_COUNT)
		{
			return vecPowerLimitList[(int)eModule];
		}
		else
		{
			return FALSE;
		}
	}

}USERINFO, *PUSERINFO;