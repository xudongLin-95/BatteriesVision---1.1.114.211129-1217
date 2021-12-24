//////////////////////////////////////////////////////////////////////////
///SpotInspection.h 点检类，对相机的测量精度点检
//////////////////////////////////////////////////////////////////////////
#pragma once

class CSpotInspection
{
public:
	enum eInspectionType
	{
		IT_Rect = 0,		//单位尺寸矩形
		IT_Circle,			//单位尺寸圆
	};
	enum eInspectionColor
	{
		IC_Light=0,			//亮区域
		IC_Dark,			//暗区域
	};
	typedef struct tagMeasureLine//测量线
	{
		HTuple		hvStartRow;
		HTuple		hvStartCol;
	};
	typedef struct tagInspectionParam//点检参数
	{
		HObject				hoSearchReigon;			//搜索范围
		eInspectionColor	eColorType;				//点检目标颜色
		int					nThres;					//分割点检物体的阈值
		double				dPixelMinSize;			//最小筛选尺寸(像素),矩形为宽度，圆为直径
		double				dPixelMaxSize;			//最大筛选尺寸(像素),矩形为宽度，圆为直径
		int					nObjectNum;				//点检目标个数
	}InspectionParam, *PInspectionParam;
	typedef struct tagInspectionInput//输入
	{
		HObject		hoImage;
	}InspectionInput, *PInspectionInput;
	typedef struct tagInspectionOutput//输出
	{
		HObject					hoDisp;					//现实区域
		TPoint					ptCenter;				//中心坐标
		TPoint					ptLeftTop;				//左上角坐标
		TPoint					ptLeftBottom;			//左下角坐标
		TPoint					ptRightTop;				//右上角坐标
		TPoint					ptRightBottom;			//右下角坐标
	}InspectionOutput, *PInspectionOutput;

	typedef struct tagSpotInspInput//输入
	{
		HObject hoImage;
		int		nMode;					//检测模式
	}SpotInspInput, *PSpotInspInput;

public:
	CSpotInspection();
	~CSpotInspection();

	void Init(const MeasureInit& tInit);
	BOOL ReadParam();
	BOOL WriteParam();
	BOOL Action(InspectionInput& tIn, InspectionOutput& tOut);

	//初始化测量输出值
	void InitOutputValue();

	//获取点检类型
	eInspectionType	GetType() { return m_eType; }
	//设置点检类型
	void SetType(eInspectionType eType) { m_eType = eType; }

	//获取点检参数
	InspectionParam	GetParam() { return m_tParam; }
	//设置点检参数
	void SetParam(InspectionParam& tParam) { m_tParam = tParam; }
	//设置分辨率
	void SetResolution(ResolutionList vecRes) { m_vecResolution = vecRes; }

	//自动生成点检参数
	BOOL AutoGenSpotInspectParm(SpotInspInput *pIn, InspectionOutput* pOut);

	//设置产品信息
	BOOL SetProductInfo(tagPDInfo tInfo) { m_tInit.tProductInfo = tInfo; return TRUE; }

private:
	BOOL RectInspect(const InspectionInput& tIn, InspectionOutput& tOut);
	BOOL CircleInspect(const InspectionInput& tIn, InspectionOutput& tOut);
private:
	eInspectionType	m_eType;
	InspectionParam m_tParam;

	CString	m_strParamFileIni;
	CString	m_strParamFileROIRegion;
	MeasureInit	m_tInit;

	InspectionInput* m_ptIn;
	InspectionOutput* m_ptOut;

	ResolutionList	m_vecResolution;
};