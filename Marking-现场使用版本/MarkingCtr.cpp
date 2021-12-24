#include "StdAfx.h"
#include "MarkingCtr.h"
#include <math.h>
#include <vector>
#include "DlgMarkingParamSet.h"

using std::vector;
extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CMarkingCtr();
}

CMarkingCtr* g_This = NULL;
//CCriticalSection CMarkingCtr::m_csLock;

CMarkingCtr::CMarkingCtr()
	:m_hvec_Data(1)
{
	g_This = this;
	m_bIsConnected = FALSE;
	m_vMarkingRes.clear();
	m_nNGCount = 0;
	InitializeCriticalSection(&m_csLock);
	initNgDescribe();//喷码器NG对照表	
	m_vResolutionList.clear();
	t_CameraBaseMsg.dHResolutionSideA = 0;
	t_CameraBaseMsg.dHResolutionSideB = 0;
	t_CameraBaseMsg.dVResolutionSideA = 0;
	t_CameraBaseMsg.dVResolutionSideB = 0;
	t_CameraBaseMsg.hv_ColumnOverA.Clear();
	t_CameraBaseMsg.hv_ColumnOverB.Clear();
	t_CameraBaseMsg.nCameraCount = 0;

	vACameraSnList.clear();
	vBCameraSnList.clear();

	GenEmptyObj(&m_ho_RegionSegment);
}

CMarkingCtr::~CMarkingCtr()
{

}

int CMarkingCtr::DoSomething(void* message, void* wParam, void* lParam)
{
	CString strMessage;
	strMessage.Format("%s", (char*)message);
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage)
	{
		return Init(*(PMARKINGINIT)wParam);
	}
	else if ("PUSH_MESSAGE" == strMessage || "Push_Message" == strMessage)
	{
		return PushMessage(*(HTuple*)wParam);
	}
	else if ("SHOW" == strMessage || "Show" == strMessage)
	{
		return ShowParamDlg();
	}
	else if ("GETCONNECTION" == strMessage || "GetConnection" == strMessage)
	{
		return GetIsConnected(*(int*)lParam);
	}
	else if ("RECONNECT" == strMessage || "Reconnect" == strMessage)
	{
		return SocketReconnect();
	}
	else if ("DISCONNECT" == strMessage || "DisConnect" == strMessage)
	{
		return SocketDisconnect();
	}
	else if ("SENDDATA" == strMessage || "SendData" == strMessage)
	{
		return SendResultToMarking(*(C_MergeWork*)wParam);//313 指针改成引用类型
	}
	else if ("SENDSTR" == strMessage || "SendStr" == strMessage)
	{
		return SendStrToMarking(*(CString*)wParam);
	}
	else if ("ReceiveDATA" == strMessage || "ReceiveData" == strMessage)
	{
		return ReceiveStringResult(*(CString*)lParam);
	}
	else if ("SETCURRENTBATCH" == strMessage || "SetCurrentBatch" == strMessage)
	{
		return SetCurrentBatch(*(CString*)wParam);
	}
	return TRUE;
}

