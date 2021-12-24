#include "StdAfx.h"
#include "Judge.h"

CJudge::CJudge()
{
	m_hvLastSpotInspTime = 0;
	m_tFirstArticleInspCount.Clear();
	m_tMeanShiftData.Clear();
}

CJudge::~CJudge()
{

}

BOOL CJudge::Init()
{
	CString strPath;
	strPath.Format("%s\\Config\\Public\\Judge\\", GetCurrentAppPath());
	CreateMultilevelPath(strPath);
	m_strParamFile.Format("%s\\Config\\Public\\Judge\\Judge.ini", GetCurrentAppPath());
	ReadParam();
	return TRUE;
}

BOOL CJudge::ReadParam()
{
	m_tParam.bUseAlarm = GetPrivateProfileInt("Judge", "UseAlarm", FALSE, m_strParamFile);
	m_tParam.bUseStopMachine = GetPrivateProfileInt("Judge", "UseStopMachine", FALSE, m_strParamFile);
	m_tParam.bUseNGAlarm = GetPrivateProfileInt("Judge", "UseNGAlarm", FALSE, m_strParamFile);
	m_tParam.dIngoreMeter = GetPrivateProfileDouble("Judge", "IngoreMeter", 5, m_strParamFile);
	m_tParam.dMeanShiftLimitFactor = GetPrivateProfileDouble("Judge", "MeanShiftLimitFactor", 0.333, m_strParamFile);
	m_dJudgeSumMetes= GetPrivateProfileDouble("Judge", "JudgeSumMetes", 0, m_strParamFile);

	m_tParam.vecAlarm.clear();
	m_tParam.vecStopMachine.clear();

	m_vecAlarmCount.clear();
	m_vecStopMachineCount.clear();

	m_tParam.vecMeanShiftAlarmParam.clear();
	m_tParam.vecMeanShiftStopMachineParam.clear();

	int nCount = NG_Count + 1;
	for (int i = 0; i < nCount; i++)
	{
		CString strVal;

		tagSingleJudgeParam tParam;
		strVal.Format("Alarm%dUse", i);
		tParam.bUse = GetPrivateProfileInt("Judge", strVal, 0, m_strParamFile);
		strVal.Format("Alarm%dMeter", i);
		tParam.hvMeters = GetPrivateProfileDouble("Judge", strVal, 30, m_strParamFile);
		strVal.Format("Alarm%dArea", i);
		tParam.hvArea = GetPrivateProfileDouble("Judge", strVal, 100, m_strParamFile);
		strVal.Format("Alarm%dNum", i);
		tParam.hvNum = (int)GetPrivateProfileInt("Judge", strVal, 100, m_strParamFile);
		m_tParam.vecAlarm.push_back(tParam);
		tParam.hvMeters = 0;
		tParam.hvArea = 0;
		tParam.hvNum = 0;
		m_vecAlarmCount.push_back(tParam);
		strVal.Format("MeanShiftAlarm%dUse", i);
		tParam.bUse = GetPrivateProfileInt("Judge", strVal, 0, m_strParamFile);
		strVal.Format("MeanShiftAlarm%dMeter", i);
		tParam.hvMeters = GetPrivateProfileDouble("Judge", strVal, 30, m_strParamFile);
		strVal.Format("MeanShiftAlarm%dArea", i);
		tParam.hvArea = GetPrivateProfileDouble("Judge", strVal, 100, m_strParamFile);
		strVal.Format("MeanShiftAlarm%dNum", i);
		tParam.hvNum = (int)GetPrivateProfileInt("Judge", strVal, 100, m_strParamFile);
		m_tParam.vecMeanShiftAlarmParam.push_back(tParam);

		strVal.Format("StopMachine%dUse", i);
		tParam.bUse = GetPrivateProfileInt("Judge", strVal, 0, m_strParamFile);
		strVal.Format("StopMachine%dMeter", i);
		tParam.hvMeters = GetPrivateProfileDouble("Judge", strVal, 60, m_strParamFile);
		strVal.Format("StopMachine%dArea", i);
		tParam.hvArea = GetPrivateProfileDouble("Judge", strVal, 200, m_strParamFile);
		strVal.Format("StopMachine%dNum", i);
		tParam.hvNum = (int)GetPrivateProfileInt("Judge", strVal, 200, m_strParamFile);
		m_tParam.vecStopMachine.push_back(tParam);
		tParam.hvMeters = 0;
		tParam.hvArea = 0;
		tParam.hvNum = 0;
		m_vecStopMachineCount.push_back(tParam);
		strVal.Format("MeanShiftStopMachine%dUse", i);
		tParam.bUse = GetPrivateProfileInt("Judge", strVal, 0, m_strParamFile);
		strVal.Format("MeanShiftStopMachine%dMeter", i);
		tParam.hvMeters = GetPrivateProfileDouble("Judge", strVal, 60, m_strParamFile);
		strVal.Format("MeanShiftStopMachine%dArea", i);
		tParam.hvArea = GetPrivateProfileDouble("Judge", strVal, 200, m_strParamFile);
		strVal.Format("MeanShiftStopMachine%dNum", i);
		tParam.hvNum = (int)GetPrivateProfileInt("Judge", strVal, 200, m_strParamFile);
		m_tParam.vecMeanShiftStopMachineParam.push_back(tParam);
	}

	//点检参数
	m_tSpotInspParam.bStart = TRUE;
	m_tSpotInspParam.nCountinousDataNum = GetPrivateProfileInt("SpotInspJudge", "nCountinousDataNum", 10, m_strParamFile);
	m_tSpotInspParam.nNoDataLimit = GetPrivateProfileInt("SpotInspJudge", "nNoDataLimit", 5, m_strParamFile);
	m_tSpotInspParam.nSpotInspInterval = GetPrivateProfileInt("SpotInspJudge", "nSpotInspInterval", 1800, m_strParamFile);
	m_tSpotInspParam.bUseTemp = GetPrivateProfileInt("SpotInspJudge", "bUseTemp", FALSE, m_strParamFile);
	m_tSpotInspParam.dMaxTemperature = GetPrivateProfileDouble("SpotInspJudge", "dMaxTemperature", 75, m_strParamFile);
	//点检统计，生产报表用
	m_tSpotInspCount.hvecDataList[0].Clear();
	m_tSpotInspCount.hvecDataList[1].Clear();
	m_tSpotInspCount.hvFlagList[0].Clear();
	m_tSpotInspCount.hvFlagList[1].Clear();
	m_tSpotInspCount.hvDataMedian[0].Clear();
	m_tSpotInspCount.hvDataMedian[1].Clear();
	m_tSpotInspCount.nNoDataCount = 0;
	m_tSpotInspCount.tStartTime = ATL::COleDateTime::GetCurrentTime();

	//首件参数
	m_tFirstArticleInspParam.nCountinousDataNum = GetPrivateProfileInt("FirstArticleInspJudge", "nCountinousDataNum", 15, m_strParamFile);
	m_tFirstArticleInspParam.nStartIgnoreDataNum = GetPrivateProfileInt("FirstArticleInspJudge", "nStartIgnoreDataNum", 30, m_strParamFile);
	m_tFirstArticleInspParam.nNoDataLimit = GetPrivateProfileInt("FirstArticleInspJudge", "nNoDataLimit", 5, m_strParamFile);
	m_tFirstArticleInspParam.dIntervalTime = GetPrivateProfileDouble("FirstArticleInspJudge", "dIntervalTime", 12, m_strParamFile);

	m_tFirstArticleInspCount.Clear();

	return TRUE;
}

BOOL CJudge::WriteParam()
{
	WritePrivateProfileInt("Judge", "UseAlarm", m_tParam.bUseAlarm, m_strParamFile);
	WritePrivateProfileInt("Judge", "UseStopMachine", m_tParam.bUseStopMachine, m_strParamFile);
	WritePrivateProfileInt("Judge", "UseNGAlarm", m_tParam.bUseNGAlarm, m_strParamFile);
	WritePrivateProfileDouble("Judge", "IngoreMeter", m_tParam.dIngoreMeter, m_strParamFile);
	WritePrivateProfileDouble("Judge", "MeanShiftLimitFactor", m_tParam.dMeanShiftLimitFactor, m_strParamFile);
	WritePrivateProfileDouble("Judge", "JudgeSumMetes", m_dJudgeSumMetes, m_strParamFile);
	

	for (int i = 0; i < m_tParam.vecAlarm.size(); i++)
	{
		CString strVal;
		tagSingleJudgeParam tParam = m_tParam.vecAlarm[i];
		strVal.Format("Alarm%dUse", i);
		WritePrivateProfileInt("Judge", strVal, tParam.bUse, m_strParamFile);
		strVal.Format("Alarm%dMeter", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvMeters.D(), m_strParamFile);
		strVal.Format("Alarm%dArea", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvArea.D(), m_strParamFile);
		strVal.Format("Alarm%dNum", i);
		WritePrivateProfileInt("Judge", strVal, tParam.hvNum.I(), m_strParamFile);

		tParam = m_tParam.vecMeanShiftAlarmParam[i];
		strVal.Format("MeanShiftAlarm%dUse", i);
		WritePrivateProfileInt("Judge", strVal, tParam.bUse, m_strParamFile);
		strVal.Format("MeanShiftAlarm%dMeter", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvMeters.D(), m_strParamFile);
		strVal.Format("MeanShiftAlarm%dArea", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvArea.D(), m_strParamFile);
		strVal.Format("MeanShiftAlarm%dNum", i);
		WritePrivateProfileInt("Judge", strVal, tParam.hvNum.I(), m_strParamFile);

		tParam = m_tParam.vecStopMachine[i];
		strVal.Format("StopMachine%dUse", i);
		WritePrivateProfileInt("Judge", strVal, tParam.bUse, m_strParamFile);
		strVal.Format("StopMachine%dMeter", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvMeters.D(), m_strParamFile);
		strVal.Format("StopMachine%dArea", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvArea.D(), m_strParamFile);
		strVal.Format("StopMachine%dNum", i);
		WritePrivateProfileInt("Judge", strVal, tParam.hvNum.I(), m_strParamFile);

		tParam = m_tParam.vecMeanShiftStopMachineParam[i];
		strVal.Format("MeanShiftStopMachine%dUse", i);
		WritePrivateProfileInt("Judge", strVal, tParam.bUse, m_strParamFile);
		strVal.Format("MeanShiftStopMachine%dMeter", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvMeters.D(), m_strParamFile);
		strVal.Format("MeanShiftStopMachine%dArea", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvArea.D(), m_strParamFile);
		strVal.Format("MeanShiftStopMachine%dNum", i);
		WritePrivateProfileInt("Judge", strVal, tParam.hvNum.I(), m_strParamFile);
	}

	//点检参数
	WritePrivateProfileInt("SpotInspJudge", "nSpotInspInterval", m_tSpotInspParam.nSpotInspInterval, m_strParamFile);
	WritePrivateProfileInt("SpotInspJudge", "nCountinousDataNum", m_tSpotInspParam.nCountinousDataNum, m_strParamFile);
	WritePrivateProfileInt("SpotInspJudge", "nNoDataLimit", m_tSpotInspParam.nNoDataLimit, m_strParamFile);
	WritePrivateProfileInt("SpotInspJudge", "bUseTemp", m_tSpotInspParam.bUseTemp, m_strParamFile);
	WritePrivateProfileDouble("SpotInspJudge", "dMaxTemperature", m_tSpotInspParam.dMaxTemperature, m_strParamFile);

	//首件参数
	WritePrivateProfileInt("FirstArticleInspJudge", "nNoDataLimit", m_tFirstArticleInspParam.nNoDataLimit, m_strParamFile);
	WritePrivateProfileInt("FirstArticleInspJudge", "nStartIgnoreDataNum", m_tFirstArticleInspParam.nStartIgnoreDataNum, m_strParamFile);
	WritePrivateProfileInt("FirstArticleInspJudge", "nCountinousDataNum", m_tFirstArticleInspParam.nCountinousDataNum, m_strParamFile);
	WritePrivateProfileDouble("FirstArticleInspJudge", "dIntervalTime", m_tFirstArticleInspParam.dIntervalTime, m_strParamFile);

	return TRUE;
}

