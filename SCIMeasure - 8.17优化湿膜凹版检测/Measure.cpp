#include "StdAfx.h"
#include "Measure.h"
#include "DlgParam.h"
#include "SCISeparate.h"
#include <fstream>

// 通过导出函数形式向调用方提供指向派生类对象的基类指针
extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CMeasure();
}

CMeasure* g_pAlgorithm;
CMeasure::CMeasure(void)
{
	g_pAlgorithm = this;
	InitializeCriticalSection(&m_csLock);
	m_nNoLineCount = 0;
}
CMeasure::~CMeasure(void)
{

}

CMeasure& CMeasure::operator= (const CMeasure& in)
{
	m_tInit = in.m_tInit;
	m_tPreprocParam = in.m_tPreprocParam;
	m_tMeasureAdvanceParam = in.m_tMeasureAdvanceParam;

	m_LineMeasure = in.m_LineMeasure;
	m_Calibration = in.m_Calibration;
	m_Defect = in.m_Defect;
	m_SpotInspection = in.m_SpotInspection;
	m_tJZ = in.m_tJZ;
	return *this;
}

int CMeasure::Init(MeasureInit &tIn)
{
	//初始化
	m_tInit = tIn;
	m_Calibration.Init(tIn);
	m_LineMeasure.Init(tIn);
	m_Defect.Init(tIn);
	m_SpotInspection.Init(m_tInit);

	//读取检测参数
	ReadParam();

	/*HTuple hvY, hvX;
	ReadTuple("d://r.tup", &hvY);
	TupleGenSequence(0, hvY.Length() - 1, 1, &hvX);
	CPolyFit::tagFitResult res;
	m_PolyFit.BinomialFit(hvX, hvY, res);
	double d1 = res.dRSquare;
	HTuple hvs = res.hvCoefficients;*/

	m_LineMeasure.m_hvOverColumn = m_Calibration.GetStripeParam().hvColumnOver;
	m_LineMeasure.SetResolution(m_Calibration.GetResolution());
	m_Defect.m_hvOverColumn = m_Calibration.GetStripeParam().hvColumnOver;
	m_Defect.SetResolution(m_Calibration.GetResolution());
	m_SpotInspection.SetResolution(m_Calibration.GetResolution());

	m_Calibration.CalStripeTmpParam();

	return TRUE;
}

void CMeasure::_GetBlackMark(HObject ho_Image, HObject ho_ROI_0, HObject *ho_SelectedRegions)
{
	// Local iconic variables
	HObject  ho_ROI_0Out, ho_ImageReduced, ho_Region;
	HObject  ho_ConnectedRegions;
	try
	{
		// Local control variables
		ErosionRectangle1(ho_ROI_0, &ho_ROI_0Out, 50, 1);
		ReduceDomain(ho_Image, ho_ROI_0Out, &ho_ImageReduced);
		//Threshold(ho_ImageReduced, &ho_Region, 0, 140);
		VarThreshold(ho_ImageReduced, &ho_Region, 200, 200, 0.2, 3, "dark");

		ClosingRectangle1(ho_Region, &ho_Region, 11, 11);
		Connection(ho_Region, &ho_ConnectedRegions);
		//SelectShape(ho_ConnectedRegions, &ho_ConnectedRegions, "circularity", "and", 0.5, 1);
		SelectShape(ho_ConnectedRegions, &ho_ConnectedRegions, "width", "and", 25, 300);
		SelectShape(ho_ConnectedRegions, &ho_ConnectedRegions, "height", "and", 15, 300);
		SelectShape(ho_ConnectedRegions, &(*ho_SelectedRegions), "area", "and", 200, 99999);
	}
	catch (HException&)
	{
		GenEmptyObj(&(*ho_SelectedRegions));
	}

	return;
}

int CMeasure::DoSomething(void* message, void* wParam, void* lParam)
{
	CString strMessage;
	strMessage.Format("%s", (char*)message);
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage || "Init" == strMessage)//初始化
	{
		return Init(*(PMeasureInit)wParam);
	}
	else if ("ACTION" == strMessage || "Action" == strMessage)
	{
		//检测
		CLockHelper lk(&m_csLock);
		int nReturn = Action(*(PMeasureInput)wParam, (PMeasureOutput)lParam);

		return nReturn;
	}
	else if ("SHOW" == strMessage || "Show" == strMessage)
	{
		CString strSubMessag = "";
		if (lParam != NULL)
			strSubMessag.Format("%s", (char*)lParam);
		return DisplaySetDialog(*(PMeasureInput)wParam, strSubMessag);
	}
	else if ("SETPRODUCTINFO" == strMessage || "SetProductInfo" == strMessage)
	{
		return SetProductInfo(*(tagPDInfo*)wParam);
	}
	else if ("CALIBRATION" == strMessage || "Calibration" == strMessage)
	{
		//实现标定接口
		//CLockHelper lk(&m_csLock);
		//标定
		BOOL bRet = m_Calibration.Calibrate(*(CCalibration::CalInput*)wParam, *(CCalibration::CalOutput*)lParam);

		return bRet;
	}
	else if ("CALDISTANCE" == strMessage || "CalDistance" == strMessage)
	{
		//实现计算距离接口
		//CLockHelper lk(&m_csLock);
		//计算距离
		m_Calibration.CalDistance(*(CCalibration::CalDisPPInput*)wParam, *(CCalibration::CalDisPPOutput*)lParam);

		return TRUE;
	}
	else if ("CALDISTANCEVERITY" == strMessage || "CalDistanceVerity" == strMessage)
	{
		//实现计算距离接口
		//CLockHelper lk(&m_csLock);
		//计算距离
		m_Calibration.CalDistancePP(*(CCalibration::CalDisPPInput*)wParam, *(CCalibration::CalDisPPOutput*)lParam);

		return TRUE;
	}
	else if ("SPOTINSPECT" == strMessage || "SpotInspect" == strMessage)
	{
		//实现点检接口
		//CLockHelper lk(&m_csLock);
		//点检
		m_SpotInspection.Action(*(CSpotInspection::InspectionInput*)wParam, *(CSpotInspection::InspectionOutput*)lParam);

		return TRUE;
	}
	else if ("GENSPOTINSPECTPARM" == strMessage || "GenSpotInspectParm" == strMessage)
	{
		int nRet = 0;
		//CLockHelper lk(&m_csLock);
		nRet = m_SpotInspection.AutoGenSpotInspectParm((CSpotInspection::SpotInspInput*)wParam, (CSpotInspection::InspectionOutput*)lParam);
		//m_SpotInspection.ReadParam();   //写入或自动生成参数后需要读取一下参数，否则有问题
		//if (nRet) //点检区域生成成功，则写入到region中
		//{
		//	m_SpotInspection.WriteParam();
		//}

		return nRet;
	}
	else if ("GENMEASURELINE" == strMessage || "GenMeasureLine" == strMessage)
	{
		int nRet = 0;
		//实现自动画测量线
		CLockHelper lk(&m_csLock);
		nRet = m_LineMeasure.AutoGenMeasureLine(*(CLineMeasure::GenLineInput*)wParam, *(CLineMeasure::GenLineOutput*)lParam);

		return nRet;
	}
	else if ("CALCOLPOINT" == strMessage || "CalColPoint" == strMessage)
	{
		return m_Calibration.CalPoint(*(HTuple*)wParam, *(HTuple*)lParam);
	}
	else if ("GENMEASURELINEWITHSTDVAL" == strMessage || "GenMeasureLineWithStdVal" == strMessage)
	{
		int nRet = 0;
		//实现自动画测量线
		CLockHelper lk(&m_csLock);
		nRet = m_LineMeasure.AutoGenMeasureLineWithStdVal(*(CLineMeasure::GenLineInput*)wParam, *(CLineMeasure::GenLineOutput*)lParam);

		return nRet;
	}
	else if ("GETKVALUE" == strMessage || "GetKValue" == strMessage)
	{
		//获取K值
		*(std::vector<double>*)lParam = m_Calibration.m_tStripeParam.vecKValue;
		return TRUE;
	}
	else if ("SETKVALUE" == strMessage || "SetKValue" == strMessage)
	{
		//设置K值
		CLockHelper lk(&m_csLock);
		m_Calibration.m_tStripeParam.vecKValue = *(std::vector<double>*)wParam;
		m_Calibration.WriteParam();
		return TRUE;
	}
	else if ("GETINTERNOFFSET" == strMessage || "GetInternOffset" == strMessage)
	{
		//获取双相机交界补偿
		//CLockHelper lk(&m_csLock);
		*(double*)lParam = m_Calibration.m_tStripeParam.dInternOffset;
		return TRUE;
	}
	else if ("SETINTERNOFFSET" == strMessage || "SetInternOffset" == strMessage)
	{
		//设置双相机交界补偿
		CLockHelper lk(&m_csLock);
		m_Calibration.m_tStripeParam.dInternOffset = *(double*)wParam;
		m_Calibration.WriteParam();
		return TRUE;
	}
	else if ("GETADAPTEDIMAGE" == strMessage || "GetAdaptedImage" == strMessage)//拷贝一个副本
	{
		*(HObject*)lParam = m_Defect.m_hoAdaptedImage;

		return TRUE;
	}
	else if ("COPYOBJECTS" == strMessage || "CopyObjects" == strMessage)//拷贝一个副本
	{
		SaveParamLog(*this, *(CMeasure*)wParam);
		CLockHelper lk(&m_csLock);
		*this = *(CMeasure*)wParam;

		return TRUE;
	}
	else if ("COPYFILES" == strMessage || "CopyFiles" == strMessage)//拷贝一个副本
	{
		CString strSrc, strDst;
		strSrc.Format("%s\\Config\\Products\\%s\\SCIMeasure\\%d", GetCurrentAppPath(), wParam,
			m_tInit.nSN);
		strDst.Format("%s\\Config\\Products\\%s\\SCIMeasure\\%d", GetCurrentAppPath(), lParam,
			m_tInit.nSN);
		CreateMultilevelPath(strDst);
		CopyFolder(strSrc, strDst);

		return TRUE;
	}
	else if ("DELETEFILES" == strMessage || "DeleteFiles" == strMessage)//拷贝一个副本
	{
		CString strDst;
		strDst.Format("%s\\Config\\Products\\%s\\SCIMeasure\\%d", GetCurrentAppPath(), wParam,
			m_tInit.nSN);
		DeleteFolder(strDst);
		HTuple hvFiles;
		strDst.Format("%s\\Config\\Products\\%s\\SCIMeasure", GetCurrentAppPath(), wParam);

		try
		{
			ListFiles(HTuple(strDst), "directories", &hvFiles);
			if (0 == hvFiles.Length())//如果里面没文件了，把上一级目录删除掉
			{
				DeleteFolder(strDst);
			}
		}
		catch (HException e)
		{
		}
		return TRUE;
	}
	else
	{
		return FALSE;
		//AfxMessageBox("CMeasure Message 异常");
	}
	return TRUE;
}

