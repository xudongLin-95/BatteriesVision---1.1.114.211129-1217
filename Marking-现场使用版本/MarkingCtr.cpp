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
	initNgDescribe();//������NG���ձ�	
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
		return SendResultToMarking(*(C_MergeWork*)wParam);//313 ָ��ĳ���������
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
			Log("CMarkingCtr:MarkingCtr ����ʧ��,��������", ExcpLog);
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
#pragma region "�������� ����NG���λ�õ�queue"
		CreateMessageQueue(&m_hvDefectToMarkigQueue);
		SetMessageQueueParam(m_hvDefectToMarkigQueue, "max_message_num", 25);

		HDevThreadContext Context;
		HDevThread* pThread = new HDevThread(Context,
			(void*)HThreadMarking, 0, 0);//����һ������2��������߳�;
		HTuple ProcThreads;
		pThread->ParStart(&ProcThreads);
#pragma endregion

		return TRUE;
	}
	catch (HException& e)
	{
		//��ӡ�쳣ͨѶ��־
		CString strException;
		strException.Format("CMarkingCtr:SocketConnect�ӿڳ����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		Log(strException, ExcpLog);

		return FALSE;
	}
}

//��ȡ��ǰ��ţ���������覴��ļ�
BOOL CMarkingCtr::SetCurrentBatch(CString strBatch)
{
	//��ȡĤ���
	m_strCurrentBatch.Format("%s", strBatch);

	return TRUE;
}