BOOL CJudge::Merge(C_MergeWork& tOutput)
{
	try
	{
		//if (theGlobal.GetSystemSet().nActionMode > DebugMode) return TRUE;
		MergeMisalignment(tOutput);
	}
	catch (HException& e)
	{

	}

	return TRUE;
}

/************************************
*@Method:    MergeMisalignment
*@Access:    private
*@Returns:   BOOL
*@Author: 	 Fowindy
*@Parameter: C_MergeWork & tOutput
*@Created:   2021/03/04 17:52
*@Describe:	 合并错位值-函数
*************************************/
BOOL CJudge::MergeMisalignment(C_MergeWork& tOutput)
{
	TupleGenConst((int)theGlobal.m_ProductTool.GetCurrentProductInfo().tParam.vecCoatWidth.size() * 2, NO_DATA, &tOutput.tMerge.hv_Misalignment);
	if (tOutput.vecB.size() < 2 || theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType != TwoSide
		|| tOutput.vecB[0].tMeasure.hv_HasCoat != TRUE && tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist != NO_DATA)
	{
		return FALSE;
	}

	//判断有没有NO_DATA的
	HTuple hv_Inc, hv_Inc2;
	TupleFind(tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist, NO_DATA, &hv_Inc);
	if (hv_Inc != -1 && hv_Inc.Length() != 0)
	{
		return FALSE;
	}
	//TupleFind(tOutput.vecB[0].tMeasure.tSize.hv_Width, NO_DATA, &hv_Inc);
	//if (hv_Inc != -1 && hv_Inc.Length() != 0) return FALSE;
	//TupleFind(tOutput.vecB[0].tMeasure.tSize.hv_Width, 0, &hv_Inc);
	//if (hv_Inc != -1 && hv_Inc.Length() != 0) return FALSE;

	if (tOutput.vecB.size() == 2)
	{
		TupleFind(tOutput.vecB[1].tMeasure.tSize.hv_MisalignedDist, NO_DATA, &hv_Inc2);
		if (hv_Inc2 != -1 && hv_Inc2.Length() != 0) return FALSE;
		TupleFind(tOutput.vecB[1].tMeasure.tSize.hv_Width, NO_DATA, &hv_Inc2);
		if (hv_Inc2 != -1 && hv_Inc2.Length() != 0) return FALSE;
		TupleFind(tOutput.vecB[1].tMeasure.tSize.hv_Width, 0, &hv_Inc2);
		if (hv_Inc2 != -1 && hv_Inc2.Length() != 0) return FALSE;
	}

	if (tOutput.vecB.size() == 2 && tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE && tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist != NO_DATA
		&& tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE && tOutput.vecB[1].tMeasure.tSize.hv_MisalignedDist != NO_DATA)
	{
		double dScale = 1;
		if (tOutput.vecB[0].hv_SideSN == 0)
		{
			dScale = tOutput.vecB[1].tMeasure.tSize.hv_WholeWidth.D() / tOutput.vecB[0].tMeasure.tSize.hv_WholeWidth.D();
			HTuple hvScaleWidth;
			TupleMult(tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist, dScale, &hvScaleWidth);
			HALCON_TRY(TupleSub(hvScaleWidth, tOutput.vecB[1].tMeasure.tSize.hv_MisalignedDist, &tOutput.tMerge.hv_Misalignment));
		}
		else
		{
			dScale = tOutput.vecB[0].tMeasure.tSize.hv_WholeWidth.D() / tOutput.vecB[1].tMeasure.tSize.hv_WholeWidth.D();
			HTuple hvScaleWidth;
			TupleMult(tOutput.vecB[1].tMeasure.tSize.hv_MisalignedDist, dScale, &hvScaleWidth);
			HALCON_TRY(TupleSub(hvScaleWidth, tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist, &tOutput.tMerge.hv_Misalignment));
		}
		//使用错位边到两边基材边的距离，右一半数值是反过来的，需要乘以-1
		if (tOutput.tMerge.hv_Misalignment.Length() > 0 && tOutput.tMerge.hv_Misalignment.Length() % 2 == 0)
		{
			HTuple hvSub, hvLeft, hvRight;
			TupleGenConst(tOutput.tMerge.hv_Misalignment.Length() / 2, 1, &hvLeft);
			TupleGenConst(tOutput.tMerge.hv_Misalignment.Length() / 2, -1, &hvRight);
			hvSub.Append(hvLeft);
			hvSub.Append(hvRight);
			HALCON_TRY(TupleMult(tOutput.tMerge.hv_Misalignment, hvSub, &tOutput.tMerge.hv_Misalignment));
		}
	}
	else
	{
		TupleGenConst((int)theGlobal.m_ProductTool.GetCurrentProductInfo().tParam.vecCoatWidth.size() * 2, NO_DATA, &tOutput.tMerge.hv_Misalignment);
		return FALSE;
	}

	return TRUE;
}

BOOL CJudge::InitJudgeResult(C_MergeWork& tOutput)
{
	tOutput.tMerge.vecSideResult.clear();
	for (int i = 0; i < tOutput.vecB.size(); i++)
	{
		SingleSideResult t;
		t.Init();
		tOutput.tMerge.vecSideResult.push_back(t);
	}

	tOutput.tMerge.hv_NGType.Clear();
	tOutput.tMerge.hv_WarnType.Clear();
	tOutput.tMerge.hv_MeasureFlag = OK;
	tOutput.tMerge.hv_DetectFlag = OK;
	tOutput.tMerge.hv_FinalFlag = OK;
	tOutput.tMerge.hv_SpotInspectionFlag = OK;

	TupleGenConst(tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist.Length(), OK, &tOutput.tMerge.hv_MisalignmentFlagList);

	tOutput.tMerge.hv_NGType2.Clear();
	tOutput.tMerge.hv_NGStripeSN.Clear();
	tOutput.tMerge.hv_NGSideSN.Clear();

	GenEmptyObj(&tOutput.tMerge.ho_NGRegion);
	return TRUE;
}

