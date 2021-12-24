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
		int nSpotInspMaxNGCount;		//点检最大NG数量

		BOOL	bUseAlarm;				//使用报警
		BOOL	bUseStopMachine;		//使用停机

		BOOL	bUseFirstArticleInsp;	//首件开关
		double	dFirstArticleInspMeter;	//首件判断的距离

		std::vector<tagSingleJudgeParam> vecAlarm;			//报警参数
		std::vector<tagSingleJudgeParam> vecStopMachine;	//停机参数
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

	void ClearSizeCount(std::vector<tagSingleJudgeParam> &vecCount);
	void ClearDefectCount(std::vector<tagSingleJudgeParam> &vecCount);

	//首件判断
	BOOL JudgeFirstArticleInspection(C_MergeWork& tOutput);
public:
	tagJudgeParam m_tParam;
private:
	CMSA MSA;
	std::vector<tagSingleJudgeParam> m_vecAlarmCount;			//报警判断计数
	std::vector<tagSingleJudgeParam> m_vecStopMachineCount;	//停机判断计数
	CString m_strParamFile;
};