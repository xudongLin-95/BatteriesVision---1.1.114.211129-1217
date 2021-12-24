//////////////////////////////////////////////////////////////////////////
///Judge.h �����жϼ�����Ƿ�OK��NG
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
		BOOL	bUse;			//�Ƿ����ж�
		HTuple	hvMeters;		//NG���������ߴ��覴ö���
		HTuple	hvNum;			//NG�ĸ�����覴���
		HTuple	hvArea;			//NG�������覴���
	};
	struct tagJudgeParam
	{
		int nSpotInspMaxNGCount;		//������NG����

		BOOL	bUseAlarm;				//ʹ�ñ���
		BOOL	bUseStopMachine;		//ʹ��ͣ��

		BOOL	bUseFirstArticleInsp;	//�׼�����
		double	dFirstArticleInspMeter;	//�׼��жϵľ���

		std::vector<tagSingleJudgeParam> vecAlarm;			//��������
		std::vector<tagSingleJudgeParam> vecStopMachine;	//ͣ������
	};
public:
	CJudge();
	~CJudge();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//�ϲ����
	BOOL Merge(C_MergeWork& tOutput);
	//�жϺϲ����
	BOOL Judge(C_MergeWork& tOutput);

	//�жϼ����
	BOOL JudgeStdTolerance(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
	//�жϼ������ֻ�ж�NG
	BOOL JudgeStdToleranceNG(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
private:
	//�ϲ���λֵ
	BOOL MergeMisalignment(C_MergeWork& tOutput);
	//�ϲ������ʼ��
	BOOL InitJudgeResult(C_MergeWork& tOutput);
	//�жϲ������
	BOOL JudgeSize(C_MergeWork& tOutput);
	//�жϵ����
	BOOL JudgeSpotInspect(C_MergeWork& tOutput);
	//�ж�覴ü����
	BOOL JudgeDefect(C_MergeWork& tOutput);
	//�ж����ս��
	BOOL JudgeFinal(C_MergeWork& tOutput);
	
	//�ж�����NG����
	BOOL JudgeContinuousNG(C_MergeWork& tOutput);
	BOOL JudgeContinuousSizeNG(C_MergeWork& tOutput);
	BOOL JudgeContinuousDefectNG(C_MergeWork& tOutput);

	void ClearSizeCount(std::vector<tagSingleJudgeParam> &vecCount);
	void ClearDefectCount(std::vector<tagSingleJudgeParam> &vecCount);

	//�׼��ж�
	BOOL JudgeFirstArticleInspection(C_MergeWork& tOutput);
public:
	tagJudgeParam m_tParam;
private:
	CMSA MSA;
	std::vector<tagSingleJudgeParam> m_vecAlarmCount;			//�����жϼ���
	std::vector<tagSingleJudgeParam> m_vecStopMachineCount;	//ͣ���жϼ���
	CString m_strParamFile;
};