BOOL CJudge::Judge(C_MergeWork& tOutput)
{
	try
	{
		InitJudgeResult(tOutput);
		//if (theGlobal.GetSystemSet().nActionMode > DebugMode) return TRUE;

		//MSA.MSAFrame(tOutput);
		JudgeSize(tOutput);//测量结果判定
		JudgeDefect(tOutput);//瑕疵结果判定
		JudgeFinal(tOutput);//最终结果判定

		JudgeSumNG(tOutput);//判断累计米数NG
		JudgeContinuousNG(tOutput);//判断连续NG

		JudgeSpotInspect(tOutput);//点检结果判定
		JudgeFirstArticleInspection(tOutput);//判断首件
	}
	catch (HException e)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CJudge::JudgeSize(C_MergeWork& tOutput)
{
	try
	{
		tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
		HTuple hvFlag(OK);

		//宽度
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			if (nSideSN == 1 && tInfo.nCoatingType == OneSide) continue;;
			if (tInfo.tParam.bUseCoatWidth)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tCoat.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecCoatWidth.size()) continue;
					JudgeStdTolerance(&(HTuple)tOutput.vecB[nSideSN].tMeasure.tCoat.hv_Width[j], tInfo.tParam.bUseCoatWidth, tInfo.tParam.vecCoatWidth[0], &hvFlag);
					tOutput.tMerge.vecSideResult[nSideSN].hv_CoatWidthFlagList.Append(hvFlag);
					if (hvFlag == NG)
					{
						tOutput.tMerge.hv_MeasureFlag = NG;
						tOutput.tMerge.hv_NGType.Append(NG_CoatWidth);

						//获取涂布尺寸的NG区域
						if (tOutput.vecB[nSideSN].tMeasure.tCoat.hv_Width[j].D() > 0)
						{
							tOutput.tMerge.hv_NGType2.Append(NG_CoatWidth);
							ConcatObj(tOutput.vecB[nSideSN].tMeasure.tCoat.ho_Region[j + 1], tOutput.tMerge.ho_NGRegion, &tOutput.tMerge.ho_NGRegion);
							tOutput.tMerge.hv_NGStripeSN.Append(tOutput.vecB[nSideSN].tMeasure.tCoat.hv_StripeSN[j]);
							tOutput.tMerge.hv_NGSideSN.Append(nSideSN);
						}

						tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = NG;
					}
					else if (hvFlag == WARN)
					{
						if (tOutput.tMerge.hv_MeasureFlag != NG)
							tOutput.tMerge.hv_MeasureFlag = WARN;
						tOutput.tMerge.hv_WarnType.Append(NG_CoatWidth);
						if (tOutput.tMerge.vecSideResult[nSideSN].hv_Flag != NG)
							tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = WARN;
					}
				}
			}
			else
			{
				TupleGenConst(tOutput.vecB[nSideSN].tMeasure.tCoat.hv_Width.Length(), OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_CoatWidthFlagList);
			}

			if (tInfo.tParam.bUseFoilWidth)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecFoilWidth.size()) continue;
					tagTolerance tFoilWidth = tInfo.tParam.vecFoilWidth[j];
					/*if (j > 0 && j < tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width.Length() - 1)
						tFoilWidth.dStandard = tInfo.tParam.vecFoilWidth[0].dStandard * 2;*/
					JudgeStdTolerance(&(HTuple)tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width[j], tInfo.tParam.bUseFoilWidth, tFoilWidth, &hvFlag);
					tOutput.tMerge.vecSideResult[nSideSN].hv_FoilWidthFlagList.Append(hvFlag);
					if (hvFlag == NG)
					{
						tOutput.tMerge.hv_MeasureFlag = NG;
						tOutput.tMerge.hv_NGType.Append(NG_FoilWidth);

						//获取极耳尺寸的NG区域
						if (tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width[j].D() > 0)
						{
							tOutput.tMerge.hv_NGType2.Append(NG_FoilWidth);
							ConcatObj(tOutput.vecB[nSideSN].tMeasure.tFoil.ho_Region[j + 1], tOutput.tMerge.ho_NGRegion, &tOutput.tMerge.ho_NGRegion);
							tOutput.tMerge.hv_NGStripeSN.Append(tOutput.vecB[nSideSN].tMeasure.tFoil.hv_StripeSN[j]);
							tOutput.tMerge.hv_NGSideSN.Append(nSideSN);
						}

						tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = NG;
					}
					else if (hvFlag == WARN)
					{
						if (tOutput.tMerge.hv_MeasureFlag != NG)
							tOutput.tMerge.hv_MeasureFlag = WARN;
						tOutput.tMerge.hv_WarnType.Append(NG_FoilWidth);
						if (tOutput.tMerge.vecSideResult[nSideSN].hv_Flag != NG)
							tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = WARN;
					}
				}
			}
			else
			{
				TupleGenConst(tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width.Length(), OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_FoilWidthFlagList);
			}

			if (tInfo.bExistAT11 && tInfo.tParam.bUseAT11Width)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tAT11.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecAT11Width.size()) continue;
					JudgeStdTolerance(&(HTuple)tOutput.vecB[nSideSN].tMeasure.tAT11.hv_Width[j], tInfo.tParam.bUseAT11Width, tInfo.tParam.vecAT11Width[0], &hvFlag);
					tOutput.tMerge.vecSideResult[nSideSN].hv_AT11WidthFlagList.Append(hvFlag);
					if (hvFlag == NG)
					{
						tOutput.tMerge.hv_MeasureFlag = NG;
						tOutput.tMerge.hv_NGType.Append(NG_AT11Width);

						//获取AT11尺寸的NG区域
						if (tOutput.vecB[nSideSN].tMeasure.tAT11.hv_Width[j].D() > 0)
						{
							tOutput.tMerge.hv_NGType2.Append(NG_AT11Width);
							ConcatObj(tOutput.vecB[nSideSN].tMeasure.tAT11.ho_Region[j + 1], tOutput.tMerge.ho_NGRegion, &tOutput.tMerge.ho_NGRegion);
							tOutput.tMerge.hv_NGStripeSN.Append(tOutput.vecB[nSideSN].tMeasure.tAT11.hv_StripeSN[j]);
							tOutput.tMerge.hv_NGSideSN.Append(nSideSN);
						}

						tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = NG;
					}
					else if (hvFlag == WARN)
					{
						if (tOutput.tMerge.hv_MeasureFlag != NG)
							tOutput.tMerge.hv_MeasureFlag = WARN;
						tOutput.tMerge.hv_WarnType.Append(NG_AT11Width);
						if (tOutput.tMerge.vecSideResult[nSideSN].hv_Flag != NG)
							tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = WARN;
					}
				}
			}
			else
			{
				TupleGenConst(tOutput.vecB[nSideSN].tMeasure.tAT11.hv_Width.Length(), OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_AT11WidthFlagList);
			}
			if (tInfo.bExistBaseCoat && tInfo.tParam.bUseBaseCoatWidth)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecBaseCoatWidth.size()) continue;
					JudgeStdTolerance(&(HTuple)tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_Width[j], tInfo.tParam.bUseBaseCoatWidth, tInfo.tParam.vecBaseCoatWidth[0], &hvFlag);
					tOutput.tMerge.vecSideResult[nSideSN].hv_BaseCoatWidthFlagList.Append(hvFlag);
					if (hvFlag == NG)
					{
						tOutput.tMerge.hv_MeasureFlag = NG;
						tOutput.tMerge.hv_NGType.Append(NG_BaseCoatWidth);

						//获取BaseCoat尺寸的NG区域
						if (tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_Width[j].D() > 0)
						{
							tOutput.tMerge.hv_NGType2.Append(NG_BaseCoatWidth);
							ConcatObj(tOutput.vecB[nSideSN].tMeasure.tBaseCoat.ho_Region[j + 1], tOutput.tMerge.ho_NGRegion, &tOutput.tMerge.ho_NGRegion);
							tOutput.tMerge.hv_NGStripeSN.Append(tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_StripeSN[j]);
							tOutput.tMerge.hv_NGSideSN.Append(nSideSN);
						}

						tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = NG;
					}
					else if (hvFlag == WARN)
					{
						if (tOutput.tMerge.hv_MeasureFlag != NG)
							tOutput.tMerge.hv_MeasureFlag = WARN;
						tOutput.tMerge.hv_WarnType.Append(NG_BaseCoatWidth);
						if (tOutput.tMerge.vecSideResult[nSideSN].hv_Flag != NG)
							tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = WARN;
					}
				}
			}
			else
			{
				TupleGenConst(tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_Width.Length(), OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_BaseCoatWidthFlagList);
			}
		}
		//A/B面错位
		if (tInfo.nCoatingType == TwoSide && tOutput.vecB.size() == 2 && tInfo.tParam.bUseMisalignment &&
			tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE &&  tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE)
		{
			tOutput.tMerge.hv_MisalignmentFlagList.Clear();
			for (int i = 0; i < tOutput.tMerge.hv_Misalignment.Length(); i++)
			{
				JudgeStdTolerance(&(HTuple)tOutput.tMerge.hv_Misalignment[i], tInfo.tParam.bUseMisalignment, tInfo.tParam.vecMisalignment[0], &hvFlag);
				tOutput.tMerge.hv_MisalignmentFlagList.Append(hvFlag);
				if (hvFlag == NG)
				{
					tOutput.tMerge.hv_MeasureFlag = NG;
					tOutput.tMerge.hv_NGType.Append(NG_ABMisalign);
					tOutput.tMerge.vecSideResult[0].hv_Flag = NG;
					tOutput.tMerge.vecSideResult[1].hv_Flag = NG;
				}
				else if (hvFlag == WARN)
				{
					if (tOutput.tMerge.hv_MeasureFlag != NG)
						tOutput.tMerge.hv_MeasureFlag = WARN;
					tOutput.tMerge.hv_WarnType.Append(NG_ABMisalign);
					if (tOutput.tMerge.vecSideResult[0].hv_Flag != NG)
						tOutput.tMerge.vecSideResult[0].hv_Flag = WARN;
					if (tOutput.tMerge.vecSideResult[1].hv_Flag != NG)
						tOutput.tMerge.vecSideResult[1].hv_Flag = WARN;
				}
			}
		}

		//合并所有宽度测量标志位
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			if (nSideSN == 1 && tInfo.nCoatingType == OneSide) continue;

			int nCnt[3] = { 0,0,0 };
			if (tOutput.tMerge.vecSideResult[nSideSN].hv_FoilWidthFlagList.Length() - 1 >= nCnt[0])
			{
				tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(tOutput.tMerge.vecSideResult[nSideSN].hv_FoilWidthFlagList[nCnt[0]++]);
			}
			else
			{
				tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(OK);
				CString strExcp;
				strExcp.Format("JudgeSize: 第%d面，合并标志位hv_FoilWidthFlagList数组长度不够", nSideSN);
				theGlobal.m_Log.Exception(strExcp);
			}

			for (int i = 0; i < tOutput.tMerge.vecSideResult[nSideSN].hv_CoatWidthFlagList.Length(); i++)
			{
				//AT11或者凹版
				if (tInfo.bExistAT11)
				{
					if (tOutput.tMerge.vecSideResult[nSideSN].hv_AT11WidthFlagList.Length() - 1 >= nCnt[1])
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(tOutput.tMerge.vecSideResult[nSideSN].hv_AT11WidthFlagList[nCnt[1]++]);
					}
					else
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(OK);
						CString strExcp;
						strExcp.Format("JudgeSize: 第%d面，合并标志位hv_AT11WidthFlagList数组长度不够", nSideSN);
						theGlobal.m_Log.Exception(strExcp);
					}
				}
				else if (tInfo.bExistBaseCoat)
				{
					if (tOutput.tMerge.vecSideResult[nSideSN].hv_BaseCoatWidthFlagList.Length() - 1 >= nCnt[1])
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(tOutput.tMerge.vecSideResult[nSideSN].hv_BaseCoatWidthFlagList[nCnt[1]++]);
					}
					else
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(OK);
						CString strExcp;
						strExcp.Format("JudgeSize: 第%d面，合并标志位hv_BaseCoatWidthFlagList数组长度不够", nSideSN);
						theGlobal.m_Log.Exception(strExcp);
					}
				}
				//涂布
				if (tOutput.tMerge.vecSideResult[nSideSN].hv_CoatWidthFlagList.Length() - 1 >= nCnt[2])
				{
					tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(tOutput.tMerge.vecSideResult[nSideSN].hv_CoatWidthFlagList[nCnt[2]++]);
				}
				else
				{
					tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(OK);
					CString strExcp;
					strExcp.Format("JudgeSize: 第%d面，合并标志位hv_CoatWidthFlagList数组长度不够", nSideSN);
					theGlobal.m_Log.Exception(strExcp);
				}
				//AT11或者凹版
				if (tInfo.bExistAT11)
				{
					if (tOutput.tMerge.vecSideResult[nSideSN].hv_AT11WidthFlagList.Length() - 1 >= nCnt[1])
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(tOutput.tMerge.vecSideResult[nSideSN].hv_AT11WidthFlagList[nCnt[1]++]);
					}
					else
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(OK);
						CString strExcp;
						strExcp.Format("JudgeSize: 第%d面，合并标志位hv_AT11WidthFlagList数组长度不够", nSideSN);
						theGlobal.m_Log.Exception(strExcp);
					}
				}
				else if (tInfo.bExistBaseCoat)
				{
					if (tOutput.tMerge.vecSideResult[nSideSN].hv_BaseCoatWidthFlagList.Length() - 1 >= nCnt[1])
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(tOutput.tMerge.vecSideResult[nSideSN].hv_BaseCoatWidthFlagList[nCnt[1]++]);
					}
					else
					{
						tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(OK);
						CString strExcp;
						strExcp.Format("JudgeSize: 第%d面，合并标志位hv_BaseCoatWidthFlagList数组长度不够", nSideSN);
						theGlobal.m_Log.Exception(strExcp);
					}
				}
				//极耳
				if (tOutput.tMerge.vecSideResult[nSideSN].hv_FoilWidthFlagList.Length() - 1 >= nCnt[0])
				{
					tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(tOutput.tMerge.vecSideResult[nSideSN].hv_FoilWidthFlagList[nCnt[0]++]);
				}
				else
				{
					tOutput.tMerge.vecSideResult[nSideSN].hv_AllWidthFlagList.Append(OK);
					CString strExcp;
					strExcp.Format("JudgeSize: 第%d面，合并标志位hv_FoilWidthFlagList数组长度不够", nSideSN);
					theGlobal.m_Log.Exception(strExcp);
				}
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeSize: 合并尺寸异常 %s(%s)", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
		return FALSE;
	}
	return FALSE;
}
//判断点检结果
BOOL CJudge::JudgeSpotInspect(C_MergeWork& tOutput)
{
	//点检
	tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
	{
		TupleGenConst(7, OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_SpotInspectionFlagList);
	}

	if (!tInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode || !m_tSpotInspParam.bStart)
	{
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			TupleGenConst(7, NO_DATA, &tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
			TupleGenConst(7, OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_SpotInspectionFlagList);
		}
		return TRUE;
	}

	HTuple hvTimeCurrent;
	static BOOL s_bContinueSpotInsp = FALSE;
	
	CountSeconds(&hvTimeCurrent);
	try
	{
		if ((hvTimeCurrent - m_hvLastSpotInspTime) < m_tSpotInspParam.nSpotInspInterval)
		{
			//点检间隔判断，没到间隔时间，不距离数据返回
			for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
			{
				TupleGenConst(tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width.Length(), NO_DATA, &tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
				TupleGenConst(7, OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_SpotInspectionFlagList);
			}

			m_tSpotInspCount.hvecDataList[0].Clear();
			m_tSpotInspCount.hvecDataList[1].Clear();
			m_tSpotInspCount.hvFlagList[0].Clear();
			m_tSpotInspCount.hvFlagList[1].Clear();
			m_tSpotInspCount.hvDataMedian[0].Clear();
			m_tSpotInspCount.hvDataMedian[1].Clear();
			m_tSpotInspCount.hvTempData[0].Clear();
			m_tSpotInspCount.hvTempData[1].Clear();
			m_tSpotInspCount.hvTempMedian[0].Clear();
			m_tSpotInspCount.hvTempMedian[1].Clear();
			m_tSpotInspCount.nNoDataCount = 0;
			m_tSpotInspCount.hvFlagFinal = OK;
			m_tSpotInspCount.hvFlagSingle[0] = OK;
			m_tSpotInspCount.hvFlagSingle[1] = OK;

			s_bContinueSpotInsp = FALSE;
			return TRUE;
		}
		if (s_bContinueSpotInsp == FALSE)
		{
			m_tSpotInspCount.tStartTime = ATL::COleDateTime::GetTickCount();
			s_bContinueSpotInsp = TRUE;
		}

		//判断点检参数是否有效
		HTuple hvSize;
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			hvSize.Append(tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
		}
		HTuple hvInc;
		TupleFind(hvSize, NO_DATA, &hvInc);
		if ((hvInc.Length() >= 1 && hvInc != -1) || hvInc.Length() == 0 || hvSize.Length() != tOutput.vecB.size()*7)
		{
			m_tSpotInspCount.nNoDataCount++;

			//如果一次点检流程无效数据超过设定次数，弹框报警
			if (m_tSpotInspCount.nNoDataCount >= m_tSpotInspParam.nNoDataLimit)
			{
				m_tSpotInspCount.nNoDataCount = 0;
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("CCD未抓取到点检片,无效数据超过%d个,请检查", m_tSpotInspParam.nNoDataLimit);
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查托辊是否有异动，点检片是否异常\n2检查点检算法搜索框是否准确，检测参数是否合适");
				theGlobal.m_Log.Alarm(tInfo);

				m_hvLastSpotInspTime = hvTimeCurrent;

				m_tSpotInspParam.bStart = TRUE;
				m_tSpotInspCount.hvecDataList[0].Clear();
				m_tSpotInspCount.hvecDataList[1].Clear();
				m_tSpotInspCount.hvFlagList[0].Clear();
				m_tSpotInspCount.hvFlagList[1].Clear();
				m_tSpotInspCount.hvDataMedian[0].Clear();
				m_tSpotInspCount.hvDataMedian[1].Clear();
				m_tSpotInspCount.hvTempData[0].Clear();
				m_tSpotInspCount.hvTempData[1].Clear();
				m_tSpotInspCount.hvTempMedian[0].Clear();
				m_tSpotInspCount.hvTempMedian[1].Clear();
				m_tSpotInspCount.nNoDataCount = 0;
				m_tSpotInspCount.hvFlagFinal = OK;
				m_tSpotInspCount.hvFlagSingle[0] = OK;
				m_tSpotInspCount.hvFlagSingle[1] = OK;

				s_bContinueSpotInsp = FALSE;
			}
			return TRUE;
		}
		//温度
		BOOL bConnect = TRUE;
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Temperature, "GetConnection", &bConnect, NULL);
		if (bConnect)
		{
			std::vector<unsigned short> vTempList;
			vTempList.resize(theGlobal.m_CommunicationTool.m_nTempertatureNums, 0);
			theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Temperature, "GetCurrentTemperature", &vTempList, NULL);
			if (vTempList.size() > 0)
			{
				m_tSpotInspCount.hvTempData[0].Append(vTempList[0]);
			}
			if (vTempList.size() > 1)
			{
				m_tSpotInspCount.hvTempData[1].Append(vTempList[1]);
			}
		}
		else
		{
			m_tSpotInspCount.hvTempData[0].Append(NO_DATA);
			m_tSpotInspCount.hvTempData[1].Append(NO_DATA);
		}
		//保留数据
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			m_tSpotInspCount.hvecDataList[nSideSN][m_tSpotInspCount.hvecDataList[nSideSN].Length()].T()
				= tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width;
		}
		if (m_tSpotInspCount.hvecDataList[0].Length() >= m_tSpotInspParam.nCountinousDataNum)
		{
			if (m_tSpotInspCount.hvTempData[0].Length() > 0)
				m_tSpotInspCount.hvTempMedian[0] = m_tSpotInspCount.hvTempData[0].TupleMedian().D();
			if (tOutput.vecB.size() > 1 && m_tSpotInspCount.hvTempData[1].Length() > 0)
				m_tSpotInspCount.hvTempMedian[1] = m_tSpotInspCount.hvTempData[1].TupleMedian().D();

			//温度判定
			if (m_tSpotInspParam.bUseTemp)
			{
				BOOL bCanJudge = TRUE;
				if (bConnect)
				{
					try
					{
						if (m_tSpotInspCount.hvTempData[0].Length() > 0)
						{
							if (m_tSpotInspCount.hvTempMedian[0].D() < m_tSpotInspParam.dMaxTemperature)//大于改为小于
							{
								bCanJudge = FALSE;
							}
						}
						if (tOutput.vecB.size() > 1 && m_tSpotInspCount.hvTempData[1].Length() > 0)
						{
							if (m_tSpotInspCount.hvTempMedian[1].D() < m_tSpotInspParam.dMaxTemperature)//大于改为小于
							{
								bCanJudge = FALSE;
							}
						}

						if (!bCanJudge)
						{
							//超出温度范围，不能判定
							m_hvLastSpotInspTime = hvTimeCurrent;
							s_bContinueSpotInsp = FALSE;
							m_tSpotInspCount.hvecDataList[0].Clear();
							m_tSpotInspCount.hvecDataList[1].Clear();
							m_tSpotInspCount.hvFlagList[0].Clear();
							m_tSpotInspCount.hvFlagList[1].Clear();
							m_tSpotInspCount.hvDataMedian[0].Clear();
							m_tSpotInspCount.hvDataMedian[1].Clear();
							m_tSpotInspCount.hvTempData[0].Clear();
							m_tSpotInspCount.hvTempData[1].Clear();
							m_tSpotInspCount.hvTempMedian[0].Clear();
							m_tSpotInspCount.hvTempMedian[1].Clear();
							m_tSpotInspCount.hvFlagFinal = OK;
							m_tSpotInspCount.nNoDataCount = 0;

							return TRUE;
						}
					}
					catch (const std::exception&)
					{
					}
					catch (...)
					{
					}
				}
			}
			//收集数量达到设定数量，求每个数据的中值
			for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
			{
				m_tSpotInspCount.hvDataMedian[nSideSN].Clear();
				for (int i = 0; i < tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width.Length(); i++)
				{
					HTuple hvTmp;
					for (int j = 0; j < m_tSpotInspCount.hvecDataList[nSideSN].Length(); j++)
					{
						hvTmp.Append(m_tSpotInspCount.hvecDataList[nSideSN][j].T()[i].D());
					}
					m_tSpotInspCount.hvDataMedian[nSideSN].Append(hvTmp.TupleMedian());
				}
			}

			//判断最终数据
			for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
			{
				HTuple hvFlagSingle = OK;
				for (int i = 0; i < m_tSpotInspCount.hvDataMedian[nSideSN].Length(); i++)
				{
					HTuple hvFlag(OK);
					if (i >= tInfo.tParam.vecChecking.size()) continue;

					tagTolerance tol = tInfo.tParam.vecChecking[i];
					if (nSideSN == 1) tol.dStandard = tol.dStandard2;
					JudgeStdToleranceNG(&(HTuple)m_tSpotInspCount.hvDataMedian[nSideSN][i].ToTuple(), tInfo.tParam.bUseChecking, tol, &hvFlag);
					m_tSpotInspCount.hvFlagList[nSideSN].Append(hvFlag);
					if (hvFlag == NG)
					{
						hvFlagSingle = NG;
						m_tSpotInspCount.hvFlagFinal = NG;
					}
				}
				m_tSpotInspCount.hvFlagSingle[nSideSN] = hvFlagSingle;
			}

			if (m_tSpotInspCount.hvFlagFinal == NG)
			{
				CErrorInfo::tagErrorInfo tInfo;
				CString strName="";
				if (m_tSpotInspCount.hvFlagSingle[0] == NG && m_tSpotInspCount.hvFlagSingle[1] == NG)
				{
					strName = "[正面和反面]";
					m_tSpotInspCount.hvFlagFinalState = 0;
				}	
				else if (m_tSpotInspCount.hvFlagSingle[0] == NG)
				{
					strName = "[正面]";
					m_tSpotInspCount.hvFlagFinalState = 2;
				}	
				else if (m_tSpotInspCount.hvFlagSingle[1] == NG)
				{
					strName = "[反面]";
					m_tSpotInspCount.hvFlagFinalState = 3;
				}
					
				tInfo.strDescribe.Format("CCD点检异常,%s尺寸超出规格,请检查!", strName);
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查托辊是否有异动，点检片是否异常\n2检查相机有异动，镜头对焦是否清晰\n3检查点检算法参数以及规格值是否合适");
				theGlobal.m_Log.Alarm(tInfo);


				m_tSpotInspParam.bStart = TRUE;
			}
			else if (m_tSpotInspCount.hvFlagFinal == OK)
			{
				m_tSpotInspCount.hvFlagFinalState = 1;
			}

			m_hvLastSpotInspTime = hvTimeCurrent;
			s_bContinueSpotInsp = FALSE;

			m_tSpotInspCount.tEndTime = ATL::COleDateTime::GetCurrentTime();
			//记录点检结果报表
			theGlobal.m_Storage.WriteSpotInspectionReport(m_tSpotInspCount);
			theGlobal.m_pSpotInspection->m_tSpotInspCount = m_tSpotInspCount;
			theGlobal.PostMessageToSpotInspDlg(WM_SPOTINSPECTION_REFRESH);
			m_tSpotInspCount.hvecDataList[0].Clear();
			m_tSpotInspCount.hvecDataList[1].Clear();
			m_tSpotInspCount.hvFlagList[0].Clear();
			m_tSpotInspCount.hvFlagList[1].Clear();
			m_tSpotInspCount.hvDataMedian[0].Clear();
			m_tSpotInspCount.hvDataMedian[1].Clear();
			m_tSpotInspCount.hvTempData[0].Clear();
			m_tSpotInspCount.hvTempData[1].Clear();
			m_tSpotInspCount.hvTempMedian[0].Clear();
			m_tSpotInspCount.hvTempMedian[1].Clear();
			m_tSpotInspCount.hvFlagFinal = OK;
			m_tSpotInspCount.hvFlagSingle[0] = OK;
			m_tSpotInspCount.hvFlagSingle[1] = OK;
			m_tSpotInspCount.nNoDataCount = 0;
		}
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeSpotInspect:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
	}


	return TRUE;
}

