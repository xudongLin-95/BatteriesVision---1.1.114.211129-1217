#ifndef _SCI_DATAFLOW_
#define _SCI_DATAFLOW_

#include "stdafx.h"
#include "DH_HalconEx.h"

#define NG	FALSE	//NG标志位
#define OK TRUE		//OK标志位
#define WARN 2		//预警标志位

typedef struct tagTA_GrabWork//采集车间产出
{
	HObject hoGrabImage;	//采集到的图像
	HTuple  hvGrabberSN;	//相机序号
	HTuple  hvFrameSN;		//第几帧
	HTuple  hvEncoder;		//编码器行数
	HTuple  hvImageSN;		//图像号
	HTuple  hvTimeStamp;	//时间戳
	HTuple  hvReserve;		//保留，备用
	HTuple  hvSignalNumber;	//输入信号计数
	HTuple  hvSpeed;		//速度参数
	HTuple	hvRunDirection;	//元素0:设备方向1前进，-1后退，0停止，元素1:进退刀:1进刀，0退刀，元素2:是否涂:1在涂，0未涂
	inline int ReadMessage(const HTuple& hvMessage, const int& nGrabberSN)//读取带序号的消息
	{
		CString strKey;
		strKey.Format("A%d_Encoder", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvEncoder));
		strKey.Format("A%d_FrameSN", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvFrameSN));
		strKey.Format("A%d_ImageSN", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvImageSN));
		strKey.Format("A%d_GrabberSN", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvGrabberSN));
		strKey.Format("A%d_TimeStamp", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvTimeStamp));
		strKey.Format("A%d_Reserve", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvReserve));
		strKey.Format("A%d_Image", nGrabberSN);
		HALCON_TRY(GetMessageObj(&hoGrabImage, hvMessage, HTuple(strKey)));
		strKey.Format("A%d_SignalNumber", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvSignalNumber));
		strKey.Format("A%d_hvSpeed", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvSpeed));
		strKey.Format("A%d_hvRunDirection", nGrabberSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hvRunDirection));

		return TRUE;
	}
	inline int WriteMessage(const HTuple& hvMessage, const int& nGrabberSN)
	{
		CString strKey;
		strKey.Format("A%d_Encoder", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvEncoder));
		strKey.Format("A%d_FrameSN", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvFrameSN));
		strKey.Format("A%d_ImageSN", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvImageSN));
		strKey.Format("A%d_GrabberSN", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvGrabberSN));
		strKey.Format("A%d_TimeStamp", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvTimeStamp));
		strKey.Format("A%d_Reserve", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvReserve));
		strKey.Format("A%d_Image", nGrabberSN);
		HALCON_TRY(SetMessageObj(hoGrabImage, hvMessage, HTuple(strKey)));
		strKey.Format("A%d_SignalNumber", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvSignalNumber));
		strKey.Format("A%d_hvSpeed", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvSpeed));
		strKey.Format("A%d_hvRunDirection", nGrabberSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hvRunDirection));

		return TRUE;
	}
}A_GrabWork, *PA_GrabWork;

