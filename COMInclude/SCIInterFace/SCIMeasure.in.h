/*********************************版本：2021-01-18***********************************
Measure.in.h 测量类，实现涂布测宽以及涂布相关检测
("Init" == strMessage)//初始化
("Action" == strMessage)//检测
("Show" == strMessage)//调用对话框
/***********************************************************************/

#pragma once
#include "stdafx.h"
#include "Log.in.h"
#include "callback.in.h"
#include "AlgorithmBase.in.h"
#include "Product.in.h"

typedef struct tagTMEASUREINIT
{
	int					nSN;				//算法序号,通常为一面一个序号
	HTuple				hvCamSNList;		//一个算法计算要几个相机里的图像
	tagPDInfo			tProductInfo;		//产品信息

	LOGCALLBACK		pLog;					//日志回调
	MAINCALLBACK	pMain;					//主系统回调
}MeasureInit, *PMeasureInit;

typedef struct tagTMEASUREINPUT
{
	HObject		hoGrabImage;	//本张图，这里为了调试方便
	int			nMode;			//检测模式
}MeasureInput, *PMeasureInput;

typedef struct tagTBaseSize
{
	HTuple		hv_StripeSN;			//尺寸所属几个区域，每一个小区域一个序号
	HTuple		hv_Width;				//所有测量尺寸
	HTuple		hv_WidthOrigin;			//所有原始测量尺寸,不含补偿
	HTuple		hv_MisalignedDist;		//错位距离
	HTuple		hv_WholeBaseCoat;		//凹版全尺寸
	HTuple		hv_WholeWidth;			//基材尺寸
}BaseSize, *PBaseSize;

typedef struct tagTSingleSize
{
	HTuple		hv_StripeSN;			//分条序号，所有宽度分别对应第几个分条
	HTuple		hv_Width;				//单个测量类型所有宽度
	HObject		ho_Region;				//单个测量类型所有区域
}SingleSize, *PSingleTypeSize;

typedef struct tagTRect
{
	TPoint		tLeftTop;				//左上角左边
	TPoint		tRightBottom;			//右下角坐标
}TRect,*PTRect;

typedef struct tagTFeature
{
	HTuple		hvWidth;				//宽度
	HTuple		hvHeight;				//高度
	HTuple		hvDiameter;				//直径
	HTuple		hvArea;					//面积
	TPoint		tCenter;				//中心物理坐标，相对于图像左上角
	TRect		tRect;					//区域外接矩形物理坐标，相对于图像左上角
}Feature, *PFeature;

typedef struct tagTRegionResult
{
	HTuple		hv_Type;			//区域类型
	HTuple		hv_StripeSN;		//分条序号，每一个小区域一个序号
	HObject		ho_Region;			//区域，与区域类型和分条序号对应
	Feature		tFeature;			//缺陷区域特征，元素长度与区域类型和分条序号对应
}RegionResult, *PRegionResult;

typedef struct tagTMEASUREOUTPUT
{
	HTuple			hv_HasCoat;				//是否有涂层 TRUE，FALSE
	HTuple			hv_HasIntermittent;		//是否有间歇区域 TRUE，FALSE

	HTuple			hv_LineNum;				//测量线数量
	Line			tLine;					//测量边坐标,包含所有线坐标

	BaseSize		tSize;					//测量基础尺寸

	SingleSize		tCoat;					//所有涂膜 尺寸和区域
	SingleSize		tFoil;					//所有极耳 尺寸和区域
	SingleSize		tAT11;					//所有陶瓷/AT9/AT11 尺寸和区域
	SingleSize		tBaseCoat;				//所有底涂/凹版/涂碳 尺寸和区域
	SingleSize		tIntermittent;			//所有间歇 尺寸和区域

	SingleSize		tSpotInspection;		//点检结果
	HObject         ho_DispImage;           //显示的图像，拼接后的图像，子界面可直接使用
	HObject			ho_Disp;				//测量产生的显示区域

	RegionResult	tDefect;				//瑕疵检测结果
}MeasureOutput, *PMeasureOutput;