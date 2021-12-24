#include "StdAfx.h"
#include "SpotInspection.h"


CSpotInspection::CSpotInspection()
{
	m_eType = IT_Rect;
}

CSpotInspection::~CSpotInspection()
{

}

void CSpotInspection::Init(const MeasureInit& tInit)
{
	m_tInit = tInit;

	m_strParamFileIni.Format("%s//Config//Public//SpotInspection//%d", GetCurrentAppPath(), m_tInit.nSN);
	CreateMultilevelPath(m_strParamFileIni);
	m_strParamFileIni.Format("%s//Config//Public//SpotInspection//%d//SpotInspection.ini", GetCurrentAppPath(), m_tInit.nSN);
	m_strParamFileROIRegion.Format("%s//Config//Public//SpotInspection//%d//InspROIRegion.hobj", GetCurrentAppPath(), m_tInit.nSN);
}

BOOL CSpotInspection::ReadParam()
{
	try
	{
		m_eType = (eInspectionType)GetPrivateProfileInt("SpotInspParam", "eInspectionType", IT_Rect, m_strParamFileIni);

		m_tParam.eColorType = (eInspectionColor)GetPrivateProfileInt("SpotInspParam", "eColorType", IC_Light, m_strParamFileIni);
		m_tParam.nThres = GetPrivateProfileInt("SpotInspParam", "nThres", 30, m_strParamFileIni);
		m_tParam.dPixelMinSize = GetPrivateProfileDouble("SpotInspParam", "dPixelMinSize", 25, m_strParamFileIni);
		m_tParam.dPixelMaxSize = GetPrivateProfileDouble("SpotInspParam", "dPixelMaxSize", 120, m_strParamFileIni);
		m_tParam.nObjectNum = GetPrivateProfileDouble("SpotInspParam", "nObjectNum", 1, m_strParamFileIni);

		GenEmptyObj(&m_tParam.hoSearchReigon);
		ReadObject(&m_tParam.hoSearchReigon, (HTuple)m_strParamFileROIRegion);
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CSpotInspection%d_ReadParam: 加载点检参数异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);

		GenEmptyObj(&m_tParam.hoSearchReigon);
		return FALSE;
	}
	return TRUE;
}

BOOL CSpotInspection::WriteParam()
{
	try
	{
		WritePrivateProfileInt("SpotInspParam", "eColorType", m_tParam.eColorType, m_strParamFileIni);
		WritePrivateProfileInt("SpotInspParam", "eInspectionType", m_eType, m_strParamFileIni);
		WritePrivateProfileInt("SpotInspParam", "nThres", m_tParam.nThres, m_strParamFileIni);
		WritePrivateProfileDouble("SpotInspParam", "dPixelMinSize", m_tParam.dPixelMinSize, m_strParamFileIni);
		WritePrivateProfileDouble("SpotInspParam", "dPixelMaxSize", m_tParam.dPixelMaxSize, m_strParamFileIni);
		WritePrivateProfileInt("SpotInspParam", "nObjectNum", m_tParam.nObjectNum, m_strParamFileIni);

		WriteObject(m_tParam.hoSearchReigon, (HTuple)m_strParamFileROIRegion);
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CSpotInspection%d_WriteParam: 写入点检参数异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
	}
	return TRUE;
}

BOOL CSpotInspection::Action(InspectionInput& tIn, InspectionOutput& tOut)
{
	m_ptOut = &tOut;
	m_ptIn = &tIn;

	InitOutputValue();

	//if (!m_tInit.tProductInfo.tParam.bUseChecking) return FALSE;

	if (IT_Rect == m_eType)
		return RectInspect(tIn, tOut);
	else if (IT_Circle == m_eType)
		return CircleInspect(tIn, tOut);

	return FALSE;
}

void CSpotInspection::InitOutputValue()
{
	GenEmptyObj(&m_ptOut->hoDisp);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptLeftTop.hv_Column);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptLeftTop.hv_Row);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptLeftBottom.hv_Column);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptLeftBottom.hv_Row);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptRightTop.hv_Column);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptRightTop.hv_Row);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptRightBottom.hv_Column);
	TupleGenConst(m_tInit.hvCamSNList.Length(), NO_DATA, &m_ptOut->ptRightBottom.hv_Row);
}

