#include "StdAfx.h"
#include "LineMeasure.h"

CLineMeasure::CLineMeasure(void)
{
	SetSystem("store_empty_region", "false");
}

CLineMeasure::~CLineMeasure(void)
{

}

BOOL CLineMeasure::IsVertical(HObject Region)
{
	HTuple hv_Phi, hv_Deg;
	OrientationRegion(Region, &hv_Phi);
	TupleDeg(hv_Phi, &hv_Deg);
	if (0 != (HTuple(HTuple(hv_Deg > 45).TupleAnd(hv_Deg < 135)).TupleOr(HTuple(hv_Deg > -135).TupleAnd(hv_Deg < -45))))
	{
		return TRUE;
	}
	return FALSE;
}

void CLineMeasure::GetLLInterSectionPoints(HTuple hvIndex, HTuple hv_CutTileImageWidth, HTuple hv_CutTileImageHeight)//极耳或者涂布的四个角点
{
	Hlong nLength = hvIndex.Length();
	HTuple hv_Height, hv_Width;
	hv_Width = hv_CutTileImageWidth;
	hv_Height = hv_CutTileImageHeight;

	for (Hlong i = 0; i < nLength; i++)
	{
		int nIndex, nIndex1, nIndex2;//该序号前一个以及后一个
		nIndex = hvIndex[i];
		if (0 == i)//第一个点
		{
			nIndex1 = hvIndex[nLength - 1];
			nIndex2 = hvIndex[i + 1];
		}
		else if (nLength - 1 == i)//最后一个点
		{
			nIndex1 = hvIndex[i - 1];
			nIndex2 = hvIndex[0];
		}
		else//中间的点
		{
			nIndex1 = hvIndex[i - 1];
			nIndex2 = hvIndex[i + 1];
		}
		HTuple Row1, Row2, Column1, Column2;
		IntersectionLl(m_hvRowBegin[nIndex], m_hvColBegin[nIndex], m_hvRowEnd[nIndex], m_hvColEnd[nIndex], m_hvRowBegin[nIndex1], m_hvColBegin[nIndex1], m_hvRowEnd[nIndex1], m_hvColEnd[nIndex1],
			&Row1, &Column1, NULL);
		IntersectionLl(m_hvRowBegin[nIndex], m_hvColBegin[nIndex], m_hvRowEnd[nIndex], m_hvColEnd[nIndex], m_hvRowBegin[nIndex2], m_hvColBegin[nIndex2], m_hvRowEnd[nIndex2], m_hvColEnd[nIndex2],
			&Row2, &Column2, NULL);

		m_hvRowBegin[nIndex] = MIN(MAX(Row1[0].D(), 0), hv_Height[0].D() - 1);
		m_hvColBegin[nIndex] = MIN(MAX(Column1[0].D(), 0), hv_Width[0].D() - 1);
		m_hvRowEnd[nIndex] = MIN(MAX(Row2[0].D(), 0), hv_Height[0].D() - 1);
		m_hvColEnd[nIndex] = MIN(MAX(Column2[0].D(), 0), hv_Width[0].D() - 1);
	}
}

