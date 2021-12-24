#include "StdAfx.h"
#include "Judge.h"

CJudge::CJudge()
{
	//m_nEACount[0] = 0;
	//m_nEACount[1] = 0;
	//ClearEA(0);
	//ClearEA(1);
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
	m_tParam.nSpotInspMaxNGCount = GetPrivateProfileInt("Judge", "nSpotInspMaxNGCount", 5, m_strParamFile);

	m_tParam.dFirstArticleInspMeter = GetPrivateProfileDouble("Judge", "dFirstArticleInspMeter", 1, m_strParamFile);

	m_tParam.bUseAlarm = GetPrivateProfileInt("Judge", "UseAlarm", FALSE, m_strParamFile);
	m_tParam.bUseStopMachine = GetPrivateProfileInt("Judge", "UseStopMachine", FALSE, m_strParamFile);

	m_tParam.vecAlarm.clear();
	m_tParam.vecStopMachine.clear();

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
	}

	return TRUE;
}

BOOL CJudge::WriteParam()
{
	//WritePrivateProfileInt("Judge", "nSpotInspMaxNGCount", m_tParam.nSpotInspMaxNGCount, m_strParamFile);
	WritePrivateProfileDouble("Judge", "dFirstArticleInspMeter", m_tParam.dFirstArticleInspMeter, m_strParamFile);
	WritePrivateProfileInt("Judge", "UseAlarm", m_tParam.bUseAlarm, m_strParamFile);
	WritePrivateProfileInt("Judge", "UseStopMachine", m_tParam.bUseStopMachine, m_strParamFile);

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

		tParam = m_tParam.vecStopMachine[i];
		strVal.Format("StopMachine%dUse", i);
		WritePrivateProfileInt("Judge", strVal, tParam.bUse, m_strParamFile);
		strVal.Format("StopMachine%dMeter", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvMeters.D(), m_strParamFile);
		strVal.Format("StopMachine%dArea", i);
		WritePrivateProfileDouble("Judge", strVal, tParam.hvArea.D(), m_strParamFile);
		strVal.Format("StopMachine%dNum", i);
		WritePrivateProfileInt("Judge", strVal, tParam.hvNum.I(), m_strParamFile);
	}

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
	TupleFind(tOutput.vecB[0].tMeasure.tSize.hv_Width, NO_DATA, &hv_Inc);
	if (hv_Inc != -1 && hv_Inc.Length() != 0) return FALSE;
	TupleFind(tOutput.vecB[0].tMeasure.tSize.hv_Width, 0, &hv_Inc);
	if (hv_Inc != -1 && hv_Inc.Length() != 0) return FALSE;

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

	tOutput.tMerge.hv_MisalignmentFlagList.Clear();

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
		JudgeSpotInspect(tOutput);//点检结果判定
		JudgeDefect(tOutput);//瑕疵结果判定
		JudgeFinal(tOutput);//最终结果判定

		JudgeContinuousNG(tOutput);//判断连续NG

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

