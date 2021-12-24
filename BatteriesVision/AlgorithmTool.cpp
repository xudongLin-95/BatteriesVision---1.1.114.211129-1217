#include "StdAfx.h"
#include "AlgorithmTool.h"
#include "SCIMeasure.in.h"
#include "SCIDetect.in.h"
#include "SCIFindMark.in.h"


CAlgorithmTool::CAlgorithmTool()
{
	m_vecAlgMeasure.clear();
	m_vecAlgDetect.clear();
	m_vecAlgFindMark.clear();
}

CAlgorithmTool::~CAlgorithmTool()
{
	for (int i=0;i<m_vecAlgMeasure.size();i++)
	{
		if (m_vecAlgMeasure[i].pAlg!=NULL)
		{
			delete m_vecAlgMeasure[i].pAlg;
			m_vecAlgMeasure[i].pAlg = NULL;
		}
	}
}

BOOL CAlgorithmTool::Init()
{
	ReadParam();
	for (int i=0;i<m_nAlgNum;i++)
	{
		m_vecAlgMeasure.push_back(InitAlg(i, AlgMeasure));//加载和相机个数相同的测量算法
	}
	/*for (int i=0;i<m_nAlgNum-1;i++)
	{
	m_vecAlgDetect.push_back(InitAlg(i,AlgSurface));
	}*/
	/*for (int i=0;i<m_nAlgNum-1;i++)
	{
	m_vecAlgFindMark.push_back(InitAlg(i,AlgFindMark));
	}*/
	

	return TRUE;
}

CAlgorithmTool::tagAlg CAlgorithmTool::InitAlg(const int& nSN,eAlgType eType)
{
	tagAlg alg;
	alg.eType = (eAlgType)eType;
	if (AlgMeasure==eType)//测量算法
	{
		//加载分割动态库
		MeasureInit InitParam;
		InitParam.tProductInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
		InitParam.nSN = nSN;
		InitParam.hvCamSNList = theGlobal.m_Project.GetSideToCamSNList(nSN);
		InitParam.pLog= theGlobal.m_Log.LogCallBack;
		InitParam.pMain = theGlobal.m_Callback.MainCallback;
		
		DHInterface* t = LoadDHInterFaceDll("SCIMeasure");
		if (t == NULL || TRUE != t->DoSomething("Init",&InitParam,NULL))
		{
			CString strException;
			strException.Format("Fail to init algorithm%d,program exists.\r\n相机%d算法初始化失败，程序将退出", nSN, nSN);
			AfxMessageBox(strException);
			exit(1);
		}
		alg.pAlg = t;
	}
	else if(AlgSurface==eType)//检测算法
	{
		DetectInit InitParam;
		InitParam.strProductName.Format("%s",theGlobal.m_ProductTool.GetCurrentProductName());
		InitParam.nSN = nSN;
		InitParam.hvCamSNList = theGlobal.m_Project.GetSideToCamSNList(nSN);
		InitParam.pLog= theGlobal.m_Log.LogCallBack;
		InitParam.nProductType = theGlobal.m_ProductTool.GetCurrentProductInfo().nPDType;
		InitParam.nCoatingType = theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType;
		InitParam.bHasAT11 = theGlobal.m_ProductTool.GetCurrentProductInfo().bExistAT11;
		InitParam.bHasBaseCoat = theGlobal.m_ProductTool.GetCurrentProductInfo().bExistBaseCoat;
		InitParam.bHasIntermittent = theGlobal.m_ProductTool.GetCurrentProductInfo().bExistIntermittent;
		DHInterface* t = LoadDHInterFaceDll("SCIDetect");
		if (t == NULL || TRUE != t->DoSomething("INITIALIZE",&InitParam,NULL))//INITIALIZE
		{
			CString strException;
			strException.Format("Fail to init algorithm%d,program exists.\r\n相机%d算法初始化失败，程序将退出", nSN, nSN);
			AfxMessageBox(strException);
			exit(1);
		}
		alg.pAlg = t;
	}
	return alg;
}

BOOL CAlgorithmTool::ReadParam()
{
	m_nAlgNum = GetIniInt("Project", "SideCount", 2);

	return TRUE;
}

BOOL CAlgorithmTool::WriteParam()
{

	return TRUE;
}

