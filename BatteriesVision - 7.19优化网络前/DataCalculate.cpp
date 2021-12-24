#include "StdAfx.h"
#include "DataCalculate.h"

CDataCalculate::CDataCalculate()
{
	for (int i = 0; i < sizeof(m_tData) / sizeof(m_tData[0]); i++)
	{
		m_tData[i].hvecData.Clear();
		m_tData[i].hvecSideCaValue.Clear();
		m_tData[i].hvecSideCpValue.Clear();
		m_tData[i].hvecSideCpkValue.Clear();
		m_tData[i].hvecSideMeanValue.Clear();
		m_tData[i].hvecSideSigmaValue.Clear();
		m_tData[i].hvecSideCaTotalValue.Clear();
		m_tData[i].hvecSideCpTotalValue.Clear();
		m_tData[i].hvecSideCpkTotalValue.Clear();
		m_tData[i].hvecSideMeanTotalValue.Clear();
		m_tData[i].hvecSideSigmaTotalValue.Clear();
	}
}

CDataCalculate::~CDataCalculate()
{

}

BOOL CDataCalculate::Init()
{


	ReadParam();
	InitData();
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	InitHTuple(TI_Foil, tPdInfo.tParam.vecFoilWidth, m_tLastData[0]);
	InitHTuple(TI_Foil, tPdInfo.tParam.vecFoilWidth, m_tLastData[1]);
	if (tPdInfo.bExistAT11)
	{
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecAT11Width, m_tLastData[0]);
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecAT11Width, m_tLastData[1]);
	}
	else
	{
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecBaseCoatWidth, m_tLastData[0]);
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecBaseCoatWidth, m_tLastData[1]);
	}
	InitHTuple(TI_Coat, tPdInfo.tParam.vecCoatWidth, m_tLastData[0]);
	InitHTuple(TI_Coat, tPdInfo.tParam.vecCoatWidth, m_tLastData[1]);
	if (tPdInfo.nCoatingType == TwoSide)
	{
		InitHTuple(TI_Misalignment, tPdInfo.tParam.vecMisalignment, m_tLastData[0]);
		InitHTuple(TI_Misalignment, tPdInfo.tParam.vecMisalignment, m_tLastData[1]);
	}
	return TRUE;
}