int CLineMeasure::_FindLine(const HObject& Image, const int nSN, const tagLineParam& FitLinePara, HObject* Line, double* dStartY, double* dStartX, double* dEndY, double* dEndX, int nLineDisplay)
{
	try
	{
#pragma region "先对拟合方式进行判断，如果不是正常拟合直接就返回了"
		HTuple Width, Height;
		GetImageSize(Image, &Width, &Height);
		tagLineParam TempFitLinePara;
		TempFitLinePara = FitLinePara;

		//TempFitLinePara.dStartRow = MAX(Height.I() / 8, MIN(TempFitLinePara.dStartRow, Height.I() / 8 * 7));
		//TempFitLinePara.dEndRow = MAX(Height.I() / 8, MIN(TempFitLinePara.dEndRow, Height.I() / 8 * 7));
		TempFitLinePara.dStartCol = MAX(0, MIN(TempFitLinePara.dStartCol, Width.I()));
		TempFitLinePara.dEndCol = MAX(0, MIN(TempFitLinePara.dEndCol, Width.I()));
		if(TempFitLinePara.nDirection == MD_Left2Right)
		{
			TempFitLinePara.dStartRow = Height.I() / 4 * 3;
			TempFitLinePara.dEndRow = Height.I() / 4;
		}
		else
		{
			TempFitLinePara.dStartRow = Height.I() / 4;
			TempFitLinePara.dEndRow = Height.I() / 4 * 3;
		}

#pragma endregion 
		// Local iconic variables 
		HObject  Rectangle, Contour;
		// Local control variables 
		HTuple  ResultRow, ResultCol;
		HTuple  MsrHandle_Measure, transition, select;
		HTuple  RowBegin, ColBegin, RowEnd, ColEnd;
		Line->GenEmptyObj();
		HObject hMeasureRect, hCross;

		if (0 != (HTuple(TempFitLinePara.nTransition) == MT_All))
		{
			transition = "all";
		}
		else if (0 != (HTuple(TempFitLinePara.nTransition) == MT_Dark2Light))
		{
			transition = "positive";
		}
		else
		{
			transition = "negative";
		}
		if (0 != (HTuple(TempFitLinePara.nSelect) == MS_First))
		{
			select = "first";
		}
		else if (0 != (HTuple(TempFitLinePara.nSelect) == MS_All))
		{
			select = "all";
		}
		else
		{
			select = "last";
		}
		if (TempFitLinePara.nDirection == MD_Left2Right)
		{
			RowBegin = max(TempFitLinePara.dEndRow, TempFitLinePara.dStartRow);
			RowEnd = min(TempFitLinePara.dEndRow, TempFitLinePara.dStartRow);
			ColBegin = TempFitLinePara.dStartCol;
			ColEnd = TempFitLinePara.dEndCol;
		}
		else
		{
			RowBegin = min(TempFitLinePara.dEndRow, TempFitLinePara.dStartRow);
			RowEnd = max(TempFitLinePara.dEndRow, TempFitLinePara.dStartRow);
			ColBegin = TempFitLinePara.dStartCol;
			ColEnd = TempFitLinePara.dEndCol;
		}
		HTuple hv_MetrologyHdl, hv_LineIndex;
		try
		{
			//先抠图再测量，测试
			/*HObject hoCrop;
			HTuple hvRow0, hvRow1, hvCol0, hvCol1;
			CropRectangle1(Image, &hoCrop, min(RowBegin, RowEnd), ColBegin- TempFitLinePara.dWidth / 2, max(RowBegin, RowEnd), ColBegin + TempFitLinePara.dWidth / 2);

			CreateMetrologyModel(&hv_MetrologyHdl);
			SetMetrologyModelImageSize(hv_MetrologyHdl, (int)TempFitLinePara.dWidth, abs(RowEnd.D() - RowBegin.D()));
			AddMetrologyObjectLineMeasure(hv_MetrologyHdl, (RowEnd.D() - RowBegin.D()) > 0 ? 0 : abs(RowEnd.D() - RowBegin.D()) - 1, TempFitLinePara.dWidth / 2,
				(RowEnd.D() - RowBegin.D()) > 0 ? abs(RowEnd.D() - RowBegin.D()) - 1 : 0, TempFitLinePara.dWidth / 2, TempFitLinePara.dWidth / 2, TempFitLinePara.dHeight,
				TempFitLinePara.dSigma, TempFitLinePara.nThr, "num_measures", TempFitLinePara.nMeasureNum, &hv_LineIndex);
			SetMetrologyObjectParam(hv_MetrologyHdl, hv_LineIndex, "min_score", 0.4);
			SetMetrologyObjectParam(hv_MetrologyHdl, hv_LineIndex, "measure_transition", transition);
			SetMetrologyObjectParam(hv_MetrologyHdl, hv_LineIndex, "measure_select", select);
			ApplyMetrologyModel(hoCrop, hv_MetrologyHdl);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "row_begin", &hvRow0);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "column_begin", &hvCol0);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "row_end", &hvRow1);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "column_end", &hvCol1);

			RowBegin = hvRow0 + min(RowBegin, RowEnd);
			RowEnd = hvRow1 + min(RowBegin, RowEnd);
			ColEnd = hvCol1 + ColBegin - TempFitLinePara.dWidth / 2;
			ColBegin = hvCol0 + ColBegin - TempFitLinePara.dWidth / 2;*/
			
			CreateMetrologyModel(&hv_MetrologyHdl);
			SetMetrologyModelImageSize(hv_MetrologyHdl, Width, Height);
			AddMetrologyObjectLineMeasure(hv_MetrologyHdl, RowBegin, ColBegin,
				RowEnd, ColEnd, TempFitLinePara.dWidth / 2, TempFitLinePara.dHeight,
				TempFitLinePara.dSigma, TempFitLinePara.nThr, "num_measures", TempFitLinePara.nMeasureNum, &hv_LineIndex);
			SetMetrologyObjectParam(hv_MetrologyHdl, hv_LineIndex, "min_score", 0.4);
			SetMetrologyObjectParam(hv_MetrologyHdl, hv_LineIndex, "measure_transition", transition);
			SetMetrologyObjectParam(hv_MetrologyHdl, hv_LineIndex, "measure_select", select);
			ApplyMetrologyModel(Image, hv_MetrologyHdl);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "row_begin", &RowBegin);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "column_begin", &ColBegin);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "row_end", &RowEnd);
			GetMetrologyObjectResult(hv_MetrologyHdl, "all", "all", "result_type", "column_end", &ColEnd);
			HTuple hvMeasureRow, hvMeasureCol;
			if (nLineDisplay)
			{
				GetMetrologyObjectMeasures(&hMeasureRect, hv_MetrologyHdl, "all", "all", &hvMeasureRow, &hvMeasureCol);
				ConcatObj(*Line, hMeasureRect, &(*Line));
				GenCrossContourXld(&hCross, hvMeasureRow, hvMeasureCol, 25, 0.78);
				ConcatObj(*Line, hCross, Line);
			}
			HObject hLine;
			if (RowBegin.Length() > 0)
			{
				GetMetrologyObjectResultContour(&hLine, hv_MetrologyHdl, "all", "all", 1);
				ConcatObj(*Line, hLine, Line);
				*dStartY = RowBegin[0].D();
				*dStartX = ColBegin[0].D();
				*dEndY = RowEnd[0].D();
				*dEndX = ColEnd[0].D();
			}
			else//如果没找到线，就是用原来的位置
			{
				*dStartY = TempFitLinePara.dStartRow;
				*dStartX = TempFitLinePara.dStartCol;
				*dEndY = TempFitLinePara.dEndRow;
				*dEndX = TempFitLinePara.dEndCol;
				return FALSE;
			}
		}
		catch (HException& e)
		{
			HObject hLine;
			GenContourPolygonXld(&hLine, HTuple(TempFitLinePara.dStartRow).TupleConcat(TempFitLinePara.dEndRow),
				HTuple(TempFitLinePara.dStartCol).TupleConcat(TempFitLinePara.dEndCol));
			ConcatObj(*Line, hLine, Line);
			*dStartY = TempFitLinePara.dStartRow;
			*dStartX = TempFitLinePara.dStartCol;
			*dEndY = TempFitLinePara.dEndRow;
			*dEndX = TempFitLinePara.dEndCol;

			CString strExcp;
			strExcp.Format("CLineMeasure__FindLine: 算法%d测量第%d条边时异常 (%s [%s])", m_tInit.nSN, nSN, e.ErrorMessage().Text(), e.ProcName().Text());
			m_tInit.pLog(strExcp, ExcpLog, SlightExcp);

			return FALSE;
		}

		return TRUE;

	}
	catch (...)
	{
		return FALSE;
	}
}

