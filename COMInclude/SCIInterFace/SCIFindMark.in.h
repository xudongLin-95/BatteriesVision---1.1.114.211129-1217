#pragma once
#include "stdafx.h"
namespace NS_SCIFindMark
{
	typedef struct tagTINITPARAM
	{
		CString strProductName;
		int     nCameraSN;
	}INITPARAM,*PINITPARAM;

	typedef struct tagTMARKRESULT
	{
		HTuple hv_Row;
		HTuple hv_Column;
		HTuple hv_Angle;
		HTuple hv_Score;
		HTuple hv_AllScore;
		HObject ho_DispObj;
	}MARKRESULT,*PMARKRESULT;
}