void CDataCalculate::InitData()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	InitHTuple(TI_Foil, tPdInfo.tParam.vecFoilWidth, m_tData[0]);
	InitHTuple(TI_Foil, tPdInfo.tParam.vecFoilWidth, m_tData[1]);
	if (tPdInfo.bExistAT11)
	{
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecAT11Width, m_tData[0]);
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecAT11Width, m_tData[1]);
	}
	else
	{
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecBaseCoatWidth, m_tData[0]);
		InitHTuple(TI_AT11BaseCoat, tPdInfo.tParam.vecBaseCoatWidth, m_tData[1]);
	}
	InitHTuple(TI_Coat, tPdInfo.tParam.vecCoatWidth, m_tData[0]);
	InitHTuple(TI_Coat, tPdInfo.tParam.vecCoatWidth, m_tData[1]);
	if (tPdInfo.nCoatingType == TwoSide)
	{
		InitHTuple(TI_Misalignment, tPdInfo.tParam.vecMisalignment, m_tData[0]);
		InitHTuple(TI_Misalignment, tPdInfo.tParam.vecMisalignment, m_tData[1]);
	}
}
void CDataCalculate::InitHTuple(eTypeIndex eType, ToleranceList toleranceList, tagData& tagDataList)
{
	for (int i = 0; i < toleranceList.size(); i++)
	{
		TupleGenConst(1, 0, &tagDataList.hvecData[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideCaValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideCpValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideMeanValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideCpkValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideSigmaValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideCaTotalValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideCpTotalValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideMeanTotalValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideCpkTotalValue[eType][i].T());
		TupleGenConst(1, 0, &tagDataList.hvecSideSigmaTotalValue[eType][i].T());
	}
}

BOOL CDataCalculate::ReadParam()
{
	m_tParam.bUse = GetIniInt("DataCalculate", "Use", FALSE);
	m_tParam.dSelectMeter = GetIniDouble("DataCalculate", "dSelectMeter", 500);
	m_tParam.dSemiAutoMeter = GetIniDouble("DataCalculate", "dSemiAutoMeter", 50);
	m_tParam.bUseStopPosDataFiltering = GetIniInt("DataCalculate", "UseStopPosDataFiltering", FALSE);
	return TRUE;
}

BOOL CDataCalculate::WriteParam()
{
	WriteIniInt("DataCalculate", "Use", m_tParam.bUse);
	WriteIniDouble("DataCalculate", "dSelectMeter", m_tParam.dSelectMeter);
	WriteIniDouble("DataCalculate", "dSemiAutoMeter", m_tParam.dSemiAutoMeter);
	WriteIniInt("DataCalculate", "UseStopPosDataFiltering", m_tParam.bUseStopPosDataFiltering);
	return TRUE;
}

void CDataCalculate::FinishCalculate()
{
	m_tLastData[0] = m_tData[0];
	m_tLastData[1] = m_tData[1];

	InitData();
}

HTuple CDataCalculate::GetLastCaValue(const int& nSideSN)
{
	if (nSideSN > theGlobal.m_Project.GetProjectConfig().nSideCount - 1) return HTuple();
	return m_tLastData[nSideSN].hvecSideCaValue[nSideSN].T();
}

HTuple CDataCalculate::GetLastCpValue(const int& nSideSN)
{
	if (nSideSN > theGlobal.m_Project.GetProjectConfig().nSideCount - 1) return HTuple();
	return m_tLastData[nSideSN].hvecSideCpValue[nSideSN].T();
}

HTuple CDataCalculate::GetLastCpkValue(const int& nSideSN)
{
	if (nSideSN > theGlobal.m_Project.GetProjectConfig().nSideCount - 1) return HTuple();
	return m_tLastData[nSideSN].hvecSideCpkValue[nSideSN].T();
}

HTuple CDataCalculate::GetLastMeanValue(const int& nSideSN)
{
	if (nSideSN > theGlobal.m_Project.GetProjectConfig().nSideCount - 1) return HTuple();
	return m_tLastData[nSideSN].hvecSideMeanValue[nSideSN].T();
}

HTuple CDataCalculate::GetLastSigmaValue(const int& nSideSN)
{
	if (nSideSN > theGlobal.m_Project.GetProjectConfig().nSideCount - 1) return HTuple();
	return m_tLastData[nSideSN].hvecSideSigmaValue[nSideSN].T();
}


/************************************
*@Method:    CalcMeasureQuotaByType
*@Access:    public
*@Returns:   void
*@Author: 	 Fowindy
*@Parameter: SingleSize singleSize
*@Parameter: ToleranceList toleranceList
*@Parameter: int typeIndex---0:极耳---1:AT11/凹版---2:涂布
*@Parameter: tagData & tagDataList
*@Parameter: int MAX_FRAMES
*@Created:   2021/03/15 19:48
*@Describe:
*************************************/
void CDataCalculate::CalcMeasureQuotaByType(HTuple hv_Width, ToleranceList toleranceList, int typeIndex, tagData &tagDataList, int MAX_FRAMES)
{
	for (int i = 0; i < hv_Width.Length(); i++)
	{
		if (hv_Width[i] == NO_DATA) continue;
		//if (hv_Width[i] == 0 && typeIndex!=TI_Misalignment) continue;
		HTuple hvFlag(OK);
		tagTolerance tTol = toleranceList[0];
		if(typeIndex == TI_Coat)
			tTol = toleranceList[0];
		else if (typeIndex == TI_Foil)
			tTol = toleranceList[i];
		else if(typeIndex == TI_AT11BaseCoat)
			tTol = toleranceList[0];
		double dUpperLimit = tTol.dStandard + abs(tTol.dTolerance);
		double dLowerLimit = tTol.dStandard - abs(tTol.dTolerance);
		//if (hv_Width[i].D() > dUpperLimit || hv_Width[i].D() < dLowerLimit) continue;
		tagDataList.hvecData[typeIndex][i].T().Append(hv_Width[i].D());

		//去掉最低和最高各2个值，需要至少8个值
		if (tagDataList.hvecData[typeIndex][i].T().Length() < 8) continue;
		//数量大于选择的数量，去掉头的值
		/*if (tagDataList.hvecData[typeIndex][i].T().Length() > MAX_FRAMES)
			TupleSelectRange(tagDataList.hvecData[typeIndex][i].T(), 1, tagDataList.hvecData[typeIndex][i].T().Length() - 1, &tagDataList.hvecData[typeIndex][i].T());*/

		//获取最近多米的数据
		HTuple hvData = tagDataList.hvecData[typeIndex][i].T();
		if (tagDataList.hvecData[typeIndex][i].T().Length() > MAX_FRAMES)
			TupleSelectRange(tagDataList.hvecData[typeIndex][i].T(), tagDataList.hvecData[typeIndex][i].T().Length() - 1 - MAX_FRAMES, tagDataList.hvecData[typeIndex][i].T().Length() - 1, &hvData);

		HTuple hv_TempIndex, hv_EffectiveIndex, hv_EffectiveData;
		HTuple hv_Mean, hv_Sigma, hv_Ca, hv_Cp, hv_Cpk;
		TupleSortIndex(hvData, &hv_TempIndex);
		TupleSelectRange(hv_TempIndex, 2, hvData.Length() - 2, &hv_EffectiveIndex);
		TupleSelect(hvData, hv_EffectiveIndex, &hv_EffectiveData);
		_CalCpk(hv_EffectiveData, dUpperLimit, dLowerLimit, &hv_Mean, &hv_Sigma, &hv_Ca, &hv_Cp, &hv_Cpk);

		tagDataList.hvecSideMeanValue[typeIndex][i].T() = hv_Mean;
		tagDataList.hvecSideCaValue[typeIndex][i].T() = hv_Ca;
		tagDataList.hvecSideCpValue[typeIndex][i].T() = hv_Cp;
		tagDataList.hvecSideCpkValue[typeIndex][i].T() = hv_Cpk;
		tagDataList.hvecSideSigmaValue[typeIndex][i].T() = hv_Sigma;

		TupleSortIndex(tagDataList.hvecData[typeIndex][i].T(), &hv_TempIndex);
		if (hv_TempIndex.Length() > 41)//前后40张数据不考虑
		//所有数据的总指标
		TupleSelectRange(hv_TempIndex, 20, hvData.Length() - 21, &hv_EffectiveIndex);
		TupleSelect(tagDataList.hvecData[typeIndex][i].T(), hv_EffectiveIndex, &hv_EffectiveData);
		_CalCpk(hv_EffectiveData, dUpperLimit, dLowerLimit, &hv_Mean, &hv_Sigma, &hv_Ca, &hv_Cp, &hv_Cpk);
		tagDataList.hvecSideMeanTotalValue[typeIndex][i].T() = hv_Mean;
		tagDataList.hvecSideCaTotalValue[typeIndex][i].T() = hv_Ca;
		tagDataList.hvecSideCpTotalValue[typeIndex][i].T() = hv_Cp;
		tagDataList.hvecSideCpkTotalValue[typeIndex][i].T() = hv_Cpk;
		tagDataList.hvecSideSigmaTotalValue[typeIndex][i].T() = hv_Sigma;
	}
}
void CDataCalculate::AddDryAndWet(const C_MergeWork& tOutput)
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//如果有无效元素，直接返回
	HTuple hv_Height, hv_Width;
	GetImageSize(tOutput.vecB[0].vecA[0].hoGrabImage, &hv_Width, &hv_Height);
	//int MAX_FRAMES = m_tParam.dSemiAutoMeter * 1000 / (theGlobal.GetResolutionList()[0].dRow * hv_Height.D());
	int MAX_FRAMES = 10;

	//进行插入操作
	int nIndex = 0;
	if (1 == tOutput.vecB[0].vecA[0].hvRunDirection.I() 
		&& 1 == tOutput.vecB[0].vecA[0].hvRunDirection[1].I() 
		&& 1 == tOutput.vecB[0].vecA[0].hvRunDirection[2].I())//正转
	{
		//正转代表是湿膜，从尾部开始插入 
		int nLength = m_tWetAndDry[nIndex].Wet.Length();
		m_tWetAndDry[nIndex].Wet.Insert(nLength, HTupleVector(tOutput.vecB[nIndex].tMeasure.tSize.hv_Width));
		if (nLength > MAX_FRAMES)
		{
			m_tWetAndDry[nIndex].Wet.Remove(0);
		}
	}
	else if (-1 == tOutput.vecB[0].vecA[0].hvRunDirection.I())//反转
	{
		//反转代表干膜，从头部插入
		m_tWetAndDry[nIndex].Dry.Insert(0, HTupleVector(tOutput.vecB[nIndex].tMeasure.tSize.hv_Width));
		int nLength = m_tWetAndDry[nIndex].Dry.Length();
		if (nLength > MAX_FRAMES)
		{
			m_tWetAndDry[nIndex].Dry.Remove(nLength - 1);
		}
	}
	//if (tPdInfo.nCoatingType == TwoSide)
	//{
	//	int nIndex = 0;
	//	if (1 == tOutput.vecB[0].vecA[0].hvRunDirection.I())//正转
	//	{
	//		int nLength = m_tWetAndDry[nIndex].Wet.Length();
	//		m_tWetAndDry[nIndex].Wet.Insert(nLength, HTupleVector(tOutput.vecB[nIndex].tMeasure.tSize.hv_Width));
	//		//m_tWetAndDry[nIndex].Wet[nLength].T() = tOutput.vecB[0].tMeasure.tSize.hv_Width;
	//		if (nLength > MAX_FRAMES)
	//		{
	//			m_tWetAndDry[nIndex].Wet.Remove(0);
	//		}
	//	}
	//	else if(-1 == tOutput.vecB[0].vecA[0].hvRunDirection.I())//反转
	//	{
	//		int nLength = m_tWetAndDry[nIndex].Dry.Length();
	//		m_tWetAndDry[nIndex].Dry.Insert(nLength, HTupleVector(tOutput.vecB[nIndex].tMeasure.tSize.hv_Width));
	//		//m_tWetAndDry[nIndex].Wet[nLength].T() = tOutput.vecB[0].tMeasure.tSize.hv_Width;
	//		if (nLength > MAX_FRAMES)
	//		{
	//			m_tWetAndDry[nIndex].Dry.Remove(0);
	//		}
	//	}
	//}
}
BOOL CDataCalculate::Calculate(const C_MergeWork& tOutput)
{
	AddDryAndWet(tOutput);
	if (!m_tParam.bUse) return TRUE;
	//设备停止或者没涂都不计入统计
	if (m_tParam.bUseStopPosDataFiltering)
	{
		if (1 != tOutput.vecB[0].vecA[0].hvRunDirection.I()
			|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[1].I()
			|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[2].I()) return TRUE;
	}
	
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	try
	{
		//先判断是不是有效数据
		if (tOutput.vecB[0].tMeasure.hv_HasCoat.I() == FALSE) return TRUE;
		if (tOutput.vecB.size() > 1 && tOutput.vecB[1].tMeasure.hv_HasCoat.I() == FALSE) return TRUE;
		if (tOutput.vecB[0].tMeasure.tSize.hv_Width.TupleFind(0).Length() > 0 && tOutput.vecB[0].tMeasure.tSize.hv_Width.TupleFind(0) != -1)return TRUE;
		/*if (tOutput.vecB[0].tMeasure.tSize.hv_Width.TupleFind(NO_DATA).Length() > 0 && tOutput.vecB[0].tMeasure.tSize.hv_Width.TupleFind(NO_DATA) != -1)return TRUE;*/
		if (tOutput.vecB.size() > 1 && tOutput.vecB[1].tMeasure.tSize.hv_Width.TupleFind(0).Length() > 0
			&& tOutput.vecB[1].tMeasure.tSize.hv_Width.TupleFind(0) != -1)return TRUE;
		/*if (tOutput.vecB.size() > 1 && tOutput.vecB[1].tMeasure.tSize.hv_Width.TupleFind(NO_DATA).Length() > 0
			&& tOutput.vecB[1].tMeasure.tSize.hv_Width.TupleFind(NO_DATA) != -1)return TRUE;*/

		HTuple hv_Height, hv_Width;
		GetImageSize(tOutput.vecB[0].vecA[0].hoGrabImage, &hv_Width, &hv_Height);
		int MAX_FRAMES = m_tParam.dSelectMeter * 1000 / (theGlobal.GetResolutionList()[0].dRow * hv_Height.D());
		SingleSize sigleSize;
		//正面数据计算
		if (tOutput.vecB[0].tMeasure.hv_HasCoat.I() == TRUE && tOutput.vecB[0].tMeasure.tCoat.hv_Width.D() != NO_DATA)
		{
			//正面极耳计算Cpk，sigma，均值
			CalcMeasureQuotaByType(tOutput.vecB[0].tMeasure.tFoil.hv_Width, tPdInfo.tParam.vecFoilWidth, TI_Foil, m_tData[0], MAX_FRAMES);
			//正面AT11或凹板计算Cpk，sigma，均值
			//正面AT11计算Cpk，sigma，均值
			if (tPdInfo.bExistAT11)
			{
				CalcMeasureQuotaByType(tOutput.vecB[0].tMeasure.tAT11.hv_Width, tPdInfo.tParam.vecAT11Width, TI_AT11BaseCoat, m_tData[0], MAX_FRAMES);
			}
			//正面凹板计算Cpk，sigma，均值
			else if (tPdInfo.bExistBaseCoat)
			{
				CalcMeasureQuotaByType(tOutput.vecB[0].tMeasure.tBaseCoat.hv_Width, tPdInfo.tParam.vecBaseCoatWidth, TI_AT11BaseCoat, m_tData[0], MAX_FRAMES);
			}
			//正面膜宽计算Cpk，sigma，均值
			CalcMeasureQuotaByType(tOutput.vecB[0].tMeasure.tCoat.hv_Width, tPdInfo.tParam.vecCoatWidth, TI_Coat, m_tData[0], MAX_FRAMES);
		}
		//反面数据计算
		if (tPdInfo.nCoatingType == TwoSide && tOutput.vecB[1].tMeasure.hv_HasCoat.I() == TRUE && tOutput.vecB[1].tMeasure.tCoat.hv_Width.D() != NO_DATA)
		{
			CalcMeasureQuotaByType(tOutput.vecB[1].tMeasure.tFoil.hv_Width, tPdInfo.tParam.vecFoilWidth, TI_Foil, m_tData[1], MAX_FRAMES);
			//反面AT11或凹板计算Cpk，sigma，均值
			//反面AT11计算Cpk，sigma，均值
			if (tPdInfo.bExistAT11)
			{
				CalcMeasureQuotaByType(tOutput.vecB[1].tMeasure.tAT11.hv_Width, tPdInfo.tParam.vecAT11Width, TI_AT11BaseCoat, m_tData[1], MAX_FRAMES);
			}
			//反面凹板计算Cpk，sigma，均值
			else if (tPdInfo.bExistBaseCoat)
			{
				CalcMeasureQuotaByType(tOutput.vecB[1].tMeasure.tBaseCoat.hv_Width, tPdInfo.tParam.vecBaseCoatWidth, TI_AT11BaseCoat, m_tData[1], MAX_FRAMES);
			}
			//反面涂布计算Cpk，sigma，均值
			CalcMeasureQuotaByType(tOutput.vecB[1].tMeasure.tCoat.hv_Width, tPdInfo.tParam.vecCoatWidth, TI_Coat, m_tData[1], MAX_FRAMES);
			//正反面错位计算Cpk，sigma，均值
			CalcMeasureQuotaByType(tOutput.tMerge.hv_Misalignment, tPdInfo.tParam.vecMisalignment, TI_Misalignment, m_tData[0], MAX_FRAMES);
		}
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("DataCalculate_Calculate: 计算数据异常 %s(%s)", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
		return FALSE;
	}
	return TRUE;
}

/************************************
*@Method:    _CalCpk
*@Access:    private
*@Returns:   void
*@Author: 	 Fowindy
*@Parameter: HTuple hv_Datas
*@Parameter: const double & dUpperLimit
*@Parameter: const double & dLowerLimit
*@Parameter: HTuple * hv_Mean
*@Parameter: HTuple * hv_Sigma
*@Parameter: HTuple * hv_Ca
*@Parameter: HTuple * hv_Cp
*@Parameter: HTuple * hv_Cpk
*@Created:   2021/03/10 13:33
*@Describe:	 计算CPK函数
*************************************/
void CDataCalculate::_CalCpk(HTuple hv_Datas, const double& dUpperLimit, const double& dLowerLimit, HTuple* hv_Mean, HTuple* hv_Sigma, HTuple* hv_Ca, HTuple* hv_Cp, HTuple* hv_Cpk)
{
	*hv_Mean = HTuple();
	*hv_Sigma = HTuple();
	*hv_Ca = HTuple();
	*hv_Cp = HTuple();
	*hv_Cpk = HTuple();

	TupleMean(hv_Datas, &(*hv_Mean));
	TupleDeviation(hv_Datas, &(*hv_Sigma));
	*hv_Cpk = MIN(dUpperLimit - (*hv_Mean).D(), (*hv_Mean).D() - dLowerLimit) / (3 * MAX(0.001, (*hv_Sigma).D()));
	*hv_Ca = abs((*hv_Mean).D() - (dUpperLimit + dLowerLimit) / 2) / ((dUpperLimit - dLowerLimit) / 2);
	*hv_Cp = (dUpperLimit - dLowerLimit) / 6 / (*hv_Sigma).D();
}


BOOL CDataCalculate::CalculateDataOffset(tagPDInfo& tPdInfo)
{
	tagData tData = m_tData[0];
	
	try
	{
		if (tData.hvecData[TI_Foil].T().Length() < 3) return FALSE;
		//所有极耳补偿
		for (int i = 0; i < tData.hvecData[TI_Foil].T().Length(); i++)
		{
			if (i > tPdInfo.tParam.vecFoilWidth.size() - 1) continue;
			HTuple hvData;
			TupleSelectRange(tData.hvecData[TI_Foil][i].T(), tData.hvecData[TI_Foil][i].Length() - 3, tData.hvecData[TI_Foil].Length() - 1, &hvData);
			TupleMean(hvData, &hvData);

			tPdInfo.tParam.vecFoilWidth[i].OffsetList[0] = tPdInfo.tParam.vecFoilWidth[0].dStandard - hvData.D();
		}

		if (tData.hvecData[TI_AT11BaseCoat].T().Length() < 3) return FALSE;
		if (tPdInfo.bExistAT11)
		{
			//所有AT11补偿
			for (int i = 0; i < tData.hvecData[TI_AT11BaseCoat].T().Length(); i++)
			{
				if (i > tPdInfo.tParam.vecAT11Width.size() - 1) continue;
				HTuple hvData;
				TupleSelectRange(tData.hvecData[TI_AT11BaseCoat][i].T(), tData.hvecData[TI_AT11BaseCoat][i].Length() - 3, tData.hvecData[TI_AT11BaseCoat].Length() - 1, &hvData);
				TupleMean(hvData, &hvData);

				tPdInfo.tParam.vecAT11Width[i].OffsetList[0] = tPdInfo.tParam.vecAT11Width[0].dStandard - hvData.D();
			}
		}
		else if (tPdInfo.bExistBaseCoat)
		{
			//所有凹版补偿
			for (int i = 0; i < tData.hvecData[TI_AT11BaseCoat].T().Length(); i++)
			{
				if (i > tPdInfo.tParam.vecBaseCoatWidth.size() - 1) continue;
				HTuple hvData;
				TupleSelectRange(tData.hvecData[TI_AT11BaseCoat][i].T(), tData.hvecData[TI_AT11BaseCoat][i].Length() - 3, tData.hvecData[TI_AT11BaseCoat].Length() - 1, &hvData);
				TupleMean(hvData, &hvData);

				tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[0] = tPdInfo.tParam.vecBaseCoatWidth[0].dStandard - hvData.D();
			}
		}

		if (tData.hvecData[TI_Coat].T().Length() < 3) return FALSE;
		//所有涂布补偿
		for (int i = 0; i < tData.hvecData[TI_Coat].T().Length(); i++)
		{
			if (i > tPdInfo.tParam.vecCoatWidth.size() - 1) continue;
			HTuple hvData;
			TupleSelectRange(tData.hvecData[TI_Coat][i].T(), tData.hvecData[TI_Coat][i].Length() - 3, tData.hvecData[TI_Coat].Length() - 1, &hvData);
			TupleMean(hvData, &hvData);

			tPdInfo.tParam.vecCoatWidth[i].OffsetList[0] = tPdInfo.tParam.vecCoatWidth[0].dStandard - hvData.D();
		}

		//B面
		if (tPdInfo.nCoatingType == TwoSide)
		{
			tagData tData = m_tData[1];
			if (tData.hvecData[TI_Foil].T().Length() < 3) return FALSE;
			//所有极耳补偿
			for (int i = 0; i < tData.hvecData[TI_Foil].T().Length(); i++)
			{
				if (i > tPdInfo.tParam.vecFoilWidth.size() - 1) continue;
				HTuple hvData;
				TupleSelectRange(tData.hvecData[TI_Foil][i].T(), tData.hvecData[TI_Foil][i].Length() - 3, tData.hvecData[TI_Foil].Length() - 1, &hvData);
				TupleMean(hvData, &hvData);

				if (i == 0 || i == tPdInfo.tParam.vecFoilWidth.size() - 1)
					tPdInfo.tParam.vecFoilWidth[i].OffsetList[1] = tPdInfo.tParam.vecFoilWidth[1].dStandard - hvData.D() / 2;
				else
					tPdInfo.tParam.vecFoilWidth[i].OffsetList[1] = tPdInfo.tParam.vecFoilWidth[1].dStandard - hvData.D();
			}

			if (tData.hvecData[TI_AT11BaseCoat].T().Length() < 3) return FALSE;
			if (tPdInfo.bExistAT11)
			{
				//所有AT11补偿
				for (int i = 0; i < tData.hvecData[TI_AT11BaseCoat].T().Length(); i++)
				{
					if (i > tPdInfo.tParam.vecAT11Width.size() - 1) continue;
					HTuple hvData;
					TupleSelectRange(tData.hvecData[TI_AT11BaseCoat][i].T(), tData.hvecData[TI_AT11BaseCoat][i].Length() - 3, tData.hvecData[TI_AT11BaseCoat].Length() - 1, &hvData);
					TupleMean(hvData, &hvData);

					tPdInfo.tParam.vecAT11Width[i].OffsetList[1] = tPdInfo.tParam.vecAT11Width[1].dStandard - hvData.D();
				}
			}
			else if (tPdInfo.bExistBaseCoat)
			{
				//所有凹版补偿
				for (int i = 0; i < tData.hvecData[TI_AT11BaseCoat].T().Length(); i++)
				{
					if (i > tPdInfo.tParam.vecBaseCoatWidth.size() - 1) continue;
					HTuple hvData;
					TupleSelectRange(tData.hvecData[TI_AT11BaseCoat][i].T(), tData.hvecData[TI_AT11BaseCoat][i].Length() - 3, tData.hvecData[TI_AT11BaseCoat].Length() - 1, &hvData);
					TupleMean(hvData, &hvData);

					tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[1] = tPdInfo.tParam.vecBaseCoatWidth[1].dStandard - hvData.D();
				}
			}

			if (tData.hvecData[TI_Coat].T().Length() < 3) return FALSE;
			//所有涂布补偿
			for (int i = 0; i < tData.hvecData[TI_Coat].T().Length(); i++)
			{
				if (i > tPdInfo.tParam.vecCoatWidth.size() - 1) continue;
				HTuple hvData;
				TupleSelectRange(tData.hvecData[TI_Coat][i].T(), tData.hvecData[TI_Coat][i].Length() - 3, tData.hvecData[TI_Coat].Length() - 1, &hvData);
				TupleMean(hvData, &hvData);

				tPdInfo.tParam.vecCoatWidth[i].OffsetList[1] = tPdInfo.tParam.vecCoatWidth[1].dStandard - hvData.D();
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("CDataCalculate_CalculateDataOffset: %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		return FALSE;
	}

	return TRUE;
}