BOOL CMeasure::SetProductInfo(tagPDInfo tInfo)
{
	m_tInit.tProductInfo = tInfo;
	m_Calibration.SetProductInfo(tInfo);
	m_LineMeasure.SetProductInfo(tInfo);
	m_Defect.SetProductInfo(tInfo);
	m_SpotInspection.SetProductInfo(tInfo);

	return TRUE;
}

HObject CMeasure::_GenSingleRegion(HTuple hvLine1Row1, HTuple hvLine1Col1, HTuple hvLine1Row2, HTuple hvLine1Col2,
	HTuple hvLine2Row1, HTuple hvLine2Col1, HTuple hvLine2Row2, HTuple hvLine2Col2)
{
	HObject hoLine, hoLineTmp;
	GenEmptyObj(&hoLine);
	HTuple hvRowBegin1, hvColBegin1, hvRowEnd1, hvColEnd1, hvRowBegin2, hvColBegin2, hvRowEnd2, hvColEnd2;
	IntersectionLl(hvLine1Row1, hvLine1Col1, hvLine1Row2, hvLine1Col2, 0, 0, 0, m_hvImageWidth - 1, &hvRowBegin1, &hvColBegin1, NULL);
	IntersectionLl(hvLine1Row1, hvLine1Col1, hvLine1Row2, hvLine1Col2, m_hvImageHeight - 1, 0, m_hvImageHeight - 1, m_hvImageWidth - 1, &hvRowEnd1, &hvColEnd1, NULL);
	hvRowBegin1 = min(max(hvRowBegin1, 0), m_hvImageHeight - 1);
	hvColBegin1 = min(max(hvColBegin1, 0), m_hvImageWidth - 1);
	hvRowEnd1 = min(max(hvRowEnd1, 0), m_hvImageHeight - 1);
	hvColEnd1 = min(max(hvColEnd1, 0), m_hvImageWidth - 1);
	GenRegionLine(&hoLineTmp, hvRowBegin1, hvColBegin1, hvRowEnd1, hvColEnd1);
	ConcatObj(hoLine, hoLineTmp, &hoLine);

	IntersectionLl(hvLine2Row1, hvLine2Col1, hvLine2Row2, hvLine2Col2, 0, 0, 0, m_hvImageWidth - 1, &hvRowBegin2, &hvColBegin2, NULL);
	IntersectionLl(hvLine2Row1, hvLine2Col1, hvLine2Row2, hvLine2Col2, m_hvImageHeight - 1, 0, m_hvImageHeight - 1, m_hvImageWidth - 1, &hvRowEnd2, &hvColEnd2, NULL);
	hvRowBegin2 = min(max(hvRowBegin2, 0), m_hvImageHeight - 1);
	hvColBegin2 = min(max(hvColBegin2, 0), m_hvImageWidth - 1);
	hvRowEnd2 = min(max(hvRowEnd2, 0), m_hvImageHeight - 1);
	hvColEnd2 = min(max(hvColEnd2, 0), m_hvImageWidth - 1);
	GenRegionLine(&hoLineTmp, hvRowBegin2, hvColBegin2, hvRowEnd2, hvColEnd2);
	ConcatObj(hoLine, hoLineTmp, &hoLine);

	GenRegionLine(&hoLineTmp, min(hvRowBegin1, hvRowEnd1), hvRowBegin1 <= hvRowEnd1 ? hvColBegin1 : hvColEnd1, min(hvRowBegin2, hvRowEnd2), hvRowBegin2 <= hvRowEnd2 ? hvColBegin2 : hvColEnd2);
	ConcatObj(hoLine, hoLineTmp, &hoLine);

	GenRegionLine(&hoLineTmp, max(hvRowBegin1, hvRowEnd1), hvRowBegin1 > hvRowEnd1 ? hvColBegin1 : hvColEnd1, max(hvRowBegin2, hvRowEnd2), hvRowBegin2 > hvRowEnd2 ? hvColBegin2 : hvColEnd2);
	ConcatObj(hoLine, hoLineTmp, &hoLine);

	Union1(hoLine, &hoLine);
	ClosingRectangle1(hoLine, &hoLine, 100, 100);
	FillUp(hoLine, &hoLine);

	return hoLine;
}

int CMeasure::SetParam(const CString& strParamName, void* vParam)
{

	return TRUE;
}

BOOL CMeasure::Measure()
{
	if (m_tInit.nSN == 1 && m_tInit.tProductInfo.nCoatingType == OneSide) return TRUE;
	//判断是否有间歇区域
	JudgeExistIntermittent();

	CLineMeasure::LineMeasureInput tIn;
	CLineMeasure::LineMeasureOutput tOut;
	tIn.hoImage = m_ptIn->hoGrabImage;
	tIn.nMode = m_ptIn->nMode;
	m_LineMeasure.Action(tIn, tOut);

	m_ptOut->hv_LineNum = tOut.hv_LineNum;
	m_ptOut->tLine = tOut.tLine;
	m_hvErrorLineIndex = tOut.hvErrorLineSN;

	//判断是否有涂布,单面的时候前相机即下相机不输出测量值
	if (!JudgeExistCoat()) return TRUE;

	if (tIn.nMode != GRRMode)
		ConcatObj(m_ptOut->ho_Disp, tOut.hoDisp, &m_ptOut->ho_Disp);

	if (m_tInit.hvCamSNList.Length() == 2 && m_Calibration.GetStripeParam().hvColumnOver.Length() == 2)
	{
		HObject hoOverRegion;
		GenRectangle1(&hoOverRegion, 0, m_Calibration.GetStripeParam().hvColumnOver.TupleMin(), m_hvImageHeight - 1, m_Calibration.GetStripeParam().hvColumnOver.TupleMax());
		ConcatObj(m_ptOut->ho_Disp, hoOverRegion, &m_ptOut->ho_Disp);
	}
	/*if (m_tInit.hvCamSNList.Length() == 2 && m_Calibration.GetStripeParam().hvLeftRightColumnOver.Length() == 2)
	{
		HObject hoOverRegion;
		GenRectangle1(&hoOverRegion, 0, m_Calibration.GetStripeParam().hvLeftRightColumnOver.TupleMin(), m_hvImageHeight - 1, m_Calibration.GetStripeParam().hvLeftRightColumnOver.TupleMax());
		ConcatObj(m_ptOut->ho_Disp, hoOverRegion, &m_ptOut->ho_Disp);
	}*/

	//修正测量边
	CorrectMeasure();
	//合并和计算测量结果
	CombinMeasure();
	return TRUE;
}

