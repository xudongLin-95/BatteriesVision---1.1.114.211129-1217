#include "stdafx.h"
#include "BetaRay.h"
#include "DlgSet.h"
#include "CRC32.h"

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CBetaRay();
}

CBetaRay* g_This = NULL;

CBetaRay::CBetaRay()
	:m_hvecBetaRayDatas(1)
	, m_hvecBetaRayDatasX(1)
{
	g_This = this;
	m_bExitThread = FALSE;
	InitializeCriticalSection(&m_csLock);

	m_hvecBetaRayDatas.Clear();
	m_hvecBetaRayDatasX.Clear();
	m_bConnect = FALSE;

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_nQueryMMDataMsgLength = 0;
	m_nQueryPdInfoMsgLength = 0;

	m_tBetaRayPdInfo.fBaseWeight = 100;
	m_tBetaRayPdInfo.fUpperTol = 9;
	m_tBetaRayPdInfo.fLowerTol = 9;
	m_tBetaRayPdInfo.fNetWeightA = 190;
	m_tBetaRayPdInfo.fNetWeightB = 190;
}

CRITICAL_SECTION CBetaRay::m_csLock;

CBetaRay::~CBetaRay()
{
	
}

int CBetaRay::DoSomething(void* message, void* wParam, void* lParam)
{
	CString strMessage;
	strMessage.Format("%s", (char*)message);
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage)
	{
		return Init(*(BETARAYINIT*)wParam);
	}
	else if ("PUSH_MESSAGE" == strMessage || "Push_Message" == strMessage)
	{
		//return PushMessage(*(HTuple*)wParam);
	}
	else if ("SHOW" == strMessage || "Show" == strMessage)
	{
		return ShowParamDlg();
	}
	else if ("GETCONNECTION" == strMessage || "GetConnection" == strMessage)
	{
		*(BOOL*)lParam = m_bConnect;
		return TRUE;
	}
	else if ("GETBETARAYMMDATA" == strMessage || "GetBetaRayMMData" == strMessage)
	{
		*(HTupleVector*)wParam = m_hvecBetaRayDatasX;
		*(HTupleVector*)lParam = m_hvecBetaRayDatas;
		
		return TRUE;
	}
	else if ("GETBETARAYPDINFO" == strMessage || "GetBetaRayPdInfo" == strMessage)
	{
		*(BetaRayPdInfo*)lParam = m_tBetaRayPdInfo;

		return TRUE;
	}
	return FALSE;
}

BOOL CBetaRay::ReadParam()
{
	m_tParam.eType = (eBetaRayType)GetPrivateProfileInt("Config", "Type", BRT_KFCK, m_strConfigFile);
	m_tParam.bUseComm = GetPrivateProfileInt("Config", "UseComm", FALSE, m_strConfigFile);
	GetPrivateProfileString("Config", "IP", "192.168.0.115", m_tParam.strIP.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigFile);
	m_tParam.strIP.ReleaseBuffer();//读取产品名称
	m_tParam.nPort = GetPrivateProfileInt("Config", "Port", 31416, m_strConfigFile);

	return TRUE;
}

BOOL CBetaRay::SaveParam()
{
	WritePrivateProfileInt("Config", "Type", m_tParam.eType, m_strConfigFile);
	WritePrivateProfileInt("Config", "UseComm", m_tParam.bUseComm, m_strConfigFile);
	WritePrivateProfileString("Config", "IP", m_tParam.strIP, m_strConfigFile);
	WritePrivateProfileInt("Config", "Port", m_tParam.nPort, m_strConfigFile);

	return TRUE;
}

BOOL CBetaRay::Log(const CString& strLog, const int& nType, const int&nExcpLevel)
{
	if (m_sInit.Log == NULL) return FALSE;
	return m_sInit.Log(strLog, nType, nExcpLevel);
}