BOOL CJudge::JudgeSpotInspect(C_MergeWork& tOutput)
{
	//点检
	tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//if (!tInfo.tParam.bUseChecking && theGlobal.GetSystemSet().nActionMode != DebugMode) return TRUE;
	if (!tInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode)
	{
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			TupleGenConst(5, NO_DATA, &tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
			TupleGenConst(5, OK, &tOutput.tMerge.vecSideResult[nSideSN].hv_SpotInspectionFlagList);
		}
		return TRUE;
	}

	static HTuple s_hvTimeCurrent = 0, s_hvTimeOld = 0;
	static BOOL s_bContinueSpotInsp = TRUE;
	//点检间隔判断
	CountSeconds(&s_hvTimeCurrent);
	if (!s_bContinueSpotInsp)
	{
		if ((s_hvTimeCurrent - s_hvTimeOld) * 1000 < theGlobal.m_tSystemSet.nSpotInspectionIntervalSecond)
		{
			for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
			{
				TupleGenConst(tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width.Length(), NO_DATA, &tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
			}
			return TRUE;
		}
	}
	

	//判断点检参数是否有效
	HTuple hvSize;
	for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
	{
		hvSize.Append(tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
	}
	HTuple hvInc;
	TupleFind(hvSize, NO_DATA, &hvInc);
	if ((hvInc.Length() >= 1 && hvInc != -1) || hvInc.Length() == 0 || hvSize.Length() != 10)
	{
		return TRUE;
	}

	//判断点检NG
	static int s_nNGCount = 0;
	try
	{
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			HTuple hvSideSNList = theGlobal.m_Project.GetSideToCamSNList(nSideSN);
			for (int i = 0; i < tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width.Length(); i++)
			{
				HTuple hvFlag(OK);
				if (i >= tInfo.tParam.vecChecking.size()) continue;
				if (tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width[i] == NO_DATA) continue;
				tagTolerance tol = tInfo.tParam.vecChecking[i];
				if (nSideSN == 1) tol.dStandard = tol.dStandard2;
				JudgeStdToleranceNG(&(HTuple)tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width[i].ToTuple(), tInfo.tParam.bUseChecking, tol, &hvFlag);
				tOutput.tMerge.vecSideResult[nSideSN].hv_SpotInspectionFlagList.Append(hvFlag);
				if (hvFlag == NG)
				{
					tOutput.tMerge.hv_SpotInspectionFlag = NG;
				}
			}
		}


		//判断报警
		if (tOutput.tMerge.hv_SpotInspectionFlag == NG)
		{
			s_bContinueSpotInsp = TRUE;
			s_nNGCount++;
			if (s_nNGCount >= m_tParam.nSpotInspMaxNGCount)
			{
				s_bContinueSpotInsp = FALSE;
				s_nNGCount = 0;

				CString strExcp;
				strExcp.Format("点检结果连续%d次超出规格,请检查", m_tParam.nSpotInspMaxNGCount);
				/*if (i != tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width.Length() - 1)
				{
					strExcp.Format("相机%d点检结果超出规格(点检尺寸:%0.3fmm, 规格:%0.2f±%0.2fmm)，请检查!", hvSideSNList[i / 2].I(),
						tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width[i].D(), tInfo.tParam.vecChecking[i].dStandard,
						tInfo.tParam.vecChecking[i].dTolerance);
				}
				else
				{
					strExcp.Format("%s面点检间距超出规格(点检尺寸:%0.3fmm, 规格:%0.2f±%0.2fmm)，请检查!", nSideSN == 0 ? "正" : "反",
						tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width[i].D(), tInfo.tParam.vecChecking[i].dStandard,
						tInfo.tParam.vecChecking[i].dTolerance);
				}*/

				theGlobal.m_Log.Alarm(strExcp);
			}
		}
		else
		{
			s_bContinueSpotInsp = FALSE;
			s_nNGCount = 0;
		}
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("Judge_JudgeSpotInspect:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
	}

	s_hvTimeOld = s_hvTimeCurrent;
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
	if (m_tParam.bUseFirstArticleInsp) return TRUE;

	JudgeContinuousSizeNG(tOutput);
	JudgeContinuousDefectNG(tOutput);

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
	static BOOL s_bFirstArticleInspStart = TRUE;
	if (!m_tParam.bUseFirstArticleInsp) return TRUE;

	//tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	if (!tOutput.vecB[0].tMeasure.hv_HasCoat.I()) return FALSE;
	if (tOutput.vecB.size() > 1 && !tOutput.vecB[1].tMeasure.hv_HasCoat.I()) return FALSE;

	const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();

	static double s_dMeterCount = m_tParam.dFirstArticleInspMeter + dCurrentMeterCount;
	if (s_dMeterCount > m_tParam.dFirstArticleInspMeter + dCurrentMeterCount)
		s_dMeterCount = m_tParam.dFirstArticleInspMeter + dCurrentMeterCount;
	static BOOL s_bFirstArticleInspNG = FALSE;
	try
	{
		if (s_bFirstArticleInspStart)
		{
			s_dMeterCount = s_dMeterCount = m_tParam.dFirstArticleInspMeter + dCurrentMeterCount;
		}
		if (s_dMeterCount <= dCurrentMeterCount)
		{
			//判断首件数据
			m_tParam.bUseFirstArticleInsp = FALSE;
			s_bFirstArticleInspStart = TRUE;

			if (tOutput.tMerge.hv_MeasureFlag == NG)
			{
				s_bFirstArticleInspNG = TRUE;
			}

			if (s_bFirstArticleInspNG)
			{
				CString strLog;
				strLog.Format("首件已经完成，结果判定为NG");
				theGlobal.m_Log.Std(strLog);
				theGlobal.SetTipInfo(TIP_FirstArticleNG);
			}
			else
			{
				CString strLog;
				strLog.Format("首件已经完成，结果判定为OK");
				theGlobal.m_Log.Std(strLog);
				theGlobal.SetTipInfo(TIP_FirstArticleOK);
			}

			s_bFirstArticleInspNG = FALSE;
		}
		else 
		{
			s_bFirstArticleInspStart = FALSE;

			//判断首件数据
			if (tOutput.tMerge.hv_MeasureFlag == NG)
			{
				s_bFirstArticleInspNG = TRUE;
			}
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