BOOL CJudge::JudgeDefect(C_MergeWork& tOutput)
{
	static int s_nNGCount = 0;
	if (tOutput.vecB[0].tMeasure.hv_HasCoat.I() == FALSE
		|| (tOutput.vecB.size() > 1 && tOutput.vecB[1].tMeasure.hv_HasCoat.I() == FALSE))
	{
		s_nNGCount = 0;
		return TRUE;
	}

	for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
	{
		if (tOutput.vecB[nSideSN].tMeasure.tDefect.ho_Region.CountObj() > 0)
		{
			tOutput.tMerge.hv_DetectFlag = NG;
			tOutput.tMerge.hv_NGType.Append(tOutput.vecB[nSideSN].tMeasure.tDefect.hv_Type);
			ConcatObj(tOutput.tMerge.ho_NGRegion, tOutput.vecB[nSideSN].tMeasure.tDefect.ho_Region, &tOutput.tMerge.ho_NGRegion);

			//有瑕疵，标志位判定为NG
			//tOutput.tMerge.vecSideResult[nSideSN].hv_Flag = NG;
		}
	}

	////临时连续NG判断逻辑
	//if (tOutput.tMerge.hv_DetectFlag == NG)
	//{
	//	s_nNGCount++;
	//	if (s_nNGCount >= m_tParam.nDefectMaxNGCount)
	//	{
	//		s_nNGCount = 0;
	//		CString strExcp;
	//		strExcp.Format("瑕疵检测结果连续%d次超出规格,请检查", m_tParam.nDefectMaxNGCount);

	//		theGlobal.m_Log.Alarm(strExcp);
	//		theGlobal.m_CommunicationTool.CCDAlarm();
	//	}
	//}
	//else
	//{
	//	s_nNGCount = 0;
	//}

	return TRUE;
}

