//////////////////////////////////////////////////////////////////////////
///Calibration.h 标定类
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "AlgorithmBase.in.h"

class CCalibration
{
public:
	enum eCalType
	{
		CT_Resolution = 0,		//整个相机的分辨率
		CT_Stripe,				//竖条纹标定
		CT_CalTab,				//halcon自带标定板
	};

	typedef struct tagCalInput
	{
		HObject hoImage;
		HObject hoROI;
	}CalInput, *PCalInput;

	typedef struct tagCalOutput
	{
		HObject hoDisp;
		HTuple	hvRes;			//计算出的分辨率值
		HTuple  hvCoefficients;	//拟合系数，f = hvCoefficients[0]x*x + hvCoefficients[1]*x + hvCoefficients[2]
		HTuple	hvResFit;		//拟合曲线对应的分辨率值
		HTuple  hvR2;			//拟合曲线的回归系数
		HTuple  hvResDiffPercent;//分辨率偏差百分比
		HTuple  hvPixelCol;		//像素横向坐标
	}CalOutput, *PCalOutput;

	typedef struct tagStripeParam//条纹参数
	{
		HTuple hvRealCol;			//实际物理坐标 / 计算一黑一白的边界
		HTuple hvPixelCol;			//像素坐标 / 计算一黑一白的边界
		HTuple hvRes;				//区间比例尺
		HTuple hvColumnOver;		//屏蔽区域左右边的像素坐标
		HTuple hvLeftRightColumnOver;//左相机标定区域最右边的坐标，右相机标定区域最左边的坐标
		double dInternOffset;		//中间交界处补偿
		std::vector<double> vecKValue;//K值,一个相机一个值
		int    nJysl;               //校验的数量
		HTuple hvJyDistance;        //校验的距离
	}StripeParam, *PStripeParam;
	typedef struct tagDoubleStripeTmp//条纹标定临时数据，主要用于存储双相机第一次标定生成的临时数据
	{
		HTuple hvLeftRealCol;				//左相机物理坐标 / 计算一黑一白的边界
		HTuple hvLeftPixelCol;				//左相机像素坐标 / 计算一黑一白的边界
		HTuple hvLeftRes;					//左相机区间比例尺
		HTuple hvLeftWholeRes;				//左相机整体分辨率
		HTuple hvLeftCirclePixelCol;		//左相机圆点的像素坐标
		HTuple hvLeftCircleRealCol;			//左相机圆点的物理坐标
		HTuple hvLeftMaxPixelCol;			//左相机最大标定像素坐标
		HTuple hvRightCirclePixelCol;		//右侧圆点的像素坐标
		HTuple hvLeanRealColDiff;			//第一次标定右相机和左相机倾斜造成的圆点x方向物理偏差

	}DoubleStripeTmp, *PDoubleStripeTmp;
	typedef struct tagCalTabParam//halcon自带标定板参数
	{

	}CalTabParam, *PCalTabParam;

	typedef struct tagCalParam
	{
		int nImageIgnoreWidth;		//图像忽略宽度
		int nThres;					//阈值分割
		int nMeasureThres;			//测量阈值
		double dUnitWidth;			//标定条的物理宽度
		double dDisUpLimit;			//边缘间距系数上限
		double dDisDownLimit;		//边缘间距系数下限

		double dRowResolution;      //瑕疵校准分辨率，行
		double dColResolution;		//瑕疵校准分辨率，列
		double dMinArea;            //瑕疵校准对象面积
		int nLowThresh;             //瑕疵暗阈值
		int nHighThresh;            //瑕疵亮阈值
		int nDefectCalType;			//校准类型，0行，1列
		int nExtractType;			//提取类型，0暗阈值，1亮阈值
		HTuple hvObjWidth;			//瑕疵校验宽度
		HTuple hvObjHeight;			//瑕疵校验高度
	}CalParam, *PCalParam;

	typedef struct tagCalDisPPInput
	{
		HTuple hvCol1;           //第一个点列坐标
		HTuple hvCol2;	         //第二个点列坐标
	}CalDisPPInput, *PCalDisPPInput;

	typedef struct tagCalDisLLInput
	{
		Line tLine1;			//线1
		Line tLine2;			//线2
	}CalDisLLInput, *PCalDisLLInput;

	typedef struct tagCalDisPPOutput
	{
		HTuple Dist;             //输出距离

	}CalDisPPOutput,CalDisLLOutput, *PCalDisPPOutput;

public:
	CCalibration();
	~CCalibration();

	void Init(const MeasureInit& tInit);
	BOOL ReadParam();
	BOOL WriteParam();

	//标定
	BOOL Calibrate(CalInput& tIn, CalOutput& tOut);
	//计算距离
	BOOL CalDistance(CalDisPPInput& pIn, CalDisPPOutput& pOut);
	//验证
	void Verify();

	//像素点转换物理坐标
	BOOL CalPoint(HTuple& hvPtColPix, HTuple& hvPtColReal);
	//物理坐标转换为像素坐标
	BOOL CalPointToPixel(HTuple& hvPtColReal, HTuple& hvPtColPix);

	//单相机计算两点间距离
	BOOL CalDistancePP(CalDisPPInput& pIn, CalDisPPOutput& pOut);

	//双相机计算两点间距离
	BOOL CalDoubleCamDistancePP(CalDisPPInput& pIn, CalDisPPOutput& pOut);

	//计算两条线之间的距离,兼容单双相机
	BOOL CalDistanceLL(CalDisLLInput& tIn, CalDisLLOutput& tOut);

	//双相机第一次标定
	BOOL StripeDoubleCamCalibrationFirst(CalInput& tIn, CalOutput& tOut);

	//双相机第二次标定
	BOOL StripeDoubleCamCalibrationSecond(CalInput& tIn, CalOutput& tOut);

	//获取标定类型
	int GetCalType() { return m_eCalType; }

	//设置标定类型
	void SetCalType(eCalType eType) { m_eCalType = eType; }

	//获取标定参数
	CalParam GetCalParam() { return m_tCalParam; }

	//获取条纹参数
	StripeParam GetStripeParam() { return m_tStripeParam; }

	//获取相机分辨率参数
	ResolutionList GetResolution() { return m_vecResolution; }

	//设置产品信息
	BOOL SetProductInfo(tagPDInfo tInfo) { m_tInit.tProductInfo = tInfo; return TRUE; }

	//根据K值计算临时标定参数
	BOOL CalStripeTmpParam();
private:
	//单相机标定
	BOOL StripeCalibration(CalInput& tIn, CalOutput& tOut);
	//双相机一次性标定
	BOOL StripeDoubleCamCalibration(CalInput& tIn, CalOutput& tOut);
	//计算线的与涂布垂线的夹角
	BOOL _GetLineAngle(int& nSN,HTuple& hvStartRow,HTuple& hvStartCol,HTuple& hvEndRow,HTuple& hvEndCol,HTuple* hvAngle);
	
public:
	CalParam		m_tCalParam;
	StripeParam		m_tStripeParam;
	StripeParam     m_tStripeTmpParam;//临时标定参数
private:
	MeasureInit		m_tInit;
	eCalType		m_eCalType;
	CalTabParam		m_tCalTabParam;
	ResolutionList	m_vecResolution;

	CString			m_strParamFile;
	CString			m_strParamFile1;
	CString			m_strParamFile2;

	DoubleStripeTmp m_tDoubleStripeTmpData;//双相机两次标定临时数据
};