BOOL CSpotInspection::CircleInspect(const InspectionInput& tIn, InspectionOutput& tOut)
{
	HObject  ho_ImageSrc, ho_ImageRoi, ho_ImageCrop, ho_Border, hoRegion, hoRegionOpening, ho_Roi;
	HObject  ho_SelectedXLD;

	HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_Area, hvUsedThreshold;
	HTuple  hv_Row, hv_Column, hv_PointOrder, hv_Row3, hv_Column3;
	HTuple  hv_Phi, hv_Length1, hv_Length2, hv_rectLen1, hv_rectLen2;
	HTuple  hv_MetrologyHandle, hv_MetroIndex, hv_Result;
	HTuple  hv_Width, hv_Height;
	GetImageSize(tIn.hoImage, &hv_Width, &hv_Height);
	try
	{
		//暂时只写1个点检目标
		InspectionOutput tOutTmp;
		GenEmptyObj(&tOut.hoDisp);
		GenEmptyObj(&tOutTmp.hoDisp);

		if (m_tParam.hoSearchReigon.CountObj() == 0) return FALSE;

		for (int i = 0; i < m_tParam.hoSearchReigon.CountObj(); i++)
		{
			HObject hoSelectObj;
			SelectObj(m_tParam.hoSearchReigon, &hoSelectObj, i + 1);
			SmallestRectangle1(hoSelectObj, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
			if (hv_Column2- hv_Column1 < 10)
			{
				tOutTmp.ptCenter.hv_Column.Append(NO_DATA);
				tOutTmp.ptCenter.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Row.Append(NO_DATA);
				continue;
			}
			GenRectangle1(&ho_Roi, 0, hv_Column1, hv_Height - 1, hv_Column2);
			ReduceDomain(tIn.hoImage, ho_Roi, &ho_ImageRoi);
			CropDomain(ho_ImageRoi, &ho_ImageCrop);
			GrayClosingRect(ho_ImageCrop, &ho_ImageCrop, 5, 1);
			GrayClosingRect(ho_ImageCrop, &ho_ImageCrop, 1, 5);

			if (m_tParam.eColorType == IC_Light)
			{
				//获取亮区域点检目标
				GenEmptyObj(&hoRegionOpening);
				ThresholdSubPix(ho_ImageCrop, &ho_Border, m_tParam.nThres);
				SelectShapeXld(ho_Border, &ho_SelectedXLD, HTuple("width").Append("height"), "and", HTuple(m_tParam.dPixelMinSize).Append(m_tParam.dPixelMinSize),
					HTuple(m_tParam.dPixelMaxSize).Append(m_tParam.dPixelMaxSize));
				SelectShapeXld(ho_SelectedXLD, &ho_SelectedXLD, HTuple("row1").Append("row2"), "and", HTuple(50).Append(50),
					HTuple(hv_Height - 50).Append(hv_Height - 50));
				SelectShapeXld(ho_SelectedXLD, &ho_SelectedXLD, "circularity", "and", 0.3, 1);
				GenRegionContourXld(ho_SelectedXLD, &hoRegion, "filled");
				HObject hoRegionSel;
				GenEmptyObj(&hoRegionSel);
				for (int i = 0; i < hoRegion.CountObj(); i++)
				{
					HObject hoRgnTmp;
					SelectGray(hoRegion.SelectObj(i+1), ho_ImageCrop, &hoRgnTmp, "mean", "and", min(m_tParam.nThres, 255), 255);
					if (hoRgnTmp.CountObj() > 0)
					{
						ConcatObj(hoRegionSel, ho_SelectedXLD.SelectObj(i + 1), &hoRegionSel);
					}
				}
				if (hoRegionSel.CountObj() > 1)
				{
					HTuple hvArea, hvRow, hvInc;
					AreaCenterXld(hoRegionSel, &hvArea, &hvRow, &HTuple(), &HTuple());
					TupleSortIndex(hvRow, &hvInc);
					ConcatObj(hoRegionOpening, hoRegionSel.SelectObj(hvInc[hvInc.Length() - 1] + 1), &hoRegionOpening);
					ConcatObj(hoRegionOpening, hoRegionSel.SelectObj(hvInc[hvInc.Length() - 2] + 1), &hoRegionOpening);
					SortContoursXld(hoRegionOpening, &hoRegionOpening, "upper_left", "true", "column");
				}
			}
			else
			{
				//获取暗区域点检目标
				GenEmptyObj(&hoRegionOpening);
				ThresholdSubPix(ho_ImageCrop, &ho_Border, m_tParam.nThres);
				SelectShapeXld(ho_Border, &ho_SelectedXLD, HTuple("width").Append("height"), "and", HTuple(m_tParam.dPixelMinSize).Append(m_tParam.dPixelMinSize),
					HTuple(m_tParam.dPixelMaxSize).Append(m_tParam.dPixelMaxSize));
				SelectShapeXld(ho_SelectedXLD, &ho_SelectedXLD, HTuple("row1").Append("row2"), "and", HTuple(50).Append(50),
					HTuple(hv_Height-50).Append(hv_Height - 50));
				SelectShapeXld(ho_SelectedXLD, &ho_SelectedXLD, "circularity", "and", 0.3, 1);
				GenRegionContourXld(ho_SelectedXLD, &hoRegion, "filled");
				HObject hoRegionSel;
				GenEmptyObj(&hoRegionSel);
				for (int i = 0; i < hoRegion.CountObj(); i++)
				{
					HObject hoRgnTmp;
					SelectGray(hoRegion.SelectObj(i + 1), ho_ImageCrop, &hoRgnTmp, "mean", "and", 0, min(m_tParam.nThres, 255));
					if (hoRgnTmp.CountObj() > 0)
					{
						ConcatObj(hoRegionSel, ho_SelectedXLD.SelectObj(i + 1), &hoRegionSel);
					}
				}
				if (hoRegionSel.CountObj() > 1)
				{
					HTuple hvArea, hvRow, hvInc;
					AreaCenterXld(hoRegionSel, &hvArea, &hvRow, &HTuple(), &HTuple());
					TupleSortIndex(hvRow, &hvInc);
					ConcatObj(hoRegionOpening, hoRegionSel.SelectObj(hvInc[hvInc.Length() - 1] + 1), &hoRegionOpening);
					ConcatObj(hoRegionOpening, hoRegionSel.SelectObj(hvInc[hvInc.Length() - 2] + 1), &hoRegionOpening);
					SortContoursXld(hoRegionOpening, &hoRegionOpening, "upper_left", "true", "column");
				}
			}

			if (hoRegionOpening.CountObj() != 2)
			{
				tOutTmp.ptCenter.hv_Column.Append(NO_DATA);
				tOutTmp.ptCenter.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Row.Append(NO_DATA);
				continue;
			}

			SmallestRectangle1Xld(hoRegionOpening, &hv_Row, &hv_Column, &hv_Row2, &hv_Column2);

			//一定概率的检测，点检目标中心不落在图像边缘的区域，才计算尺寸，防止点检目标没拍全
			if (hv_Row2[0] > hv_Height - 50 || hv_Row[0] < 50 ||
				hv_Row2[1] > hv_Height - 50 || hv_Row[1] < 50)
			{
				tOutTmp.ptCenter.hv_Column.Append(NO_DATA);
				tOutTmp.ptCenter.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Row.Append(NO_DATA);
				continue;
			}

			//计算点检尺寸
			//提取测量圆形左右点坐标
			HTuple hvCenterRow, hvCenterCol, hvPointOrder;
			AreaCenterXld(hoRegionOpening, &hv_Area, &hvCenterRow, &hvCenterCol, &hvPointOrder);

			tOutTmp.ptCenter.hv_Row.Append(hvCenterRow.TupleMean());
			tOutTmp.ptCenter.hv_Column.Append(hvCenterCol.TupleMean() + hv_Column1);
			tOutTmp.ptLeftTop.hv_Row.Append(hv_Row.TupleMean());
			tOutTmp.ptLeftTop.hv_Column.Append(hvCenterCol[0] + hv_Column1);
			tOutTmp.ptRightBottom.hv_Row.Append(hv_Row2.TupleMean());
			tOutTmp.ptRightBottom.hv_Column.Append(hvCenterCol[1] + hv_Column1);


			HObject  ho_Region;
			GenRegionContourXld(hoRegionOpening, &ho_Region, "filled");
			MoveRegion(ho_Region, &ho_Region, 0, hv_Column1);
			Union1(ho_Region, &ho_Region);
			ConcatObj(tOutTmp.hoDisp, ho_Region, &tOutTmp.hoDisp);
		}

		if (tOutTmp.ptCenter.hv_Column.Length() > 0) tOut = tOutTmp;
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CSpotInspection_RectInspect: 标定点检异常 %s [%s]", e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}
	return TRUE;
}

BOOL CSpotInspection::RectInspect(const InspectionInput& tIn, InspectionOutput& tOut)
{
	
	HObject  ho_ImageSrc, ho_ImageRoi, ho_ImageCrop, ho_Border,hoRegion,hoRegionOpening,ho_Roi;
	HObject  ho_SelectedXLD;

	HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_Area, hvUsedThreshold;
	HTuple  hv_Row, hv_Column, hv_PointOrder, hv_Row3, hv_Column3;
	HTuple  hv_Phi, hv_Length1, hv_Length2, hv_rectLen1, hv_rectLen2;
	HTuple  hv_MetrologyHandle,hv_MetroIndex,hv_Result;
	HTuple  hv_Width, hv_Height;
	GetImageSize(tIn.hoImage, &hv_Width, &hv_Height);
	try
	{
		//暂时只写1个点检目标
		InspectionOutput tOutTmp;
		GenEmptyObj(&tOut.hoDisp);
		GenEmptyObj(&tOutTmp.hoDisp);

		if (m_tParam.hoSearchReigon.CountObj() == 0) return FALSE;

		for (int i = 0; i < m_tParam.hoSearchReigon.CountObj(); i++)
		{
			HObject hoSelectObj;
			SelectObj(m_tParam.hoSearchReigon, &hoSelectObj, i + 1);
			SmallestRectangle1(hoSelectObj, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
			GenRectangle1(&ho_Roi, 0, hv_Column1, hv_Height - 1, hv_Column2);
			ReduceDomain(tIn.hoImage, ho_Roi, &ho_ImageRoi);
			CropDomain(ho_ImageRoi, &ho_ImageCrop);

			if (m_tParam.eColorType == IC_Light)
			{
				//获取亮区域点检目标
				VarThreshold(ho_ImageCrop, &hoRegion, min(100, hv_Column2 - hv_Column1), 100, 0.5, 5, "light");
				//BinaryThreshold(ho_ImageRoi, &hoRegion, "max_separability", "light", &hvUsedThreshold);
				OpeningRectangle1(hoRegion, &hoRegionOpening, 1, 10);
				OpeningRectangle1(hoRegionOpening, &hoRegionOpening, 10, 1);
				Connection(hoRegionOpening, &hoRegionOpening);
				FillUp(hoRegionOpening, &hoRegionOpening);
				SelectShape(hoRegionOpening, &hoRegionOpening, HTuple("width").Append("height"), "and", HTuple(m_tParam.dPixelMinSize).Append(m_tParam.dPixelMinSize),
					HTuple(m_tParam.dPixelMaxSize).Append(m_tParam.dPixelMaxSize));
				SelectShape(hoRegionOpening, &hoRegionOpening, "rectangularity", "and", 0.9, 1);
				//改成找最上面的
				if (hoRegionOpening.CountObj() > 0)
				{
					SortRegion(hoRegionOpening, &hoRegionOpening, "first_point", "true", "row");
					SelectObj(hoRegionOpening, &hoRegionOpening, 1);
				}
				//SelectShapeStd(hoRegionOpening, &hoRegionOpening, "max_area", 70);
				MoveRegion(hoRegionOpening, &hoRegionOpening, 0, hv_Column1);
			}
			else
			{
				//获取暗区域点检目标
				VarThreshold(ho_ImageCrop, &hoRegion, min(100, hv_Column2 - hv_Column1), 100, 0.5, 5, "dark");
				//BinaryThreshold(ho_ImageRoi, &hoRegion, "max_separability", "dark", &hvUsedThreshold);
				OpeningRectangle1(hoRegion, &hoRegionOpening, 1, 10);
				OpeningRectangle1(hoRegionOpening, &hoRegionOpening, 10, 1);
				Connection(hoRegionOpening, &hoRegionOpening);
				FillUp(hoRegionOpening, &hoRegionOpening);
				SelectShape(hoRegionOpening, &hoRegionOpening, HTuple("width").Append("height"), "and", HTuple(m_tParam.dPixelMinSize).Append(m_tParam.dPixelMinSize),
					HTuple(m_tParam.dPixelMaxSize).Append(m_tParam.dPixelMaxSize));
				SelectShape(hoRegionOpening, &hoRegionOpening, "rectangularity", "and", 0.9, 1);
				//改成找最上面的
				if (hoRegionOpening.CountObj() > 0)
				{
					SortRegion(hoRegionOpening, &hoRegionOpening, "first_point", "true", "row");
					SelectObj(hoRegionOpening, &hoRegionOpening, 1);
				}
				//SelectShapeStd(hoRegionOpening, &hoRegionOpening, "max_area", 70);
				MoveRegion(hoRegionOpening, &hoRegionOpening, 0, hv_Column1);
			}

			if (hoRegionOpening.CountObj() == 0)
			{
				tOutTmp.ptLeftTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Row.Append(NO_DATA);
				continue;
			}

			SmallestRectangle2(hoRegionOpening, &hv_Row, &hv_Column, &hv_Phi, &hv_Length1, &hv_Length2);

			HTuple hvTransition = "positive";
			if (m_tParam.eColorType == IC_Light)
				hvTransition = "negative";

			CreateMetrologyModel(&hv_MetrologyHandle);
			SetMetrologyModelImageSize(hv_MetrologyHandle, hv_Width, hv_Height);
			AddMetrologyObjectRectangle2Measure(hv_MetrologyHandle, hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2, hv_Length1 / 2, hv_Length2 / 5, 2, m_tParam.nThres, HTuple(), HTuple(), &hv_MetroIndex);
			SetMetrologyObjectParam(hv_MetrologyHandle, hv_MetroIndex, "min_score", 0.6);
			SetMetrologyObjectParam(hv_MetrologyHandle, hv_MetroIndex, "num_measures", 5);
			SetMetrologyObjectParam(hv_MetrologyHandle, hv_MetroIndex, "measure_select", "all");
			SetMetrologyObjectParam(hv_MetrologyHandle, hv_MetroIndex, "measure_transition", hvTransition);
			ApplyMetrologyModel(ho_ImageRoi, hv_MetrologyHandle);

			GetMetrologyObjectResult(hv_MetrologyHandle, hv_MetroIndex, "all", "result_type", "all_param", &hv_Result);
			GetMetrologyObjectResultContour(&ho_SelectedXLD, hv_MetrologyHandle, hv_MetroIndex, "all", 1.5);

			//如果找到了矩形点检目标
			if (ho_SelectedXLD.CountObj() != 1 || hv_Result.Length() != 5)
			{
				tOutTmp.ptLeftTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Row.Append(NO_DATA);
				continue;
			}

			HTuple hvCenterRow, hvCenterCol, hvPhi, hvLen1, hvLen2;
			hvCenterRow = hv_Result[0];
			hvCenterCol = hv_Result[1];
			hvPhi = hv_Result[2];
			hvLen1 = hv_Result[3];
			hvLen2 = hv_Result[4];

			//一定概率的检测，点检目标中心不落在图像边缘的区域，才计算尺寸，防止点检目标没拍全
			if (hvCenterRow > hv_Height - 200 || hvCenterRow < 200)
			{
				tOutTmp.ptLeftTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightTop.hv_Row.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptRightBottom.hv_Row.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Column.Append(NO_DATA);
				tOutTmp.ptLeftBottom.hv_Row.Append(NO_DATA);
				continue;
			}

			//计算点检尺寸
			//提取测量矩形的四个顶点坐标
			HTuple hvCos, hvSin, hvLTX, hvLTY, hvRTX, hvRTY, hvRBX, hvRBY, hvLBX, hvLBY;
			//角度小于0，旋转90度
			if (hvPhi.D() < -90 && hvPhi.D() > -PI)
			{
				hvPhi += PI / 2;
				hvLen1 = hv_Result[4];
				hvLen2 = hv_Result[3];
			}
			
			TupleCos(hvPhi, &hvCos);
			TupleSin(hvPhi, &hvSin);

			//左下点
			hvLBX = hvCenterCol + (-hvLen1*hvCos - hvLen2*hvSin);
			hvLBY = hvCenterRow - (-hvLen1*hvSin + hvLen2*hvCos);
			//左上点
			hvLTX = hvCenterCol + (hvLen1*hvCos - hvLen2*hvSin);
			hvLTY = hvCenterRow - (hvLen1*hvSin + hvLen2*hvCos);
			//右上点
			hvRTX = hvCenterCol + (hvLen1*hvCos + hvLen2*hvSin);
			hvRTY = hvCenterRow - (hvLen1*hvSin - hvLen2*hvCos);
			//右下点
			hvRBX = hvCenterCol + (-hvLen1*hvCos + hvLen2*hvSin);
			hvRBY = hvCenterRow - (-hvLen1*hvSin - hvLen2*hvCos);

			tOutTmp.ptCenter.hv_Row.Append(hvCenterRow);
			tOutTmp.ptCenter.hv_Column.Append(hvCenterCol);
			tOutTmp.ptLeftTop.hv_Column.Append(hvLTX);
			tOutTmp.ptLeftTop.hv_Row.Append(hvLTY);
			tOutTmp.ptRightTop.hv_Column.Append(hvRTX);
			tOutTmp.ptRightTop.hv_Row.Append(hvRTY);
			tOutTmp.ptRightBottom.hv_Column.Append(hvRBX);
			tOutTmp.ptRightBottom.hv_Row.Append(hvRBY);
			tOutTmp.ptLeftBottom.hv_Column.Append(hvLBX);
			tOutTmp.ptLeftBottom.hv_Row.Append(hvLBY);


			HObject  ho_Region;
			GenRegionContourXld(ho_SelectedXLD, &ho_Region, "filled");
			ConcatObj(tOutTmp.hoDisp, ho_Region, &tOutTmp.hoDisp);
		}

		if (tOutTmp.ptLeftTop.hv_Column.Length() > 0) tOut = tOutTmp;
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CSpotInspection_RectInspect: 标定点检异常 %s [%s]", e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}
	return TRUE;
}

BOOL CSpotInspection::AutoGenSpotInspectParm(SpotInspInput *pIn, InspectionOutput* pOut)
{
	InspectionParam inspParm;
	//自动计算参数
	// Local iconic variables
	GenEmptyObj(&m_tParam.hoSearchReigon);
	HObject  ho_Image, ho_Region, ho_ConnectedRegions;
	HObject  ho_SelectedRegions, ho_Rectangle, ho_SelectedRegionsRect;
	HObject  ho_Rectangle1, ho_Rectangle2;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_Number, hv_Area;
	HTuple  hv_Row, hv_Column, hv_r, hv_Number1, hv_AreaRect;
	HTuple  hv_RowRect, hv_ColumnRect, hv_HeightRect, hv_WidthRect;
	HTuple  hv_Ratio;

	GetImageSize(pIn->hoImage, &hv_Width, &hv_Height);
	Threshold(pIn->hoImage, &ho_Region, 225, 255);   //暂时用225的参数二值化
	Connection(ho_Region, &ho_ConnectedRegions);

	if (IT_Rect == m_eType)
	{
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegionsRect, (HTuple("area").Append("ratio")),
			"and", (HTuple(m_tParam.dPixelMinSize).Append(0)), (HTuple(9999999).Append(1.2)));
		CountObj(ho_SelectedRegionsRect, &hv_Number1);
		//1个点检目标
		if (0 != (int(hv_Number1 == 1)))
		{
			AreaCenter(ho_SelectedRegionsRect, &hv_AreaRect, &hv_RowRect, &hv_ColumnRect);
			HeightWidthRatio(ho_SelectedRegionsRect, &hv_HeightRect, &hv_WidthRect, &hv_Ratio);
			if (hv_ColumnRect.Length() == 1)
			{
				if (0 != (int(hv_ColumnRect > hv_Width / 2)))
				{
					GenRectangle1(&ho_Rectangle1, hv_HeightRect, hv_ColumnRect - (hv_WidthRect*1.5),
						hv_Height - hv_HeightRect, hv_Width - 1);
				}
				else
				{
					GenRectangle1(&ho_Rectangle1, hv_HeightRect, 0, hv_Height - hv_HeightRect, hv_ColumnRect + (hv_WidthRect*1.5));
				}
				ConcatObj(m_tParam.hoSearchReigon, ho_Rectangle1, &m_tParam.hoSearchReigon);
				m_tParam.nThres = 225;
				m_tParam.nObjectNum = 1;
				pOut->hoDisp = m_tParam.hoSearchReigon;
			}
		}
		//2个点检目标
		else if (0 != (int(hv_Number1 == 2)))
		{
			//L
			AreaCenter(ho_SelectedRegionsRect, &hv_AreaRect, &hv_RowRect, &hv_ColumnRect);
			HeightWidthRatio(ho_SelectedRegionsRect, &hv_HeightRect, &hv_WidthRect, &hv_Ratio);
			if (hv_ColumnRect.Length() == 2)
			{
				//left rect
				if (0 != (int(hv_ColumnRect[0] > hv_Width / 2)))
				{
					GenRectangle1(&ho_Rectangle1, hv_HeightRect[0], hv_ColumnRect[0] - (hv_WidthRect[0] * 1.5),
						hv_Height - hv_HeightRect[0], hv_Width - 1);
				}
				else
				{
					GenRectangle1(&ho_Rectangle1, hv_HeightRect[0], 0, hv_Height - hv_HeightRect[0], hv_ColumnRect[0] + (hv_WidthRect[0] * 1.5));
				}
				//right rect
				if (0 != (int(hv_ColumnRect[1] > hv_Width / 2)))
				{
					GenRectangle1(&ho_Rectangle2, hv_HeightRect[1], hv_ColumnRect[1] - (hv_WidthRect[1] * 1.5),
						hv_Height - hv_HeightRect[1], hv_Width - 1);
				}
				else
				{
					GenRectangle1(&ho_Rectangle2, hv_HeightRect[1], 0, hv_Height - hv_HeightRect[1], hv_ColumnRect[1] + (hv_WidthRect[1] * 1.5));
				}

				ConcatObj(m_tParam.hoSearchReigon, ho_Rectangle1, &m_tParam.hoSearchReigon);
				ConcatObj(m_tParam.hoSearchReigon, ho_Rectangle2, &m_tParam.hoSearchReigon);
				m_tParam.nThres = 225;
				m_tParam.nObjectNum = 2;
				pOut->hoDisp = m_tParam.hoSearchReigon;
			}
		}
		else
		{
			return 0;            //0表示点检自动生成参数失败
		}
	}
	// 圆暂且不写2个目标
	else if (IT_Circle == m_eType)
	{
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, (HTuple("area").Append("circularity")),
			"and", (HTuple(300).Append(0.6)), (HTuple(9999999).Append(1)));
		CountObj(ho_SelectedRegions, &hv_Number);
		if (0 != (int(hv_Number == 1)))
		{
			AreaCenter(ho_SelectedRegions, &hv_Area, &hv_Row, &hv_Column);
			hv_r = (hv_Area / 3.14).TupleSqrt();
			if (0 != (int(hv_Column > hv_Width / 2)))
			{
				GenRectangle1(&ho_Rectangle, 1.5*hv_r, (hv_Column - hv_r) - hv_r, hv_Height - (1.5*hv_r),
					hv_Width - 1);
			}
			else
			{
				GenRectangle1(&ho_Rectangle, 1.5*hv_r, 0, hv_Height - (1.5*hv_r), (hv_Column + hv_r) + hv_r);
			}

			ConcatObj(m_tParam.hoSearchReigon, ho_Rectangle, &m_tParam.hoSearchReigon);
			m_tParam.nThres = 225;

		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
