#include "StdAfx.h"
#include "Defect.h"
#include "PageDefectSet.h"

CEvent	g_evtComplete;		//检测是否完成
CEvent	g_evtStart;			//检测是否开始

void* HThreadAction(void *hcthread)
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		T_Action();
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CDefect_HThreadAction is exceptional(HThreadAction线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_pAlgorithm->m_tInit.pLog(strException, ExcpLog, SlightExcp);
		bool is_direct_call = hcppthread->IsDirectCall();
		hcppthread->Exit();
		delete hcppthread;
		hcppthread = NULL;
		if (is_direct_call)
			throw e;
	}
}

void T_Action()
{
	HTuple hv_StartTime, hv_EndTime;
	while (TRUE)
	{
		WaitForSingleObject(g_evtStart.m_hObject, INFINITE);

		//执行检测函数
		g_pAlgorithm->m_Defect.Detect();
		//执行后完成
		g_evtComplete.SetEvent();
	}
}

CDefect::CDefect()
{
	GenEmptyObj(&m_hoZoomImage);
	GenEmptyObj(&m_hoAdaptedImage);
	m_bDetectEnd = FALSE;
	m_pThreadHdl = NULL;

	g_evtComplete.ResetEvent();
	g_evtStart.ResetEvent();

	//开启检测瑕疵小线程
	int nStartThread = 0;
	HDevThreadContext   hTContext;
	for (int i = 0; i < 1; i++)
	{
		m_pThreadHdl = new HDevThread(hTContext,
			(void*)HThreadAction, 0, 0);
		m_pThreadHdl->Start();
		nStartThread++;
	}
}

CDefect::~CDefect()
{
	if (NULL != m_pThreadHdl)
	{
		HALCON_TRY(m_pThreadHdl->Exit())
			delete m_pThreadHdl;
		m_pThreadHdl = NULL;
	}
}

void CDefect::Init(const MeasureInit& tInit)
{
	m_tInit = tInit;
	m_strParamFile.Format("%s//Config//Products//%s//Defect//%d", GetCurrentAppPath(), tInit.tProductInfo.strPDName, m_tInit.nSN);
	CreateMultilevelPath(m_strParamFile);
	m_strParamFile.Format("%s//Config//Products//%s//Defect//%d//Param.ini", GetCurrentAppPath(), tInit.tProductInfo.strPDName, m_tInit.nSN);

}