void CLineMeasure::GenRegion(HObject* Region, HTuple hvIndex)
{
	Hlong nLength = hvIndex.Length();
	HTuple RowBegin, ColBegin, RowEnd, ColEnd;

	for (Hlong i = 0; i < nLength; i++)
	{
		RowBegin.Append(m_hvRowBegin[hvIndex[i].I()]);
		ColBegin.Append(m_hvColBegin[hvIndex[i].I()]);
		RowEnd.Append(m_hvRowEnd[hvIndex[i].I()]);
		ColEnd.Append(m_hvColEnd[hvIndex[i].I()]);
	}

	HObject RegionLines, RegionUnion, RegionClosing, RegionFillUp;
	GenRegionLine(&RegionLines, RowBegin, ColBegin, RowEnd, ColEnd);
	Union1(RegionLines, &RegionUnion);
	ClosingRectangle1(RegionUnion, &RegionClosing, 100, 100);
	FillUp(RegionClosing, Region);
}

void CLineMeasure::Init(MeasureInit &tIn)
{
	m_tInit = tIn;

	m_strMyConfig.Format("%s//Config//Products//%s//SCIMeasure//%d//Config.ini", GetCurrentAppPath(), m_tInit.tProductInfo.strPDName, m_tInit.nSN);
	m_strSystemVis.Format("%s//Config//System.vis", GetCurrentAppPath());

	//根据产品类型初始化线
	GenDefultLineProp();
}

void CLineMeasure::GenDefultLineProp()
{
	m_tMeasureParam.tLineProp.hv_CamSNList.Clear();
	m_tMeasureParam.tLineProp.hv_SNList.Clear();
	m_tMeasureParam.tLineProp.vecLineTypeList.clear();

	if (m_tInit.tProductInfo.nPDType != ThreeCavity)
	{
		//0边背景箔材边
		int nCoatCount = m_tInit.tProductInfo.nPDType;
		m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_BgFoil);
		for (int i = 0; i < nCoatCount; i++)
		{
			if (m_tInit.tProductInfo.bExistAT11 || m_tInit.tProductInfo.bExistBaseCoat)
			{
				//箔材和AT11/底涂边
				if (m_tInit.tProductInfo.bExistAT11)
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_FoilAT11);
				else
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_FoilBaseCoat);
				//AT11/底涂和涂布边
				if (m_tInit.tProductInfo.bExistAT11)
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_AT11Coat);
				else
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_BaseCoatCoat);
				//涂布和底涂/AT11边
				if (m_tInit.tProductInfo.bExistAT11)
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_CoatAT11);
				else
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_CoatBaseCoat);
				//底涂/AT11和箔材边
				if (m_tInit.tProductInfo.bExistAT11)
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_AT11Foil);
				else
					m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_BaseCoatFoil);
			}
			else
			{
				//箔材涂布
				m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_FoilCoat);
				//涂布箔材
				m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_CoatFoil);
			}
		}
		//倒数第1个边，箔材背景边
		m_tMeasureParam.tLineProp.vecLineTypeList.push_back(LT_FoilBg);
	}

	for (int i = 0; i < m_tInit.tProductInfo.nMeasureLineNum; i++)
	{
		m_tMeasureParam.tLineProp.hv_SNList.Append(i);
		m_tMeasureParam.tLineProp.hv_CamSNList.Append(0);
	}
}

BOOL CLineMeasure::ReadParam()
{
	m_tMeasureParam.vecLineParam.clear();
	for (int i = 0; i < m_tInit.tProductInfo.nMeasureLineNum; i++)
	{
		m_tMeasureParam.vecLineParam.push_back(ReadLineParam(i));
	}

	m_tMeasureParam.dMaxLeanDiff = GetPrivateProfileDouble("LineMeasure", "dMaxLeanDiff", 100, m_strMyConfig);

	return TRUE;
}

BOOL CLineMeasure::WriteParam()
{
	for (int i = 0; i < m_tMeasureParam.vecLineParam.size(); i++)
	{
		WriteLineParam(i, m_tMeasureParam.vecLineParam[i]);
	}

	WritePrivateProfileDouble("LineMeasure", "dMaxLeanDiff", m_tMeasureParam.dMaxLeanDiff, m_strMyConfig);

	return TRUE;
}

void CLineMeasure::InitOutputValue()
{
	try
	{
		GenEmptyObj(&m_ptOut->hoDisp);

		m_ptOut->hv_LineNum = m_tInit.tProductInfo.nMeasureLineNum;
		m_ptOut->tLine.hv_EndColumn.Clear();
		m_ptOut->tLine.hv_EndRow.Clear();
		m_ptOut->tLine.hv_StartRow.Clear();
		m_ptOut->tLine.hv_StartColumn.Clear();
		m_ptOut->hvErrorLineSN.Clear();
		m_hvErrorLineIndex.Clear();

		m_dMeasureLineMoveDiff = 0;
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CLineMeasure_InitOutputValue: 算法%d输出结果初始化时异常 (%s [%s])", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
	}
}

BOOL CLineMeasure::Action(LineMeasureInput& tIn, LineMeasureOutput& tOut)
{
	//测量执行
	m_ptOut = &tOut;
	m_ptIn = &tIn;

	//初始化输出变量
	InitOutputValue();
	//测量所有线
	if (!FitAllLines()) return FALSE;

	return TRUE;
}

