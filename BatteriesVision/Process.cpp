#include "StdAfx.h"
#include "Process.h"
#include "SCIFindMark.in.h"

BYTE* CProcess::m_pBufferFirstImage[2] = { NULL,NULL };
BYTE* CProcess::m_pBufferNextImage[2] = { NULL,NULL };
BYTE* CProcess::m_pBufferTilePostion[2] = { NULL,NULL };

void GetImage(int nGrabberSN, HObject* hoGrabberImage)
{
	__int64 nAddr(0), nImageSize(0), nPixelSize(0), nImageWidth(0), nImageHeight(0), nGrabberType(0);
	GBParamID Param = GBImageBufferAddr;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nAddr);
	Param = GBImageBufferSize;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nImageSize);
	Param = GBImagePixelSize;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nPixelSize);
	Param = GBImageWidth;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nImageWidth);
	Param = GBImageHeight;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nImageHeight);
	Param = GBGrabberTypeSN;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nGrabberType);
	if (1 == nPixelSize)
	{
		try
		{
			GenImage1(hoGrabberImage, "byte", nImageWidth, nImageHeight, nAddr);
		}
		catch (HException e)
		{
			CString strException;
			strException.Format("地址取图\r\n异常信息%s  [ %s() ]", e.ErrorMessage().Text()
				, e.ProcName().Text());
			//g_pLog->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);
		}
		//GenImage1Extern(hoGrabberImage, "byte", nImageWidth,nImageHeight, nAddr,0);
	}
	if (0 == nGrabberType)//如果是模拟
	{
		MirrorImage(*hoGrabberImage, hoGrabberImage, "row");

	}

	if (3 == nGrabberType && 0 == nGrabberSN)
	{
		MirrorImage(*hoGrabberImage, hoGrabberImage, "column");
	}

	/*if (1 == g_tSystemSet.bMirrorRow)
	{
		MirrorImage(*hoGrabberImage,hoGrabberImage,"row");
	}*/

}
double GetEncoder(const int& nGrabberSN)
{
	__int64 nImageHeight;
	GBParamID Param = GBImageHeight;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nImageHeight);
	static double dEnCoder[2] = { 0.0 };
	dEnCoder[nGrabberSN] = dEnCoder[nGrabberSN] + nImageHeight * theGlobal.m_vecResolution[nGrabberSN].dRow;
	return dEnCoder[nGrabberSN];
}
int  GetFrameSN(const int& nGrabberSN)
{
	static int nFrameSN[4] = { 0 };
	return ++nFrameSN[nGrabberSN];
}

CProcess::CProcess()
	:m_ImageQueues(1),
	m_ResultQueues(1),
	m_DisplayQueues(1)
{
	m_nAcqThreadNum = 1;
	m_nDetectThreadNum = 2;
	m_nMergeThreadNum = 1;
	m_vecThreadHandle.clear();
	m_pBufferFirstImage[0] = NULL;
	m_pBufferNextImage[0] = NULL;
	m_pBufferTilePostion[0] = NULL;
	m_pBufferFirstImage[1] = NULL;
	m_pBufferNextImage[1] = NULL;
	m_pBufferTilePostion[1] = NULL;
}

CProcess::~CProcess()
{
	ReleaseThread();
	ReleaseQueues();

	if (m_pBufferFirstImage[0] != NULL)
	{
		delete[] m_pBufferFirstImage[0];
		m_pBufferFirstImage[0] = NULL;
	}
	if (m_pBufferFirstImage[1] != NULL)
	{
		delete[] m_pBufferFirstImage[1];
		m_pBufferFirstImage[1] = NULL;
	}
}

BOOL CProcess::Init()
{
	ReadParam();
	InitQueues();
	InitThread();
	return TRUE;
}

BOOL CProcess::ReadParam()
{
	m_nAcqThreadNum = GetIniInt("Project", "GrabberCount", 4);
	m_nDetectThreadNum = GetIniInt("Project", "SideCount", 2);
	m_nMergeThreadNum = GetIniInt("Project", "MergeCount", 1);

	return TRUE;
}

BOOL CProcess::WriteParam()
{
	return TRUE;
}

