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

int CLineMeasure::_FindLine(const HObject& Image, const int nSN, const tagLineParam& FitLinePara, 
	HObject* Line, double* dStartY, double* dStartX, double* dEndY, double* dEndX, int nFitSingleLine, int nLineDisplay)
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
			TempFitLinePara.dStartRow = Height.I() / 3 * 2;
			TempFitLinePara.dEndRow = Height.I() / 3 * 1;
		}
		else
		{
			TempFitLinePara.dStartRow = Height.I() / 3 * 1;
			TempFitLinePara.dEndRow = Height.I() / 3 * 2;
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
		if (PRE_PointFilter == FitLinePara.ePreproc)
		{
			//点选择的时候，选择所有点
			select = "all";
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

			eLineType eType = m_tMeasureParam.tLineProp.vecLineTypeList[nSN];
			if (TRUE != nFitSingleLine)
			{
				if (LT_BaseCoatCoat == eType)
				{
					int nWidth = m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard / m_vecResolution[0].dColRes;
					nWidth = min(nWidth, 80);
					//相对左边极耳边的坐标位移30像素
					ColBegin = ColEnd = (m_hvColBegin[nSN - 1].D() + m_hvColEnd[nSN - 1].D()) / 2 + nWidth;
				}
				else if (LT_CoatBaseCoat == eType)
				{
					int nWidth = m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard / m_vecResolution[0].dColRes;
					nWidth = min(nWidth, 80);
					//相对右边极耳边的坐标位移30像素
					ColBegin = ColEnd = (m_hvColBegin[nSN + 1].D() + m_hvColEnd[nSN + 1].D()) / 2 - nWidth;
				}
				else if (LT_CoatAT11 == eType)
				{
					int nWidth = m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard / m_vecResolution[0].dColRes;
					nWidth = min(nWidth, 150);
					//相对左边极耳边的坐标位移30像素
					ColBegin = ColEnd = (m_hvColBegin[nSN + 1].D() + m_hvColEnd[nSN + 1].D()) / 2 + nWidth;
				}
				else if (LT_AT11Coat == eType)
				{
					int nWidth = m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard / m_vecResolution[0].dColRes;
					nWidth = min(nWidth, 150);
					//相对右边极耳边的坐标位移30像素
					ColBegin = ColEnd = (m_hvColBegin[nSN - 1].D() + m_hvColEnd[nSN - 1].D()) / 2 - nWidth;
				}
			}

			//找边,如果有凹版湿膜检测，就按照单独检测的方式
			if (PRE_BaseCoatWet == FitLinePara.ePreproc && TRUE!=nFitSingleLine
				&& (LT_BaseCoatCoat == eType || LT_CoatBaseCoat == eType))
			{
				//湿膜和凹版边，找边处理,对多个点进行阈值分割，找到多个有效的边求平均值
				double dMaxRow = max(RowBegin.D(), RowEnd.D());
				double dMinRow = min(RowBegin.D(), RowEnd.D());
				double dHeight = abs(dMaxRow - dMinRow) / (TempFitLinePara.nMeasureNum - 1);
				HTuple hvEdgeCol;

				const int WIDTH = 50;

				for (int i = 0; i < FitLinePara.nMeasureNum;i++)
				{
					HTuple hvRow = dMinRow + i*dHeight;

					HTuple hvColTmp;
					if (_SegmentEdge(Image, hvRow, ColBegin, TempFitLinePara, eType, &hvColTmp))
						hvEdgeCol.Append(hvColTmp);
				}
				if (hvEdgeCol.Length() > 0)
				{
					HTuple hvCol = hvEdgeCol.TupleMedian();
					HObject hLine;
					GenRegionLine(&hLine, min(TempFitLinePara.dStartRow, TempFitLinePara.dEndRow), hvCol,
						max(TempFitLinePara.dStartRow, TempFitLinePara.dEndRow), hvCol);
					ConcatObj(*Line, hLine, Line);
					*dStartY = TempFitLinePara.dStartRow;
					*dStartX = hvCol.D();
					*dEndY = TempFitLinePara.dEndRow;
					*dEndX = hvCol.D();

					return TRUE;
				}

				//如果凹版没找到边
				{
					if (LT_BaseCoatCoat == eType || LT_CoatBaseCoat == eType)
					{
						//判断是否极耳覆盖凹版，使用极耳和凹版的边进行测量
						HObject hoLeftRegion, hoRightRegion;
						HTuple hvLeftMean, hvRightMean, hvLeftSig, hvRightSig;
						HTuple hvCol;
						if (LT_BaseCoatCoat == eType)
						{
							hvCol = (m_hvColBegin[nSN - 1].D() + m_hvColEnd[nSN - 1].D()) / 2;

							GenRectangle1(&hoLeftRegion, Height.I() / 2 - 10, hvCol - 20, Height.I() / 2 + 10, hvCol - 2);
							GenRectangle1(&hoRightRegion, Height.I() / 2 - 10, hvCol + 2, Height.I() / 2 + 10, hvCol + 20);
							Intensity(hoLeftRegion, Image, &hvLeftMean, &hvLeftSig);
							Intensity(hoRightRegion, Image, &hvRightMean, &hvRightSig);

							if (hvLeftMean.D() - hvRightMean.D() > 20 && hvLeftMean.D() > 150)
							{
								//左侧区域灰度大于右侧区域，说明左侧区域主要是极耳
								ColBegin = m_hvColBegin[nSN - 1];
								ColEnd = m_hvColEnd[nSN - 1];
								RowBegin = m_hvRowBegin[nSN - 1];
								RowEnd = m_hvRowEnd[nSN - 1];
								HObject hLine;
								GenRegionLine(&hLine, RowBegin, ColBegin, RowEnd, ColEnd);
								ConcatObj(*Line, hLine, Line);
								*dStartY = RowBegin[0].D();
								*dStartX = ColBegin[0].D();
								*dEndY = RowEnd[0].D();
								*dEndX = ColEnd[0].D();

								return TRUE;
							}
						}
						else
						{
							hvCol = (m_hvColBegin[nSN + 1].D() + m_hvColEnd[nSN + 1].D()) / 2;

							GenRectangle1(&hoLeftRegion, Height.I() / 2 - 10, hvCol - 20, Height.I() / 2 + 10, hvCol - 2);
							GenRectangle1(&hoRightRegion, Height.I() / 2 - 10, hvCol + 2, Height.I() / 2 + 10, hvCol + 20);
							Intensity(hoLeftRegion, Image, &hvLeftMean, &hvLeftSig);
							Intensity(hoRightRegion, Image, &hvRightMean, &hvRightSig);

							if (hvRightMean.D() - hvLeftMean.D() > 20 && hvRightMean.D() > 150)
							{
								//右侧区域灰度大于左侧区域，说明右侧区域主要是极耳
								ColBegin = m_hvColBegin[nSN + 1];
								ColEnd = m_hvColEnd[nSN + 1];
								RowBegin = m_hvRowBegin[nSN + 1];
								RowEnd = m_hvRowEnd[nSN + 1];
								HObject hLine;
								GenRegionLine(&hLine, RowBegin, ColBegin, RowEnd, ColEnd);
								ConcatObj(*Line, hLine, Line);
								*dStartY = RowBegin[0].D();
								*dStartX = ColBegin[0].D();
								*dEndY = RowEnd[0].D();
								*dEndX = ColEnd[0].D();

								return TRUE;
							}
							
						}
						//如果凹版和涂膜区域的平均灰度和标准差都比较小
					}
				}
			}
			else if(PRE_AT11Virtual == FitLinePara.ePreproc && TRUE != nFitSingleLine
				&& (LT_AT11Coat == eType || LT_CoatAT11 == eType))
			{
				//AT虚边处理
				HObject hoROIRegion, ImageReduced;
				HTuple HorProjection, VertProjection, VFunction, SmoothedVF, DerSmoothedVF, Der_XValues, Der_YValues, DerSmoothedVF_1, Der1_XValues, Der1_YValues, Min_Der1_YValues, Number_Min, MinIndex;
				GenRectangle1(&hoROIRegion, Height.I() / 2 - 100, ColBegin, Height.I() / 2 + 100, ColBegin + 50);
				ReduceDomain(Image, hoROIRegion, &ImageReduced);
				GrayProjections(hoROIRegion, ImageReduced, "simple", &HorProjection, &VertProjection);
				CreateFunct1dArray(VertProjection, &VFunction);
				SmoothFunct1dGauss(VFunction, 1, &SmoothedVF);
				DerivateFunct1d(SmoothedVF, "first", &DerSmoothedVF);
				Funct1dToPairs(DerSmoothedVF, &Der_XValues, &Der_YValues);
				HTuple tupleDer_YValues = Der_YValues[1, 30];
				HTuple Sum = 0;
				for (int i = 0; i < tupleDer_YValues.Length() - 1; i++)
				{
					if (tupleDer_YValues[i] > 0)
						Sum = Sum + tupleDer_YValues[i];
				}
				//二阶导数
				DerivateFunct1d(DerSmoothedVF, "first", &DerSmoothedVF_1);
				Funct1dToPairs(DerSmoothedVF_1, &Der1_XValues, &Der1_YValues);
				//一阶导数最小
				TupleMin(Der1_YValues, &Min_Der1_YValues);
				TupleFind(Der_YValues, Min_Der1_YValues, &Number_Min);
				//求一阶导数函数曲线凹点
				HTuple tuple2, Grayva, Dex, Grayval, GrayvalMean, hvCol;
				for (int i = 0; i < 30; i++)
				{
					if (Der1_YValues[i] < 0 && Der1_YValues[i + 1]>0)
					{
						tuple2.Append(i);
					}
				}
				TupleFindFirst(Der_YValues, Min_Der1_YValues, &MinIndex);
				if (Sum < 0.15)
				{
					Dex = MinIndex;
				}
				else
				{
					for (int i = 0; i < tuple2.Length() - 1; i++)
					{
						if (MinIndex <= tuple2[i])
						{
							HTuple row11;
							HTuple clow1;
							for (int j = 0; j < 5; j++)
							{
								row11.Append(500 + j);
								clow1.Append(ColBegin + tuple2[i]);//待完善
							}
							GetGrayval(ImageReduced, row11, clow1, &Grayval);
							TupleMean(Grayval, &GrayvalMean);
							if (GrayvalMean > 22)
							{
								Dex.Append(tuple2[i]);
							}
							if (i == (tuple2.Length() - 1) && Dex.Length() == 0)
							{
								Dex = MinIndex;
							}
						}
					}
					if (Dex.Length() > 1)
					{
						Dex = Dex.TupleMax();
					}
				}
				if (Dex.Length() > 0)
				{
					hvCol = ColBegin + Dex;
					HObject hLine;
					GenRegionLine(&hLine, min(TempFitLinePara.dStartRow, TempFitLinePara.dEndRow), hvCol,
						max(TempFitLinePara.dStartRow, TempFitLinePara.dEndRow), hvCol);
					ConcatObj(*Line, hLine, Line);
					*dStartY = TempFitLinePara.dStartRow;
					*dStartX = hvCol.D();
					*dEndY = TempFitLinePara.dEndRow;
					*dEndX = hvCol.D();

					return TRUE;
				}
				
			}

			//没找到边就用原始的测量边的方式重新找
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
			GetMetrologyObjectMeasures(&hMeasureRect, hv_MetrologyHdl, "all", "all", &hvMeasureRow, &hvMeasureCol);
			if (PRE_PointFilter == FitLinePara.ePreproc)
			{
				//将所有点筛选一下
				HTuple hvSelectRow, hvSelectCol;
				for (int i = 0; i < hvMeasureRow.Length(); i++)
				{
					HTuple hvRowTmp = hvMeasureRow[i];
					HTuple hvColTmp = hvMeasureCol[i];

					//判断点左右灰度差，以及单侧灰度标准差
					HObject hoLeftRegion, hoRightRegion;
					HTuple hvLeftMean, hvRightMean, hvLeftSig, hvRightSig;
					GenRectangle1(&hoLeftRegion, hvRowTmp - 20, hvColTmp - 12, hvRowTmp + 20, hvColTmp - 3);
					GenRectangle1(&hoRightRegion, hvRowTmp - 20, hvColTmp + 3, hvRowTmp + 20, hvColTmp + 12);
					Intensity(hoLeftRegion, Image, &hvLeftMean, &hvLeftSig);
					Intensity(hoRightRegion, Image, &hvRightMean, &hvRightSig);

					if (abs(hvLeftMean.D() - hvRightMean.D()) > TempFitLinePara.nThr*2 && hvLeftSig.D() < 70 && hvRightSig.D() < 70)
					{
						if ((LT_BaseCoatFoil == eType || LT_CoatFoil == eType) && hvRightMean > 150)
						{
							hvSelectRow.Append(hvRowTmp);
							hvSelectCol.Append(hvColTmp);
						}
						else if ((LT_FoilBaseCoat == eType || LT_FoilCoat == eType) && hvLeftMean > 150)
						{
							hvSelectRow.Append(hvRowTmp);
							hvSelectCol.Append(hvColTmp);
						}
						else if (LT_BgFoil == eType && hvRightMean > 150)
						{
							hvSelectRow.Append(hvRowTmp);
							hvSelectCol.Append(hvColTmp);
						}
						else if (LT_FoilBg == eType && hvLeftMean > 150)
						{
							hvSelectRow.Append(hvRowTmp);
							hvSelectCol.Append(hvColTmp);
						}
						else if (LT_FoilBaseCoat != eType && LT_FoilCoat != eType && LT_BaseCoatFoil != eType && LT_CoatFoil != eType)
						{
							hvSelectRow.Append(hvRowTmp);
							hvSelectCol.Append(hvColTmp);
						}
					}
				}
				//二次筛选坐标，取横坐标中位数，如果其他横坐标与中位数差5个像素以上，过滤掉
				HTuple hvRowSecond, hvColSecond;
				for (int i = 0; i < hvSelectCol.Length(); i++)
				{
					if (abs(hvSelectCol[i].D() - hvSelectCol.TupleMedian().D()) <= 5)
					{
						hvRowSecond.Append(hvSelectRow[i]);
						hvColSecond.Append(hvSelectCol[i]);
					}
				}

				if (hvRowSecond.Length() > 4)
				{
					HObject hoContourTmp;
					HTuple Nr, Nc, Dist;
					GenContourPolygonXld(&hoContourTmp, hvRowSecond, hvColSecond);//tukey
					FitLineContourXld(hoContourTmp, "tukey", -1, 0, 5, 2, &RowBegin,
						&ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);
				}
			}
			if (nLineDisplay)
			{
				ConcatObj(*Line, hMeasureRect, &(*Line));
				GenCrossContourXld(&hCross, hvMeasureRow, hvMeasureCol, 25, 0.78);
				ConcatObj(*Line, hCross, Line);
			}
			HObject hLine;
			if (RowBegin.Length() > 0)
			{
				//GetMetrologyObjectResultContour(&hLine, hv_MetrologyHdl, "all", "all", 1);
				//GenContourPolygonXld(&hLine, RowBegin.TupleConcat(RowEnd), ColBegin.TupleConcat(ColEnd));
				GenRegionLine(&hLine, RowBegin, ColBegin, RowEnd, ColEnd);
				ConcatObj(*Line, hLine, Line);
				*dStartY = RowBegin[0].D();
				*dStartX = ColBegin[0].D();
				*dEndY = RowEnd[0].D();
				*dEndX = ColEnd[0].D();
			}
			else//如果没找到线，就是用原来的位置
			{
				//如果凹版没找到边
				{
					if (LT_BaseCoatCoat == eType || LT_CoatBaseCoat == eType)
					{
						//判断是否极耳覆盖凹版，使用极耳和凹版的边进行测量
						HObject hoLeftRegion, hoRightRegion;
						HTuple hvLeftMean, hvRightMean, hvLeftSig, hvRightSig;
						HTuple hvCol;
						if (LT_BaseCoatCoat == eType)
						{
							hvCol = (m_hvColBegin[nSN - 1].D() + m_hvColEnd[nSN - 1].D()) / 2;

							GenRectangle1(&hoLeftRegion, Height.I() / 2 - 10, hvCol - 20, Height.I() / 2 + 10, hvCol - 2);
							GenRectangle1(&hoRightRegion, Height.I() / 2 - 10, hvCol + 2, Height.I() / 2 + 10, hvCol + 20);
							Intensity(hoLeftRegion, Image, &hvLeftMean, &hvLeftSig);
							Intensity(hoRightRegion, Image, &hvRightMean, &hvRightSig);

							if (hvLeftMean.D() - hvRightMean.D() > 20 && hvLeftMean.D() > 150)
							{
								//左侧区域灰度大于右侧区域，说明左侧区域主要是极耳
								ColBegin = m_hvColBegin[nSN - 1];
								ColEnd = m_hvColEnd[nSN - 1];
								RowBegin = m_hvRowBegin[nSN - 1];
								RowEnd = m_hvRowEnd[nSN - 1];
								HObject hLine;
								GenRegionLine(&hLine, RowBegin, ColBegin, RowEnd, ColEnd);
								ConcatObj(*Line, hLine, Line);
								*dStartY = RowBegin[0].D();
								*dStartX = ColBegin[0].D();
								*dEndY = RowEnd[0].D();
								*dEndX = ColEnd[0].D();

								return TRUE;
							}
						}
						else
						{
							hvCol = (m_hvColBegin[nSN + 1].D() + m_hvColEnd[nSN + 1].D()) / 2;

							GenRectangle1(&hoLeftRegion, Height.I() / 2 - 10, hvCol - 20, Height.I() / 2 + 10, hvCol - 2);
							GenRectangle1(&hoRightRegion, Height.I() / 2 - 10, hvCol + 2, Height.I() / 2 + 10, hvCol + 20);
							Intensity(hoLeftRegion, Image, &hvLeftMean, &hvLeftSig);
							Intensity(hoRightRegion, Image, &hvRightMean, &hvRightSig);

							if (hvRightMean.D() - hvLeftMean.D() > 20 && hvRightMean.D() > 150)
							{
								//右侧区域灰度大于左侧区域，说明右侧区域主要是极耳
								ColBegin = m_hvColBegin[nSN + 1];
								ColEnd = m_hvColEnd[nSN + 1];
								RowBegin = m_hvRowBegin[nSN + 1];
								RowEnd = m_hvRowEnd[nSN + 1];
								HObject hLine;
								GenRegionLine(&hLine, RowBegin, ColBegin, RowEnd, ColEnd);
								ConcatObj(*Line, hLine, Line);
								*dStartY = RowBegin[0].D();
								*dStartX = ColBegin[0].D();
								*dEndY = RowEnd[0].D();
								*dEndX = ColEnd[0].D();

								return TRUE;
							}

						}
						//如果凹版和涂膜区域的平均灰度和标准差都比较小
					}
				}
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

BOOL CLineMeasure::_SegmentEdge(const HObject& Image, const HTuple& hvRow, const HTuple& hvCol, const tagLineParam& FitLinePara,
	const eLineType& eType, HTuple* hvEdgeCol)
{
	try
	{
		(*hvEdgeCol).Clear();

		HTuple Height, Width;
		GetImageSize(Image, &Height, &Width);

		if (LT_BaseCoatCoat == eType || LT_CoatBaseCoat == eType)//凹版和涂膜的边
		{
			HObject hoROIRegion, hoReduce, hoGrayClosing, hoRegion, hoBaseCoatRegion, hoReduceScaled;
			HTuple hvVer, hvHor, hvUsedThreshold, hvMean, hvInc, hvCoatGray, hv_Min, hv_Max, hv_Range;
			const int WIDTH = 100;
			const int HEIGHT = 30;
			GenRectangle1(&hoROIRegion, hvRow - HEIGHT, hvCol - WIDTH, hvRow + HEIGHT, hvCol + WIDTH);
			//获取涂布平均灰度
			//if (LT_BaseCoatCoat == eType)
			//{
			//	HObject hoCoatRegion;
			//	GenRectangle1(&hoCoatRegion, hvRow - HEIGHT, hvCol + WIDTH/4*3, hvRow + HEIGHT, hvCol + WIDTH);
			//	MinMaxGray(hoCoatRegion, Image, 0, &hv_Min, &hv_Max, &hv_Range);
			//	/*Intensity(hoCoatRegion, Image, &hvCoatGray, &HTuple());*/
			//}
			//else if (LT_CoatBaseCoat == eType)
			//{
			//	HObject hoCoatRegion;
			//	GenRectangle1(&hoCoatRegion, hvRow - HEIGHT, hvCol - WIDTH, hvRow + HEIGHT, hvCol - WIDTH/4*3);
			//	MinMaxGray(hoCoatRegion, Image, 0, &hv_Min, &hv_Max, &hv_Range);
			//	/*Intensity(hoCoatRegion, Image, &hvCoatGray, &HTuple());*/
			//}
			ReduceDomain(Image, hoROIRegion, &hoReduce);
			CropDomain(hoReduce, &hoReduce);
			MinMaxGray(hoROIRegion, Image, 0, &hv_Min, &hv_Max, &hv_Range);
			double dScale = 250.0 / (hv_Max.D()- hv_Min.D());
			double dAdd = -dScale*hv_Min.D();
			ScaleImage(hoReduce, &hoReduceScaled, dScale, dAdd);
			GrayClosingRect(hoReduceScaled, &hoReduceScaled, 25, 2);
			//BinaryThreshold(hoGrayClosing, &hoRegion, "max_separability", "dark", &hvUsedThreshold);
			VarThreshold(hoReduceScaled, &hoRegion, WIDTH/2, HEIGHT*2-1, 0.3, FitLinePara.nThr, "dark");
			ClosingRectangle1(hoRegion, &hoRegion, 2, 5);
			OpeningRectangle1(hoRegion, &hoRegion, 1, 11);
			ClosingRectangle1(hoRegion, &hoRegion, 8, 1);//横向闭运算，为了把拉丝区域合并起来
			ClosingRectangle1(hoRegion, &hoRegion, 1, 25);//纵向闭运算，为了把拉丝区域合并起来
			Connection(hoRegion, &hoRegion);
			SelectShape(hoRegion, &hoRegion, "height", "and", HEIGHT * 2 - 10, HEIGHT * 2 + 1);
			Intensity(hoRegion, hoReduceScaled, &hvMean, &HTuple());
			if (hvMean.Length() > 0)
			{
				TupleFind(hvMean, hvMean.TupleMin(), &hvInc);
				if (hvInc.Length() > 0 && hvInc != -1)
				{
					SelectObj(hoRegion, &hoBaseCoatRegion, hvInc + 1);
					SelectShapeStd(hoBaseCoatRegion, &hoBaseCoatRegion, "max_area", 70);

					HTuple hvColTmp;
					if (LT_BaseCoatCoat == eType)
					{
						SmallestRectangle1(hoBaseCoatRegion, &HTuple(), &HTuple(), &HTuple(), &hvColTmp);
						HObject hoLeftRegion, hoRightRegion;
						HTuple hvLeftMean, hvRightMean, hvLeftSig, hvRightSig;
						GenRectangle1(&hoLeftRegion, hvRow - 20, hvCol + hvColTmp - WIDTH - 5, hvRow + 20, hvCol + hvColTmp - WIDTH - 3);
						GenRectangle1(&hoRightRegion, hvRow - 20, hvCol + hvColTmp - WIDTH + 3, hvRow + 20, hvCol + hvColTmp - WIDTH + 5);
						Intensity(hoLeftRegion, Image, &hvLeftMean, &hvLeftSig);
						Intensity(hoRightRegion, Image, &hvRightMean, &hvRightSig);
						if (abs(hvRightMean.D() - hvLeftMean.D()) < FitLinePara.nThr)
						{
							hvColTmp.Clear();
						}
					}
					else if (LT_CoatBaseCoat == eType)
					{
						SmallestRectangle1(hoBaseCoatRegion, &HTuple(), &hvColTmp, &HTuple(), &HTuple());
						HObject hoLeftRegion, hoRightRegion;
						HTuple hvLeftMean, hvRightMean, hvLeftSig, hvRightSig;
						GenRectangle1(&hoLeftRegion, hvRow - 20, hvCol + hvColTmp - WIDTH - 5, hvRow + 20, hvCol + hvColTmp - WIDTH - 3);
						GenRectangle1(&hoRightRegion, hvRow - 20, hvCol + hvColTmp - WIDTH + 3, hvRow + 20, hvCol + hvColTmp - WIDTH + 5);
						Intensity(hoLeftRegion, Image, &hvLeftMean, &hvLeftSig);
						Intensity(hoRightRegion, Image, &hvRightMean, &hvRightSig);
						if (abs(hvLeftMean.D() - hvRightMean.D()) < FitLinePara.nThr)
						{
							hvColTmp.Clear();
						}
					}

					(*hvEdgeCol) = hvColTmp;
					if (hvColTmp.Length() > 0)
					{
						(*hvEdgeCol) = hvColTmp - WIDTH + hvCol;
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}
			}
		}
	}
	catch (HException&e)
	{
		return FALSE;
	}

	return TRUE;
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
		if (FALSE == _FindLine(tIn.hoImage, nSN, tLineParam, &hoL, &dStartRow, &dStartColumn, &dEndRow, &dEndColumn, TRUE, bDisp))
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
		//测量线移动位置，左基材或者右基材边找到线后，其他测量区域根据位置差移动
		double dLeftLineMoveDiff = 0, dRightLineMoveDiff = 0;
		std::vector<int> vecParam;
		//先放入基材宽
		vecParam.push_back(0);
		vecParam.push_back(m_tMeasureParam.vecLineParam.size()-1);
		//再放入极耳边
		for (int i = 1; i < m_tMeasureParam.vecLineParam.size()-1; i++)
		{
			if (LT_FoilBaseCoat == m_tMeasureParam.tLineProp.vecLineTypeList[i] ||
				LT_FoilCoat == m_tMeasureParam.tLineProp.vecLineTypeList[i] ||
				LT_FoilAT11 == m_tMeasureParam.tLineProp.vecLineTypeList[i] ||
				LT_BaseCoatFoil == m_tMeasureParam.tLineProp.vecLineTypeList[i] ||
				LT_CoatFoil == m_tMeasureParam.tLineProp.vecLineTypeList[i] ||
				LT_AT11Foil == m_tMeasureParam.tLineProp.vecLineTypeList[i])
			{
				vecParam.push_back(i);
			}
		}
		//最后放入其他边
		for (int i = 1; i < m_tMeasureParam.vecLineParam.size() - 1; i++)
		{
			if (LT_FoilBaseCoat != m_tMeasureParam.tLineProp.vecLineTypeList[i] &&
				LT_FoilCoat != m_tMeasureParam.tLineProp.vecLineTypeList[i] &&
				LT_FoilAT11 != m_tMeasureParam.tLineProp.vecLineTypeList[i] &&
				LT_BaseCoatFoil != m_tMeasureParam.tLineProp.vecLineTypeList[i] &&
				LT_CoatFoil != m_tMeasureParam.tLineProp.vecLineTypeList[i] &&
				LT_AT11Foil != m_tMeasureParam.tLineProp.vecLineTypeList[i])
			{
				vecParam.push_back(i);
			}
		}
		for (int j = 0; j < vecParam.size(); j++)
		{
			int i = vecParam[j];

			HObject hoL;
			double dStartRow(0.0), dStartColumn(0.0), dEndRow(0.0), dEndColumn(0.0);
			tagLineParam tLineParam = m_tMeasureParam.vecLineParam[i];
			if (i < vecParam.size() / 2)
			{
				tLineParam.dStartCol += dLeftLineMoveDiff;
				tLineParam.dEndCol += dLeftLineMoveDiff;
			}
			else
			{
				tLineParam.dStartCol += dRightLineMoveDiff;
				tLineParam.dEndCol += dRightLineMoveDiff;
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
						dLeftLineMoveDiff = (dStartColumn + dEndColumn) / 2 - (tLineParam.dStartCol + tLineParam.dEndCol) / 2;
					}
					else if(i == vecParam.size()-1)
					{
						dRightLineMoveDiff = (dStartColumn + dEndColumn) / 2 - (tLineParam.dStartCol + tLineParam.dEndCol) / 2;
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
			GenRectangle1(&hoRect, hvHeight / 2 - 10, 0, hvHeight / 2 + 10, hvWidth - 1);
			ReduceDomain(tIn.hoImage, hoRect, &hoReduce);
			GrayClosingRect(hoReduce, &hoImage, 1, 100);
			GrayOpeningRect(hoImage, &hoImage, 10, 1);
			GenMeasureRectangle2(hvHeight / 2, hvWidth / 2, 0, hvWidth / 2, 25, hvWidth, hvHeight, "nearest_neighbor", &hvMeasureHandl);
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

			int nLocalOffset = 30;
			if (m_tInit.tProductInfo.bExistBaseCoat && m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard!=0)
			{
				nLocalOffset = m_tInit.tProductInfo.tParam.vecBaseCoatWidth[0].dStandard / m_vecResolution[0].dColRes;
			}
			else if (m_tInit.tProductInfo.bExistAT11 && m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard != 0)
			{
				nLocalOffset = m_tInit.tProductInfo.tParam.vecAT11Width[0].dStandard / m_vecResolution[0].dColRes;
			}

			//生成测量
			for (int i = 0; i < hvLineCol.Length(); i++)
			{
				if (i == 0 || i == hvLineCol.Length() - 1)
					m_tMeasureParam.vecLineParam[i].dWidth = 2000;
				else
					m_tMeasureParam.vecLineParam[i].dWidth = 450;

				m_tMeasureParam.vecLineParam[i].dHeight = 25;
				if (i == 0 || i == hvLineCol.Length() - 1)
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D();
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D();
				}
				else if (LT_BaseCoatCoat == m_tMeasureParam.tLineProp.vecLineTypeList[i] ||
					LT_AT11Coat == m_tMeasureParam.tLineProp.vecLineTypeList[i])
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() + 30;
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() + 30;
				}
				else if (LT_CoatBaseCoat == m_tMeasureParam.tLineProp.vecLineTypeList[i] ||
					LT_CoatAT11 == m_tMeasureParam.tLineProp.vecLineTypeList[i])
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() - 30;
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() - 30;
				}
				else
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D();
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D();
				}
				/*else if (i % 2 == 0)
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() + 30;
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() + 30;
				}
				else if (i % 2 == 1)
				{
					m_tMeasureParam.vecLineParam[i].dStartCol = hvLineCol[i].D() - 30;
					m_tMeasureParam.vecLineParam[i].dEndCol = hvLineCol[i].D() - 30;
				}*/
				
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
					m_tMeasureParam.vecLineParam[i].nThr = 10;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatAT11)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
					m_tMeasureParam.vecLineParam[i].nThr = 10;

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
					m_tMeasureParam.vecLineParam[i].nThr = 30;

					m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
					m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;
				}
				else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatFoil)
				{
					m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
					m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
					m_tMeasureParam.vecLineParam[i].nThr = 30;

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
		m_tInit.pLog(strExcp, ExcpLog, NoExcp);
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

		HTuple hvLineCol, hvTmp, hvWidth, hvHeight, hvTmpRealLeft, hvTmpRealRight, hvBaseLineLeftColumn, hvBaseLineRightColumn;
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
			SmallestRectangle1(hoObjectSelected, &HTuple(), &hvBaseLineLeftColumn, &HTuple(), &HTuple());
			SelectObj(hoSelectedRegions, &hoObjectSelected, hoSelectedRegions.CountObj());
			SmallestRectangle1(hoObjectSelected, &HTuple(), &HTuple(), &HTuple(), &hvBaseLineRightColumn);
		}
		catch (HException&)
		{
			return FALSE;
		}
		if (hvBaseLineLeftColumn.Length() == 0 || hvBaseLineLeftColumn < 0) return FALSE;
		if (hvBaseLineRightColumn.Length() == 0 || hvBaseLineRightColumn < 0) return FALSE;
		

		//通过物理坐标转换像素坐标，将每一个尺寸规格都转换成对应测量的位置
		hvLineCol.Append(hvBaseLineLeftColumn);
		hvTmp = hvBaseLineLeftColumn;
		g_pAlgorithm->m_Calibration.CalPoint(hvTmp, hvTmpRealLeft);
		hvTmp = hvBaseLineRightColumn;
		g_pAlgorithm->m_Calibration.CalPoint(hvTmp, hvTmpRealRight);
		for (int i = 0; i < hvStdSize.Length(); i++)
		{
			if (i < hvStdSize.Length() / 2)
			{
				hvTmpRealLeft += hvStdSize[i].D();
				g_pAlgorithm->m_Calibration.CalPointToPixel(hvTmpRealLeft, hvTmp);
				hvLineCol.Append(hvTmp.TupleMin());
			}
			else
			{
				break;
			}
		}
		hvLineCol.Append(hvBaseLineRightColumn);
		for (int i = hvStdSize.Length()-1; i >= 0; i--)
		{
			if (i > hvStdSize.Length() / 2)
			{
				hvTmpRealRight -= hvStdSize[i].D();
				g_pAlgorithm->m_Calibration.CalPointToPixel(hvTmpRealRight, hvTmp);
				hvLineCol.Append(hvTmp.TupleMax());
			}
			else
			{
				break;
			}
		}

		TupleSort(hvLineCol, &hvLineCol);
		//生成测量线
		if (hvLineCol.Length() != m_tInit.tProductInfo.nMeasureLineNum || hvLineCol.Length() != m_tMeasureParam.vecLineParam.size()) return FALSE;

		double dRow1 = hvHeight.D() / 8;
		double dRow2 = hvHeight.D() / 8 * 7;

		//生成测量
		for (int i = 0; i < hvLineCol.Length(); i++)
		{
			if (i == 0 || i == hvLineCol.Length() - 1)
				m_tMeasureParam.vecLineParam[i].dWidth = 2000;
			else
				m_tMeasureParam.vecLineParam[i].dWidth = 450;

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

				m_tMeasureParam.vecLineParam[i].dWidth = 300;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Foil)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 15;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;

				m_tMeasureParam.vecLineParam[i].dWidth = 300;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_AT11Coat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 10;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;

				m_tMeasureParam.vecLineParam[i].dWidth = 200;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatAT11)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 10;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;

				m_tMeasureParam.vecLineParam[i].dWidth = 200;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 25;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;

				m_tMeasureParam.vecLineParam[i].dWidth = 300;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatFoil)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Dark2Light;
				m_tMeasureParam.vecLineParam[i].nThr = 25;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;

				m_tMeasureParam.vecLineParam[i].dWidth = 300;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_FoilBaseCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 30;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;

				m_tMeasureParam.vecLineParam[i].dWidth = 300;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatFoil)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 30;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;

				m_tMeasureParam.vecLineParam[i].dWidth = 300;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_BaseCoatCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Right2Left;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 10;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow1;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow2;

				m_tMeasureParam.vecLineParam[i].dWidth = 200;
			}
			else if (m_tMeasureParam.tLineProp.vecLineTypeList[i] == LT_CoatBaseCoat)
			{
				m_tMeasureParam.vecLineParam[i].nDirection = MD_Left2Right;
				m_tMeasureParam.vecLineParam[i].nTransition = MT_Light2Dark;
				m_tMeasureParam.vecLineParam[i].nThr = 10;

				m_tMeasureParam.vecLineParam[i].dStartRow = dRow2;
				m_tMeasureParam.vecLineParam[i].dEndRow = dRow1;

				m_tMeasureParam.vecLineParam[i].dWidth = 200;
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
	tPara.ePreproc = (ePreprocess)GetPrivateProfileInt("Param", "ePreproc", PRE_None, strFile);
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

	WritePrivateProfileInt("Param", "ePreproc", tPara.ePreproc, strFile);
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