BOOL CLineMeasure::FindSingleLine(int nSN, LineMeasureInput& tIn, LineMeasureOutput& tOut, BOOL bDisp)
{
	//测量执行
	if (m_tMeasureParam.vecLineParam.size() != m_tInit.tProductInfo.nMeasureLineNum) return FALSE;

	tOut.tLine.hv_StartRow.Clear();
	tOut.tLine.hv_StartColumn.Clear();
	tOut.tLine.hv_EndRow.Clear();
	tOut.tLine.hv_EndColumn.Clear();
	GenEmptyObj(&tOut.hoDisp);
	tOut.hv_LineNum = 0;
	try
	{
		tagLineParam tLineParam = m_tMeasureParam.vecLineParam[nSN];
		HObject hoL;
		double dStartRow(0.0), dStartColumn(0.0), dEndRow(0.0), dEndColumn(0.0);
		if (FALSE == _FindLine(tIn.hoImage, nSN, tLineParam, &hoL, &dStartRow, &dStartColumn, &dEndRow, &dEndColumn, bDisp))
		{
			//测量线失败
			m_hvRowBegin[nSN] = m_tMeasureParam.vecLineParam[nSN].dStartRow;
			m_hvColBegin[nSN] = m_tMeasureParam.vecLineParam[nSN].dStartCol;
			m_hvRowEnd[nSN] = m_tMeasureParam.vecLineParam[nSN].dEndRow;
			m_hvColEnd[nSN] = m_tMeasureParam.vecLineParam[nSN].dEndCol;
		}
		else
		{
			if (fabs(dStartColumn - dEndColumn) > m_tMeasureParam.dMaxLeanDiff)//如果是起始终止列小于若干列则认为寻边失败
			{
				m_hvRowBegin[nSN] = m_tMeasureParam.vecLineParam[nSN].dStartRow;
				m_hvColBegin[nSN] = m_tMeasureParam.vecLineParam[nSN].dStartCol;
				m_hvRowEnd[nSN] = m_tMeasureParam.vecLineParam[nSN].dEndRow;
				m_hvColEnd[nSN] = m_tMeasureParam.vecLineParam[nSN].dEndCol;
			}
			else
			{
				tOut.tLine.hv_StartRow = dStartRow;
				tOut.tLine.hv_StartColumn = dStartColumn;
				tOut.tLine.hv_EndRow = dEndRow;
				tOut.tLine.hv_EndColumn = dEndColumn;
				tOut.hv_LineNum = 1;
				tOut.hoDisp = hoL;
			}
		}
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CLineMeasure_FindSingleLine: 算法%d测量边时异常 (%s [%s])", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
	}
	return TRUE;
}

BOOL CLineMeasure::FitAllLines()
{
	if (m_tMeasureParam.vecLineParam.size() != m_tInit.tProductInfo.nMeasureLineNum) return FALSE;

	HTuple hv_StartTime, hv_EndTime;
	HObject hoLines;
	GenEmptyObj(&hoLines);

	try
	{
		for (int i = 0; i < m_tMeasureParam.vecLineParam.size(); i++)
		{
			HObject hoL;
			double dStartRow(0.0), dStartColumn(0.0), dEndRow(0.0), dEndColumn(0.0);
			tagLineParam tLineParam = m_tMeasureParam.vecLineParam[i];
			if (i != 0)
			{
				tLineParam.dStartCol += m_dMeasureLineMoveDiff;
				tLineParam.dEndCol += m_dMeasureLineMoveDiff;
			}
			if (FALSE == _FindLine(m_ptIn->hoImage, i, tLineParam, &hoL, &dStartRow, &dStartColumn, &dEndRow, &dEndColumn))
			{
				//测量线失败
				m_hvRowBegin[i] = tLineParam.dStartRow;
				m_hvColBegin[i] = tLineParam.dStartCol;
				m_hvRowEnd[i] = tLineParam.dEndRow;
				m_hvColEnd[i] = tLineParam.dEndCol;
				m_hvErrorLineIndex.Append(i);
			}
			else
			{
				if (fabs(dStartColumn - dEndColumn) > m_tMeasureParam.dMaxLeanDiff)//如果是起始终止列小于若干列则认为寻边失败
				{
					m_hvRowBegin[i] = tLineParam.dStartRow;
					m_hvColBegin[i] = tLineParam.dStartCol;
					m_hvRowEnd[i] = tLineParam.dEndRow;
					m_hvColEnd[i] = tLineParam.dEndCol;
					m_hvErrorLineIndex.Append(i);
				}
				else
				{
					m_hvRowBegin[i] = dStartRow;
					m_hvColBegin[i] = dStartColumn;
					m_hvRowEnd[i] = dEndRow;
					m_hvColEnd[i] = dEndColumn;

					if (i == 0)
					{
						m_dMeasureLineMoveDiff = (dStartColumn + dEndColumn) / 2 - (tLineParam.dStartCol + tLineParam.dEndCol) / 2;
					}
				}
			}
			ConcatObj(m_ptOut->hoDisp, hoL, &m_ptOut->hoDisp);
		}
		m_ptOut->tLine.hv_StartRow = m_hvRowBegin;
		m_ptOut->tLine.hv_StartColumn = m_hvColBegin;
		m_ptOut->tLine.hv_EndRow = m_hvRowEnd;
		m_ptOut->tLine.hv_EndColumn = m_hvColEnd;
		m_ptOut->hvErrorLineSN = m_hvErrorLineIndex;
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CLineMeasure_FitAllLines: 算法%d测量边时异常 (%s [%s])", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
	}
	return TRUE;
}