typedef std::vector<A_GrabWork>		AWorkList;
typedef struct tagTB_DetectWork//检测车间产出
{
	AWorkList				vecA;				//A车间数组
	HTuple					hv_UsedCamSNList;	//关联了几个相机
	HTuple					hv_SideSN;			//检测面序号
	MeasureOutput			tMeasure;			//检测结果
	HTuple					hv_TimeStamp;		//时间戳
	inline int ReadMessage(const HTuple& hvMessage, const int& nSideSN)
	{
		CString strKey;

		strKey.Format("B%d_hv_UsedCamSNList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_UsedCamSNList));
		strKey.Format("B%d_hv_SideSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_SideSN));
		strKey.Format("B%d_hv_TimeStamp", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_TimeStamp));
		/////测量
		strKey.Format("B%d_tMeasure_tSize_hv_Width", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSize.hv_Width));
		strKey.Format("B%d_tMeasure_tSize_hv_WidthOrigin", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSize.hv_WidthOrigin));
		strKey.Format("B%d_tMeasure_tSize_hv_MisalignedDist", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSize.hv_MisalignedDist));
		strKey.Format("B%d_tMeasure_tSize_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSize.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tSize_hv_WholeBaseCoat", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSize.hv_WholeBaseCoat));
		strKey.Format("B%d_tMeasure_tSize_hv_WholeWidth", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSize.hv_WholeWidth));
		strKey.Format("B%d_tMeasure_hv_LineNum", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.hv_LineNum));
		strKey.Format("B%d_tMeasure_tLine_hv_StartRow", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tLine.hv_StartRow));
		strKey.Format("B%d_tMeasure_tLine_hv_EndRow", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tLine.hv_EndRow));
		strKey.Format("B%d_tMeasure_tLine_hv_StartColumn", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tLine.hv_StartColumn));
		strKey.Format("B%d_tMeasure_tLine_hv_EndColumn", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tLine.hv_EndColumn));
		strKey.Format("B%d_tMeasure_hv_HasCoat", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.hv_HasCoat));
		strKey.Format("B%d_tMeasure_hv_HasIntermittent", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.hv_HasIntermittent));

		strKey.Format("B%d_tMeasure_tCoat_hv_Width", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tCoat.hv_Width));
		strKey.Format("B%d_tMeasure_tCoat_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tCoat.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tCoat_ho_Region", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.tCoat.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tFoil_hv_Width", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tFoil.hv_Width));
		strKey.Format("B%d_tMeasure_tFoil_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tFoil.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tFoil_ho_Region", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.tFoil.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tAT11_hv_Width", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tAT11.hv_Width));
		strKey.Format("B%d_tMeasure_tAT11_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tAT11.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tAT11_ho_Region", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.tAT11.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tBaseCoat_hv_Width", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tBaseCoat.hv_Width));
		strKey.Format("B%d_tMeasure_tBaseCoat_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tBaseCoat.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tBaseCoat_ho_Region", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.tBaseCoat.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tIntermittent_hv_Width", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tIntermittent.hv_Width));
		strKey.Format("B%d_tMeasure_tIntermittent_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tIntermittent.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tIntermittent_ho_Region", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.tIntermittent.ho_Region, hvMessage, HTuple(strKey)));
		//点检结果
		strKey.Format("B%d_tMeasure_tSpotInspection_hv_Width", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSpotInspection.hv_Width));
		strKey.Format("B%d_tMeasure_tSpotInspection_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tSpotInspection.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tSpotInspection_ho_Region", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.tSpotInspection.ho_Region, hvMessage, HTuple(strKey)));
		//瑕疵结果
		strKey.Format("B%d_tMeasure_tDefect_hv_Type", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.hv_Type));
		strKey.Format("B%d_tMeasure_tDefect_hv_StripeSN", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tDefect_ho_Region", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.tDefect.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tCenter_hv_Row", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.tCenter.hv_Row));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tCenter_hv_Column", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.tCenter.hv_Column));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvArea", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.hvArea));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvWidth", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.hvWidth));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvHeight", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.hvHeight));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvDiameter", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.hvDiameter));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tLeftTop_hv_Row", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Row));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tLeftTop_hv_Column", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Column));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tRightBottom_hv_Row", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.tRect.tRightBottom.hv_Row));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tRightBottom_hv_Column", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &tMeasure.tDefect.tFeature.tRect.tRightBottom.hv_Column));

		strKey.Format("B%d_tMeasure_ho_Disp", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.ho_Disp, hvMessage, HTuple(strKey)));
		//新添加 用于结果图像显示
		strKey.Format("B%d_tMeasure_ho_DispImage", nSideSN);
		HALCON_TRY(GetMessageObj(&tMeasure.ho_DispImage, hvMessage, HTuple(strKey)));

		vecA.clear();
		for (int i = 0; i < hv_UsedCamSNList.Length(); i++)
		{
			A_GrabWork tA;
			tA.ReadMessage(hvMessage, hv_UsedCamSNList[i]);
			vecA.push_back(tA);
		}

		return TRUE;
	}
	inline int WriteMessage(const HTuple& hvMessage, const int& nSideSN)
	{
		CString strKey;
		strKey.Format("B%d_hv_UsedCamSNList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_UsedCamSNList));
		strKey.Format("B%d_hv_SideSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_SideSN));
		strKey.Format("B%d_hv_TimeStamp", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_TimeStamp));
		/////测量
		strKey.Format("B%d_tMeasure_tSize_hv_Width", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSize.hv_Width));
		strKey.Format("B%d_tMeasure_tSize_hv_WidthOrigin", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSize.hv_WidthOrigin));
		strKey.Format("B%d_tMeasure_tSize_hv_MisalignedDist", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSize.hv_MisalignedDist));
		strKey.Format("B%d_tMeasure_tSize_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSize.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tSize_hv_WholeBaseCoat", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSize.hv_WholeBaseCoat));
		strKey.Format("B%d_tMeasure_tSize_hv_WholeWidth", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSize.hv_WholeWidth));
		strKey.Format("B%d_tMeasure_hv_LineNum", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.hv_LineNum));
		strKey.Format("B%d_tMeasure_tLine_hv_StartRow", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tLine.hv_StartRow));
		strKey.Format("B%d_tMeasure_tLine_hv_EndRow", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tLine.hv_EndRow));
		strKey.Format("B%d_tMeasure_tLine_hv_StartColumn", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tLine.hv_StartColumn));
		strKey.Format("B%d_tMeasure_tLine_hv_EndColumn", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tLine.hv_EndColumn));
		strKey.Format("B%d_tMeasure_hv_HasCoat", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.hv_HasCoat));
		strKey.Format("B%d_tMeasure_hv_HasIntermittent", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.hv_HasIntermittent));

		strKey.Format("B%d_tMeasure_tCoat_hv_Width", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tCoat.hv_Width));
		strKey.Format("B%d_tMeasure_tCoat_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tCoat.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tCoat_ho_Region", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.tCoat.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tFoil_hv_Width", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tFoil.hv_Width));
		strKey.Format("B%d_tMeasure_tFoil_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tFoil.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tFoil_ho_Region", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.tFoil.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tAT11_hv_Width", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tAT11.hv_Width));
		strKey.Format("B%d_tMeasure_tAT11_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tAT11.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tAT11_ho_Region", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.tAT11.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tBaseCoat_hv_Width", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tBaseCoat.hv_Width));
		strKey.Format("B%d_tMeasure_tBaseCoat_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tBaseCoat.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tBaseCoat_ho_Region", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.tBaseCoat.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tIntermittent_hv_Width", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tIntermittent.hv_Width));
		strKey.Format("B%d_tMeasure_tIntermittent_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tIntermittent.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tIntermittent_ho_Region", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.tIntermittent.ho_Region, hvMessage, HTuple(strKey)));

		//点检结果
		strKey.Format("B%d_tMeasure_tSpotInspection_hv_Width", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSpotInspection.hv_Width));
		strKey.Format("B%d_tMeasure_tSpotInspection_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tSpotInspection.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tSpotInspection_ho_Region", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.tSpotInspection.ho_Region, hvMessage, HTuple(strKey)));
		//瑕疵结果
		strKey.Format("B%d_tMeasure_tDefect_hv_Type", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.hv_Type));
		strKey.Format("B%d_tMeasure_tDefect_hv_StripeSN", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.hv_StripeSN));
		strKey.Format("B%d_tMeasure_tDefect_ho_Region", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.tDefect.ho_Region, hvMessage, HTuple(strKey)));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tCenter_hv_Row", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.tCenter.hv_Row));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tCenter_hv_Column", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.tCenter.hv_Column));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvArea", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.hvArea));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvWidth", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.hvWidth));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvHeight", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.hvHeight));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_hvDiameter", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.hvDiameter));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tLeftTop_hv_Row", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Row));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tLeftTop_hv_Column", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Column));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tRightBottom_hv_Row", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.tRect.tRightBottom.hv_Row));
		strKey.Format("B%d_tMeasure_tDefect_tFeature_tRect_tRightBottom_hv_Column", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), tMeasure.tDefect.tFeature.tRect.tRightBottom.hv_Column));

		strKey.Format("B%d_tMeasure_ho_Disp", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.ho_Disp, hvMessage, HTuple(strKey)));
		//新添加 合并结果图像
		strKey.Format("B%d_tMeasure_ho_DispImage", nSideSN);
		HALCON_TRY(SetMessageObj(tMeasure.ho_DispImage, hvMessage, HTuple(strKey)));

		for (int i = 0; i < hv_UsedCamSNList.Length(); i++)
		{
			vecA[i].WriteMessage(hvMessage, hv_UsedCamSNList[i]);
		}

		return TRUE;
	}
}B_DetectWork, *PB_DetectWork;
typedef std::vector<B_DetectWork>		BWorkList;