//��������Ϣ��ȡ
BOOL CMarkingCtr::GetCameraSetting(vector<tagResolution>& vResolutionList, tagCameraBaseMsg& tCameraBaseMsg)
{
	try
	{
		//*�������ļ���ȡ��������������ţ����г�ʼ��

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
		//*��ȡ����ֱ���(���滮��)
		GetCameraResolution(vResolutionList, tCameraBaseMsg);

		//*��ȡA/B�������ƴͼ�Ĺ�������
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
//��ȡ����ֱ���(���滮��) :����1���������������2���ֱ����б�
BOOL CMarkingCtr::GetCameraResolution(vector<tagResolution>& vResolutionList, tagCameraBaseMsg& tCameraBaseMsg)
{
	try
	{
		if (m_sInit.tPdInfo.nCoatingType == OneSide)
		{
			//��ȡ���������б�
			m_sInit.Main(MT_Marking, "GetSideToCamSNList", "Single", &vACameraSnList, NULL);

			//����������������㵥����ķֱ���(��ʱ�������ƽ���ֱ���)
			for (int i = 0; i < vACameraSnList.size(); i++)
			{
				//ˮƽ����
				tCameraBaseMsg.dHResolutionSideA += vResolutionList[vACameraSnList[i]].dColumn;
				tCameraBaseMsg.dHResolutionSideB = 0;
				//��ֱ����
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
			//A��
			for (int i = 0; i < vACameraSnList.size(); i++)
			{
				//ˮƽ����
				tCameraBaseMsg.dHResolutionSideA += vResolutionList[vACameraSnList[i]].dColumn;
				//��ֱ����
				tCameraBaseMsg.dVResolutionSideA += vResolutionList[vACameraSnList[i]].dRow;
			}
			//A������ֱ���
			tCameraBaseMsg.dHResolutionSideA /= vACameraSnList.size();//ˮƽ����		
			tCameraBaseMsg.dVResolutionSideA /= vACameraSnList.size();//��ֱ����

			//B��
			for (int i = 0; i < vBCameraSnList.size(); i++)
			{
				//ˮƽ����
				tCameraBaseMsg.dHResolutionSideB += vResolutionList[vBCameraSnList[i]].dColumn;
				//��ֱ����
				tCameraBaseMsg.dVResolutionSideB += vResolutionList[vBCameraSnList[i]].dRow;
			}
			//B������ֱ���
			tCameraBaseMsg.dHResolutionSideB /= vBCameraSnList.size();//ˮƽ����	
			tCameraBaseMsg.dVResolutionSideB /= vBCameraSnList.size();//��ֱ����
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
	m_tSocketSet.strIP.ReleaseBuffer();//��ȡ��Ʒ����
	m_tSocketSet.nPort = GetPrivateProfileInt("Server", "Port", 60009, m_strConfigMarkingFile);
	m_tSocketSet.nUseCommu = GetPrivateProfileInt("Config", "UseCmmu", FALSE, m_strConfigMarkingFile);
	m_tSocketSet.nUseCheckDefeat = GetPrivateProfileInt("Config", "UseCheckDefeat", FALSE, m_strConfigMarkingFile);
	m_tSocketSet.nUseCheckSize = GetPrivateProfileInt("Config", "UseCheckSize", FALSE, m_strConfigMarkingFile);
	m_tSocketSet.nImgDiff = GetPrivateProfileInt("Config", "Diff", 3, m_strConfigMarkingFile);
	m_tSocketSet.nMarkingWidth = GetPrivateProfileInt("Config", "MarkingWidth", 1260, m_strConfigMarkingFile);
	m_tSocketSet.nMarkingHeight = GetPrivateProfileInt("Config", "MarkingHeight", 200, m_strConfigMarkingFile);

	m_tSocketSet.nDivImageNum = GetPrivateProfileInt("Config", "DivImageNum", 2, m_strConfigMarkingFile);//ÿ��ͼ����Ϊ2������
}

void CMarkingCtr::WriteParam()
{
	//��ʼ����������
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

		//��ӡ�쳣ͨѶ��־
		//CString strException;
		//strException.Format("CMarkingCtr:SocketConnect�ӿڳ����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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
		//��ӡ�쳣ͨѶ��־
		CString strException;
		strException.Format("CMarkingCtr:SocketDisconnect�ӿڳ����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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
		//��ӡ�쳣ͨѶ��־
		//CString strException;
		//strException.Format("CMarkingCtr:SocketReconnect�ӿڳ����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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

int CMarkingCtr::PushMessage(const HTuple& hvMessageSend)//������Ϣ�������
{
	if (!m_tSocketSet.nUseCommu)
	{
		return TRUE;
	}

	try
	{
		EnqueueMessage(m_hvDefectToMarkigQueue, hvMessageSend, HTuple(), HTuple());//����ϢͶ�ݵ�覴ý������
	}
	catch (HException& e)
	{
		if (H_ERR_MQOVL == e.ErrorCode())
		{
			CString strException;
			strException.Format("Marking��������,�����߳����˳�,���Ų�ԭ��");
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

//��ȡˮƽ���������Ե��Ϣ���·���
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
			//*��ȡ������������(�м伫����Ҫ���м�ֿ������ߵĲ���)	
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

			//*��ȡͿ����������(Ϳ������Ҫ�м�ֿ�)
			HTuple coatRow1, coatCol1, coatRow2, coatCol2;
			SmallestRectangle1(Result.vecB[sideSN].tMeasure.tCoat.ho_Region, &coatRow1, &coatCol1, &coatRow2, &coatCol2);
			for (int z = 0; z < coatCol1.Length(); z++)
			{
				vPartitionMsg[sideSN].hv_PartEdgeCol.Append((coatCol2[z].D() + coatCol1[z].D()) / 2);
			}
			//*������Ե����
			vPartitionMsg[sideSN].hv_PartEdgeCol = vPartitionMsg[sideSN].hv_PartEdgeCol.TupleSort();

			//���ɷ�������
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
		//��ӡ�쳣ͨѶ��־
		CString strException;
		strException.Format("CMarkingCtr:GetRegionOfPartition�쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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

//��ȡ覴�NG�������(Ŀǰ�����а�������)
BOOL CMarkingCtr::GetDefectInNewRegion(C_MergeWork& Result, int SideSN, vector<PARTITIONMSG>& vPartitionMsg, vector<tagDefect>& mtagDefccts)
{
	try
	{
		//���ӣ����Ϳ��һ��ߴ�Ϊ0��˵��û��Ϳ����ʱ������覴ü��	
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
			return FALSE;//*���0��X��Ĭ����ͿĤ�������覴�
		}

		HTuple hv_wtdth, hv_height;
		int unitHeight = m_tSocketSet.nMarkingHeight;
		GetImageSize(Result.vecB[SideSN].tMeasure.ho_DispImage, &hv_wtdth, &hv_height);
		int nHeight = hv_height.I() * t_CameraBaseMsg.dVResolutionSideA;
		m_tSocketSet.nDivImageNum = (int)(nHeight / unitHeight) + ((nHeight%unitHeight) > 0 ? 1 : 0);

		CString strCamSN; //����±�

		//���»������������覴�+���ͣ�ͳһ����һ������������
		int len = Result.vecB[SideSN].tMeasure.tDefect.ho_Region.CountObj(); //����覴��������
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
		//��ӡ�쳣ͨѶ��־
		CString strException;
		strException.Format("CMarkingCtr:GetDefeatInNewRegion�쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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

//��ȡ�ߴ�NG�������
BOOL CMarkingCtr::GetDefectSize(C_MergeWork& Result, vector<PARTITIONMSG>& vPartitionMsg, vector<tagDefect>& mtagDefects)
{
	try
	{
		//���ӣ����Ϳ��һ��ߴ�Ϊ0��˵��û��Ϳ����ʱ�����гߴ���	
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

		//*���»������������覴�+���ͣ�ͳһ����һ������ʱû����
		int len = Result.tMerge.hv_NGStripeSN.Length();
		NEWREGIONMSG newRegionMsg;
		GenEmptyObj(&newRegionMsg.ho_NewtRegion);
		for (int i = 0; i < len; i++)
		{
			int sideSN = Result.tMerge.hv_NGSideSN[i].I();

			if (sideSN == 0 && !checkSideA)
			{
				continue;//*���1��A��Ĭ����ͿĤ�������ߴ�
			}
			else if (sideSN == 1 && !checkSideB)
			{
				continue;//*���2��B��Ĭ����ͿĤ�������ߴ�
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
		//��ӡ�쳣ͨѶ��־
		CString strException;
		strException.Format("CMarkingCtr:GetDefeatSize�쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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

//����NG���(覴� + �ߴ�)�ַ����������
BOOL CMarkingCtr::SendResultToMarking(C_MergeWork& Result)
{
	try
	{
		//��Ҫ�Ƚ�������
		if (!m_tSocketSet.nUseCommu || !m_bIsConnected) return FALSE;
		if (!m_tSocketSet.nUseCheckDefeat && !m_tSocketSet.nUseCheckSize)
		{
			return FALSE;
		}
		int nImgNum = Result.vecB[0].vecA[0].hvImageSN.I();  //ͼ���
		int count = 0;//覴�����ͳ��
		int num = Result.vecB.size();

		double dSpeed = 0;
		m_sInit.Main(MT_Marking, "ReadSpeed", NULL, &dSpeed, NULL);

		MARKINGRES tMarkingRes;
		tMarkingRes.nImgNum = nImgNum; //ͼ�����
		tMarkingRes.nImgSNTarget = nImgNum + m_tSocketSet.nImgDiff;
		tMarkingRes.sResult = "";
		tMarkingRes.dSpeed = dSpeed;
		tMarkingRes.dTotalMeter = Result.hv_MeterCount.D();
		tMarkingRes.vSaveResult.clear();

		//1.��ȡ������������Ϣ
		vector<PARTITIONMSG> m_vPartitionMsg(num);
		GetPartitionInHorizontalDirection(Result, m_vPartitionMsg);

		//2.���ܸ�����覴����ݣ�����ȡ�����Ϣ
		//�������ݲ�����һ����
		int nPatitionNum = m_sInit.tPdInfo.nPDType * 2;
		vector<tagDefect> m_tagDefects(nPatitionNum);

		if (m_tSocketSet.nUseCheckDefeat)
		{
			for (int i = 0; i < num; i++)
			{
				//*��ȡ覴ý����Ϣ
				GetDefectInNewRegion(Result, i, m_vPartitionMsg, m_tagDefects);
			}
			for (int num = 0; num < m_tagDefects.size(); num++)
			{
				count += m_tagDefects[num].nCol1.size();
			}
		}

		//3.�ߴ�NG���ݻ�ȡ
		if (m_tSocketSet.nUseCheckSize)
		{
			GetDefectSize(Result, m_vPartitionMsg, m_tagDefects);

			for (int num = 0; num < m_tagDefects.size(); num++)
			{
				count += m_tagDefects[num].nCol1.size();
			}
		}

		//4.���ܺ�����ȱ�ݴ�����1.���ͽ������������2.�洢(覴�+�ߴ�)����
		CString strAllResult = "";//˫���ۺϽ��	
		if (count != 0)
		{
			tMarkingRes.dWholeWidth = Result.vecB[0].tMeasure.tSize.hv_WholeWidth.D();
			//����������˫����Ҫ���͸���������覴�����
			CString type = "j";
			map<CString, CString>::iterator iter;
			tMarkingRes.vSaveResult.resize(m_tagDefects.size(), "");//���ڴ洢��������覴ý��(���� + ����)
			for (int m = 0; m < m_tagDefects.size(); m++)
			{
				for (int n = 0; n < m_tagDefects[m].nType.size(); n++)
				{
					if (m_tagDefects[m].nType[n] == NG_ExposeFoil)//Ϳ��©����/����/����
						type = "a";//->©����
					else if (m_tagDefects[m].nType[n] == NG_Tape)//����
						type = "b";//->�Ӵ�
					else if (m_tagDefects[m].nType[n] == NG_Uncoated)//��ͿδͿ
						type = "c";//->��̼
					else if (m_tagDefects[m].nType[n] == NG_Dent)//����/��͹��
						type = "d";//->����
					//else if (m_tagDefects[m].nType[n] == NG_Dent)//Ԥ��+����
					//	type = "e";//->����
					else if (m_tagDefects[m].nType[n] == NG_Streak || m_tagDefects[m].nType[n] == NG_Fold)//���� + Ĥ������
						type = "f";//->����(����)
					else if (m_tagDefects[m].nType[n] == NG_Bubble)//����
						type = "g";//->����
					else if (m_tagDefects[m].nType[n] == NG_FoilBlackSpot || m_tagDefects[m].nType[n] == NG_BlackSpot)//���� + Ϳ���ڵ�/��ȱ��
						type = "h";//->��Ƭ�ڵ�
					else if (m_tagDefects[m].nType[n] == NG_FoilWidth || m_tagDefects[m].nType[n] == NG_CoatWidth || m_tagDefects[m].nType[n] == NG_AT11Width || m_tagDefects[m].nType[n] == NG_BaseCoatWidth)
						type = "j";//->��Ƭ�ߴ粻��
					else
						continue;//δȷ��NG���ͣ�����ӣ�

					CString str;
					str.Format("%s#x1=%d#y1=%d#x2=%d#y2=%d;", type, (int)m_tagDefects[m].nCol1[n], (int)m_tagDefects[m].nRow1[n], (int)m_tagDefects[m].nCol2[n], (int)m_tagDefects[m].nRow2[n]);
					strAllResult += str;
					//2>:���ر���覴���������
					CString strNg;
					iter = NgDescribe.find(type);//����NG���ͻ�ȡ��Ӧ����		
					if (iter != NgDescribe.end())
					{
						strNg.Format("%s-%s;(%d %d %d %d);", type, iter->second, (int)m_tagDefects[m].nCol1[n], (int)m_tagDefects[m].nRow1[n], (int)m_tagDefects[m].nCol2[n], (int)m_tagDefects[m].nRow2[n]);
						tMarkingRes.vSaveResult[m] += strNg;
					}
				}
			}
			m_tagDefects.clear();

			tMarkingRes.sResult = strAllResult;     //����ַ���
		}


		CLockHelper lk(&m_csLock);
		m_vMarkingRes.push_back(tMarkingRes);
		return TRUE;
	}
	catch (HException& e)
	{
		MARKINGRES tMarkingRes;
		tMarkingRes.nImgNum = Result.vecB[0].vecA[0].hvImageSN.I(); //ͼ�����
		tMarkingRes.nImgSNTarget = Result.vecB[0].vecA[0].hvImageSN.I() + m_tSocketSet.nImgDiff;
		tMarkingRes.sResult = "";
		CLockHelper lk(&m_csLock);
		m_vMarkingRes.push_back(tMarkingRes);

		//��ӡ�쳣ͨѶ��־
		CString strException;
		strException.Format("CMarkingCtr:SendResultToMarking�ӿڳ����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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
	//����覴�NG����ַ����������
	try
	{
		int nLen = sResult.GetLength();
		CString sFormate;
		sFormate.Format("A%d", nLen);
		if (m_bIsConnected)
		{
			//Ĭ����������Ϊ��z�����ݳ��ȿɱ���ַ���
			SendData(m_hv_Socket, (HTuple)sFormate, (HTuple)sResult, HTuple());

			//��ӡ������־
			CString sData;
			sData.Format("����ͼ���:%d ����ͼ���:%d ����������������Ϊ:%s", sendSN, newSN, sResult);
			//Log(sData, DebugLog);
		}
		return TRUE;
	}
	catch (HException& e)
	{
		m_bIsConnected = FALSE;
		//��ӡ�쳣ͨѶ��־
		CString strException;
		strException.Format("CMarkingCtr:SendStrToMarking�����������������쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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

			//��ӡ������־
			CString rData;
			rData.Format("����������������Ϊ:%s", sResult);
			Log(rData, DebugLog);
		}
		return TRUE;
	}
	catch (HException& e)
	{
		//CString strException;
		//strException.Format("CMarkingCtr:ReceiveStringResult�����������������쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
			//, e.ProcName().Text());
		//Log(strException, ExcpLog);
		return FALSE;
	}
}

void* CMarkingCtr::HThreadMarking(void *hcthread)//�������������߳�
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
		strException.Format("CMarkingCtr:HThreadMarking�����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
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
	//�㷨����������IO�������ӳ�
	BOOL delayFlag = false;
	while (TRUE)
	{
		//����IO�źţ����ͽ��
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
					int nCurrentImgNum = m_vMarkingRes.back().nImgNum;		//��ǰͼ���
					MARKINGRES tMarkingRes = m_vMarkingRes.front();

					//strLog.Format("3. nImgNum:%d nImgSNTarget:%d", nImgNum, nImgSNTarget);
					//Log(strLog, DebugLog);

					//*��������ͼ��� = Ŀ��ͼ��ţ���Ҫ����
					//if (nImgNum == nImgSNTarget)
					{
						if (tMarkingRes.sResult != "")
						{
							strLog.Format("Marking:������������ ��ǰͼ���:%d, Ŀ��ͼ���:%d, 覴�����ͼ���:%d, IO����:%d)", nCurrentImgNum, tMarkingRes.nImgSNTarget, tMarkingRes.nImgNum, m_nIOCount);
							strLog += ", ��������:" + tMarkingRes.sResult;
							Log(strLog, DebugLog);

							CString strAllResult;
							strAllResult.Format("cam_a#w=%d#l=%d;time#delay=%d;", (int)tMarkingRes.dWholeWidth, m_tSocketSet.nMarkingHeight, ++m_nNGCount);
							tMarkingRes.sResult = strAllResult + tMarkingRes.sResult;
							SendStrToMarking(tMarkingRes.sResult, tMarkingRes.nImgNum, nCurrentImgNum);
							//�����ļ�����覴�����
							SaveDataMarking(tMarkingRes.nImgNum, tMarkingRes.dTotalMeter, tMarkingRes.dSpeed, tMarkingRes.vSaveResult);
						}
						CLockHelper lk(&m_csLock);
						m_vMarkingRes.erase(m_vMarkingRes.begin());
					}
				}
				else if (m_vMarkingRes.size() > abs(m_tSocketSet.nImgDiff) + 1)
				{
					strLog.Format("Marking: ������г���%d�����ӳ�֡��%d,����ն���Ԫ�� ", m_vMarkingRes.size(), m_tSocketSet.nImgDiff);
					Log(strLog, DebugLog);

					CLockHelper lk(&m_csLock);
					m_vMarkingRes.erase(m_vMarkingRes.begin());
				}

				if (m_vMarkingRes.size() >= 10)
				{
					strLog.Format("Marking: ������г��ȹ���%d,���� ", m_vMarkingRes.size());
					Log(strLog, ExcpLog);
				}
			}
		}
		catch (HException& e)
		{
			//CString strException;
			//strException.Format("CMarkingCtr:marking_thread�����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
			//	, e.ProcName().Text());
			//g_This->Log(strException, ExcpLog);
		}
		//�ж�����
		if (SocketReconnect())
		{
			CLockHelper lk(&m_csLock);
			m_vMarkingRes.clear();
			CString strMess;
			strMess.Format("CMarkingCtr: �Ѿ����ӷ�������������������");
			Log(strMess, StdLog);
		}
		Sleep(20); //��ʱ
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
			strException.Format("CMarkingCtr_CreateMarkingFile: ����csv�ļ�ʧ�� (%s)", strFileName);
			Log(strException, ExcpLog);
			return false;
		}
		//ʱ�䣬��ƬID����Ź�����������ٶȣ��Ƴ����߶ȣ�����1������2������3������4....
		CString strItem = "ʱ��,��ƬID,��Ź���,���,�ٶ�,�Ƴ�,�߶�,";
		CString strTmp;
		for (int i = 0; i < nPartNum; i++)
		{
			strTmp.Format("����%d,", i + 1);
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
		strException.Format("CMarkingCtr:CreateMarkingFile ����csv�ļ������쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_This->Log(strException, ExcpLog);
		return false;
	}
}
//������Ϳ������
BOOL CMarkingCtr::SaveDataMarking(int nImgNum, double dTM, double dS, vector<CString> saveResults)
{
	try
	{
		CString llog;
		//ʱ��
		SYSTEMTIME	tSaveTime;
		::GetLocalTime(&tSaveTime);
		CString sTime;
		sTime.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay,
			tSaveTime.wHour, tSaveTime.wMinute, tSaveTime.wSecond, tSaveTime.wMilliseconds);
		//��ƬID   =======>ͼ���
		//int nImgNum = Result.vecB[0].vecA[0].hvImageSN.I();
		//��Ź���=========ng
		int nNGCount = m_nNGCount;
		//���
		int nCameraNo = 0;
		//�ٶ�
		double speed = dS;
		//�Ƴ�
		double totalMeter = dTM;
		//�߶�
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
			strException.Format("Storage_CreateRecordFile: ������������ļ�ʧ�� (%s)", strFileName);
			Log(strException, ExcpLog);
		}

		//ʱ�䣬��ƬID����Ź�����������ٶȣ��Ƴ����߶ȣ�����1������2������3������4....
		CString strItem,strTmp;
		strItem.Format("%s,%d,%d,%d,%f,%f,%f,", sTime, nImgNum, nNGCount, nCameraNo, speed, totalMeter,
			height);
		for (int i = 0; i < saveResults.size(); i++)
		{
			strTmp.Format("%s,", saveResults[i]);
			strItem += strTmp;
		}
		strItem += "\n";

		file.SeekToEnd();//�ļ����ڣ���׷����Ϣ
		file.WriteString(strItem);
		file.Close();
		return True;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CMarkingCtr:SaveDataMarking�����쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_This->Log(strException, ExcpLog);
		return false;
	}
}