BOOL CMeasure::CorrectMeasure()
{
	//判断是否有涂膜覆盖凹版的情况
	//if (m_tInit.tProductInfo.bExistBaseCoat)
	//{
	//	for (int i = 0; i < m_ptOut->hv_LineNum.I()-1; i++)
	//	{
	//		if (CLineMeasure::LT_FoilBaseCoat == m_LineMeasure.m_tMeasureParam.tLineProp.vecLineTypeList[i] &&
	//			CLineMeasure::LT_BaseCoatCoat == m_LineMeasure.m_tMeasureParam.tLineProp.vecLineTypeList[i+1])
	//		{
	//			if ((m_hvErrorLineIndex.TupleFind(i) ==-1 || m_hvErrorLineIndex.TupleFind(i).Length() == 0) &&
	//				(m_hvErrorLineIndex.TupleFind(i+1) != -1 || m_hvErrorLineIndex.TupleFind(i).Length() > 0))
	//			{
	//				//发现极耳凹版边找到了，凹版涂布边没找到，先判断涂膜覆盖凹版的情况

	//			}
	//		}
	//	}
	//}

	if (m_ptIn->nMode == DebugMode) return TRUE;
	//判断是否有个别边没找到，如果有，就用之前找到的边
	try
	{
		//找边报警
		if (m_hvErrorLineIndex.Length() > 0 && m_hvErrorLineIndex != -1 && m_ptOut->hv_HasCoat.I() && m_ptIn->nMode == RunMode)
		{
			if (m_hvNoLineFindNGCount.Length() < m_ptOut->tLine.hv_StartColumn.Length())
				TupleGenConst(m_ptOut->tLine.hv_StartColumn.Length(), 0, &m_hvNoLineFindNGCount);
			for (int i = 0; i < m_ptOut->tLine.hv_StartColumn.Length(); i++)
			{
				HTuple hvInc;
				TupleFind(m_hvErrorLineIndex, i, &hvInc);
				
				if (hvInc.Length() > 0 && hvInc != -1)
					m_hvNoLineFindNGCount[i] += 1;
				if (m_hvNoLineFindNGCount[i] > 15)
				{
					//连续10条边没找到，报警
					CString strLog;
					strLog.Format("算法%d: 连续15张图像没找到边,请检查", m_tInit.nSN);
					m_tInit.pLog(strLog, AlarmLog, NoExcp);
					TupleGenConst(m_ptOut->tLine.hv_StartColumn.Length(), 0, &m_hvNoLineFindNGCount);
				}
			}
		}

		//if (m_hvErrorLineIndex.Length() > 0 && m_hvErrorLineIndex != -1)
		//{
		//	//三个以内的边没找到就认为是测量没找到边，使用之前的边
		//	if (m_vecLastLine.size() != 0
		//		/*&& m_tLastLine.hv_EndColumn.Length() != 0*/
		//		&& m_hvErrorLineIndex.Length() < 4
		//		&& m_nNoLineCount < 3)
		//	{
		//		m_nNoLineCount++;
		//		Line tLine = m_ptOut->tLine;
		//		for (int i = 0; i < m_hvErrorLineIndex.Length(); i++)
		//		{
		//			//求中位数，找到对应位置
		//			HTuple hvQueue;
		//			for (int j = 0; j < m_vecLastLine.size(); j++)
		//			{
		//				hvQueue.Append(m_vecLastLine[0].hv_StartColumn[m_hvErrorLineIndex[i].I()]);
		//			}
		//			if (hvQueue.Length() == 0) break;

		//			HTuple hvInc, hvIndex;
		//			TupleSortIndex(hvQueue, &hvInc);
		//			TupleSelect(hvInc, hvInc.Length() / 2, &hvIndex);

		//			//取中位数的边
		//			tLine.hv_StartColumn[m_hvErrorLineIndex[i].I()] = m_vecLastLine[hvIndex.I()].hv_StartColumn[m_hvErrorLineIndex[i].I()];
		//			tLine.hv_StartRow[m_hvErrorLineIndex[i].I()] = m_vecLastLine[hvIndex.I()].hv_StartRow[m_hvErrorLineIndex[i].I()];
		//			tLine.hv_EndColumn[m_hvErrorLineIndex[i].I()] = m_vecLastLine[hvIndex.I()].hv_EndColumn[m_hvErrorLineIndex[i].I()];
		//			tLine.hv_EndRow[m_hvErrorLineIndex[i].I()] = m_vecLastLine[hvIndex.I()].hv_EndRow[m_hvErrorLineIndex[i].I()];
		//		}
		//		m_ptOut->tLine = tLine;
		//		m_hvErrorLineIndex.Clear();
		//	}
		//}
		//else
		//{
		//	//全都找到边时，进行赋值
		//	m_nNoLineCount = 0;
		//	m_vecLastLine.push_back(m_ptOut->tLine);
		//	if (m_vecLastLine.size() > 5)
		//		m_vecLastLine.erase(m_vecLastLine.begin());
		//}

		if (m_vecLastLine.size() > 0 && m_hvErrorLineIndex.Length() < 4 && m_nNoLineCount < 3)
		{
			//全都找到边时,先判断有没有测量到的边和之前相比位置差别很大，如果有，就沿用之前找到的边

			//先对之前的边进行滤波
			HTuple hvLastColumnMean;
			for (int j = 0; j < m_vecLastLine[0].hv_StartColumn.Length(); j++)
			{
				HTuple hvTmpCol;
				for (int i = 0; i < m_vecLastLine.size(); i++)
				{
					if (m_vecLastLine[i].hv_StartColumn[j] != NO_DATA && m_vecLastLine[i].hv_EndColumn[j] != NO_DATA)
						hvTmpCol.Append((m_vecLastLine[i].hv_StartColumn[j] + m_vecLastLine[i].hv_EndColumn[j]) / 2);
				}

				if (hvTmpCol.Length() > 0)
					hvLastColumnMean.Append(hvTmpCol.TupleMedian().D());
				else
					hvLastColumnMean.Append(NO_DATA);
			}
			int nCorrectCount = 0;
			//修正测量边
			Line tLine = m_ptOut->tLine;
			for (int i = 0; i < tLine.hv_StartColumn.Length(); i++)
			{
				if (hvLastColumnMean.Length() < i + 1)continue;

				if (hvLastColumnMean[i] != NO_DATA)
				{
					//和之前的偏差大于5个像素，就认为是找边有问题，使用之前找边的位置
					if (abs((tLine.hv_StartColumn[i].D() + tLine.hv_EndColumn[i].D()) / 2 - hvLastColumnMean[i].D()) > 5)
					{
						tLine.hv_StartColumn[i] = hvLastColumnMean[i];
						tLine.hv_EndColumn[i] = hvLastColumnMean[i];

						nCorrectCount++;

						if ((tLine.hv_StartColumn[i] == NO_DATA || tLine.hv_EndColumn[i] == NO_DATA) 
							&& m_hvErrorLineIndex != -1)
						{
							//去除没找到边的序号
							HTuple hvInc;
							TupleFind(m_hvErrorLineIndex, i, &hvInc);
							if (hvInc.Length() > 0 && hvInc != -1)
							{
								TupleRemove(m_hvErrorLineIndex, hvInc, &m_hvErrorLineIndex);
							}
						}
					}
				}
			}

			if (nCorrectCount > 0)m_nNoLineCount++;
		}
		else
		{
			//全都找到边时，进行赋值
			m_nNoLineCount = 0;
		}

		m_vecLastLine.push_back(m_ptOut->tLine);
		if (m_vecLastLine.size() > 5)
			m_vecLastLine.erase(m_vecLastLine.begin());
	}
	catch (HException&e)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMeasure::CombinMeasure()
{
	m_ptOut->tFoil.hv_StripeSN.Clear();
	m_ptOut->tFoil.hv_Width.Clear();
	m_ptOut->tCoat.hv_StripeSN.Clear();
	m_ptOut->tCoat.hv_Width.Clear();
	m_ptOut->tAT11.hv_StripeSN.Clear();
	m_ptOut->tAT11.hv_Width.Clear();
	m_ptOut->tBaseCoat.hv_StripeSN.Clear();
	m_ptOut->tBaseCoat.hv_Width.Clear();
	m_ptOut->tSize.hv_Width.Clear();
	m_ptOut->tSize.hv_WidthOrigin.Clear();
	m_ptOut->tSize.hv_StripeSN.Clear();
	m_ptOut->tSize.hv_WholeWidth.Clear();
	m_ptOut->tSize.hv_WholeBaseCoat.Clear();
	m_ptOut->tIntermittent.hv_Width.Clear();
	m_ptOut->tIntermittent.hv_StripeSN.Clear();
	if (m_tInit.tProductInfo.nCoatingType == TwoSide)
		m_ptOut->tSize.hv_MisalignedDist.Clear();

	BOOL bUseAutoCompensation = FALSE;//干湿膜自动补偿
	if (m_tInit.nSN == 0)
		m_tInit.pMain(MT_Measure, "GetAutoCompensationEnable", "", NULL, &bUseAutoCompensation);

	CCalibration::CalDisLLInput tIn;
	CCalibration::CalDisLLOutput tOut;
	int nMisalignmentSN = 0;
	HTuple hvWidthOffline;
	if (m_tInit.tProductInfo.nPDType != ThreeCavity)
	{
		for (int i = 0; i < m_ptOut->hv_LineNum.I() - 1; i++)
		{
			//测量出的区域
			HObject hoTmp = _GenSingleRegion(m_ptOut->tLine.hv_StartRow[i], m_ptOut->tLine.hv_StartColumn[i], m_ptOut->tLine.hv_EndRow[i], m_ptOut->tLine.hv_EndColumn[i],
				m_ptOut->tLine.hv_StartRow[i + 1], m_ptOut->tLine.hv_StartColumn[i + 1], m_ptOut->tLine.hv_EndRow[i + 1], m_ptOut->tLine.hv_EndColumn[i + 1]);

			int nLeftSN = i, nRightSN = i + 1;
			int nType = m_LineMeasure.GetMeasureParam().tLineProp.vecLineTypeList[nLeftSN];
			tIn.tLine1.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nLeftSN];
			tIn.tLine1.hv_StartRow = m_ptOut->tLine.hv_StartRow[nLeftSN];
			tIn.tLine1.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nLeftSN];
			tIn.tLine1.hv_EndRow = m_ptOut->tLine.hv_EndRow[nLeftSN];
			tIn.tLine2.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nRightSN];
			tIn.tLine2.hv_StartRow = m_ptOut->tLine.hv_StartRow[nRightSN];
			tIn.tLine2.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nRightSN];
			tIn.tLine2.hv_EndRow = m_ptOut->tLine.hv_EndRow[nRightSN];

			//判断测量边是否有效
			HTuple hvWidth(0);
			if (IsLineFind(nLeftSN) && IsLineFind(nRightSN))
			{
				m_Calibration.CalDistanceLL(tIn, tOut);
				hvWidth = tOut.Dist;
			}
			else
			{
				hvWidth = 0;
			}
			if (nType == CLineMeasure::LT_BgFoil || nType == CLineMeasure::LT_AT11Foil || nType == CLineMeasure::LT_CoatFoil || nType == CLineMeasure::LT_BaseCoatFoil)
			{
				//极耳计算 2021.3.5修改为判断计算方式
				if (m_tMeasureAdvanceParam.eFoilSelect == FS_FoilBaseCoat && (m_tInit.tProductInfo.bExistBaseCoat || m_tInit.tProductInfo.bExistAT11))
				{
					//计算极耳宽包含凹版
					if (i == 0)
					{
						//左侧极耳包含一侧凹版
						nLeftSN = i;
						nRightSN = i + 2;
					}
					else if (i == m_ptOut->hv_LineNum.I() - 2)
					{
						//右侧极耳包含一侧凹版
						nLeftSN = i - 1;
						nRightSN = i + 1;
					}
					else
					{
						//中间极耳包含两侧凹版
						nLeftSN = i - 1;
						nRightSN = i + 2;
					}

					//判断测量边是否有效
					if (IsLineFind(nLeftSN) && IsLineFind(nRightSN))
					{
						tIn.tLine1.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nLeftSN];
						tIn.tLine1.hv_StartRow = m_ptOut->tLine.hv_StartRow[nLeftSN];
						tIn.tLine1.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nLeftSN];
						tIn.tLine1.hv_EndRow = m_ptOut->tLine.hv_EndRow[nLeftSN];
						tIn.tLine2.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nRightSN];
						tIn.tLine2.hv_StartRow = m_ptOut->tLine.hv_StartRow[nRightSN];
						tIn.tLine2.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nRightSN];
						tIn.tLine2.hv_EndRow = m_ptOut->tLine.hv_EndRow[nRightSN];
						m_Calibration.CalDistanceLL(tIn, tOut);
						hvWidth = tOut.Dist;
					}
					else
					{
						hvWidth = 0;
					}
				}

				if (hvWidth != NO_DATA && hvWidth != 0)
				{
					hvWidth += m_tInit.tProductInfo.tParam.vecFoilWidth[m_ptOut->tFoil.hv_Width.Length()].OffsetList[m_tInit.nSN];
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
					if (m_tInit.nSN == 0 && bUseAutoCompensation)//自动干湿膜补偿
						hvWidth += m_tInit.tProductInfo.tParam.vecFoilWidth[m_ptOut->tFoil.hv_Width.Length()].dOffsetOL;
				}
				else
				{
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
				}
				m_ptOut->tFoil.hv_StripeSN.Append(i);
				m_ptOut->tFoil.hv_Width.Append(hvWidth);
				ConcatObj(m_ptOut->tFoil.ho_Region, hoTmp, &m_ptOut->tFoil.ho_Region);
			}
			else if (nType == CLineMeasure::LT_FoilAT11 || nType == CLineMeasure::LT_CoatAT11)
			{
				//AT11
				if (hvWidth != NO_DATA && hvWidth != 0)
				{
					hvWidth += m_tInit.tProductInfo.tParam.vecAT11Width[m_ptOut->tAT11.hv_Width.Length()].OffsetList[m_tInit.nSN];
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
					if (m_tInit.nSN == 0 && bUseAutoCompensation)//自动干湿膜补偿
						hvWidth += m_tInit.tProductInfo.tParam.vecAT11Width[m_ptOut->tAT11.hv_Width.Length()].dOffsetOL;
				}
				else
				{
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
				}
				m_ptOut->tAT11.hv_StripeSN.Append(i);
				m_ptOut->tAT11.hv_Width.Append(hvWidth);
				ConcatObj(m_ptOut->tAT11.ho_Region, hoTmp, &m_ptOut->tAT11.ho_Region);
			}
			else if (nType == CLineMeasure::LT_FoilBaseCoat || nType == CLineMeasure::LT_CoatBaseCoat)
			{
				//底涂
				if (hvWidth != NO_DATA && hvWidth != 0)
				{
					hvWidth += m_tInit.tProductInfo.tParam.vecBaseCoatWidth[m_ptOut->tBaseCoat.hv_Width.Length()].OffsetList[m_tInit.nSN];
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
					if (m_tInit.nSN == 0 && bUseAutoCompensation)//自动干湿膜补偿
						hvWidth += m_tInit.tProductInfo.tParam.vecBaseCoatWidth[m_ptOut->tBaseCoat.hv_Width.Length()].dOffsetOL;
				}
				else
				{
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
				}
				m_ptOut->tBaseCoat.hv_StripeSN.Append(i);
				m_ptOut->tBaseCoat.hv_Width.Append(hvWidth);
				ConcatObj(m_ptOut->tBaseCoat.ho_Region, hoTmp, &m_ptOut->tBaseCoat.ho_Region);
			}
			else
			{
				//涂布
				if (m_tMeasureAdvanceParam.eCoatSelect == CS_FullCoat)
				{
					//凹版被遮挡也计算涂布尺寸
					int nLeftSN(i), nRightSN(i + 1);
					if (!IsLineFind(i) && IsLineFind(i - 1))
					{
						//涂布左边没找到，但是凹版和极耳边找到了，就用左侧凹版极耳边
						nLeftSN = i - 1;
					}
					if (!IsLineFind(i + 1) && IsLineFind(i + 2))
					{
						//涂布右边没找到，右侧凹版和极耳边找到了，就用右侧凹版极耳边
						nRightSN = i + 2;
					}
					/*tIn.tLine1.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nLeftSN];
					tIn.tLine1.hv_StartRow = m_ptOut->tLine.hv_StartRow[nLeftSN];
					tIn.tLine1.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nLeftSNi];
					tIn.tLine1.hv_EndRow = m_ptOut->tLine.hv_EndRow[nLeftSN];
					tIn.tLine2.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nRightSN];
					tIn.tLine2.hv_StartRow = m_ptOut->tLine.hv_StartRow[nRightSN];
					tIn.tLine2.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nRightSN];
					tIn.tLine2.hv_EndRow = m_ptOut->tLine.hv_EndRow[nRightSN];
					m_Calibration.CalDistanceLL(tIn, tOut);
					hvWidth = tOut.Dist;*/
				}
				
				if (hvWidth != NO_DATA && hvWidth != 0)
				{
					hvWidth += m_tInit.tProductInfo.tParam.vecCoatWidth[m_ptOut->tCoat.hv_Width.Length()].OffsetList[m_tInit.nSN];
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
					if (m_tInit.nSN == 0 && bUseAutoCompensation)//自动干湿膜补偿
						hvWidth += m_tInit.tProductInfo.tParam.vecCoatWidth[m_ptOut->tCoat.hv_Width.Length()].dOffsetOL;
				}
				else
				{
					m_ptOut->tSize.hv_WidthOrigin.Append(hvWidth);
				}
				m_ptOut->tCoat.hv_StripeSN.Append(i);
				m_ptOut->tCoat.hv_Width.Append(hvWidth);
				ConcatObj(m_ptOut->tCoat.ho_Region, hoTmp, &m_ptOut->tCoat.ho_Region);
			}

			m_ptOut->tSize.hv_StripeSN.Append(i);
			m_ptOut->tSize.hv_Width.Append(hvWidth);

			//计算错位距离,2021.2.22改成只输出涂膜两边的错位距离
			int nType2 = m_LineMeasure.GetMeasureParam().tLineProp.vecLineTypeList[i + 1];
			if (m_tInit.tProductInfo.nCoatingType == TwoSide && (
				nType2 == CLineMeasure::LT_CoatFoil || nType2 == CLineMeasure::LT_FoilCoat ||
				nType2 == CLineMeasure::LT_CoatBaseCoat || nType2 == CLineMeasure::LT_BaseCoatCoat ||
				nType2 == CLineMeasure::LT_CoatAT11 || nType2 == CLineMeasure::LT_AT11Coat))
			{
				HTuple hvDist;
				if (m_tMeasureAdvanceParam.eMisalignmentSelect == MS_LineDist)
				{
					//2021.3.5修改计算错位距离方式 错位距离用线与线之间的距离计算
					if (i + 1 < m_ptOut->hv_LineNum.I() / 2)
					{
						nLeftSN = 0;
						nRightSN = i + 1;
					}
					else
					{
						nLeftSN = i + 1;
						nRightSN = m_ptOut->hv_LineNum.I() - 1;
					}

					if (IsLineFind(nLeftSN) && IsLineFind(nRightSN))
					{
						tIn.tLine1.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nLeftSN];
						tIn.tLine1.hv_StartRow = m_ptOut->tLine.hv_StartRow[nLeftSN];
						tIn.tLine1.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nLeftSN];
						tIn.tLine1.hv_EndRow = m_ptOut->tLine.hv_EndRow[nLeftSN];
						tIn.tLine2.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nRightSN];
						tIn.tLine2.hv_StartRow = m_ptOut->tLine.hv_StartRow[nRightSN];
						tIn.tLine2.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nRightSN];
						tIn.tLine2.hv_EndRow = m_ptOut->tLine.hv_EndRow[nRightSN];
						m_Calibration.CalDistanceLL(tIn, tOut);
						hvDist = tOut.Dist;
					}
					else
					{
						hvDist = 0;
					}
					if (hvDist != NO_DATA && hvDist != 0) hvDist += m_tInit.tProductInfo.tParam.vecMisalignment[nMisalignmentSN++].OffsetList[m_tInit.nSN];
					m_ptOut->tSize.hv_MisalignedDist.Append(hvDist);
				}
				//else
				//{
				//	//错位距离采用宽度累加的方式
				//	hvDist = m_ptOut->tSize.hv_Width.TupleSum();
				//}
			}

			//调试模式输出整个凹版的宽度
			//if (/*m_ptIn->nMode == DebugMode && */m_tInit.tProductInfo.bExistBaseCoat)
			{
				if (nType == CLineMeasure::LT_FoilBaseCoat)
				{
					//底涂
					nLeftSN = i;
					nRightSN = i + 3;
					HTuple hvDist;
					if (IsLineFind(nLeftSN) && IsLineFind(nRightSN))
					{
						tIn.tLine1.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nLeftSN];
						tIn.tLine1.hv_StartRow = m_ptOut->tLine.hv_StartRow[nLeftSN];
						tIn.tLine1.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nLeftSN];
						tIn.tLine1.hv_EndRow = m_ptOut->tLine.hv_EndRow[nLeftSN];
						tIn.tLine2.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nRightSN];
						tIn.tLine2.hv_StartRow = m_ptOut->tLine.hv_StartRow[nRightSN];
						tIn.tLine2.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nRightSN];
						tIn.tLine2.hv_EndRow = m_ptOut->tLine.hv_EndRow[nRightSN];
						m_Calibration.CalDistanceLL(tIn, tOut);
						hvDist = tOut.Dist;
					}
					else
					{
						hvDist = 0;
					}
					//if (hvWidth != NO_DATA) hvWidth += m_tInit.tProductInfo.tParam.vecBaseCoatWidth[m_ptOut->tBaseCoat.hv_Width.Length()].OffsetList[m_tInit.nSN];
					m_ptOut->tSize.hv_WholeBaseCoat.Append(tOut.Dist);
				}

				int nType1 = m_LineMeasure.GetMeasureParam().tLineProp.vecLineTypeList[i + 1];
				if (nType1 == CLineMeasure::LT_FoilBg)
				{
					//输出基材宽
					nLeftSN = 0;
					nRightSN = i + 1;
					HTuple hvDist;
					if (IsLineFind(nLeftSN) && IsLineFind(nRightSN))
					{
						tIn.tLine1.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nLeftSN];
						tIn.tLine1.hv_StartRow = m_ptOut->tLine.hv_StartRow[nLeftSN];
						tIn.tLine1.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nLeftSN];
						tIn.tLine1.hv_EndRow = m_ptOut->tLine.hv_EndRow[nLeftSN];
						tIn.tLine2.hv_StartColumn = m_ptOut->tLine.hv_StartColumn[nRightSN];
						tIn.tLine2.hv_StartRow = m_ptOut->tLine.hv_StartRow[nRightSN];
						tIn.tLine2.hv_EndColumn = m_ptOut->tLine.hv_EndColumn[nRightSN];
						tIn.tLine2.hv_EndRow = m_ptOut->tLine.hv_EndRow[nRightSN];
						m_Calibration.CalDistanceLL(tIn, tOut);
						hvDist = tOut.Dist;
					}
					else
					{
						hvDist = 0;
					}
					//if (hvWidth != NO_DATA) hvWidth += m_tInit.tProductInfo.tParam.vecBaseCoatWidth[m_ptOut->tBaseCoat.hv_Width.Length()].OffsetList[m_tInit.nSN];
					m_ptOut->tSize.hv_WholeWidth.Append(hvDist);
				}
			}
		}

		if (m_tMeasureAdvanceParam.eMisalignmentSelect == MS_WidthSum)
		{
			//使用宽度累加，计算错位
			m_ptOut->tSize.hv_MisalignedDist.Clear();
			//左侧
			for (int i = 0; i < (m_ptOut->hv_LineNum.I() / 2 - 1); i++)
			{
				int nType2 = m_LineMeasure.GetMeasureParam().tLineProp.vecLineTypeList[i + 1];
				if (m_tInit.tProductInfo.nCoatingType == TwoSide && (
					nType2 == CLineMeasure::LT_CoatFoil || nType2 == CLineMeasure::LT_FoilCoat ||
					nType2 == CLineMeasure::LT_CoatBaseCoat || nType2 == CLineMeasure::LT_BaseCoatCoat ||
					nType2 == CLineMeasure::LT_CoatAT11 || nType2 == CLineMeasure::LT_AT11Coat))
				{
					HTuple hvDist;
					TupleSelectRange(m_ptOut->tSize.hv_Width, 0, i, &hvDist);
					m_ptOut->tSize.hv_MisalignedDist.Append(hvDist.TupleSum());
				}
			}
			//右侧
			for (int i = m_ptOut->hv_LineNum.I() / 2; i < m_ptOut->hv_LineNum.I()-1; i++)
			{
				int nType2 = m_LineMeasure.GetMeasureParam().tLineProp.vecLineTypeList[i];
				if (m_tInit.tProductInfo.nCoatingType == TwoSide && (
					nType2 == CLineMeasure::LT_CoatFoil || nType2 == CLineMeasure::LT_FoilCoat ||
					nType2 == CLineMeasure::LT_CoatBaseCoat || nType2 == CLineMeasure::LT_BaseCoatCoat ||
					nType2 == CLineMeasure::LT_CoatAT11 || nType2 == CLineMeasure::LT_AT11Coat))
				{
					HTuple hvDist;
					TupleSelectRange(m_ptOut->tSize.hv_Width, i, m_ptOut->tSize.hv_Width.Length() - 1, &hvDist);
					m_ptOut->tSize.hv_MisalignedDist.Append(hvDist.TupleSum());
				}
			}
		}
	}

	if (m_tMeasureAdvanceParam.eFoilSelect == FS_FoilBaseCoat && m_tInit.tProductInfo.bExistBaseCoat)
	{
		m_ptOut->tSize.hv_WholeWidth.Append(m_ptOut->tSize.hv_Width.TupleSum().D() - m_ptOut->tBaseCoat.hv_Width.TupleSum().D());
	}
	else
	{
		m_ptOut->tSize.hv_WholeWidth.Append(m_ptOut->tSize.hv_Width.TupleSum().D());
	}
	if (bUseAutoCompensation)
	{
		//记录在线补偿日志
		CString strLog, strTmp;
		strLog.Format("%d面原始测量值:,", m_tInit.nSN);
		for (int i = 0; i < m_ptOut->tSize.hv_WidthOrigin.Length(); i++)
		{
			strTmp.Format("%0.4f,", m_ptOut->tSize.hv_WidthOrigin[i].D());
			strLog += strTmp;
		}
		strTmp.Format("补偿后测量值:,");
		strLog += strTmp;
		for (int i = 0; i < m_ptOut->tSize.hv_Width.Length(); i++)
		{
			strTmp.Format("%0.4f,", m_ptOut->tSize.hv_Width[i].D());
			strLog += strTmp;
		}
		m_tInit.pLog(strLog, StdLog, NoExcp);
	}
	

	return TRUE;
}

