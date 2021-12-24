#include "StdAfx.h"
#include "HalconCpp.h"
#include "HDevThread.h"
using namespace HalconCpp;

// Short Description: �ڵ��� 
void BlackSpot (HObject ho_Image, HObject ho_Region, HObject *ho_ErrorRegion, HTuple hv_Param);
// Short Description: Ϳ��©����� 
void FoilLeak (HObject ho_Image, HObject ho_Region, HObject *ho_ErrorRegion, HTuple hv_Param);
// Short Description: �׵��� 
void WhiteSpot (HObject ho_Image, HObject ho_Region, HObject *ho_ErrorRegion, HTuple hv_Param);


void TileImages(HObject ho_Images, HObject *ho_TiledImage);

// Short Description:  This procedure plots tuples representing functions or curves in a coordinate system. 
void plot_tuple(HTuple hv_WindowHandle, HTuple hv_XValues, HTuple hv_YValues, HTuple hv_XLabel,
	HTuple hv_YLabel, HTuple hv_Color, HTuple hv_GenParamName, HTuple hv_GenParamValue);