BOOL CProcess::InitQueues()
{
	for (int i = 0; i < m_nAcqThreadNum; i++)
	{
		CreateMessageQueue(&m_ImageQueues[i].T());
		SetMessageQueueParam(m_ImageQueues[i].T(), "max_message_num", MAX_QUEUE_NUM);
	}
	for (int i = 0; i < m_nDetectThreadNum; i++)
	{
		CreateMessageQueue(&m_ResultQueues[i].T());
		SetMessageQueueParam(m_ResultQueues[i].T(), "max_message_num", MAX_QUEUE_NUM);
	}
	for (int i = 0; i < 1; i++)
	{
		CreateMessageQueue(&m_DisplayQueues[i].T());
		SetMessageQueueParam(m_DisplayQueues[i].T(), "max_message_num", MAX_QUEUE_NUM);
	}
	return TRUE;
}

BOOL CProcess::ReleaseQueues()
{
	for (int i = 0; i < m_ImageQueues.Length(); i++)
	{
		HALCON_TRY(SetMessageQueueParam(m_ImageQueues[i].T(), "flush_queue", 1));
	}
	for (int i = 0; i < m_ResultQueues.Length(); i++)
	{
		HALCON_TRY(SetMessageQueueParam(m_ResultQueues[i].T(), "flush_queue", 1));
	}
	for (int i = 0; i < m_DisplayQueues.Length(); i++)
	{
		HALCON_TRY(SetMessageQueueParam(m_DisplayQueues[i].T(), "flush_queue", 1));
	}

	return TRUE;
}
BOOL CProcess::InitThread()
{
	int nStartThread = 0;
	HDevThread*         pHDThread;
	HDevThreadContext   hTContext;
	for (int i = 0; i < m_nDetectThreadNum; i++)//原图测量检测线程
	{
		pHDThread = new HDevThread(hTContext,
			(void*)HThreadDetect, 3, 0);
		pHDThread->SetInputCtrlParamVector(0, m_ImageQueues);
		pHDThread->SetInputCtrlParamVector(1, m_ResultQueues);
		pHDThread->SetInputCtrlParamTuple(2, i);
		pHDThread->Start();
		nStartThread++;
		m_vecThreadHandle.push_back(pHDThread);
	}
	for (int i = 0; i < m_nMergeThreadNum; i++)//综合结果线程
	{
		pHDThread = new HDevThread(hTContext,
			(void*)HThreadMerge, 3, 0);
		pHDThread->SetInputCtrlParamVector(0, m_ResultQueues);
		pHDThread->SetInputCtrlParamTuple(1, m_DisplayQueues[i].T());
		pHDThread->SetInputCtrlParamTuple(2, i);
		pHDThread->Start();
		nStartThread++;
		m_vecThreadHandle.push_back(pHDThread);
	}
	for (int i = 0; i < 1; i++)//监听线程
	{
		pHDThread = new HDevThread(hTContext,
			(void*)HThreadListen, 0, 0);
		pHDThread->Start();
		nStartThread++;
		m_vecThreadHandle.push_back(pHDThread);
	}
	return TRUE;
}

BOOL CProcess::ReleaseThread()
{
	for (int i = 0; i < m_vecThreadHandle.size(); i++)
	{
		if (NULL != m_vecThreadHandle[i])
		{
			HALCON_TRY(m_vecThreadHandle[i]->Exit())
				delete m_vecThreadHandle[i];
			m_vecThreadHandle[i] = NULL;
		}
	}
	return TRUE;
}

void WINAPI CProcess::AcquisitionCallBack(const s_GBSIGNALINFO* SigInfo)
{
	T_Grab(SigInfo, theGlobal.m_Process.GetImageQueues());
}