BOOL CDefect::ReadParam()
{
	m_tParameters.tPretreatParam.dBorderWidth = GetPrivateProfileDouble("tPretreatParam", "dBorderWidth", 1, m_strParamFile);
	m_tParameters.tPretreatParam.dScaleFactor = GetPrivateProfileDouble("tPretreatParam", "dScaleFactor", 1, m_strParamFile);

	m_tParameters.tPretreatParam.dMidLightCof = GetPrivateProfileDouble("tPretreatParam", "dMidLightCof", 0.5, m_strParamFile);
	m_tParameters.tPretreatParam.dMidDarkCof = GetPrivateProfileDouble("tPretreatParam", "dMidDarkCof", 0.5, m_strParamFile);
	m_tParameters.tPretreatParam.dBorderLightCof = GetPrivateProfileDouble("tPretreatParam", "dBorderLightCof", 0.5, m_strParamFile);
	m_tParameters.tPretreatParam.dBorderDarkCof = GetPrivateProfileDouble("tPretreatParam", "dBorderDarkCof", 0.5, m_strParamFile);
	m_tParameters.tPretreatParam.dZoomFactor = GetPrivateProfileDouble("tPretreatParam", "dZoomFactor", 0.5, m_strParamFile);

	m_tParameters.tLeakFoilParam.bUseDetect = GetPrivateProfileInt("LeakFoil", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tLeakFoilParam.nThresh = GetPrivateProfileInt("LeakFoil", "nThresh", 100, m_strParamFile);    //128为默认值
	m_tParameters.tLeakFoilParam.dMinArea = GetPrivateProfileDouble("LeakFoil", "dMinArea", 2, m_strParamFile);
	m_tParameters.tLeakFoilParam.nDynThresh = GetPrivateProfileInt("LeakFoil", "nDynThresh", 100, m_strParamFile);
	m_tParameters.tLeakFoilParam.dMinDiameter = GetPrivateProfileDouble("LeakFoil", "dMinDiameter", 0.5, m_strParamFile);
	m_tParameters.tLeakFoilParam.dMaxDiameter = GetPrivateProfileDouble("LeakFoil", "dMaxDiameter", 90, m_strParamFile);
	m_tParameters.tLeakFoilParam.dDeviation = GetPrivateProfileDouble("LeakFoil", "dDeviation", 12, m_strParamFile);

	m_tParameters.tAT11LeakFoilParam.bUseDetect = GetPrivateProfileInt("AT11LeakFoil", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tAT11LeakFoilParam.nMinThresh = GetPrivateProfileInt("AT11LeakFoil", "nMinThresh", 180, m_strParamFile);
	m_tParameters.tAT11LeakFoilParam.dMinWidth = GetPrivateProfileDouble("AT11LeakFoil", "dMinWidth", 1, m_strParamFile);
	m_tParameters.tAT11LeakFoilParam.dMinHeight = GetPrivateProfileInt("AT11LeakFoil", "dMinHeight", 1, m_strParamFile);

	//m_tParameters.tBubbleParam.bUseDetect = GetPrivateProfileInt("tBubbleParam", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tBubbleParam.bUseDetect = GetPrivateProfileInt("Bubble", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tBubbleParam.nDynThresh = GetPrivateProfileInt("Bubble", "nDynThresh", 12, m_strParamFile);
	m_tParameters.tBubbleParam.dMinDiameter = GetPrivateProfileDouble("Bubble", "dMinDiameter", 100, m_strParamFile);
	m_tParameters.tBubbleParam.dMinCircularity = GetPrivateProfileDouble("Bubble", "dMinCircularity", 0.5, m_strParamFile);
	m_tParameters.tBubbleParam.nInOutGrayDiff = GetPrivateProfileInt("Bubble", "nInOutGrayDiff", 3, m_strParamFile);

	m_tParameters.tLightLineParam.bUseDetect = GetPrivateProfileInt("LightLine", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tLightLineParam.nDynThresh = GetPrivateProfileInt("LightLine", "nDynThresh", 5, m_strParamFile);
	m_tParameters.tLightLineParam.dMinWidth = GetPrivateProfileDouble("LightLine", "dMinWidth", 0.5, m_strParamFile);
	m_tParameters.tLightLineParam.dMinHeight = GetPrivateProfileDouble("LightLine", "dMinHeight", 0.5, m_strParamFile);
	m_tParameters.tLightLineParam.dMinLengthWidthRatio = GetPrivateProfileDouble("LightLine", "dMinLengthWidthRatio", 5, m_strParamFile);

	m_tParameters.tDarkLineParam.bUseDetect = GetPrivateProfileInt("DarkLine", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tDarkLineParam.nDynThresh = GetPrivateProfileInt("DarkLine", "nDynThresh", 12, m_strParamFile);
	m_tParameters.tDarkLineParam.dMinWidth = GetPrivateProfileDouble("DarkLine", "dMinWidth", 0.5, m_strParamFile);
	m_tParameters.tDarkLineParam.dMinHeight = GetPrivateProfileDouble("DarkLine", "dMinHeight", 0.5, m_strParamFile);
	m_tParameters.tDarkLineParam.dMinLengthWidthRatio = GetPrivateProfileDouble("DarkLine", "dMinLengthWidthRatio", 5, m_strParamFile);

	m_tParameters.tLightDotParam.bUseDetect = GetPrivateProfileInt("LightDot", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tLightDotParam.nMinThresh = GetPrivateProfileInt("LightDot", "nMinThresh", 120, m_strParamFile);
	m_tParameters.tLightDotParam.nMaxThresh = GetPrivateProfileInt("LightDot", "nMaxThresh", 200, m_strParamFile);
	m_tParameters.tLightDotParam.dMinWidth = GetPrivateProfileDouble("LightDot", "dMinWidth", 1, m_strParamFile);
	m_tParameters.tLightDotParam.dMinHeight = GetPrivateProfileDouble("LightDot", "dMinHeight", 1, m_strParamFile);
	m_tParameters.tLightDotParam.dMinArea = GetPrivateProfileDouble("LightDot", "dMinArea", 0.8, m_strParamFile);
	m_tParameters.tLightDotParam.dMaxArea = GetPrivateProfileDouble("LightDot", "dMaxArea", 36000, m_strParamFile);
	m_tParameters.tLightDotParam.nMinDeviation = GetPrivateProfileInt("LightDot", "nMinDeviation", 20, m_strParamFile);
	m_tParameters.tLightDotParam.nMinGrayRange = GetPrivateProfileInt("LightDot", "nMinGrayRange", 40, m_strParamFile);

	m_tParameters.tDarkDotParam.bUseDetect = GetPrivateProfileInt("DarkDot", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tDarkDotParam.nMaxThresh = GetPrivateProfileInt("DarkDot", "nMinThresh", 50, m_strParamFile);
	m_tParameters.tDarkDotParam.dMinWidth = GetPrivateProfileDouble("DarkDot", "dMinWidth", 1, m_strParamFile);
	m_tParameters.tDarkDotParam.dMinHeight = GetPrivateProfileDouble("DarkDot", "dMinHeight", 1, m_strParamFile);
	m_tParameters.tDarkDotParam.dMinArea = GetPrivateProfileDouble("DarkDot", "dMinArea", 0.8, m_strParamFile);
	m_tParameters.tDarkDotParam.dMaxArea = GetPrivateProfileDouble("DarkDot", "dMaxArea", 36000, m_strParamFile);

	m_tParameters.tDropParam.bUseDetect = GetPrivateProfileInt("Drop", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tDropParam.nMaxThresh = GetPrivateProfileInt("Drop", "nMaxThresh", 50, m_strParamFile);
	m_tParameters.tDropParam.dMinArea = GetPrivateProfileDouble("Drop", "dMinArea", 5, m_strParamFile);
	m_tParameters.tDropParam.nLTMinThresh = GetPrivateProfileInt("Drop", "nLTMinThresh", 120, m_strParamFile);
	m_tParameters.tDropParam.nLTMaxThresh = GetPrivateProfileInt("Drop", "nLTMaxThresh", 180, m_strParamFile);
	m_tParameters.tDropParam.dLTMinArea = GetPrivateProfileDouble("Drop", "dLTMinArea", 15, m_strParamFile);
	m_tParameters.tDropParam.nAT9MaxThresh = GetPrivateProfileInt("Drop", "nAT9MaxThresh", 20, m_strParamFile);
	m_tParameters.tDropParam.dAT9DarkMinWidth = GetPrivateProfileDouble("Drop", "dAT9DarkMinWidth", 0.5, m_strParamFile);
	m_tParameters.tDropParam.dAT9DarkMinArea = GetPrivateProfileDouble("Drop", "dAT9DarkMinArea", 0.5, m_strParamFile);

	m_tParameters.tTapeParam.bUseDetect = GetPrivateProfileInt("Tape", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tTapeParam.nDarkThresh = GetPrivateProfileInt("Tape", "nDarkThresh", 50, m_strParamFile);
	m_tParameters.tTapeParam.dDarkMinArea = GetPrivateProfileDouble("Tape", "dDarkMinArea", 200, m_strParamFile);
	m_tParameters.tTapeParam.nLightThresh = GetPrivateProfileInt("Tape", "nLightThresh", 180, m_strParamFile);
	m_tParameters.tTapeParam.dLightMinArea = GetPrivateProfileDouble("Tape", "dLightMinArea", 200, m_strParamFile);

	m_tParameters.tFoilParam.bUseDetect = GetPrivateProfileInt("Foil", "bUseDetect", FALSE, m_strParamFile);
	m_tParameters.tFoilParam.nMaxDarkThresh = GetPrivateProfileInt("Foil", "nMaxDarkThresh", 50, m_strParamFile);
	m_tParameters.tFoilParam.dMinDarkWidth = GetPrivateProfileDouble("Foil", "dMinDarkWidth", 1, m_strParamFile);
	m_tParameters.tFoilParam.dMinDarkHeight = GetPrivateProfileDouble("Foil", "dMinDarkHeight", 1, m_strParamFile);
	m_tParameters.tFoilParam.dMinDarkArea = GetPrivateProfileDouble("Foil", "dMinDarkArea", 0.8, m_strParamFile);
	m_tParameters.tFoilParam.nMaxDarkNum = GetPrivateProfileInt("Foil", "nMaxDarkNum", 0, m_strParamFile);
	m_tParameters.tFoilParam.nLightThresh = GetPrivateProfileInt("Foil", "nLightThresh", 120, m_strParamFile);
	m_tParameters.tFoilParam.dMinLightArea = GetPrivateProfileDouble("Foil", "dMinLightArea", 1, m_strParamFile);
	m_tParameters.tFoilParam.dFoilLightMinWidth = GetPrivateProfileDouble("Foil", "dFoilLightMinWidth", 3, m_strParamFile);

	/////////////////缺陷名读取//////////
	CString strCommonFile;
	strCommonFile.Format("%s//Config//Common.dh", GetCurrentAppPath());
	m_hvErrorName.Clear();
	for (int i = 0; i < NG_Count; i++)
	{
		CString strKey;
		strKey.Format("Error%dName", i);
		m_hvErrorName.Append((HTuple)GetPrivateProfileCString("ErrorInfo", strKey, "OtherDefect", strCommonFile));
	}

	//////////////////图像特征参数读取
	m_hvStrFeature.Clear();
	for (int i = 0; i < FEATURE_COUNT; i++)
	{
		m_hvStrFeature.Append((HTuple)NAME_Feature[i]);
	}

	/////////////////自定义参数//////////
	m_tParameters.tFeatureSelect.nGroupNum = GetPrivateProfileInt("Feature", "FeatureGroupNum", 0, m_strParamFile);
	int nGroupNums = m_tParameters.tFeatureSelect.nGroupNum;//一共多少组自定义的缺陷类型
	CString strAppName, strValue;
	CString strSubKey;
	HTuple hv_SingleFeature;
	for (int i = 0; i < nGroupNums; i++)
	{
		strAppName.Format("FeatureGroup%d", i);
		SingleErrorGroup tSingleErrorGroup;
		tSingleErrorGroup.nSingleGroupNum = GetPrivateProfileInt(strAppName, "SingleGroupNum", 0, m_strParamFile);
		tSingleErrorGroup.strErrorName = GetPrivateProfileCString(strAppName, "缺陷组名", "面积", m_strParamFile);
		for (int j = 0; j < tSingleErrorGroup.nSingleGroupNum; j = j + 3)
		{
			SingleFeature tSingleFeature;
			strSubKey.Format("特征%d", j);
			tSingleFeature.StrSingleFeatureName = GetPrivateProfileCString(strAppName, strSubKey, "面积", m_strParamFile);
			strSubKey.Format("MIN%d", j);
			tSingleFeature.dMin = GetPrivateProfileDouble(strAppName, strSubKey, 0, m_strParamFile);
			strSubKey.Format("MAX%d", j);
			tSingleFeature.dMax = GetPrivateProfileDouble(strAppName, strSubKey, 9999, m_strParamFile);
			tSingleErrorGroup.Vec_tSingleGroupFeatures.push_back(tSingleFeature);
		}
		m_tParameters.tFeatureSelect.Vec_FSelects.push_back(tSingleErrorGroup);
	}

	//生成屏蔽区域,双相机用,单相机无屏蔽区域
	try
	{
		GenEmptyObj(&m_hoOverRegion);
		if (m_tInit.hvCamSNList.Length() == 2 && g_pAlgorithm->m_Calibration.GetStripeParam().hvColumnOver.Length() == 2)
		{
			HTuple hv = g_pAlgorithm->m_Calibration.GetStripeParam().hvColumnOver.TupleMin();
			HTuple hv2 = g_pAlgorithm->m_Calibration.GetStripeParam().hvColumnOver.TupleMax();
			GenRectangle1(&m_hoOverRegion, 0, g_pAlgorithm->m_Calibration.GetStripeParam().hvColumnOver.TupleMin(),
				9999 - 1, g_pAlgorithm->m_Calibration.GetStripeParam().hvColumnOver.TupleMax());
		}
	}
	catch (HException&)
	{

	}

	CString strFilePath;
	strFilePath.Format("%s//Config//System.vis", GetCurrentAppPath());
	m_nMaxDetectTime = GetPrivateProfileInt("Algorithm", "MaxDetectTime", 150, strFilePath);

	return TRUE;
}

BOOL CDefect::WriteParam()
{
	WritePrivateProfileDouble("tPretreatParam", "dBorderWidth", m_tParameters.tPretreatParam.dBorderWidth, m_strParamFile);
	WritePrivateProfileDouble("tPretreatParam", "dScaleFactor", m_tParameters.tPretreatParam.dScaleFactor, m_strParamFile);

	WritePrivateProfileDouble("tPretreatParam", "dMidLightCof", m_tParameters.tPretreatParam.dMidLightCof, m_strParamFile);
	WritePrivateProfileDouble("tPretreatParam", "dMidDarkCof", m_tParameters.tPretreatParam.dMidDarkCof, m_strParamFile);
	WritePrivateProfileDouble("tPretreatParam", "dBorderLightCof", m_tParameters.tPretreatParam.dBorderLightCof, m_strParamFile);
	WritePrivateProfileDouble("tPretreatParam", "dBorderDarkCof", m_tParameters.tPretreatParam.dBorderDarkCof, m_strParamFile);
	WritePrivateProfileDouble("tPretreatParam", "dZoomFactor", m_tParameters.tPretreatParam.dZoomFactor, m_strParamFile);

	WritePrivateProfileInt("LeakFoil", "bUseDetect", m_tParameters.tLeakFoilParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("LeakFoil", "nThresh", m_tParameters.tLeakFoilParam.nThresh, m_strParamFile);    
    WritePrivateProfileDouble("LeakFoil", "dMinArea", m_tParameters.tLeakFoilParam.dMinArea, m_strParamFile);
	WritePrivateProfileInt("LeakFoil", "nDynThresh", m_tParameters.tLeakFoilParam.nDynThresh, m_strParamFile);
	WritePrivateProfileDouble("LeakFoil", "dMinDiameter", m_tParameters.tLeakFoilParam.dMinDiameter, m_strParamFile);
	WritePrivateProfileDouble("LeakFoil", "dMaxDiameter", m_tParameters.tLeakFoilParam.dMaxDiameter, m_strParamFile);
	WritePrivateProfileDouble("LeakFoil", "dDeviation", m_tParameters.tLeakFoilParam.dDeviation, m_strParamFile);

	WritePrivateProfileInt("AT11LeakFoil", "bUseDetect", m_tParameters.tAT11LeakFoilParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("AT11LeakFoil", "nMinThresh", m_tParameters.tAT11LeakFoilParam.nMinThresh, m_strParamFile);
	WritePrivateProfileDouble("AT11LeakFoil", "dMinWidth", m_tParameters.tAT11LeakFoilParam.dMinWidth, m_strParamFile);
	WritePrivateProfileInt("AT11LeakFoil", "dMinHeight", m_tParameters.tAT11LeakFoilParam.dMinHeight, m_strParamFile);

	WritePrivateProfileInt("Bubble", "bUseDetect", m_tParameters.tBubbleParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("Bubble", "nDynThresh", m_tParameters.tBubbleParam.nDynThresh, m_strParamFile);
	WritePrivateProfileDouble("Bubble", "dMinDiameter", m_tParameters.tBubbleParam.dMinDiameter, m_strParamFile);
	WritePrivateProfileDouble("Bubble", "dMinCircularity", m_tParameters.tBubbleParam.dMinCircularity, m_strParamFile);
	WritePrivateProfileInt("Bubble", "nInOutGrayDiff", m_tParameters.tBubbleParam.nInOutGrayDiff, m_strParamFile);

	//WritePrivateProfileInt("Bubble", "bUseDetect", m_tParameters.tLightLineParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("LightLine", "bUseDetect", m_tParameters.tLightLineParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("LightLine", "nDynThresh", m_tParameters.tLightLineParam.nDynThresh, m_strParamFile);
	WritePrivateProfileDouble("LightLine", "dMinWidth", m_tParameters.tLightLineParam.dMinWidth, m_strParamFile);
	WritePrivateProfileDouble("LightLine", "dMinHeight", m_tParameters.tLightLineParam.dMinHeight, m_strParamFile);
	WritePrivateProfileDouble("LightLine", "dMinLengthWidthRatio", m_tParameters.tLightLineParam.dMinLengthWidthRatio, m_strParamFile);

	WritePrivateProfileInt("DarkLine", "bUseDetect", m_tParameters.tDarkLineParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("DarkLine", "nDynThresh", m_tParameters.tDarkLineParam.nDynThresh, m_strParamFile);
	WritePrivateProfileDouble("DarkLine", "dMinWidth", m_tParameters.tDarkLineParam.dMinWidth, m_strParamFile);
	WritePrivateProfileDouble("DarkLine", "dMinHeight", m_tParameters.tDarkLineParam.dMinHeight, m_strParamFile);
	WritePrivateProfileDouble("DarkLine", "dMinLengthWidthRatio", m_tParameters.tDarkLineParam.dMinLengthWidthRatio, m_strParamFile);

	WritePrivateProfileInt("LightDot", "bUseDetect", m_tParameters.tLightDotParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("LightDot", "nMinThresh", m_tParameters.tLightDotParam.nMinThresh, m_strParamFile);
	WritePrivateProfileInt("LightDot", "nMaxThresh", m_tParameters.tLightDotParam.nMaxThresh, m_strParamFile);
	WritePrivateProfileDouble("LightDot", "dMinWidth", m_tParameters.tLightDotParam.dMinWidth, m_strParamFile);
	WritePrivateProfileDouble("LightDot", "dMinHeight", m_tParameters.tLightDotParam.dMinHeight, m_strParamFile);
	WritePrivateProfileDouble("LightDot", "dMinArea", m_tParameters.tLightDotParam.dMinArea, m_strParamFile);
	WritePrivateProfileDouble("LightDot", "dMaxArea", m_tParameters.tLightDotParam.dMaxArea, m_strParamFile);
	WritePrivateProfileInt("LightDot", "nMinDeviation", m_tParameters.tLightDotParam.nMinDeviation, m_strParamFile);
	WritePrivateProfileInt("LightDot", "nMinGrayRange", m_tParameters.tLightDotParam.nMinGrayRange, m_strParamFile);

	WritePrivateProfileInt("DarkDot", "bUseDetect", m_tParameters.tDarkDotParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("DarkDot", "nMinThresh", m_tParameters.tDarkDotParam.nMaxThresh, m_strParamFile);
	WritePrivateProfileDouble("DarkDot", "dMinWidth", m_tParameters.tDarkDotParam.dMinWidth, m_strParamFile);
	WritePrivateProfileDouble("DarkDot", "dMinHeight", m_tParameters.tDarkDotParam.dMinHeight, m_strParamFile);
	WritePrivateProfileDouble("DarkDot", "dMinArea", m_tParameters.tDarkDotParam.dMinArea, m_strParamFile);
	WritePrivateProfileDouble("DarkDot", "dMaxArea", m_tParameters.tDarkDotParam.dMaxArea, m_strParamFile);

	WritePrivateProfileInt("Drop", "bUseDetect", m_tParameters.tDropParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("Drop", "nMaxThresh", m_tParameters.tDropParam.nMaxThresh, m_strParamFile);
	WritePrivateProfileDouble("Drop", "dMinArea", m_tParameters.tDropParam.dMinArea, m_strParamFile);
	WritePrivateProfileInt("Drop", "nLTMinThresh", m_tParameters.tDropParam.nLTMinThresh, m_strParamFile);
	WritePrivateProfileInt("Drop", "nLTMaxThresh", m_tParameters.tDropParam.nLTMaxThresh, m_strParamFile);
	WritePrivateProfileDouble("Drop", "dLTMinArea", m_tParameters.tDropParam.dLTMinArea, m_strParamFile);
	WritePrivateProfileInt("Drop", "nAT9MaxThresh", m_tParameters.tDropParam.nAT9MaxThresh, m_strParamFile);
	WritePrivateProfileDouble("Drop", "dAT9DarkMinWidth", m_tParameters.tDropParam.dAT9DarkMinWidth, m_strParamFile);
	WritePrivateProfileDouble("Drop", "dAT9DarkMinArea", m_tParameters.tDropParam.dAT9DarkMinArea, m_strParamFile);

	WritePrivateProfileInt("Tape", "bUseDetect", m_tParameters.tTapeParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("Tape", "nDarkThresh", m_tParameters.tTapeParam.nDarkThresh, m_strParamFile);
	WritePrivateProfileDouble("Tape", "dDarkMinArea", m_tParameters.tTapeParam.dDarkMinArea, m_strParamFile);
	WritePrivateProfileInt("Tape", "nLightThresh", m_tParameters.tTapeParam.nLightThresh, m_strParamFile);
	WritePrivateProfileDouble("Tape", "dLightMinArea", m_tParameters.tTapeParam.dLightMinArea, m_strParamFile);

	WritePrivateProfileInt("Foil", "bUseDetect", m_tParameters.tFoilParam.bUseDetect, m_strParamFile);
	WritePrivateProfileInt("Foil", "nMaxDarkThresh", m_tParameters.tFoilParam.nMaxDarkThresh, m_strParamFile);
	WritePrivateProfileDouble("Foil", "dMinDarkWidth", m_tParameters.tFoilParam.dMinDarkWidth, m_strParamFile);
	WritePrivateProfileDouble("Foil", "dMinDarkHeight", m_tParameters.tFoilParam.dMinDarkHeight, m_strParamFile);
	WritePrivateProfileDouble("Foil", "dMinDarkArea", m_tParameters.tFoilParam.dMinDarkArea, m_strParamFile);
	WritePrivateProfileInt("Foil", "nMaxDarkNum", m_tParameters.tFoilParam.nMaxDarkNum, m_strParamFile);
	WritePrivateProfileInt("Foil", "nLightThresh", m_tParameters.tFoilParam.nLightThresh, m_strParamFile);
	WritePrivateProfileDouble("Foil", "dMinLightArea", m_tParameters.tFoilParam.dMinLightArea, m_strParamFile);
	WritePrivateProfileDouble("Foil", "dFoilLightMinWidth", m_tParameters.tFoilParam.dFoilLightMinWidth, m_strParamFile);


	////////存储自定义参数
	WritePrivateProfileInt("Feature", "FeatureGroupNum", m_tParameters.tFeatureSelect.nGroupNum, m_strParamFile);
	CString strAppName;
	CString strSubKey;
	HTuple hv_SingleFeature;
	for (int i = 0; i < m_tParameters.tFeatureSelect.nGroupNum; i++)
	{
		strAppName.Format("FeatureGroup%d", i);
		WritePrivateProfileInt(strAppName, "SingleGroupNum", m_tParameters.tFeatureSelect.Vec_FSelects.at(i).nSingleGroupNum, m_strParamFile);

		SingleErrorGroup tSingleErrorGroup;
		tSingleErrorGroup = m_tParameters.tFeatureSelect.Vec_FSelects.at(i);
		WritePrivateProfileInt(strAppName, "SingleGroupNum", tSingleErrorGroup.nSingleGroupNum, m_strParamFile);
		WritePrivateProfileString(strAppName, "缺陷组名", tSingleErrorGroup.strErrorName, m_strParamFile);
		for (int j = 0; j < tSingleErrorGroup.nSingleGroupNum; j = j + 3)
		{
			SingleFeature tSingleFeature;
			tSingleFeature = tSingleErrorGroup.Vec_tSingleGroupFeatures.at(j / 3);
			strSubKey.Format("特征%d", j);
			WritePrivateProfileString(strAppName, strSubKey, tSingleFeature.StrSingleFeatureName, m_strParamFile);
			strSubKey.Format("MIN%d", j);
			WritePrivateProfileDouble(strAppName, strSubKey, tSingleFeature.dMin, m_strParamFile);
			strSubKey.Format("MAX%d", j);
			WritePrivateProfileDouble(strAppName, strSubKey, tSingleFeature.dMax, m_strParamFile);
		}
	}
	return TRUE;
}

void CDefect::SetResolution(ResolutionList vecRes)
{
	m_vecResolution = vecRes;
	double dVal = 0;
	for (int i = 0; i < vecRes.size(); i++)
	{
		dVal += vecRes[i].dColRes;
	}
	m_dColResolution = dVal / vecRes.size();
	m_dRowResolution = vecRes[0].dRowRes;
}

void CDefect::InitOutputValue()
{
	//初始化输出变量
	GenEmptyObj(&m_ptOut->hoDisp);
	m_ptOut->hv_Type = HTuple();
	m_ptOut->hv_StripeSN = HTuple();
	m_ptOut->tFeature.tCenter.hv_Row = HTuple();
	m_ptOut->tFeature.tCenter.hv_Column = HTuple();
	m_ptOut->tFeature.hvArea = HTuple();
	m_ptOut->tFeature.hvWidth = HTuple();
	m_ptOut->tFeature.hvHeight = HTuple();
	m_ptOut->tFeature.hvDiameter = HTuple();

	m_ptOut->tFeature.tRect.tLeftTop.hv_Column = HTuple();
	m_ptOut->tFeature.tRect.tLeftTop.hv_Row = HTuple();
	m_ptOut->tFeature.tRect.tRightBottom.hv_Column = HTuple();
	m_ptOut->tFeature.tRect.tRightBottom.hv_Row = HTuple();

	GenEmptyObj(&m_hoBoderRegion);
	GenEmptyObj(&m_hoCoatMid);
	GenEmptyObj(&m_hoFoilRegion);
	GenEmptyObj(&m_hoAT11BaseCoatRegion);
	GenEmptyObj(&m_hoSegRegions);
	GenEmptyObj(&m_hoLEnableChoiceRegions);
	GenEmptyObj(&m_hoDEnableChoiceRegions);
}

void CDefect::Preprocess()
{
	//灰度拉伸
	if (m_tParameters.tPretreatParam.dScaleFactor != 1)
	{
		ScaleImage(m_ptIn->hoImage, &m_hoAdaptedImage, m_tParameters.tPretreatParam.dScaleFactor, 0);
	}
	else
	{
		m_hoAdaptedImage = m_ptIn->hoImage;
	}

	double dZoomFactor = max(0.1, min(m_tParameters.tPretreatParam.dZoomFactor, 1));
	try
	{
		if (1 != dZoomFactor)
		{
			ZoomImageFactor(m_hoAdaptedImage, &m_hoZoomImage, dZoomFactor, dZoomFactor, "constant");
		}
		else
		{
			m_hoZoomImage = m_hoAdaptedImage;
		}
	}
	catch (const std::exception&)
	{

	}

	HObject hoRegion, hoTmp;
	//涂布区域去除边界
	try
	{
		if (m_ptIn->tCoat.ho_Region.CountObj() > 0)
		{
			HObject hoCoatUnion;
			Union1(m_ptIn->tCoat.ho_Region, &hoCoatUnion);
			if (m_tParameters.tPretreatParam.dBorderWidth > 0)
				ErosionRectangle1(hoCoatUnion, &hoCoatUnion, (int)(m_tParameters.tPretreatParam.dBorderWidth / m_dColResolution * 2), 1);
			Difference(m_ptIn->tCoat.ho_Region, hoCoatUnion, &hoTmp);
			ConcatObj(m_hoBoderRegion, hoTmp, &m_hoBoderRegion);
			Difference(hoCoatUnion, m_hoOverRegion, &m_hoCoatMid);

			ZoomRegion(m_hoCoatMid, &m_hoZoomCoatMid, dZoomFactor, dZoomFactor);
		}
	}
	catch (HException&)
	{
	}
	//极耳区域去除边界
	try
	{
		if (m_ptIn->tFoil.ho_Region.CountObj() > 0)
		{
			//ErosionRectangle1(m_ptIn->tFoil.ho_Region, &m_hoFoilRegion, m_tParameters.tPretreatParam.dBorderWidth / m_vecResolution[0].dColRes * 2, 1);
			Difference(m_ptIn->tFoil.ho_Region, m_hoOverRegion, &m_hoFoilRegion);
			Union1(m_hoFoilRegion, &m_hoFoilRegion);
			//ConcatObj(m_hoBoderRegion, hoTmp, &m_hoBoderRegion);
		}
	}
	catch (HException&) 
	{
	}
	//AT11区域，凹版区域去除边界
	try
	{
		if (m_tInit.tProductInfo.bExistAT11 && m_ptIn->tAT11.ho_Region.CountObj() > 0)
		{
			Difference(m_ptIn->tAT11.ho_Region, m_hoOverRegion, &m_hoAT11BaseCoatRegion);
			Union1(m_hoAT11BaseCoatRegion, &m_hoAT11BaseCoatRegion);
			/*ErosionRectangle1(m_ptIn->tAT11.ho_Region, &hoRegion, m_tParameters.tPretreatParam.dBorderWidth / m_vecResolution[0].dColRes * 2, 1);
			Difference(m_ptIn->tAT11.ho_Region, hoRegion, &hoTmp);
			ConcatObj(m_hoBoderRegion, hoTmp, &m_hoBoderRegion);
			m_ptIn->tAT11.ho_Region = hoRegion;*/
		}
		else if (m_tInit.tProductInfo.bExistBaseCoat && m_ptIn->tBaseCoat.ho_Region.CountObj() > 0)
		{
			Difference(m_ptIn->tBaseCoat.ho_Region, m_hoOverRegion, &m_hoAT11BaseCoatRegion);
			Union1(m_hoAT11BaseCoatRegion, &m_hoAT11BaseCoatRegion);
			/*ErosionRectangle1(m_ptIn->tBaseCoat.ho_Region, &hoRegion, m_tParameters.tPretreatParam.dBorderWidth / m_vecResolution[0].dColRes * 2, 1);
			Difference(m_ptIn->tBaseCoat.ho_Region, hoRegion, &hoTmp);
			ConcatObj(m_hoBoderRegion, hoTmp, &m_hoBoderRegion);
			m_ptIn->tBaseCoat.ho_Region = hoRegion;*/
		}
	}
	catch (HException&) 
	{
	}
	
}

void CDefect::Action(tagDefectInput& tIn, tagDefectOutput& tOut)
{
	m_ptIn = &tIn;
	m_ptOut = &tOut;

	//初始化输出值和中间变量
	InitOutputValue();

	if (tIn.nMode == GRRMode) return;

	GetImageSize(tIn.hoImage, &m_hvImageWidth, &m_hvImageHeight);

	//初始化事件
	g_evtComplete.ResetEvent();
	g_evtStart.ResetEvent();
	g_evtStart.SetEvent();

	try
	{
		m_bDetectEnd = FALSE;
		if (WAIT_TIMEOUT == WaitForMultipleObjects(1, &g_evtComplete.m_hObject, TRUE, m_nMaxDetectTime)) //等待线程结束 //超时报警 
		{
			m_bDetectEnd = TRUE;
			CString strLog;
			strLog.Format("CDefect_Action: 算法%d超时,提前退出", m_tInit.nSN);
			m_tInit.pLog(strLog, DebugLog, NoExcp);
			return;
		}
	}
	catch (HException& e)
	{

	}
	return;



	//预处理
	Preprocess();

	try
	{
		//缺陷检测
		GetDynRegion();
		//自定义筛选瑕疵
		DIYErrorOut();
		if (HasDefect()) return;
		//漏箔
		LeakFoilDetect();
		if (HasDefect()) return;
		//胶带检测
		TapeDetect();
		if (HasDefect()) return;
		//亮条痕
		LightLineDetect();
		if (HasDefect()) return;
		//暗条痕
		BlackLineDetect();
		if (HasDefect()) return;
		//气泡
		BubbleDetect();
		if (HasDefect()) return;
		//极耳带料
		FoilDetect();
		if (HasDefect()) return;
		//亮点
		LightDotDetect();
		if (HasDefect()) return;
		//黑点
		BlackDotDetect();
		if (HasDefect()) return;
		//脱碳
		//DropDetect();
		//if (HasDefect()) return;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CDefect_Action: 算法%d异常,%s(%s)", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strException, ExcpLog, SlightExcp);
	}
}

void CDefect::Detect()
{
	//线程内检测的函数，需要保证使用的成员变量不能同时修改
	//预处理
	Preprocess();

	try
	{
		//缺陷检测
		GetDynRegion();
		//自定义筛选瑕疵
		DIYErrorOut();
		if (HasDefect()) return;
		//漏箔
		LeakFoilDetect();
		if (HasDefect()) return;
		//胶带检测
		TapeDetect();
		if (HasDefect()) return;
		//亮条痕
		LightLineDetect();
		if (HasDefect()) return;
		//暗条痕
		BlackLineDetect();
		if (HasDefect()) return;
		//气泡
		BubbleDetect();
		if (HasDefect()) return;
		//极耳带料
		FoilDetect();
		if (HasDefect()) return;
		//亮点
		LightDotDetect();
		if (HasDefect()) return;
		//黑点
		BlackDotDetect();
		if (HasDefect()) return;
		//脱碳
		//DropDetect();
		//if (HasDefect()) return;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CDefect_Action: 算法%d异常,%s(%s)", m_tInit.nSN, e.ErrorMessage().Text(), e.ProcName().Text());
		m_tInit.pLog(strException, ExcpLog, SlightExcp);
	}
}

CDefect& CDefect::operator= (const CDefect& in)
{
	m_tInit = in.m_tInit;
	m_tParameters = in.m_tParameters;
	m_nMaxDetectTime = in.m_nMaxDetectTime;
	m_dColResolution = in.m_dColResolution;
	m_dRowResolution = in.m_dRowResolution;
	m_hvStrFeature = in.m_hvStrFeature;
	m_vecResolution = in.m_vecResolution;
	m_hvErrorName = in.m_hvErrorName;
	m_hoOverRegion = in.m_hoOverRegion;

	return *this;
}

void CDefect::DIYErrorOut()
{
	try
	{
		CString strAppName, strValue;
		CString strSubKey;
		HTuple hv_SingleFeature;

		for (int i = 0; i < m_tParameters.tFeatureSelect.nGroupNum; i++)
		{
			BOOL bGrayFeature = FALSE;
			BOOL bShapeFearture = FALSE;
			HTuple hv_FeatureName, hv_FeatureMin, hv_FeatureMax;
			HTuple hv_ShapeName, hv_ShapeMin, hv_ShapeMax;
			HTuple hv_Index;
			SingleErrorGroup tSingleErrorGroup;
			tSingleErrorGroup = m_tParameters.tFeatureSelect.Vec_FSelects.at(i);//获取单组的缺陷的筛选项目
			tSingleErrorGroup.strErrorName;//缺陷类型
			TupleFind(m_hvErrorName, (HTuple)tSingleErrorGroup.strErrorName, &hv_Index);
			int n = hv_Index.I();
			if (hv_Index != -1 && hv_Index.Length() > 0)
			{
				for (int j = 0; j < tSingleErrorGroup.nSingleGroupNum; j = j + 3)
				{
					SingleFeature tSingleFeature = tSingleErrorGroup.Vec_tSingleGroupFeatures.at(j / 3);

					HTuple hv_FeatureIndex;
					TupleFind(m_hvStrFeature, (HTuple)tSingleFeature.StrSingleFeatureName, &hv_FeatureIndex);
					if (G_min == hv_FeatureIndex || G_max == hv_FeatureIndex || G_mean == hv_FeatureIndex || G_deviation == hv_FeatureIndex)
					{
						bGrayFeature = TRUE;
						hv_FeatureName.Append((HTuple)STR_Feature[hv_FeatureIndex.I()]);
						hv_FeatureMin.Append(tSingleFeature.dMin);
						hv_FeatureMax.Append(tSingleFeature.dMax);
					}
					else
					{
						bShapeFearture = TRUE;
						hv_ShapeName.Append((HTuple)STR_Feature[hv_FeatureIndex.I()]);
						hv_ShapeMin.Append(tSingleFeature.dMin);
						hv_ShapeMax.Append(tSingleFeature.dMax);
					}
				}
				HObject hoLightRegion, hoDarkRegion;
				HObject hoSelectRegion;
				hoSelectRegion.GenEmptyObj();

				GetChoiceRegions(hoLightRegion, hoDarkRegion);
				ConcatObj(hoSelectRegion, hoLightRegion, &hoSelectRegion);
				ConcatObj(hoSelectRegion, hoDarkRegion, &hoSelectRegion);
				if (bGrayFeature == TRUE)
				{
					SelectGray(hoSelectRegion, m_hoAdaptedImage, &hoSelectRegion, hv_FeatureName, "and", hv_FeatureMin, hv_FeatureMax);
				}
				if (bShapeFearture == TRUE)
				{
					SelectShape(hoSelectRegion, &hoSelectRegion, hv_ShapeName, "and", hv_ShapeMin, hv_ShapeMax);
				}
				AddError(hoSelectRegion, (e_ErrorType)hv_Index.I());
			}
		}
	}
	catch (HException& e)
	{
	}
}

BOOL CDefect::AddError(HObject ho_ErrorObject, e_ErrorType eErrorType, eErrorRegionType eType)
{
	if (eType == RT_AT11 && !m_tInit.tProductInfo.bExistAT11) return FALSE;
	if (eType == RT_BaseCoat && !m_tInit.tProductInfo.bExistBaseCoat) return FALSE;

	SingleSize tRegion;
	switch (eType)
	{
	case RT_Coat://涂布
		tRegion = m_ptIn->tCoat;
		break;
	case RT_Foil://极耳
		tRegion = m_ptIn->tFoil;
		break;
	case RT_AT11://AT9
		tRegion = m_ptIn->tAT11;
		break;
	case RT_BaseCoat://凹版
		tRegion = m_ptIn->tBaseCoat;
		break;
	default:
		break;
	}

	if (tRegion.ho_Region.CountObj() == 0)
	{
		CString strExcp;
		strExcp.Format("CDefect%d_AddError: 缺陷所在区域为空，请检查", m_tInit.nSN);
		m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}

	HObject ho_SingleRegion, ho_InterErrorRegion;
	for (int i = 0; i < tRegion.ho_Region.CountObj(); i++)
	{
		SelectObj(tRegion.ho_Region, &ho_SingleRegion, i + 1);
		//缺陷区域和每个单个大区域交集
		Intersection(ho_SingleRegion, ho_ErrorObject, &ho_InterErrorRegion);
		Connection(ho_InterErrorRegion, &ho_InterErrorRegion);

		if(ho_InterErrorRegion.CountObj() == 0) continue;

		//暂时不限制最大输出缺陷数量 2021.4.21 限制最大数量最大输出8个
		if (m_ptOut->hv_Type.Length() > 8) break;

		try
		{
			HTuple hv_StripeSN, hv_ErrorType, hv_CenterRow, hv_CenterCol, hvArea, hvWidth, hvHeight, hvDiameter, hvRow1, hvRow2, hvColumn1, hvColumn2;
			TupleGenConst(ho_InterErrorRegion.CountObj(), tRegion.hv_StripeSN[i], &hv_StripeSN);
			TupleGenConst(ho_InterErrorRegion.CountObj(), eErrorType, &hv_ErrorType);
			AreaCenter(ho_InterErrorRegion, &hvArea, &hv_CenterRow, &hv_CenterCol);
			RegionFeatures(ho_InterErrorRegion, "width", &hvWidth);
			RegionFeatures(ho_InterErrorRegion, "height", &hvHeight);
			RegionFeatures(ho_InterErrorRegion, "max_diameter", &hvDiameter);
			SmallestRectangle1(ho_InterErrorRegion, &hvRow1, &hvColumn1, &hvRow2, &hvColumn2);

			//for (int i = 0; i < hv_StripeSN.Length(); i++)
			//{
			//	if (i > 7) break;
			//	m_ptOut->hv_StripeSN.Append(hv_StripeSN[i]);
			//	m_ptOut->hv_Type.Append(hv_ErrorType[i]);
			//	ConcatObj(m_ptOut->hoDisp, ho_InterErrorRegion.SelectObj(i+1), &m_ptOut->hoDisp);
			//	m_ptOut->tFeature.hvArea.Append(hvArea[i] * m_dColResolution * m_dRowResolution);
			//	m_ptOut->tFeature.hvDiameter.Append(hvDiameter[i] * m_dColResolution);
			//	m_ptOut->tFeature.hvWidth.Append(hvWidth[i] * m_dColResolution);
			//	m_ptOut->tFeature.hvHeight.Append(hvHeight[i] * m_dRowResolution);
			//	m_ptOut->tFeature.tCenter.hv_Row.Append(hv_CenterRow[i] * m_dRowResolution);
			//	m_ptOut->tFeature.tRect.tLeftTop.hv_Row.Append(hvRow1[i] * m_dRowResolution);
			//	m_ptOut->tFeature.tRect.tRightBottom.hv_Row.Append(hvRow2[i] * m_dRowResolution);

			//	//这个地方放像素坐标，接口外面计算成物理坐标
			//	m_ptOut->tFeature.tCenter.hv_Column.Append(hv_CenterCol[i]);
			//	m_ptOut->tFeature.tRect.tLeftTop.hv_Column.Append(hvColumn1[i]);
			//	m_ptOut->tFeature.tRect.tRightBottom.hv_Column.Append(hvColumn2[i]);
			//}

			m_ptOut->hv_StripeSN.Append(hv_StripeSN);
			m_ptOut->hv_Type.Append(hv_ErrorType);
			ConcatObj(m_ptOut->hoDisp, ho_InterErrorRegion, &m_ptOut->hoDisp);
			m_ptOut->tFeature.hvArea.Append(hvArea * m_dColResolution * m_dRowResolution);
			m_ptOut->tFeature.hvDiameter.Append(hvDiameter * m_dColResolution);
			m_ptOut->tFeature.hvWidth.Append(hvWidth * m_dColResolution);
			m_ptOut->tFeature.hvHeight.Append(hvHeight * m_dRowResolution);
			m_ptOut->tFeature.tCenter.hv_Row.Append(hv_CenterRow * m_dRowResolution);
			m_ptOut->tFeature.tRect.tLeftTop.hv_Row.Append(hvRow1 * m_dRowResolution);
			m_ptOut->tFeature.tRect.tRightBottom.hv_Row.Append(hvRow2 * m_dRowResolution);

			//这个地方放像素坐标，接口外面计算成物理坐标
			m_ptOut->tFeature.tCenter.hv_Column.Append(hv_CenterCol);
			m_ptOut->tFeature.tRect.tLeftTop.hv_Column.Append(hvColumn1);
			m_ptOut->tFeature.tRect.tRightBottom.hv_Column.Append(hvColumn2);
		}
		catch (HException&)
		{

		}
	}

	return TRUE;
}

void CDefect::LeakFoilDetect()
{
	try
	{
		// Local iconic variables
		HObject  ho_LeakFoilRegion, ho_ImageReduced, ho_RegionDynThresh;
		HObject  ho_RegionClosing, ho_RegionFillUp, ho_ConnectedRegions;
		HObject  ho_RegionTrans, ho_SelectedRegions, ho_LeakFoilError1;
		HObject  ho_Region, ho_RegionClosing1, ho_ConnectedRegions1;
		HObject  ho_RegionTrans1, ho_LeakFoilError2;

		// Local control variables
		HTuple  hv_bDetect, hv_dResolution, hv_dDynThresh;
		HTuple  hv_dLeakFoilThresh, hv_dMinDiameter, hv_dMaxDiameter;
		HTuple  hv_dMinDeviation, hv_Concat, hv_Max, hv_Min, hv_LeakFoilMinArea;

		HObject ho_ErrorRegion;
		ho_ErrorRegion.GenEmptyObj();
		if (FALSE == m_tParameters.tLeakFoilParam.bUseDetect)
		{
			return;
		}
		hv_dLeakFoilThresh = m_tParameters.tLeakFoilParam.nThresh;
		hv_LeakFoilMinArea = m_tParameters.tLeakFoilParam.dMinArea / m_dColResolution / m_dRowResolution;
		hv_dDynThresh = m_tParameters.tLeakFoilParam.nDynThresh;

		hv_dMinDiameter = m_tParameters.tLeakFoilParam.dMinDiameter / m_dColResolution;
		hv_dMaxDiameter = m_tParameters.tLeakFoilParam.dMaxDiameter / m_dColResolution;
		hv_dMinDeviation = m_tParameters.tLeakFoilParam.dDeviation;
		//////检测大缺陷
		HObject hoRegion, ho_LeakFoilError0, hoCoat;
		ReduceDomain(m_hoAdaptedImage, m_hoCoatMid, &ho_ImageReduced);
		FastThreshold(ho_ImageReduced, &hoRegion, MIN(hv_dLeakFoilThresh, 255), 255, 10);
		Difference(hoRegion, m_ptOut->hoDisp, &hoRegion);
		Connection(hoRegion, &hoRegion);
		SelectShape(hoRegion, &ho_ErrorRegion, "area", "and", hv_LeakFoilMinArea, INT_MAX);

		if (ho_ErrorRegion.CountObj() > 0)
		{
			AddError(ho_ErrorRegion, NG_ExposeFoil, RT_Coat);
			ho_ErrorRegion.GenEmptyObj();
		}

		/////AT9检测 检测漏金属需要向涂布区域扩充一下,使用灰度变换前的图像
		if (m_tInit.tProductInfo.bExistAT11 && m_hoAT11BaseCoatRegion.CountObj() > 0)
		{
			HTuple hv_AT9Thresh, hv_MinWidth, hv_MinHeight;
			HObject hoAT9Region;
			hv_AT9Thresh = m_tParameters.tAT11LeakFoilParam.nMinThresh;
			hv_MinWidth = m_tParameters.tAT11LeakFoilParam.dMinWidth / m_dRowResolution;
			hv_MinHeight = m_tParameters.tAT11LeakFoilParam.dMinHeight / m_dRowResolution;
			//Connection(m_hoAT11BaseCoatRegion, &hoAT9Region);
			ErosionRectangle1(m_hoAT11BaseCoatRegion, &hoAT9Region, 5, 1);
			HTuple hv_LeftTopRow, hv_LeftColumn, hv_RightTopRow, hv_RightColumn;
			SmallestRectangle1(hoAT9Region, &hv_LeftTopRow, &hv_LeftColumn, &hv_RightTopRow, &hv_RightColumn);
			ReduceDomain(m_ptIn->hoImage, hoAT9Region, &ho_ImageReduced);
			Threshold(ho_ImageReduced, &hoRegion, MIN(hv_AT9Thresh, 255), 255);
			Connection(hoRegion, &hoRegion);
			SelectShape(hoRegion, &ho_LeakFoilError0, HTuple("height").Append("width"), "and", hv_MinHeight.Append(hv_MinWidth), HTuple(INT_MAX).Append(INT_MAX));
			ConcatObj(ho_ErrorRegion, ho_LeakFoilError0, &ho_ErrorRegion);
			if (ho_ErrorRegion.CountObj() > 0)
			{
				AddError(ho_ErrorRegion, NG_ExposeFoil, RT_AT11);
				ho_ErrorRegion.GenEmptyObj();
			}

		}
		HObject hoChoiceRegions;
		HObject hoLChoiceRegions, hoDChoiceRegions;
		GetChoiceRegions(hoLChoiceRegions, hoDChoiceRegions);
		hoChoiceRegions = hoLChoiceRegions;
		if (FALSE == hoChoiceRegions.CountObj())
		{

			return;
		}
		ClosingRectangle1(hoChoiceRegions, &ho_RegionClosing, 3.5, 3.5);
		FillUp(ho_RegionClosing, &ho_RegionFillUp);
		Connection(ho_RegionFillUp, &ho_ConnectedRegions);
		//*******LeakFoilError1选择白点中心有黑点的漏箔缺陷********************
		TupleConcat(10, MAX(0, m_hvPeakGrayMid - hv_dDynThresh), &hv_Concat);
		TupleMax(hv_Concat, &hv_Max);
		TupleMin(hv_Concat, &hv_Min);
		SelectGray(ho_ConnectedRegions, m_hoAdaptedImage, &ho_SelectedRegions, HTuple("min").Append("deviation"), "and", hv_Min.TupleConcat(hv_dMinDeviation), hv_Max.TupleConcat(100));
		SelectShape(ho_SelectedRegions, &ho_LeakFoilError1, "max_diameter", "and", hv_dMinDiameter, hv_dMaxDiameter);
		ConcatObj(ho_ErrorRegion, ho_LeakFoilError1, &ho_ErrorRegion);
		if (ho_ErrorRegion.CountObj() > 0)
		{
			AddError(ho_ErrorRegion, NG_ExposeFoil);
		}
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("LeakFoilDetect 函数出现异常:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());

	}
	return;
}

BOOL CDefect::GetDynRegion()
{
	try
	{
		//涂布数量大于2，单个涂布等分成5份
		int SEGMENT_NUM = 2;
		if (m_ptIn->tCoat.ho_Region.CountObj() <= 2) SEGMENT_NUM = 4;

		// Local iconic variables
		HObject  hoCoat, ho_ObjectSelected;
		HObject  ho_ImageReduced, ho_LightRegion, ho_DarkRegion;
		// Local control variables
		HTuple  hv_WidthValue, hv_HeightValue, hv_Mean;
		HTuple  hv_Deviation;
		//ho_ImageOut = ho_Image;
		HTuple hv_AllMean;
		HTuple hv_DiffMean;
		double dMean;
		double dZoomFactor = max(0.1, min(m_tParameters.tPretreatParam.dZoomFactor, 1));
		//两边各腐蚀5个像素，防止没涂好误检
		ErosionRectangle1(m_hoZoomCoatMid, &hoCoat, 5, 1);
		//缩放图像和涂布区域，为了减低检测耗时
		for (int i = 1; i <= hoCoat.CountObj(); i++)
		{
			HObject hoSegRegions;
			RegionFeatures(hoCoat.SelectObj(i), "width", &hv_WidthValue);
			RegionFeatures(hoCoat.SelectObj(i), "height", &hv_HeightValue);
			PartitionRectangle(hoCoat.SelectObj(i), &hoSegRegions, hv_WidthValue / SEGMENT_NUM, hv_HeightValue);
			Intensity(hoSegRegions, m_hoZoomImage, &hv_AllMean, &hv_Deviation);
			Intensity(hoCoat.SelectObj(i), m_hoZoomImage, &m_hvPeakGrayMid, NULL);
			int nLightLowThresh, nDarkHighThresh;
			double dDarkScale, dLightScale;
			for (int hv_j = 1; hv_j <= SEGMENT_NUM; hv_j += 1)
			{
				SelectObj(hoSegRegions, &ho_ObjectSelected, hv_j);
				ReduceDomain(m_hoZoomImage, ho_ObjectSelected, &ho_ImageReduced);
				dMean = hv_AllMean[hv_j - 1].D();
				hv_DiffMean = m_hvPeakGrayMid.D() - dMean;
				if (hv_DiffMean.TupleAbs() > 80)
				{
					dMean = m_hvPeakGrayMid.D();
				}
				else if (dMean < 45 || dMean > 150)
				{
					dMean = 60;
				}
				
				if (hv_j == 1 || hv_j == SEGMENT_NUM)
				{
					dLightScale = m_tParameters.tPretreatParam.dBorderLightCof;
					dDarkScale = m_tParameters.tPretreatParam.dBorderDarkCof;
				}
				else
				{
					dLightScale = m_tParameters.tPretreatParam.dMidLightCof;
					dDarkScale = m_tParameters.tPretreatParam.dMidDarkCof;
				}
				nLightLowThresh = (1 + dLightScale)*dMean;
				nDarkHighThresh = (1 - dDarkScale)*dMean;
				FastThreshold(ho_ImageReduced, &ho_LightRegion, MAX(10, MIN(nLightLowThresh, 255)), 255, MIN(0.2*m_tParameters.tPretreatParam.dZoomFactor / m_dColResolution, 0.2*m_tParameters.tPretreatParam.dZoomFactor / m_dRowResolution));
				FastThreshold(ho_ImageReduced, &ho_DarkRegion, 0, MAX(MIN(255, nDarkHighThresh), 10), MIN(0.2*m_tParameters.tPretreatParam.dZoomFactor / m_dColResolution, 0.2*m_tParameters.tPretreatParam.dZoomFactor / m_dRowResolution));
				//Threshold(ho_ImageReduced, &ho_LightRegion, MAX(10, MIN(nLightLowThresh, 255)), 255);
				//Threshold(ho_ImageReduced, &ho_DarkRegion, 0, MAX(MIN(255, nDarkHighThresh), 10));
				ConcatObj(m_hoLEnableChoiceRegions, ho_LightRegion, &m_hoLEnableChoiceRegions);
				ConcatObj(m_hoDEnableChoiceRegions, ho_DarkRegion, &m_hoDEnableChoiceRegions);
			}
			ConcatObj(m_hoSegRegions, hoSegRegions, &m_hoSegRegions);
		}
		//检测后的区域缩放回原始图像对应的大小
		if (1 != dZoomFactor)
		{
			ZoomRegion(m_hoSegRegions, &m_hoSegRegions, 1 / dZoomFactor, 1 / dZoomFactor);
			ZoomRegion(m_hoLEnableChoiceRegions, &m_hoLEnableChoiceRegions, 1 / dZoomFactor, 1 / dZoomFactor);
			ZoomRegion(m_hoDEnableChoiceRegions, &m_hoDEnableChoiceRegions, 1 / dZoomFactor, 1 / dZoomFactor);
		}
		
		//合并筛选结果
		Union1(m_hoLEnableChoiceRegions, &m_hoLEnableChoiceRegions);
		ClosingRectangle1(m_hoLEnableChoiceRegions, &m_hoLEnableChoiceRegions, 5.5, 5.5);
		Connection(m_hoLEnableChoiceRegions, &m_hoLEnableChoiceRegions);
		FillUp(m_hoLEnableChoiceRegions, &m_hoLEnableChoiceRegions);
		SelectShape(m_hoLEnableChoiceRegions, &m_hoLEnableChoiceRegions, "area", "and", 20, INT_MAX);
		int nNum1 = m_hoLEnableChoiceRegions.CountObj();
		Union1(m_hoDEnableChoiceRegions, &m_hoDEnableChoiceRegions);
		ClosingRectangle1(m_hoDEnableChoiceRegions, &m_hoDEnableChoiceRegions, 5.5, 5.5);
		Connection(m_hoDEnableChoiceRegions, &m_hoDEnableChoiceRegions);
		FillUp(m_hoDEnableChoiceRegions, &m_hoDEnableChoiceRegions);
		SelectShape(m_hoDEnableChoiceRegions, &m_hoDEnableChoiceRegions, "area", "and", 20, INT_MAX);
		int nNum2 = m_hoDEnableChoiceRegions.CountObj();
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("GetDynRegion 函数出现异常:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		m_tInit.pLog(strException, ExcpLog, SlightExcp);
		return FALSE;
	}
	return TRUE;
}

void CDefect::GetChoiceRegions(HObject& hoLChoiceRegions, HObject& hoDChoiceRegions)
{
	//hoLChoiceRegions.GenEmptyObj();
	//hoDChoiceRegions.GenEmptyObj();
	hoLChoiceRegions = m_hoLEnableChoiceRegions;
	hoDChoiceRegions = m_hoDEnableChoiceRegions;

	//Difference(m_hoLEnableChoiceRegions, m_ptOut->hoDisp, &hoLChoiceRegions);//去除已经检测出来的缺陷
	//Difference(m_hoDEnableChoiceRegions, m_ptOut->hoDisp, &hoDChoiceRegions);//去除已经检测出来的缺陷
	return;
}

void CDefect::BubbleDetect()
{
	try
	{
		// Local iconic variables
		HObject  ho_BubbleRegion, ho_ImageReduced, ho_ImageReduceMean;
		HObject  ho_RegionDynThresh, ho_RegionClosing, ho_ConnectedRegions;
		HObject  ho_SelectedRegions, ho_RegionFillUp, ho_SelectedRegions1;
		HObject  ho_SelectBubbleObject, ho_ObjectSelected, ho_RegionDilation;
		HObject  ho_RegionDifference;

		// Local control variables
		HTuple  hv_bDetect, hv_dResolution, hv_dDynThresh;
		HTuple  hv_dMinDiameter, hv_dMaxDiameter, hv_dMinCircularity;
		HTuple  hv_dNum, hv_Number, hv_i, hv_MeanIn, hv_Deviation, hv_GrayDiff;
		HTuple  hv_MeanOut, hv_Deviation1;


		HObject ho_ErrorRegion;
		HTupleVector hvec_Param(1);
		ho_ErrorRegion.GenEmptyObj();

		if (FALSE == m_tParameters.tBubbleParam.bUseDetect)
		{
			return;
		}

		hv_dDynThresh = m_tParameters.tBubbleParam.nDynThresh;
		hv_dMinDiameter = m_tParameters.tBubbleParam.dMinDiameter / m_dColResolution;
		hv_dMinCircularity = m_tParameters.tBubbleParam.dMinCircularity;
		hv_GrayDiff = m_tParameters.tBubbleParam.nInOutGrayDiff;

		HObject hoChoiceRegions;
		HObject hoLChoiceRegions, hoDChoiceRegions;
		HObject ho_RegionDilationRectangle, hoNearRegion;
		HTuple  hv_NearMean;
		GetChoiceRegions(hoLChoiceRegions, hoDChoiceRegions);
		hoChoiceRegions = hoDChoiceRegions;
		if (FALSE == hoChoiceRegions.CountObj())
		{
			return;
		}
		ClosingCircle(hoChoiceRegions, &ho_RegionClosing, 8.5);
		Connection(ho_RegionClosing, &ho_ConnectedRegions);
		FillUp(ho_ConnectedRegions, &ho_RegionFillUp);
		SelectShape(ho_RegionFillUp, &ho_SelectedRegions1, (HTuple("circularity").Append("max_diameter").Append("area")), "and", hv_dMinCircularity.TupleConcat(hv_dMinDiameter).TupleConcat(50), (HTuple(1).Append(INT_MAX).Append(INT_MAX)));
		SelectGray(ho_SelectedRegions1, m_hoAdaptedImage, &ho_SelectedRegions1, HTuple("mean"), "and", 0, MAX(0, m_hvPeakGrayMid - 3));
		CountObj(ho_SelectedRegions1, &hv_Number);
		GenEmptyObj(&ho_SelectBubbleObject);
		{
			HTuple end_val28 = hv_Number;
			HTuple step_val28 = 1;
			for (hv_i = 1; hv_i.Continue(end_val28, step_val28); hv_i += step_val28)
			{
				SelectObj(ho_SelectedRegions1, &ho_ObjectSelected, hv_i);
				DilationCircle(ho_ObjectSelected, &ho_RegionDilation, 13.5);
				DilationRectangle1(ho_RegionDilation, &ho_RegionDilationRectangle, 1, 41);
				Difference(ho_RegionDilationRectangle, ho_RegionDilation, &hoNearRegion);
				Difference(ho_RegionDilation, ho_ObjectSelected, &ho_RegionDifference);

				Intensity(ho_ObjectSelected, m_hoAdaptedImage, &hv_MeanIn, &hv_Deviation);
				Intensity(ho_RegionDifference, m_hoAdaptedImage, &hv_MeanOut, &hv_Deviation1);
				Intensity(hoNearRegion, m_hoAdaptedImage, &hv_NearMean, NULL);

				if (((hv_MeanIn + hv_GrayDiff) < hv_MeanOut) && ((hv_NearMean + hv_GrayDiff / 3) < hv_MeanOut))
				{
					ConcatObj(ho_ErrorRegion, ho_ObjectSelected, &ho_ErrorRegion);
				}
			}
		}
		if (ho_ErrorRegion.CountObj() > 0)
		{
			AddError(ho_ErrorRegion, NG_Bubble, RT_Coat);
		}
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("BubbleDetect 函数出现异常:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		/*	((DHInterface*)(m_tInit.pLogView))->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);*/
	}

}

void CDefect::SelectLine(HObject ho_AllLineRegion, HObject *ho_SelectLineRegion, HTuple hv_StandarWidth,
	HTuple hv_StandarLength, HTuple *hv_WidthValue, HTuple *hv_LengthValue)
{

	// Local iconic variables
	HObject  ho_ObjectSelected, ho_RegionMoved, ho_Skeleton;
	HObject  ho_DistanceImage;

	// Local control variables
	HTuple  hv_Number, hv_i, hv_Row1, hv_Column1;
	HTuple  hv_Row2, hv_Column2, hv_null, hv_Diameter, hv_Min;
	HTuple  hv_Max, hv_Range, hv_Area, hv_Row, hv_Column;
	HTuple hv_Radius;
	GenEmptyObj(&(*ho_SelectLineRegion));
	CountObj(ho_AllLineRegion, &hv_Number);
	TupleGenConst(hv_Number, 0, &(*hv_WidthValue));
	TupleGenConst(hv_Number, 0, &(*hv_LengthValue));
	HTuple hvLengthSub, hvWidthSub, hvResult;
	HTuple hvLengthFlag, hvWidthFlag;
	HTuple hv_ErrorIndex;
	if (hv_Number > 0)
	{
		Skeleton(ho_AllLineRegion, &ho_Skeleton);
		AreaCenter(ho_Skeleton, &hv_Area, &hv_Row, &hv_Column);
		InnerCircle(ho_AllLineRegion, NULL, NULL, &hv_Radius);
		TupleSub(hv_Area, hv_StandarLength, &hvLengthSub);
		TupleSub(hv_Radius * 2, hv_StandarWidth, &hvWidthSub);
		TupleGreaterEqualElem(hvLengthSub, 0, &hvLengthFlag);
		TupleGreaterEqualElem(hvWidthSub, 0, &hvWidthFlag);
		TupleAnd(hvLengthFlag, hvWidthFlag, &hvResult);
		TupleFind(hvResult, 1, &hv_ErrorIndex);

		if (hv_ErrorIndex.Length() > 0 && hv_ErrorIndex != -1)
		{

			SelectObj(ho_AllLineRegion, &ho_ObjectSelected, hv_ErrorIndex + 1);
			TupleSelect(hv_Area, hv_ErrorIndex, &(*hv_LengthValue));
			TupleSelect(hv_Radius * 2, hv_ErrorIndex, &(*hv_WidthValue));
			ConcatObj((*ho_SelectLineRegion), ho_ObjectSelected, &(*ho_SelectLineRegion));
		}
	}
	return;
}


void CDefect::LightLineDetect()
{
	try
	{
		// Local iconic variables
		HObject  ho_LightLineRegion, ho_ImageReduced;
		HObject  ho_RegionDynThresh, ho_RegionClosing, ho_ConnectedRegions;
		HObject  ho_SelectedRegions, ho_RegionFillUp, ho_SelectRegion;

		// Local control variables
		HTuple  hv_bDetect, hv_dResolution, hv_dDynThresh;
		HTuple  hv_dMinWidth, hv_dMinLength, hv_dChangKuanBi, hv_dNum;
		HTuple  hv_WidthValue, hv_LengthValue, hv_Number;
		HObject ho_ErrorRegion;
		GenEmptyObj(&ho_ErrorRegion);

		if (FALSE == m_tParameters.tLightLineParam.bUseDetect)
		{
			return;
		}
		hv_dDynThresh = m_tParameters.tLightLineParam.nDynThresh;
		hv_dMinWidth = m_tParameters.tLightLineParam.dMinWidth / m_dColResolution;
		hv_dMinLength = m_tParameters.tLightLineParam.dMinHeight / m_dRowResolution;
		hv_dChangKuanBi = m_tParameters.tLightLineParam.dMinLengthWidthRatio;


		HObject hoChoiceRegions;
		HObject hoLChoiceRegions, hoDChoiceRegions;
		GetChoiceRegions(hoLChoiceRegions, hoDChoiceRegions);
		hoChoiceRegions = hoLChoiceRegions;
		if (FALSE == hoChoiceRegions.CountObj())
		{
			return;
		}
		SelectShape(hoChoiceRegions, &hoChoiceRegions, "area", "and", 300, INT_MAX);
		Union1(hoChoiceRegions, &ho_RegionClosing);
		ClosingRectangle1(ho_RegionClosing, &ho_RegionClosing, 15, 55);
		Connection(ho_RegionClosing, &ho_ConnectedRegions);
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "anisometry", "and", hv_dChangKuanBi, INT_MAX);
		SelectGray(ho_SelectedRegions, m_hoAdaptedImage, &ho_ConnectedRegions, "mean", "and", MAX(0, m_hvPeakGrayMid + hv_dDynThresh), 255);
		SelectLine(ho_ConnectedRegions, &ho_ErrorRegion, hv_dMinWidth, hv_dMinLength, &hv_WidthValue, &hv_LengthValue);
		if (0 != ho_ErrorRegion.CountObj())
		{
			AddError(ho_ErrorRegion, NG_Streak);
		}
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("LightLineDetect 函数出现异常:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		//((DHInterface*)(m_tInit.pLogView))->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);

	}

	return;
}

void CDefect::BlackLineDetect()
{
	try
	{
		// Local iconic variables
		HObject  ho_BlackLineRegion, ho_ImageReduced;
		HObject  ho_RegionDynThresh, ho_RegionClosing, ho_ConnectedRegions;
		HObject  ho_SelectedRegions, ho_RegionFillUp, ho_SelectedRegions1;
		HObject  ho_SelectedRegions2;

		// Local control variables
		HTuple  hv_bDetect, hv_dResolution, hv_dDynThresh;
		HTuple  hv_dMinWidth, hv_dMinLength, hv_dMinChangKuanBi;
		HTuple  hv_dNum, hv_WidthValue, hv_LengthValue, hv_Number;
		HObject ho_ErrorRegion;
		GenEmptyObj(&ho_ErrorRegion);

		if (FALSE == m_tParameters.tDarkLineParam.bUseDetect)
		{
			return;
		}


		hv_dDynThresh = m_tParameters.tDarkLineParam.nDynThresh;
		hv_dMinWidth = m_tParameters.tDarkLineParam.dMinWidth / m_dColResolution;
		hv_dMinLength = m_tParameters.tDarkLineParam.dMinHeight / m_dRowResolution;
		hv_dMinChangKuanBi = m_tParameters.tDarkLineParam.dMinLengthWidthRatio;


		HObject hoChoiceRegions;
		HObject hoLChoiceRegions, hoDChoiceRegions;
		GetChoiceRegions(hoLChoiceRegions, hoDChoiceRegions);
		hoChoiceRegions = hoDChoiceRegions;
		if (FALSE == hoChoiceRegions.CountObj())
		{
			return;
		}
		SelectShape(hoChoiceRegions, &hoChoiceRegions, "area", "and", 300, INT_MAX);
		ClosingRectangle1(hoChoiceRegions, &ho_RegionClosing, 15, 55);
		Connection(ho_RegionClosing, &ho_ConnectedRegions);
		FillUp(ho_ConnectedRegions, &ho_RegionFillUp);
		SelectShape(ho_RegionFillUp, &ho_SelectedRegions, "anisometry", "and", hv_dMinChangKuanBi, INT_MAX);
		SelectGray(ho_SelectedRegions, m_hoAdaptedImage, &ho_ConnectedRegions, "mean", "and", 0, MAX(0, m_hvPeakGrayMid - hv_dDynThresh));
		SelectLine(ho_ConnectedRegions, &ho_ErrorRegion, hv_dMinWidth, hv_dMinLength, &hv_WidthValue, &hv_LengthValue);
		if (ho_ErrorRegion.CountObj() > 0)
		{
			AddError(ho_ErrorRegion, NG_Streak);
		}
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("BlackLineDetect 函数出现异常:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		//((DHInterface*)(m_tInit.pLogView))->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);

	}
	return;
}

void CDefect::LightDotDetect()
{
	try
	{
		// Local iconic variables
		HObject  ho_LightDotRegion, ho_ImageReduced, ho_Region;
		HObject  ho_RegionClosing, ho_RegionFillUp, ho_ConnectedRegions;

		// Local control variables
		HTuple  hv_bDetect, hv_dResolution;
		HTuple  hv_dMinWidth, hv_dMinHeight, hv_dMinArea, hv_dMaxArea, hv_Number1;

		HObject ho_ErrorRegion;
		HTupleVector hvec_Param(1);
		GenEmptyObj(&ho_ErrorRegion);

		if (FALSE == m_tParameters.tLightDotParam.bUseDetect)
		{
			return;
		}
		HTuple hv_dLowThresh, hv_dHighThresh, hv_MinDeviation, hv_MinGrayRange;
		hv_dLowThresh = m_tParameters.tLightDotParam.nMinThresh;
		hv_dHighThresh = m_tParameters.tLightDotParam.nMaxThresh;
		hv_dMinWidth = m_tParameters.tLightDotParam.dMinWidth / m_dColResolution;
		hv_dMinHeight = m_tParameters.tLightDotParam.dMinHeight / m_dRowResolution;
		hv_dMinArea = m_tParameters.tLightDotParam.dMinArea / m_dColResolution / m_dRowResolution;
		hv_dMaxArea = m_tParameters.tLightDotParam.dMaxArea / m_dColResolution / m_dRowResolution;

		hv_MinDeviation = m_tParameters.tLightDotParam.nMinDeviation;
		hv_MinGrayRange = m_tParameters.tLightDotParam.nMinGrayRange;


		HObject hoChoiceRegions;
		HObject hoLChoiceRegions, hoDChoiceRegions;
		GetChoiceRegions(hoLChoiceRegions, hoDChoiceRegions);
		hoChoiceRegions = hoLChoiceRegions;
		if (FALSE == hoChoiceRegions.CountObj())
		{
			return;
		}
		ClosingRectangle1(hoChoiceRegions, &ho_RegionClosing, 15, 15);
		SelectGray(ho_RegionClosing, m_hoAdaptedImage, &ho_ConnectedRegions, "mean", "and", MIN(hv_dLowThresh.I(), hv_dHighThresh.I()), MAX(hv_dLowThresh.I(), hv_dHighThresh.I()));
		SelectShape(ho_ConnectedRegions, &ho_ConnectedRegions, HTuple("width").Append("height").Append("area"), "and", hv_dMinWidth.Append(hv_dMinHeight).Append(hv_dMinArea), (HTuple(INT_MAX)).Append(INT_MAX).Append(hv_dMaxArea));

		ErosionCircle(ho_ConnectedRegions, &ho_ConnectedRegions, 3.5);
		if (ho_ConnectedRegions.CountObj() > 0)
		{
			HTuple hv_Deviation, hvGrayRange, hv_Result, hv_FindIndex;
			HTuple hv_DeviationCompareTuple, hv_GrayRangeCompareTuple;
			Intensity(ho_ConnectedRegions, m_hoAdaptedImage, NULL, &hv_Deviation);
			MinMaxGray(ho_ConnectedRegions, m_hoAdaptedImage, 0, NULL, NULL, &hvGrayRange);
			TupleLessEqualElem(hv_Deviation, hv_MinDeviation, &hv_DeviationCompareTuple);
			TupleLessEqualElem(hvGrayRange, hv_MinGrayRange, &hv_GrayRangeCompareTuple);
			TupleOr(hv_DeviationCompareTuple, hv_GrayRangeCompareTuple, &hv_Result);
			TupleFind(hv_Result, 1, &hv_FindIndex);
			if (hv_FindIndex != -1 && hv_FindIndex.Length() > 0)
			{
				SelectObj(ho_ConnectedRegions, &ho_ConnectedRegions, hv_FindIndex + 1);
				DilationCircle(ho_ConnectedRegions, &ho_ErrorRegion, 3.5);
				if (ho_ErrorRegion.CountObj())
				{
					AddError(ho_ErrorRegion, NG_WhiteSpot);
				}
			}
		}
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("LightDotDetect 函数出现异常:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
	}
	return;

}

void CDefect::BlackDotDetect()
{

	try
	{
		// Local iconic variables
		HObject  ho_BlackDotRegion, ho_ImageReduced, ho_Region;
		HObject  ho_RegionClosing, ho_RegionFillUp, ho_ConnectedRegions;
		HObject  ho_SelectedRegions, ho_RegionTrans;

		// Local control variables
		HTuple  hv_bDetect, hv_dResolution, hv_dThresh;
		HTuple  hv_dMinWidth, hv_dMinHeight, hv_dMinArea, hv_dMaxArea;

		HObject ho_ErrorRegion;
		GenEmptyObj(&ho_ErrorRegion);

		if (FALSE == m_tParameters.tDarkDotParam.bUseDetect)
		{
			return;
		}
		hv_dThresh = m_tParameters.tDarkDotParam.nMaxThresh;
		hv_dMinWidth = m_tParameters.tDarkDotParam.dMinWidth / m_dColResolution;
		hv_dMinHeight = m_tParameters.tDarkDotParam.dMinHeight / m_dRowResolution;
		hv_dMinArea = m_tParameters.tDarkDotParam.dMinArea / m_dColResolution / m_dColResolution;
		hv_dMaxArea = m_tParameters.tDarkDotParam.dMaxArea / m_dColResolution / m_dColResolution;

		HObject hoChoiceRegions;
		HObject hoLChoiceRegions, hoDChoiceRegions;
		GetChoiceRegions(hoLChoiceRegions, hoDChoiceRegions);
		hoChoiceRegions = hoDChoiceRegions;
		if (FALSE == hoChoiceRegions.CountObj())
		{
			return;
		}
		ClosingRectangle1(hoChoiceRegions, &ho_RegionClosing, 15, 15);
		SelectShape(ho_RegionClosing, &ho_ConnectedRegions, HTuple("width").Append("height").Append("area"), "and", hv_dMinWidth.TupleConcat(hv_dMinHeight).TupleConcat(hv_dMinArea), (HTuple(9999999).Append(9999999).Append(hv_dMaxArea)));
		SelectGray(ho_ConnectedRegions, m_hoAdaptedImage, &ho_ErrorRegion, "mean", "and", 0, hv_dThresh);
		
		if (ho_ErrorRegion.CountObj() > 0)
		{
			AddError(ho_ErrorRegion, NG_BlackSpot);
		}
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("BlackDotDetect 函数出现异常:%s_%s", e.ProcName().Text(), e.ErrorMessage().Text());
		//	((DHInterface*)(m_tInit.pLogView))->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);

	}
	return;
}

BOOL CDefect::HasDefect()
{
	return (m_ptOut->hoDisp.CountObj() > 0) || m_bDetectEnd;
}

BOOL CDefect::TapeDetect()
{
	if (m_tParameters.tTapeParam.bUseDetect == FALSE)
	{
		return TRUE;
	}

	HTuple hv_dDarkThresh, hv_dLightThresh;
	HTuple hv_MinDarkArea, hv_MinLightArea;
	hv_dDarkThresh = m_tParameters.tTapeParam.nDarkThresh;
	hv_MinDarkArea = m_tParameters.tTapeParam.dDarkMinArea / m_dColResolution / m_dRowResolution;

	hv_dLightThresh = m_tParameters.tTapeParam.nLightThresh;
	hv_MinLightArea = m_tParameters.tTapeParam.dLightMinArea / m_dColResolution / m_dRowResolution;

	HObject hoImageReduced, hoRegion, hoConnectionRegions, hoFillUp;
	HObject hoClosinRegion, hoErrorRegions;
	HObject hoLightJiaoDai, hoDarkJiaoDai;
	hoLightJiaoDai.GenEmptyObj();
	hoDarkJiaoDai.GenEmptyObj();
	hoErrorRegions.GenEmptyObj();
	ReduceDomain(m_hoAdaptedImage, m_hoCoatMid, &hoImageReduced);
	Threshold(hoImageReduced, &hoRegion, 0, hv_dDarkThresh);
	Connection(hoRegion, &hoRegion);
	SelectShape(hoRegion, &hoConnectionRegions, "area", "and", 150, INT_MAX);
	Union1(hoConnectionRegions, &hoConnectionRegions);
	FillUp(hoConnectionRegions, &hoFillUp);
	ClosingRectangle1(hoFillUp, &hoClosinRegion, 31, 71);
	Connection(hoClosinRegion, &hoConnectionRegions);
	SelectShape(hoConnectionRegions, &hoDarkJiaoDai, "area", "and", hv_MinDarkArea, INT_MAX);
	ConcatObj(hoErrorRegions, hoDarkJiaoDai, &hoErrorRegions);
	Threshold(hoImageReduced, &hoRegion, hv_dLightThresh, 255);
	Connection(hoRegion, &hoRegion);
	SelectShape(hoRegion, &hoConnectionRegions, "area", "and", 150, INT_MAX);
	Union1(hoConnectionRegions, &hoConnectionRegions);
	FillUp(hoConnectionRegions, &hoFillUp);
	ClosingRectangle1(hoFillUp, &hoClosinRegion, 31, 71);
	Connection(hoClosinRegion, &hoConnectionRegions);
	SelectShape(hoConnectionRegions, &hoLightJiaoDai, "area", "and", hv_MinLightArea, INT_MAX);
	ConcatObj(hoErrorRegions, hoLightJiaoDai, &hoErrorRegions);
	if (hoErrorRegions.CountObj() > 0)
	{
		AddError(hoErrorRegions, NG_Tape, RT_Coat);
		return FALSE;
	}

	return TRUE;
}

void CDefect::DropDetect()
{
	HObject hoErrorRegion;
	hoErrorRegion.GenEmptyObj();
	if (FALSE == m_tParameters.tDropParam.bUseDetect)
	{
		return;
	}
	HObject ho_ImageReduced, ho_Region, ho_Connections, ho_FillUp;
	HTuple hv_MaxThresh, hv_MinArea;
	hv_MaxThresh = m_tParameters.tDropParam.nMaxThresh;
	hv_MinArea = m_tParameters.tDropParam.dMinArea / m_dRowResolution / m_dColResolution;
	HObject hoUnion;
	Union2(m_hoCoatMid, m_hoAT11BaseCoatRegion, &hoUnion);
	ReduceDomain(m_hoAdaptedImage, m_hoCoatMid, &ho_ImageReduced);
	//Threshold(ho_ImageReduced,&ho_Region,0,hv_MaxThresh);
	HObject hoAllRegion;
	hoAllRegion.GenEmptyObj();
	Union2(m_hoLEnableChoiceRegions, m_hoDEnableChoiceRegions, &hoAllRegion);
	ClosingRectangle1(hoAllRegion, &hoAllRegion, 15, 15);
	Connection(hoAllRegion, &ho_Connections);
	SelectShape(ho_Connections, &ho_Connections, "area", "and", hv_MinArea, INT_MAX);
	FillUp(ho_Connections, &ho_FillUp);
	Connection(ho_FillUp, &ho_Connections);
	SelectGray(ho_Connections, m_hoAdaptedImage, &hoErrorRegion, "deviation", "and", hv_MaxThresh, INT_MAX);
	if (hoErrorRegion.CountObj() > 0)
	{
		AddError(hoErrorRegion, NG_Uncoated, RT_Coat);
		hoErrorRegion.GenEmptyObj();
	}
	HTuple hv_LightMinThresh, hv_LightMaxThresh, hv_LightMinArea;
	hv_LightMinThresh = m_tParameters.tDropParam.nLTMinThresh;
	hv_LightMaxThresh = m_tParameters.tDropParam.nLTMaxThresh;
	hv_LightMinArea = m_tParameters.tDropParam.dLTMinArea / m_dRowResolution / m_dColResolution;
	Threshold(ho_ImageReduced, &ho_Region, hv_LightMinThresh, hv_LightMaxThresh);
	Connection(ho_Region, &ho_Connections);
	SelectShape(ho_Connections, &ho_Connections, "area", "and", 100, INT_MAX);
	FillUp(ho_Connections, &ho_FillUp);
	ClosingRectangle1(ho_FillUp, &ho_FillUp, 31, 31);
	Connection(ho_FillUp, &ho_Connections);
	SelectShape(ho_Connections, &hoErrorRegion, "area", "and", hv_LightMinArea, INT_MAX);
	if (hoErrorRegion.CountObj() > 0)
	{
		AddError(hoErrorRegion, NG_Uncoated, RT_Coat);
		GenEmptyObj(&hoErrorRegion);
		return;
	}


	HObject hoBorderRegion;
	hoBorderRegion.GenEmptyObj();
	if (m_tInit.tProductInfo.bExistAT11)//正极是AT9 负极是边界
	{
		hoBorderRegion = m_hoAT11BaseCoatRegion;
		if (hoBorderRegion.CountObj() > 0)
		{
			int nMaxThresh = m_tParameters.tDropParam.nAT9MaxThresh;
			double dMinWidth = m_tParameters.tDropParam.dAT9DarkMinWidth / m_dColResolution;
			double dMinArea = m_tParameters.tDropParam.dAT9DarkMinArea / m_dColResolution / m_dRowResolution;
			HObject hoAt9Region;
			ErosionRectangle1(hoBorderRegion, &hoAt9Region, 5, 1);
			ReduceDomain(m_ptIn->hoImage, hoAt9Region, &ho_ImageReduced);
			Threshold(ho_ImageReduced, &ho_Region, 0, nMaxThresh);
			Connection(ho_Region, &ho_Connections);
			SelectShape(ho_Connections, &ho_Connections, "area", "and", 5, INT_MAX);
			FillUp(ho_Connections, &ho_FillUp);
			ClosingRectangle1(ho_FillUp, &ho_FillUp, 31, 31);
			Connection(ho_FillUp, &ho_Connections);
			SelectShape(ho_Connections, &hoErrorRegion, HTuple("area").Append("width"), "and", HTuple(dMinArea).Append(dMinWidth), HTuple(INT_MAX).Append(INT_MAX));
			if (hoErrorRegion.CountObj() > 0)
			{
				AddError(hoErrorRegion, NG_BlackSpot, RT_AT11);
				GenEmptyObj(&hoErrorRegion);
			}
		}
	}
	else
	{
		//波浪边暂时不检测
		/*hoBorderRegion = m_hoBoderRegion;
		if (hoBorderRegion.CountObj() > 0)
		{
			int nMaxThresh = m_tParameters.tDropParam.nAT9MaxThresh;
			double dMinWidth = m_tParameters.tDropParam.dAT9DarkMinWidth / m_dRowResolution;
			double dMinArea = m_tParameters.tDropParam.dAT9DarkMinArea / m_dRowResolution / m_dRowResolution;
			HObject hoAt9Region;
			ErosionRectangle1(hoBorderRegion, &hoAt9Region, 5, 1);
			ReduceDomain(m_ptIn->hoImage, hoAt9Region, &ho_ImageReduced);
			Threshold(ho_ImageReduced, &ho_Region, 0, nMaxThresh);
			Connection(ho_Region, &ho_Connections);
			SelectShape(ho_Connections, &ho_Connections, "area", "and", 5, INT_MAX);
			FillUp(ho_Connections, &ho_FillUp);
			ClosingRectangle1(ho_FillUp, &ho_FillUp, 31, 31);
			Connection(ho_FillUp, &ho_Connections);
			SelectShape(ho_Connections, &hoErrorRegion, HTuple("area").Append("width"), "and", HTuple(dMinArea).Append(dMinWidth), HTuple(INT_MAX).Append(INT_MAX));
			if (hoErrorRegion.CountObj() > 0)
			{
				AddError(hoErrorRegion, NG_WavyEdge, RT_Coat);
				hoErrorRegion.GenEmptyObj();
			}

		}*/

	}
}

void CDefect::FoilDetect()
{
	HObject  ho_Region, ho_ConnectedRegions, ho_reduce_image, ho_FoilUnionRegion;
	HObject  ho_RegionOpening, ho_SelectedRegions, ho_DarkDotRegion;

	if (FALSE == m_tParameters.tFoilParam.bUseDetect)
	{
		return;
	}

	try
	{
		//Union1(m_ptIn->tFoil.ho_Region, &ho_FoilUnionRegion);
		ErosionRectangle1(m_hoFoilRegion, &ho_FoilUnionRegion, 5, 1);
		//极耳带料/黑点
		ReduceDomain(m_ptIn->hoImage, ho_FoilUnionRegion, &ho_reduce_image);
		Threshold(ho_reduce_image, &ho_Region, 0, m_tParameters.tFoilParam.nMaxDarkThresh);//50
		if(m_tInit.hvCamSNList.Length()==2)
			Difference(ho_Region, m_hoOverRegion, &ho_Region);
		Connection(ho_Region, &ho_ConnectedRegions);
		OpeningRectangle1(ho_ConnectedRegions, &ho_RegionOpening, 15, 15);
		SelectShape(ho_RegionOpening, &ho_SelectedRegions, "width", "and", m_tParameters.tFoilParam.dMinDarkWidth / m_dColResolution, INT_MAX);//1
		SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "height", "and", m_tParameters.tFoilParam.dMinDarkHeight / m_dRowResolution, INT_MAX);//1
		SelectShape(ho_SelectedRegions, &ho_DarkDotRegion, "area", "and", m_tParameters.tFoilParam.dMinDarkArea / m_dRowResolution / m_dColResolution, INT_MAX);
		AddError(ho_DarkDotRegion, NG_FoilBlackSpot, RT_Foil);
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CDefect%d_DarkDotDetect 函数出现异常:%s_%s", m_tInit.nSN, e.ProcName().Text(), e.ErrorMessage().Text());
		m_tInit.pLog(strException, ExcpLog, SlightExcp);
	}
	return;
}