int CMarkingCtr::Init(MARKINGINIT& Init)
{
	try
	{
		m_sInit = Init;

		CString strFilePath;
		strFilePath.Format("%s\\Config\\Public\\Marking", GetCurrentAppPath());
		if (!PathFileExists(strFilePath))
			CreateMultilevelPath(strFilePath);

		m_strConfigMarkingFile.Format("%s\\Config\\Public\\Marking\\Marking.ini", GetCurrentAppPath());
		ReadParam();

		if (!SocketConnect(m_tSocketSet.strIP, m_tSocketSet.nPort))
		{
			Log("CMarkingCtr:MarkingCtr 连接失败,请检查网络", ExcpLog);
		}

		//m_sInit.Main(MT_Marking, "GetCurrentBatch", NULL, &m_strCurrentBatch, NULL);

		GetCameraSetting(m_vResolutionList, t_CameraBaseMsg);

		//if (m_sInit.tPdInfo.nCoatingType == TwoSide)
		//{
		//	ho_Images.resize(3);
		//}
		//else
		//{
		//	ho_Images.resize(1);
		//}
#pragma region "创建队列 发送NG结果位置的queue"
		CreateMessageQueue(&m_hvDefectToMarkigQueue);
		SetMessageQueueParam(m_hvDefectToMarkigQueue, "max_message_num", 25);

		HDevThreadContext Context;
		HDevThread* pThread = new HDevThread(Context,
			(void*)HThreadMarking, 0, 0);//开启一个具有2个输入的线程;
		HTuple ProcThreads;
		pThread->ParStart(&ProcThreads);
#pragma endregion

		return TRUE;
	}
	catch (HException& e)
	{
		//打印异常通讯日志
		CString strException;
		strException.Format("CMarkingCtr:SocketConnect接口出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);

		return FALSE;
	}
}

//获取当前卷号，用于命名瑕疵文件
BOOL CMarkingCtr::SetCurrentBatch(CString strBatch)
{
	//获取膜卷号
	m_strCurrentBatch.Format("%s", strBatch);

	return TRUE;
}

//相机相关信息获取
BOOL CMarkingCtr::GetCameraSetting(vector<tagResolution>& vResolutionList, tagCameraBaseMsg& tCameraBaseMsg)
{
	try
	{
		//*从配置文件获取单面关联的相机序号，进行初始化

		CString m_strSystemFilePath;
		m_strSystemFilePath.Format("%s/Config/System.vis", GetCurrentAppPath());
		tCameraBaseMsg.nCameraCount = GetPrivateProfileInt("Project", "GrabberCount", 4, m_strSystemFilePath);

		CString strFileResolution;
		vResolutionList.resize(tCameraBaseMsg.nCameraCount);

		for (int i = 0; i < tCameraBaseMsg.nCameraCount; i++)
		{
			strFileResolution.Format("%s\\Config\\Public\\Calibration\\%d\\CameraParma.ini", GetCurrentAppPath(), i);
			vResolutionList[i].dRow = GetPrivateProfileDouble("CalibrationParam", "YResolution", 0.03, strFileResolution);
			vResolutionList[i].dColumn = GetPrivateProfileDouble("CalibrationParam", "XResolution", 0.03, strFileResolution);
		}
		//*获取相机分辨率(按面划分)
		GetCameraResolution(vResolutionList, tCameraBaseMsg);

		//*获取A/B面相机的拼图的公共区域
		CString strColumnOverFilePath;
		strColumnOverFilePath.Format("%s\\Config\\Public\\Calibration\\0\\hvColumnOver.tup", GetCurrentAppPath());
		HALCON_TRY(ReadTuple(HTuple(strColumnOverFilePath), &tCameraBaseMsg.hv_ColumnOverA));
		if (m_sInit.tPdInfo.nCoatingType == TwoSide)
		{
			strColumnOverFilePath.Format("%s\\Config\\Public\\Calibration\\1\\hvColumnOver.tup", GetCurrentAppPath());
			HALCON_TRY(ReadTuple(HTuple(strColumnOverFilePath), &tCameraBaseMsg.hv_ColumnOverB));
		}
	}
	catch (HException&e)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
//获取相机分辨率(按面划分) :参数1：相机个数，参数2：分辨率列表
BOOL CMarkingCtr::GetCameraResolution(vector<tagResolution>& vResolutionList, tagCameraBaseMsg& tCameraBaseMsg)
{
	try
	{
		if (m_sInit.tPdInfo.nCoatingType == OneSide)
		{
			//获取相机的序号列表
			m_sInit.Main(MT_Marking, "GetSideToCamSNList", "Single", &vACameraSnList, NULL);

			//根据相机数量，计算单个面的分辨率(暂时按单面的平均分辨率)
			for (int i = 0; i < vACameraSnList.size(); i++)
			{
				//水平方向
				tCameraBaseMsg.dHResolutionSideA += vResolutionList[vACameraSnList[i]].dColumn;
				tCameraBaseMsg.dHResolutionSideB = 0;
				//垂直方向
				tCameraBaseMsg.dVResolutionSideA += vResolutionList[vACameraSnList[i]].dRow;
				tCameraBaseMsg.dVResolutionSideB = 0;
			}

			tCameraBaseMsg.dHResolutionSideA /= vACameraSnList.size();
			tCameraBaseMsg.dHResolutionSideB = 0;

			tCameraBaseMsg.dVResolutionSideA /= vACameraSnList.size();
			tCameraBaseMsg.dVResolutionSideB = 0;
		}
		else if (m_sInit.tPdInfo.nCoatingType == TwoSide)
		{
			m_sInit.Main(MT_Marking, "GetSideToCamSNList", "Double", &vACameraSnList, &vBCameraSnList);
			//A面
			for (int i = 0; i < vACameraSnList.size(); i++)
			{
				//水平方向
				tCameraBaseMsg.dHResolutionSideA += vResolutionList[vACameraSnList[i]].dColumn;
				//垂直方向
				tCameraBaseMsg.dVResolutionSideA += vResolutionList[vACameraSnList[i]].dRow;
			}
			//A面相机分辨率
			tCameraBaseMsg.dHResolutionSideA /= vACameraSnList.size();//水平方向		
			tCameraBaseMsg.dVResolutionSideA /= vACameraSnList.size();//垂直方向

			//B面
			for (int i = 0; i < vBCameraSnList.size(); i++)
			{
				//水平方向
				tCameraBaseMsg.dHResolutionSideB += vResolutionList[vBCameraSnList[i]].dColumn;
				//垂直方向
				tCameraBaseMsg.dVResolutionSideB += vResolutionList[vBCameraSnList[i]].dRow;
			}
			//B面相机分辨率
			tCameraBaseMsg.dHResolutionSideB /= vBCameraSnList.size();//水平方向	
			tCameraBaseMsg.dVResolutionSideB /= vBCameraSnList.size();//垂直方向
		}
	}
	catch (HException& e)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

void CMarkingCtr::ReadParam()
{
	GetPrivateProfileString("Server", "IP", "192.168.0.150", m_tSocketSet.strIP.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigMarkingFile);
	m_tSocketSet.strIP.ReleaseBuffer();//读取产品名称
	m_tSocketSet.nPort = GetPrivateProfileInt("Server", "Port", 60009, m_strConfigMarkingFile);
	m_tSocketSet.nUseCommu = GetPrivateProfileInt("Config", "UseCmmu", FALSE, m_strConfigMarkingFile);
	m_tSocketSet.nUseCheckDefeat = GetPrivateProfileInt("Config", "UseCheckDefeat", FALSE, m_strConfigMarkingFile);
	m_tSocketSet.nUseCheckSize = GetPrivateProfileInt("Config", "UseCheckSize", FALSE, m_strConfigMarkingFile);
	m_tSocketSet.nImgDiff = GetPrivateProfileInt("Config", "Diff", 3, m_strConfigMarkingFile);
	m_tSocketSet.nMarkingWidth = GetPrivateProfileInt("Config", "MarkingWidth", 1260, m_strConfigMarkingFile);
	m_tSocketSet.nMarkingHeight = GetPrivateProfileInt("Config", "MarkingHeight", 200, m_strConfigMarkingFile);

	m_tSocketSet.nDivImageNum = GetPrivateProfileInt("Config", "DivImageNum", 2, m_strConfigMarkingFile);//每张图划分为2个区域
}

void CMarkingCtr::WriteParam()
{
	//初始化保存设置
	WritePrivateProfileString("Server", "IP", m_tSocketSet.strIP, m_strConfigMarkingFile);
	WritePrivateProfileInt("Server", "Port", m_tSocketSet.nPort, m_strConfigMarkingFile);
	WritePrivateProfileInt("Config", "UseCmmu", m_tSocketSet.nUseCommu, m_strConfigMarkingFile);
	WritePrivateProfileInt("Config", "UseCheckDefeat", m_tSocketSet.nUseCheckDefeat, m_strConfigMarkingFile);
	WritePrivateProfileInt("Config", "UseCheckSize", m_tSocketSet.nUseCheckSize, m_strConfigMarkingFile);
	WritePrivateProfileInt("Config", "Diff", m_tSocketSet.nImgDiff, m_strConfigMarkingFile);
	WritePrivateProfileInt("Config", "MarkingWidth", m_tSocketSet.nMarkingWidth, m_strConfigMarkingFile);
	WritePrivateProfileInt("Config", "MarkingHeight", m_tSocketSet.nMarkingHeight, m_strConfigMarkingFile);

	WritePrivateProfileInt("Config", "DivImageNum", m_tSocketSet.nDivImageNum, m_strConfigMarkingFile);
}

BOOL CMarkingCtr::Log(const CString& strLog, const int& nType, const int&nExcpLevel)
{
	if (m_sInit.Log == NULL) return FALSE;
	return m_sInit.Log(strLog, nType, nExcpLevel);
}

BOOL CMarkingCtr::ShowParamDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgMarkingParamSet dlg;
	dlg.m_pOwner = this;
	dlg.DoModal();
	return TRUE;
}

BOOL CMarkingCtr::SocketConnect(const CString& strIP, const int& nPort)
{
	if (!m_tSocketSet.nUseCommu) return FALSE;
	try
	{
		OpenSocketConnect(HTuple(strIP), nPort, HTuple("protocol").Append("timeout"), HTuple("TCP").Append(0.3), &m_hv_Socket);
		m_bIsConnected = TRUE;
		return TRUE;
	}
	catch (HException& e)
	{
		try
		{
			CloseSocket(m_hv_Socket);
			m_bIsConnected = FALSE;
		}
		catch (HException) {}
		m_hv_Socket.Clear();

		//打印异常通讯日志
		//CString strException;
		//strException.Format("CMarkingCtr:SocketConnect接口出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
		//	, e.ProcName().Text());
		//Log(strException, ExcpLog);

		return FALSE;
	}
}

BOOL CMarkingCtr::SocketDisconnect()
{
	try
	{
		CloseSocket(m_hv_Socket);
		m_hv_Socket.Clear();
		m_bIsConnected = FALSE;
		return TRUE;
	}
	catch (HException& e)
	{
		//打印异常通讯日志
		CString strException;
		strException.Format("CMarkingCtr:SocketDisconnect接口出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);

		m_bIsConnected = FALSE;
		return FALSE;
	}
}

BOOL CMarkingCtr::SocketReconnect()
{
	try
	{
		if (FALSE == m_bIsConnected)
		{
			SocketDisconnect();
			return SocketConnect(m_tSocketSet.strIP, m_tSocketSet.nPort);
		}
	}
	catch (HException& e)
	{
		//打印异常通讯日志
		//CString strException;
		//strException.Format("CMarkingCtr:SocketReconnect接口出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
		//	, e.ProcName().Text());
		//Log(strException, ExcpLog);

		m_bIsConnected = FALSE;
		return FALSE;
	}
	return FALSE;
}

BOOL CMarkingCtr::GetIsConnected(int& res)
{
	res = m_bIsConnected;
	return TRUE;
}

int CMarkingCtr::PushMessage(const HTuple& hvMessageSend)//放置消息进入队列
{
	if (!m_tSocketSet.nUseCommu)
	{
		return TRUE;
	}

	try
	{
		EnqueueMessage(m_hvDefectToMarkigQueue, hvMessageSend, HTuple(), HTuple());//将消息投递到瑕疵结果队列
	}
	catch (HException& e)
	{
		if (H_ERR_MQOVL == e.ErrorCode())
		{
			CString strException;
			strException.Format("Marking队列已满,整合线程已退出,请排查原因");
			Log(strException, ExcpLog);
		}
		else
		{
			throw e;
		}
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

//获取水平方向分区边缘信息和新分区
BOOL CMarkingCtr::GetPartitionInHorizontalDirection(C_MergeWork& Result, vector<PARTITIONMSG>& vPartitionMsg)
{
	try
	{

		for (int sideSN = 0; sideSN < Result.vecB.size(); sideSN++)
		{
			HTuple foilRow1, foilCol1, foilRow2, foilCol2;
			if (Result.vecB[sideSN].tMeasure.tFoil.ho_Region.CountObj() > 0)
			{
				SmallestRectangle1(Result.vecB[sideSN].tMeasure.tFoil.ho_Region, &foilRow1, &foilCol1, &foilRow2, &foilCol2);
			}
			//*获取极耳区域中心(中间极耳需要从中间分开，两边的不用)	
			for (int x = 0; x < foilCol1.Length(); x++)
			{
				if ((x == 0))
				{
					vPartitionMsg[sideSN].hv_PartEdgeCol.Append(foilCol1[x].D());
				}
				else if (x == foilCol1.Length() - 1)
				{
					vPartitionMsg[sideSN].hv_PartEdgeCol.Append(foilCol2[x].D());
				}
				else
				{
					vPartitionMsg[sideSN].hv_PartEdgeCol.Append((foilCol2[x].D() + foilCol1[x].D()) / 2);
				}
			}

			//*获取涂布区域中心(涂布都需要中间分开)
			HTuple coatRow1, coatCol1, coatRow2, coatCol2;
			SmallestRectangle1(Result.vecB[sideSN].tMeasure.tCoat.ho_Region, &coatRow1, &coatCol1, &coatRow2, &coatCol2);
			for (int z = 0; z < coatCol1.Length(); z++)
			{
				vPartitionMsg[sideSN].hv_PartEdgeCol.Append((coatCol2[z].D() + coatCol1[z].D()) / 2);
			}
			//*分区边缘排序
			vPartitionMsg[sideSN].hv_PartEdgeCol = vPartitionMsg[sideSN].hv_PartEdgeCol.TupleSort();

			//生成分区区域
			HTuple hvCol1, hvCol2, hvHeight, hvRow1, hvRow2;
			TupleRemove(vPartitionMsg[sideSN].hv_PartEdgeCol, vPartitionMsg[sideSN].hv_PartEdgeCol.Length() - 1, &hvCol1);
			TupleRemove(vPartitionMsg[sideSN].hv_PartEdgeCol, 0, &hvCol2);
			GetImageSize(Result.vecB[sideSN].tMeasure.ho_DispImage, &HTuple(), &hvHeight);
			TupleGenConst(hvCol1.Length(), 0, &hvRow1);
			TupleGenConst(hvCol1.Length(), hvHeight - 1, &hvRow2);
			GenRectangle1(&vPartitionMsg[sideSN].ho_PartAll, hvRow1, hvCol1, hvRow2, hvCol2);
		}
	}
	catch (HException& e)
	{
		//打印异常通讯日志
		CString strException;
		strException.Format("CMarkingCtr:GetRegionOfPartition异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);
		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

//获取瑕疵NG相关数据(目前适用有凹版的情况)
BOOL CMarkingCtr::GetDefectInNewRegion(C_MergeWork& Result, int SideSN, vector<PARTITIONMSG>& vPartitionMsg, vector<tagDefect>& mtagDefccts)
{
	try
	{
		//增加：检测涂布一半尺寸为0，说明没有涂，这时不进行瑕疵检测	
		int num = 0;
		if (Result.vecB[SideSN].tMeasure.hv_HasCoat.I() == FALSE) return FALSE;
		for (int j = 0; j < Result.vecB[SideSN].tMeasure.tCoat.hv_Width.Length(); j++)
		{
			if (Result.vecB[SideSN].tMeasure.tCoat.hv_Width[j].D() > 0)
				continue;

			++num;
		}

		if (num > (Result.vecB[SideSN].tMeasure.tCoat.hv_Width.Length() / 2))
		{
			return FALSE;//*情况0：X面默认无涂膜，不检测瑕疵
		}

		HTuple hv_wtdth, hv_height;
		int unitHeight = m_tSocketSet.nMarkingHeight;
		GetImageSize(Result.vecB[SideSN].tMeasure.ho_DispImage, &hv_wtdth, &hv_height);
		int nHeight = hv_height.I() * t_CameraBaseMsg.dVResolutionSideA;
		m_tSocketSet.nDivImageNum = (int)(nHeight / unitHeight) + ((nHeight%unitHeight) > 0 ? 1 : 0);

		CString strCamSN; //相机下标

		//重新汇总所有区域的瑕疵+类型，统一处理。一出三单独考虑
		int len = Result.vecB[SideSN].tMeasure.tDefect.ho_Region.CountObj(); //所有瑕疵项的数量
		for (int i = 0; i < len; i++)
		{
			int stripSN = Result.vecB[SideSN].tMeasure.tDefect.hv_StripeSN[i].I();
			int type = Result.vecB[SideSN].tMeasure.tDefect.hv_Type[i].I();
			HObject hoDefect;
			SelectObj(Result.vecB[SideSN].tMeasure.tDefect.ho_Region, &hoDefect, i + 1);
			HTuple hvRow1, hvRow2, hvCol1, hvCol2;
			SmallestRectangle1(hoDefect, &hvRow1, &hvCol1, &hvRow2, &hvCol2);

			HTuple hvInc, hvGreat;
			TupleGreaterEqualElem(vPartitionMsg[SideSN].hv_PartEdgeCol, hvCol1, &hvGreat);
			TupleFindFirst(hvGreat, 1, &hvInc);
			if (hvInc.Length() == 0 || hvInc == -1) continue;
			if (hvInc.I() > mtagDefccts.size()) continue;

			int k = hvInc.I() - 1;
			double dRow1 = Result.vecB[SideSN].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Row[i].D();
			for (int nDivNum = 0; nDivNum < m_tSocketSet.nDivImageNum; nDivNum++)
			{
				mtagDefccts[k].nType.push_back(type);
				if ((nDivNum + 1)*m_tSocketSet.nMarkingHeight >= Result.vecB[SideSN].tMeasure.tDefect.tFeature.tRect.tRightBottom.hv_Row[i].D())
				{
					mtagDefccts[k].nRow1.push_back(dRow1);
					mtagDefccts[k].nRow2.push_back(Result.vecB[SideSN].tMeasure.tDefect.tFeature.tRect.tRightBottom.hv_Row[i].D());
				}
				else
				{
					mtagDefccts[k].nRow1.push_back(dRow1);
					mtagDefccts[k].nRow2.push_back((nDivNum + 1)*m_tSocketSet.nMarkingHeight);
				}
				dRow1 = (nDivNum + 1)*m_tSocketSet.nMarkingHeight;

				mtagDefccts[k].nCol1.push_back(Result.vecB[SideSN].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Column[i].D());
				mtagDefccts[k].nCol2.push_back(Result.vecB[SideSN].tMeasure.tDefect.tFeature.tRect.tRightBottom.hv_Column[i].D());
			}
		}
	}
	catch (HException& e)
	{
		//打印异常通讯日志
		CString strException;
		strException.Format("CMarkingCtr:GetDefeatInNewRegion异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);
		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

//获取尺寸NG相关数据
BOOL CMarkingCtr::GetDefectSize(C_MergeWork& Result, vector<PARTITIONMSG>& vPartitionMsg, vector<tagDefect>& mtagDefects)
{
	try
	{
		//增加：检测涂布一半尺寸为0，说明没有涂，这时不进行尺寸检测	
		int num = 0;
		bool checkSideA = true;
		bool checkSideB = true;
		for (int i = 0; i < Result.vecB.size(); i++)
		{
			num = 0;
			int iit = Result.vecB[i].tMeasure.tCoat.hv_Width.Length();
			for (int j = 0; j < Result.vecB[i].tMeasure.tCoat.hv_Width.Length(); j++)
			{
				if (Result.vecB[i].tMeasure.tCoat.hv_Width[j].D() > 0)
					continue;

				++num;
			}

			if (num > Result.vecB[i].tMeasure.tCoat.hv_Width.Length() / 2)
			{
				if (i == 0)
					checkSideA = false;
				else if (i == 1)
					checkSideB = false;
			}
		}

		//*重新汇总所有区域的瑕疵+类型，统一处理。一出三暂时没考虑
		int len = Result.tMerge.hv_NGStripeSN.Length();
		NEWREGIONMSG newRegionMsg;
		GenEmptyObj(&newRegionMsg.ho_NewtRegion);
		for (int i = 0; i < len; i++)
		{
			int sideSN = Result.tMerge.hv_NGSideSN[i].I();

			if (sideSN == 0 && !checkSideA)
			{
				continue;//*情况1：A面默认无涂膜，不检测尺寸
			}
			else if (sideSN == 1 && !checkSideB)
			{
				continue;//*情况2：B面默认无涂膜，不检测尺寸
			}

			int stripSN = Result.tMerge.hv_NGStripeSN[i].I();

			if (!m_sInit.tPdInfo.tParam.bUseFoilWidth && (stripSN % 4 == 0))
				continue;
			else if (!m_sInit.tPdInfo.tParam.bUseCoatWidth && (stripSN == 2 || stripSN == 6 || stripSN == 10 || stripSN == 14 || stripSN == 18))
				continue;
			else if (!(m_sInit.tPdInfo.bExistBaseCoat || m_sInit.tPdInfo.bExistAT11) && !(m_sInit.tPdInfo.tParam.bUseBaseCoatWidth || m_sInit.tPdInfo.tParam.bUseAT11Width) && (stripSN % 2 != 0))
				continue;

			int type = Result.tMerge.hv_NGType2[i].I();
		}
	}
	catch (HException& e)
	{
		//打印异常通讯日志
		CString strException;
		strException.Format("CMarkingCtr:GetDefeatSize异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);

		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

//发送NG结果(瑕疵 + 尺寸)字符串给喷码机
BOOL CMarkingCtr::SendResultToMarking(C_MergeWork& Result)
{
	try
	{
		//需要先建立连接
		if (!m_tSocketSet.nUseCommu || !m_bIsConnected) return FALSE;
		if (!m_tSocketSet.nUseCheckDefeat && !m_tSocketSet.nUseCheckSize)
		{
			return FALSE;
		}
		int nImgNum = Result.vecB[0].vecA[0].hvImageSN.I();  //图像号
		int count = 0;//瑕疵数量统计
		int num = Result.vecB.size();

		double dSpeed = 0;
		m_sInit.Main(MT_Marking, "ReadSpeed", NULL, &dSpeed, NULL);

		MARKINGRES tMarkingRes;
		tMarkingRes.nImgNum = nImgNum; //图像序号
		tMarkingRes.nImgSNTarget = nImgNum + m_tSocketSet.nImgDiff;
		tMarkingRes.sResult = "";
		tMarkingRes.dSpeed = dSpeed;
		tMarkingRes.dTotalMeter = Result.hv_MeterCount.D();
		tMarkingRes.vSaveResult.clear();

		//1.获取横向分区相关信息
		vector<PARTITIONMSG> m_vPartitionMsg(num);
		GetPartitionInHorizontalDirection(Result, m_vPartitionMsg);

		//2.汇总各分区瑕疵数据，并获取结果信息
		//分区数暂不考虑一出三
		int nPatitionNum = m_sInit.tPdInfo.nPDType * 2;
		vector<tagDefect> m_tagDefects(nPatitionNum);

		if (m_tSocketSet.nUseCheckDefeat)
		{
			for (int i = 0; i < num; i++)
			{
				//*获取瑕疵结果信息
				GetDefectInNewRegion(Result, i, m_vPartitionMsg, m_tagDefects);
			}
			for (int num = 0; num < m_tagDefects.size(); num++)
			{
				count += m_tagDefects[num].nCol1.size();
			}
		}

		//3.尺寸NG数据获取
		if (m_tSocketSet.nUseCheckSize)
		{
			GetDefectSize(Result, m_vPartitionMsg, m_tagDefects);

			for (int num = 0; num < m_tagDefects.size(); num++)
			{
				count += m_tagDefects[num].nCol1.size();
			}
		}

		//4.汇总后若有缺陷存在则：1.发送结果到喷码器；2.存储(瑕疵+尺寸)数据
		CString strAllResult = "";//双面综合结果	
		if (count != 0)
		{
			tMarkingRes.dWholeWidth = Result.vecB[0].tMeasure.tSize.hv_WholeWidth.D();
			//按分区整合双面需要发送给喷码器的瑕疵数据
			CString type = "j";
			map<CString, CString>::iterator iter;
			tMarkingRes.vSaveResult.resize(m_tagDefects.size(), "");//用于存储各分区的瑕疵结果(类型 + 区域)
			for (int m = 0; m < m_tagDefects.size(); m++)
			{
				for (int n = 0; n < m_tagDefects[m].nType.size(); n++)
				{
					if (m_tagDefects[m].nType[n] == NG_ExposeFoil)//涂布漏金属/划痕/颗粒
						type = "a";//->漏金属
					else if (m_tagDefects[m].nType[n] == NG_Tape)//胶带
						type = "b";//->接带
					else if (m_tagDefects[m].nType[n] == NG_Uncoated)//底涂未涂
						type = "c";//->脱碳
					else if (m_tagDefects[m].nType[n] == NG_Dent)//凹痕/凹凸点
						type = "d";//->汽包
					//else if (m_tagDefects[m].nType[n] == NG_Dent)//预留+备用
					//	type = "e";//->划痕
					else if (m_tagDefects[m].nType[n] == NG_Streak || m_tagDefects[m].nType[n] == NG_Fold)//条痕 + 膜区褶皱
						type = "f";//->条痕(打皱)
					else if (m_tagDefects[m].nType[n] == NG_Bubble)//气泡
						type = "g";//->气泡
					else if (m_tagDefects[m].nType[n] == NG_FoilBlackSpot || m_tagDefects[m].nType[n] == NG_BlackSpot)//极耳 + 涂布黑点/暗缺陷
						type = "h";//->极片黑点
					else if (m_tagDefects[m].nType[n] == NG_FoilWidth || m_tagDefects[m].nType[n] == NG_CoatWidth || m_tagDefects[m].nType[n] == NG_AT11Width || m_tagDefects[m].nType[n] == NG_BaseCoatWidth)
						type = "j";//->极片尺寸不良
					else
						continue;//未确定NG类型，不添加；

					CString str;
					str.Format("%s#x1=%d#y1=%d#x2=%d#y2=%d;", type, (int)m_tagDefects[m].nCol1[n], (int)m_tagDefects[m].nRow1[n], (int)m_tagDefects[m].nCol2[n], (int)m_tagDefects[m].nRow2[n]);
					strAllResult += str;
					//2>:本地保存瑕疵数据整合
					CString strNg;
					iter = NgDescribe.find(type);//根据NG类型获取对应描述		
					if (iter != NgDescribe.end())
					{
						strNg.Format("%s-%s;(%d %d %d %d);", type, iter->second, (int)m_tagDefects[m].nCol1[n], (int)m_tagDefects[m].nRow1[n], (int)m_tagDefects[m].nCol2[n], (int)m_tagDefects[m].nRow2[n]);
						tMarkingRes.vSaveResult[m] += strNg;
					}
				}
			}
			m_tagDefects.clear();

			tMarkingRes.sResult = strAllResult;     //结果字符串
		}


		CLockHelper lk(&m_csLock);
		m_vMarkingRes.push_back(tMarkingRes);
		return TRUE;
	}
	catch (HException& e)
	{
		MARKINGRES tMarkingRes;
		tMarkingRes.nImgNum = Result.vecB[0].vecA[0].hvImageSN.I(); //图像序号
		tMarkingRes.nImgSNTarget = Result.vecB[0].vecA[0].hvImageSN.I() + m_tSocketSet.nImgDiff;
		tMarkingRes.sResult = "";
		CLockHelper lk(&m_csLock);
		m_vMarkingRes.push_back(tMarkingRes);

		//打印异常通讯日志
		CString strException;
		strException.Format("CMarkingCtr:SendResultToMarking接口出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);

		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return FALSE;
}

BOOL CMarkingCtr::SendStrToMarking(CString sResult, int sendSN, int newSN)
{
	//发送瑕疵NG结果字符串给喷码机
	try
	{
		int nLen = sResult.GetLength();
		CString sFormate;
		sFormate.Format("A%d", nLen);
		if (m_bIsConnected)
		{
			//默认数据类型为‘z’数据长度可变的字符串
			SendData(m_hv_Socket, (HTuple)sFormate, (HTuple)sResult, HTuple());

			//打印调试日志
			CString sData;
			sData.Format("发送图像号:%d 最新图像号:%d 向喷码器发送数据为:%s", sendSN, newSN, sResult);
			//Log(sData, DebugLog);
		}
		return TRUE;
	}
	catch (HException& e)
	{
		m_bIsConnected = FALSE;
		//打印异常通讯日志
		CString strException;
		strException.Format("CMarkingCtr:SendStrToMarking向喷码器发送数据异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);
		return FALSE;
	}
}

BOOL CMarkingCtr::ReceiveStringResult(CString& sResult)
{
	try
	{
		HTuple hv_RecvData, hv_From;
		//CString sFormate="C1024";
		if (m_bIsConnected)
		{
			ReceiveData(m_hv_Socket, "z", &hv_RecvData, &hv_From);
			//CString s1;
			if (hv_RecvData.Length() > 0)
				sResult = hv_RecvData[0].S().Text();

			//打印调试日志
			CString rData;
			rData.Format("从喷码器接收数据为:%s", sResult);
			Log(rData, DebugLog);
		}
		return TRUE;
	}
	catch (HException& e)
	{
		//CString strException;
		//strException.Format("CMarkingCtr:ReceiveStringResult从喷码器接收数据异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			//, e.ProcName().Text());
		//Log(strException, ExcpLog);
		return FALSE;
	}
}

void* CMarkingCtr::HThreadMarking(void *hcthread)//发送喷码机结果线程
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		g_This->marking_thread();
		hcppthread->Exit();
		delete hcppthread;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CMarkingCtr:HThreadMarking出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_This->Log(strException, ExcpLog);
		bool is_direct_call = hcppthread->IsDirectCall();
		hcppthread->Exit();
		delete hcppthread;
		if (is_direct_call)
			throw e;
	}
	return NULL;
}

void CMarkingCtr::marking_thread()
{
	int IONo(0);
	m_sInit.Main(MT_Marking, "ReadCount", "In0", &IONo, NULL);
	int nOldIOCount = 0;
	m_nIOCount = 0;
	//算法出结果，相对IO计数有延迟
	BOOL delayFlag = false;
	while (TRUE)
	{
		//监听IO信号，发送结果
		try
		{
			if(!m_tSocketSet.nUseCheckSize && !m_tSocketSet.nUseCheckDefeat)
			{
				Sleep(100);
				continue;
			}

			CString strLog;
			m_sInit.Main(MT_Marking, "ReadCount", "In0", &IONo, NULL);
			if (nOldIOCount != IONo)
			{
				if (m_vMarkingRes.size() == abs(m_tSocketSet.nImgDiff) + 1)
				{
					nOldIOCount = IONo;
					m_nIOCount++;
					int nCurrentImgNum = m_vMarkingRes.back().nImgNum;		//当前图像号
					MARKINGRES tMarkingRes = m_vMarkingRes.front();

					//strLog.Format("3. nImgNum:%d nImgSNTarget:%d", nImgNum, nImgSNTarget);
					//Log(strLog, DebugLog);

					//*队列最新图像号 = 目标图像号，需要发送
					//if (nImgNum == nImgSNTarget)
					{
						if (tMarkingRes.sResult != "")
						{
							strLog.Format("Marking:发送喷码数据 当前图像号:%d, 目标图像号:%d, 瑕疵所在图像号:%d, IO计数:%d)", nCurrentImgNum, tMarkingRes.nImgSNTarget, tMarkingRes.nImgNum, m_nIOCount);
							strLog += ", 喷码数据:" + tMarkingRes.sResult;
							Log(strLog, DebugLog);

							CString strAllResult;
							strAllResult.Format("cam_a#w=%d#l=%d;time#delay=%d;", (int)tMarkingRes.dWholeWidth, m_tSocketSet.nMarkingHeight, ++m_nNGCount);
							tMarkingRes.sResult = strAllResult + tMarkingRes.sResult;
							SendStrToMarking(tMarkingRes.sResult, tMarkingRes.nImgNum, nCurrentImgNum);
							//本地文件保存瑕疵数据
							SaveDataMarking(tMarkingRes.nImgNum, tMarkingRes.dTotalMeter, tMarkingRes.dSpeed, tMarkingRes.vSaveResult);
						}
						CLockHelper lk(&m_csLock);
						m_vMarkingRes.erase(m_vMarkingRes.begin());
					}
				}
				else if (m_vMarkingRes.size() > abs(m_tSocketSet.nImgDiff) + 1)
				{
					strLog.Format("Marking: 喷码队列长度%d超过延迟帧数%d,将清空多余元素 ", m_vMarkingRes.size(), m_tSocketSet.nImgDiff);
					Log(strLog, DebugLog);

					CLockHelper lk(&m_csLock);
					m_vMarkingRes.erase(m_vMarkingRes.begin());
				}

				if (m_vMarkingRes.size() >= 10)
				{
					strLog.Format("Marking: 喷码队列长度过长%d,请检查 ", m_vMarkingRes.size());
					Log(strLog, ExcpLog);
				}
			}
		}
		catch (HException& e)
		{
			//CString strException;
			//strException.Format("CMarkingCtr:marking_thread出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			//	, e.ProcName().Text());
			//g_This->Log(strException, ExcpLog);
		}
		//判断重连
		if (SocketReconnect())
		{
			CLockHelper lk(&m_csLock);
			m_vMarkingRes.clear();
			CString strMess;
			strMess.Format("CMarkingCtr: 已经连接服务器，喷码队列已清空");
			Log(strMess, StdLog);
		}
		Sleep(20); //延时
	}
}

BOOL CMarkingCtr::CreateMarkingFile(const CString& strFilePath, int nPartNum)
{
	try
	{
		CStdioFile file;
		if (TRUE != file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("CMarkingCtr_CreateMarkingFile: 创建csv文件失败 (%s)", strFileName);
			Log(strException, ExcpLog);
			return false;
		}
		//时间，切片ID，编号关联，相机，速度，计长，高度，分区1，分区2，分区3，分区4....
		CString strItem = "时间,切片ID,编号关联,相机,速度,计长,高度,";
		CString strTmp;
		for (int i = 0; i < nPartNum; i++)
		{
			strTmp.Format("分区%d,", i + 1);
			strItem += strTmp;
		}
		strItem += "\n";
		file.WriteString(strItem);
		file.Close();

		return true;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CMarkingCtr:CreateMarkingFile 创建csv文件出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_This->Log(strException, ExcpLog);
		return false;
	}
}
//保存喷涂的数据
BOOL CMarkingCtr::SaveDataMarking(int nImgNum, double dTM, double dS, vector<CString> saveResults)
{
	try
	{
		CString llog;
		//时间
		SYSTEMTIME	tSaveTime;
		::GetLocalTime(&tSaveTime);
		CString sTime;
		sTime.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay,
			tSaveTime.wHour, tSaveTime.wMinute, tSaveTime.wSecond, tSaveTime.wMilliseconds);
		//切片ID   =======>图像号
		//int nImgNum = Result.vecB[0].vecA[0].hvImageSN.I();
		//编号关联=========ng
		int nNGCount = m_nNGCount;
		//相机
		int nCameraNo = 0;
		//速度
		double speed = dS;
		//计长
		double totalMeter = dTM;
		//高度
		double height = m_tSocketSet.nMarkingHeight;

		CString pDirectorPath;

		pDirectorPath.Format("%s\\Log\\Marking\\Data\\%04d\\%02d\\%02d", GetCurrentAppPath(), tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay);
		strFileName.Format("%s\\%s.csv", pDirectorPath, m_strCurrentBatch);
		CStdioFile file;

		if (!PathFileExists(pDirectorPath))CreateMultilevelPath(pDirectorPath);
		if (!PathFileExists(strFileName))
		{
			if (FALSE == CreateMarkingFile(strFileName, saveResults.size())) return false;
		}

		if (TRUE != file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("Storage_CreateRecordFile: 创建检查数据文件失败 (%s)", strFileName);
			Log(strException, ExcpLog);
		}

		//时间，切片ID，编号关联，相机，速度，计长，高度，分区1，分区2，分区3，分区4....
		CString strItem,strTmp;
		strItem.Format("%s,%d,%d,%d,%f,%f,%f,", sTime, nImgNum, nNGCount, nCameraNo, speed, totalMeter,
			height);
		for (int i = 0; i < saveResults.size(); i++)
		{
			strTmp.Format("%s,", saveResults[i]);
			strItem += strTmp;
		}
		strItem += "\n";

		file.SeekToEnd();//文件存在，则追加消息
		file.WriteString(strItem);
		file.Close();
		return True;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CMarkingCtr:SaveDataMarking出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_This->Log(strException, ExcpLog);
		return false;
	}
}