int CBetaRay::Init(BETARAYINIT &Init)
{
	m_sInit = Init;

	CString strFilePath;
	strFilePath.Format("%s\\Config\\Public\\BetaRay", GetCurrentAppPath());
	m_strConfigFile.Format("%s\\BetaRayConfig.ini", strFilePath);
	if (!PathFileExists(strFilePath))
		CreateMultilevelPath(strFilePath);

	//读取参数信息
	ReadParam();

	//return TRUE;
	
	//生成请求重量数据报文
	HTuple hvQueryMMDataMsg = GenQueryBetaRayMMDataMsg();
	m_nQueryMMDataMsgLength = hvQueryMMDataMsg.Length();
	for (int i = 0; i < hvQueryMMDataMsg.Length(); i++)
	{
		m_chQueryMMDataMsg[i] = (char)(hvQueryMMDataMsg[i].I() & 0xFF);
	}

	//生成请求配方信息报文
	HTuple hvQueryPdInfoMsg = GenQueryBetaRayPdInfoMsg();
	m_nQueryPdInfoMsgLength = hvQueryPdInfoMsg.Length();
	for (int i = 0; i < hvQueryPdInfoMsg.Length(); i++)
	{
		m_chQueryPdInfoMsg[i] = (char)(hvQueryPdInfoMsg[i].I() & 0xFF);
	}

	//初始化线程
	HDevThread*         pHDThread;
	HDevThreadContext   hTContext;
	pHDThread = new HDevThread(hTContext,
		(void*)HThreadComm, 0, 0);
	pHDThread->Start();

	return TRUE;
}

BOOL CBetaRay::ShowParamDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgSet dlg;
	dlg.m_pOwner = this;
	dlg.DoModal();
	return TRUE;
}

BOOL CBetaRay::Open() 
{
	try
	{
		//关闭连接
		try
		{
			g_This->m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in		Addr;
			Addr.sin_family = AF_INET;
			Addr.sin_addr.S_un.S_addr = inet_addr(g_This->m_tParam.strIP);
			Addr.sin_port = htons((short)g_This->m_tParam.nPort);
			memset(Addr.sin_zero, '\0', sizeof(Addr.sin_zero));
			int nRes = connect(g_This->m_socket, (sockaddr*)&Addr, sizeof(sockaddr));
			if (nRes == SOCKET_ERROR)
			{
				return FALSE;
			}
			//设置收发超时时间
			int iTimeOut = 100;
			setsockopt(g_This->m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeOut, sizeof(iTimeOut));
			setsockopt(g_This->m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeOut, sizeof(iTimeOut));

			g_This->m_bConnect = TRUE;
		}
		catch (HException e)
		{
			CString strLog;
			strLog.Format("%s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
			//theGlobal.m_Log.Std(strLog);
			g_This->m_bConnect = FALSE;
		}
	}
	catch (const std::exception&)
	{
		g_This->m_bConnect = FALSE;
		return FALSE;
	}

	return TRUE;
}

BOOL CBetaRay::Close() 
{
	int nRes = closesocket(g_This->m_socket);
	g_This->m_bConnect = FALSE;
	return nRes != SOCKET_ERROR;
}

void* CBetaRay::HThreadComm(void *hcthread)
{
	// +++ define thread context for this procedure
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		T_Comm();
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadComm is exceptional(HThreadComm线程异常):%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_This->Log(strException, ExcpLog, SlightExcp);
		bool is_direct_call = hcppthread->IsDirectCall();
		hcppthread->Exit();
		delete hcppthread;
		hcppthread = NULL;
		if (is_direct_call)
			throw e;
	}
	return NULL;
}

void CBetaRay::T_Comm()
{
	while (!g_This->m_bExitThread)
	{
		if (!g_This->m_tParam.bUseComm)
		{
			Sleep(100);
			continue;
		}

		if (FALSE == g_This->m_bConnect)
		{
			g_This->Close();
			if (!g_This->Open())
			{
				//g_This->m_bConnect = FALSE;
				Sleep(50);
				continue;
			}

			//获取操作单
			int nRes = send(g_This->m_socket, g_This->m_chQueryPdInfoMsg, g_This->m_nQueryPdInfoMsgLength, 0);
			//接收
			char chRecv[10000];
			int nNum = recv(g_This->m_socket, chRecv, 10000, 0);
			chRecv[nNum] = 0;
			if (nNum > 0)
			{
				//接收数据成功
				g_This->_GetPdInfoFromRecv(chRecv, nNum);
			}
			else
			{
				//超时或者断开连接
				CString strLog;
				strLog.Format("BetaRayComm: 接收配方信息失败，断开重连");
				g_This->Log(strLog, CommunicationLog, NoExcp);
				g_This->m_bConnect = FALSE;
			}
			
			g_This->m_bConnect = TRUE;
			CString strLog;
			strLog.Format("BetaRay: 连接服务器成功，IP:%s,端口:%d", g_This->m_tParam.strIP, g_This->m_tParam.nPort);
			g_This->Log(strLog, CommunicationLog, NoExcp);
		}
		else
		{
			//发送请求数据的报文
			int nRes = send(g_This->m_socket, g_This->m_chQueryMMDataMsg, g_This->m_nQueryPdInfoMsgLength, 0);
			//接收
			char chRecv[10000];
			int nNum = recv(g_This->m_socket, chRecv, 10000, 0);
			chRecv[nNum] = 0;

			//处理接收重量数据
			if (nNum > 0)
			{
				//接收数据成功
				g_This->_GetMMDataFromRecv(chRecv, nNum);

				Sleep(500);
				continue;
			}
			else
			{
				//超时或者断开连接
				CString strLog;
				strLog.Format("BetaRayComm: 接收数据失败，断开重连");
				g_This->Log(strLog, CommunicationLog, NoExcp);
				g_This->m_bConnect = FALSE;
			}
		}
	}
	closesocket(g_This->m_socket);
}