BOOL CJudge::JudgeFinal(C_MergeWork& tOutput)
{
	if (tOutput.tMerge.hv_MeasureFlag == NG || tOutput.tMerge.hv_DetectFlag == NG)
	{
		tOutput.tMerge.hv_FinalFlag = NG;
	}
	else if (tOutput.tMerge.hv_MeasureFlag == OK && tOutput.tMerge.hv_DetectFlag == OK)
	{
		tOutput.tMerge.hv_FinalFlag = OK;
	}
	else
	{
		tOutput.tMerge.hv_FinalFlag = WARN;
	}
	return TRUE;
}

BOOL CJudge::JudgeContinuousNG(C_MergeWork& tOutput)
{
	if (m_tFirstArticleInspCount.bStart) return TRUE;
	static double s_dMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();
	BOOL bHasCoat = tOutput.vecB[0].tMeasure.hv_HasCoat.I();
	if(tOutput.vecB.size() > 1) bHasCoat = bHasCoat && tOutput.vecB[1].tMeasure.hv_HasCoat.I();

	if (bHasCoat)
	{
		if (s_dMeterCount > theGlobal.m_Counter.GetCurrentMeterCount())s_dMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();
		if (theGlobal.m_Counter.GetCurrentMeterCount() < s_dMeterCount + m_tParam.dIngoreMeter)
		{
			CString strLog;
			strLog.Format("JudgeContinuousNG: 设备起涂，当前记米%0.2fm，%0.2fm内不判断连续NG", theGlobal.m_Counter.GetCurrentMeterCount(), s_dMeterCount + m_tParam.dIngoreMeter);
			//m_sInit.Log(strLog, DebugLog, NoExcp);

			ClearSizeCount(m_vecAlarmCount);
			ClearSizeCount(m_vecStopMachineCount);
			ClearDefectCount(m_vecAlarmCount);
			ClearDefectCount(m_vecStopMachineCount);

			return TRUE;
		}
	}
	else
	{
		s_dMeterCount = tOutput.hv_MeterCount.D();
		return TRUE;
	}
	
	JudgeContinuousSizeNG(tOutput);
	JudgeContinuousDefectNG(tOutput);
	JudgeMeanShiftContinuousSizeNG(tOutput);//判断Mean Shift连续NG

	return TRUE;
}