Line CMeasure::GetWorldCoordsFromFitLine()
{
	double dResY = m_Calibration.GetResolution()[0].dRowRes;
	Line tLine = m_ptOut->tLine;

	for (int i = 0; i < m_ptOut->tLine.hv_StartRow.Length(); i++)
	{
		BOOL bRet = TRUE;
		HTuple hvVal;
		m_Calibration.CalPoint((HTuple)m_ptOut->tLine.hv_StartColumn[i], hvVal);
		tLine.hv_StartColumn[i] = hvVal;
		m_Calibration.CalPoint((HTuple)m_ptOut->tLine.hv_EndColumn[i], hvVal);
		tLine.hv_EndColumn[i] = hvVal;

		tLine.hv_StartRow[i] = 0;
		tLine.hv_EndRow[i] = abs(m_ptOut->tLine.hv_StartRow[i].D() - m_ptOut->tLine.hv_EndRow[i].D()) * dResY;
	}

	return tLine;
}

void CMeasure::_DistanceLL(HTuple hvStartX1, HTuple hvStartY1, HTuple hvEndX1, HTuple hvEndY1,
	HTuple hvStartX2, HTuple hvStartY2, HTuple hvEndX2, HTuple hvEndY2, HTuple *hvDist)
{
	*hvDist = 0;
	if (hvStartX1 < 0 || hvEndX1 < 0 || hvStartX2 < 0 || hvEndX2 < 0) return;

	HTuple hvCenterX, hvCenterY;
	hvCenterY = (hvStartY1.D() + hvEndY1.D()) / 2.0;
	hvCenterX = (hvStartX1.D() + hvEndX1.D()) / 2.0;

	DistancePl(hvCenterX, hvCenterY, hvStartY2, hvStartX2, hvEndX2, hvEndY2, &(*hvDist));
}