HTuple CBetaRay::GenQueryBetaRayMMDataMsg()
{
	HTuple hvInfo;
	//起始信息+头信息+头说明信息+数据长度+数据部分+CRC校验码+结束信息
	//1起始信息8字节
	hvInfo.Append(0xB6);
	hvInfo.Append(0x1D);
	hvInfo.Append(0x27);
	hvInfo.Append(0xF6);
	hvInfo.Append(0xDB);
	hvInfo.Append(0x7D);
	hvInfo.Append(0xF2);
	hvInfo.Append(0x3F);

	//2头信息8字节
	//H0 以int指定，1请求，2响应
	//H1 以int指定，当H0=1时，1：发送指令，控制机器运动 2：请求数据 当H0=2时，0：正确发送指令，1发送了错误
	//H2 以ASCII码指定R和W
	//H3 以int指定，请求反馈报告的说明0：需要反馈报告 1：不需要反馈报告 H2=R H3均为0，H2=W,H3=0时，发送反馈报告
	//H4 保留
	//H5 保留
	//H6 保留
	//H7 保留
	hvInfo.Append(0x01);
	hvInfo.Append(0x02);
	hvInfo.Append(82);//R
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//3头说明信息24字节
	hvInfo.Append(int('Q'));
	hvInfo.Append(int('U'));
	hvInfo.Append(int('E'));
	hvInfo.Append(int('R'));
	hvInfo.Append(int('Y'));
	hvInfo.Append(int('L'));
	hvInfo.Append(int('A'));
	hvInfo.Append(int('T'));
	hvInfo.Append(int('E'));
	hvInfo.Append(int('S'));
	hvInfo.Append(int('T'));
	hvInfo.Append(int('F'));
	hvInfo.Append(int('I'));
	hvInfo.Append(int('L'));
	hvInfo.Append(int('T'));
	hvInfo.Append(int('E'));
	hvInfo.Append(int('R'));
	hvInfo.Append(int('M'));
	hvInfo.Append(int('M'));
	hvInfo.Append(int('D'));
	hvInfo.Append(int('A'));
	hvInfo.Append(int('T'));
	hvInfo.Append(int('A'));
	hvInfo.Append(int(0x00));
	/*hvInfo.Append(int('G'));
	hvInfo.Append(int('E'));
	hvInfo.Append(int('T'));
	hvInfo.Append(int('D'));
	hvInfo.Append(int('A'));
	hvInfo.Append(int('T'));
	hvInfo.Append(int('A'));
	for (int i = 0; i < 17; i++)
	{
	hvInfo.Append(int(0x00));
	}*/

	//4数据长度4字节
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//5数据内容，无


	//6 CRC校验码，4字节
	HTuple hvCRC = _GetCRC32T4Byte(hvInfo);
	hvInfo.Append(hvCRC.TupleInverse());

	//7 结束信息,8字节
	hvInfo.Append(0x71);
	hvInfo.Append(0xD4);
	hvInfo.Append(0x45);
	hvInfo.Append(0x0B);
	hvInfo.Append(0x58);
	hvInfo.Append(0x14);
	hvInfo.Append(0x21);
	hvInfo.Append(0x40);

	return hvInfo;
}