BOOL CJudge::JudgeContinuousSizeNG(C_MergeWork& tOutput)
{
	tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	HTuple hvHeight;
	GetImageSize(tOutput.vecB[0].vecA[0].hoGrabImage, &HTuple(), &hvHeight);
	double dHeightMeter = hvHeight.D() * theGlobal.m_vecResolution[0].dRow / 1000;
	try
	{
		HTuple hvNGType = tOutput.tMerge.hv_NGType;
		//尺寸,连续NG判断逻辑
		if (hvNGType.Length() > 0)
		{
			for (int i = 0; i <= NG_ABMisalign; i++)
			{
				HTuple hvInc;
				TupleFind(hvNGType, i, &hvInc);
				if (hvInc != -1 && hvInc.Length() > 0)
				{
					if (m_tParam.bUseAlarm && m_tParam.vecAlarm[i].bUse)
					{
						m_vecAlarmCount[i].hvMeters += dHeightMeter;
						if (m_vecAlarmCount[i].hvMeters.D() >= m_tParam.vecAlarm[i].hvMeters.D())
						{
							CErrorInfo::tagErrorInfo tInfo;
							tInfo.strDescribe.Format("%s连续%0.2fm(报警),请检查", theGlobal.m_Counter.GetNGTypeName(i), m_tParam.vecAlarm[i].hvMeters.D());
							tInfo.nID = 0;
							tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
							theGlobal.m_Log.Alarm(tInfo);
							theGlobal.m_CommunicationTool.CCDAlarm();
							theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
							ClearSizeCount(m_vecAlarmCount);
						}
					}
					if (m_tParam.bUseStopMachine && m_tParam.vecStopMachine[i].bUse)
					{
						m_vecStopMachineCount[i].hvMeters += dHeightMeter;
						if (m_vecStopMachineCount[i].hvMeters.D() >= m_tParam.vecStopMachine[i].hvMeters.D())
						{
							CErrorInfo::tagErrorInfo tInfo;
							tInfo.strDescribe.Format("%s连续%0.2fm(停机),请检查", theGlobal.m_Counter.GetNGTypeName(i), m_tParam.vecStopMachine[i].hvMeters.D());
							tInfo.nID = 0;
							tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
							theGlobal.m_Log.Alarm(tInfo);
							theGlobal.m_CommunicationTool.CCDStop();
							theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
							ClearSizeCount(m_vecStopMachineCount);
						}
					}
				}
				else
				{
					//当前尺寸没有报错，米数清零
					m_vecAlarmCount[i].hvMeters = 0;
					m_vecStopMachineCount[i].hvMeters = 0;
				}
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeContinuousSizeNG:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
	}



	try
	{
		//AB面基材互校准
		if (tInfo.nCoatingType == TwoSide && tOutput.vecB.size() == 2 &&
			tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE &&  tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE)
		{
			//AB面互校,判断AB面基材尺寸相差是否超出0.3
			if (abs(tOutput.vecB[0].tMeasure.tSize.hv_WholeWidth.D() - tOutput.vecB[1].tMeasure.tSize.hv_WholeWidth.D()) > 0.3)
			{
				if (m_tParam.bUseAlarm && m_tParam.vecAlarm[NG_Count + 0].bUse)
				{
					m_vecAlarmCount[NG_Count + 0].hvMeters += dHeightMeter;
					if (m_vecAlarmCount[NG_Count + 0].hvMeters.D() >= m_tParam.vecAlarm[NG_Count + 0].hvMeters.D())
					{
						CErrorInfo::tagErrorInfo tInfo;
						tInfo.strDescribe.Format("AB面基材宽度偏差连续%0.2fm超过0.3mm(报警)，请检查", m_tParam.vecAlarm[NG_Count + 0].hvMeters.D());
						tInfo.nID = 0;
						tInfo.strSolution.Format("1检查相机的k值是否合适\n");
						theGlobal.m_Log.Alarm(tInfo);
						theGlobal.m_CommunicationTool.CCDAlarm();
						theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
						m_vecAlarmCount[NG_Count + 0].hvMeters = 0;
					}
				}
				if (m_tParam.bUseStopMachine && m_tParam.vecStopMachine[NG_Count + 0].bUse)
				{
					m_vecStopMachineCount[NG_Count + 0].hvMeters += dHeightMeter;
					if (m_vecStopMachineCount[NG_Count + 0].hvMeters.D() >= m_tParam.vecStopMachine[NG_Count + 0].hvMeters.D())
					{
						CErrorInfo::tagErrorInfo tInfo;
						tInfo.strDescribe.Format("AB面基材宽度偏差连续%0.2fm超过0.3mm(停机)，请检查", m_tParam.vecStopMachine[NG_Count + 0].hvMeters.D());
						tInfo.nID = 0;
						tInfo.strSolution.Format("1检查相机的k值是否合适\n");
						theGlobal.m_Log.Alarm(tInfo);
						theGlobal.m_CommunicationTool.CCDStop();
						theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
						m_vecStopMachineCount[NG_Count + 0].hvMeters = 0;
					}
				}
			}
			else
			{
				m_vecAlarmCount[NG_Count + 0].hvMeters = 0;
				m_vecStopMachineCount[NG_Count + 0].hvMeters = 0;
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeContinuousSizeNG:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
	}


	return TRUE;
}

BOOL CJudge::JudgeSumNG(C_MergeWork& tOutput)
{
	tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	HTuple hvHeight;
	GetImageSize(tOutput.vecB[0].vecA[0].hoGrabImage, &HTuple(), &hvHeight);
	double dHeightMeter = hvHeight.D() * theGlobal.m_vecResolution[0].dRow / 1000;
	try
	{
		//累计NG判断逻辑
		static bool bNgFlag;
		static bool bNgFlag1;
		if (tOutput.tMerge.hv_FinalFlag.I() == NG && m_tParam.bUseNGAlarm)
		{
			m_dJudgeSumMetes += dHeightMeter;
			if (m_dJudgeSumMetes<30)
			{
				bNgFlag = true;
			}
			if (m_dJudgeSumMetes<55)
			{
				bNgFlag1 = true;
			}
			if (m_dJudgeSumMetes >= 30&& m_dJudgeSumMetes<45&& bNgFlag)
			{
				bNgFlag = false;
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("本卷累计NG米数超过30米,请检查！当前NG米数：%0.2fm", m_dJudgeSumMetes);
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
			}
			if (m_dJudgeSumMetes > 45&& bNgFlag1)
			{
				bNgFlag1 = false;
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("本卷累计NG米数超过45米,请检查！当前NG米数：%0.2fm", m_dJudgeSumMetes);
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeSumNG:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
	}
	return TRUE;
}

BOOL CJudge::JudgeContinuousDefectNG(C_MergeWork& tOutput)
{
	tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	HTuple hvHeight;
	GetImageSize(tOutput.vecB[0].vecA[0].hoGrabImage, &HTuple(), &hvHeight);
	double dHeightMeter = hvHeight.D() * theGlobal.m_vecResolution[0].dRow / 1000;

	try
	{
		//瑕疵,连续NG判断逻辑,实现连续多少米内，瑕疵面积大于多少或者数量大于多少就报警/停机
		HTuple hvDefectType, hvDefectArea;
		hvDefectType.Append(tOutput.vecB[0].tMeasure.tDefect.hv_Type);
		hvDefectArea.Append(tOutput.vecB[0].tMeasure.tDefect.tFeature.hvArea);
		if (tOutput.vecB.size() > 1)
		{
			hvDefectType.Append(tOutput.vecB[1].tMeasure.tDefect.hv_Type);
			hvDefectArea.Append(tOutput.vecB[1].tMeasure.tDefect.tFeature.hvArea);
		}
		if (hvDefectType.Length() > 0)
		{
			for (int i = NG_ExposeFoil; i < NG_Count; i++)
			{
				HTuple hvInc;
				TupleFind(hvDefectType, i, &hvInc);
				if (hvInc != -1 && hvInc.Length() > 0)
				{
					if (m_tParam.bUseAlarm && m_tParam.vecAlarm[i].bUse)
					{
						m_vecAlarmCount[i].hvMeters.Append(dHeightMeter);
						HTuple hvArea;
						TupleSelect(hvDefectArea, hvInc, &hvArea);
						m_vecAlarmCount[i].hvArea.Append(hvArea.TupleSum().D());
						m_vecAlarmCount[i].hvNum.Append(hvArea.Length());
						if (m_vecAlarmCount[i].hvMeters.TupleSum().D() > m_tParam.vecAlarm[i].hvMeters.D())
						{
							TupleRemove(m_vecAlarmCount[i].hvMeters, 0, &m_vecAlarmCount[i].hvMeters);
							TupleRemove(m_vecAlarmCount[i].hvArea, 0, &m_vecAlarmCount[i].hvArea);
							TupleRemove(m_vecAlarmCount[i].hvNum, 0, &m_vecAlarmCount[i].hvNum);
						}
						if ((m_vecAlarmCount[i].hvArea.TupleSum().D() > m_tParam.vecAlarm[i].hvArea)
							|| (m_vecAlarmCount[i].hvNum.TupleSum().I() > m_tParam.vecAlarm[i].hvNum))
						{
							CErrorInfo::tagErrorInfo tInfo;
							tInfo.strDescribe.Format("%s%0.2fm内出现数量大于%d或者面积大于%0.2fmm2(报警),请检查", theGlobal.m_Counter.GetNGTypeName(i),
								m_tParam.vecAlarm[i].hvMeters.D(), m_tParam.vecAlarm[i].hvNum.I(), m_tParam.vecAlarm[i].hvArea.D());
							tInfo.nID = 0;
							tInfo.strSolution.Format("1检查模头涂的是否有问题\n2检查瑕疵规格是否合适\n");
							theGlobal.ShowDefectImageDlg(tOutput);//报警弹窗的同时显示瑕疵图像——1116
							theGlobal.m_Log.Alarm(tInfo);
							theGlobal.m_CommunicationTool.CCDAlarm();
							theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
							ClearDefectCount(m_vecAlarmCount);
						}
					}
					if (m_tParam.bUseAlarm && m_tParam.vecStopMachine[i].bUse)
					{
						m_vecStopMachineCount[i].hvMeters.Append(dHeightMeter);
						HTuple hvArea;
						TupleSelect(hvDefectArea, hvInc, &hvArea);
						m_vecStopMachineCount[i].hvArea.Append(hvArea.TupleSum().D());
						m_vecStopMachineCount[i].hvNum.Append(hvArea.Length());
						if (m_vecStopMachineCount[i].hvMeters.TupleSum().D() > m_tParam.vecStopMachine[i].hvMeters.D())
						{
							TupleRemove(m_vecStopMachineCount[i].hvMeters, 0, &m_vecStopMachineCount[i].hvMeters);
							TupleRemove(m_vecStopMachineCount[i].hvArea, 0, &m_vecStopMachineCount[i].hvArea);
							TupleRemove(m_vecStopMachineCount[i].hvNum, 0, &m_vecStopMachineCount[i].hvNum);
						}
						if ((m_vecStopMachineCount[i].hvArea.TupleSum().D() > m_tParam.vecAlarm[i].hvArea)
							|| (m_vecStopMachineCount[i].hvNum.TupleSum().I() > m_tParam.vecAlarm[i].hvNum))
						{
							CErrorInfo::tagErrorInfo tInfo;
							tInfo.strDescribe.Format("%s%0.2fm内出现数量大于%d或者面积大于%0.2fmm2(停机)，请检查", theGlobal.m_Counter.GetNGTypeName(i),
								m_tParam.vecStopMachine[i].hvMeters.D(), m_tParam.vecStopMachine[i].hvNum.I(), m_tParam.vecStopMachine[i].hvArea.D());
							tInfo.nID = 0;
							tInfo.strSolution.Format("1检查模头涂的是否有问题\n2检查瑕疵规格是否合适\n");
							theGlobal.m_Log.Alarm(tInfo);
							theGlobal.m_Log.Alarm(tInfo);
							theGlobal.m_CommunicationTool.CCDStop();
							theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
							ClearDefectCount(m_vecStopMachineCount);
						}
					}
				}
				else
				{
					//报警统计
					m_vecAlarmCount[i].hvMeters.Append(dHeightMeter);
					m_vecAlarmCount[i].hvArea.Append(0);
					m_vecAlarmCount[i].hvNum.Append(0);
					if (m_vecAlarmCount[i].hvMeters.TupleSum().D() > m_tParam.vecAlarm[i].hvMeters.D())
					{
						TupleRemove(m_vecAlarmCount[i].hvMeters, 0, &m_vecAlarmCount[i].hvMeters);
						TupleRemove(m_vecAlarmCount[i].hvArea, 0, &m_vecAlarmCount[i].hvArea);
						TupleRemove(m_vecAlarmCount[i].hvNum, 0, &m_vecAlarmCount[i].hvNum);
					}
					//停机统计
					m_vecStopMachineCount[i].hvMeters.Append(dHeightMeter);
					m_vecStopMachineCount[i].hvArea.Append(0);
					m_vecStopMachineCount[i].hvNum.Append(0);
					if (m_vecStopMachineCount[i].hvMeters.TupleSum().D() > m_tParam.vecStopMachine[i].hvMeters.D())
					{
						TupleRemove(m_vecStopMachineCount[i].hvMeters, 0, &m_vecStopMachineCount[i].hvMeters);
						TupleRemove(m_vecStopMachineCount[i].hvArea, 0, &m_vecStopMachineCount[i].hvArea);
						TupleRemove(m_vecStopMachineCount[i].hvNum, 0, &m_vecStopMachineCount[i].hvNum);
					}
				}
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeContinuousDefectNG:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
		return FALSE;
	}

	return TRUE;
}

BOOL CJudge::JudgeMeanShiftContinuousSizeNG(C_MergeWork& tOutput)
{
	if (m_tFirstArticleInspCount.bStart) return TRUE;
	tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	HTuple hvHeight;
	GetImageSize(tOutput.vecB[0].vecA[0].hoGrabImage, &HTuple(), &hvHeight);
	double dHeightMeter = hvHeight.D() * theGlobal.m_vecResolution[0].dRow / 1000;

	try
	{
		//A面尺寸
		m_tMeanShiftData.hvecCoatData[0][m_tMeanShiftData.hvecCoatData[0].Length()].T() = tOutput.vecB[0].tMeasure.tCoat.hv_Width;
		m_tMeanShiftData.hvecFoilData[0][m_tMeanShiftData.hvecFoilData[0].Length()].T() = tOutput.vecB[0].tMeasure.tFoil.hv_Width;
		if(tInfo.bExistAT11)
			m_tMeanShiftData.hvecATBaseCoatData[0][m_tMeanShiftData.hvecATBaseCoatData[0].Length()].T() = tOutput.vecB[0].tMeasure.tAT11.hv_Width;
		else if(tInfo.bExistBaseCoat)
			m_tMeanShiftData.hvecATBaseCoatData[0][m_tMeanShiftData.hvecATBaseCoatData[0].Length()].T() = tOutput.vecB[0].tMeasure.tAT11.hv_Width;
		
		//B面尺寸
		if (tOutput.vecB.size() > 1 && tInfo.nCoatingType == TwoSide)
		{
			m_tMeanShiftData.hvecCoatData[1][m_tMeanShiftData.hvecCoatData[1].Length()].T() = tOutput.vecB[1].tMeasure.tCoat.hv_Width;
			m_tMeanShiftData.hvecFoilData[1][m_tMeanShiftData.hvecFoilData[1].Length()].T() = tOutput.vecB[1].tMeasure.tFoil.hv_Width;
			if (tInfo.bExistAT11)
				m_tMeanShiftData.hvecATBaseCoatData[1][m_tMeanShiftData.hvecATBaseCoatData[1].Length()].T() = tOutput.vecB[1].tMeasure.tAT11.hv_Width;
			else if (tInfo.bExistBaseCoat)
				m_tMeanShiftData.hvecATBaseCoatData[1][m_tMeanShiftData.hvecATBaseCoatData[1].Length()].T() = tOutput.vecB[1].tMeasure.tAT11.hv_Width;

			//错位
			m_tMeanShiftData.hvecMisalignmentData[m_tMeanShiftData.hvecMisalignmentData.Length()].T() = tOutput.tMerge.hv_Misalignment;
		}

		//判断Mean Shift,涂布尺寸
		if (m_tMeanShiftData.hvecCoatData[0].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_CoatWidth].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_CoatWidth].bUse)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecCoatData[0][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecCoatData[0].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecCoatData[0][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecCoatData[0][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecCoatWidth[0].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecCoatWidth[0].dTolerance;
				double dLimitUpper = tInfo.tParam.vecCoatWidth[0].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecCoatWidth[0].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("正面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth), m_tParam.vecMeanShiftAlarmParam[NG_CoatWidth].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecCoatData[0].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecCoatData[0] = m_tMeanShiftData.hvecCoatData[0].Remove(0);
			}
		}
		if (m_tMeanShiftData.hvecCoatData[1].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_CoatWidth].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_CoatWidth].bUse && tInfo.nCoatingType == TwoSide)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecCoatData[1][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecCoatData[1].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecCoatData[1][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecCoatData[1][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecCoatWidth[0].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecCoatWidth[0].dTolerance;
				double dLimitUpper = tInfo.tParam.vecCoatWidth[0].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecCoatWidth[0].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("反面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth), m_tParam.vecMeanShiftAlarmParam[NG_CoatWidth].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecCoatData[1].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecCoatData[1] = m_tMeanShiftData.hvecCoatData[1].Remove(0);
			}
		}

		//判断Mean Shift,极耳尺寸
		if (m_tMeanShiftData.hvecFoilData[0].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_FoilWidth].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_FoilWidth].bUse)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecFoilData[0][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecFoilData[0].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecFoilData[0][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecFoilData[0][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				if (i > tInfo.tParam.vecFoilWidth.size() - 1)continue;
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecFoilWidth[i].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecFoilWidth[i].dTolerance;
				double dLimitUpper = tInfo.tParam.vecFoilWidth[i].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecFoilWidth[i].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("正面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_FoilWidth), m_tParam.vecMeanShiftAlarmParam[NG_FoilWidth].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecFoilData[0].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecFoilData[0] = m_tMeanShiftData.hvecFoilData[0].Remove(0);
			}
		}
		if (m_tMeanShiftData.hvecFoilData[1].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_FoilWidth].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_FoilWidth].bUse && tInfo.nCoatingType == TwoSide)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecFoilData[1][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecFoilData[1].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecFoilData[1][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecFoilData[1][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				if (i > tInfo.tParam.vecFoilWidth.size() - 1)continue;
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecFoilWidth[i].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecFoilWidth[i].dTolerance;
				double dLimitUpper = tInfo.tParam.vecFoilWidth[i].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecFoilWidth[i].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("反面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_FoilWidth), m_tParam.vecMeanShiftAlarmParam[NG_FoilWidth].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecFoilData[1].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecFoilData[1] = m_tMeanShiftData.hvecFoilData[1].Remove(0);
			}
		}

		//判断Mean Shift,AT尺寸
		if (m_tMeanShiftData.hvecATBaseCoatData[0].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_AT11Width].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_AT11Width].bUse && tInfo.bExistAT11)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecATBaseCoatData[0][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecATBaseCoatData[0].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecATBaseCoatData[0][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecATBaseCoatData[0][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecAT11Width[0].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecAT11Width[0].dTolerance;
				double dLimitUpper = tInfo.tParam.vecAT11Width[0].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecAT11Width[0].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("正面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_AT11Width), m_tParam.vecMeanShiftAlarmParam[NG_AT11Width].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecATBaseCoatData[0].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecATBaseCoatData[0] = m_tMeanShiftData.hvecATBaseCoatData[0].Remove(0);
			}
		}
		if (m_tMeanShiftData.hvecATBaseCoatData[1].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_AT11Width].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_AT11Width].bUse && tInfo.bExistAT11 && tInfo.nCoatingType == TwoSide)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecATBaseCoatData[1][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecATBaseCoatData[1].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecATBaseCoatData[1][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecATBaseCoatData[1][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecAT11Width[0].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecAT11Width[0].dTolerance;
				double dLimitUpper = tInfo.tParam.vecAT11Width[0].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecAT11Width[0].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("反面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_AT11Width), m_tParam.vecMeanShiftAlarmParam[NG_AT11Width].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecATBaseCoatData[1].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecATBaseCoatData[1] = m_tMeanShiftData.hvecATBaseCoatData[1].Remove(0);
			}
		}

		//判断Mean Shift,凹版尺寸
		if (m_tMeanShiftData.hvecATBaseCoatData[0].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_BaseCoatWidth].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_BaseCoatWidth].bUse && tInfo.bExistBaseCoat)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecATBaseCoatData[0][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecATBaseCoatData[0].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecATBaseCoatData[0][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecATBaseCoatData[0][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecBaseCoatWidth[0].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecBaseCoatWidth[0].dTolerance;
				double dLimitUpper = tInfo.tParam.vecBaseCoatWidth[0].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecBaseCoatWidth[0].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("正面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_BaseCoatWidth), m_tParam.vecMeanShiftAlarmParam[NG_BaseCoatWidth].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecATBaseCoatData[0].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecATBaseCoatData[0] = m_tMeanShiftData.hvecATBaseCoatData[0].Remove(0);
			}
		}
		if (m_tMeanShiftData.hvecATBaseCoatData[1].Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_BaseCoatWidth].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_BaseCoatWidth].bUse && tInfo.bExistBaseCoat && tInfo.nCoatingType == TwoSide)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecATBaseCoatData[1][0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecATBaseCoatData[1].Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecATBaseCoatData[1][j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecATBaseCoatData[1][j].T()[i].D());
				}
				hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecBaseCoatWidth[0].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecBaseCoatWidth[0].dTolerance;
				double dLimitUpper = tInfo.tParam.vecBaseCoatWidth[0].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecBaseCoatWidth[0].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("反面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_BaseCoatWidth), m_tParam.vecMeanShiftAlarmParam[NG_BaseCoatWidth].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecATBaseCoatData[1].Clear();
			}
			else
			{
				m_tMeanShiftData.hvecATBaseCoatData[1] = m_tMeanShiftData.hvecATBaseCoatData[1].Remove(0);
			}
		}

		//判断Mean Shift,错位尺寸
		if (m_tMeanShiftData.hvecMisalignmentData.Length()*dHeightMeter >= m_tParam.vecMeanShiftAlarmParam[NG_ABMisalign].hvMeters.D()
			&& m_tParam.vecMeanShiftAlarmParam[NG_ABMisalign].bUse&& tInfo.nCoatingType == TwoSide)
		{
			HTuple hvMeanShift;
			for (int i = 0; i < m_tMeanShiftData.hvecMisalignmentData[0].T().Length(); i++)
			{
				HTuple hvTmp;
				for (int j = 0; j < m_tMeanShiftData.hvecMisalignmentData.Length(); j++)
				{
					if (abs(m_tMeanShiftData.hvecMisalignmentData[j].T()[i].D()) < abs(NO_DATA / 4))
						hvTmp.Append(m_tMeanShiftData.hvecMisalignmentData[j].T()[i].D());
				}
				if(hvTmp.Length() > 0)
					hvMeanShift.Append(hvTmp.TupleMean());
			}
			//判断NG规格
			HTuple hvFlag = OK;
			for (int i = 0; i < hvMeanShift.Length(); i++)
			{
				double dVal = hvMeanShift[i].D();
				double dLimitLower = tInfo.tParam.vecMisalignment[0].dStandard - abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecMisalignment[0].dTolerance;
				double dLimitUpper = tInfo.tParam.vecMisalignment[0].dStandard + abs(m_tParam.dMeanShiftLimitFactor)*tInfo.tParam.vecMisalignment[0].dTolerance;
				if (dVal > dLimitUpper || dVal <= dLimitLower)
				{
					if (hvFlag != NG)
						hvFlag = NG;
				}
			}
			if (NG == hvFlag)
			{
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("反面%s连续%0.2fm Mean Shift NG(报警),请检查",
					theGlobal.m_Counter.GetNGTypeName(NG_ABMisalign), m_tParam.vecMeanShiftAlarmParam[NG_ABMisalign].hvMeters.D());
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查设备模头涂的尺寸是否合适\n2检查CCD寻边是否正确\n3检查设定配方规格是否准确");
				theGlobal.m_Log.Alarm(tInfo);
				theGlobal.m_CommunicationTool.CCDAlarm();
				theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
				m_tMeanShiftData.hvecMisalignmentData.Clear();
			}
			else
			{
				m_tMeanShiftData.hvecMisalignmentData = m_tMeanShiftData.hvecMisalignmentData.Remove(0);
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeMeanShiftContinuousSizeNG:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
	}


	return TRUE;
}

BOOL CJudge::JudgeStdTolerance(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag)
{
	*hvFlag = OK;
	if ((*hvVal).Length() == 0 || (*hvVal) == NO_DATA || !bJudge) return FALSE;

	if (*hvVal > (tStdTolerance.dStandard + abs(tStdTolerance.dTolerance)) || *hvVal < (tStdTolerance.dStandard - abs(tStdTolerance.dTolerance)))
	{
		*hvFlag = NG;
		return TRUE;
	}

	if (*hvVal > (tStdTolerance.dStandard + abs(tStdTolerance.dWarnTolerance)) || *hvVal < (tStdTolerance.dStandard - abs(tStdTolerance.dWarnTolerance)))
	{
		*hvFlag = WARN;
	}

	return TRUE;
}

BOOL CJudge::JudgeStdToleranceNG(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag)
{
	*hvFlag = OK;
	if ((*hvVal).Length() == 0 || (*hvVal) == NO_DATA || !bJudge)
	{
		*hvFlag = FALSE;
		return FALSE;
	}

	if (*hvVal > (tStdTolerance.dStandard + abs(tStdTolerance.dTolerance)) || *hvVal < (tStdTolerance.dStandard - abs(tStdTolerance.dTolerance)))
	{
		*hvFlag = NG;
	}

	return TRUE;
}

BOOL CJudge::JudgeFAITolerance(HTuple* hvVal, const BOOL& bJudge, const tagTolerance& tStdTolerance, HTuple* hvFlag)
{
	*hvFlag = OK;
	if ((*hvVal).Length() == 0 || (*hvVal) == NO_DATA || !bJudge) return FALSE;

	if (*hvVal > (tStdTolerance.dStandard + abs(tStdTolerance.dFAITolerance)) || *hvVal < (tStdTolerance.dStandard - abs(tStdTolerance.dFAITolerance)))
	{
		*hvFlag = NG;
		return TRUE;
	}

	return TRUE;
}

void CJudge::ClearSizeCount(std::vector<tagSingleJudgeParam> &vecCount)
{
	for (int i = 0; i <= NG_ABMisalign; i++)
	{
		if (i >= vecCount.size()) break;
		vecCount[i].hvMeters = 0;
	}
}

void CJudge::ClearDefectCount(std::vector<tagSingleJudgeParam> &vecCount)
{
	for (int i = NG_ExposeFoil; i < NG_Count; i++)
	{
		if (i >= vecCount.size()) break;
		vecCount[i].hvArea = 0;
		vecCount[i].hvMeters = 0;
		vecCount[i].hvNum = 0;
	}
}

BOOL CJudge::JudgeFirstArticleInspection(C_MergeWork& tOutput)
{
	if (!m_tFirstArticleInspCount.bStartIgnore || m_tFirstArticleInspCount.bStartCalculateCompen) return TRUE;

	if (!tOutput.vecB[0].tMeasure.hv_HasCoat.I()) return FALSE;
	if (tOutput.vecB.size() > 1 && !tOutput.vecB[1].tMeasure.hv_HasCoat.I()) return FALSE;

	m_tFirstArticleInspCount.nIgnoreDataCount++;

	if (m_tFirstArticleInspCount.nIgnoreDataCount <= m_tFirstArticleInspParam.nStartIgnoreDataNum) return TRUE;
	if (m_tFirstArticleInspCount.nIgnoreDataCount == m_tFirstArticleInspParam.nStartIgnoreDataNum + 1)
	{
		//确认寻边
		theGlobal.m_AlgTool.CheckMeasure();
		//m_tFirstArticleInspCount.bStart = TRUE;
	}

	if (!m_tFirstArticleInspCount.bStart) return TRUE;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	try
	{
		//判断首件数据是否有效
		HTuple hvSize;
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			hvSize.Append(tOutput.vecB[nSideSN].tMeasure.tSize.hv_Width);
		}
		HTuple hvInc;
		TupleFind(hvSize, NO_DATA, &hvInc);
		if ((hvInc.Length() >= 1 && hvInc != -1) || hvInc.Length() == 0)
		{
			m_tFirstArticleInspCount.nNoDataCount++;

			//如果一次点检流程无效数据超过设定次数，弹框报警
			if (m_tFirstArticleInspCount.nNoDataCount > m_tFirstArticleInspParam.nNoDataLimit)
			{
				m_tFirstArticleInspCount.nNoDataCount = 0;
				CErrorInfo::tagErrorInfo tInfo;
				tInfo.strDescribe.Format("首件无效数据超过%d个,请检查", m_tFirstArticleInspParam.nNoDataLimit);
				tInfo.nID = 0;
				tInfo.strSolution.Format("1检查是否有空带，或者空带打皱\n2检查测量边是否有问题");
				theGlobal.m_Log.Alarm(tInfo);

				m_tFirstArticleInspCount.Clear();
				m_tFirstArticleInspCount.bStart = FALSE;
				m_tFirstArticleInspCount.bStartIgnore = FALSE;
				m_tFirstArticleInspCount.nIgnoreDataCount = 0;
			}
			return TRUE;
		}
		//保留数据
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			m_tFirstArticleInspCount.hvecDataList[nSideSN][m_tFirstArticleInspCount.hvecDataList[nSideSN].Length()].T()
				= tOutput.vecB[nSideSN].tMeasure.tSize.hv_Width;
		}
		if (tOutput.vecB.size() > 1)
		{
			m_tFirstArticleInspCount.hvecMisaglinDataList[m_tFirstArticleInspCount.hvecMisaglinDataList.Length()].T()
				= tOutput.tMerge.hv_Misalignment;
		}
		if (m_tFirstArticleInspCount.hvecDataList[0].Length() > m_tFirstArticleInspParam.nCountinousDataNum)
		{
			//收集数量达到设定数量，每5个数据求一个中值
			//尺寸
			for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
			{
				m_tFirstArticleInspCount.hvecDataFilterList[nSideSN].Clear();
				for (int i = 0; i < tOutput.vecB[nSideSN].tMeasure.tSize.hv_Width.Length(); i++)
				{
					HTuple hvTmp;
					for (int j = 0; j < m_tFirstArticleInspCount.hvecDataList[nSideSN].Length(); j++)
					{
						hvTmp.Append(m_tFirstArticleInspCount.hvecDataList[nSideSN][j].T()[i].D());
						if (hvTmp.Length() == 5)
						{
							m_tFirstArticleInspCount.hvecDataFilterList[nSideSN][j / 5].T().Append(hvTmp.TupleMean());
							hvTmp.Clear();
						}
					}
				}
			}
			//错位
			if (tOutput.vecB.size() > 1)
			{
				m_tFirstArticleInspCount.hvecMisaglinDataFilterList.Clear();
				for (int i = 0; i < tOutput.tMerge.hv_Misalignment.Length(); i++)
				{
					HTuple hvTmp;
					for (int j = 0; j < m_tFirstArticleInspCount.hvecMisaglinDataList.Length(); j++)
					{
						hvTmp.Append(m_tFirstArticleInspCount.hvecMisaglinDataList[j].T()[i].D());
						if (hvTmp.Length() == 5)
						{
							m_tFirstArticleInspCount.hvecMisaglinDataFilterList[j / 5].T().Append(hvTmp.TupleMean());
							hvTmp.Clear();
						}
					}
				}
			}
	
			//判断最终数据
			for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
			{
				HTuple hvData;
				for (int i = 0; i < m_tFirstArticleInspCount.hvecDataFilterList[nSideSN].Length(); i++)
				{
					hvData = m_tFirstArticleInspCount.hvecDataFilterList[nSideSN][i].T();

					int nCount = 0;
					HTuple hvFoil, hvATBaseCoat, hvCoat, hvFlag;
					//极耳
					hvFoil.Append(hvData[nCount++].D());
					for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
					{
						if (tPdInfo.bExistAT11)
						{
							hvATBaseCoat.Append(hvData[nCount++].D());
						}
						else if (tPdInfo.bExistBaseCoat)
						{
							hvATBaseCoat.Append(hvData[nCount++].D());
						}
						hvCoat.Append(hvData[nCount++].D());
						if (tPdInfo.bExistAT11)
						{
							hvATBaseCoat.Append(hvData[nCount++].D());
						}
						else if (tPdInfo.bExistBaseCoat)
						{
							hvATBaseCoat.Append(hvData[nCount++].D());
						}
						hvFoil.Append(hvData[nCount++].D());
					}

					//极耳判断
					for (int i = 0; i < hvFoil.Length(); i++)
					{
						JudgeFAITolerance(&(HTuple)hvFoil[i], TRUE, tPdInfo.tParam.vecFoilWidth[i], &hvFlag);
						//m_tFirstArticleInspCount.hvFlagList[0].Append(hvFlag);
						if (hvFlag == NG) m_tFirstArticleInspCount.hvFlagFinal = NG;
					}
					//AT判断
					if (tPdInfo.bExistAT11)
					{
						for (int i = 0; i < hvATBaseCoat.Length(); i++)
						{
							JudgeFAITolerance(&(HTuple)hvATBaseCoat[i], TRUE, tPdInfo.tParam.vecAT11Width[0], &hvFlag);
							//m_tFirstArticleInspCount.hvFlagList[0].Append(hvFlag);
							if (hvFlag == NG) m_tFirstArticleInspCount.hvFlagFinal = NG;
						}
					}
					else if (tPdInfo.bExistBaseCoat)
					{
						for (int i = 0; i < hvATBaseCoat.Length(); i++)
						{
							JudgeFAITolerance(&(HTuple)hvATBaseCoat[i], TRUE, tPdInfo.tParam.vecBaseCoatWidth[0], &hvFlag);
							//m_tFirstArticleInspCount.hvFlagList[0].Append(hvFlag);
							if (hvFlag == NG) m_tFirstArticleInspCount.hvFlagFinal = NG;
						}
					}
					//涂膜判断
					for (int i = 0; i < hvCoat.Length(); i++)
					{
						JudgeFAITolerance(&(HTuple)hvCoat[i], TRUE, tPdInfo.tParam.vecCoatWidth[i], &hvFlag);
						//m_tFirstArticleInspCount.hvFlagList[0].Append(hvFlag);
						if (hvFlag == NG) m_tFirstArticleInspCount.hvFlagFinal = NG;
					}
				}
			}
			//错位判断
			for (int i = 0; i < m_tFirstArticleInspCount.hvecMisaglinDataFilterList.Length(); i++)
			{
				HTuple hvData = m_tFirstArticleInspCount.hvecMisaglinDataFilterList[i].T();
				for (int j = 0; j < hvData.Length(); j++)
				{
					HTuple hvFlag;
					JudgeFAITolerance(&(HTuple)hvData[j], TRUE, tPdInfo.tParam.vecMisalignment[0], &hvFlag);
					if (hvFlag == NG) m_tFirstArticleInspCount.hvFlagFinal = NG;
				}
			}

			if (m_tFirstArticleInspCount.hvFlagFinal == NG)
			{
				CString strLog;
				strLog.Format("CCD首件NG,尺寸超出规格");
				theGlobal.m_Log.Std(strLog);
				theGlobal.SetTipInfo(TIP_FirstArticleNG);
				theGlobal.m_CommunicationTool.CCDAlarm();
			}
			else
			{
				CString strLog;
				strLog.Format("CCD首件OK");
				theGlobal.m_Log.Std(strLog);
				theGlobal.SetTipInfo(TIP_FirstArticleOK);
			}

			//写首件结果数据
			theGlobal.m_Storage.WriteFirstArticleInspReport(m_tFirstArticleInspCount);

			m_tFirstArticleInspCount.Clear();
			m_tFirstArticleInspCount.bStartIgnore = FALSE;
			m_tFirstArticleInspCount.bStart = FALSE;
			m_tFirstArticleInspCount.nIgnoreDataCount = 0;
		}

	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeFirstArticleInspection:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
		return FALSE;
	}

	return TRUE;
}

void CJudge::ClearJudgeSumMetes()
{
	m_dJudgeSumMetes = 0;
}

int CJudge::JudgeCheck() 
{
	try
	{
		int n_CheckState = 0;
		if (m_tSpotInspCount.hvFlagFinalState.Length()==0)
		{
			n_CheckState = 0;
			return n_CheckState;
		}
		else if (m_tSpotInspCount.hvFlagFinalState.Length() > 0 && m_tSpotInspCount.hvFlagFinalState == 0)
		{
			n_CheckState = 1;
			return n_CheckState;
		}
		else if (m_tSpotInspCount.hvFlagFinalState.Length() > 0 && m_tSpotInspCount.hvFlagFinalState == 1)
		{
			n_CheckState = 2;
			return n_CheckState;
		}
		else if (m_tSpotInspCount.hvFlagFinalState.Length() > 0 && m_tSpotInspCount.hvFlagFinalState == 2)
		{
			n_CheckState = 3;
			return n_CheckState;
		}
		else if (m_tSpotInspCount.hvFlagFinalState.Length() > 0 && m_tSpotInspCount.hvFlagFinalState == 3)
		{
			n_CheckState = 4;
			return n_CheckState;
		}
	}
	catch (HException& e)
	{
		return FALSE;
	}
}