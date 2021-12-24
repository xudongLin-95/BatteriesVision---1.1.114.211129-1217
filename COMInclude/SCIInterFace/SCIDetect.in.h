#pragma once
#include "Log.in.h"
#include "AlgorithmBase.in.h"
#include "Product.in.h"
#include "SCIMeasure.in.h"

/*********************************版本：2020-02-02***********************************
    ("INITIALIZE" == strMessage)//初始化
	("ACTION" == strMessage)//检测
	("SHOW" == strMessage)//调用对话框
	和Seekers系统共用的头文件，作用是声明结构体变量
/***********************************************************************/

const CString g_strErrorName[]={
	_T("OK"),
	_T("涂布宽度NG"),
	_T("AT9尺寸NG"),
	_T("EA总长NG"),
	_T("总宽NG"),
};

typedef struct tagTDETECTINIT//SCIDetect算法动态库初始化结构体
{
	CString			strProductName;		//品种名称
	int				nSN;				//算法序号,通常为一面一个序号
	HTuple			hvCamSNList;		//一个算法计算要几个相机里的图像
	int				nProductType;		//产品类型，一出几
	int				nCoatingType;		//涂布类型，单面双面，单面只0面检测，双面0和1面都检测
	BOOL			bHasBaseCoat;		//是否包含底涂/凹版
	BOOL			bHasAT11;			//是否包含AT9/AT11
	BOOL			bHasIntermittent;	//是否包含间歇区域

	LOGCALLBACK		pLog;				//日志回调
}DetectInit, *PDetectInit;

typedef struct tagTDETECTINPUT//SCIDetect算法动态库输入结构体
{
	HObject			hoGrabImage;		//采集到的图片
	int				nMode;				//检测模式
}DetectInput, *PDetectInput;

typedef struct tagTMEASUREOUTPUT MeasureOut;
typedef struct tagTDETECTOUTPUT//SCIDetect算法动态库输出结构体
{
	MeasureOut				tMeasure;			//测量结果

	HTuple					hv_ErrorType;		//错误类型
	HTuple					hv_StripeSN;		//属于第几个区域，每一个小区域一个序号
	HObject					ho_ErrorRegion;		//错误区域 个数

	HObject					ho_Disp;			//显示区域

}DetectOutput, *PDetectOutput;