HTuple CBetaRay::GenQueryBetaRayPdInfoMsg()
{
	HTuple hvInfo;
	//起始信息+头信息+头说明信息+数据长度+数据部分+CRC校验码+结束信息
	//1起始信息8字节
	hvInfo.Append(0xB6);
	hvInfo.Append(0x1D);
	hvInfo.Append(0x27);
	hvInfo.Append(0xF6);
	hvInfo.Append(0xDB);
	hvInfo.Append(0x7D);
	hvInfo.Append(0xF2);
	hvInfo.Append(0x3F);

	//2头信息8字节
	//H0 以int指定，1请求，2响应
	//H1 以int指定，当H0=1时，1：发送指令，控制机器运动 2：请求数据 当H0=2时，0：正确发送指令，1发送了错误
	//H2 以ASCII码指定R和W
	//H3 以int指定，请求反馈报告的说明0：需要反馈报告 1：不需要反馈报告 H2=R H3均为0，H2=W,H3=0时，发送反馈报告
	//H4 保留
	//H5 保留
	//H6 保留
	//H7 保留
	hvInfo.Append(0x01);
	hvInfo.Append(0x02);
	hvInfo.Append(82);//R
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//3头说明信息24字节
	hvInfo.Append(int('O'));
	hvInfo.Append(int('P'));
	hvInfo.Append(int('E'));
	hvInfo.Append(int('R'));
	hvInfo.Append(int('A'));
	hvInfo.Append(int('T'));
	hvInfo.Append(int('I'));
	hvInfo.Append(int('O'));
	hvInfo.Append(int('N'));
	hvInfo.Append(int('S'));
	hvInfo.Append(int('H'));
	hvInfo.Append(int('E'));
	hvInfo.Append(int('E'));
	hvInfo.Append(int('T'));
	for (int i = 0; i < 10; i++)
	{
		hvInfo.Append(int(0x00));
	}

	//4数据长度4字节
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//5数据内容，无


	//6 CRC校验码，4字节
	HTuple hvCRC = _GetCRC32T4Byte(hvInfo);
	hvInfo.Append(hvCRC.TupleInverse());

	//7 结束信息,8字节
	hvInfo.Append(0x71);
	hvInfo.Append(0xD4);
	hvInfo.Append(0x45);
	hvInfo.Append(0x0B);
	hvInfo.Append(0x58);
	hvInfo.Append(0x14);
	hvInfo.Append(0x21);
	hvInfo.Append(0x40);

	return hvInfo;
}

HTuple CBetaRay::_GetCRC32T4Byte(HTuple hvInfo)
{
	unsigned char bt[1024];
	for (int i = 0; i < hvInfo.Length(); i++)
	{
		bt[i] = hvInfo[i].I();
	}

	CCRC32 Crc;
	UINT32 nRes = Crc.GetCRC32(bt, hvInfo.Length());
	/*CString strHex;
	strHex.Format("%08X", nRes);*/

	HTuple hvCRC;
	hvCRC.Append((int)(nRes >> 24) & 0xFF);
	hvCRC.Append((int)(nRes >> 16) & 0xFF);
	hvCRC.Append((int)(nRes >> 8) & 0xFF);
	hvCRC.Append((int)(nRes & 0xFF));

	return hvCRC;
}