BOOL CLineMeasure::AutoGenMeasureLine(const GenLineInput& tIn, GenLineOutput& tOut)
{
	//根据产品类型初始化线
	GenDefultLineProp();
	//自动生成测量区域
	try
	{
		GenEmptyObj(&tOut.hoLine);
		GenEmptyObj(&tOut.hoDisp);

		if (m_tInit.tProductInfo.nPDType != ThreeCavity)
		{
			HTuple hvFoilCol1, hvFoilCol2, hvCoatCol1, hvCoatCol2, hvWidth, hvHeight, hvMeasureHandl;
			HObject hoImage, hoRegion, hoConnections, hoRect, hoReduce;
			GetImageSize(tIn.hoImage, &hvWidth, &hvHeight);
			//获取极耳区域行坐标
			GenRectangle1(&hoRect, hvHeight / 2 - 100, 0, hvHeight / 2 + 100, hvWidth - 1);
			ReduceDomain(tIn.hoImage, hoRect, &hoReduce);
			GrayClosingRect(hoReduce, &hoImage, 1, 300);
			GrayOpeningRect(hoImage, &hoImage, 100, 1);
			GenMeasureRectangle2(hvHeight / 2, hvWidth / 2, 0, hvWidth / 2 - 1000, 25, hvWidth, hvHeight, "nearest_neighbor", &hvMeasureHandl);
			MeasurePairs(hoImage, hvMeasureHandl, 3, 30, "positive", "all", &HTuple(), &hvFoilCol1, &HTuple(), &HTuple(), &hvFoilCol2, &HTuple(), &HTuple(), &HTuple());

			if (hvFoilCol1.Length() == 0 || hvFoilCol2.Length() == 0) return FALSE;
			//过滤多余的点，两个点之间的横向距离要超过300个像素才认为是有效的极耳边
			HTuple hvTmp, hvFilt;
			hvTmp = hvFoilCol1[0];
			hvFilt = hvFoilCol1[0];
			for (int i = 1; i < hvFoilCol1.Length(); i++)
			{
				if (hvFoilCol1[i] - hvTmp > 350)
				{
					hvFilt.Append(hvFoilCol1[i]);
					hvTmp = hvFoilCol1[i];
				}
			}
			hvFoilCol1 = hvFilt;

			hvTmp = hvFoilCol2[0];
			hvFilt = hvFoilCol2[0];
			for (int i = 1; i < hvFoilCol2.Length(); i++)
			{
				if (hvFoilCol2[i] - hvTmp > 350)
				{
					hvFilt.Append(hvFoilCol2[i]);
					hvTmp = hvFoilCol2[i];
				}
			}
			hvFoilCol2 = hvFilt;

			if (m_tInit.hvCamSNList.Length() == 2
				&& (hvFoilCol1.Length() == m_tInit.tProductInfo.nPDType + 2
					|| hvFoilCol2.Length() == m_tInit.tProductInfo.nPDType + 2))
			{
				//单面双相机，如果极耳多找到一个，就认为是双相机多拍了一个极耳，需要去掉中间两个极耳的内边
				hvFoilCol1 = hvFoilCol1.TupleRemove(hvFoilCol1.Length() / 2);
				hvFoilCol2 = hvFoilCol2.TupleRemove(hvFoilCol2.Length() / 2 - 1);
			}
			//涂布坐标等于极耳坐标去掉首尾
			hvCoatCol1 = hvFoilCol1.TupleRemove(0);
			TupleAdd(hvCoatCol1, 3, &hvCoatCol1);
			hvCoatCol2 = hvFoilCol2.TupleRemove(hvFoilCol2.Length() - 1);
			TupleSub(hvCoatCol2, 3, &hvCoatCol2);
			if (hvCoatCol1.Length() != hvCoatCol2.Length() || hvCoatCol1.Length() != m_tInit.tProductInfo.nPDType)
				return FALSE;
			//合并行坐标
			HTuple hvLineCol;
			if (m_tInit.tProductInfo.bExistAT11 || m_tInit.tProductInfo.bExistBaseCoat)
			{
				hvLineCol.Append(hvFoilCol1);
				hvLineCol.Append(hvFoilCol2);
				hvLineCol.Append(hvCoatCol1);
				hvLineCol.Append(hvCoatCol2);
			}
			else
			{
				hvLineCol.Append(hvFoilCol1.TupleMin());
				hvLineCol.Append(hvCoatCol1);
				hvLineCol.Append(hvCoatCol2);
				hvLineCol.Append(hvFoilCol2.TupleMax());
			}
			TupleSort(hvLineCol, &hvLineCol);

			if (hvLineCol.Length() != m_tInit.tProductInfo.nMeasureLineNum || hvLineCol.Length() != m_tMeasureParam.vecLineParam.size())
				return FALSE;

			double dRow1 = hvHeight.D() / 8;
			double dRow2 = hvHeight.D() / 8 * 7;

			//生成测量
			for (int i = 0; i < hvLineCol.Length(); i++)
			{
				if (i == 0 || i == hvLineCol.Length() - 1)
					m_tMeasureParam.vecLineParam[i].dWidth = 600;
				else
					m_tMeasureParam.vecLineParam[i].dWidth = 350;

				m_tMeasureParam.vecLineParam[i].dHeight = 25;
				if (i == 0 || i == hvLineCol.Length() - 1)
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D();
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D();
				}
				else if (i % 2 == 0)
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() + 30;
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() + 30;
				}
				else if (i % 2 == 1)
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() - 30;
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() - 30;
				}
				
				m_tMeasureParam.vecLineParam[i].nSelect = MS_First;

				if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BgFoil)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
					m_tMeasureParam.vecLineParam[i].nThr = 30;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilBg)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
					m_tMeasureParam.vecLineParam[i].nThr = 30;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilAT11)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
					m_tMeasureParam.vecLineParam[i].nThr = 15;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Foil)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
					m_tMeasureParam.vecLineParam[i].nThr = 15;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Coat)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
					m_tMeasureParam.vecLineParam[i].nThr = 9;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatAT11)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
					m_tMeasureParam.vecLineParam[i].nThr = 9;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilCoat)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
					m_tMeasureParam.vecLineParam[i].nThr = 25;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatFoil)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
					m_tMeasureParam.vecLineParam[i].nThr = 25;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilBaseCoat)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
					m_tMeasureParam.vecLineParam[i].nThr = 20;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatFoil)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
					m_tMeasureParam.vecLineParam[i].nThr = 20;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatCoat)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
					m_tMeasureParam.vecLineParam[i].nThr = 10;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatBaseCoat)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
					m_tMeasureParam.vecLineParam[i].nThr = 10;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
				}
			}

			HObject hoLine;
			HTuple hvRow1, hvRow2;
			TupleGenConst(hvLineCol.Length(), hvHeight.D() / 8, &hvRow1);
			TupleGenConst(hvLineCol.Length(), hvHeight.D() / 8 * 7, &hvRow2);
			GenRectangle1(&hoLine, hvRow1, hvLineCol - 20, hvRow2, hvLineCol + 20);
			SortRegion(hoLine, &hoLine, "upper_left", "true", "column");
			tOut.hoLine = hoLine;

			tOut.tLine.hv_StartRow = hvRow1;
			tOut.tLine.hv_EndRow = hvRow2;
			tOut.tLine.hv_StartColumn = hvLineCol;
			tOut.tLine.hv_EndColumn = hvLineCol;
		}
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CLineMeasure_AutoGenMeasureLine: 算法%d自动生成测量线异常 %s[%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		return FALSE;
	}

	return TRUE;
}

