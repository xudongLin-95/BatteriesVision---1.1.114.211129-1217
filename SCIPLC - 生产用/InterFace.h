#pragma once
#include "stdafx.h"
#include <vector>
#include<atlimage.h>
typedef struct  tagTALGORITHMINIT
{
	CString strProductName;//品种名称
	int nGrabberSN;//相机序号，默认为0
	int nAlgSN;//算法编号，默认为0
	int nDetectType;//产品类型
}ALGORITHMINIT, *PALGORITHMINIT;

typedef struct  tagTDETECTINPUT1
{
	HObject hoGrabImage;

}DETECTINPUT1, *PDETECTINPUT1;

typedef struct tagTDETECTINFO
{
	HTuple  hvWidth;
	HTuple  hvHeight;
	HTuple  hvAngle;
	HTuple  hvDistance;
	HObject hoDispRegion;

}DETECTOUTPUT1;
