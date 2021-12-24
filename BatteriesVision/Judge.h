//////////////////////////////////////////////////////////////////////////
///Judge.h 用于判断检测结果是否OK或NG
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"
#include "SCIDataFlow.in.h"
#include "MSA.h"

class CJudge : public CBaseTmpl
{
public:
	struct tagSingleJudgeParam
	{
		BOOL	bUse;			//是否开启判断
		HTuple	hvMeters;		//NG的米数，尺寸和瑕疵都用
		HTuple	hvNum;			//NG的个数，瑕疵用
		HTuple	hvArea;			//NG的面积，瑕疵用
	};
	struct tagJudgeParam
	{
		BOOL	bUseAlarm;				//使用报警
		BOOL	bUseStopMachine;		//使用停机
		BOOL bUseNGAlarm;            //使用NG米数报警
		double	dIngoreMeter;			//起涂忽略米数
		double	dMeanShiftLimitFactor;	//MeanShift公差系数

		std::vector<tagSingleJudgeParam> vecAlarm;			//报警参数
		std::vector<tagSingleJudgeParam> vecStopMachine;	//停机参数

		std::vector<tagSingleJudgeParam> vecMeanShiftAlarmParam;		//Mean Shift报警参数
		std::vector<tagSingleJudgeParam> vecMeanShiftStopMachineParam;	//Mean Shift停机参数
	};
	struct tagSpotInspectionParam
	{
		BOOL	bStart;					//状态，是否开始点检
		int		nCountinousDataNum;		//连续采集数据量
		int		nNoDataLimit;			//无效数据数量限制
		int		nSpotInspInterval;		//点检间隔时间，单位秒
		BOOL	bUseTemp;				//使用温度限制
		double	dMaxTemperature;		//最小限制温度
	};
	struct tagSpotInspCount
	{
		tagSpotInspCount()
			:hvecDataList(2)
		{}

		ATL::COleDateTime	tStartTime;		//开始时间
		ATL::COleDateTime	tEndTime;		//停止时间
		HTuple				hvFlagFinal;	//最终点检标志位
		HTuple				hvFlagFinalState;//主界面点检状态位 0 正反面都NG，1 OK，2正面NG，3反面NG
		HTuple				hvFlagList[2];	//判断标志位
		HTuple				hvFlagSingle[2];//正反面单个标志位
		HTupleVector		hvecDataList;//正反面点检数据，0正面，1反面
		HTuple				hvDataMedian[2];//中值数据，0正面，1反面
		HTuple				hvTempData[2];//温度数据，0正面，1反面
		HTuple				hvTempMedian[2];//中值温度，0正面，1反面
		int					nNoDataCount;	//无效数据次数
	};

	struct tagFirstArticleInspParam
	{
		int		nCountinousDataNum;		//连续采集数据量
		int		nStartIgnoreDataNum;	//开始的时候忽略的数据量
		int		nNoDataLimit;			//无效数据数量限制
		double	dIntervalTime;			//点检间隔时间，单位h
	};

	struct tagFirstArticleInspCount
	{
		tagFirstArticleInspCount()
			:hvecDataList(2)
			, hvecDataFilterList(2)
			, hvecMisaglinDataFilterList(1)
			, hvecMisaglinDataList(1)
		{}

		BOOL				bStartIgnore;	//开始忽略数据
		BOOL				bStart;			//开始计算首件
		BOOL				bStartCalculateCompen;	//开始利用动态补偿计算首件补偿
		HTuple				hvFlagFinal;	//最终标志位
		HTuple				hvFlagList[2];	//判断标志位
		HTupleVector		hvecDataList;	//正反面数据，0正面，1反面
		HTupleVector		hvecDataFilterList;	//正反面滤波数据，0正面，1反面
		HTupleVector		hvecMisaglinDataList;	//错位数据，0正面，1反面
		HTupleVector		hvecMisaglinDataFilterList;	//错位滤波数据，0正面，1反面
		int					nIgnoreDataCount;	//开始首件忽略数据量
		int					nNoDataCount;	//无效数据次数

