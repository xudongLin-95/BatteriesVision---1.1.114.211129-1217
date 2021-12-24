#pragma once
#include <vector>

enum e_ProductType //产品类型
{
	ThreeCavity = 0,		//1出3，每面一个半涂布
	TwoCavity,				//1出2，每面一个涂布
	FourCavity,				//1出4，每面两个涂布
	SixCavity,				//1出6，每面三个涂布
	EightCavity,			//1出8，每面四个涂布
	TenCavity,				//1出10，每面五个涂布
};

enum e_CoatingType
{
	OneSide = 0,			//单面涂布
	TwoSide,				//双面涂布
};

typedef std::vector<double> DoubleList;
struct tagTolerance
{
	double		dStandard;			//标准值
	double		dStandard2;			//标准值2,备用
	double		dTolerance;			//存在就贴标/报警的公差值
	double		dWarnTolerance;		//预警的公差值
	double		dFAITolerance;		//首件的公差值
	DoubleList	OffsetList;			//double补偿数组，0A面补偿，1B面补偿
	double		dOffsetOL;			//double在线补偿值，A面干湿膜补偿

	tagTolerance& operator = (const tagTolerance &t)
	{
		if (this != &t)
		{
			dStandard = t.dStandard;
			dStandard2 = t.dStandard2;
			dTolerance = t.dTolerance;
			dWarnTolerance = t.dWarnTolerance;
			dFAITolerance = t.dFAITolerance;
			OffsetList = t.OffsetList;
			dOffsetOL = t.dOffsetOL;
		}
		return *this;
	}
};
typedef std::vector<tagTolerance> ToleranceList;	//公差列表

typedef std::vector<int> IntList;
struct tagToleranceN
{
	int		nStandard;			//标准值
	int		nTolerance;			//存在就贴标/报警的公差值
	int		nWarnTolerance;		//预警的公差值
	IntList OffsetList;			//int补偿数组，0A面补偿，1B面补偿

	tagToleranceN& operator = (const tagToleranceN &t)
	{
		if (this != &t)
		{
			nStandard = t.nStandard;
			nTolerance = t.nTolerance;
			nWarnTolerance = t.nWarnTolerance;
			OffsetList = t.OffsetList;
		}
		return *this;
	}
};

struct tagPDJudgeParam//产品判定参数
{
	bool			bUseCoatWidth;			//是否使用涂布宽
	ToleranceList	vecCoatWidth;
	bool			bUseFoilWidth;			//是否使用箔材宽
	ToleranceList	vecFoilWidth;
	bool			bUseAT11Width;			//是否使用AT9/AT11宽检测
	ToleranceList	vecAT11Width;
	bool			bUseBaseCoatWidth;		//是否使用底涂检测
	ToleranceList	vecBaseCoatWidth;
	bool			bUseMisalignment;		//是否使用错位检测
	ToleranceList	vecMisalignment;
	bool			bUseChecking;			//是否使用设备点检
	ToleranceList	vecChecking;
	bool			bUseSurfaceDetect;		//是否使用表面缺陷检测
};

struct tagSingleLabelParam
{
	int				nNGNum;					//NG数量
	double			dNGSize;				//NG超标尺寸
	double			dNGArea;				//NG超标面积
};
typedef std::vector<tagSingleLabelParam> LabelParamList;

struct tagPDLabelJudgeParam
{
	LabelParamList ParamList;				//打标/报警判断参数数组，
};
typedef std::vector<CString> NameList;
struct tagPDInfo
{
	CString					strPDName;			//产品名
	CString					strNote;			//产品备注
	BOOL     				bExistAT11;			//是否包含AT9/AT11，正极
	BOOL     				bExistBaseCoat;		//是否包含底涂，负极
	BOOL     				bExistIntermittent;	//是否包含间歇区域
	int						nPDType;			//产品类型
	int						nCoatingType;		//涂的类型，0单面，1双面
	int						nMeasureLineNum;	//测量线数量
	tagPDJudgeParam			tParam;				//判定参数
	tagPDLabelJudgeParam	tLabelParam;		//贴标/报警判断参数
	NameList				vecNameList;		//显示名称
};
typedef std::vector<tagPDInfo> ProductList;