typedef struct tagTSingleResultResult//单条结果
{
	HTuple hv_FinalFlag;		//单条最终结果标志位 OK, NG, WARN
	HTuple hv_MeasureFlag;		//单条测量最终结果标志位 OK, NG, WARN
	HTuple hv_DetectFlag;		//单条缺陷最终结果标志位 OK, NG, WARN
	HTuple hv_ErrorType;		//单条NG/预警缺陷类型，可包含多个

	inline int ReadMessage(const HTuple& hvMessage, const int& nStripeSN)
	{
		CString strKey;
		strKey.Format("C_Stripe%d_hv_FinalFlag", nStripeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_FinalFlag));
		strKey.Format("C_Stripe%d_hv_MeasureFlag", nStripeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_MeasureFlag));
		strKey.Format("C_Stripe%d_hv_DetectFlag", nStripeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_DetectFlag));
		strKey.Format("C_Stripe%d_hv_ErrorType", nStripeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_ErrorType));

		return TRUE;
	}

	inline int WriteMessage(const HTuple& hvMessage, const int& nStripeSN)
	{
		CString strKey;
		strKey.Format("C_Stripe%d_hv_FinalFlag", nStripeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_FinalFlag));
		strKey.Format("C_Stripe%d_hv_MeasureFlag", nStripeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_MeasureFlag));
		strKey.Format("C_Stripe%d_hv_DetectFlag", nStripeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_DetectFlag));
		strKey.Format("C_Stripe%d_hv_ErrorType", nStripeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_ErrorType));

		return TRUE;
	}

}SingleStripeResult, *PSingleStripeResult;
typedef std::vector<SingleStripeResult>		StripeResultList;

