//////////////////////////////////////////////////////////////////////////
///Measure.h 用于测量涂布各个边的尺寸和检测表面缺陷
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "Product.in.h"
#include "LineMeasure.h"
#include "Calibration.h"
#include "Defect.h"
#include "SpotInspection.h"
#include "PolyFit.h"

class CMeasure : public DHInterface
{
public:
	enum eFoilSelect//极耳计算方式
	{
		FS_OnlyFoil = 0,	//仅箔材
		FS_FoilBaseCoat,	//箔材+凹版
	};
	enum eMisalignmentSelect//错位值计算方式
	{
		MS_WidthSum = 0,	//宽度累加
		MS_LineDist,		//线与线之间的距离
	};
	enum eCoatSelect
	{
		CS_Nomal,			//普通计算
		CS_FullCoat,		//涂布覆盖凹版时，计算涂布和极耳的边
	};
	typedef struct tagPreprocParam
	{
		BOOL	bUseJudgeUncoat;	//是否判断无涂膜
		int		nUncoatThresLight;	//无涂膜(空箔材)阈值
		int		nUncoatThresDark;	//无涂膜(空凹版)阈值
		double	dUncoatAreaRate;	//无涂膜面积占比
	};
	typedef struct tagMeasureAdvanceParam//测量高级参数
	{
		eFoilSelect			eFoilSelect;
		eMisalignmentSelect eMisalignmentSelect;
		eCoatSelect			eCoatSelect;
	};

	typedef struct tagTJiaoZhun//校准
	{
		HTuple hvReal[2];//实际物理值
		HTuple hvPix[2];//像素值
		HTuple hvRes[2];//比例尺
		
		int    nJysl;//校验的数量
		double dSingleWidth;//单个条纹的宽度
		double dSigma;
		int    nThres;
		HTuple hvJyDistance[2];//校验的距离
	}JiaoZhun;

public:
	CMeasure(void);
	~CMeasure(void);

	virtual int DoSomething(void* message, void* wParam, void* lParam);

	CMeasure& operator= (const CMeasure& in);

	//初始化算法
	int		Init(MeasureInit &tIn);
	//检测执行接口
	int		Action(MeasureInput &tIn,PMeasureOutput ptOut);
	//调用算法对话框
	int		DisplaySetDialog(MeasureInput & tIn);
	//读参数
	BOOL	ReadParam();
	//写参数
	BOOL	WriteParam();

private:
	//初始化测量输出值
	void	InitOutputValue();
	//测量
	BOOL	Measure();
	//检测
	void	Detect();
	//点检
	void	SpotInspect();
	//判断是否有涂布
	BOOL	JudgeExistCoat();
	//判断是否有间歇区域
	void	JudgeExistIntermittent();
	//从外界设置参数
	int		SetParam(const CString& strParamName, void* vParam);
	//根据测量线生成宽度数据
	BOOL	CombinMeasure();
	//设置产品参数
	BOOL	SetProductInfo(tagPDInfo tInfo);
	//GRR模式测试
	BOOL	GRRTest(MeasureInput &tIn);
	//修正测量边
	BOOL	CorrectMeasure();

	//生成单一条的区域
	HObject _GenSingleRegion(HTuple hvLine1Row1, HTuple hvLine1Col1, HTuple hvLine1Row2, HTuple hvLine1Col2,
		HTuple hvLine2Row1, HTuple hvLine2Col1, HTuple hvLine2Row2, HTuple hvLine2Col2);

	//找箔材上的标记点，用于GRR模式
	void _GetBlackMark(HObject ho_Image, HObject ho_ROI_0, HObject *ho_SelectedRegions);

	//判断测量边有没有找到边
	BOOL IsLineFind(const int& nLineSN);

	//计算线的夹角
	BOOL _GetLineAngle(int& nSN,HTuple& hvStartRow,HTuple& hvStartCol,HTuple& hvEndRow,HTuple& hvEndCol,HTuple* hvAngle);

	//根据找边的像素坐标生成物理坐标
	Line GetWorldCoordsFromFitLine();

	//计算线与线之间的距离
	void _DistanceLL(HTuple hvStartX1,HTuple hvStartY1,HTuple hvEndX1,HTuple hvEndY1,
		HTuple hvStartX2,HTuple hvStartY2,HTuple hvEndX2,HTuple hvEndY2,HTuple *hvDist);

	//修正当前值，与标准值靠近
	void _CorrectValue(HTuple hvVal, double dStd, double dTolerance, HTuple* hvValCorrect);

	//参数修改日志
	void SaveParamLog(CMeasure Old, CMeasure New);

	//操作日志
	void _OperateLog(const double& dOld, const double& dCurrent, const CString& strOperateName);
	void _OperateLog(int& nOld, int& nCurrent, const CString& strOperateName);
	void _OperateLog(const CString& strOld, const CString& strCurrent, const CString& strOperateName);
public:
	MeasureOutput*	m_ptOut;
	MeasureInput*	m_ptIn;
	MeasureInit		m_tInit;			//初始化
	tagPreprocParam	m_tPreprocParam;	//预处理
	tagMeasureAdvanceParam m_tMeasureAdvanceParam;	//测量高级参数
	CLineMeasure	m_LineMeasure;					//寻边测量类
	CCalibration	m_Calibration;					//标定类
	CDefect         m_Defect;						//瑕疵检测类
	CSpotInspection m_SpotInspection;				//点检检测类
	CPolyFit		m_PolyFit;						//多项式拟合类
	JiaoZhun m_tJZ;//校准
	void ReadJZ();
	void WriteJZ();
private:
	CRITICAL_SECTION m_csLock;
	CString			m_strSystemVis, m_strMyConfig, m_strCommondh, m_strParamFile;
	HTuple			m_hvImageWidth, m_hvImageHeight;
	HTuple			m_hvErrorLineIndex;

	std::vector<Line> m_vecLastLine;	//前几张找到的完整的边
	int				m_nNoLineCount;		//连续没找到边的计数
};

extern CMeasure* g_pAlgorithm;