int	CAlgorithmTool::DoSomething(const int& nSideSN,eAlgType eType,void *message,void *wParam,void *lParam)
{
	AlgList vec;
	if (AlgMeasure==eType)
		vec = m_vecAlgMeasure;
	else if (AlgSurface==eType)
		vec = m_vecAlgDetect;
	else
		vec = m_vecAlgFindMark;
	if (nSideSN >= vec.size() || NULL==vec[nSideSN].pAlg)
		return -1;

	return vec[nSideSN].pAlg->DoSomething(message,wParam,lParam);
}

int CAlgorithmTool::ShowParamDlg(const int& nSideSN, eAlgType eType, void* wParam, void* lParam)
{
	AlgList vec;
	if (AlgMeasure==eType)
		vec = m_vecAlgMeasure;
	else if (AlgSurface==eType)
		vec = m_vecAlgDetect;
	else
		vec = m_vecAlgFindMark;
	if (nSideSN >= vec.size() || NULL==vec[nSideSN].pAlg)
		return -1;

	return vec[nSideSN].pAlg->DoSomething("Show", wParam, lParam);
}

int CAlgorithmTool::Detect(const int& nSideSN,eAlgType eType,void *wParam,void *lParam)
{
	AlgList vec;
	if (AlgMeasure==eType)
		vec = m_vecAlgMeasure;
	else if (AlgSurface==eType)
		vec = m_vecAlgDetect;
	else
		vec = m_vecAlgFindMark;
	if (nSideSN >= vec.size() || NULL==vec[nSideSN].pAlg)
		return FALSE;

	if (AlgMeasure==eType)
		return vec[nSideSN].pAlg->DoSomething("Action",wParam,lParam);
	else if (AlgSurface==eType)
		return vec[nSideSN].pAlg->DoSomething("Action",wParam,lParam);
	else
		return vec[nSideSN].pAlg->DoSomething("FIND",wParam,lParam);
}

void CAlgorithmTool::CheckMeasure()
{
	//开启小线程，确认测量寻边
	int nStartThread = 0;
	HDevThread*         pHDThread;
	HDevThreadContext   hTContext;
	for (int i = 0; i < 1; i++)//原图测量检测线程
	{
		pHDThread = new HDevThread(hTContext,
			(void*)HThreadCheckMeasure, 0, 0);
		pHDThread->Start();
		nStartThread++;
	}
}

void* CAlgorithmTool::HThreadCheckMeasure(void *hcthread)
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		//开小线程确认测量线
		//先确认正面
		if (theGlobal.m_hovecHistroyImage[0].O().CountObj() == 0)
		{
			CErrorInfo::tagErrorInfo tInfo;
			tInfo.strDescribe.Format("首件测量边确认无历史图像,请检查");
			tInfo.nID = 0;
			tInfo.strSolution.Format("1检查是否涂布机已经开涂\n2检查系统历史图像个数是否大于0");
			theGlobal.m_Log.Alarm(tInfo);
			return NULL;
		}
		MeasureInput tInput;
		tInput.nMode = DebugMode;
		GenEmptyObj(&tInput.hoGrabImage);
		HTuple hvGrabSNList = theGlobal.m_Project.GetSideToCamSNList(0);
		for (int k = 0; k < hvGrabSNList.Length(); k++)
		{
			HObject hoTmp;
			SelectObj(theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O(), &hoTmp, theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O().CountObj());
			ConcatObj(tInput.hoGrabImage, hoTmp, &tInput.hoGrabImage);
		}
		theGlobal.m_AlgTool.ShowParamDlg(0, CAlgorithmTool::AlgMeasure, &tInput, "CheckMeasure");

		//确认反面
		if (theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
		{
			tInput.nMode = DebugMode;
			GenEmptyObj(&tInput.hoGrabImage);
			hvGrabSNList = theGlobal.m_Project.GetSideToCamSNList(1);
			for (int k = 0; k < hvGrabSNList.Length(); k++)
			{
				HObject hoTmp;
				SelectObj(theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O(), &hoTmp, theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O().CountObj());
				ConcatObj(tInput.hoGrabImage, hoTmp, &tInput.hoGrabImage);
			}
			theGlobal.m_AlgTool.ShowParamDlg(1, CAlgorithmTool::AlgMeasure, &tInput, "CheckMeasure");
		}

		theGlobal.m_Judge.m_tFirstArticleInspCount.bStart = TRUE;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadCheckMeasure is exceptional(HThreadCheckMeasure线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
		bool is_direct_call = hcppthread->IsDirectCall();
		hcppthread->Exit();
		delete hcppthread;
		hcppthread = NULL;
		if (is_direct_call)
			throw e;
	}
	return NULL;
}