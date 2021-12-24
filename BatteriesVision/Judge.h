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
		BOOL	bUseAlarm;				//ʹ�ñ���
		BOOL	bUseStopMachine;		//ʹ��ͣ��
		BOOL bUseNGAlarm;            //ʹ��NG��������
		double	dIngoreMeter;			//��Ϳ��������
		double	dMeanShiftLimitFactor;	//MeanShift����ϵ��

		std::vector<tagSingleJudgeParam> vecAlarm;			//��������
		std::vector<tagSingleJudgeParam> vecStopMachine;	//ͣ������

		std::vector<tagSingleJudgeParam> vecMeanShiftAlarmParam;		//Mean Shift��������
		std::vector<tagSingleJudgeParam> vecMeanShiftStopMachineParam;	//Mean Shiftͣ������
	};
	struct tagSpotInspectionParam
	{
		BOOL	bStart;					//״̬���Ƿ�ʼ���
		int		nCountinousDataNum;		//�����ɼ�������
		int		nNoDataLimit;			//��Ч������������
		int		nSpotInspInterval;		//�����ʱ�䣬��λ��
		BOOL	bUseTemp;				//ʹ���¶�����
		double	dMaxTemperature;		//��С�����¶�
	};
	struct tagSpotInspCount
	{
		tagSpotInspCount()
			:hvecDataList(2)
		{}

		ATL::COleDateTime	tStartTime;		//��ʼʱ��
		ATL::COleDateTime	tEndTime;		//ֹͣʱ��
		HTuple				hvFlagFinal;	//���յ���־λ
		HTuple				hvFlagFinalState;//��������״̬λ 0 �����涼NG��1 OK��2����NG��3����NG
		HTuple				hvFlagList[2];	//�жϱ�־λ
		HTuple				hvFlagSingle[2];//�����浥����־λ
		HTupleVector		hvecDataList;//�����������ݣ�0���棬1����
		HTuple				hvDataMedian[2];//��ֵ���ݣ�0���棬1����
		HTuple				hvTempData[2];//�¶����ݣ�0���棬1����
		HTuple				hvTempMedian[2];//��ֵ�¶ȣ�0���棬1����
		int					nNoDataCount;	//��Ч���ݴ���
	};

	struct tagFirstArticleInspParam
	{
		int		nCountinousDataNum;		//�����ɼ�������
		int		nStartIgnoreDataNum;	//��ʼ��ʱ����Ե�������
		int		nNoDataLimit;			//��Ч������������
		double	dIntervalTime;			//�����ʱ�䣬��λh
	};

	struct tagFirstArticleInspCount
	{
		tagFirstArticleInspCount()
			:hvecDataList(2)
			, hvecDataFilterList(2)
			, hvecMisaglinDataFilterList(1)
			, hvecMisaglinDataList(1)
		{}

		BOOL				bStartIgnore;	//��ʼ��������
		BOOL				bStart;			//��ʼ�����׼�
		BOOL				bStartCalculateCompen;	//��ʼ���ö�̬���������׼�����
		HTuple				hvFlagFinal;	//���ձ�־λ
		HTuple				hvFlagList[2];	//�жϱ�־λ
		HTupleVector		hvecDataList;	//���������ݣ�0���棬1����
		HTupleVector		hvecDataFilterList;	//�������˲����ݣ�0���棬1����
		HTupleVector		hvecMisaglinDataList;	//��λ���ݣ�0���棬1����
		HTupleVector		hvecMisaglinDataFilterList;	//��λ�˲����ݣ�0���棬1����
		int					nIgnoreDataCount;	//��ʼ�׼�����������
		int					nNoDataCount;	//��Ч���ݴ���

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

		HTupleVector		hvecCoatData;			//���������ݣ�0���棬1����
		HTupleVector		hvecFoilData;			//�������ݣ�0���棬1����
		HTupleVector		hvecATBaseCoatData;		//AT/�������ݣ�0���棬1����
		HTupleVector		hvecMisalignmentData;	//��λ���ݣ�0���棬1����

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

	//�ϲ����
	BOOL Merge(C_MergeWork& tOutput);
	//�жϺϲ����
	BOOL Judge(C_MergeWork& tOutput);

	//�жϼ����
	BOOL JudgeStdTolerance(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
	//�жϼ������ֻ�ж�NG
	BOOL JudgeStdToleranceNG(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
	//�жϼ������ֻ�ж��׼�NG
	BOOL JudgeFAITolerance(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag);
	int JudgeCheck();
	void ClearJudgeSumMetes();
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
	//���Mean Shift����NG
	BOOL JudgeMeanShiftContinuousSizeNG(C_MergeWork& tOutput);
	BOOL JudgeSumNG(C_MergeWork& tOutput);//�ж��ۼ�NG

	void ClearSizeCount(std::vector<tagSingleJudgeParam> &vecCount);
	void ClearDefectCount(std::vector<tagSingleJudgeParam> &vecCount);
	//�׼��ж�
	BOOL JudgeFirstArticleInspection(C_MergeWork& tOutput);
public:
	tagJudgeParam				m_tParam;
	tagSpotInspectionParam		m_tSpotInspParam;
	HTuple						m_hvLastSpotInspTime;				//�ϴε��ʱ���
	tagFirstArticleInspParam	m_tFirstArticleInspParam;		
	HTuple						m_hvLastFirstArticleInsppTime;		//�ϴ��׼�ʱ���
	tagFirstArticleInspCount	m_tFirstArticleInspCount;			//�׼�ͳ��
	tagMeanShiftData			m_tMeanShiftData;					//��ֵ����
	double m_dJudgeSumMetes;                                            //�ۼ�NG����
private:
	CMSA MSA;
	std::vector<tagSingleJudgeParam>	m_vecAlarmCount;			//�����жϼ���
	std::vector<tagSingleJudgeParam>	m_vecStopMachineCount;		//ͣ���жϼ���
	tagSpotInspCount					m_tSpotInspCount;			//���ͳ��
	CString m_strParamFile;
};