BOOL CMeasure::GRRTest(MeasureInput &tIn)
{
	if (tIn.nMode == GRRMode)
	{
		HObject hoSelectRegions;
		HObject hoROI;
		if (m_ptOut->tFoil.ho_Region.CountObj() > 0)
			hoROI = m_ptOut->tFoil.ho_Region.SelectObj(1);
		else
			hoROI = m_ptIn->hoGrabImage;

		_GetBlackMark(m_ptIn->hoGrabImage, hoROI, &hoSelectRegions);
		ConcatObj(m_ptOut->ho_Disp, hoSelectRegions, &m_ptOut->ho_Disp);
	}
	return TRUE;
}


void CMeasure::Detect()
{
	if (m_tInit.nSN == 1 && m_tInit.tProductInfo.nCoatingType == OneSide) return;
	if (m_hvErrorLineIndex.Length() > 0 && m_hvErrorLineIndex != -1)return;
	CDefect::DefectInput tIn;
	CDefect::DefectOutput tOut;
	GenEmptyObj(&tOut.hoDisp);

	tIn.hoImage = m_ptIn->hoGrabImage;
	tIn.nMode = m_ptIn->nMode;
	tIn.tFoil = m_ptOut->tFoil;
	tIn.tAT11 = m_ptOut->tAT11;
	tIn.tCoat = m_ptOut->tCoat;
	tIn.tBaseCoat = m_ptOut->tBaseCoat;
	///瑕疵检测
	m_Defect.Action(tIn, tOut);
	m_ptOut->tDefect.ho_Region = tOut.hoDisp;
	m_ptOut->tDefect.hv_StripeSN = tOut.hv_StripeSN;
	m_ptOut->tDefect.hv_Type = tOut.hv_Type;
	//缺陷区域中心坐标，横向坐标转换为相对箔材边的
	HTuple hvSideRealCol, hvColReal;
	m_Calibration.CalPoint((HTuple)m_ptOut->tLine.hv_StartColumn[0], hvSideRealCol);
	for (int i = 0; i < tOut.tFeature.tCenter.hv_Column.Length(); i++)
	{
		HTuple hvCol;
		m_Calibration.CalPoint((HTuple)tOut.tFeature.tCenter.hv_Column[i], hvCol);
		tOut.tFeature.tCenter.hv_Column[i] = hvCol - hvSideRealCol.D();

		m_Calibration.CalPoint((HTuple)tOut.tFeature.tRect.tLeftTop.hv_Column[i], hvCol);
		tOut.tFeature.tRect.tLeftTop.hv_Column[i] = hvCol - hvSideRealCol.D();
		m_Calibration.CalPoint((HTuple)tOut.tFeature.tRect.tRightBottom.hv_Column[i], hvCol);
		tOut.tFeature.tRect.tRightBottom.hv_Column[i] = hvCol - hvSideRealCol.D();
	}
	m_ptOut->tDefect.tFeature = tOut.tFeature;
}