typedef struct tagTSingleSideResult//单个检测面所有区域的结果
{
	HTuple hv_Flag;								//单面检测标志位，OK, NG, WARN
	HTuple hv_AllWidthFlagList;					//所有区域宽度标志位，OK, NG, WARN
	HTuple hv_AllDetectFlagList;				//所有区域瑕疵标志位，OK, NG, WARN
	HTuple hv_CoatWidthFlagList;				//涂布区域宽度测量标志位，OK, NG, WARN
	HTuple hv_CoatDetectFlagList;				//涂布区域瑕疵检测标志位，OK, NG, WARN
	HTuple hv_FoilWidthFlagList;				//箔材区域宽度测量标志位，OK, NG, WARN
	HTuple hv_FoilDetectFlagList;				//箔材区域瑕疵检测标志位，OK, NG, WARN
	HTuple hv_AT11WidthFlagList;				//AT11/AT9区域宽度测量标志位，OK, NG, WARN
	HTuple hv_AT11DetectFlagList;				//AT11/AT9区域瑕疵检测标志位，OK, NG, WARN
	HTuple hv_BaseCoatWidthFlagList;			//凹版/底涂区域宽度测量标志位，OK, NG, WARN
	HTuple hv_BaseCoatDetectFlagList;			//凹版/底涂区域瑕疵检测标志位，OK, NG, WARN
	HTuple hv_SpotInspectionFlagList;			//点检标志位，OK, NG
	
	inline void Init()
	{
		hv_Flag = TRUE;
		hv_CoatWidthFlagList.Clear();
		hv_CoatDetectFlagList.Clear();
		hv_FoilWidthFlagList.Clear();
		hv_FoilDetectFlagList.Clear();
		hv_AT11WidthFlagList.Clear();
		hv_AT11DetectFlagList.Clear();
		hv_BaseCoatWidthFlagList.Clear();
		hv_BaseCoatDetectFlagList.Clear();
		hv_AllWidthFlagList.Clear();
		hv_AllDetectFlagList.Clear();
		hv_SpotInspectionFlagList.Clear();
	}

	inline int ReadMessage(const HTuple& hvMessage, const int& nSideSN)
	{
		CString strKey;
		strKey.Format("C_Side%d_hv_Flag", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_Flag));
		strKey.Format("C_Side%d_hv_AllWidthFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_AllWidthFlagList));
		strKey.Format("C_Side%d_hv_AllDetectFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_AllDetectFlagList));
		strKey.Format("C_Side%d_hv_CoatWidthFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_CoatWidthFlagList));
		strKey.Format("C_Side%d_hv_CoatDetectFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_CoatDetectFlagList));
		strKey.Format("C_Side%d_hv_FoilWidthFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_FoilWidthFlagList));
		strKey.Format("C_Side%d_hv_FoilDetectFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_FoilDetectFlagList));
		strKey.Format("C_Side%d_hv_AT11WidthFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_AT11WidthFlagList));
		strKey.Format("C_Side%d_hv_AT11DetectFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_AT11DetectFlagList));
		strKey.Format("C_Side%d_hv_BaseCoatWidthFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_BaseCoatWidthFlagList));
		strKey.Format("C_Side%d_hv_BaseCoatDetectFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_BaseCoatDetectFlagList));
		strKey.Format("C_Side%d_hv_SpotInspectionFlagList", nSideSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_SpotInspectionFlagList));

		return TRUE;
	}

	inline int WriteMessage(const HTuple& hvMessage, const int& nSideSN)
	{
		CString strKey;
		strKey.Format("C_Side%d_hv_Flag", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_Flag));
		strKey.Format("C_Side%d_hv_AllWidthFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_AllWidthFlagList));
		strKey.Format("C_Side%d_hv_AllDetectFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_AllDetectFlagList));
		strKey.Format("C_Side%d_hv_CoatWidthFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_CoatWidthFlagList));
		strKey.Format("C_Side%d_hv_CoatDetectFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_CoatDetectFlagList));
		strKey.Format("C_Side%d_hv_FoilWidthFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_FoilWidthFlagList));
		strKey.Format("C_Side%d_hv_FoilDetectFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_FoilDetectFlagList));
		strKey.Format("C_Side%d_hv_AT11WidthFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_AT11WidthFlagList));
		strKey.Format("C_Side%d_hv_AT11DetectFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_AT11DetectFlagList));
		strKey.Format("C_Side%d_hv_BaseCoatWidthFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_BaseCoatWidthFlagList));
		strKey.Format("C_Side%d_hv_BaseCoatDetectFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_BaseCoatDetectFlagList));
		strKey.Format("C_Side%d_hv_SpotInspectionFlagList", nSideSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_SpotInspectionFlagList));

		return TRUE;
	}

}SingleSideResult, *PSingleSideResult;
typedef std::vector<SingleSideResult>		SideResultList;

typedef struct tagTMergeResult//合并最终结果
{
	HTuple				hv_FinalFlag;			//合并结果总检测结果 OK, NG, WARN
	HTuple				hv_MeasureFlag;			//合并结果测量结果 OK, NG, WARN
	HTuple				hv_DetectFlag;			//合并结果瑕疵检测结果 OK, NG, WARN
	HTuple				hv_NGType;				//合并结果NG缺陷类型，可包含多个
	HTuple				hv_WarnType;			//合并结果预警缺陷类型，可包含多个
	HTuple				hv_Misalignment;		//AB面综合错位值
	HTuple				hv_MisalignmentFlagList;//AB面综合错位标志位，可包含多个 OK, NG, WARN
	HTuple				hv_SpotInspectionFlag;	//点检结果标志位，OK, NG
	//用于喷码
	HTuple				hv_NGType2;				//合并结果NG缺陷类型，不包含错位值，错位值无区域，可包含多个
	HObject				ho_NGRegion;			//合并结果NG缺陷类型，对应区域
	HTuple				hv_NGStripeSN;			//合并结果NG缺陷类型，对应分条数
	HTuple				hv_NGSideSN;			//合并结果NG缺陷类型，对应AB面

	SideResultList		vecSideResult;			//合并结果后的多个检测面结果
	inline void Init()
	{
		hv_FinalFlag = TRUE;
		hv_MeasureFlag = TRUE;
		hv_DetectFlag = TRUE;
		hv_NGType.Clear();
		hv_WarnType.Clear();
		hv_Misalignment = 0;
		hv_MisalignmentFlagList.Clear();
		hv_SpotInspectionFlag.Clear();
		hv_NGType2.Clear();
		GenEmptyObj(&ho_NGRegion);
		hv_NGStripeSN.Clear();
		hv_NGSideSN.Clear();
	}

	inline int ReadMessage(const HTuple& hvMessage, HTuple hvSNList, const int& nMergeSN)
	{
		CString strKey;
		strKey.Format("C%d_tMerge_hv_FinalFlag", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_FinalFlag));
		strKey.Format("C%d_tMerge_hv_MeasureFlag", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_MeasureFlag));
		strKey.Format("C%d_tMerge_hv_DetectFlag", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_DetectFlag));
		strKey.Format("C%d_tMerge_hv_NGType", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_NGType));
		strKey.Format("C%d_tMerge_hv_WarnType", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_WarnType));
		strKey.Format("C%d_tMerge_hv_Misalignment", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_Misalignment));
		strKey.Format("C%d_tMerge_hv_MisalignmentFlagList", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_MisalignmentFlagList));
		strKey.Format("C%d_tMerge_hv_SpotInspectionFlag", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_SpotInspectionFlag));
		//喷码用
		strKey.Format("C%d_tMerge_hv_NGType2", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_NGType2));
		strKey.Format("C%d_tMerge_hv_NGStripeSN", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_NGStripeSN));
		strKey.Format("C%d_tMerge_hv_NGSideSN", nMergeSN);
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_NGSideSN));
		strKey.Format("C%d_tMerge_ho_NGRegion", nMergeSN);
		HALCON_TRY(GetMessageObj(&ho_NGRegion, hvMessage, HTuple(strKey)));

		vecSideResult.clear();
		for (int i = 0; i < hvSNList.Length(); i++)
		{
			SingleSideResult tSSR;
			tSSR.ReadMessage(hvMessage, hvSNList[i]);
			vecSideResult.push_back(tSSR);
		}

		return TRUE;
	}

	inline int WriteMessage(const HTuple& hvMessage, HTuple hvSNList, const int& nMergeSN)
	{
		CString strKey;
		strKey.Format("C%d_tMerge_hv_FinalFlag", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_FinalFlag));
		strKey.Format("C%d_tMerge_hv_MeasureFlag", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_MeasureFlag));
		strKey.Format("C%d_tMerge_hv_DetectFlag", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_DetectFlag));
		strKey.Format("C%d_tMerge_hv_NGType", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_NGType));
		strKey.Format("C%d_tMerge_hv_WarnType", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_WarnType));
		strKey.Format("C%d_tMerge_hv_Misalignment", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_Misalignment));
		strKey.Format("C%d_tMerge_hv_MisalignmentFlagList", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_MisalignmentFlagList));
		strKey.Format("C%d_tMerge_hv_SpotInspectionFlag", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_SpotInspectionFlag));
		//喷码用
		strKey.Format("C%d_tMerge_hv_NGType2", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_NGType2));
		strKey.Format("C%d_tMerge_hv_NGStripeSN", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_NGStripeSN));
		strKey.Format("C%d_tMerge_hv_NGSideSN", nMergeSN);
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_NGSideSN));
		strKey.Format("C%d_tMerge_ho_NGRegion", nMergeSN);
		HALCON_TRY(SetMessageObj(ho_NGRegion, hvMessage, HTuple(strKey)));

		for (int i = 0; i < vecSideResult.size(); i++)
		{
			vecSideResult[i].WriteMessage(hvMessage, hvSNList[i]);
		}

		return TRUE;
	}
}MergeResult, *PMergeResult;