void WINAPI CProcess::T_Grab(const s_GBSIGNALINFO* SigInfo, HTupleVector hvec_QueueImages)
{
	WPARAM wParam = MAKEWPARAM(SigInfo->nGrabberSN, SigInfo->nErrorCode);
	int nGrabberSN = SigInfo->nGrabberSN;
	LPARAM lParam = MAKELPARAM(SigInfo->nFrameCount, SigInfo->nFrameCount);//图像号放到状态栏里
	theGlobal.PostMessageToMainDlg(WM_GRABBER, wParam, lParam);//通知状态栏采集到了一帧

	if (SigInfo->nErrorCode != 0)
	{
		CString strExcp;
		strExcp.Format("T_Grab%d采图异常: ImageSN%d ErrorCode%d Description:%s", SigInfo->nGrabberSN, SigInfo->nFrameCount, SigInfo->nErrorCode, SigInfo->strDescription);
		theGlobal.m_Log.Exception(strExcp);

		if (SigInfo->nErrorCode == 20)
		{
			CErrorInfo::tagErrorInfo tInfo;
			tInfo.strDescribe.Format("相机%d出现采图异常,请检查", SigInfo->nGrabberSN, SigInfo->nFrameCount, SigInfo->nErrorCode, SigInfo->strDescription);
			tInfo.strSolution.Format("1 检查相机是否正常工作，如果异常需要断电重启\n2 检查工控机是否运行缓慢，可以重启工控机");
			tInfo.nID = 0;
			theGlobal.m_Log.Alarm(tInfo);
		}
		return;
	}

	static long long sl_Encoder = 0;

	HTuple hv_TimeStart, hv_TimeEnd;
	CountSeconds(&hv_TimeStart);//开始计时
	A_GrabWork tOutPut;
	tOutPut.hvGrabberSN = SigInfo->nGrabberSN;
	GetImage(nGrabberSN, &tOutPut.hoGrabImage);
	tOutPut.hvImageSN = SigInfo->nFrameCount;
	tOutPut.hvFrameSN = GetFrameSN(nGrabberSN);
	tOutPut.hvSpeed = theGlobal.m_Speed.GetSpeed();
	//
	CString strLogP;
	strLogP.Format("PLC信号状态%d-%d-%d",
		theGlobal.m_CommunicationTool.m_nPlcRunDirection, 
		theGlobal.m_CommunicationTool.m_nPlcDieHeadDirection, 
		theGlobal.m_CommunicationTool.m_nPlcStartCoat);
	theGlobal.m_Log.Debug(strLogP);
	//
	tOutPut.hvRunDirection.Append(theGlobal.m_CommunicationTool.m_nPlcRunDirection);
	tOutPut.hvRunDirection.Append(theGlobal.m_CommunicationTool.m_nPlcDieHeadDirection);
	tOutPut.hvRunDirection.Append(theGlobal.m_CommunicationTool.m_nPlcStartCoat);
	HTuple hvHeight;
	GetImageSize(tOutPut.hoGrabImage, NULL, &hvHeight);
	sl_Encoder += hvHeight.I();
	tOutPut.hvEncoder = (double)sl_Encoder*theGlobal.m_vecResolution[0].dRow;

	int nImageSN = tOutPut.hvImageSN;
	if (1 == SigInfo->iGrabberTypeSN)
	{
		tOutPut.hvEncoder = GetEncoder(nGrabberSN);
		//int nReadCount = 28;
		//theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::IO, "READCOUNT", &nReadCount, &nImageSN);
		//tOutPut.hvImageSN = nImageSN;
		if (theGlobal.m_tSystemSet.bGrabAlign)
		{
			if ((0 == nGrabberSN || 1 == nGrabberSN) && theGlobal.m_Project.GetProjectConfig().nCamCount == 4 
				&& theGlobal.m_Project.GetProjectConfig().nSideCount == 2)//四相机，相机01需要和相机2对位
			{
				if (FALSE == AlignCameraImages(tOutPut.hoGrabImage, tOutPut.hvGrabberSN, tOutPut.hvEncoder, tOutPut.hvImageSN))
					return;
			}
			else if (0 == nGrabberSN && theGlobal.m_Project.GetProjectConfig().nCamCount == 2
				&& theGlobal.m_Project.GetProjectConfig().nSideCount == 2)//双相机双面，相机0和相机1位对
			{
				if (FALSE == AlignCameraImages(tOutPut.hoGrabImage, tOutPut.hvGrabberSN, tOutPut.hvEncoder, tOutPut.hvImageSN))
					return;
			}
		}
	}
	CountSeconds(&hv_TimeEnd);//执行结束
	tOutPut.hvTimeStamp = (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000;
	HTuple hv_MessageSend;
	CreateMessage(&hv_MessageSend);
	try
	{
		/////投递消息
		tOutPut.WriteMessage(hv_MessageSend, nGrabberSN);
		try
		{
			EnqueueMessage(hvec_QueueImages[nGrabberSN].T(), hv_MessageSend, HTuple(), HTuple());
		}
		catch (HException& e)
		{
			if (H_ERR_MQOVL == e.ErrorCode())
			{
				CString strException;
				strException.Format("相机%d采集队列已满,该线程已退出,请排查原因", nGrabberSN);
				theGlobal.m_Log.Exception(strException);
			}
			else
			{
				throw  e;
			}
		}
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("T_Grab exceptional(T_Grab异常):%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
	ClearMessage(hv_MessageSend);
	CountSeconds(&hv_TimeEnd);//执行结束

	CString strTime;
	strTime.Format("Acquisition%d:ImageSN:%d Consume%05.03fms", nGrabberSN, SigInfo->nFrameCount, (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000);
	theGlobal.m_Log.Debug(strTime);
}


void* CProcess::HThreadDetect(void *hcthread)
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		const HTupleVector      &QueueImages = hcppthread->GetInputCtrlParamVector(0);
		const HTupleVector      &QueueResults = hcppthread->GetInputCtrlParamVector(1);
		const HTuple			&ThreadIndex = hcppthread->GetInputCtrlParamTuple(2);
		T_Detect(QueueImages, QueueResults, ThreadIndex);
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadDetect is exceptional(HThreadDetect线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
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

void CProcess::T_Detect(HTupleVector hvec_QueueImages, HTupleVector hvec_QueueResults, HTuple hv_ThreadIndex)
{
	HTuple hv_MessageRecv, hv_StopProcInfo, hv_TimeStart, hv_TimeEnd;
	const int nSideSN = hv_ThreadIndex.I();//检测面序号
	const HTuple hvCamSNList = theGlobal.m_Project.GetSideToCamSNList(nSideSN);//检测面关联相机序号
	const HTuple hvMergeSNList = theGlobal.m_Project.GetSideToMergeSNList(nSideSN);//检测面关联合并结果序号
	while (!theGlobal.m_bExitThread)
	{
		//创建输出结构体
		B_DetectWork tOutput;
		tOutput.hv_SideSN = nSideSN;
		for (int i = 0; i < hvCamSNList.Length(); i++)
		{
			DequeueMessage(hvec_QueueImages[hvCamSNList[i]].T(), "timeout", "infinite", &hv_MessageRecv);//从队列里取出一个元素

			A_GrabWork tA;
			tA.ReadMessage(hv_MessageRecv, hvCamSNList[i]);
			tOutput.hv_UsedCamSNList.Append(hvCamSNList[i]);
			tOutput.vecA.push_back(tA);
			ClearMessage(hv_MessageRecv);
		}
		CountSeconds(&hv_TimeStart);

		int nDetectInfoType = MeasureCount;
		WPARAM wParam = MAKEWPARAM(nDetectInfoType, nSideSN);
		LPARAM lParam = MAKELPARAM(0, 0);
		theGlobal.PostMessageToMainDlg(WM_DETECT, wParam, lParam);

		try
		{
			/////////////////////////测量
			MeasureInput  tIn;
			tIn.nMode = theGlobal.GetSystemSet().nActionMode;

			GenEmptyObj(&tIn.hoGrabImage);

			for (int i = 0; i < tOutput.vecA.size(); i++)
			{
				ConcatObj(tIn.hoGrabImage, tOutput.vecA[i].hoGrabImage, &tIn.hoGrabImage);
			}

			//开始检测
			if (TRUE != theGlobal.m_AlgTool.Detect(nSideSN, CAlgorithmTool::AlgMeasure, &tIn, &tOutput.tMeasure))
			{
				CString strException;
				strException.Format("T_Detect%d执行检测失败,请排查原因", nSideSN);
				theGlobal.m_Log.Exception(strException);
				theGlobal.m_Log.Alarm(strException);
				//break;
			}

			CountSeconds(&hv_TimeEnd);
			tOutput.hv_TimeStamp = (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000;
			tOutput.hv_UsedCamSNList = hvCamSNList;
			/////投递消息
			HTuple hvMessageSend;
			CreateMessage(&hvMessageSend);
			tOutput.WriteMessage(hvMessageSend, nSideSN);
			try
			{
				EnqueueMessage(hvec_QueueResults[nSideSN].T(), hvMessageSend, HTuple(), HTuple());
			}
			catch (HException& e)
			{
				if (H_ERR_MQOVL == e.ErrorCode())
				{
					CString strException;
					strException.Format("T_Detect%d结果队列已满,该线程已退出,请排查原因", nSideSN);
					theGlobal.m_Log.Exception(strException);
					theGlobal.m_Log.Alarm(strException);
					break;
				}
				else
				{
					throw  e;
				}
			}
			ClearMessage(hvMessageSend);

		}
		catch (HException&e)
		{
			CString strException;
			strException.Format("T_Detect is exceptional(T_Detect线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
				, e.ProcName().Text());
			theGlobal.m_Log.Exception(strException);
		}

		CountSeconds(&hv_TimeEnd);
		CString strTime;
		strTime.Format("T_Detect%d:ImageSN:%d Consume%05.03fms", nSideSN, tOutput.vecA[0].hvImageSN.I(), (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000);
		theGlobal.m_Log.Debug(strTime);
	}
}

void* CProcess::HThreadMerge(void *hcthread)
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		const HTupleVector  &QueueResults = hcppthread->GetInputCtrlParamVector(0);
		const HTuple        &QueueDisplay = hcppthread->GetInputCtrlParamTuple(1);
		const HTuple        &ThreadIndex = hcppthread->GetInputCtrlParamTuple(2);
		T_Merge(QueueResults, QueueDisplay, ThreadIndex);
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadMerge is exceptional(HThreadMerge线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
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

void CProcess::T_Merge(HTupleVector hvc_QueueResults, HTuple hv_QueueDisplay, HTuple hv_ThreadIndex)
{
	HTuple hv_MessageRecv, hv_StopProcInfo, hv_TimeStart, hv_TimeEnd;
	//合并线程序号
	const int nMergeSN = hv_ThreadIndex.I();
	//合并结果需要的检测面序号
	const HTuple hvSideSNList = theGlobal.m_Project.GetMergeToSideSNList(nMergeSN);

	while (!theGlobal.m_bExitThread)
	{
		//创建输出结构体
		C_MergeWork tOutput;
		tOutput.hv_MergeSN = nMergeSN;
		for (int i = 0; i < hvSideSNList.Length(); i++)
		{
			DequeueMessage(hvc_QueueResults[hvSideSNList[i]].T(), "timeout", "infinite", &hv_MessageRecv);
			B_DetectWork tB;
			tB.ReadMessage(hv_MessageRecv, hvSideSNList[i]);
			tOutput.vecB.push_back(tB);
			tOutput.hv_UsedSideSNList.Append(hvSideSNList[i]);
			ClearMessage(hv_MessageRecv);
		}
		CountSeconds(&hv_TimeStart);
		try
		{
			/////////////////////////综合结果
			tOutput.hv_PackageSN = tOutput.vecB[0].vecA[0].hvFrameSN;
			//合并结果
			theGlobal.m_Judge.Merge(tOutput);
			//数据滤波
			theGlobal.m_DataFilter.Filtering(tOutput);
			//判断测量结果
			theGlobal.m_Judge.Judge(tOutput);

			if (!theGlobal.IsStart() && (theGlobal.GetSystemSet().nActionMode!=SimulateMode)) continue;

			//计算Cpk等数据
			theGlobal.m_DataCalculate.Calculate(tOutput);
			//结果统计
			theGlobal.m_Counter.Count(tOutput);

			CountSeconds(&hv_TimeEnd);
			tOutput.hv_TimeStamp = (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000;

			/////投递消息
			HTuple hvMessageSend;
			CreateMessage(&hvMessageSend);
			try
			{
				//写消息
				tOutput.WriteMessage(hvMessageSend);
				//IO队列入队
				//theGlobal.m_CommunicationTool.Enqueue(CCommunicationTool::IO, hvMessageSend);
				//PLC队列入队，闭环
				theGlobal.m_CommunicationTool.Enqueue(CCommunicationTool::PLC, hvMessageSend);
				//MES队列入队，闭环
				//theGlobal.m_CommunicationTool.Enqueue(CCommunicationTool::MES, hvMessageSend);
				//喷码
				theGlobal.m_CommunicationTool.Mark(tOutput);
				//显示队列入队
				EnqueueMessage(hv_QueueDisplay, hvMessageSend, HTuple(), HTuple());
				//发送消息通知主界面显示
				theGlobal.PostMessageToMainDlg(WM_DISP);
				//存储队列入队
				theGlobal.m_Storage.Enqueue(0, hvMessageSend); 
				//判断是否需要进行报警
				//theGlobal.m_Process.Warning(tOutput);
				//防呆判断
				theGlobal.m_FoolProofing.FoolProofing(tOutput);
				//与其他工位交互数据
				theGlobal.m_Upload.Action(tOutput);
			}
			catch (HException& e)
			{
				if (H_ERR_MQOVL == e.ErrorCode())
				{
					CString strException;
					strException.Format("T_Merge%d显示队列已满,该线程已退出,请排查原因", nMergeSN);
					theGlobal.m_Log.Exception(strException);
					//break;
				}
				/*else
				{
					throw  e;
				}*/
			}
			ClearMessage(hvMessageSend);
		}
		catch (HException&e)
		{
			CString strException;
			strException.Format("T_Merge is exceptional(T_Merge线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
				, e.ProcName().Text());
			theGlobal.m_Log.Exception(strException);
		}

		CountSeconds(&hv_TimeEnd);
		CString strTime;
		strTime.Format("T_Merge%d:ImageSN:%d Consume%05.03fms", nMergeSN, tOutput.vecB[0].vecA[0].hvImageSN.I(), (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000);
		theGlobal.m_Log.Debug(strTime);
	}
}

void* CProcess::HThreadListen(void *hcthread)
{
	// +++ define thread context for this procedure
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		T_Listen();
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadListen is exceptional(HThreadListen线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
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

void CProcess::T_Listen()
{
	static BOOL s_bFirstListen = TRUE;
	static int  s_nListenCount = 0;
	while (!theGlobal.m_bExitThread)
	{
		//写心跳信号
		if (theGlobal.IsStart() && (s_nListenCount++)%4==0) theGlobal.m_CommunicationTool.RefreshRunStatus();

		//速度计算
		if (theGlobal.m_GrabberTool.GetGrabberType(0) != (int)CGrabberTool::SimulateCard)
			theGlobal.m_Speed.CalculateSpeed(theGlobal.m_CommunicationTool.GetIOCardLineRate());

		//删除过期缓存，定时备份数据
		theGlobal.m_Storage.CachesOperate();

		//判断
		theGlobal.m_Upload.ListenStartRunIgnoreCompen();

		//监听设备换卷信号
		theGlobal.m_Process.ListenChangeBatchAuto();

		//获取设备PLC状态
		theGlobal.m_CommunicationTool.GetPlcEquipmentSignal();

		//判断重连PLC通信
		theGlobal.m_CommunicationTool.PLCReconnect();
		Sleep(200);
	}
}

void* CProcess::HThreadSimluateGrab(void *hcthread)
{
	// +++ define thread context for this procedure
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		T_SimluateGrab();
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadSimluateGrab is exceptional(HThreadSimluateGrab线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
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

void CProcess::T_SimluateGrab()
{
	if (theGlobal.m_tSystemSet.nActionMode != SimulateMode) return;
	HTuple hvImageFiles;
	list_image_files(HTuple(theGlobal.m_Process.m_strSimulateGrabPath), "default", HTuple(), &hvImageFiles);
	if (hvImageFiles.Length() == 0)
	{
		AfxMessageBox("路径无仿真图像");
		return;
	}
	//while (!theGlobal.m_Process.m_bSimuateGrabStop)
	{
		HTuple hvInc, hvSelected;
		HTupleVector hvecFilePath(1);
		for (int i = 0; i < hvImageFiles.Length(); i++)
		{
			CString strFilePath = hvImageFiles[i];
			if (strFilePath.Find("Cam0") != -1)
				hvecFilePath[0].T().Append(HTuple(strFilePath));
			else if (strFilePath.Find("Cam1") != -1)
				hvecFilePath[1].T().Append(HTuple(strFilePath));
			else if (strFilePath.Find("Cam2") != -1)
				hvecFilePath[2].T().Append(HTuple(strFilePath));
			else if (strFilePath.Find("Cam3") != -1)
				hvecFilePath[3].T().Append(HTuple(strFilePath));
		}
		
		if (hvecFilePath.Length() == 0 || hvecFilePath[0].Length() != hvecFilePath[hvecFilePath.Length() - 1].Length())
		{
			AfxMessageBox("路径不相符");
			return;
		}
		for (int i = 0; i < hvecFilePath[0].T().Length(); i++)
		{
			if (theGlobal.m_Process.m_bSimuateGrabStop) return;
			for (int j = 0; j < hvecFilePath.Length(); j++)
			{
				try
				{
					HTuple hv_TimeEnd, hv_TimeStart;
					CountSeconds(&hv_TimeStart);
					A_GrabWork tOutPut;
					ReadImage(&tOutPut.hoGrabImage, hvecFilePath[j].T()[i]);
					tOutPut.hvGrabberSN = j;

					tOutPut.hvImageSN = i;
					tOutPut.hvFrameSN = i;
					tOutPut.hvSpeed = 0;

					CountSeconds(&hv_TimeEnd);//执行结束
					tOutPut.hvTimeStamp = (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000;
					HTuple hv_MessageSend;
					CreateMessage(&hv_MessageSend);
					try
					{
						/////投递消息
						tOutPut.WriteMessage(hv_MessageSend, j);
						try
						{
							EnqueueMessage(theGlobal.m_Process.GetImageQueue(j), hv_MessageSend, HTuple(), HTuple());
						}
						catch (HException& e)
						{
							if (H_ERR_MQOVL == e.ErrorCode())
							{
								CString strException;
								strException.Format("T_SimluateGrab: 相机%d采集队列已满,该线程已退出,请排查原因", j);
								theGlobal.m_Log.Exception(strException);
							}
							else
							{
								throw  e;
							}
						}
					}
					catch (HException& e)
					{
						CString strException;
						strException.Format("T_SimluateGrab exceptional(T_SimluateGrab异常):%s  [ %s() ]", e.ErrorMessage().Text()
							, e.ProcName().Text());
						theGlobal.m_Log.Exception(strException);
					}
					ClearMessage(hv_MessageSend);
				}
				catch (HException&)
				{

				}
			}
			Sleep(theGlobal.m_Process.m_nSimulateGrabTime);
		}
	}
}

BOOL CProcess::AlignCameraImages(HObject& ho_GrabImage, HTuple& hv_GrabberSN, HTuple& hv_Encoder, HTuple& hv_ImageSN)
{
#pragma region    "内存对位"
	int  nGrabberSN = hv_GrabberSN;
	__int64 nAddr(0), nImageSize(0), nPixelSize(0), nImageWidth(0), nImageHeight(0), nGrabberType(0);
	GBParamID Param = GBImageBufferAddr;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nAddr);
	Param = GBImageBufferSize;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nImageSize);
	Param = GBImagePixelSize;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nPixelSize);
	Param = GBImageWidth;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nImageWidth);
	Param = GBImageHeight;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nImageHeight);
	Param = GBGrabberTypeSN;
	theGlobal.m_GrabberTool.DoSomething(nGrabberSN, "GETPARAM", &Param, &nGrabberType);
	static int nStart[2] = { 0,0 };
	static Hlong lImageSN[2] = { 0,0 };
	if (0 == nStart[hv_GrabberSN.I()])
	{
		m_pBufferFirstImage[hv_GrabberSN.I()] = new BYTE[2 * nImageSize];
		m_pBufferNextImage[hv_GrabberSN.I()] = m_pBufferFirstImage[hv_GrabberSN.I()] + nImageSize;
		memset(m_pBufferFirstImage[hv_GrabberSN.I()], 0, 2 * nImageSize);
		nStart[hv_GrabberSN.I()]++;
	}
	else
	{
		memcpy(m_pBufferFirstImage[hv_GrabberSN.I()], m_pBufferNextImage[hv_GrabberSN.I()], nImageSize);
		memcpy(m_pBufferNextImage[hv_GrabberSN.I()], (BYTE*)nAddr, nImageSize);
	}
	static long long nThrowLine[2] = { 0,0 };//已经丢弃的行数
	static long long nCutLine[2] = { -1,-1 };//寻找出的裁切行的标志行
	static HObject ho_Top, ho_Bottom;
	static int nDistanceCamera[2] = { theGlobal.m_tSystemSet.nCameraDistance,theGlobal.m_tSystemSet.nCameraDistance };
	if (nThrowLine[hv_GrabberSN.I()] + nImageHeight < theGlobal.m_tSystemSet.nCameraDistance && nCutLine[hv_GrabberSN.I()] == -1)//如果丢弃的行数还没到设置的相机距离
	{
		nThrowLine[hv_GrabberSN.I()] = nThrowLine[hv_GrabberSN.I()] + nImageHeight;
		return FALSE;
	}
	else//已经到了拼接点了
	{
		hv_ImageSN = HTuple(++lImageSN[hv_GrabberSN.I()]);
		if (-1 == nCutLine[hv_GrabberSN.I()])//还没有得出拼接点
		{
			nCutLine[hv_GrabberSN.I()] = theGlobal.m_tSystemSet.nCameraDistance - nThrowLine[hv_GrabberSN.I()];
			if (nCutLine[hv_GrabberSN.I()] == nImageHeight)//如果正好整除,那么为了保存程序正常运行，这里-1
			{
				nCutLine[hv_GrabberSN.I()] -= 1;
			}
			m_pBufferTilePostion[hv_GrabberSN.I()] = m_pBufferFirstImage[hv_GrabberSN.I()] + nCutLine[hv_GrabberSN.I()] * nImageWidth;
			return FALSE;
		}
		else
		{
			GenImage1(&ho_GrabImage, "byte", (Hlong)nImageWidth, (Hlong)nImageHeight, (Hlong)(__int64)m_pBufferTilePostion[hv_GrabberSN.I()]);
			hv_Encoder = hv_Encoder - ((nImageHeight - nCutLine[hv_GrabberSN.I()]) + theGlobal.m_tSystemSet.nCameraDistance)*theGlobal.m_vecResolution[0].dRow;
			return TRUE;
		}
	}
#pragma endregion
}

void CProcess::StartSimulateGrab()
{
	m_bSimuateGrabStop = FALSE;


	HDevThread*         pHDThread;
	HDevThreadContext   hTContext;

	pHDThread = new HDevThread(hTContext,
		(void*)HThreadSimluateGrab, 3, 0);
	pHDThread->Start();
}

void CProcess::ListenChangeBatchAuto()
{
	if (!theGlobal.m_tSystemSet.bUseChangeBatchAuto || theGlobal.m_bExitThread) return;

	CString strBatchName = "";
	CString strProductType = "";
	CString strSoftwareVersion = "";
	//测试用
		/*static int nBatchName = 0;
	SYSTEMTIME systime;
	::GetSystemTime(&systime);
	static int nTime = systime.wMinute;
	if ((systime.wMinute - nTime) > 20)
	{
		nBatchName++;
		nTime = systime.wMinute;
	}
	strBatchName.Format("%d", nBatchName);*/

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		//获取卷号
		BOOL bRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "ReadBatchName", NULL, &strBatchName);
		if (!bRet) return;

		if (strBatchName == theGlobal.m_Counter.GetCurrentBatch().strName)
		{
			//自动上传膜卷号
			BOOL bWriteBatch= theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "WriteBatchNameVerify", NULL, &strBatchName);
			if (!bWriteBatch)
			{
				CString strLog;
				strLog.Format("上传膜卷号失败，当前膜卷号：%s", theGlobal.m_Counter.GetCurrentBatch().strName);
				theGlobal.m_Log.Communication(strLog);
			}

			//获取品种号
			BOOL bProductType= theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetProductTypeName", NULL, &strProductType);
			if (!bProductType)
			{
				CString strLog;
				strLog.Format("获取品种号失败");
				theGlobal.m_Log.Communication(strLog);
			}
			else
			{
				//上传品种号
				BOOL bWriteProductTypeRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "WriteProductTypeVerify", NULL, &strProductType);
				if (!bWriteProductTypeRet)
				{
					CString strLog;
					strLog.Format("上传品种号失败，当前品种号：%s", theGlobal.m_ProductTool.GetCurrentProductName());
					theGlobal.m_Log.Communication(strLog);
				}
			}
			
			//上传软件版本号
			strSoftwareVersion = theGlobal.GetVersion();
			BOOL bWriteSoftwareVersionRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "WriteSoftwareVersionVerify", NULL, &strSoftwareVersion);
			if (!bWriteSoftwareVersionRet)
			{
				CString strLog;
				strLog.Format("上传软件版本号失败，当前软件版本号：%s", theGlobal.m_strVersion);
				theGlobal.m_Log.Communication(strLog);
			}
			return;
		}
	}
	else
	{
		strBatchName = theGlobal.m_Upload.GetBatchRecv();
		if (strBatchName == theGlobal.m_Counter.GetCurrentBatch().strName) return;
	}
	//开始判断延迟距离，换卷
	static double s_dDelayMeter = 0;
	static BOOL bChangeBatch = TRUE;
	if (bChangeBatch)
	{
		bChangeBatch = FALSE;
		s_dDelayMeter = theGlobal.m_Counter.GetCurrentMeterCount();
	}
	if (theGlobal.m_Counter.GetCurrentMeterCount() < s_dDelayMeter + theGlobal.m_tSystemSet.dChangeBatchDelayMeter) return;

	bChangeBatch = TRUE;

	theGlobal.m_DataCalculate.FinishCalculate();
	theGlobal.m_Counter.EndBatch();
	theGlobal.m_Counter.StartBatch(strBatchName);
	theGlobal.m_Storage.GenDataFileSavePath();
	theGlobal.m_Counter.WriteParam();

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
		theGlobal.m_Upload.SendBatchToClient(strBatchName);//把卷号发送给机尾和涂2

	//设置喷码换卷
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Marking, "SetCurrentBatch", &strBatchName, NULL);

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.bCheckProductInfoByChangeBatch)
	{
		//多工位配方校验
		//theGlobal.m_Upload.SendProductInfoToClient();
		theGlobal.SetTipInfo(TIP_CheckProduct);
	}

	CString strLog;
	strLog.Format("已换卷,卷号:%s,换卷延迟:%0.3fm", strBatchName, theGlobal.m_tSystemSet.dChangeBatchDelayMeter);
	theGlobal.m_Log.Std(strLog);
}