#pragma once
#include <vector>

#define NO_DATA -100000	//作为各个数据变量的初始数据值 2020.3.14增加
#define NO_MARK NO_DATA

enum eActionMode//检测模式
{
	RunMode = 0,							//正常运行模式,正常生产，正常报警
	DebugMode,								//调试模式，判断结果但不报警
	SimulateMode,							//仿真模式
	GRRMode,								//GRR验证模式
};

enum e_ErrorType
{
	NG_CoatWidth,			//涂布宽度NG
	NG_FoilWidth,			//留白/极耳宽度NG
	NG_AT11Width,			//AT11宽度NG
	NG_BaseCoatWidth,		//底涂/凹版宽度NG
	NG_ABMisalign,			//AB面错位
	NG_ExposeFoil,			//涂布漏金属/划痕/颗粒
	NG_Fold,				//膜区褶皱
	NG_FoilBlackSpot,		//极耳黑点/暗缺陷
	NG_Tape,				//极耳带胶
	NG_Bubble,				//气泡
	NG_Uncoated,			//漏涂
	NG_WhiteSpot,			//涂布白点
	NG_BlackSpot,			//涂布黑点
	NG_Dent,				//凹痕/凹凸点
	NG_Streak,				//条痕

	NG_Count,				//NG缺陷种类数量
};

typedef struct tagTLINE//线的坐标 2020.12.29增加
{
	HTuple  hv_StartRow;				//开始纵向坐标
	HTuple  hv_StartColumn;				//开始横向坐标
	HTuple  hv_EndRow;					//结束纵向坐标
	HTuple  hv_EndColumn;				//结束横向坐标
}Line, *PLine;

typedef struct tagTPoint//点的坐标 2021.2.14增加
{
	HTuple hv_Row;			//纵坐标
	HTuple hv_Column;		//横坐标
}TPoint, *PPoint;

typedef struct tagResolution//分辨率
{
	double dColRes;				//横向分辨率
	double dRowRes;				//纵向分辨率
}Resolution, *PResolution;
typedef std::vector<Resolution> ResolutionList;