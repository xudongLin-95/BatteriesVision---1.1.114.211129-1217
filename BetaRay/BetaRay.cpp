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
	m_tParam.strIP.ReleaseBuffer();//????????????
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

	//????????????
	ReadParam();

	//return TRUE;
	
	//????????????????????
	HTuple hvQueryMMDataMsg = GenQueryBetaRayMMDataMsg();
	m_nQueryMMDataMsgLength = hvQueryMMDataMsg.Length();
	for (int i = 0; i < hvQueryMMDataMsg.Length(); i++)
	{
		m_chQueryMMDataMsg[i] = (char)(hvQueryMMDataMsg[i].I() & 0xFF);
	}

	//????????????????????
	HTuple hvQueryPdInfoMsg = GenQueryBetaRayPdInfoMsg();
	m_nQueryPdInfoMsgLength = hvQueryPdInfoMsg.Length();
	for (int i = 0; i < hvQueryPdInfoMsg.Length(); i++)
	{
		m_chQueryPdInfoMsg[i] = (char)(hvQueryPdInfoMsg[i].I() & 0xFF);
	}

	//??????????
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
		//????????
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
			//????????????????
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
		strException.Format("HThreadComm is exceptional(HThreadComm????????):%s  [ %s() ]", e.ErrorMessage().Text()
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

			//??????????
			int nRes = send(g_This->m_socket, g_This->m_chQueryPdInfoMsg, g_This->m_nQueryPdInfoMsgLength, 0);
			//????
			char chRecv[10000];
			int nNum = recv(g_This->m_socket, chRecv, 10000, 0);
			chRecv[nNum] = 0;
			if (nNum > 0)
			{
				//????????????
				g_This->_GetPdInfoFromRecv(chRecv, nNum);
			}
			else
			{
				//????????????????
				CString strLog;
				strLog.Format("BetaRayComm: ??????????????????????????");
				g_This->Log(strLog, CommunicationLog, NoExcp);
				g_This->m_bConnect = FALSE;
			}
			
			g_This->m_bConnect = TRUE;
			CString strLog;
			strLog.Format("BetaRay: ????????????????IP:%s,????:%d", g_This->m_tParam.strIP, g_This->m_tParam.nPort);
			g_This->Log(strLog, CommunicationLog, NoExcp);
		}
		else
		{
			//??????????????????
			int nRes = send(g_This->m_socket, g_This->m_chQueryMMDataMsg, g_This->m_nQueryPdInfoMsgLength, 0);
			//????
			char chRecv[10000];
			int nNum = recv(g_This->m_socket, chRecv, 10000, 0);
			chRecv[nNum] = 0;

			//????????????????
			if (nNum > 0)
			{
				//????????????
				g_This->_GetMMDataFromRecv(chRecv, nNum);

				Sleep(500);
				continue;
			}
			else
			{
				//????????????????
				CString strLog;
				strLog.Format("BetaRayComm: ??????????????????????");
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
	//????????+??????+??????????+????????+????????+CRC??????+????????
	//1????????8????
	hvInfo.Append(0xB6);
	hvInfo.Append(0x1D);
	hvInfo.Append(0x27);
	hvInfo.Append(0xF6);
	hvInfo.Append(0xDB);
	hvInfo.Append(0x7D);
	hvInfo.Append(0xF2);
	hvInfo.Append(0x3F);

	//2??????8????
	//H0 ??int??????1??????2????
	//H1 ??int????????H0=1????1???????????????????????? 2?????????? ??H0=2????0????????????????1??????????
	//H2 ??ASCII??????R??W
	//H3 ??int????????????????????????0?????????????? 1???????????????? H2=R H3????0??H2=W,H3=0????????????????
	//H4 ????
	//H5 ????
	//H6 ????
	//H7 ????
	hvInfo.Append(0x01);
	hvInfo.Append(0x02);
	hvInfo.Append(82);//R
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//3??????????24????
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

	//4????????4????
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//5????????????


	//6 CRC????????4????
	HTuple hvCRC = _GetCRC32T4Byte(hvInfo);
	hvInfo.Append(hvCRC.TupleInverse());

	//7 ????????,8????
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
	//????????+??????+??????????+????????+????????+CRC??????+????????
	//1????????8????
	hvInfo.Append(0xB6);
	hvInfo.Append(0x1D);
	hvInfo.Append(0x27);
	hvInfo.Append(0xF6);
	hvInfo.Append(0xDB);
	hvInfo.Append(0x7D);
	hvInfo.Append(0xF2);
	hvInfo.Append(0x3F);

	//2??????8????
	//H0 ??int??????1??????2????
	//H1 ??int????????H0=1????1???????????????????????? 2?????????? ??H0=2????0????????????????1??????????
	//H2 ??ASCII??????R??W
	//H3 ??int????????????????????????0?????????????? 1???????????????? H2=R H3????0??H2=W,H3=0????????????????
	//H4 ????
	//H5 ????
	//H6 ????
	//H7 ????
	hvInfo.Append(0x01);
	hvInfo.Append(0x02);
	hvInfo.Append(82);//R
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//3??????????24????
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

	//4????????4????
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);
	hvInfo.Append(0x00);

	//5????????????


	//6 CRC????????4????
	HTuple hvCRC = _GetCRC32T4Byte(hvInfo);
	hvInfo.Append(hvCRC.TupleInverse());

	//7 ????????,8????
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
	//????????
	CString strMsg;
	strMsg.Format("%s", buff);
	try
	{
		int nPos1 = strMsg.Find(';');//????????
		int nPos2 = strMsg.ReverseFind(';');//????????;
		if (nPos1 == -1 || nPos2 == -1) return FALSE;
		//????????
		CString strMMData = strMsg.Mid(nPos2 + 1, strMsg.GetLength() - 1);//????????????????????????????????????
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
			//??????????????0.001??????????????????????????
			HTuple hvSub,hvGreater,hvInc;
			TupleSub(m_hvecBetaRayDatas[0].T(), hvMMRawData, &hvSub);
			TupleGreaterElem(hvSub.TupleAbs(), 0.001, &hvGreater);
			TupleFind(hvGreater, 1, &hvInc);
			if (hvInc==-1 || hvInc.Length()==0)
			{
				return TRUE;
			}
		}

		
		//??????????????????
		CString strMMDataX = strMsg.Mid(nPos1 + 1, nPos2 - 1 - nPos1);//??nPos1 + 1????????????????????nPos2 - 1 - nPos1??
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

		//????????????
		m_hvecBetaRayDatasX[0].T() = hvMMRawDataX;
		m_hvecBetaRayDatas[0].T() = hvMMRawData;
		//WriteTuple(hvMMRawDataX, (HTuple)"D:\\hvMMRawDataX.tup");
		//WriteTuple(hvMMRawData, (HTuple)"D:\\hvMMRawData.tup");

		CString strLog;
		strLog.Format("BetaRay_GetMMDataFromRecv: ??????????????????????:,%s", strMsg);
		Log(strLog, CommunicationLog, NoExcp);
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("BetaRay_GetMMDataFromRecv: ??????????????????????:%s,%s", e.ErrorMessage().Text(), e.ProcName().Text());
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
		//1??????????8????
		//2????????8????
		//3????????????24????
		//4??????????4????

		//5??????
		//??????????76-79????
		chData[0] = buff[76];
		chData[1] = buff[77];
		chData[2] = buff[78];
		chData[3] = buff[79];
		tBetaRayInfo.fBaseWeight = *(float*)chData;

		//A??????????80-83????
		chData[0] = buff[80];
		chData[1] = buff[81];
		chData[2] = buff[82];
		chData[3] = buff[83];
		tBetaRayInfo.fNetWeightA = *(float*)chData;

		//B??????????84-87????
		chData[0] = buff[84];
		chData[1] = buff[85];
		chData[2] = buff[86];
		chData[3] = buff[87];
		tBetaRayInfo.fNetWeightB = *(float*)chData;

		//????????88-91????
		chData[0] = buff[88];
		chData[1] = buff[89];
		chData[2] = buff[90];
		chData[3] = buff[91];
		tBetaRayInfo.fUpperTol = *(float*)chData;

		//????????92-95????
		chData[0] = buff[92];
		chData[1] = buff[93];
		chData[2] = buff[94];
		chData[3] = buff[95];
		tBetaRayInfo.fLowerTol = *(float*)chData;

		m_tBetaRayPdInfo = tBetaRayInfo;

		CString strLog;
		strLog.Format("BetaRay_GetPdInfoFromRecv: ??????????????????:??????:%0.2f,A??????:%0.2f,B??????:%0.2f,????????:%0.2f,????????:%0.2f", 
			tBetaRayInfo.fBaseWeight, tBetaRayInfo.fNetWeightA, tBetaRayInfo.fNetWeightB, tBetaRayInfo.fUpperTol, tBetaRayInfo.fLowerTol);
		Log(strLog, CommunicationLog, NoExcp);
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("BetaRay_GetPdInfoFromRecv: ??????????????????????:%s,%s", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strLog, ExcpLog, SlightExcp);

		return FALSE;
	}

	return TRUE;
}