typedef struct tagTC_MergeWork
{
	BWorkList			vecB;					//B检测车间产出数组,（正反面元素0对应正面，元素1对应反面）,一个合并结果对应几个检测面
	HTuple				hv_MergeSN;				//合并结果序号，最终第几个结果
	HTuple				hv_UsedSideSNList;		//关联几个检测面
	MergeResult			tMerge;					//单个合并结果，综合一或多个检测面的结果
	HTuple				hv_PackageSN;			//第几个数据包
	HTuple				hv_MeterCount;			//记米
	HTuple				hv_TimeStamp;			//时间戳

	inline int ReadMessage(const HTuple& hvMessage)
	{
		CString strKey;
		strKey.Format("C_hv_MergeSN");
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_MergeSN));
		strKey.Format("C%d_hv_PackageSN", hv_MergeSN.I());
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_PackageSN));
		strKey.Format("C%d_hv_TimeStamp", hv_MergeSN.I());
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_TimeStamp));
		strKey.Format("C%d_hv_UsedSideSNList", hv_MergeSN.I());
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_UsedSideSNList));
		strKey.Format("C%d_hv_MeterCount", hv_MergeSN.I());
		HALCON_TRY(GetMessageTuple(hvMessage, HTuple(strKey), &hv_MeterCount));
	
		//合并结果
		tMerge.ReadMessage(hvMessage, hv_UsedSideSNList, hv_MergeSN.I());

		//B检测车间工位
		vecB.clear();
		for (int i = 0; i < hv_UsedSideSNList.Length(); i++)
		{
			B_DetectWork tB;
			tB.ReadMessage(hvMessage, hv_UsedSideSNList[i]);
			vecB.push_back(tB);
		}

		return TRUE;
	}

	inline int WriteMessage(const HTuple& hvMessage)
	{
		CString strKey;
		strKey.Format("C_hv_MergeSN");
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_MergeSN));
		strKey.Format("C%d_hv_PackageSN", hv_MergeSN.I());
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_PackageSN));
		strKey.Format("C%d_hv_TimeStamp", hv_MergeSN.I());
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_TimeStamp));
		strKey.Format("C%d_hv_UsedSideSNList", hv_MergeSN.I());
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_UsedSideSNList));
		strKey.Format("C%d_hv_MeterCount", hv_MergeSN.I());
		HALCON_TRY(SetMessageTuple(hvMessage, HTuple(strKey), hv_MeterCount));

		//合并结果
		tMerge.WriteMessage(hvMessage, hv_UsedSideSNList, hv_MergeSN.I());

		//B检测车间工位
		for (int i = 0; i < hv_UsedSideSNList.Length(); i++)
		{
			vecB[i].WriteMessage(hvMessage, hv_UsedSideSNList[i]);
		}

		return TRUE;
	}
}C_MergeWork, *PC_MergeWork;

#endif