//BOOL CLineMeasure::AutoGenMeasureLineWithStdVal(const GenLineInput& tIn, GenLineOutput& tOut)
//{
//	HTuple hvStdSize;
//	//生成规格
//	if (m_tInit.tProductInfo.nPDType != ThreeCavity)
//	{
//		hvStdSize.Append(m_tInit.tProductInfo.tParam.vecFoilWidth[0].dStandard);
//		for (int i = 0; i < m_tInit.tProductInfo.nPDType; i++)
//		{
//			//AT11,底涂
//			if (m_tInit.tProductInfo.bExistAT11)
//				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard);
//			else if (m_tInit.tProductInfo.bExistBaseCoat)
//				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard);
//			//涂布
//			hvStdSize.Append(m_tInit.tProductInfo.tParam.vecCoatWidth[0].dStandard);
//			//AT11,底涂
//			if (m_tInit.tProductInfo.bExistAT11)
//				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard);
//			else if (m_tInit.tProductInfo.bExistBaseCoat)
//				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard);
//			//极耳
//			if (i < (m_tInit.tProductInfo.nPDType - 1))
//				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecFoilWidth[0].dStandard * 2);
//			else
//				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecFoilWidth[0].dStandard);
//		}
//
//		if (hvStdSize.Length() != m_tInit.tProductInfo.nMeasureLineNum - 1)
//		{
//			return FALSE;
//		}
//
//		HTuple hvLineCol, hvTmp, hvHeight;
//		GetImageSize(tIn.hoImage, &HTuple(), &hvHeight);
//		hvLineCol.Append(tIn.hvBaseLineColumn);
//		hvTmp = tIn.hvBaseLineColumn;
//		BOOL bFirstBorder = TRUE;
//		for (int i = 0; i < hvStdSize.Length(); i++)
//		{
//			if (m_tInit.hvCamSNList.Length() > 1 && m_hvOverColumn.Length() == 2)
//			{
//				//双相机的时候判断哪些在屏蔽线左侧，哪些在屏蔽线右侧
//				if (hvTmp < m_hvOverColumn.TupleMin() || bFirstBorder == FALSE)
//				{
//					hvTmp = hvTmp + hvStdSize[i].D() / m_vecResolution[0].dColRes;
//				}
//				else if (hvTmp >= m_hvOverColumn.TupleMax())
//				{
//					hvTmp = hvTmp + hvStdSize[i].D()/ m_vecResolution[1].dColRes;
//				}
//
//				if (bFirstBorder == TRUE && hvTmp >= m_hvOverColumn.TupleMin())
//				{
//					bFirstBorder = FALSE;
//					hvTmp = hvTmp + (m_hvOverColumn.TupleMax() - m_hvOverColumn.TupleMin());
//				}
//			}
//			else
//			{
//				hvTmp = hvTmp + hvStdSize[i].D()/m_vecResolution[0].dColRes;
//			}
//			hvLineCol.Append(hvTmp);
//		}
//		
//
//		//生成测量线
//		if (hvLineCol.Length() != m_tInit.tProductInfo.nMeasureLineNum || hvLineCol.Length() != m_tMeasureParam.vecLineParam.size())
//			return FALSE;
//
//		double dRow1 = hvHeight.D() / 8;
//		double dRow2 = hvHeight.D() / 8 * 7;
//
//		TupleSort(hvLineCol, &hvLineCol);
//
//		//生成测量
//		for (int i = 0; i < hvLineCol.Length(); i++)
//		{
//			if (i == 0)
//				m_tMeasureParam.vecLineParam[i].dWidth = 550;
//			else
//				m_tMeasureParam.vecLineParam[i].dWidth = 350;
//
//			m_tMeasureParam.vecLineParam[i].dHeight = 25;
//			if (i == 0 || i == hvLineCol.Length() - 1)
//			{
//				m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D();
//				m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D();
//			}
//			else if (i % 2 == 0)
//			{
//				m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() + 30;
//				m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() + 30;
//			}
//			else if (i % 2 == 1)
//			{
//				m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() - 30;
//				m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() - 30;
//			}
//
//			m_tMeasureParam.vecLineParam[i].nSelect = MS_First;
//
//			if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BgFoil)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 30;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilBg)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 30;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilAT11)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
//				m_tMeasureParam.vecLineParam[i].nThr = 15;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Foil)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
//				m_tMeasureParam.vecLineParam[i].nThr = 15;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Coat)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 9;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatAT11)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 9;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilCoat)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 25;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatFoil)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 25;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilBaseCoat)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
//				m_tMeasureParam.vecLineParam[i].nThr = 20;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatFoil)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
//				m_tMeasureParam.vecLineParam[i].nThr = 20;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatCoat)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 10;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
//			}
//			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatBaseCoat)
//			{
//				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
//				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
//				m_tMeasureParam.vecLineParam[i].nThr = 10;
//
//				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
//				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
//			}
//		}
//
//		HObject hoLine;
//		HTuple hvRow1, hvRow2;
//		TupleGenConst(hvLineCol.Length(), hvHeight.D() / 8, &hvRow1);
//		TupleGenConst(hvLineCol.Length(), hvHeight.D() / 8 * 7, &hvRow2);
//		GenRectangle1(&hoLine, hvRow1, hvLineCol - 20, hvRow2, hvLineCol + 20);
//		SortRegion(hoLine, &hoLine, "upper_left", "true", "column");
//		tOut.hoLine = hoLine;
//
//		tOut.tLine.hv_StartRow = hvRow1;
//		tOut.tLine.hv_EndRow = hvRow2;
//		tOut.tLine.hv_StartColumn = hvLineCol;
//		tOut.tLine.hv_EndColumn = hvLineCol;
//	}
//
//	return TRUE;
//}