void CMeasure::SpotInspect()
{
	CSpotInspection::InspectionInput tIn;
	CSpotInspection::InspectionOutput tOut;
	tIn.hoImage = m_ptIn->hoGrabImage;
	if (!m_SpotInspection.Action(tIn, tOut)) return;

	CCalibration::CalDisLLInput tLLIn;
	CCalibration::CalDisLLOutput tLLOut;
	try
	{
		if (m_SpotInspection.GetType() == CSpotInspection::IT_Rect && tOut.ptLeftTop.hv_Row.Length() >= 1)
		{
			m_ptOut->tSpotInspection.hv_Width.Clear();
			//2021.6.9修改为通常一个检测面两个点检目标
			for (int i = 0; i < tOut.ptLeftTop.hv_Row.Length(); i++)
			{
				if (i > 1) continue;

				if (tOut.ptCenter.hv_Row[i] == NO_DATA ||
					tOut.ptCenter.hv_Column[i] == NO_DATA ||
					tOut.ptLeftTop.hv_Row[i] == NO_DATA ||
					tOut.ptLeftTop.hv_Column[i] == NO_DATA ||
					tOut.ptLeftBottom.hv_Row[i] == NO_DATA ||
					tOut.ptLeftBottom.hv_Column[i] == NO_DATA ||
					tOut.ptRightTop.hv_Row[i] == NO_DATA ||
					tOut.ptRightTop.hv_Column[i] == NO_DATA ||
					tOut.ptRightBottom.hv_Row[i] == NO_DATA ||
					tOut.ptRightBottom.hv_Column[i] == NO_DATA)
				{
					//没有找到点检的边，放入无效值
					m_ptOut->tSpotInspection.hv_Width.Append(NO_DATA);
					m_ptOut->tSpotInspection.hv_Width.Append(NO_DATA);
					continue;
				}

				DoubleList OffsetList;
				HTuple hvSizeTmp;
				//计算点检目标的宽度
				tLLIn.tLine1.hv_StartRow = tOut.ptLeftTop.hv_Row[i];
				tLLIn.tLine1.hv_StartColumn = tOut.ptLeftTop.hv_Column[i];
				tLLIn.tLine1.hv_EndRow = tOut.ptLeftBottom.hv_Row[i];
				tLLIn.tLine1.hv_EndColumn = tOut.ptLeftBottom.hv_Column[i];
				tLLIn.tLine2.hv_StartRow = tOut.ptRightTop.hv_Row[i];
				tLLIn.tLine2.hv_StartColumn = tOut.ptRightTop.hv_Column[i];
				tLLIn.tLine2.hv_EndRow = tOut.ptRightBottom.hv_Row[i];
				tLLIn.tLine2.hv_EndColumn = tOut.ptRightBottom.hv_Column[i];
				m_Calibration.CalDistanceLL(tLLIn, tLLOut);
				OffsetList = m_tInit.tProductInfo.tParam.vecChecking[i * 2].OffsetList;
				hvSizeTmp = tLLOut.Dist.D() + OffsetList[m_tInit.nSN];
				//如果需要上手段，就把_CorrectValue打开
				//_CorrectValue(hvSizeTmp, m_tInit.tProductInfo.tParam.vecChecking[0].dStandard, m_tInit.tProductInfo.tParam.vecChecking[0].dTolerance, &hvSizeTmp);
				m_ptOut->tSpotInspection.hv_Width.Append(hvSizeTmp);

				//计算点检目标的高度
				tLLIn.tLine1.hv_StartRow = tOut.ptLeftTop.hv_Row[i];
				tLLIn.tLine1.hv_StartColumn = tOut.ptLeftTop.hv_Column[i];
				tLLIn.tLine1.hv_EndRow = tOut.ptRightTop.hv_Row[i];
				tLLIn.tLine1.hv_EndColumn = tOut.ptRightTop.hv_Column[i];
				tLLIn.tLine2.hv_StartRow = tOut.ptLeftBottom.hv_Row[i];
				tLLIn.tLine2.hv_StartColumn = tOut.ptLeftBottom.hv_Column[i];
				tLLIn.tLine2.hv_EndRow = tOut.ptRightBottom.hv_Row[i];
				tLLIn.tLine2.hv_EndColumn = tOut.ptRightBottom.hv_Column[i];
				m_Calibration.CalDistanceLL(tLLIn, tLLOut);
				OffsetList = m_tInit.tProductInfo.tParam.vecChecking[i * 2].OffsetList;
				hvSizeTmp = tLLOut.Dist.D() + OffsetList[m_tInit.nSN];
				//如果需要上手段，就把_CorrectValue打开
				//_CorrectValue(hvSizeTmp, m_tInit.tProductInfo.tParam.vecChecking[1].dStandard, m_tInit.tProductInfo.tParam.vecChecking[1].dTolerance, &hvSizeTmp);
				m_ptOut->tSpotInspection.hv_Width.Append(hvSizeTmp);
			}

			//计算两个点检模块的距离
			if (tOut.ptLeftTop.hv_Row.Length() > 1)
			{
				/*tLLIn.tLine1.hv_StartRow = tOut.ptLeftTop.hv_Row[0];
				tLLIn.tLine1.hv_StartColumn = tOut.ptLeftTop.hv_Column[0];
				tLLIn.tLine1.hv_EndRow = tOut.ptLeftBottom.hv_Row[0];
				tLLIn.tLine1.hv_EndColumn = tOut.ptLeftBottom.hv_Column[0];
				tLLIn.tLine2.hv_StartRow = tOut.ptRightTop.hv_Row[1];
				tLLIn.tLine2.hv_StartColumn = tOut.ptRightTop.hv_Column[1];
				tLLIn.tLine2.hv_EndRow = tOut.ptRightBottom.hv_Row[1];
				tLLIn.tLine2.hv_EndColumn = tOut.ptRightBottom.hv_Column[1];
				m_Calibration.CalDistanceLL(tLLIn, tLLOut);*/
				//OffsetList = m_tInit.tProductInfo.tParam.vecChecking[i * 2].OffsetList;
				//HTuple hvSizeTmp = tLLOut.Dist.D()/* + OffsetList[m_tInit.nSN]*/;

				CCalibration::CalDisPPInput tPPIn;
				CCalibration::CalDisPPOutput tPPOut;
				tPPIn.hvCol1 = tOut.ptCenter.hv_Column[0];
				tPPIn.hvCol2 = tOut.ptCenter.hv_Column[1];
				m_Calibration.CalDistancePP(tPPIn, tPPOut);
				HTuple hvSizeTmp = tPPOut.Dist.D();
				//如果需要上手段，就把_CorrectValue打开
				//_CorrectValue(hvSizeTmp, m_tInit.tProductInfo.tParam.vecChecking[1].dStandard, m_tInit.tProductInfo.tParam.vecChecking[1].dTolerance, &hvSizeTmp);
				m_ptOut->tSpotInspection.hv_Width.Append(hvSizeTmp);
			}
			else
			{
				m_ptOut->tSpotInspection.hv_Width.Append(NO_DATA);
			}

			m_ptOut->tSpotInspection.ho_Region = tOut.hoDisp;
		}
		else if (m_SpotInspection.GetType() == CSpotInspection::IT_Circle && tOut.ptCenter.hv_Row.Length() >= 1)
		{
			m_ptOut->tSpotInspection.hv_Width.Clear();
			//2021.6.9修改为通常一个检测面两个点检目标
			for (int i = 0; i < tOut.ptLeftTop.hv_Row.Length(); i++)
			{
				if (i > 1) continue;

				if (tOut.ptCenter.hv_Row[i] == NO_DATA ||
					tOut.ptCenter.hv_Column[i] == NO_DATA ||
					tOut.ptLeftTop.hv_Row[i] == NO_DATA ||
					tOut.ptLeftTop.hv_Column[i] == NO_DATA ||
					tOut.ptRightBottom.hv_Row[i] == NO_DATA ||
					tOut.ptRightBottom.hv_Column[i] == NO_DATA)
				{
					//没有找到点检的边，放入无效值
					m_ptOut->tSpotInspection.hv_Width.Append(NO_DATA);
					m_ptOut->tSpotInspection.hv_Width.Append(NO_DATA);
					continue;
				}

				DoubleList OffsetList;
				HTuple hvSizeTmp;
				//计算圆形点检目标的宽度
				CCalibration::CalDisPPInput tPPIn;
				CCalibration::CalDisPPOutput tPPOut;
				tPPIn.hvCol1 = tOut.ptLeftTop.hv_Column[i];
				tPPIn.hvCol2 = tOut.ptRightBottom.hv_Column[i];
				m_Calibration.CalDistancePP(tPPIn, tPPOut);
				OffsetList = m_tInit.tProductInfo.tParam.vecChecking[i * 2].OffsetList;
				hvSizeTmp = tPPOut.Dist.D() + OffsetList[m_tInit.nSN];
				//如果需要上手段，就把_CorrectValue打开
				//_CorrectValue(hvSizeTmp, m_tInit.tProductInfo.tParam.vecChecking[0].dStandard, m_tInit.tProductInfo.tParam.vecChecking[0].dTolerance, &hvSizeTmp);
				m_ptOut->tSpotInspection.hv_Width.Append(hvSizeTmp);

				//计算圆形点检目标的高度
				OffsetList = m_tInit.tProductInfo.tParam.vecChecking[i * 2].OffsetList;
				hvSizeTmp = abs(tOut.ptLeftTop.hv_Row[i].D() - tOut.ptRightBottom.hv_Row[i].D()) * m_Calibration.GetResolution()[0].dRowRes + OffsetList[m_tInit.nSN];
				//如果需要上手段，就把_CorrectValue打开
				//_CorrectValue(hvSizeTmp, m_tInit.tProductInfo.tParam.vecChecking[1].dStandard, m_tInit.tProductInfo.tParam.vecChecking[1].dTolerance, &hvSizeTmp);
				m_ptOut->tSpotInspection.hv_Width.Append(hvSizeTmp);
			}

			//计算两个点检圆心的距离
			if (tOut.ptLeftTop.hv_Row.Length() > 1)
			{
				CCalibration::CalDisPPInput tPPIn;
				CCalibration::CalDisPPOutput tPPOut;
				tPPIn.hvCol1 = tOut.ptCenter.hv_Column[0];
				tPPIn.hvCol2 = tOut.ptCenter.hv_Column[1];
				m_Calibration.CalDistancePP(tPPIn, tPPOut);
				HTuple hvSizeTmp = tPPOut.Dist.D();
				//如果需要上手段，就把_CorrectValue打开
				//_CorrectValue(hvSizeTmp, m_tInit.tProductInfo.tParam.vecChecking[1].dStandard, m_tInit.tProductInfo.tParam.vecChecking[1].dTolerance, &hvSizeTmp);
				m_ptOut->tSpotInspection.hv_Width.Append(hvSizeTmp);
			}
			else
			{
				m_ptOut->tSpotInspection.hv_Width.Append(NO_DATA);
			}

			m_ptOut->tSpotInspection.ho_Region = tOut.hoDisp;
		}
	}
	catch (HException&e)
	{
		TupleGenConst(m_tInit.hvCamSNList.Length() * 2, NO_DATA, &m_ptOut->tSpotInspection.hv_Width);
		m_ptOut->tSpotInspection.hv_StripeSN.Clear();
		GenEmptyObj(&m_ptOut->tSpotInspection.ho_Region);
	}

}