BOOL CBetaRay::_GetMMDataFromRecv(char* buff, int nLength)
{
	tagPDInfo tPdInfo = m_sInit.tPdInfo;
	//接收消息
	CString strMsg;
	strMsg.Format("%s", buff);
	try
	{
		int nPos1 = strMsg.Find(';');//第一个；
		int nPos2 = strMsg.ReverseFind(';');//最后一个;
		if (nPos1 == -1 || nPos2 == -1) return FALSE;
		//重量数据
		CString strMMData = strMsg.Mid(nPos2 + 1, strMsg.GetLength() - 1);//从最后一个；开始，提取后面所有字符串
		CString strTmp;
		HTuple hvMMRawData;
		int nPosTmpOld = 0;
		int nPosTmp = strMMData.Find(",", nPosTmpOld);
		while (nPosTmp != -1)
		{
			strTmp = strMMData.Mid(nPosTmpOld, nPosTmp - nPosTmpOld);
			hvMMRawData.Append(atof(strTmp));
			nPosTmpOld = nPosTmp + 1;

			nPosTmp = strMMData.Find(",", nPosTmpOld);
		}
		strTmp = strMMData.Mid(nPosTmpOld, strMMData.GetLength() - 1);
		hvMMRawData.Append(atof(strTmp));
		if (hvMMRawData.Length() <= 1) return TRUE;
		if (m_hvecBetaRayDatas[0].T().Length() == hvMMRawData.Length())
		{
			//两次重量数据差0.001以上，就认为是新的一组数据
			HTuple hvSub,hvGreater,hvInc;
			TupleSub(m_hvecBetaRayDatas[0].T(), hvMMRawData, &hvSub);
			TupleGreaterElem(hvSub.TupleAbs(), 0.001, &hvGreater);
			TupleFind(hvGreater, 1, &hvInc);
			if (hvInc==-1 || hvInc.Length()==0)
			{
				return TRUE;
			}
		}

		
		//重量数据对应的坐标
		CString strMMDataX = strMsg.Mid(nPos1 + 1, nPos2 - 1 - nPos1);//从nPos1 + 1开始提取字符串，提取nPos2 - 1 - nPos1位
		HTuple hvMMRawDataX;
		nPosTmpOld = 0;
		nPosTmp = strMMDataX.Find(",", nPosTmpOld);
		while (nPosTmp != -1)
		{
			strTmp = strMMDataX.Mid(nPosTmpOld, nPosTmp - nPosTmpOld);
			hvMMRawDataX.Append(atoi(strTmp));
			nPosTmpOld = nPosTmp + 1;

			nPosTmp = strMMDataX.Find(",", nPosTmpOld);
		}
		strTmp = strMMDataX.Mid(nPosTmpOld, strMMDataX.GetLength() - 1);
		hvMMRawDataX.Append(atoi(strTmp));
		if (m_hvecBetaRayDatasX[0].T() == hvMMRawDataX ||
			m_hvecBetaRayDatas[0].T() == hvMMRawData) return FALSE;

		//重量数据赋值
		m_hvecBetaRayDatasX[0].T() = hvMMRawDataX;
		m_hvecBetaRayDatas[0].T() = hvMMRawData;
		//WriteTuple(hvMMRawDataX, (HTuple)"D:\\hvMMRawDataX.tup");
		//WriteTuple(hvMMRawData, (HTuple)"D:\\hvMMRawData.tup");

		CString strLog;
		strLog.Format("BetaRay_GetMMDataFromRecv: 获取测厚仪重量数据完成:,%s", strMsg);
		Log(strLog, CommunicationLog, NoExcp);
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("BetaRay_GetMMDataFromRecv: 获取测厚仪重量数据失败:%s,%s", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strLog, ExcpLog, SlightExcp);

		return FALSE;
	}

	return TRUE;
}

BOOL CBetaRay::_GetPdInfoFromRecv(char* buff, int nLength)
{
	HTuple hvRawData;
	try
	{
		//ReadTuple((HTuple)"D:\\OperationSheet.tup", &hvRawData);
		unsigned char chData[4];
		tagBetaRayPdInfo tBetaRayInfo;
		//1起始信息，8字节
		//2头信息，8字节
		//3头说明信息，24字节
		//4数据长度，4字节

		//5数据段
		//基材重，第76-79字节
		chData[0] = buff[76];
		chData[1] = buff[77];
		chData[2] = buff[78];
		chData[3] = buff[79];
		tBetaRayInfo.fBaseWeight = *(float*)chData;

		//A面净重，第80-83字节
		chData[0] = buff[80];
		chData[1] = buff[81];
		chData[2] = buff[82];
		chData[3] = buff[83];
		tBetaRayInfo.fNetWeightA = *(float*)chData;

		//B面净重，第84-87字节
		chData[0] = buff[84];
		chData[1] = buff[85];
		chData[2] = buff[86];
		chData[3] = buff[87];
		tBetaRayInfo.fNetWeightB = *(float*)chData;

		//上限，第88-91字节
		chData[0] = buff[88];
		chData[1] = buff[89];
		chData[2] = buff[90];
		chData[3] = buff[91];
		tBetaRayInfo.fUpperTol = *(float*)chData;

		//下限，第92-95字节
		chData[0] = buff[92];
		chData[1] = buff[93];
		chData[2] = buff[94];
		chData[3] = buff[95];
		tBetaRayInfo.fLowerTol = *(float*)chData;

		m_tBetaRayPdInfo = tBetaRayInfo;

		CString strLog;
		strLog.Format("BetaRay_GetPdInfoFromRecv: 获取测厚仪配方信息:基材重:%0.2f,A面净重:%0.2f,B面净重:%0.2f,公差上限:%0.2f,公差下限:%0.2f", 
			tBetaRayInfo.fBaseWeight, tBetaRayInfo.fNetWeightA, tBetaRayInfo.fNetWeightB, tBetaRayInfo.fUpperTol, tBetaRayInfo.fLowerTol);
		Log(strLog, CommunicationLog, NoExcp);
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("BetaRay_GetPdInfoFromRecv: 获取测厚仪配方信息失败:%s,%s", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strLog, ExcpLog, SlightExcp);

		return FALSE;
	}

	return TRUE;
}