#include "StdAfx.h"
#include "Calibration.h"

CCalibration::CCalibration()
{
	m_eCalType = CT_Stripe;
}

CCalibration::~CCalibration()
{
}

void CCalibration::Init(const MeasureInit& tInit)
{
	m_tInit = tInit;

	m_strParamFile.Format("%s//Config//Public//Calibration//%d", GetCurrentAppPath(), m_tInit.nSN);
	CreateMultilevelPath(m_strParamFile);
	m_strParamFile.Format("%s//Config//Public//Calibration//%d//Param.ini", GetCurrentAppPath(), m_tInit.nSN);
	m_strParamFile1.Format("%s//Config//Public//Calibration//%d//CameraParma.ini", GetCurrentAppPath(), m_tInit.nSN);
}

BOOL CCalibration::ReadParam()
{
	m_eCalType = (eCalType)GetPrivateProfileInt("Info", "eCalType", CT_Stripe, m_strParamFile);

	m_tCalParam.nImageIgnoreWidth = GetPrivateProfileInt("CalParam", "nImageIgnoreWidth", 1000, m_strParamFile);
	m_tCalParam.nThres = GetPrivateProfileInt("CalParam", "nThres", 128, m_strParamFile);    //128为默认值
	m_tCalParam.dUnitWidth = GetPrivateProfileDouble("CalParam", "dUnitWidth", 2, m_strParamFile);
	m_tCalParam.nMeasureThres = GetPrivateProfileInt("CalParam", "nMeasureThres", 50, m_strParamFile);
	m_tCalParam.dDisUpLimit = GetPrivateProfileDouble("CalParam", "dDisUpLimit", 1.1, m_strParamFile);
	m_tCalParam.dDisDownLimit = GetPrivateProfileDouble("CalParam", "dDisDownLimit", 0.9, m_strParamFile);
	//m_tJZ.dColRes = GetPrivateProfileDouble("CalibrationParam", "XResolution", 0.05, m_strParamFile1);//统一使用和系统及其他模块一样的配置文件和参数
	//m_tJZ.dRowRes = GetPrivateProfileDouble("CalibrationParam", "YResolution", 0.05, m_strParamFile1);//统一使用和系统及其他模块一样的配置文件和参数
	m_tStripeParam.hvJyDistance = GetPrivateProfileDouble("StripeParam", "hvJyDistance", 100, m_strParamFile);
	m_tStripeParam.dInternOffset = GetPrivateProfileDouble("StripeParam", "dInternOffset", 0, m_strParamFile);

	m_tCalParam.dRowResolution = GetPrivateProfileDouble("JiaoZhun", "RowResolution", 0.052, m_strParamFile);
	m_tCalParam.dColResolution = GetPrivateProfileDouble("JiaoZhun", "ColResolution", 0.052, m_strParamFile);
	m_tCalParam.dMinArea = GetPrivateProfileInt("JiaoZhun", "dMinArea", 5, m_strParamFile);
	m_tCalParam.nLowThresh = GetPrivateProfileInt("JiaoZhun", "nLowThresh", 5, m_strParamFile);
	m_tCalParam.nHighThresh = GetPrivateProfileInt("JiaoZhun", "nHighThresh", 80, m_strParamFile);
	m_tCalParam.nDefectCalType = GetPrivateProfileInt("JiaoZhun", "nDefectCalType", 1, m_strParamFile);
	m_tCalParam.nExtractType = GetPrivateProfileInt("JiaoZhun", "nExtractType", 0, m_strParamFile);

	if (m_eCalType == CT_Stripe)
	{
		try
		{
			if (m_tInit.hvCamSNList.Length() == 0)
			{
				CString strExcp;
				strExcp.Format("CCalibration%d_ReadParam: 相机配置序号为空,请检查", m_tInit.nSN);
				m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
			}
			CString strFile;
			m_vecResolution.clear();
			for (int i = 0; i < m_tInit.hvCamSNList.Length(); i++)
			{
				strFile.Format("%s//Config//Public//Calibration//%d//CameraParma.ini", GetCurrentAppPath(), m_tInit.hvCamSNList[i].I());
				Resolution tRs;
				tRs.dRowRes = GetPrivateProfileDouble("CalibrationParam", "YResolution", 0.05, strFile);
				tRs.dColRes = GetPrivateProfileDouble("CalibrationParam", "XResolution", 0.05, strFile);
				m_vecResolution.push_back(tRs);
				m_tStripeParam.vecKValue.push_back(GetPrivateProfileDouble("MeasureParam", "dKValue", 1, strFile));
			}

			strFile.Format("%s//Config//Public//Calibration//%d//hvPixelPos.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(ReadTuple(HTuple(strFile), &m_tStripeParam.hvPixelCol));
			strFile.Format("%s//Config//Public//Calibration//%d//hvRealPos.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(ReadTuple(HTuple(strFile), &m_tStripeParam.hvRealCol));
			strFile.Format("%s//Config//Public//Calibration//%d//hvResolution.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(ReadTuple(HTuple(strFile), &m_tStripeParam.hvRes));

			strFile.Format("%s//Config//Public//Calibration//%d//hvColumnOver.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(ReadTuple(HTuple(strFile), &m_tStripeParam.hvColumnOver));
			strFile.Format("%s//Config//Public//Calibration//%d//hvLeftRightColumnOver.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(ReadTuple(HTuple(strFile), &m_tStripeParam.hvLeftRightColumnOver));

			strFile.Format("%s//Config//Public//Calibration//%d//hvObjWidth.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(ReadTuple(HTuple(strFile), &m_tCalParam.hvObjWidth));
			strFile.Format("%s//Config//Public//Calibration//%d//hvObjHeight.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(ReadTuple(HTuple(strFile), &m_tCalParam.hvObjHeight));
		}
		catch (HException& e)
		{
			CString strExcp;
			strExcp.Format("CCalibration%d_ReadParam: 读取条纹标定参数异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		}
	}

	return TRUE;
}

BOOL CCalibration::WriteParam()
{
	WritePrivateProfileInt("Info", "eCalType", CT_Stripe, m_strParamFile);
	WritePrivateProfileInt("CalParam", "nImageIgnoreWidth", m_tCalParam.nImageIgnoreWidth, m_strParamFile);
	WritePrivateProfileInt("CalParam", "nThres", m_tCalParam.nThres, m_strParamFile);
	WritePrivateProfileDouble("CalParam", "dUnitWidth", m_tCalParam.dUnitWidth, m_strParamFile);
	WritePrivateProfileInt("CalParam", "nMeasureThres", m_tCalParam.nMeasureThres, m_strParamFile);
	WritePrivateProfileDouble("CalParam", "dDisUpLimit", m_tCalParam.dDisUpLimit, m_strParamFile);
	WritePrivateProfileDouble("CalParam", "dDisDownLimit", m_tCalParam.dDisDownLimit, m_strParamFile);
	//WritePrivateProfileDouble("CalibrationParam", "XResolution", m_tJZ.dColRes, m_strParamFile1);//统一使用和系统及其他模块一样的配置文件和参数
	//WritePrivateProfileDouble("CalibrationParam", "YResolution", m_tJZ.dRowRes, m_strParamFile1);//统一使用和系统及其他模块一样的配置文件和参数
	//WritePrivateProfileDouble("StripeParam", "dKValue", m_tStripeParam.dKValue, m_strParamFile);
	WritePrivateProfileDouble("StripeParam", "hvJyDistance", m_tStripeParam.hvJyDistance, m_strParamFile);
	WritePrivateProfileDouble("StripeParam", "dInternOffset", m_tStripeParam.dInternOffset, m_strParamFile);

	WritePrivateProfileDouble("JiaoZhun", "RowResolution", m_tCalParam.dRowResolution, m_strParamFile);
	WritePrivateProfileDouble("JiaoZhun", "ColResolution", m_tCalParam.dColResolution, m_strParamFile);
	WritePrivateProfileInt("JiaoZhun", "dMinArea", m_tCalParam.dMinArea, m_strParamFile);
	WritePrivateProfileInt("JiaoZhun", "nLowThresh", m_tCalParam.nLowThresh, m_strParamFile);
	WritePrivateProfileInt("JiaoZhun", "nHighThresh", m_tCalParam.nHighThresh, m_strParamFile);
	WritePrivateProfileInt("JiaoZhun", "nDefectCalType", m_tCalParam.nDefectCalType, m_strParamFile);
	WritePrivateProfileInt("JiaoZhun", "nExtractType", m_tCalParam.nExtractType, m_strParamFile);

	if (m_eCalType == CT_Stripe)
	{
		try
		{
			CString strFile;
			if (m_tInit.hvCamSNList.Length() != m_vecResolution.size())
			{
				CString strExcp;
				strExcp.Format("CCalibration%d_WriteParam: 相机数量和分辨率数量不相等，请检查");
				m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
			}
			for (int i = 0; i < m_tInit.hvCamSNList.Length(); i++)
			{
				strFile.Format("%s//Config//Public//Calibration//%d", GetCurrentAppPath(), m_tInit.hvCamSNList[i].I());
				CreateMultilevelPath(strFile);
				strFile.Format("%s//Config//Public//Calibration//%d//CameraParma.ini", GetCurrentAppPath(), m_tInit.hvCamSNList[i].I());
				WritePrivateProfileDouble("CalibrationParam", "YResolution", m_vecResolution[i].dRowRes, strFile);
				WritePrivateProfileDouble("CalibrationParam", "XResolution", m_vecResolution[i].dColRes, strFile);
				WritePrivateProfileDouble("MeasureParam", "dKValue", m_tStripeParam.vecKValue[i], strFile);
			}

			strFile.Format("%s//Config//Public//Calibration//%d//hvPixelPos.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(WriteTuple(m_tStripeParam.hvPixelCol, HTuple(strFile)));
			strFile.Format("%s//Config//Public//Calibration//%d//hvRealPos.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(WriteTuple(m_tStripeParam.hvRealCol, HTuple(strFile)));
			strFile.Format("%s//Config//Public//Calibration//%d//hvResolution.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(WriteTuple(m_tStripeParam.hvRes, HTuple(strFile)));

			strFile.Format("%s//Config//Public//Calibration//%d//hvColumnOver.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(WriteTuple(m_tStripeParam.hvColumnOver, HTuple(strFile)));
			strFile.Format("%s//Config//Public//Calibration//%d//hvLeftRightColumnOver.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(WriteTuple(m_tStripeParam.hvLeftRightColumnOver, HTuple(strFile)));

			strFile.Format("%s//Config//Public//Calibration//%d//hvObjWidth.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(WriteTuple(m_tCalParam.hvObjWidth, HTuple(strFile)));
			strFile.Format("%s//Config//Public//Calibration//%d//hvObjHeight.tup", GetCurrentAppPath(), m_tInit.nSN);
			HALCON_TRY(WriteTuple(m_tCalParam.hvObjHeight, HTuple(strFile)));
		}
		catch (HException& e)
		{
			CString strExcp;
			strExcp.Format("CCalibration%d_WriteParam: 写入条纹标定参数异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		}
	}

	return TRUE;
}

BOOL CCalibration::Calibrate(CalInput& tIn, CalOutput& tOut)
{
	HTuple hv_Length;
	TupleLength(m_tInit.hvCamSNList, &hv_Length);
	if (m_eCalType == CT_Stripe)
	{
		if (hv_Length == 1)
		{
			return StripeCalibration(tIn, tOut);
		}
		else if (hv_Length == 2)
		{
			return StripeDoubleCamCalibration(tIn, tOut);
		}
	}
	return FALSE;
}

BOOL CCalibration::CalDistance(CalDisPPInput& pIn, CalDisPPOutput& pOut)
{
	HTuple hv_Length;
	TupleLength(m_tInit.hvCamSNList, &hv_Length);
	if (m_eCalType == CT_Stripe)
	{
		if (m_tInit.hvCamSNList.Length() == 1)
			return CalDistancePP(pIn, pOut);
		else if (m_tInit.hvCamSNList.Length() == 2)
			return CalDoubleCamDistancePP(pIn, pOut);
	}
	return FALSE;
}

BOOL CCalibration::StripeCalibration(CalInput& tIn, CalOutput& tOut)
{
	HObject  ho_Region, ho_Rectangle, ho_ImageReduced, Cross;
	HObject  ho_Rectangle1, ho_RegionLines, hoRegionRec, hoReduce, ho_ConnectedRegions, ho_SelectedRegions, ho_RegionUnion;
	// Local control variables
	HTuple  hv_Xrow, hv_Indics1, hv_Indics2, hv_Width;
	HTuple  hv_Height, hv_Row1, hv_Column1;
	HTuple  hv_Row2, hv_Column2, hv_Area, hv_Row, hv_Column;
	HTuple  hv_MeasureHandle, hv_RowEdge, hv_ColumnEdge, hv_ColumnEdge2, hv_Amplitude;
	HTuple  hv_Distance, hv_Length, hv_i, hv_PixtoDis;
	HTuple  hv_IndexStart, hv_average;
	HTuple  hv_IndexTest, hv_total, hv_ImageWidth, hv_ImageHeight;
	HTuple  hv_totalresolution, hv_ColumnEdgeStart, hv_ColumnEdgeEnd, hv_RealLength, hv_Width1, hv_WidthMedian;

	try
	{
		GetImageSize(tIn.hoImage, &hv_ImageWidth, &hv_ImageHeight);
		GenEmptyObj(&tOut.hoDisp);

		HTuple hvROIRow0, hvROICol0, hvROIRow1, hvROICol1;
		SmallestRectangle1(tIn.hoROI, &hvROIRow0, &hvROICol0, &hvROIRow1, &hvROICol1);
		ReduceDomain(tIn.hoImage, tIn.hoROI, &hoReduce);
		CropDomain(hoReduce, &hoReduce);
		VarThreshold(hoReduce, &ho_Region, min(hvROICol1 - hvROICol0 - 1, 200), min(hvROIRow1 - hvROIRow0 - 1, 20), 0.5, m_tCalParam.nThres, "dark");
		ClosingRectangle1(ho_Region, &ho_Region, 5, 1);
		OpeningRectangle1(ho_Region, &ho_Region, 1, 20);
		//BinaryThreshold(hoReduce, &ho_Region, "max_separability", "dark", &HTuple());
		//AutoThreshold(hoReduce, &ho_Region, m_tCalParam.nThres);
		MoveRegion(ho_Region, &ho_Region, hvROIRow0, hvROICol0);

		Connection(ho_Region, &ho_ConnectedRegions);
		//粗筛选，把大致的标定黑条筛选出来
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "height", "and", 20, (hvROIRow1 - hvROIRow0) + 1);
		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "rectangularity", "and", 0.85, 1);

		//筛选所有细黑条
		RegionFeatures(ho_SelectedRegions, "width", &hv_Width1);
		TupleMedian(hv_Width1, &hv_WidthMedian);
		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "width", "and", hv_WidthMedian * m_tCalParam.dDisDownLimit, hv_WidthMedian * m_tCalParam.dDisUpLimit);
		Union1(ho_SelectedRegions, &ho_RegionUnion);
		Connection(ho_RegionUnion, &ho_ConnectedRegions);
		SortRegion(ho_ConnectedRegions, &ho_ConnectedRegions, "upper_left", "true", "column");

		SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		//所有细黑条左边的点
		hv_ColumnEdge.Clear();
		for (int i = 0; i < hv_Column1.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2((hv_Row1[i] + hv_Row2[i]) / 2, hv_Column1[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "negative", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge.Append(hvColEdg);
		}
		//所有细黑条右边的点
		hv_ColumnEdge2.Clear();
		for (int i = 0; i < hv_Column2.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2((hv_Row1[i] + hv_Row2[i]) / 2, hv_Column2[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "positive", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge2.Append(hvColEdg);
		}
		if (hv_ColumnEdge.Length() != hv_ColumnEdge2.Length())
		{
			CString strExcp;
			strExcp.Format("搜索到的黑条左右两边点数量不一致，请检查");
			return FALSE;
		}

		//细黑条左右两边求中心点
		HTuple hvSum, hvCenterCol;
		TupleAdd(hv_ColumnEdge, hv_ColumnEdge2, &hvSum);
		TupleDiv(hvSum, 2, &hvCenterCol);

		//判断找到的区间点是否均匀
		HTuple hvCol00, hvCol11, hvPixWidth;
		TupleRemove(hvCenterCol, 0, &hvCol00);
		TupleRemove(hvCenterCol, hvCenterCol.Length() - 1, &hvCol11);
		TupleSub(hvCol00, hvCol11, &hvPixWidth);
		if (hvPixWidth.TupleMax() / hvPixWidth.TupleMin() > 1.5)
		{
			CString strExcp;
			strExcp.Format("标定板区间点不均匀");
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
			return FALSE;
		}

		TupleGenSequence(0, (hvCenterCol.Length() - 1) * m_tCalParam.dUnitWidth, m_tCalParam.dUnitWidth, &m_tStripeParam.hvRealCol);
		m_tStripeParam.hvPixelCol = hvCenterCol;
		TupleDiv(m_tCalParam.dUnitWidth, hvPixWidth, &m_tStripeParam.hvRes);

		m_vecResolution[0].dColRes = (hvPixWidth.Length() * m_tCalParam.dUnitWidth) / hvPixWidth.TupleSum();//修改为分辨率结构体

		//拟合曲线
		HTuple hvX;
		CPolyFit::tagFitResult tRes;
		TupleGenSequence(0, m_tStripeParam.hvRes.Length() - 1, 1, &hvX);
		g_pAlgorithm->m_PolyFit.BinomialFit(hvX, m_tStripeParam.hvRes, tRes);
		tOut.hvCoefficients = tRes.hvCoefficients;
		tOut.hvR2 = tRes.dRSquare;
		tOut.hvResFit = tRes.hvFitY;
		tOut.hvRes = m_tStripeParam.hvRes;
		tOut.hvPixelCol = hvCenterCol;

		//偏差百分比
		HTuple hvResDiff, hvRes1, hvRes2, hvDiffPercent;
		TupleRemove(tOut.hvRes, 0, &hvRes1);
		TupleRemove(tOut.hvRes, tOut.hvRes.Length() - 1, &hvRes2);
		TupleSub(hvRes1, hvRes2, &hvResDiff);
		TupleDiv(hvResDiff, hvRes1, &hvDiffPercent);
		tOut.hvResDiffPercent = hvDiffPercent * 100;

		HTuple hvRows;
		TupleGenConst(hvCenterCol.Length(), hv_ImageHeight / 2, &hvRows);
		GenCrossContourXld(&Cross, (hv_Row1 + hv_Row2) / 2, hvCenterCol, 30, 0.785398);
		ConcatObj(tOut.hoDisp, Cross, &tOut.hoDisp);
		GenRegionLine(&ho_RegionLines, (hv_Row1 + hv_Row2) / 2 - 20, hvCenterCol, (hv_Row1 + hv_Row2) / 2 + 20, hvCenterCol);
		ConcatObj(tOut.hoDisp, ho_RegionLines, &tOut.hoDisp);
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CCalibration_StripeCalibration: 标定过程异常 %s [%s]", e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}

	return TRUE;
}

BOOL CCalibration::CalDistancePP(CalDisPPInput& pIn, CalDisPPOutput& pOut)
{
	if (pIn.hvCol1 == NO_DATA || pIn.hvCol2 == NO_DATA)
	{
		pOut.Dist = NO_DATA;
		return FALSE;
	}

	//计算的是不是在标定的有效范围内
	if ((pIn.hvCol1 > m_tStripeTmpParam.hvPixelCol.TupleMax() || pIn.hvCol1 < m_tStripeTmpParam.hvPixelCol.TupleMin()) ||
		(pIn.hvCol2 > m_tStripeTmpParam.hvPixelCol.TupleMax() || pIn.hvCol2 < m_tStripeTmpParam.hvPixelCol.TupleMin()))
	{
		CString strExcp;
		strExcp.Format("CCalibration_CalDoubleCamDistancePP: 算法%d超出像素校准区域", m_tInit.nSN);
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		pOut.Dist = NO_DATA;
		return FALSE;
	}

	HTuple hvGreater, hvCol1, hvCol2, hvColReal1, hvColReal2;
	//保证为左为小坐标，右为大的坐标
	hvCol1 = min(pIn.hvCol1.D(), pIn.hvCol2.D());
	hvCol2 = max(pIn.hvCol1.D(), pIn.hvCol2.D());
	CalPoint(hvCol1, hvColReal1);
	CalPoint(hvCol2, hvColReal2);
	pOut.Dist = abs(hvColReal2.D() - hvColReal1.D());

	//TupleGreaterEqualElem(m_tStripeParam.hvPixelCol, hvCol1, &hvGreater);
	//TupleFindFirst(hvGreater, 1, &hv_Indics1);
	//TupleGreaterEqualElem(m_tStripeParam.hvPixelCol, hvCol2, &hvGreater);
	//TupleFindFirst(hvGreater, 1, &hv_Indics2);

	//hv_DistanceWorld = (m_tStripeParam.hvRealCol[hv_Indics2].D() - m_tStripeParam.hvRealCol[hv_Indics1].D())
	//	+ ((m_tStripeParam.hvPixelCol[hv_Indics1].D() - hvCol1.D())*m_tStripeParam.hvRes[hv_Indics1 - 1].D())
	//	- ((m_tStripeParam.hvPixelCol[hv_Indics2].D() - hvCol2.D())*m_tStripeParam.hvRes[hv_Indics2 - 1].D());
	//pOut.Dist = abs(hv_DistanceWorld.D() * m_tStripeParam.vecKValue[0]);//2021.2.13 增加k值补偿

	return TRUE;
}

BOOL CCalibration::CalDistanceLL(CalDisLLInput& tIn, CalDisLLOutput& tOut)
{
	double dResY = m_vecResolution[0].dRowRes;

	try
	{
		if (tIn.tLine1.hv_StartColumn < m_tStripeTmpParam.hvPixelCol.TupleMin()
			|| tIn.tLine1.hv_StartColumn > m_tStripeTmpParam.hvPixelCol.TupleMax()
			|| tIn.tLine2.hv_StartColumn < m_tStripeTmpParam.hvPixelCol.TupleMin()
			|| tIn.tLine2.hv_StartColumn > m_tStripeTmpParam.hvPixelCol.TupleMax())
		{
			tOut.Dist = NO_DATA;
			return TRUE;
		}
		HTuple hvStartCol1,hvEndCol1,hvStartRow1,hvEndRow1,hvStartCol2,hvEndCol2,hvStartRow2,hvEndRow2;
		if (m_tInit.hvCamSNList.Length() == 2 && m_tStripeTmpParam.hvLeftRightColumnOver.Length() == 2)
		{
			if ((tIn.tLine1.hv_StartColumn < m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin().D()
				|| tIn.tLine1.hv_StartColumn > m_tStripeTmpParam.hvLeftRightColumnOver.TupleMax().D())
				&& (tIn.tLine2.hv_StartColumn < m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin().D()
					|| tIn.tLine2.hv_StartColumn > m_tStripeTmpParam.hvLeftRightColumnOver.TupleMax().D()))
			{
				//两条线都在标定有效区域内的情况
				//计算Line1物理坐标
				CalPoint(tIn.tLine1.hv_StartColumn, hvStartCol1);
				CalPoint(tIn.tLine1.hv_EndColumn, hvEndCol1);
				hvStartRow1 = tIn.tLine1.hv_StartRow.D() * dResY;
				hvEndRow1 = tIn.tLine1.hv_EndRow.D() * dResY;
				//计算Line2物理坐标
				CalPoint(tIn.tLine2.hv_StartColumn, hvStartCol2);
				CalPoint(tIn.tLine2.hv_EndColumn, hvEndCol2);
				hvStartRow2 = tIn.tLine2.hv_StartRow.D() * dResY;
				hvEndRow2 = tIn.tLine2.hv_EndRow.D() * dResY;

				//计算两条线之间的距离
				HTuple hvCenterX, hvCenterY;
				hvCenterX = (hvStartCol1 + hvEndCol1) / 2;
				hvCenterY = (hvStartRow1 + hvEndRow1) / 2;

				DistancePl(hvCenterY, hvCenterX, hvStartRow2, hvStartCol2, hvEndRow2, hvEndCol2, &tOut.Dist);

				if((tIn.tLine1.hv_StartColumn < m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin().D() 
					&& tIn.tLine2.hv_StartColumn > m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin().D())
					|| (tIn.tLine1.hv_StartColumn > m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin().D()
						&& tIn.tLine2.hv_StartColumn < m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin().D()))
				{
					tOut.Dist += m_tStripeTmpParam.dInternOffset;
				}
			}
			else
			{
				//两条边至少有一条在中间非有效标定区域内
				//计算Line1物理坐标
				CalPoint(tIn.tLine1.hv_StartColumn, hvStartCol1);
				CalPoint(tIn.tLine1.hv_EndColumn, hvEndCol1);
				hvStartRow1 = tIn.tLine1.hv_StartRow.D() * dResY;
				hvEndRow1 = tIn.tLine1.hv_EndRow.D() * dResY;
				//计算Line2物理坐标
				CalPoint(tIn.tLine2.hv_StartColumn, hvStartCol2);
				CalPoint(tIn.tLine2.hv_EndColumn, hvEndCol2);
				hvStartRow2 = tIn.tLine2.hv_StartRow.D() * dResY;
				hvEndRow2 = tIn.tLine2.hv_EndRow.D() * dResY;

				//计算两条线之间的距离
				HTuple hvCenterX, hvCenterY;
				hvCenterX = (hvStartCol1 + hvEndCol1) / 2;
				hvCenterY = (hvStartRow1 + hvEndRow1) / 2;

				DistancePl(hvCenterY, hvCenterX, hvStartRow2, hvStartCol2, hvEndRow2, hvEndCol2, &tOut.Dist);
			}
		}
		else if (m_tInit.hvCamSNList.Length() == 1)
		{
			//计算Line1物理坐标
			CalPoint(tIn.tLine1.hv_StartColumn, hvStartCol1);
			CalPoint(tIn.tLine1.hv_EndColumn, hvEndCol1);
			hvStartRow1 = tIn.tLine1.hv_StartRow.D() * dResY;
			hvEndRow1 = tIn.tLine1.hv_EndRow.D() * dResY;

			//计算Line2物理坐标
			CalPoint(tIn.tLine2.hv_StartColumn, hvStartCol2);
			CalPoint(tIn.tLine2.hv_EndColumn, hvEndCol2);
			hvStartRow2 = tIn.tLine2.hv_StartRow.D() * dResY;
			hvEndRow2 = tIn.tLine2.hv_EndRow.D() * dResY;

			//计算两条线之间的距离
			HTuple hvCenterX, hvCenterY;
			hvCenterX = (hvStartCol1 + hvEndCol1) / 2;
			hvCenterY = (hvStartRow1 + hvEndRow1) / 2;

			DistancePl(hvCenterY, hvCenterX, hvStartRow2, hvStartCol2, hvEndRow2, hvEndCol2, &tOut.Dist);
		}
		else
		{
			tOut.Dist = NO_DATA;
		}
	}
	catch(HException&e)
	{
		tOut.Dist = NO_DATA;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CCalibration::CalStripeTmpParam()
{
	m_tStripeTmpParam = m_tStripeParam;

	try
	{
		//双相机根据k值重新计算临时标定参数
		if (m_tInit.hvCamSNList.Length() == 2 && m_tStripeTmpParam.hvColumnOver.Length() == 2)
		{
			HTuple hvIncLeftMax, hvIncRightMin, hvIncLeftCircle;
			//左相机标定区域最右侧坐标
			TupleFind(m_tStripeTmpParam.hvPixelCol, m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin(), &hvIncLeftMax);
			TupleFind(m_tStripeTmpParam.hvPixelCol, m_tStripeTmpParam.hvLeftRightColumnOver.TupleMax(), &hvIncRightMin);
			//左相机屏蔽区域序号
			TupleFind(m_tStripeTmpParam.hvPixelCol, m_tStripeTmpParam.hvColumnOver.TupleMin(), &hvIncLeftCircle);

			HTuple hvTmp;
			//矫正左相机像素分辨率,左相机乘以k0,右相机乘以k1	
			m_tStripeTmpParam.hvRes.Clear();
			TupleSelectRange(m_tStripeParam.hvRes, 0, hvIncLeftMax - 1, &hvTmp);
			m_tStripeTmpParam.hvRes.Append(hvTmp*m_tStripeParam.vecKValue[0]);
			TupleSelectRange(m_tStripeParam.hvRes, hvIncLeftMax, hvIncRightMin - 1, &hvTmp);
			m_tStripeTmpParam.hvRes.Append(hvTmp*(m_tStripeParam.vecKValue[0] + m_tStripeTmpParam.vecKValue[1]) / 2);
			TupleSelectRange(m_tStripeParam.hvRes, hvIncRightMin, m_tStripeParam.hvRes.Length() - 1, &hvTmp);
			m_tStripeTmpParam.hvRes.Append(hvTmp*m_tStripeTmpParam.vecKValue[1]);
			//矫正左相机物理坐标,右相机与左坐标差值相减再乘以k值，加上左圆点矫正物理坐标
			HTuple hvLeftCircleRealCol;
			hvLeftCircleRealCol = m_tStripeParam.hvRealCol[hvIncLeftCircle].D();
			m_tStripeTmpParam.hvRealCol.Clear();
			TupleSelectRange(m_tStripeParam.hvRealCol, 0, hvIncLeftMax, &hvTmp);
			m_tStripeTmpParam.hvRealCol.Append(hvTmp*m_tStripeParam.vecKValue[0]);
			TupleSelectRange(m_tStripeParam.hvRealCol, hvIncRightMin, m_tStripeParam.hvRealCol.Length() - 1, &hvTmp);
			HTuple hvRightRealCol = (hvTmp - hvLeftCircleRealCol) * m_tStripeParam.vecKValue[1] + hvLeftCircleRealCol*m_tStripeParam.vecKValue[0];
			m_tStripeTmpParam.hvRealCol.Append(hvRightRealCol);
		}
		else if (m_tInit.hvCamSNList.Length() == 1)
		{
			//矫正相机像素分辨率,乘以k0
			TupleMult(m_tStripeTmpParam.hvRes, m_tStripeParam.vecKValue[0], &m_tStripeTmpParam.hvRes);
			//矫正相机物理坐标，乘以k0
			TupleMult(m_tStripeTmpParam.hvRealCol, m_tStripeParam.vecKValue[0], &m_tStripeTmpParam.hvRealCol);
		}
	}
	catch (HException &e)
	{
		m_tStripeTmpParam = m_tStripeParam;
		return FALSE;
	}

	return TRUE;
}

BOOL CCalibration::CalPoint(HTuple& hvPtColPix, HTuple& hvPtColReal)
{
	hvPtColReal = NO_DATA;
	try
	{
		if (m_tInit.hvCamSNList.Length() == 2 && hvPtColPix.Length() != 0 && m_tStripeTmpParam.hvLeftRightColumnOver.Length() == 2)
		{
			if (hvPtColPix >= m_tStripeTmpParam.hvPixelCol.TupleMax() || hvPtColPix <= m_tStripeTmpParam.hvPixelCol.TupleMin()) return FALSE;
			if (hvPtColPix > m_tStripeTmpParam.hvLeftRightColumnOver.TupleMin() && hvPtColPix < m_tStripeTmpParam.hvLeftRightColumnOver.TupleMax()) return FALSE;
			//双相机的时候
			HTuple hvGreater, hvInc;

			TupleGreaterEqualElem(m_tStripeTmpParam.hvPixelCol, hvPtColPix, &hvGreater);
			TupleFindFirst(hvGreater, 1, &hvInc);
			if (hvInc.Length() == 0 || hvInc == -1) return FALSE;

			hvPtColReal = m_tStripeTmpParam.hvRealCol[hvInc] - (m_tStripeTmpParam.hvPixelCol[hvInc] - hvPtColPix)*m_tStripeTmpParam.hvRes[hvInc - 1];

			return TRUE;
		}
		else if (m_tInit.hvCamSNList.Length() == 1)
		{
			if (hvPtColPix >= m_tStripeTmpParam.hvPixelCol.TupleMax() || hvPtColPix <= m_tStripeTmpParam.hvPixelCol.TupleMin()) return FALSE;
			//单相机的时候
			HTuple hvTmp, hvGreater, hvInc;
			TupleGreaterEqualElem(m_tStripeTmpParam.hvPixelCol, hvPtColPix, &hvGreater);
			TupleFindFirst(hvGreater, 1, &hvInc);
			if (hvInc.Length() == 0 || hvInc == -1) return FALSE;

			hvPtColReal = m_tStripeTmpParam.hvRealCol[hvInc] - (m_tStripeTmpParam.hvPixelCol[hvInc] - hvPtColPix)*m_tStripeTmpParam.hvRes[hvInc - 1];

			return TRUE;
		}
	}
	catch (HException&)
	{

	}
	return FALSE;
}

BOOL CCalibration::CalPointToPixel(HTuple& hvPtColReal, HTuple& hvPtColPix)
{
	hvPtColPix = -1;
	try
	{
		if (m_tInit.hvCamSNList.Length() == 2 && hvPtColReal.Length() != 0 && m_tStripeTmpParam.hvColumnOver.Length() == 2)
		{
			if (hvPtColReal >= m_tStripeTmpParam.hvRealCol.TupleMax() || hvPtColReal <= m_tStripeTmpParam.hvRealCol.TupleMin()) return FALSE;
			//双相机的时候
			HTuple hvGreater, hvInc, hvInc2;

			//从左向右寻找第一个大于输入物理坐标对应的像素坐标
			TupleGreaterEqualElem(m_tStripeTmpParam.hvRealCol, hvPtColReal, &hvGreater);
			TupleFindFirst(hvGreater, 1, &hvInc);
			if (hvInc.Length() == 0 || hvInc == -1) return FALSE;
			hvPtColPix = m_tStripeTmpParam.hvPixelCol[hvInc] - (m_tStripeTmpParam.hvRealCol[hvInc] - hvPtColReal) / m_tStripeTmpParam.hvRes[hvInc - 1];
			//从右向左寻找第一个大于输入物理坐标对应的像素坐标
			TupleFindLast(hvGreater, 0, &hvInc2);
			if (hvInc2.Length() == 0 || hvInc2 == -1 || (hvInc2 + 1 == hvInc)) return TRUE;
			HTuple hvColPix2 = m_tStripeTmpParam.hvPixelCol[hvInc2 + 1] - (m_tStripeTmpParam.hvRealCol[hvInc2 + 1] - hvPtColReal) / m_tStripeTmpParam.hvRes[hvInc2];
			hvPtColPix.Append(hvColPix2);

			return TRUE;
		}
		else if (m_tInit.hvCamSNList.Length() == 1)
		{
			if (hvPtColReal >= m_tStripeTmpParam.hvPixelCol.TupleMax() || hvPtColReal <= m_tStripeTmpParam.hvPixelCol.TupleMin()) return FALSE;
			//单相机的时候
			HTuple hvTmp, hvGreater, hvInc;
			TupleGreaterEqualElem(m_tStripeTmpParam.hvRealCol, hvPtColReal, &hvGreater);
			TupleFindFirst(hvGreater, 1, &hvInc);
			if (hvInc.Length() == 0 || hvInc == -1) return FALSE;

			hvPtColPix = m_tStripeTmpParam.hvPixelCol[hvInc] - (m_tStripeTmpParam.hvRealCol[hvInc] - hvPtColReal) / m_tStripeTmpParam.hvRes[hvInc - 1];

			return TRUE;
		}
	}
	catch (HException&)
	{

	}
	return FALSE;
}

//BOOL CCalibration::StripeDoubleCamCalibration(CalInput& tIn, CalOutput& tOut)
//{
//	HObject  ho_Region, ho_Rectangle, ho_ImageReduced, Cross;
//	HObject  ho_Rectangle1, ho_RegionLines, hoRegionRec, hoReduce;
//	HObject  ho_RegionUnion, ho_ConnectedRegions, ho_SelectedRegions, hoSelectedRegions1, ho_BigStripe;
//	// Local control variables
//	HTuple  hv_DistanceOver, hv_ColumnOver;
//	HTuple  hv_IndexStart1, hv_IndexOver;
//	HTuple  hv_Xrow, hv_Indics1, hv_Indics2, hv_Width;
//	HTuple  hv_Height, hv_Row1, hv_Column1;
//	HTuple  hv_Row2, hv_Column2, hv_Area, hv_Row, hv_Column;
//	HTuple  hv_MeasureHandle, hv_RowEdge, hv_ColumnEdge, hv_Amplitude, hv_ColumnEdge2, hv_ColumnBigStripeEdge, hv_ColumnBigStripeEdge2;
//	HTuple  hv_Distance, hv_Length, hv_i, hv_ii, hv_j, hv_iii, hv_PixtoDis;
//	HTuple  hv_IndexStart, hv_average;
//	HTuple  hv_IndexTest, hv_total, hv_LengthOver;
//	HTuple  hv_totalresolution, hv_ColumnEdgeStart, hv_ColumnEdgeEnd, hv_RealLength;
//	HTuple	hv_ImageWidth, hv_ImageHeight, hv_Width1, hv_Max1, hv_BigStripeCol, hv_BigStripeCol2, hv_WidthMedian;
//
//	try
//	{
//		GetImageSize(tIn.hoImage, &hv_ImageWidth, &hv_ImageHeight);
//		GenEmptyObj(&tOut.hoDisp);
//
//		hv_Xrow = HTuple();
//		hv_DistanceOver = HTuple();
//		hv_ColumnOver = HTuple();
//		hv_IndexOver = HTuple();
//		hv_Indics1 = 0;
//		hv_Indics2 = 0;
//		hv_total = 0;
//		hv_average = 0;
//
//		//Threshold(tIn.hoImage, &ho_Region, m_tCalParam.nThres, 255);
//		GenRectangle1(&hoRegionRec, 0, max(m_tCalParam.nImageIgnoreWidth, 0), hv_ImageHeight - 1, min(hv_ImageWidth - m_tCalParam.nImageIgnoreWidth, hv_ImageWidth - 1));
//		ReduceDomain(tIn.hoImage, hoRegionRec, &hoReduce);
//		VarThreshold(hoReduce, &ho_Region, min(hv_ImageWidth, 300), min(hv_ImageHeight, 300), 0.2, m_tCalParam.nThres, "dark");
//		ClosingRectangle1(ho_Region, &ho_Region, 5, 1);
//		OpeningRectangle1(ho_Region, &ho_Region, 1, 20);
//		Connection(ho_Region, &ho_ConnectedRegions);
//		//粗筛选，把大致的标定黑条筛选出来
//		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "width", "and", 15, 900);
//		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "height", "and", hv_ImageHeight - 5, hv_ImageHeight);
//		//筛选判断2个黑粗条，判断是不是有效的标定图像，图像范围限定为中间的一半
//		SelectShape(ho_SelectedRegions, &hoSelectedRegions1, "column", "and", hv_ImageWidth / 4, hv_ImageWidth / 4 * 3);
//		RegionFeatures(hoSelectedRegions1, "width", &hv_Width1);
//		TupleMax(hv_Width1, &hv_Max1);
//		SelectShape(hoSelectedRegions1, &hoSelectedRegions1, "width", "and", hv_Max1 * m_tCalParam.dDisDownLimit, hv_Max1 * m_tCalParam.dDisUpLimit);
//		if (hoSelectedRegions1.CountObj() != 2)
//		{
//			CString strExcp;
//			strExcp.Format("未发现2个有效的粗条，请检查标定板");
//			return FALSE;
//		}
//		//获取黑粗条的左坐标
//		SortRegion(hoSelectedRegions1, &ho_BigStripe, "upper_left", "true", "column");
//		RegionFeatures(ho_BigStripe, "column1", &hv_BigStripeCol);
//		//获取黑粗条的右坐标
//		RegionFeatures(ho_BigStripe, "column2", &hv_BigStripeCol2);
//
//		//筛选所有细黑条
//		RegionFeatures(ho_SelectedRegions, "width", &hv_Width1);
//		TupleMedian(hv_Width1, &hv_WidthMedian);
//		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "width", "and", hv_WidthMedian * m_tCalParam.dDisDownLimit, hv_WidthMedian * m_tCalParam.dDisUpLimit);
//		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, HTuple("column").Append("column"), "or", HTuple(0).Append(hv_BigStripeCol.TupleMax()), HTuple(hv_BigStripeCol.TupleMin()).Append(hv_ImageWidth - 1));
//		//ConcatObj(ho_SelectedRegions, ho_BigStripe, &ho_SelectedRegions);
//		Union1(ho_SelectedRegions, &ho_RegionUnion);
//		Connection(ho_RegionUnion, &ho_ConnectedRegions);
//		SortRegion(ho_ConnectedRegions, &ho_ConnectedRegions, "upper_left", "true", "column");
//
//
//		SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
//		//所有细黑条左边的点
//		hv_ColumnEdge.Clear();
//		for (int i = 0; i < hv_Column1.Length(); i++)
//		{
//			HTuple hvRowEdg, hvColEdg;
//			GenMeasureRectangle2(hv_ImageHeight / 2 - 20, hv_Column1[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
//				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
//			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "negative", "first", &hvRowEdg,
//				&hvColEdg, &HTuple(), &HTuple());
//			CloseMeasure(hv_MeasureHandle);
//			if (hvColEdg.Length() > 0)
//				hv_ColumnEdge.Append(hvColEdg);
//		}
//
//		//所有细黑条右边的点
//		hv_ColumnEdge2.Clear();
//		for (int i = 0; i < hv_Column2.Length(); i++)
//		{
//			HTuple hvRowEdg, hvColEdg;
//			GenMeasureRectangle2(hv_ImageHeight / 2 - 20, hv_Column2[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
//				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
//			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "positive", "first", &hvRowEdg,
//				&hvColEdg, &HTuple(), &HTuple());
//			CloseMeasure(hv_MeasureHandle);
//			if (hvColEdg.Length() > 0)
//				hv_ColumnEdge2.Append(hvColEdg);
//		}
//		if (hv_ColumnEdge.Length() != hv_ColumnEdge2.Length())
//		{
//			CString strExcp;
//			strExcp.Format("搜索到的黑条左右两边点数量不一致，请检查");
//			return FALSE;
//		}
//
//		//细黑条左右两边求中心点
//		HTuple hvSum, hvCenterCol;
//		TupleAdd(hv_ColumnEdge, hv_ColumnEdge2, &hvSum);
//		TupleDiv(hvSum, 2, &hvCenterCol);
//
//		//黑粗条左边的点
//		hv_ColumnBigStripeEdge.Clear();
//		for (int i = 0; i < hv_BigStripeCol.Length(); i++)
//		{
//			HTuple hvRowEdg, hvColEdg;
//			GenMeasureRectangle2(hv_ImageHeight / 2 - 20, hv_BigStripeCol[i], 0, (hv_Column2[0] - hv_Column1[0]) / 4, 20,
//				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
//			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "negative", "first", &hvRowEdg,
//				&hvColEdg, &HTuple(), &HTuple());
//			CloseMeasure(hv_MeasureHandle);
//			if (hvColEdg.Length() > 0)
//				hv_ColumnBigStripeEdge.Append(hvColEdg);
//		}
//
//		//黑粗条右边的点
//		hv_ColumnBigStripeEdge2.Clear();
//		for (int i = 0; i < hv_BigStripeCol2.Length(); i++)
//		{
//			HTuple hvRowEdg, hvColEdg;
//			GenMeasureRectangle2(hv_ImageHeight / 2 - 20, hv_BigStripeCol2[i], 0, 10, 20,
//				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
//			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "positive", "first", &hvRowEdg,
//				&hvColEdg, &HTuple(), &HTuple());
//			CloseMeasure(hv_MeasureHandle);
//			if (hvColEdg.Length() > 0)
//				hv_ColumnBigStripeEdge2.Append(hvColEdg);
//		}
//		if (hv_ColumnBigStripeEdge.Length() != hv_ColumnBigStripeEdge2.Length())
//		{
//			CString strExcp;
//			strExcp.Format("搜索到的黑粗条左右两边点数量不一致，请检查");
//			return FALSE;
//		}
//		//粗黑条取中点
//		HTuple hvBigStripeCenterCol;
//		TupleAdd(hv_ColumnBigStripeEdge, hv_ColumnBigStripeEdge2, &hvSum);
//		TupleDiv(hvSum, 2, &hvBigStripeCenterCol);
//
//		//筛选屏蔽线两边有效的点,包含屏蔽线
//		TupleSort(hvCenterCol, &hvCenterCol);
//		HTuple hv_Greater, hv_Smaller, hvOr, hvInc, hvColRightEffect, hvColLeftEffect, hvColRightNotEffect, hvColLeftNotEffect, hvCol00, hvCol11, hvLeftPixWidth, hvRightPixWidth, hvLeftRealCol, hvRightRealCol;
//		TupleGreaterEqualElem(hvCenterCol, hvBigStripeCenterCol.TupleMax() - 20, &hv_Greater);
//		TupleLessEqualElem(hvCenterCol, hvBigStripeCenterCol.TupleMin() + 20, &hv_Smaller);
//		TupleFind(hv_Greater, 1, &hvInc);
//		TupleSelect(hvCenterCol, hvInc, &hvColRightEffect);
//		TupleFind(hv_Smaller, 1, &hvInc);
//		TupleSelect(hvCenterCol, hvInc, &hvColLeftEffect);
//
//		//判断点找的对不对
//		TupleRemove(hvColLeftEffect, 0, &hvCol00);
//		TupleRemove(hvColLeftEffect, hvColLeftEffect.Length() - 1, &hvCol11);
//		TupleSub(hvCol00, hvCol11, &hvLeftPixWidth);
//		if (hvLeftPixWidth.TupleMax() / hvLeftPixWidth.TupleMin() > 1.5)
//		{
//			CString strExcp;
//			strExcp.Format("左相机区间点不均匀");
//			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
//			return FALSE;
//		}
//		TupleRemove(hvColRightEffect, 0, &hvCol00);
//		TupleRemove(hvColRightEffect, hvColRightEffect.Length() - 1, &hvCol11);
//		TupleSub(hvCol00, hvCol11, &hvRightPixWidth);
//		if (hvRightPixWidth.TupleMax() / hvRightPixWidth.TupleMin() > 1.5)
//		{
//			CString strExcp;
//			strExcp.Format("右相机区间点不均匀");
//			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
//			return FALSE;
//		}
//		//生成物理坐标
//		HTuple hvRealColBigStripe, hvRealColBigStripe2;
//		TupleGenSequence(0, (hvColLeftEffect.Length() - 1) * m_tCalParam.dUnitWidth, m_tCalParam.dUnitWidth, &hvLeftRealCol);
//
//		hvRealColBigStripe = hvLeftRealCol.TupleMax() + m_tCalParam.dUnitWidth * 1.5;
//		hvRealColBigStripe2 = hvRealColBigStripe.TupleMax() + m_tCalParam.dUnitWidth * 1.5;
//		TupleGenSequence(hvRealColBigStripe2 + m_tCalParam.dUnitWidth, hvRealColBigStripe2 + (hvColRightEffect.Length() - 1)*m_tCalParam.dUnitWidth, m_tCalParam.dUnitWidth, &hvRightRealCol);
//		m_tStripeParam.hvRealCol.Clear();
//		m_tStripeParam.hvRealCol.Append(hvLeftRealCol);
//		m_tStripeParam.hvRealCol.Append(hvRealColBigStripe);
//		m_tStripeParam.hvRealCol.Append(hvRealColBigStripe);//强插入一个物理左边，相当于两个相机粗黑条中间的间隔
//		m_tStripeParam.hvRealCol.Append(hvRealColBigStripe2);
//		m_tStripeParam.hvRealCol.Append(hvRightRealCol);
//
//		//生成区间分辨率
//		HTuple hvResLeft, hvResBigStripe, hvResBigStripe2, hvResRight;
//		TupleDiv(m_tCalParam.dUnitWidth, hvLeftPixWidth, &hvResLeft);
//		TupleDiv(m_tCalParam.dUnitWidth * 1.5, hvBigStripeCenterCol.TupleMin() - hvColLeftEffect.TupleMax(), &hvResBigStripe);
//		TupleDiv(m_tCalParam.dUnitWidth * 1.5, hvColRightEffect.TupleMin() - hvBigStripeCenterCol.TupleMax(), &hvResBigStripe2);
//		TupleDiv(m_tCalParam.dUnitWidth, hvRightPixWidth, &hvResRight);
//		m_tStripeParam.hvRes.Clear();
//		m_tStripeParam.hvRes.Append(hvResLeft);
//		m_tStripeParam.hvRes.Append(hvResBigStripe);
//		m_tStripeParam.hvRes.Append((hvResLeft.TupleMean() + hvResRight.TupleMean()) / 2);//两个相机粗黑条中间的分辨率用均值赋值，正常用不到，只用来占位
//		m_tStripeParam.hvRes.Append(hvResBigStripe2);
//		m_tStripeParam.hvRes.Append(hvResRight);
//
//		//对区间分辨率进行滤波
//		/*HTuple hvResFilter;
//		for (int i=0;i<m_tStripeParam.hvRes.Length();i++)
//		{
//			HTuple hvSel, hvMean;
//			if (i < 2)
//			{
//				TupleSelectRange(m_tStripeParam.hvRes, 0, i, &hvSel);
//				TupleMean(hvSel, &hvMean);
//				hvResFilter.Append(hvMean);
//			}
//			else
//			{
//				TupleSelectRange(m_tStripeParam.hvRes, i - 2, i, &hvSel);
//				TupleMean(hvSel, &hvMean);
//				hvResFilter.Append(hvMean);
//			}
//		}
//		m_tStripeParam.hvRes = hvResFilter;*/
//
//		//生成像素坐标
//		m_tStripeParam.hvPixelCol.Clear();
//		TupleConcat(hvColLeftEffect, hvBigStripeCenterCol, &m_tStripeParam.hvPixelCol);
//		TupleConcat(m_tStripeParam.hvPixelCol, hvColRightEffect, &m_tStripeParam.hvPixelCol);
//
//		//计算左右两边相机的分辨率
//		if (m_vecResolution.size() < 2) m_vecResolution.resize(2, m_vecResolution[0]);
//		m_vecResolution[0].dColRes = (double)(hvColLeftEffect.Length()) * m_tCalParam.dUnitWidth / (hvColLeftEffect.TupleMax().D() - hvColLeftEffect.TupleMin().D());
//		m_vecResolution[1].dColRes = (double)hvColRightEffect.Length() * m_tCalParam.dUnitWidth / (hvColRightEffect.TupleMax().D() - hvColRightEffect.TupleMin().D());
//
//		m_tStripeParam.hvColumnOver.Clear();
//		m_tStripeParam.hvColumnOver.Append(hvBigStripeCenterCol.TupleMin());
//		m_tStripeParam.hvColumnOver.Append(hvBigStripeCenterCol.TupleMax());
//
//		//m_tJZ.dColRes = (m_vecResolution[0].dColRes + m_vecResolution[1].dColRes) / 2;//修改为分辨率结构体
//
//		HTuple hvRow1, hvRow2;
//		TupleGenConst(m_tStripeParam.hvPixelCol.Length(), hv_ImageHeight / 2 - 20, &hvRow1);
//		TupleGenConst(m_tStripeParam.hvPixelCol.Length(), hv_ImageHeight / 2 + 20, &hvRow2);
//		GenCrossContourXld(&Cross, hvRow1, m_tStripeParam.hvPixelCol, 30, 0);
//		ConcatObj(tOut.hoDisp, Cross, &tOut.hoDisp);
//		GenRegionLine(&ho_RegionLines, hvRow1, m_tStripeParam.hvPixelCol, hvRow2, m_tStripeParam.hvPixelCol);
//		ConcatObj(tOut.hoDisp, ho_RegionLines, &tOut.hoDisp);
//	}
//	catch (HException& e)
//	{
//		CString strExcp;
//		strExcp.Format("CCalibration_StripeCalibration: 标定过程异常 %s [%s]", e.ErrorMessage().Text(), e.ProcName().Text());
//		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
//		return FALSE;
//	}
//
//
//
//	return TRUE;
//}

BOOL CCalibration::StripeDoubleCamCalibration(CalInput& tIn, CalOutput& tOut)
{
	HObject  ho_Region, ho_Rectangle, ho_ImageReduced, Cross, ho_RegionOpen;
	HObject  ho_Rectangle1, ho_RegionLines, hoRegionRec, hoReduce;
	HObject  ho_RegionUnion, ho_ConnectedRegions,ho_RegionsTmp, ho_SelectedRegions, hoSelectedRegions1, ho_BigStripe,hoRegionDilation;
	HObject  hoImageReduced1;
	// Local control variables
	HTuple  hv_DistanceOver, hv_ColumnOver;
	HTuple  hv_IndexStart1, hv_IndexOver;
	HTuple  hv_Xrow, hv_Indics1, hv_Indics2, hv_Width;
	HTuple  hv_Height, hv_Row1, hv_Column1;
	HTuple  hv_Row2, hv_Column2, hv_Area, hv_Row, hv_Column;
	HTuple  hv_MeasureHandle, hv_RowEdge, hv_ColumnEdge, hv_Amplitude, hv_ColumnEdge2, hv_ColumnBigStripeEdge, hv_ColumnBigStripeEdge2;
	HTuple  hv_Distance, hv_Length, hv_i, hv_ii, hv_j, hv_iii, hv_PixtoDis;
	HTuple  hv_IndexStart, hv_average;
	HTuple  hv_IndexTest, hv_total, hv_LengthOver;
	HTuple  hv_totalresolution, hv_ColumnEdgeStart, hv_ColumnEdgeEnd, hv_RealLength;
	HTuple	hv_ImageWidth, hv_ImageHeight, hv_Width1,hv_Height1, hv_Max1, hv_BigStripeCol, hv_WidthMedian;
	HTuple  hv_Row11, hv_Column11;

	try
	{
		GetImageSize(tIn.hoImage, &hv_ImageWidth, &hv_ImageHeight);
		GenEmptyObj(&tOut.hoDisp);

		hv_Xrow = HTuple();
		hv_DistanceOver = HTuple();
		hv_ColumnOver = HTuple();
		hv_IndexOver = HTuple();
		hv_Indics1 = 0;
		hv_Indics2 = 0;
		hv_total = 0;
		hv_average = 0;

		//Threshold(tIn.hoImage, &ho_Region, m_tCalParam.nThres, 255);
		HTuple hvROIRow0, hvROICol0, hvROIRow1, hvROICol1;
		SmallestRectangle1(tIn.hoROI, &hvROIRow0, &hvROICol0, &hvROIRow1, &hvROICol1);
		//GenRectangle1(&hoRegionRec, 0, max(m_tCalParam.nImageIgnoreWidth, 0), hv_ImageHeight - 1, min(hv_ImageWidth - m_tCalParam.nImageIgnoreWidth, hv_ImageWidth - 1));
		ReduceDomain(tIn.hoImage, tIn.hoROI, &hoReduce);
		//VarThreshold(hoReduce, &ho_Region, min(hvROICol1- hvROICol0-1, 100), min(hvROIRow1- hvROIRow0-1, 80), 0.5, m_tCalParam.nThres, "dark");
		//BinaryThreshold(hoReduce, &ho_Region, "max_separability", "dark", &HTuple());
		AutoThreshold(hoReduce, &ho_Region, m_tCalParam.nThres);
		Connection(ho_Region, &ho_ConnectedRegions);
		SelectShape(ho_ConnectedRegions, &ho_RegionsTmp, "height", "and", 20, (hvROIRow1 - hvROIRow0)+1);
		RegionFeatures(ho_RegionsTmp, "width", &hv_Width1);
		RegionFeatures(ho_RegionsTmp, "height", &hv_Height1);
		//筛选圆点
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "width", "and", hv_Width1.TupleMedian().D() * m_tCalParam.dDisDownLimit, 
			hv_Width1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "height", "and", hv_Height1.TupleMedian().D() / 8, hv_Height1.TupleMedian().D() /3);
		//SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "circularity", "and", 0.45, 1);
		FillUp(ho_SelectedRegions,&ho_SelectedRegions);
		SortRegion(ho_SelectedRegions,&ho_SelectedRegions,"first_point","true","column");
		if (ho_SelectedRegions.CountObj() != 2)
		{
			return FALSE;
		}
		HTuple hvHorProjection,hvVertProjection,hvIndices,hvYRow,hvXCol,hv_Phi, hv_Length1, hv_Length2,hv_Area,hv_MeasureHandle,hvColumnEdgeFirst,hvIntraDistance;
		//找左侧圆心
		HObject hoLeftCicle;
		SelectObj(ho_SelectedRegions,&hoLeftCicle,1);
		DilationRectangle1 (hoLeftCicle, &hoRegionDilation, 5, 5);
		ShapeTrans(hoRegionDilation, &hoRegionDilation, "rectangle1");
		SmallestRectangle1 (hoRegionDilation, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
		//找左侧圆心`
		ReduceDomain (hoReduce, hoRegionDilation, &hoImageReduced1);
		//圆心ROW坐标 ，利用投影计算，更精准的方式逐行扫描就最大宽度
		GrayProjections (hoRegionDilation, hoImageReduced1, "simple", &hvHorProjection, &hvVertProjection);
		TupleSortIndex (hvHorProjection, &hvIndices);

		hvYRow=hv_Row11+hvIndices[0].D();
		//圆心COL坐标 ，利用YROW，做水平MEASURE
		SmallestRectangle2 (hoRegionDilation, &hv_Row1, &hv_Column1, &hv_Phi, &hv_Length1, &hv_Length2);
		AreaCenter (hoRegionDilation, &hv_Area, &hv_Row, &hv_Column);

		GenMeasureRectangle2 (hvYRow, hv_Column, 0, hv_Length1+5, 4, hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
		MeasurePairs (hoReduce, hv_MeasureHandle, 2, 9, "all", "first", &HTuple(), &hvColumnEdgeFirst, &HTuple(), &HTuple(), &HTuple(), &HTuple(), &hvIntraDistance, &HTuple());
		CloseMeasure(hv_MeasureHandle);
		//这里需要判断下有么有值
		hvXCol=hvColumnEdgeFirst+hvIntraDistance[0]/2.0;

		//屏蔽区域
		hv_BigStripeCol.Clear();
		hv_BigStripeCol.Append(hvXCol);

		HTuple hvYRow2,hvXCol2;
		//找右侧圆心
		HObject hoRightCicle;
		SelectObj(ho_SelectedRegions,&hoRightCicle,2);
		DilationRectangle1 (hoRightCicle, &hoRegionDilation, 5, 5);
		ShapeTrans(hoRegionDilation, &hoRegionDilation, "rectangle1");
		SmallestRectangle1 (hoRegionDilation, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
		//找右侧圆心
		ReduceDomain (hoReduce, hoRegionDilation, &hoImageReduced1);
		//圆心ROW坐标 ，利用投影计算，更精准的方式逐行扫描就最大宽度
		GrayProjections (hoRegionDilation, hoImageReduced1, "simple", &hvHorProjection, &hvVertProjection);
		TupleSortIndex (hvHorProjection, &hvIndices);

		hvYRow2=hv_Row11+hvIndices[0].D();
		//圆心COL2坐标 ，利用YROW2，做水平MEASURE
		SmallestRectangle2 (hoRegionDilation, &hv_Row1, &hv_Column1, &hv_Phi, &hv_Length1, &hv_Length2);
		AreaCenter (hoRegionDilation, &hv_Area, &hv_Row, &hv_Column);

		GenMeasureRectangle2 (hvYRow2, hv_Column, 0, hv_Length1+5, 2, hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
		MeasurePairs (hoReduce, hv_MeasureHandle, 2, 9, "all", "first", &HTuple(), &hvColumnEdgeFirst, &HTuple(), &HTuple(), &HTuple(), &HTuple(), &hvIntraDistance, &HTuple());
		CloseMeasure(hv_MeasureHandle);
		//这里需要判断下有么有值
		hvXCol2=hvColumnEdgeFirst+hvIntraDistance[0]/2.0;

		//屏蔽区域
		hv_BigStripeCol.Append(hvXCol2);


		//筛选黑条
		SelectShape(ho_ConnectedRegions, &hoSelectedRegions1, "width", "and", hv_Width1.TupleMedian().D() * m_tCalParam.dDisDownLimit, 
			hv_Width1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(hoSelectedRegions1, &hoSelectedRegions1, "height", "and", hv_Height1.TupleMedian().D() * m_tCalParam.dDisDownLimit, 
			hv_Height1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(hoSelectedRegions1, &hoSelectedRegions1, HTuple("column").Append("column"), "or", HTuple(0).Append(hv_BigStripeCol.TupleMax()), HTuple(hv_BigStripeCol.TupleMin()).Append(hv_ImageWidth - 1));
		//ConcatObj(ho_SelectedRegions, ho_BigStripe, &ho_SelectedRegions);
		Union1(hoSelectedRegions1, &ho_RegionUnion);
		Connection(ho_RegionUnion, &ho_ConnectedRegions);
		SortRegion(ho_ConnectedRegions, &ho_ConnectedRegions, "upper_left", "true", "column");

		HTuple hvGreater,hvIndex,hvMesureCol1;
		SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		TupleGreaterElem (hv_Column1, hvXCol2, &hvGreater);
		TupleFindFirst (hvGreater, 1, &hvIndex);
		hvMesureCol1 = hv_Column1[hvIndex.I()].D();

		//生成测量参数，右圆右侧的第一个黑条的左边
		HTuple hvMetrologyHandle,hvMetroIndex1,hvParameter,hvPhi1,hvDeg,hvTan,hvDiff,XCrossPT;
		CreateMetrologyModel (&hvMetrologyHandle);
		SetMetrologyModelImageSize(hvMetrologyHandle, hv_ImageWidth, hv_ImageHeight);
		AddMetrologyObjectLineMeasure (hvMetrologyHandle, hv_Row1[hvIndex.I()].D(), hvMesureCol1.D(), hv_Row2[hvIndex.I()].D(), hvMesureCol1.D(), 20, (hv_Row2[hvIndex.I()].D()- hv_Row1[hvIndex.I()].D())/5, 1, 30, "num_measures", 5, &hvMetroIndex1);
		SetMetrologyObjectParam(hvMetrologyHandle, hvMetroIndex1, "min_score", 0.6);
		SetMetrologyObjectParam(hvMetrologyHandle, hvMetroIndex1, "num_measures", 5);
		ApplyMetrologyModel (hoReduce, hvMetrologyHandle);
		GetMetrologyObjectResult (hvMetrologyHandle, hvMetroIndex1, "all", "result_type", "all_param", &hvParameter);
		//GetMetrologyObjectResultContour (Contour, MetrologyHandle, 0, 'all', 1.5)

		//找右图上的圆右侧的黑条的左边线
		LineOrientation (hvParameter[0], hvParameter[1], hvParameter[2], hvParameter[3], &hvPhi1);
		TupleDeg (hvPhi1, &hvDeg);
		TupleTan (hvPhi1, &hvTan);
		hvDiff=(hvYRow-hvYRow2)/hvTan;
		//XCrossPT就是在右侧图像上对应左侧圆心实际的像素重合位置。
		//注意这里，两个点的上下坐标，斜率都会影响加减变大变小。
		XCrossPT==hvXCol2+hvDiff;

		//拿黑白条纹标定右侧相机；计算XcrossPT对应的右侧相机单独坐标。
		//用XCrossPT，和左侧物理X坐标，修正所有的右侧相机标定结果。
		

		//所有细黑条左边的点
		hv_ColumnEdge.Clear();
		for (int i = 0; i < hv_Column1.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2((hv_Row1[i]+hv_Row2[i])/2 - 20, hv_Column1[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "negative", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge.Append(hvColEdg);
		}

		//所有细黑条右边的点
		hv_ColumnEdge2.Clear();
		for (int i = 0; i < hv_Column2.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2((hv_Row1[i]+hv_Row2[i])/2 - 20, hv_Column2[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "positive", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge2.Append(hvColEdg);
		}
		if (hv_ColumnEdge.Length() != hv_ColumnEdge2.Length())
		{
			CString strExcp;
			strExcp.Format("搜索到的黑条左右两边点数量不一致，请检查");
			return FALSE;
		}

		//细黑条左右两边求中心点
		HTuple hvSum, hvCenterCol;
		TupleAdd(hv_ColumnEdge, hv_ColumnEdge2, &hvSum);
		TupleDiv(hvSum, 2, &hvCenterCol);

		//筛选屏蔽线两边有效的点,包含屏蔽线
		TupleSort(hvCenterCol, &hvCenterCol);
		HTuple hv_Greater, hv_Smaller, hvOr, hvInc, hvColRightEffect, hvColLeftEffect, hvColRightNotEffect, hvColLeftNotEffect, hvCol00, hvCol11, hvLeftPixWidth, hvRightPixWidth, hvLeftRealCol, hvRightRealCol;
		TupleGreaterEqualElem(hvCenterCol, hv_BigStripeCol.TupleMax() - 20, &hv_Greater);
		TupleLessEqualElem(hvCenterCol, hv_BigStripeCol.TupleMin() + 20, &hv_Smaller);
		TupleFind(hv_Greater, 1, &hvInc);
		TupleSelect(hvCenterCol, hvInc, &hvColRightEffect);
		TupleFind(hv_Smaller, 1, &hvInc);
		TupleSelect(hvCenterCol, hvInc, &hvColLeftEffect);

		//判断点找的对不对
		TupleRemove(hvColLeftEffect, 0, &hvCol00);
		TupleRemove(hvColLeftEffect, hvColLeftEffect.Length() - 1, &hvCol11);
		TupleSub(hvCol00, hvCol11, &hvLeftPixWidth);
		if (hvLeftPixWidth.TupleMax() / hvLeftPixWidth.TupleMin() > 1.5)
		{
			CString strExcp;
			strExcp.Format("左相机区间点不均匀");
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
			return FALSE;
		}
		TupleRemove(hvColRightEffect, 0, &hvCol00);
		TupleRemove(hvColRightEffect, hvColRightEffect.Length() - 1, &hvCol11);
		TupleSub(hvCol00, hvCol11, &hvRightPixWidth);
		if (hvRightPixWidth.TupleMax() / hvRightPixWidth.TupleMin() > 1.5)
		{
			CString strExcp;
			strExcp.Format("右相机区间点不均匀");
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
			return FALSE;
		}

		//计算XCrossPT
		HTuple hvResTmp = m_tCalParam.dUnitWidth/(hvColRightEffect.TupleMin().D() - hvXCol2);
		HTuple hvXDiffReal = hvDiff*hvResTmp;

		//生成物理坐标
		HTuple hvRealColBigStripe, hvRealColBigStripe2;
		TupleGenSequence(0, (hvColLeftEffect.Length() - 1) * m_tCalParam.dUnitWidth, m_tCalParam.dUnitWidth, &hvLeftRealCol);

		hvRealColBigStripe = hvLeftRealCol.TupleMax() + m_tCalParam.dUnitWidth;
		hvRealColBigStripe2 = hvRealColBigStripe.TupleMax() + m_tCalParam.dUnitWidth + hvXDiffReal;
		TupleGenSequence(hvRealColBigStripe2 + m_tCalParam.dUnitWidth, hvRealColBigStripe2 + (hvColRightEffect.Length() - 1)*m_tCalParam.dUnitWidth, m_tCalParam.dUnitWidth, &hvRightRealCol);
		m_tStripeParam.hvRealCol.Clear();
		m_tStripeParam.hvRealCol.Append(hvLeftRealCol);
		m_tStripeParam.hvRealCol.Append(hvRealColBigStripe);
		m_tStripeParam.hvRealCol.Append(hvRealColBigStripe + hvXDiffReal);//强插入一个物理左边，相当于两个相机粗黑条中间的间隔
		m_tStripeParam.hvRealCol.Append(hvRealColBigStripe2);
		m_tStripeParam.hvRealCol.Append(hvRightRealCol);

		//生成区间分辨率
		HTuple hvResLeft, hvResBigStripe, hvResBigStripe2, hvResRight;
		TupleDiv(m_tCalParam.dUnitWidth, hvLeftPixWidth, &hvResLeft);
		TupleDiv(m_tCalParam.dUnitWidth, hv_BigStripeCol.TupleMin() - hvColLeftEffect.TupleMax(), &hvResBigStripe);
		TupleDiv(m_tCalParam.dUnitWidth, hvColRightEffect.TupleMin() - hv_BigStripeCol.TupleMax(), &hvResBigStripe2);
		TupleDiv(m_tCalParam.dUnitWidth, hvRightPixWidth, &hvResRight);
		m_tStripeParam.hvRes.Clear();
		m_tStripeParam.hvRes.Append(hvResLeft);
		m_tStripeParam.hvRes.Append(hvResBigStripe);
		m_tStripeParam.hvRes.Append((hvResLeft[hvResLeft.Length()-1].D() + hvResRight[hvResRight.Length()-1].D()) / 2);//两个相机粗黑条中间的分辨率用均值赋值，正常用不到，只用来占位
		m_tStripeParam.hvRes.Append(hvResBigStripe2);
		m_tStripeParam.hvRes.Append(hvResRight);

		//对区间分辨率进行滤波
		/*HTuple hvResFilter;
		for (int i=0;i<m_tStripeParam.hvRes.Length();i++)
		{
			HTuple hvSel, hvMean;
			if (i < 2)
			{
				TupleSelectRange(m_tStripeParam.hvRes, 0, i, &hvSel);
				TupleMean(hvSel, &hvMean);
				hvResFilter.Append(hvMean);
			}
			else
			{
				TupleSelectRange(m_tStripeParam.hvRes, i - 2, i, &hvSel);
				TupleMean(hvSel, &hvMean);
				hvResFilter.Append(hvMean);
			}
		}
		m_tStripeParam.hvRes = hvResFilter;*/

		//生成像素坐标
		m_tStripeParam.hvPixelCol.Clear();
		TupleConcat(hvColLeftEffect, hv_BigStripeCol, &m_tStripeParam.hvPixelCol);
		TupleConcat(m_tStripeParam.hvPixelCol, hvColRightEffect, &m_tStripeParam.hvPixelCol);

		//计算左右两边相机的分辨率
		if (m_vecResolution.size() < 2) m_vecResolution.resize(2, m_vecResolution[0]);
		m_vecResolution[0].dColRes = (double)(hvColLeftEffect.Length()) * m_tCalParam.dUnitWidth / (hvColLeftEffect.TupleMax().D() - hvColLeftEffect.TupleMin().D());
		m_vecResolution[1].dColRes = (double)hvColRightEffect.Length() * m_tCalParam.dUnitWidth / (hvColRightEffect.TupleMax().D() - hvColRightEffect.TupleMin().D());

		m_tStripeParam.hvColumnOver.Clear();
		m_tStripeParam.hvColumnOver.Append(hv_BigStripeCol.TupleMin());
		m_tStripeParam.hvColumnOver.Append(hv_BigStripeCol.TupleMax());

		//m_tJZ.dColRes = (m_vecResolution[0].dColRes + m_vecResolution[1].dColRes) / 2;//修改为分辨率结构体

		HTuple hvRow1, hvRow2;
		TupleGenConst(m_tStripeParam.hvPixelCol.Length(), (hvROIRow0+ hvROIRow1) / 2 - 20, &hvRow1);
		TupleGenConst(m_tStripeParam.hvPixelCol.Length(), (hvROIRow0 + hvROIRow1) / 2 + 20, &hvRow2);
		GenCrossContourXld(&Cross, hvRow1, m_tStripeParam.hvPixelCol, 30, 0);
		ConcatObj(tOut.hoDisp, Cross, &tOut.hoDisp);
		GenRegionLine(&ho_RegionLines, hvRow1, m_tStripeParam.hvPixelCol, hvRow2, m_tStripeParam.hvPixelCol);
		ConcatObj(tOut.hoDisp, ho_RegionLines, &tOut.hoDisp);
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CCalibration_StripeCalibration: 标定过程异常 %s [%s]", e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}



	return TRUE;
}

BOOL CCalibration::StripeDoubleCamCalibrationFirst(CalInput& tIn, CalOutput& tOut)
{
	HObject  ho_Region, ho_Rectangle, ho_ImageReduced, Cross, ho_RegionOpen;
	HObject  ho_Rectangle1, ho_RegionLines, hoRegionRec, hoReduce;
	HObject  ho_RegionUnion, ho_ConnectedRegions, ho_RegionsTmp, ho_SelectedRegions, hoSelectedRegions1, ho_BigStripe, hoRegionDilation;
	HObject  hoImageReduced1;
	// Local control variables
	HTuple  hv_IndexStart1;
	HTuple  hv_Width;
	HTuple  hv_Height, hv_Row1, hv_Column1;
	HTuple  hv_Row2, hv_Column2, hv_Area, hv_Row, hv_Column;
	HTuple  hv_MeasureHandle, hv_RowEdge, hv_ColumnEdge, hv_Amplitude, hv_ColumnEdge2, hv_ColumnBigStripeEdge, hv_ColumnBigStripeEdge2;
	HTuple  hv_Distance, hv_Length, hv_i, hv_ii, hv_j, hv_iii, hv_PixtoDis;
	HTuple  hv_IndexStart;
	HTuple  hv_IndexTest, hv_LengthOver;
	HTuple  hv_totalresolution, hv_ColumnEdgeStart, hv_ColumnEdgeEnd, hv_RealLength;
	HTuple	hv_ImageWidth, hv_ImageHeight, hv_Width1, hv_Height1, hv_Max1, hv_BigStripeCol, hv_WidthMedian;
	HTuple  hv_Row11, hv_Column11;

	//双相机第一次标定要实现的功能是标定左相机的黑条坐标，计算右相机的圆点和左相机的圆点的物理差，计算右相机的圆点像素位置
	try
	{
		GetImageSize(tIn.hoImage, &hv_ImageWidth, &hv_ImageHeight);
		GenEmptyObj(&tOut.hoDisp);

		HTuple hvROIRow0, hvROICol0, hvROIRow1, hvROICol1;
		SmallestRectangle1(tIn.hoROI, &hvROIRow0, &hvROICol0, &hvROIRow1, &hvROICol1);
		ReduceDomain(tIn.hoImage, tIn.hoROI, &hoReduce);
		CropDomain(hoReduce, &hoReduce);
		VarThreshold(hoReduce, &ho_Region, min(hvROICol1- hvROICol0-1, 200), min(hvROIRow1- hvROIRow0-1, 20), 0.5, m_tCalParam.nThres, "dark");
		//BinaryThreshold(hoReduce, &ho_Region, "max_separability", "dark", &HTuple());
		//AutoThreshold(hoReduce, &ho_Region, m_tCalParam.nThres);
		MoveRegion(ho_Region, &ho_Region, hvROIRow0, hvROICol0);
		//OpeningRectangle1(ho_Region, &ho_Region, 10, 1);
		//OpeningRectangle1(ho_Region, &ho_Region, 1, 10);
		Connection(ho_Region, &ho_ConnectedRegions);
		SelectShape(ho_ConnectedRegions, &ho_RegionsTmp, "height", "and", 20, (hvROIRow1 - hvROIRow0) + 1);
		SelectShape(ho_RegionsTmp, &ho_RegionsTmp, "rectangularity", "and", 0.85, 1);
		//SelectShape(ho_RegionsTmp, &ho_RegionsTmp, "anisometry", "and", 1.5, 999);

		RegionFeatures(ho_RegionsTmp, "width", &hv_Width1);
		RegionFeatures(ho_RegionsTmp, "height", &hv_Height1);

		//去除部分干扰
		Union1(ho_RegionsTmp, &ho_RegionUnion);
		ClosingRectangle1(ho_RegionUnion, &ho_RegionUnion, hv_Width1.TupleMedian().D() * 4, 1);
		ShapeTrans(ho_RegionUnion, &ho_RegionUnion, "convex");
		Intersection(ho_ConnectedRegions, ho_RegionUnion, &ho_ConnectedRegions);
		Connection(ho_ConnectedRegions, &ho_ConnectedRegions);

		//筛选圆点
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "width", "and", hv_Width1.TupleMedian().D() * m_tCalParam.dDisDownLimit,
			hv_Width1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "height", "and", hv_Height1.TupleMedian().D() / 8, hv_Height1.TupleMedian().D() / 3);
		FillUp(ho_SelectedRegions, &ho_SelectedRegions);
		SortRegion(ho_SelectedRegions, &ho_SelectedRegions, "first_point", "true", "column");
		if (ho_SelectedRegions.CountObj() != 2)
		{
			return FALSE;
		}
		HTuple hvHorProjection, hvVertProjection, hvIndices, hvYRow, hvXCol, hv_Phi, hv_Length1, hv_Length2, hv_Area, hv_MeasureHandle, hvColumnEdgeFirst, hvIntraDistance;
		//找左侧圆心
		HObject hoLeftCicle;
		SelectObj(ho_SelectedRegions, &hoLeftCicle, 1);
		DilationRectangle1(hoLeftCicle, &hoRegionDilation, 5, 5);
		ShapeTrans(hoRegionDilation, &hoRegionDilation, "rectangle1");
		SmallestRectangle1(hoRegionDilation, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
		//找左侧圆心
		ReduceDomain(tIn.hoImage, hoRegionDilation, &hoImageReduced1);
		//圆心ROW坐标 ，利用投影计算，更精准的方式逐行扫描就最大宽度
		GrayProjections(hoRegionDilation, hoImageReduced1, "simple", &hvHorProjection, &hvVertProjection);
		TupleSortIndex(hvHorProjection, &hvIndices);

		hvYRow = hv_Row11 + hvIndices[0].D();
		//圆心COL坐标 ，利用YROW，做水平MEASURE
		SmallestRectangle2(hoRegionDilation, &hv_Row1, &hv_Column1, &hv_Phi, &hv_Length1, &hv_Length2);
		AreaCenter(hoRegionDilation, &hv_Area, &hv_Row, &hv_Column);

		GenMeasureRectangle2(hvYRow, hv_Column, 0, hv_Length1 + 5, 4, hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
		MeasurePairs(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "all", "first", &HTuple(), &hvColumnEdgeFirst, &HTuple(), &HTuple(), &HTuple(), &HTuple(), &hvIntraDistance, &HTuple());
		CloseMeasure(hv_MeasureHandle);
		//这里需要判断下有么有值
		hvXCol = hvColumnEdgeFirst + hvIntraDistance[0] / 2.0;

		//屏蔽区域
		hv_BigStripeCol.Clear();
		hv_BigStripeCol.Append(hvXCol);
		m_tDoubleStripeTmpData.hvLeftCirclePixelCol = hvXCol;

		HTuple hvYRow2, hvXCol2;
		//找右侧圆心
		HObject hoRightCicle;
		SelectObj(ho_SelectedRegions, &hoRightCicle, 2);
		DilationRectangle1(hoRightCicle, &hoRegionDilation, 5, 5);
		ShapeTrans(hoRegionDilation, &hoRegionDilation, "rectangle1");
		SmallestRectangle1(hoRegionDilation, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
		//找右侧圆心
		ReduceDomain(tIn.hoImage, hoRegionDilation, &hoImageReduced1);
		//圆心ROW坐标 ，利用投影计算，更精准的方式逐行扫描就最大宽度
		GrayProjections(hoRegionDilation, hoImageReduced1, "simple", &hvHorProjection, &hvVertProjection);
		TupleSortIndex(hvHorProjection, &hvIndices);

		hvYRow2 = hv_Row11 + hvIndices[0].D();
		//圆心COL2坐标 ，利用YROW2，做水平MEASURE
		SmallestRectangle2(hoRegionDilation, &hv_Row1, &hv_Column1, &hv_Phi, &hv_Length1, &hv_Length2);
		AreaCenter(hoRegionDilation, &hv_Area, &hv_Row, &hv_Column);

		GenMeasureRectangle2(hvYRow2, hv_Column, 0, hv_Length1 + 5, 2, hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
		MeasurePairs(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "all", "first", &HTuple(), &hvColumnEdgeFirst, &HTuple(), &HTuple(), &HTuple(), &HTuple(), &hvIntraDistance, &HTuple());
		CloseMeasure(hv_MeasureHandle);
		//这里需要判断下有么有值
		hvXCol2 = hvColumnEdgeFirst + hvIntraDistance[0] / 2.0;

		//屏蔽区域
		hv_BigStripeCol.Append(hvXCol2);
		m_tDoubleStripeTmpData.hvRightCirclePixelCol = hvXCol2;

		//筛选右相机黑条
		SelectShape(ho_ConnectedRegions, &hoSelectedRegions1, "width", "and", hv_Width1.TupleMedian().D() * m_tCalParam.dDisDownLimit,
			hv_Width1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(hoSelectedRegions1, &hoSelectedRegions1, "height", "and", hv_Height1.TupleMedian().D() * m_tCalParam.dDisDownLimit,
			hv_Height1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(hoSelectedRegions1, &hoSelectedRegions1, "rectangularity", "and", 0.85, 1);
		//SelectShape(hoSelectedRegions1, &hoSelectedRegions1, HTuple("column"), "and", HTuple(0), HTuple(hv_ImageWidth / 2 - 1));
		Union1(hoSelectedRegions1, &ho_RegionUnion);
		Connection(ho_RegionUnion, &ho_ConnectedRegions);
		SortRegion(ho_ConnectedRegions, &ho_ConnectedRegions, "upper_left", "true", "column");

		HTuple hvGreater, hvIndex, hvMesureCol1;
		SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		TupleGreaterElem(hv_Column1, hvXCol2, &hvGreater);
		TupleFindFirst(hvGreater, 1, &hvIndex);
		hvMesureCol1 = hv_Column1[hvIndex.I()].D();

		//生成测量参数，右圆右侧的第一个黑条的左边
		HTuple hvMetrologyHandle, hvMetroIndex1, hvParameter, hvPhi1, hvDeg, hvTan, hvDiff, XCrossPT;
		CreateMetrologyModel(&hvMetrologyHandle);
		SetMetrologyModelImageSize(hvMetrologyHandle, hv_ImageWidth, hv_ImageHeight);
		AddMetrologyObjectLineMeasure(hvMetrologyHandle, hv_Row1[hvIndex.I()].D(), hvMesureCol1.D(), hv_Row2[hvIndex.I()].D(), hvMesureCol1.D(), 20, (hv_Row2[hvIndex.I()].D() - hv_Row1[hvIndex.I()].D()) / 5, 1, 30, "num_measures", 5, &hvMetroIndex1);
		SetMetrologyObjectParam(hvMetrologyHandle, hvMetroIndex1, "min_score", 0.6);
		SetMetrologyObjectParam(hvMetrologyHandle, hvMetroIndex1, "num_measures", 5);
		ApplyMetrologyModel(tIn.hoImage, hvMetrologyHandle);
		GetMetrologyObjectResult(hvMetrologyHandle, hvMetroIndex1, "all", "result_type", "all_param", &hvParameter);
		//GetMetrologyObjectResultContour (Contour, MetrologyHandle, 0, 'all', 1.5)

		//找右图上的圆右侧的黑条的左边线
		LineOrientation(hvParameter[0], hvParameter[1], hvParameter[2], hvParameter[3], &hvPhi1);
		TupleDeg(hvPhi1, &hvDeg);
		TupleTan(hvPhi1, &hvTan);
		hvDiff = (hvYRow - hvYRow2) / hvTan;
		//XCrossPT就是在右侧图像上对应左侧圆心实际的像素重合位置。
		//注意这里，两个点的上下坐标，斜率都会影响加减变大变小。
		//XCrossPT == hvXCol2 + hvDiff;

		//拿黑白条纹标定右侧相机；计算XcrossPT对应的右侧相机单独坐标。
		//用XCrossPT，和左侧物理X坐标，修正所有的右侧相机标定结果。
		

		//只保留左相机的黑条
		SelectShape(hoSelectedRegions1, &hoSelectedRegions1, HTuple("column"), "and", HTuple(0), HTuple(hv_ImageWidth / 2 - 1));
		Union1(hoSelectedRegions1, &ho_RegionUnion);
		Connection(ho_RegionUnion, &ho_ConnectedRegions);
		SortRegion(ho_ConnectedRegions, &ho_ConnectedRegions, "upper_left", "true", "column");
		SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		//如果左相机最后一个黑条挨着图像右边，就去除掉
		if (hv_Column2[hv_Column2.Length() - 1].D() >= hv_ImageWidth / 2 - 2)
		{
			RemoveObj(ho_ConnectedRegions, &ho_ConnectedRegions, ho_ConnectedRegions.CountObj());
			SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		}

		//所有细黑条左边的点
		hv_ColumnEdge.Clear();
		for (int i = 0; i < hv_Column1.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2(hvYRow, hv_Column1[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "negative", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge.Append(hvColEdg);
		}

		//所有细黑条右边的点
		hv_ColumnEdge2.Clear();
		for (int i = 0; i < hv_Column2.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2(hvYRow, hv_Column2[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "positive", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge2.Append(hvColEdg);
		}
		if (hv_ColumnEdge.Length() != hv_ColumnEdge2.Length())
		{
			CString strExcp;
			strExcp.Format("搜索到的黑条左右两边点数量不一致，请检查");
			return FALSE;
		}

		//细黑条左右两边求中心点
		HTuple hvSum, hvCenterCol;
		TupleAdd(hv_ColumnEdge, hv_ColumnEdge2, &hvSum);
		TupleDiv(hvSum, 2, &hvCenterCol);

		//筛选左相机有效的点，包含屏蔽区域区域内的点
		hvCenterCol.Append(hvXCol);
		hvCenterCol.Append(hvXCol2);
		TupleSort(hvCenterCol, &hvCenterCol);
		HTuple hv_Smaller, hvOr, hvInc, hvColLeftEffect, hvCol00, hvCol11, hvLeftPixWidth, hvLeftRealCol;
		TupleLessEqualElem(hvCenterCol, hv_ImageWidth / 2 - 1, &hv_Smaller);
		TupleFind(hv_Smaller, 1, &hvInc);
		TupleSelect(hvCenterCol, hvInc, &hvColLeftEffect);

		//判断点找的对不对
		TupleRemove(hvColLeftEffect, 0, &hvCol00);
		TupleRemove(hvColLeftEffect, hvColLeftEffect.Length() - 1, &hvCol11);
		TupleSub(hvCol00, hvCol11, &hvLeftPixWidth);
		if (hvLeftPixWidth.TupleMax() / hvLeftPixWidth.TupleMin() > 1.5)
		{
			CString strExcp;
			strExcp.Format("左相机区间点不均匀");
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
			return FALSE;
		}

		//计算两圆心中心X方向物理坐标偏差
		TupleLessEqualElem(hvCenterCol, hvXCol2, &hv_Smaller);
		TupleFindFirst(hv_Smaller, 0, &hvInc);
		HTuple hvResTmp = m_tCalParam.dUnitWidth / abs(hvXCol2.D() - hvCenterCol[hvInc].D());
		m_tDoubleStripeTmpData.hvLeanRealColDiff = hvDiff*hvResTmp;

		//生成左相机像素坐标
		m_tDoubleStripeTmpData.hvLeftPixelCol = hvColLeftEffect;

		//左相机的标定区域最右侧
		m_tDoubleStripeTmpData.hvLeftMaxPixelCol = hvColLeftEffect.TupleMax().D();

		//生成左相机物理坐标
		TupleGenSequence(0, (hvColLeftEffect.Length() - 1) * m_tCalParam.dUnitWidth, m_tCalParam.dUnitWidth, &hvLeftRealCol);
		m_tDoubleStripeTmpData.hvLeftRealCol = hvLeftRealCol;
		//左相机圆点的物理坐标
		TupleFind(hvColLeftEffect, hvXCol, &hvInc);
		m_tDoubleStripeTmpData.hvLeftCircleRealCol = hvLeftRealCol[hvInc].D();

		//生成左相机区间分辨率
		HTuple hvResLeft;
		TupleDiv(m_tCalParam.dUnitWidth, hvLeftPixWidth, &hvResLeft);
		m_tDoubleStripeTmpData.hvLeftRes = hvResLeft;

		//计算左相机的整体分辨率
		m_tDoubleStripeTmpData.hvLeftWholeRes = (double)(hvColLeftEffect.Length()) * m_tCalParam.dUnitWidth / (hvColLeftEffect.TupleMax().D() - hvColLeftEffect.TupleMin().D());

		//拟合曲线
		HTuple hvX;
		CPolyFit::tagFitResult tRes;
		TupleGenSequence(0, hvResLeft.Length() - 1, 1, &hvX);
		g_pAlgorithm->m_PolyFit.BinomialFit(hvX, hvResLeft, tRes);
		tOut.hvCoefficients = tRes.hvCoefficients;
		tOut.hvR2 = tRes.dRSquare;
		tOut.hvResFit = tRes.hvFitY;
		tOut.hvRes = hvResLeft;
		tOut.hvPixelCol = hvColLeftEffect;

		//偏差百分比
		HTuple hvResDiff, hvRes1, hvRes2, hvDiffPercent;
		TupleRemove(tOut.hvRes, 0, &hvRes1);
		TupleRemove(tOut.hvRes, tOut.hvRes.Length() - 1, &hvRes2);
		TupleSub(hvRes1, hvRes2, &hvResDiff);
		TupleDiv(hvResDiff, hvRes1, &hvDiffPercent);
		tOut.hvResDiffPercent = hvDiffPercent * 100;

		HTuple hvRow1, hvRow2;
		TupleGenConst(hvColLeftEffect.Length(), hvYRow - 20, &hvRow1);
		TupleGenConst(hvColLeftEffect.Length(), hvYRow + 20, &hvRow2);
		GenCrossContourXld(&Cross, (hvRow1+ hvRow2)/2, hvColLeftEffect, 30, 0.785398);
		ConcatObj(tOut.hoDisp, Cross, &tOut.hoDisp);
		GenRegionLine(&ho_RegionLines, hvRow1, hvColLeftEffect, hvRow2, hvColLeftEffect);
		ConcatObj(tOut.hoDisp, ho_RegionLines, &tOut.hoDisp);
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CCalibration_StripeDoubleCamCalibrationFirst: 标定过程异常 %s [%s]", e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}

	return TRUE;
}

BOOL CCalibration::StripeDoubleCamCalibrationSecond(CalInput& tIn, CalOutput& tOut)
{
	HObject  ho_Region, ho_Rectangle, ho_ImageReduced, Cross, ho_RegionOpen, ho_ROI;
	HObject  ho_Rectangle1, ho_RegionLines, hoRegionRec, hoReduce;
	HObject  ho_RegionUnion, ho_ConnectedRegions, ho_RegionsTmp, ho_SelectedRegions, hoSelectedRegions1, ho_BigStripe, hoRegionDilation;
	HObject  hoImageReduced1;
	// Local control variables
	HTuple  hv_IndexStart1;
	HTuple  hv_Width;
	HTuple  hv_Height, hv_Row1, hv_Column1;
	HTuple  hv_Row2, hv_Column2, hv_Area, hv_Row, hv_Column;
	HTuple  hv_MeasureHandle, hv_RowEdge, hv_ColumnEdge, hv_Amplitude, hv_ColumnEdge2, hv_ColumnBigStripeEdge, hv_ColumnBigStripeEdge2;
	HTuple  hv_Distance, hv_Length, hv_i, hv_ii, hv_j, hv_iii, hv_PixtoDis;
	HTuple  hv_IndexStart;
	HTuple  hv_IndexTest, hv_LengthOver;
	HTuple  hv_totalresolution, hv_ColumnEdgeStart, hv_ColumnEdgeEnd, hv_RealLength;
	HTuple	hv_ImageWidth, hv_ImageHeight, hv_Width1, hv_Height1, hv_Max1, hv_BigStripeCol, hv_WidthMedian;
	HTuple  hv_Row11, hv_Column11;

	//双相机第二次标定要实现的功能是，标定右相机的黑条，将左相机黑条跟右相机对应上
	try
	{
		GetImageSize(tIn.hoImage, &hv_ImageWidth, &hv_ImageHeight);
		GenEmptyObj(&tOut.hoDisp);
		GenRectangle1(&ho_Rectangle, 0, hv_ImageWidth / 2, hv_ImageHeight - 1, hv_ImageWidth - 1);
		HTuple hvROIRow0, hvROICol0, hvROIRow1, hvROICol1;
		Intersection(ho_Rectangle, tIn.hoROI, &ho_ROI);
		SmallestRectangle1(ho_ROI, &hvROIRow0, &hvROICol0, &hvROIRow1, &hvROICol1);
		ReduceDomain(tIn.hoImage, ho_ROI, &hoReduce);
		CropDomain(hoReduce, &hoReduce);
		VarThreshold(hoReduce, &ho_Region, min(hvROICol1- hvROICol0-1, 200), min(hvROIRow1- hvROIRow0-1, 20), 0.5, m_tCalParam.nThres, "dark");
		//BinaryThreshold(hoReduce, &ho_Region, "max_separability", "dark", &HTuple());
		//AutoThreshold(hoReduce, &ho_Region, m_tCalParam.nThres);
		MoveRegion(ho_Region, &ho_Region, hvROIRow0, hvROICol0);
		Connection(ho_Region, &ho_ConnectedRegions);
		SelectShape(ho_ConnectedRegions, &ho_RegionsTmp, "height", "and", 20, (hvROIRow1 - hvROIRow0) + 1);
		SelectShape(ho_RegionsTmp, &ho_RegionsTmp, "rectangularity", "and", 0.85, 1);
		//SelectShape(ho_RegionsTmp, &ho_RegionsTmp, "anisometry", "and", 1.5, 999);

		RegionFeatures(ho_RegionsTmp, "width", &hv_Width1);
		RegionFeatures(ho_RegionsTmp, "height", &hv_Height1);

		//去除部分干扰
		Union1(ho_RegionsTmp, &ho_RegionUnion);
		ClosingRectangle1(ho_RegionUnion, &ho_RegionUnion, hv_Width1.TupleMedian().D() * 4, 1);
		ShapeTrans(ho_RegionUnion, &ho_RegionUnion, "convex");
		Intersection(ho_ConnectedRegions, ho_RegionUnion, &ho_ConnectedRegions);
		Connection(ho_ConnectedRegions, &ho_ConnectedRegions);

		//筛选圆点
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "width", "and", hv_Width1.TupleMedian().D() * m_tCalParam.dDisDownLimit,
			hv_Width1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "height", "and", hv_Height1.TupleMedian().D() / 8, hv_Height1.TupleMedian().D() / 3);
		FillUp(ho_SelectedRegions, &ho_SelectedRegions);
		if (ho_SelectedRegions.CountObj() != 1)
		{
			return FALSE;
		}
		HTuple hvHorProjection, hvVertProjection, hvIndices, hv_Phi, hv_Length1, hv_Length2, hv_Area, hv_MeasureHandle, hvColumnEdgeFirst, hvIntraDistance;
		HTuple hvYRow2, hvXCol2;
		//找右侧圆心
		HObject hoRightCicle;
		SelectObj(ho_SelectedRegions, &hoRightCicle, 1);
		DilationRectangle1(hoRightCicle, &hoRegionDilation, 5, 5);
		ShapeTrans(hoRegionDilation, &hoRegionDilation, "rectangle1");
		SmallestRectangle1(hoRegionDilation, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
		//找右侧圆心
		ReduceDomain(tIn.hoImage, hoRegionDilation, &hoImageReduced1);
		//圆心ROW坐标 ，利用投影计算，更精准的方式逐行扫描就最大宽度
		GrayProjections(hoRegionDilation, hoImageReduced1, "simple", &hvHorProjection, &hvVertProjection);
		TupleSortIndex(hvHorProjection, &hvIndices);

		hvYRow2 = hv_Row11 + hvIndices[0].D();
		//圆心COL2坐标 ，利用YROW2，做水平MEASURE
		SmallestRectangle2(hoRegionDilation, &hv_Row1, &hv_Column1, &hv_Phi, &hv_Length1, &hv_Length2);
		AreaCenter(hoRegionDilation, &hv_Area, &hv_Row, &hv_Column);

		GenMeasureRectangle2(hvYRow2, hv_Column, 0, hv_Length1 + 5, 2, hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
		MeasurePairs(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "all", "first", &HTuple(), &hvColumnEdgeFirst, &HTuple(), &HTuple(), &HTuple(), &HTuple(), &hvIntraDistance, &HTuple());
		CloseMeasure(hv_MeasureHandle);
		//这里需要判断下有么有值
		hvXCol2 = hvColumnEdgeFirst + hvIntraDistance[0] / 2.0;


		//筛选黑条
		SelectShape(ho_ConnectedRegions, &hoSelectedRegions1, "width", "and", hv_Width1.TupleMedian().D() * m_tCalParam.dDisDownLimit,
			hv_Width1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		SelectShape(hoSelectedRegions1, &hoSelectedRegions1, "height", "and", hv_Height1.TupleMedian().D() * m_tCalParam.dDisDownLimit,
			hv_Height1.TupleMedian().D() * m_tCalParam.dDisUpLimit);
		//SelectShape(hoSelectedRegions1, &hoSelectedRegions1, HTuple("column").Append("column"), "or", HTuple(0).Append(hv_BigStripeCol.TupleMax()), HTuple(hv_BigStripeCol.TupleMin()).Append(hv_ImageWidth - 1));
		Union1(hoSelectedRegions1, &ho_RegionUnion);
		Connection(ho_RegionUnion, &ho_ConnectedRegions);
		SortRegion(ho_ConnectedRegions, &ho_ConnectedRegions, "upper_left", "true", "column");

		HTuple hvGreater, hvIndex, hvMesureCol1;
		SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		//右相机第一个黑粗条挨着相机的图像左边，就去掉
		if (hv_Column1[0] <= hv_ImageWidth / 2+1)
		{
			RemoveObj(ho_ConnectedRegions, &ho_ConnectedRegions, 1);
			SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		}
		
		//所有细黑条左边的点
		hv_ColumnEdge.Clear();
		for (int i = 0; i < hv_Column1.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2(hvYRow2, hv_Column1[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "negative", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge.Append(hvColEdg);
		}

		//所有细黑条右边的点
		hv_ColumnEdge2.Clear();
		for (int i = 0; i < hv_Column2.Length(); i++)
		{
			HTuple hvRowEdg, hvColEdg;
			GenMeasureRectangle2(hvYRow2, hv_Column2[i], 0, (hv_Column2[i] - hv_Column1[i]) / 4, 20,
				hv_ImageWidth, hv_ImageHeight, "nearest_neighbor", &hv_MeasureHandle);
			MeasurePos(tIn.hoImage, hv_MeasureHandle, 2, m_tCalParam.nMeasureThres, "positive", "first", &hvRowEdg,
				&hvColEdg, &HTuple(), &HTuple());
			CloseMeasure(hv_MeasureHandle);
			if (hvColEdg.Length() > 0)
				hv_ColumnEdge2.Append(hvColEdg);
		}
		if (hv_ColumnEdge.Length() != hv_ColumnEdge2.Length())
		{
			CString strExcp;
			strExcp.Format("搜索到的黑条左右两边点数量不一致，请检查");
			return FALSE;
		}

		//细黑条左右两边求中心点
		HTuple hvSum, hvCenterCol;
		TupleAdd(hv_ColumnEdge, hv_ColumnEdge2, &hvSum);
		TupleDiv(hvSum, 2, &hvCenterCol);

		//筛选右相机有效的点，包含屏蔽区域区域内的点
		hvCenterCol.Append(hvXCol2);
		TupleSort(hvCenterCol, &hvCenterCol);
		HTuple hv_Greater, hv_Smaller, hvOr, hvInc, hvColRightEffect, hvCol00, hvCol11, hvLeftPixWidth, hvRightPixWidth, hvRightRealCol;
		TupleGreaterEqualElem(hvCenterCol, hv_ImageWidth / 2, &hv_Greater);
		TupleFind(hv_Greater, 1, &hvInc);
		TupleSelect(hvCenterCol, hvInc, &hvColRightEffect);

		//判断点找的对不对
		TupleRemove(hvColRightEffect, 0, &hvCol00);
		TupleRemove(hvColRightEffect, hvColRightEffect.Length() - 1, &hvCol11);
		TupleSub(hvCol00, hvCol11, &hvRightPixWidth);
		if (hvRightPixWidth.TupleMax() / hvRightPixWidth.TupleMin() > 1.5)
		{
			CString strExcp;
			strExcp.Format("右相机区间点不均匀");
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
			return FALSE;
		}

		//计算第一次标定的右相机圆点位置和当前黑条的中心点物理偏差，生成物理坐标
		//先找到跟圆点中心最接近的黑条中心
		HTuple hvCircleLeftStripeCol, hvCircleRightStripeCol, hvPixelDiff, hvInc1;
		//先生成单个相机内的物理坐标，0开始
		TupleGenSequence(0, (hvColRightEffect.Length() - 1) * m_tCalParam.dUnitWidth, m_tCalParam.dUnitWidth, &hvRightRealCol);
		TupleGreaterEqualElem(hvColRightEffect, m_tDoubleStripeTmpData.hvRightCirclePixelCol, &hv_Greater);
		TupleFindFirst(hv_Greater, 1, &hvInc1);
		TupleSelect(hvColRightEffect, hvInc1, &hvCircleRightStripeCol);
		hvCircleLeftStripeCol = hvColRightEffect[hvInc1 - 1];
		//计算黑点和右侧黑条物理位置差
		HTuple hvCompenReal = m_tCalParam.dUnitWidth / (hvCircleRightStripeCol.D() - hvCircleLeftStripeCol.D()) * (hvCircleRightStripeCol.D() - m_tDoubleStripeTmpData.hvRightCirclePixelCol.D());
		//计算右相机第一个黑条与第一次拍图的圆点中心的物理位置差，加上第一次标定计算出的两个相机夹角造成的同一个圆点的物理位置差
		HTuple hvRealDiff = m_tDoubleStripeTmpData.hvLeftCircleRealCol - hvRightRealCol[hvInc1].D()
			+ hvCompenReal + m_tDoubleStripeTmpData.hvLeanRealColDiff;
		HTuple hvRightRealColFinal;
		TupleAdd(hvRightRealCol, hvRealDiff, &hvRightRealColFinal);

		//生成左右相机的统一物理坐标
		m_tStripeParam.hvRealCol.Clear();
		m_tStripeParam.hvRealCol.Append(m_tDoubleStripeTmpData.hvLeftRealCol);
		m_tStripeParam.hvRealCol.Append(hvRightRealColFinal);

		//生成左右相机的统一像素坐标
		m_tStripeParam.hvPixelCol.Clear();
		m_tStripeParam.hvPixelCol.Append(m_tDoubleStripeTmpData.hvLeftPixelCol);
		m_tStripeParam.hvPixelCol.Append(hvColRightEffect);

		//生成左相机标定区域最右边的像素坐标，右相机标定区域最左边的像素坐标
		m_tStripeParam.hvLeftRightColumnOver.Clear();
		m_tStripeParam.hvLeftRightColumnOver.Append(m_tDoubleStripeTmpData.hvLeftMaxPixelCol);
		m_tStripeParam.hvLeftRightColumnOver.Append(hvColRightEffect.TupleMin().D());

		//生成左右相机的标定条区间分辨率
		HTuple hvResRight;
		TupleDiv(m_tCalParam.dUnitWidth, hvRightPixWidth, &hvResRight);
		m_tStripeParam.hvRes.Clear();
		m_tStripeParam.hvRes.Append(m_tDoubleStripeTmpData.hvLeftRes);
		m_tStripeParam.hvRes.Append((m_tDoubleStripeTmpData.hvLeftRes[m_tDoubleStripeTmpData.hvLeftRes.Length() - 1].D() + hvResRight[0].D()) / 2);//两个相机中间未标定的区间分辨率用均值赋值，正常用不到，只用来占位
		m_tStripeParam.hvRes.Append(hvResRight);

		//计算左右两边相机的分辨率
		if (m_vecResolution.size() < 2) m_vecResolution.resize(2, m_vecResolution[0]);
		m_vecResolution[0].dColRes = m_tDoubleStripeTmpData.hvLeftWholeRes.D();
		m_vecResolution[1].dColRes = (double)hvColRightEffect.Length() * m_tCalParam.dUnitWidth / (hvColRightEffect.TupleMax().D() - hvColRightEffect.TupleMin().D());

		//屏蔽区域两边的坐标
		m_tStripeParam.hvColumnOver.Clear();
		m_tStripeParam.hvColumnOver.Append(m_tDoubleStripeTmpData.hvLeftCirclePixelCol);
		m_tStripeParam.hvColumnOver.Append(m_tDoubleStripeTmpData.hvRightCirclePixelCol);

		//相机交界处误差初始默认为0
		m_tStripeParam.dInternOffset = 0;

		//拟合曲线
		HTuple hvX;
		CPolyFit::tagFitResult tRes;
		TupleGenSequence(0, hvResRight.Length() - 1, 1, &hvX);
		g_pAlgorithm->m_PolyFit.BinomialFit(hvX, hvResRight, tRes);
		tOut.hvCoefficients = tRes.hvCoefficients;
		tOut.hvR2 = tRes.dRSquare;
		tOut.hvResFit = tRes.hvFitY;
		tOut.hvRes = hvResRight;
		tOut.hvPixelCol = hvColRightEffect;

		//偏差百分比
		HTuple hvResDiff, hvRes1, hvRes2, hvDiffPercent;
		TupleRemove(tOut.hvRes, 0, &hvRes1);
		TupleRemove(tOut.hvRes, tOut.hvRes.Length() - 1, &hvRes2);
		TupleSub(hvRes1, hvRes2, &hvResDiff);
		TupleDiv(hvResDiff, hvRes1, &hvDiffPercent);
		tOut.hvResDiffPercent = hvDiffPercent * 100;

		HTuple hvRow1, hvRow2;
		TupleGenConst(m_tStripeParam.hvPixelCol.Length(), hvYRow2 - 20, &hvRow1);
		TupleGenConst(m_tStripeParam.hvPixelCol.Length(), hvYRow2 + 20, &hvRow2);
		GenCrossContourXld(&Cross, (hvRow1 + hvRow2) / 2, m_tStripeParam.hvPixelCol, 30, 0.785398);
		ConcatObj(tOut.hoDisp, Cross, &tOut.hoDisp);
		GenRegionLine(&ho_RegionLines, hvRow1, m_tStripeParam.hvPixelCol, hvRow2, m_tStripeParam.hvPixelCol);
		ConcatObj(tOut.hoDisp, ho_RegionLines, &tOut.hoDisp);
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CCalibration_StripeCalibration: 标定过程异常 %s [%s]", e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}


	return TRUE;
}

BOOL CCalibration::CalDoubleCamDistancePP(CalDisPPInput& pIn, CalDisPPOutput& pOut)
{
	// Local control variables
	HTuple  hv_Indics1, hv_Indics2;
	HTuple  hv_DistanceWorld;

	if (pIn.hvCol1 == NO_DATA || pIn.hvCol2 == NO_DATA)
	{
		pOut.Dist = NO_DATA;
		return FALSE;
	}

	//判断是否有两个屏蔽区域的边坐标
	if (m_tStripeParam.hvColumnOver.Length() != 2)
	{
		CString strExcp;
		strExcp.Format("CCalibration_CalDoubleCamDistancePP: 算法%d计算屏蔽区域无效，不是2个边", m_tInit.nSN);
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		pOut.Dist = NO_DATA;
		return FALSE;
	}
	//判断是否有点落在了屏蔽区域
	if ((pIn.hvCol1 > m_tStripeParam.hvColumnOver.TupleMin().D() && pIn.hvCol1 < m_tStripeParam.hvColumnOver.TupleMax().D()) ||
		(pIn.hvCol2 > m_tStripeParam.hvColumnOver.TupleMin().D() && pIn.hvCol2 < m_tStripeParam.hvColumnOver.TupleMax().D()))
	{
		CString strExcp;
		strExcp.Format("CCalibration_CalDoubleCamDistancePP: 算法%d,计算点属于屏蔽区域", m_tInit.nSN);
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		pOut.Dist = NO_DATA;
		return FALSE;
	}
	//计算的是不是在标定的有效范围内
	if ((pIn.hvCol1 > m_tStripeParam.hvPixelCol.TupleMax() || pIn.hvCol1 < m_tStripeParam.hvPixelCol.TupleMin()) ||
		(pIn.hvCol2 > m_tStripeParam.hvPixelCol.TupleMax() || pIn.hvCol2 < m_tStripeParam.hvPixelCol.TupleMin()))
	{
		CString strExcp;
		strExcp.Format("CCalibration_CalDoubleCamDistancePP: 算法%d超出像素校准区域", m_tInit.nSN);
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		pOut.Dist = NO_DATA;
		return FALSE;
	}

	HTuple hvGreater, hvCol1, hvCol2;
	//找到屏蔽区域的边序号
	HTuple hvColumnLeftOverIndex(-1), hvColumnRightOverIndex(-1);
	TupleGreaterEqualElem(m_tStripeParam.hvPixelCol, m_tStripeParam.hvColumnOver.TupleMin(), &hvGreater);
	TupleFindFirst(hvGreater, 1, &hv_Indics1);
	hvColumnLeftOverIndex = hv_Indics1.TupleMin();
	TupleGreaterEqualElem(m_tStripeParam.hvPixelCol, m_tStripeParam.hvColumnOver.TupleMax(), &hvGreater);
	TupleFindFirst(hvGreater, 1, &hv_Indics1);
	hvColumnRightOverIndex = hv_Indics1.TupleMax();

	//保证为左为小坐标，右为大的坐标
	hvCol1 = min(pIn.hvCol1.D(), pIn.hvCol2.D());
	hvCol2 = max(pIn.hvCol1.D(), pIn.hvCol2.D());
	TupleGreaterEqualElem(m_tStripeParam.hvPixelCol, hvCol1, &hvGreater);
	TupleFindFirst(hvGreater, 1, &hv_Indics1);
	TupleGreaterEqualElem(m_tStripeParam.hvPixelCol, hvCol2, &hvGreater);
	TupleFindFirst(hvGreater, 1, &hv_Indics2);

	//计算两点之间的距离
	if ((hvCol1 <= m_tStripeParam.hvColumnOver.TupleMin() && hvCol2 <= m_tStripeParam.hvColumnOver.TupleMin()) ||
		(hvCol1 >= m_tStripeParam.hvColumnOver.TupleMax() && hvCol2 >= m_tStripeParam.hvColumnOver.TupleMax()))
	{
		//两个点都在左相机视野，或者两个点都在右相机视野
		double dKValue = 1;
		if (hv_Indics1 <= hvColumnLeftOverIndex && hv_Indics2 <= hvColumnLeftOverIndex)
			dKValue = m_tStripeParam.vecKValue[0];
		else if (m_tStripeParam.vecKValue.size() > 1)
			dKValue = m_tStripeParam.vecKValue[1];

		hv_DistanceWorld = (m_tStripeParam.hvRealCol[hv_Indics2].D() - m_tStripeParam.hvRealCol[hv_Indics1].D())
			+ ((m_tStripeParam.hvPixelCol[hv_Indics1].D() - hvCol1.D())*m_tStripeParam.hvRes[hv_Indics1 - 1].D())
			- ((m_tStripeParam.hvPixelCol[hv_Indics2].D() - hvCol2.D())*m_tStripeParam.hvRes[hv_Indics2 - 1].D());
		pOut.Dist = abs(hv_DistanceWorld.D() * dKValue);

		/*hv_DistanceWorld = (hv_Indics2 - hv_Indics1 - 1) * m_tCalParam.dUnitWidth
			+ (m_tStripeParam.hvPixelCol[hv_Indics1].D() - hvCol1.D())*m_tStripeParam.hvRes[hv_Indics1 - 1].D()
			+ (hvCol2.D() - m_tStripeParam.hvPixelCol[hv_Indics2 - 1].D())*m_tStripeParam.hvRes[hv_Indics2 - 1].D();*/

	}
	else if (hvCol1 < m_tStripeParam.hvColumnOver.TupleMin() && hvCol2 > m_tStripeParam.hvColumnOver.TupleMax())
	{
		//两个点分别在左右相机的视野，分别计算两个点到屏蔽区域的两边再求和
		HTuple hvLeftDist(0), hvRightDist(0);// hvColumnLeftOverIndex
		hvLeftDist = (m_tStripeParam.hvRealCol[hvColumnLeftOverIndex].D() - m_tStripeParam.hvRealCol[hv_Indics1].D())
			+ (m_tStripeParam.hvPixelCol[hv_Indics1] - hvCol1.D())*m_tStripeParam.hvRes[hv_Indics1 - 1].D();

		HTuple hv1 = (m_tStripeParam.hvRealCol[hv_Indics2].D() - m_tStripeParam.hvRealCol[hvColumnRightOverIndex].D());
		HTuple hv2 = ((m_tStripeParam.hvPixelCol[hv_Indics2].D() - hvCol2.D())*m_tStripeParam.hvRes[hv_Indics2 - 1].D());
		hvRightDist = (m_tStripeParam.hvRealCol[hv_Indics2].D() - m_tStripeParam.hvRealCol[hvColumnRightOverIndex].D())
			- ((m_tStripeParam.hvPixelCol[hv_Indics2].D() - hvCol2.D())*m_tStripeParam.hvRes[hv_Indics2 - 1].D());

		//if (hv_Indics2 > hvColumnRightOverIndex + 1)
		//{
		//	//右边在粗条区域右边
		//	hvRightDist = m_tCalParam.dUnitWidth * 2
		//		+ (hv_Indics2 - 1 - hvColumnRightOverIndex - 1) * m_tCalParam.dUnitWidth
		//		+ (hvCol2.D() - m_tStripeParam.hvPixelCol[hv_Indics2 - 1].D())*m_tStripeParam.hvRes[hv_Indics2 - 1].D();
		//}
		//else
		//{
		//	//右边在粗条区域里面
		//	hvRightDist = (hvCol2.D() - m_tStripeParam.hvColumnOver.TupleMax().D())*m_tStripeParam.hvRes[hv_Indics2 - 1].D();
		//}
		hv_DistanceWorld = abs(hvLeftDist.D() *m_tStripeParam.vecKValue[0]) + abs(hvRightDist.D()*m_tStripeParam.vecKValue[1]);
		pOut.Dist = hv_DistanceWorld.D();
	}
	else
	{
		CString strExcp;
		strExcp.Format("CCalibration_CalDoubleCamDistancePP: 算法%d，计算的两个点超出计算区域", m_tInit.nSN);
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
	}

	return TRUE;
}

BOOL CCalibration::_GetLineAngle(int& nSN,HTuple& hvStartRow,HTuple& hvStartCol,HTuple& hvEndRow,HTuple& hvEndCol,HTuple* hvAngle)
{
	if(m_tInit.hvCamSNList.Length() == 1 && nSN>0) return FALSE;
	if(m_tInit.hvCamSNList.Length() == 2 && nSN>1) return FALSE;

	double dResX = m_vecResolution[nSN].dColRes;
	double dResY = m_vecResolution[nSN].dRowRes;

	try
	{
		HTuple hvWX1,hvWX2,hvWY1,hvWY2;
		if(!CalPoint(hvStartCol, hvWX1)) return FALSE;
		if(!CalPoint(hvEndCol, hvWX2)) return FALSE;
		hvWY1=0.0;
		hvWY2=(hvEndRow-hvStartRow)*dResY;
		HTuple hvWk=(hvWY2-hvWY1)/(hvWX2-hvWX1);
		HTuple hvPhi,hvAbs,hvAbs2,hvDiff,hvCos;
		//*********角度转换
		LineOrientation ( hvWY2, hvWX2,hvWY1, hvWX1, &hvPhi);
		TupleFabs (hvPhi, &hvAbs);
		TupleSub (hvAbs, PI/2, &hvDiff);
		TupleFabs (hvDiff, &hvAbs2);
		//*********角度判断,halcon里应该有个设置，角度好像可以从系统设置里是0-360，还是-180到180
		//********这里角度的大小方向可能会有问题，需要测试
		TupleCos (hvAbs2, &hvCos);

		*hvAngle = hvCos;
	}
	catch(HException&e)
	{
		return FALSE;
	}
	
   return TRUE;
}