		inline void Clear()
		{
			bStartIgnore = FALSE;
			bStart = FALSE;
			hvFlagFinal = OK;
			hvFlagList[0] = OK;
			hvFlagList[1] = OK;
			hvecDataList[0].Clear();
			hvecDataList[1].Clear();
			hvecDataFilterList[0].Clear();
			hvecDataFilterList[1].Clear();
			hvecMisaglinDataList.Clear();
			hvecMisaglinDataFilterList.Clear();
			nIgnoreDataCount = 0;
			nNoDataCount = 0;
			bStartCalculateCompen = FALSE;
		}
	};

	struct tagMeanShiftData
	{
		tagMeanShiftData()
			:hvecCoatData(2)
			, hvecFoilData(2)
			, hvecATBaseCoatData(2)
			, hvecMisalignmentData(1)
		{}

		HTupleVector		hvecCoatData;			//正反面数据，0正面，1反面
		HTupleVector		hvecFoilData;			//极耳数据，0正面，1反面
		HTupleVector		hvecATBaseCoatData;		//AT/凹版数据，0正面，1反面
		HTupleVector		hvecMisalignmentData;	//错位数据，0正面，1反面

		inline void Clear()
		{
			hvecCoatData[0].Clear();
			hvecCoatData[1].Clear();
			hvecFoilData[0].Clear();
			hvecFoilData[1].Clear();
			hvecATBaseCoatData[0].Clear();
			hvecATBaseCoatData[1].Clear();
			hvecMisalignmentData.Clear();
		}
	};

public:
	CJudge();
	~CJudge();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//合并结果
	BOOL Merge(C_MergeWork& tOutput);
	//判断合并结果
	BOOL Judge(C_MergeWork& tOutput);

	//判断检测结果
	BOOL JudgeStdTolerance(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
	//判断检测结果，只判断NG
	BOOL JudgeStdToleranceNG(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
	//判断检测结果，只判断首件NG
	BOOL JudgeFAITolerance(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
	int JudgeCheck();
	void ClearJudgeSumMetes();
private:
	//合并错位值
	BOOL MergeMisalignment(C_MergeWork& tOutput);
	//合并结果初始化
	BOOL InitJudgeResult(C_MergeWork& tOutput);
	//判断测量结果
	BOOL JudgeSize(C_MergeWork& tOutput);
	//判断点检结果
	BOOL JudgeSpotInspect(C_MergeWork& tOutput);
	//判断瑕疵检测结果
	BOOL JudgeDefect(C_MergeWork& tOutput);
	//判断最终结果
	BOOL JudgeFinal(C_MergeWork& tOutput);
	
	//判断连续NG报警
	BOOL JudgeContinuousNG(C_MergeWork& tOutput);
	BOOL JudgeContinuousSizeNG(C_MergeWork& tOutput);
	BOOL JudgeContinuousDefectNG(C_MergeWork& tOutput);
	//监控Mean Shift连续NG
	BOOL JudgeMeanShiftContinuousSizeNG(C_MergeWork& tOutput);
	BOOL JudgeSumNG(C_MergeWork& tOutput);//判断累计NG

	void ClearSizeCount(std::vector<tagSingleJudgeParam> &vecCount);
	void ClearDefectCount(std::vector<tagSingleJudgeParam> &vecCount);
	//首件判断
	BOOL JudgeFirstArticleInspection(C_MergeWork& tOutput);
public:
	tagJudgeParam				m_tParam;
	tagSpotInspectionParam		m_tSpotInspParam;
	HTuple						m_hvLastSpotInspTime;				//上次点检时间戳
	tagFirstArticleInspParam	m_tFirstArticleInspParam;		
	HTuple						m_hvLastFirstArticleInsppTime;		//上次首件时间戳
	tagFirstArticleInspCount	m_tFirstArticleInspCount;			//首件统计
	tagMeanShiftData			m_tMeanShiftData;					//均值数据
	double m_dJudgeSumMetes;                                            //累计NG米数
private:
	CMSA MSA;
	std::vector<tagSingleJudgeParam>	m_vecAlarmCount;			//报警判断计数
	std::vector<tagSingleJudgeParam>	m_vecStopMachineCount;		//停机判断计数
	tagSpotInspCount					m_tSpotInspCount;			//点检统计
	CString m_strParamFile;
};