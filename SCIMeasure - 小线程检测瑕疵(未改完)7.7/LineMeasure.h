//////////////////////////////////////////////////////////////////////////
///LineMeasure.h 用于线测量
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "SCIMeasure.in.h"

class CLineMeasure
{
public:
	enum eMeasureDirection
	{
		MD_Left2Right = 0,	//测量方向从左到右
		MD_Right2Left,		//测量方向从右到左
	};
	enum eMeasureTransition
	{
		MT_All = 0,			//all，所有方向
		MT_Dark2Light,		//positive，暗到亮
		MT_Light2Dark,		//negative，亮到暗
	};
	enum eMeasureSelect
	{
		MS_All = 0,			//all，所有点
		MS_First,			//first，第一点
		MS_Last,			//last，最后一点
	};
	typedef struct tagLineMeasureInput//输入
	{
		HObject hoImage;	//图像
		int		nMode;		//检测模式
	}LineMeasureInput, *PLineMeasureInput;
	typedef struct tagLineMeasureOutput//输出
	{
		HTuple			hv_LineNum;				//测量线数量
		Line			tLine;					//像素测量边坐标,包含所有线坐标

		HTuple			hvErrorLineSN;			//测量错误的线序号

		HObject			hoDisp;					//用于显示的测量线
	}LineMeasureOutput, *PLineMeasureOutput;
	typedef struct tagGenLineInput//输入
	{
		HObject			hoImage;				//图像
		HTuple			hvBaseLineColumn;		//基准线坐标
	}GenLineInput, *PGenLineInput;
	typedef struct tagGenLineOutput//输出
	{
		HObject			hoLine;					//显示测量线
		Line			tLine;					//测量边坐标,包含所有线坐标
		HObject			hoDisp;					//显示
	}GenLineOutput, *PGenLineOutput;

	enum eLineType
	{
		LT_BgFoil = 0,			//背景和极耳交界
		LT_FoilAT11,			//极耳和AT11交界
		LT_AT11Coat,			//AT11到涂布交界
		LT_CoatAT11,			//涂布和AT11交界
		LT_AT11Foil,			//AT11和极耳交界
		LT_FoilBg,				//极耳和背景交界
		LT_FoilCoat,			//极耳和涂布交界
		LT_CoatFoil,			//涂布和极耳交界
		LT_FoilBaseCoat,		//极耳和底涂交界
		LT_BaseCoatFoil,		//底涂和极耳交界
		LT_BaseCoatCoat,		//底涂和涂布交界
		LT_CoatBaseCoat,		//涂布和底涂交界
	};
	typedef std::vector<eLineType> LineTypeList;	//线类型数组
	typedef struct tagLineProp//线属性
	{
		HTuple			hv_CamSNList;			//线所属相机序号，一般从左到右
		HTuple			hv_SNList;				//线序号，一般从左到右
		LineTypeList	vecLineTypeList;		//线的类型，参考eLineType
	}LineProp, *PtagLineProp;
	typedef struct tagLineParam
	{
		int		nDirection;		//搜索线方向，0 从左到右, 1 从右到左
		int		nThr;			//对应measure_pos中的Threshold
		int		nTransition;	//对应measure_pos中的Transition:0-'all', 1-'positive', 2-'negative' 
		int		nSelect;		//对应measure_pos中的Select:0-'all', 1-'first', 2-'last'
		double	dSigma;			//对应measure_pos中的Sigma
		int		nMeasureNum;	//检测框的个数

		double	dWidth;			//单个检测框的宽度
		double	dHeight;		//单个检测框的高度
		
		double	dStartRow;		//起始点的Row坐标
		double	dStartCol;		//起始点的Col坐标
		double	dEndRow;		//终止点的Row坐标
		double	dEndCol;		//终止点的Col坐标
	}LineParam, *PLineParam;
	typedef std::vector<tagLineParam> LineParamList;	//线属性列表
	typedef struct tagMeasureParam
	{
		LineParamList	vecLineParam;		//测量线的配置
		LineProp		tLineProp;			//测量线的属性
		double			dMaxLeanDiff;		//最大倾斜偏差，像素
	}MeasureParam, *PMeasureParam;
	
public:
	CLineMeasure(void);
    ~CLineMeasure(void);

	//初始化
	void Init(MeasureInit &tIn);
	//读参数
	BOOL ReadParam();
	//写参数
	BOOL WriteParam();

	//测量接口
	BOOL Action(LineMeasureInput& tIn, LineMeasureOutput& tOut);
	//单条线测量
	BOOL FindSingleLine(int nSN, LineMeasureInput& tIn, LineMeasureOutput& tOut, BOOL bDisp = FALSE);
	//根据图像自动生成测量参数
	BOOL AutoGenMeasureLine(const GenLineInput& tIn, GenLineOutput& tOut);
	//根据测量规格生成测量参数
	BOOL AutoGenMeasureLineWithStdVal(const GenLineInput& tIn, GenLineOutput& tOut);

	//获取测量参数
	MeasureParam GetMeasureParam() { return m_tMeasureParam; }
	//设置产品信息
	BOOL SetProductInfo(tagPDInfo tInfo) { m_tInit.tProductInfo = tInfo; return TRUE; }
	//设置分辨率
	void SetResolution(ResolutionList vecRes) { m_vecResolution = vecRes; }
private:
	//生成测量线的默认属性
	void GenDefultLineProp();
	//初始化输出变量
	void InitOutputValue();
	//拟合所有测量线
	BOOL FitAllLines();
	//单个线拟合
	int  _FindLine(const HObject& Image, const int nSN, const tagLineParam& FitLinePara, HObject* Line,
		double* dStartY, double* dStartX, double* dEndY, double* dEndX, int nLineDisplay = 0);
	//极耳或者涂布的四个角点
	void GetLLInterSectionPoints(HTuple hvIndex,HTuple hv_CutTileImageWidth,HTuple hv_CutTileImageHeight);
	//根据四个角点生成极耳或者涂布区域
	void GenRegion(HObject* Region,HTuple hvIndex);
	//判断是否垂直
    BOOL IsVertical (HObject Region);

	LineParam ReadLineParam(const int& nSN);
	void WriteLineParam(const int& nSN, const LineParam& tPara);

public:
	MeasureParam			m_tMeasureParam;	//测量参数

	HTuple					m_hvOverColumn;		//屏蔽区域坐标
private:
	MeasureInit				m_tInit;
	LineMeasureInput*		m_ptIn;
	LineMeasureOutput*		m_ptOut;
	

	HTuple m_hvErrorLineIndex;	//线拟合的错误索引

	ResolutionList	m_vecResolution;
	
	HTuple m_hvRowBegin;
	HTuple m_hvColBegin;
	HTuple m_hvRowEnd;
	HTuple m_hvColEnd;

	CString m_strMyConfig;
	CString m_strSystemVis;

	double	m_dMeasureLineMoveDiff;//测量线移动位置，0边找到线后，其他测量区域根据位置差移动
};