BOOL CLineMeasure::AutoGenMeasureLineWithStdVal(const GenLineInput& tIn, GenLineOutput& tOut)
{
	//根据产品类型初始化线
	GenDefultLineProp();
	HTuple hvStdSize;
	//生成规格
	if (m_tInit.tProductInfo.nPDType != ThreeCavity)
	{
		hvStdSize.Append(m_tInit.tProductInfo.tParam.vecFoilWidth[0].dStandard);
		for (int i = 0; i < m_tInit.tProductInfo.nPDType; i++)
		{
			//AT11,底涂
			if (m_tInit.tProductInfo.bExistAT11)
				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard);
			else if (m_tInit.tProductInfo.bExistBaseCoat)
				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard);
			//涂布
			hvStdSize.Append(m_tInit.tProductInfo.tParam.vecCoatWidth[0].dStandard);
			//AT11,底涂
			if (m_tInit.tProductInfo.bExistAT11)
				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard);
			else if (m_tInit.tProductInfo.bExistBaseCoat)
				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard);
			//极耳
			if (i < (m_tInit.tProductInfo.nPDType - 1))
				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecFoilWidth[0].dStandard * 2);
			else
				hvStdSize.Append(m_tInit.tProductInfo.tParam.vecFoilWidth[0].dStandard);
		}

		if (hvStdSize.Length() != m_tInit.tProductInfo.nMeasureLineNum - 1)
		{
			return FALSE;
		}

		HTuple hvLineCol, hvTmp, hvWidth, hvHeight, hvTmpReal, hvBaseLineColumn;
		GetImageSize(tIn.hoImage, &hvWidth, &hvHeight);
		//图像去找基材最左边的坐标
		HObject hoRect, hoImageReduced, hoImageOpening, hoRegion, hoRegionOpening, hoConnectedRegions, hoSelectedRegions, hoObjectSelected;
		try
		{
			GenRectangle1(&hoRect, hvHeight / 2 - 20, 200, hvHeight / 2 + 20, hvWidth - 200);
			ReduceDomain(tIn.hoImage, hoRect, &hoImageReduced);
			GrayOpeningRect(hoImageReduced, &hoImageOpening, 20, 1);
			GrayClosingRect(hoImageOpening, &hoImageOpening, 1, 500);

			BinaryThreshold(hoImageOpening, &hoRegion, "max_separability", "light", &HTuple());
			OpeningRectangle1(hoRegion, &hoRegionOpening, 1, 10);
			OpeningRectangle1(hoRegionOpening, &hoRegionOpening, 10, 1);
			Connection(hoRegionOpening, &hoConnectedRegions);
			SelectShape(hoConnectedRegions, &hoSelectedRegions, "width", "and", 50, 999999);
			SelectShape(hoSelectedRegions, &hoSelectedRegions, "height", "and", 20, 99999);
			SortRegion(hoSelectedRegions, &hoSelectedRegions, "first_point", "true", "column");
			SelectObj(hoSelectedRegions, &hoObjectSelected, 1);
			SmallestRectangle1(hoObjectSelected, &HTuple(), &hvBaseLineColumn, &HTuple(), &HTuple());
		}
		catch (HException&)
		{
			return FALSE;
		}
		if (hvBaseLineColumn.Length() == 0 || hvBaseLineColumn < 0) return FALSE;
		

		//通过物理坐标转换像素坐标，将每一个尺寸规格都转换成对应测量的位置
		hvLineCol.Append(hvBaseLineColumn);
		hvTmp = hvBaseLineColumn;
		g_pAlgorithm->m_Calibration.CalPoint(hvTmp, hvTmpReal);
		for (int i = 0; i < hvStdSize.Length(); i++)
		{
			hvTmpReal += hvStdSize[i].D();
			g_pAlgorithm->m_Calibration.CalPointToPixel(hvTmpReal, hvTmp);
			if(i < hvStdSize.Length()/2)
				hvLineCol.Append(hvTmp.TupleMin());
			else
				hvLineCol.Append(hvTmp.TupleMax());
		}

		//生成测量线
		if (hvLineCol.Length() != m_tInit.tProductInfo.nMeasureLineNum || hvLineCol.Length() != m_tMeasureParam.vecLineParam.size()) return FALSE;

		double dRow1 = hvHeight.D() / 8;
		double dRow2 = hvHeight.D() / 8 * 7;

		//生成测量
		for (int i = 0; i < hvLineCol.Length(); i++)
		{
			if (i == 0 || i == hvLineCol.Length() - 1)
				m_tMeasureParam.vecLineParam[i].dWidth = 600;
			else
				m_tMeasureParam.vecLineParam[i].dWidth = 350;

			m_tMeasureParam.vecLineParam[i].dHeight = 25;
			if (i == 0 || i == hvLineCol.Length() - 1)
			{
				m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D();
				m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D();
			}
			else if (i % 2 == 0)
			{
				m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() + 30;
				m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() + 30;
			}
			else if (i % 2 == 1)
			{
				m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() - 30;
				m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() - 30;
			}

			m_tMeasureParam.vecLineParam[i].nSelect = MS_First;

			if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BgFoil)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 30;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilBg)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 30;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilAT11)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 15;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Foil)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 15;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Coat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 9;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatAT11)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 9;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 25;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatFoil)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 25;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilBaseCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 20;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatFoil)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 20;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 10;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatBaseCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 10;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
			}
		}

		HObject hoLine;
		HTuple hvRow1, hvRow2;
		TupleGenConst(hvLineCol.Length(), hvHeight.D() / 8, &hvRow1);
		TupleGenConst(hvLineCol.Length(), hvHeight.D() / 8 * 7, &hvRow2);
		GenRectangle1(&hoLine, hvRow1, hvLineCol - 20, hvRow2, hvLineCol + 20);
		SortRegion(hoLine, &hoLine, "upper_left", "true", "column");
		tOut.hoLine = hoLine;

		tOut.tLine.hv_StartRow = hvRow1;
		tOut.tLine.hv_EndRow = hvRow2;
		tOut.tLine.hv_StartColumn = hvLineCol;
		tOut.tLine.hv_EndColumn = hvLineCol;
	}

	return TRUE;
}