BOOL CMeasure::JudgeExistCoat()
{
	m_ptOut->hv_HasCoat = TRUE;
	if (!m_tPreprocParam.bUseJudgeUncoat) return TRUE;
	//写判断是否有涂层的逻辑
	//取图像中间一部分区域，阈值分割出空箔材和空凹版，计算占整个图像的占比
	int REGION_HEIGHT = 20;
	HObject hoRegion, hoImagePart, hoUncoatRegionLight, hoUncoatRegionDark;
	try
	{
		GenRectangle1(&hoRegion, m_hvImageHeight / 2 - REGION_HEIGHT / 2, (m_ptOut->tLine.hv_StartColumn[0] + m_ptOut->tLine.hv_EndColumn[0]) / 2,
			m_hvImageHeight / 2 + REGION_HEIGHT / 2, (m_ptOut->tLine.hv_StartColumn[m_ptOut->tLine.hv_StartColumn.Length() - 1] 
				+ m_ptOut->tLine.hv_EndColumn[m_ptOut->tLine.hv_EndColumn.Length() - 1]) / 2);

		ReduceDomain(m_ptIn->hoGrabImage, hoRegion, &hoImagePart);
		//亮阈值，主要分割空箔材
		FastThreshold(hoImagePart, &hoUncoatRegionLight, MIN(m_tPreprocParam.nUncoatThresLight, 255), 255, REGION_HEIGHT);
		//暗阈值，主要分割空凹版
		FastThreshold(hoImagePart, &hoUncoatRegionDark, 0, MIN(m_tPreprocParam.nUncoatThresDark, 255), REGION_HEIGHT);
		HTuple hvAreaLight, hvAreaDark;
		AreaCenter(hoUncoatRegionLight, &hvAreaLight, &HTuple(), &HTuple());
		AreaCenter(hoUncoatRegionDark, &hvAreaDark, &HTuple(), &HTuple());

		double dAreaTotal = ((m_ptOut->tLine.hv_StartColumn[m_ptOut->tLine.hv_StartColumn.Length() - 1]
			+ m_ptOut->tLine.hv_EndColumn[m_ptOut->tLine.hv_EndColumn.Length() - 1]) / 2 
			- (m_ptOut->tLine.hv_StartColumn[0] + m_ptOut->tLine.hv_EndColumn[0]) / 2) * REGION_HEIGHT;

		if (hvAreaLight.Length() == 0 && hvAreaDark.Length() == 0) return TRUE;

		if (hvAreaLight.Length() > 0)
		{
			double dAreaRate = hvAreaLight.D() / dAreaTotal;
			if (dAreaRate > m_tPreprocParam.dUncoatAreaRate)
			{
				m_ptOut->hv_HasCoat = FALSE;
				return FALSE;
			}
		}
		if (hvAreaDark.Length() > 0)
		{
			double dAreaRate = hvAreaDark.D() / dAreaTotal;
			if (dAreaRate > m_tPreprocParam.dUncoatAreaRate)
			{
				m_ptOut->hv_HasCoat = FALSE;
				return FALSE;
			}
		}
	}
	catch (HException&e)
	{
		CString strMessageBox;
		strMessageBox.Format("CMeasure_JudgeExistCoat: 算法%d出现异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strMessageBox, ExcpLog, SlightExcp);
	}
	return TRUE;
}

void CMeasure::JudgeExistIntermittent()
{
	m_ptOut->hv_HasIntermittent = FALSE;
	//写判断是否有间歇区域的逻辑
}

void CMeasure::InitOutputValue()
{
	GenEmptyObj(&m_ptOut->tCoat.ho_Region);
	GenEmptyObj(&m_ptOut->tBaseCoat.ho_Region);
	GenEmptyObj(&m_ptOut->tFoil.ho_Region);
	GenEmptyObj(&m_ptOut->ho_Disp);
	GenEmptyObj(&m_ptOut->tIntermittent.ho_Region);
	GenEmptyObj(&m_ptOut->tAT11.ho_Region);
	GenEmptyObj(&m_ptOut->tSpotInspection.ho_Region);
	GenEmptyObj(&m_ptOut->tDefect.ho_Region);

	m_ptOut->hv_HasCoat = TRUE;
	m_ptOut->hv_HasIntermittent = FALSE;
	m_ptOut->hv_LineNum = m_tInit.tProductInfo.nMeasureLineNum;
	TupleGenConst(m_tInit.tProductInfo.nMeasureLineNum - 1, NO_DATA, &m_ptOut->tSize.hv_StripeSN);
	TupleGenConst(m_tInit.tProductInfo.nMeasureLineNum - 1, NO_DATA, &m_ptOut->tSize.hv_MisalignedDist);
	TupleGenConst(m_tInit.tProductInfo.nMeasureLineNum - 1, NO_DATA, &m_ptOut->tSize.hv_Width);
	TupleGenConst((int)m_tInit.tProductInfo.tParam.vecCoatWidth.size(), NO_DATA, &m_ptOut->tSize.hv_WholeBaseCoat);
	m_ptOut->tSize.hv_WholeWidth = NO_DATA;

	TupleGenConst(m_tInit.tProductInfo.nMeasureLineNum, NO_DATA, &m_ptOut->tLine.hv_StartColumn);
	TupleGenConst(m_tInit.tProductInfo.nMeasureLineNum, NO_DATA, &m_ptOut->tLine.hv_EndColumn);
	TupleGenConst(m_tInit.tProductInfo.nMeasureLineNum, NO_DATA, &m_ptOut->tLine.hv_StartRow);
	TupleGenConst(m_tInit.tProductInfo.nMeasureLineNum, NO_DATA, &m_ptOut->tLine.hv_EndRow);

	TupleGenConst((int)m_tInit.tProductInfo.tParam.vecCoatWidth.size(), NO_DATA, &m_ptOut->tCoat.hv_Width);
	TupleGenConst((int)m_tInit.tProductInfo.tParam.vecCoatWidth.size(), NO_DATA, &m_ptOut->tCoat.hv_StripeSN);

	TupleGenConst((int)m_tInit.tProductInfo.tParam.vecFoilWidth.size(), NO_DATA, &m_ptOut->tFoil.hv_Width);
	TupleGenConst((int)m_tInit.tProductInfo.tParam.vecFoilWidth.size(), NO_DATA, &m_ptOut->tFoil.hv_StripeSN);

	if (m_tInit.tProductInfo.bExistAT11)
	{
		TupleGenConst((int)m_tInit.tProductInfo.tParam.vecAT11Width.size(), NO_DATA, &m_ptOut->tAT11.hv_Width);
		TupleGenConst((int)m_tInit.tProductInfo.tParam.vecAT11Width.size(), NO_DATA, &m_ptOut->tAT11.hv_StripeSN);
	}
	else if (m_tInit.tProductInfo.bExistBaseCoat)
	{
		TupleGenConst((int)m_tInit.tProductInfo.tParam.vecBaseCoatWidth.size(), NO_DATA, &m_ptOut->tBaseCoat.hv_Width);
		TupleGenConst((int)m_tInit.tProductInfo.tParam.vecBaseCoatWidth.size(), NO_DATA, &m_ptOut->tBaseCoat.hv_StripeSN);
	}

	/*m_ptOut->tSize.hv_StripeSN.Clear();
	m_ptOut->tSize.hv_MisalignedDist.Clear();
	m_ptOut->tSize.hv_Width.Clear();
	m_ptOut->tLine.hv_StartColumn.Clear();
	m_ptOut->tLine.hv_EndColumn.Clear();
	m_ptOut->tLine.hv_StartRow.Clear();
	m_ptOut->tLine.hv_EndRow.Clear();
	m_ptOut->tCoat.hv_StripeSN.Clear();
	m_ptOut->tCoat.hv_Width.Clear();
	m_ptOut->tFoil.hv_StripeSN.Clear();
	m_ptOut->tFoil.hv_Width.Clear();
	m_ptOut->tAT11.hv_StripeSN.Clear();
	m_ptOut->tAT11.hv_Width.Clear();
	m_ptOut->tBaseCoat.hv_StripeSN.Clear();
	m_ptOut->tBaseCoat.hv_Width.Clear();*/
	m_ptOut->tIntermittent.hv_StripeSN.Clear();
	m_ptOut->tIntermittent.hv_Width.Clear();
	m_ptOut->tSpotInspection.hv_StripeSN.Clear();
	m_ptOut->tSpotInspection.hv_Width.Clear();

	TupleGenConst(5, NO_DATA, &m_ptOut->tSpotInspection.hv_Width);

	m_ptOut->tDefect.hv_StripeSN.Clear();
	m_ptOut->tDefect.hv_Type.Clear();

	m_hvErrorLineIndex.Clear();
}

int CMeasure::Action(MeasureInput &tIn, PMeasureOutput ptOut)
{
	m_ptOut = ptOut;
	m_ptIn = &tIn;
	HTuple hvStart, hvEnd;
	CountSeconds(&hvStart);
	if (m_tInit.hvCamSNList.Length() == 2 && m_ptIn->hoGrabImage.CountObj() > 1)
	{
		//两个相机图像拼接成一张图像
		TileImages(m_ptIn->hoGrabImage, &m_ptIn->hoGrabImage, 2, "horizontal");
	}
	CountSeconds(&hvEnd);

	CString strTime;
	strTime.Format("CMeasure: TileImages Consume%0.3fms", (hvEnd.D() - hvStart.D()) * 1000);
	m_tInit.pLog(strTime, DebugLog, NoExcp);

	GetImageSize(tIn.hoGrabImage, &m_hvImageWidth, &m_hvImageHeight);
	ptOut->ho_DispImage = tIn.hoGrabImage;

	BOOL bRet = TRUE;
	//初始化输出结果
	InitOutputValue();
	m_Calibration.CalStripeTmpParam();
	try
	{
		//if (tIn.nMode != SpotInspectionMode)
		{
			//测量
			Measure();

			//GRR模式
			GRRTest(tIn);
		}
	}
	catch (HException& e)
	{
		CString strMessageBox;
		strMessageBox.Format("CMeasure_Action: 测量算法%d出现异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strMessageBox, ExcpLog, SlightExcp);

		bRet = FALSE;
	}
	try
	{
		//点检
		if (tIn.nMode == DebugMode || tIn.nMode == RunMode)
		{
			SpotInspect();
		}
	}
	catch (HException& e)
	{
		CString strMessageBox;
		strMessageBox.Format("CMeasure_Action: 点检算法%d出现异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strMessageBox, ExcpLog, SlightExcp);

		bRet = FALSE;
	}
	try
	{
		if (tIn.nMode != GRRMode && ptOut->hv_HasCoat.I() == TRUE && m_tInit.tProductInfo.tParam.bUseSurfaceDetect)
		{
			//表面检测
			Detect();
		}
	}
	catch (HException& e)
	{
		CString strMessageBox;
		strMessageBox.Format("CMeasure_Action: 瑕疵算法%d出现异常 %s [%s]", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strMessageBox, ExcpLog, SlightExcp);

		bRet = FALSE;
	}
	
	return bRet;
}

BOOL CMeasure::WriteParam()
{
	m_LineMeasure.WriteParam();
	m_Calibration.WriteParam();
	m_Defect.WriteParam();
	m_SpotInspection.WriteParam();

	//无涂层判断
	WritePrivateProfileInt("PreprocParam", "bUseJudgeUncoat", m_tPreprocParam.bUseJudgeUncoat, m_strParamFile);
	WritePrivateProfileInt("PreprocParam", "nUncoatThresLight", m_tPreprocParam.nUncoatThresLight, m_strParamFile);
	WritePrivateProfileInt("PreprocParam", "nUncoatThresDark", m_tPreprocParam.nUncoatThresDark, m_strParamFile);
	WritePrivateProfileDouble("PreprocParam", "dUncoatAreaRate", m_tPreprocParam.dUncoatAreaRate, m_strParamFile);

	WritePrivateProfileInt("MeasureAdvanceParam", "FoilCalSelect", m_tMeasureAdvanceParam.eFoilSelect, m_strParamFile);
	WritePrivateProfileInt("MeasureAdvanceParam", "MisalignmentSelect", m_tMeasureAdvanceParam.eMisalignmentSelect, m_strParamFile);
	WritePrivateProfileInt("MeasureAdvanceParam", "CoatSelect", m_tMeasureAdvanceParam.eCoatSelect, m_strParamFile);

	return TRUE;
}

int CMeasure::DisplaySetDialog(MeasureInput & tIn, CString strSubMessage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgParam dlg;
	dlg.m_pReal = this;
	*dlg.m_pTemp = *this;
	dlg.m_tInput = tIn;
	if (strSubMessage == "CheckMeasure")
		dlg.m_bConfirmMeasure = TRUE;

	dlg.DoModal();

	return TRUE;
}

BOOL CMeasure::ReadParam()
{
	m_strParamFile.Format("%s//Config//Products//%s//SCIMeasure//%d//Parameters.ini", GetCurrentAppPath(), m_tInit.tProductInfo.strPDName, m_tInit.nSN);

	//涂层判断
	m_tPreprocParam.bUseJudgeUncoat = GetPrivateProfileInt("PreprocParam", "bUseJudgeUncoat", 0, m_strParamFile);
	m_tPreprocParam.nUncoatThresLight = GetPrivateProfileInt("PreprocParam", "nUncoatThresLight", 200, m_strParamFile);
	m_tPreprocParam.nUncoatThresDark = GetPrivateProfileInt("PreprocParam", "nUncoatThresDark", 30, m_strParamFile);
	m_tPreprocParam.dUncoatAreaRate = GetPrivateProfileDouble("PreprocParam", "dUncoatAreaRate", 0.6, m_strParamFile);

	m_tMeasureAdvanceParam.eFoilSelect = (eFoilSelect)GetPrivateProfileInt("MeasureAdvanceParam", "FoilCalSelect", FS_OnlyFoil, m_strParamFile);
	m_tMeasureAdvanceParam.eMisalignmentSelect = (eMisalignmentSelect)GetPrivateProfileInt("MeasureAdvanceParam", "MisalignmentSelect", MS_WidthSum, m_strParamFile);
	m_tMeasureAdvanceParam.eCoatSelect = (eCoatSelect)GetPrivateProfileInt("MeasureAdvanceParam", "CoatSelect", CS_FullCoat, m_strParamFile);

	m_LineMeasure.ReadParam();
	m_Calibration.ReadParam();
	m_Defect.ReadParam();
	m_SpotInspection.ReadParam();
	ReadJZ();
	return TRUE;
}
void CMeasure::ReadJZ()
{
	CString strFile;
	strFile.Format("%s//Config//Public//JiaoZhun//%d//Param.ini", GetCurrentAppPath(), m_tInit.nSN);
	m_tJZ.nJysl = GetPrivateProfileInt("Param", "nJysl", 4, strFile);
	m_tJZ.dSingleWidth = GetPrivateProfileDouble("Param", "dSingleWidth", 2.0, strFile);
	m_tJZ.dSigma = GetPrivateProfileDouble("Param", "dSigma", 5.0, strFile);;
	m_tJZ.nThres = GetPrivateProfileInt("Param", "nThres", 50, strFile);

	for (int i = 0; i < m_tInit.hvCamSNList.Length(); i++)
	{

		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvJyDistance-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(ReadTuple(HTuple(strFile), &m_tJZ.hvJyDistance[i]));
		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvPix-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(ReadTuple(HTuple(strFile), &m_tJZ.hvPix[i]));
		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvReal-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(ReadTuple(HTuple(strFile), &m_tJZ.hvReal[i]));
		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvRes-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(ReadTuple(HTuple(strFile), &m_tJZ.hvRes[i]));
	}
}
void CMeasure::WriteJZ()
{
	CString strPath, strFile;
	strPath.Format("%s//Config//Public//JiaoZhun//%d//", GetCurrentAppPath(), m_tInit.nSN);
	if (!PathFileExists(strPath))
	{
		CreateMultilevelPath(strPath);
	}
	strFile.Format("%s//Config//Public//JiaoZhun//%d//Param.ini", GetCurrentAppPath(), m_tInit.nSN);
	WritePrivateProfileInt("Param", "nJysl", m_tJZ.nJysl, strFile);
	WritePrivateProfileDouble("Param", "dSingleWidth", m_tJZ.dSingleWidth, strFile);
	WritePrivateProfileDouble("Param", "dSigma", m_tJZ.dSigma, strFile);;
	WritePrivateProfileInt("Param", "nThres", m_tJZ.nThres, strFile);

	for (int i = 0; i < m_tInit.hvCamSNList.Length(); i++)
	{
		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvPix-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(WriteTuple(m_tJZ.hvPix[i], HTuple(strFile)));
		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvReal-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(WriteTuple(m_tJZ.hvReal[i], HTuple(strFile)));
		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvRes-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(WriteTuple(m_tJZ.hvRes[i], HTuple(strFile)));
		strFile.Format("%s//Config//Public//JiaoZhun//%d//hvJyDistance-%d.tup", GetCurrentAppPath(), m_tInit.nSN, m_tInit.hvCamSNList[i].I());
		HALCON_TRY(WriteTuple(m_tJZ.hvJyDistance[i], HTuple(strFile)));
	}
}
BOOL CMeasure::IsLineFind(const int& nLineSN)
{
	//如果不需要判断找没找到边，返回TRUE
	//return TRUE;
	if (nLineSN < 0 || nLineSN >= m_tInit.tProductInfo.nMeasureLineNum) return FALSE;

	try
	{
		HTuple hvFind;
		TupleFind(m_hvErrorLineIndex, nLineSN, &hvFind);
		if (hvFind != -1 && hvFind.Length() > 0)
			return FALSE;
		else
			return TRUE;
	}
	catch (HException& e)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMeasure::_GetLineAngle(int& nSN, HTuple& hvStartRow, HTuple& hvStartCol, HTuple& hvEndRow, HTuple& hvEndCol, HTuple* hvAngle)
{
	if (m_tInit.hvCamSNList.Length() == 1 && nSN > 0) return FALSE;
	if (m_tInit.hvCamSNList.Length() == 2 && nSN > 1) return FALSE;

	double dResX = m_Calibration.GetResolution()[nSN].dColRes;
	double dResY = m_Calibration.GetResolution()[nSN].dRowRes;

	try
	{
		HTuple hvWX1, hvWX2, hvWY1, hvWY2;
		if (!m_Calibration.CalPoint(hvStartCol, hvWX1)) return FALSE;
		if (!m_Calibration.CalPoint(hvEndCol, hvWX2)) return FALSE;
		hvWY1 = 0.0;
		hvWY2 = (hvEndRow - hvStartRow)*dResY;
		HTuple hvWk = (hvWY2 - hvWY1) / (hvWX2 - hvWX1);
		HTuple hvPhi, hvAbs, hvAbs2, hvDiff, hvCos;
		//*********角度转换
		LineOrientation(hvWY2, hvWX2, hvWY1, hvWX1, &hvPhi);
		TupleFabs(hvPhi, &hvAbs);
		TupleSub(hvAbs, PI / 2, &hvDiff);
		TupleFabs(hvDiff, &hvAbs2);
		//*********角度判断,halcon里应该有个设置，角度好像可以从系统设置里是0-360，还是-180到180
		//********这里角度的大小方向可能会有问题，需要测试
		TupleCos(hvAbs2, &hvCos);

		*hvAngle = hvCos;
	}
	catch (HException&e)
	{
		return FALSE;
	}

	return TRUE;
}

void CMeasure::_CorrectValue(HTuple hvVal, double dStd, double dTolerance, HTuple* hvValCorrect)
{
	HTuple hvSub;
	TupleSub(dStd, hvVal, &hvSub);
	//小于公差-0.01的，不用修正，大于公差的/5，可以将0.5以内的误差修正到误差0.1以内
	/**hvValCorrect = dStd - (hvSub.TupleAbs().D() < (max(0,abs(dTolerance)-0.01)) ? hvSub.D() : hvSub.D() / 5.0);*/
	*hvValCorrect = dStd - (hvSub.TupleAbs().D() < /*(max(0, abs(dTolerance) - 0.01))*/0.09 ? hvSub.D() : hvSub.D() / 5.0);
}

void CMeasure::SaveParamLog(CMeasure Old, CMeasure New)
{
	//参数修改日志
	CString strName;
	//k值参数
	for (int i = 0; i < Old.m_Calibration.m_tStripeParam.vecKValue.size(); i++)
	{
		strName.Format("算法%d,k值%d", m_tInit.nSN, i);
		_OperateLog(Old.m_Calibration.m_tStripeParam.vecKValue[i], New.m_Calibration.m_tStripeParam.vecKValue[i], strName);
	}

	//两相机交界处补偿参数
	if (m_tInit.hvCamSNList.Length() == 2)
	{
		strName.Format("算法%d相机交界处补偿", m_tInit.nSN);
		_OperateLog(Old.m_Calibration.m_tStripeParam.dInternOffset, New.m_Calibration.m_tStripeParam.dInternOffset, strName);
	}
}

void CMeasure::_OperateLog(const double& dOld, const double& dCurrent, const CString& strOperateName)
{
	if (dOld == dCurrent) return;

	CString strLog;
	strLog.Format("'%s'由(%f)更改为(%f)", strOperateName, dOld, dCurrent);
	m_tInit.pLog(strLog, (LogType)OperateLog, NoExcp);
}

void CMeasure::_OperateLog(int& nOld, int& nCurrent, const CString& strOperateName)
{
	if (nOld == nCurrent) return;

	CString strLog;
	strLog.Format("'%s'由(%d)更改为(%d)", strOperateName, nOld, nCurrent);
	m_tInit.pLog(strLog, (LogType)OperateLog, NoExcp);
}

void CMeasure::_OperateLog(const CString& strOld, const CString& strCurrent, const CString& strOperateName)
{
	if (strOld == strCurrent) return;

	CString strLog;
	strLog.Format("'%s'由(%s)更改为(%s)", strOperateName, strOld, strCurrent);
	m_tInit.pLog(strLog, (LogType)OperateLog, NoExcp);
}