CLineMeasure::LineParam CLineMeasure::ReadLineParam(const int& nSN)
{
	CString strFile;
	strFile.Format("%s\\Config\\Products\\%s\\SCIMeasure\\%d\\%d.ini", GetCurrentAppPath(), m_tInit.tProductInfo.strPDName, m_tInit.nSN, nSN);

	LineParam tPara;
	char tmpchar[MAX_PATH];
	tPara.nDirection = GetPrivateProfileInt("Param", "nDirection", MD_Left2Right, strFile);
	tPara.nTransition = GetPrivateProfileInt("Param", "nTransition", MT_All, strFile);
	tPara.nSelect = GetPrivateProfileInt("Param", "nSelect", MS_First, strFile);
	tPara.nThr = GetPrivateProfileInt("Param", "nThr", 20, strFile);
	tPara.dSigma = GetPrivateProfileDouble("Param", "dSigma", 1.0, strFile);

	tPara.dWidth = GetPrivateProfileDouble("Param", "dWidth", 300, strFile);
	tPara.dHeight = 15;

	tPara.nMeasureNum = GetPrivateProfileInt("Param", "nMeasureNum", 15, strFile);
	//测量点数量写死为5个
	tPara.nMeasureNum = 5;

	tPara.dStartRow = GetPrivateProfileDouble("Param", "dStartRow", 100, strFile);
	tPara.dStartCol = GetPrivateProfileDouble("Param", "dStartCol", 100, strFile);
	tPara.dEndRow = GetPrivateProfileDouble("Param", "dEndRow", 300.0, strFile);
	tPara.dEndCol = GetPrivateProfileDouble("Param", "dEndCol", 300.0, strFile);

	return tPara;
}

void CLineMeasure::WriteLineParam(const int& nSN, const CLineMeasure::LineParam& tPara)
{
	CString strFile;
	strFile.Format("%s\\Config\\Products\\%s\\SCIMeasure\\%d", GetCurrentAppPath(), m_tInit.tProductInfo.strPDName, m_tInit.nSN);
	CreateMultilevelPath(strFile);
	strFile.Format("%s\\Config\\Products\\%s\\SCIMeasure\\%d\\%d.ini", GetCurrentAppPath(), m_tInit.tProductInfo.strPDName, m_tInit.nSN, nSN);

	WritePrivateProfileInt("Param", "nDirection", tPara.nDirection, strFile);
	WritePrivateProfileInt("Param", "nTransition", tPara.nTransition, strFile);
	WritePrivateProfileInt("Param", "nSelect", tPara.nSelect, strFile);
	WritePrivateProfileInt("Param", "nThr", tPara.nThr, strFile);
	WritePrivateProfileDouble("Param", "dSigma", tPara.dSigma, strFile);

	WritePrivateProfileDouble("Param", "dWidth", tPara.dWidth, strFile);

	WritePrivateProfileInt("Param", "nMeasureNum", tPara.nMeasureNum, strFile);

	WritePrivateProfileDouble("Param", "dStartRow", tPara.dStartRow, strFile);
	WritePrivateProfileDouble("Param", "dStartCol", tPara.dStartCol, strFile);
	WritePrivateProfileDouble("Param", "dEndRow", tPara.dEndRow, strFile);
	WritePrivateProfileDouble("Param", "dEndCol", tPara.dEndCol, strFile);
}