// DHSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "SCIRectifcation.h"
#include "DHSocket.h"


// CDHSocket
CDHSocket* g_This=NULL;
CDHSocket::CDHSocket()
{
	m_bConnect = FALSE;
	m_hvDiff.Clear();
	m_nJumpFrame = 0;
	m_evtGetOffset.ResetEvent();
	g_This=this;
}

CDHSocket::~CDHSocket()
{
	Close();
}


// CDHSocket 成员函数


double CDHSocket::ReadOffset(int bMainThread)
{
	//转成字符串
	static short nData[29]={0};
	//转成字符串
	char strData[59]={0};
	BYTE bHIGH, bLow;
	//转成字符串
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;	
		sprintf(&strData[2*i],"%c",bHIGH);
		sprintf(&strData[2*i+1],"%c",bLow);
	}
	m_evtGetOffset.ResetEvent();
	if(SOCKET_ERROR != SendTo(strData,58,m_tParam.nOffsetDestPort,m_tParam.strOffsetDestIP))
	{
		if (FALSE == bMainThread)
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_evtGetOffset.m_hObject, 1000))
			{
				return m_dCurrentOffset;
			}
		}
		else
		{
			try
			{
				char sData[59] = { 0 };
				Receive(sData, 58);

				short nOffset = 0;
				BYTE bHIGH, bLow;
				bHIGH = sData[22];
				bLow = sData[23];
				nOffset = (short)(bHIGH << 8 | bLow);
				m_dCurrentOffset = (double)nOffset / 10;
			}
			catch (const std::exception&)
			{
				m_dCurrentOffset = 0;
			}
			

			return m_dCurrentOffset;
		}
	}
	return 0.0;
}
void CDHSocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	try
	{
		char sData[59] = { 0 };
		Receive(sData, 58);

		short nOffset = 0;
		BYTE bHIGH, bLow;
		bHIGH = sData[22];
		bLow = sData[23];
		nOffset = (short)(bHIGH << 8 | bLow);
		m_dCurrentOffset = (double)nOffset / 10;

		
	}
	catch (const std::exception&)
	{

	}
	
	m_evtGetOffset.SetEvent();


	RECTIFICATIONCALL tCall;
	tCall.nType = 1;//代表SendMessage
	m_InitParam.RectficaionCall(&tCall);
	CSocket::OnReceive(nErrorCode);
}

BOOL CDHSocket::Init(PRECTIFICATIONINIT pInit)
{
	m_InitParam.Log = pInit->Log;
	m_InitParam.nProductType = pInit->nProductType;
	m_InitParam.nSN = pInit->nSN;
	m_InitParam.pDlg = pInit->pDlg;
	m_InitParam.RectficaionCall = pInit->RectficaionCall;
	m_InitParam.strProductName = pInit->strProductName;
	m_InitParam.Log = pInit->Log;


	ReadParam();

	Close();
	if(FALSE == Create(m_tParam.nSrcPort,SOCK_DGRAM,m_tParam.strSrcIP))
	{
		CString strMessageBox;
		strMessageBox.Format("SCIRectifcation: CDHSocket_Create:%s:%d失败",m_tParam.strSrcIP,m_tParam.nSrcPort);
		//AfxMessageBox(strMessageBox);
		m_InitParam.Log(strMessageBox, ExcpLog, SlightExcp);
	}
	
	return TRUE;
}

void CDHSocket::ReadParam()
{

	CString strFilePath;
	CString strSection;
	strSection.Format(_T("RECTIFICATION%d"),m_InitParam.nSN);
	strFilePath.Format("%s\\Config\\Public\\SCIRectification\\",GetCurrentAppPath());
	CreateMultilevelPath(strFilePath);
	m_strIniFile.Format("%s\\Rectification.ini",strFilePath);
	m_tParam.nSrcPort = GetPrivateProfileInt(strSection,"nSrcPort",2200,m_strIniFile);
	m_tParam.nOffsetDestPort = GetPrivateProfileInt(strSection,"nOffsetDestPort",2200,m_strIniFile);
	GetPrivateProfileString(strSection,"strSrcIP","127.0.0.1",m_tParam.strSrcIP.GetBuffer(MAX_PATH),MAX_PATH,m_strIniFile);
	m_tParam.strSrcIP.ReleaseBuffer();
	GetPrivateProfileString(strSection,"strOffsetDestIP","192.168.1.154",m_tParam.strOffsetDestIP.GetBuffer(MAX_PATH),MAX_PATH,m_strIniFile);
	m_tParam.strOffsetDestIP.ReleaseBuffer();

	GetPrivateProfileString(strSection,"strPositionDestIP","192.168.1.154",m_tParam.strPositionDestIP.GetBuffer(MAX_PATH),MAX_PATH,m_strIniFile);
	m_tParam.strPositionDestIP.ReleaseBuffer();
	m_tParam.nPositionDestPort = GetPrivateProfileInt(strSection,"nPositionDestPort",2200,m_strIniFile);

	m_tParam.dMax = GetPrivateProfileDouble(strSection,"dMax",1,m_strIniFile);
	m_tParam.dMin = GetPrivateProfileDouble(strSection,"dMin",-1,m_strIniFile);
	m_tParam.nMode =  GetPrivateProfileInt(strSection,"nMode",0,m_strIniFile);

	m_tParam.dOffsetMax = GetPrivateProfileDouble(strSection, "dOffsetMax", 1, m_strIniFile);

	m_PID.m_nDelay  = GetPrivateProfileInt(strSection,"tPID.nDelay",10,m_strIniFile);
	m_PID.m_dKp     = GetPrivateProfileDouble(strSection,"tPID.dKp",0.2,m_strIniFile);
	m_PID.m_dActionThresh =  GetPrivateProfileDouble(strSection,"tPID.dActionThresh",0.05,m_strIniFile);
	m_PID.m_bEnable =  GetPrivateProfileDouble(strSection,"tPID.bEnable",0,m_strIniFile);
	
	m_tParam.tDif.nFrameSeg = GetPrivateProfileInt(strSection,"tDif.nFrameSeg",10,m_strIniFile);
	m_tParam.tDif.nUseFrame = GetPrivateProfileInt(strSection,"tDif.nUseFrame",5,m_strIniFile);
	m_tParam.tDif.bEnable = GetPrivateProfileInt(strSection,"tDif.bEnable",0,m_strIniFile);
}
void CDHSocket::WriteParam()
{
	CString strFilePath;
	CString strSection;
	strSection.Format(_T("RECTIFICATION%d"),m_InitParam.nSN);
	strFilePath.Format("%s\\Config\\Public\\SCIRectification\\",GetCurrentAppPath());
	CreateMultilevelPath(strFilePath);
	m_strIniFile.Format("%s\\Rectification.ini",strFilePath);
	WritePrivateProfileInt(strSection,"nSrcPort",m_tParam.nSrcPort,m_strIniFile);
	WritePrivateProfileInt(strSection,"nOffsetDestPort",m_tParam.nOffsetDestPort,m_strIniFile);
	WritePrivateProfileString(strSection,"strSrcIP",m_tParam.strSrcIP,m_strIniFile);
	WritePrivateProfileString(strSection,"strOffsetDestIP",m_tParam.strOffsetDestIP,m_strIniFile);
	WritePrivateProfileString(strSection,"strPositionDestIP",m_tParam.strPositionDestIP,m_strIniFile);
	WritePrivateProfileInt(strSection,"nPositionDestPort",m_tParam.nPositionDestPort,m_strIniFile);

	WritePrivateProfileDouble(strSection,"dMax",m_tParam.dMax,m_strIniFile);
	WritePrivateProfileDouble(strSection,"dMin",m_tParam.dMin,m_strIniFile);
	WritePrivateProfileInt(strSection,"nMode",m_tParam.nMode,m_strIniFile);

	WritePrivateProfileDouble(strSection, "dOffsetMax", m_tParam.dOffsetMax, m_strIniFile);


	WritePrivateProfileInt(strSection,"tPID.nDelay",m_PID.m_nDelay,m_strIniFile);
	WritePrivateProfileDouble(strSection,"tPID.dKp",m_PID.m_dKp,m_strIniFile);
	WritePrivateProfileDouble(strSection,"tPID.dActionThresh",m_PID.m_dActionThresh,m_strIniFile);
	WritePrivateProfileInt(strSection,"tPID.bEnable",m_PID.m_bEnable,m_strIniFile);

	WritePrivateProfileInt(strSection,"tDif.nFrameSeg",m_tParam.tDif.nFrameSeg,m_strIniFile);
	WritePrivateProfileInt(strSection,"tDif.nUseFrame",m_tParam.tDif.nUseFrame,m_strIniFile);
	WritePrivateProfileInt(strSection,"tDif.bEnable",m_tParam.tDif.bEnable,m_strIniFile);
}
BOOL CDHSocket::DisplayTestDlg(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgDHSocket dlg;
	dlg.m_pSocket = this;
	m_pMainDlg = &dlg;
	dlg.DoModal();
	return TRUE;
}

void CDHSocket::WriteMove(int nDirection)//设置移动的方向
{
	WriteCommand(0);
	static short nData[29]={0};
	if (0 == nDirection)//左转
	{
		nData[2] = 0x0700;//
	}else if (1 == nDirection)//右转
	{
		nData[2] = 0x0050;
	}
	//转成字符串
	char strData[59]={0};
	BYTE bHIGH, bLow;
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;	
		sprintf(&strData[2*i],"%c",bHIGH);
		sprintf(&strData[2*i+1],"%c",bLow);
	}
	SendTo(strData,58,m_tParam.nOffsetDestPort,m_tParam.strOffsetDestIP);

	if (0 == nDirection)//左转
	{
		nData[2] = 0x0700;//
	}else if (1 == nDirection)//右转
	{
		nData[2] = 0x4050;
	}
	//转成字符串
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;	
		sprintf(&strData[2*i],"%c",bHIGH);
		sprintf(&strData[2*i+1],"%c",bLow);
	}
	SendTo(strData,58,m_tParam.nOffsetDestPort,m_tParam.strOffsetDestIP);
}

void CDHSocket::WriteCommand(int nCommand)//0代表手动 1代表自动
{ 
	static short nData[29]={0};
	if (2 == nCommand)//手动模式
	{
		nData[2] = 0x0004;//手动
	}else if (0 == nCommand)
	{
		nData[2] = 0x0001;//自动
	}else if (1 == nCommand)
	{
		nData[2] = 0x0002;//手动
	}
	//转成字符串
	char strData[59]={0};
	BYTE bHIGH, bLow;
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;	
		sprintf(&strData[2*i],"%c",bHIGH);
		sprintf(&strData[2*i+1],"%c",bLow);
	}
	SendTo(strData,58,m_tParam.nOffsetDestPort,m_tParam.strOffsetDestIP);

	if (2 == nCommand)//手动模式
	{
		nData[2] = 0x000c;//手动
	}
	else if (0 == nCommand)
	{
		nData[2] = 0x0009;//自动
	}
	else if (1 == nCommand)
	{
		nData[2] = 0x000a;//手动
	}
	//转成字符串
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;	
		sprintf(&strData[2*i],"%c",bHIGH);
		sprintf(&strData[2*i+1],"%c",bLow);
	}
	SendTo(strData,58,m_tParam.nOffsetDestPort,m_tParam.strOffsetDestIP);

	
}

void CDHSocket::RecvOffset(double& dValue)//收到Offset值，需要依据PID算法更改基准值了
{
	if (FALSE == m_PID.m_bEnable)
	{
		return;
	}else
	{
		m_PID.PIDWork(dValue);
	}
}

double CDHSocket::ScaleOffset(double dDiff)
{
	if (dDiff > 0)
	{
		if (dDiff > 1)
		{
			return dDiff - 0.2;
		}
		else if (dDiff > 0.6)
		{
			return 0.4;
		}
		else if (dDiff > 0.4)
		{
			return 0.2;
		}
		else if (dDiff >= 0.06)
		{
			return 0.1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (dDiff < -1)
		{
			return dDiff + 0.2;
		}
		else if (dDiff < -0.6)
		{
			return -0.4;
		}
		else if (dDiff < -0.4)
		{
			return -0.2;
		}
		else if (dDiff <= -0.06)
		{
			return -0.1;
		}
		else
		{
			return 0;
		}
	}
}
void CDHSocket::WriteDiff(double dValue, int bMainThread)//写入宽度差异值
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strText;
	strText.Format("%2d:%2d:%2d::%2dms  WriteDiff:%.1f", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, dValue);
	m_InitParam.Log(strText, StdLog, NoExcp);
	if (0 == m_nJumpFrame)//如果不是刚刚发送过
	{
		m_hvDiff.Append(dValue);
		if (m_hvDiff.Length() > m_tParam.tDif.nFrameSeg)//如果已经超出了帧数限制需要更新列表里的值
		{
			m_hvDiff.TupleRemove(0);//删除旧值	
		}else
		{
			return;
		}
		if (m_hvDiff.Length() > m_tParam.tDif.nUseFrame)//大出了需要使用的帧数，就需要计算了，计算是否需要重新发送
		{
			HTuple Seleted,Median;
			TupleSelectRange(m_hvDiff,m_hvDiff.Length()-m_tParam.tDif.nUseFrame,m_hvDiff.Length()-1,&Seleted);
			TupleMedian(Seleted,&Median);

			//根据D的值需要做不同的值设定
			double dSend(0);
			dSend = ScaleOffset(Median.D());
			if (dSend!=0)
			{
				double dCurOffset = ReadOffset(bMainThread);
				dSend += dCurOffset;
				int nSuccess;
				WriteOffset(dSend,nSuccess);

				GetLocalTime(&time);
				CString strText;
				strText.Format("%2d:%2d:%2d::%2dms  offsetChanged:%.1f->%.1f", 
					time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, dCurOffset, dSend);
				m_InitParam.Log(strText, StdLog, NoExcp);
				//CString strMessage;
				//strMessage.Format("读补偿值:%.1f，反馈:%.1f,设置补偿:%.1f",dCurOffset,Median.D(),dSend);
				//AfxMessageBox(strMessage);
				m_nJumpFrame = m_tParam.tDif.nFrameSeg;
				m_hvDiff.Clear();
			}	
		}
	}else
	{
		m_nJumpFrame--;
	}
	
}
void CDHSocket::WriteOffset(double dValue,int& bSuccess)//写入offset值
{
	static short nData[29] = { 0 };
	nData[6] = dValue > 0 ? (short)(dValue * 10 + 0.5) : (short)(dValue * 10 - 0.5);
	//转成字符串
	char strData[59] = { 0 };
	BYTE bHIGH, bLow;
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;
		sprintf(&strData[2 * i], "%c", bHIGH);
		sprintf(&strData[2 * i + 1], "%c", bLow);
	}
	if (SOCKET_ERROR == SendTo(strData, 58, m_tParam.nOffsetDestPort, m_tParam.strOffsetDestIP))
	{
		bSuccess = FALSE;
	}
	else
	{
		bSuccess = TRUE;
	}
	nData[4] = 0x0001;
	nData[6] = dValue>0?(short)(dValue*10+0.5):(short)(dValue*10-0.5);
	
	//转成字符串
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;	
		sprintf(&strData[2*i],"%c",bHIGH);
		sprintf(&strData[2*i+1],"%c",bLow);
	}
	if(SOCKET_ERROR == SendTo(strData,58,m_tParam.nOffsetDestPort,m_tParam.strOffsetDestIP))
	{
		bSuccess = FALSE;
	}else
	{
		bSuccess = TRUE;
	}

	nData[4] = 0x0000;
	nData[6] = dValue>0?(short)(dValue*10+0.5):(short)(dValue*10-0.5);
	
	//转成字符串
	for (int i = 0; i < 29; i++)
	{
		bHIGH = nData[i] >> 8;
		bLow = nData[i] & 0xFF;	
		sprintf(&strData[2*i],"%c",bHIGH);
		sprintf(&strData[2*i+1],"%c",bLow);
	}
	if(SOCKET_ERROR == SendTo(strData,58,m_tParam.nOffsetDestPort,m_tParam.strOffsetDestIP))
	{
		bSuccess = FALSE;
	}else
	{
		bSuccess = TRUE;
	}
	if (m_pMainDlg != NULL)
	{
		CString strData;
		strData.Format("%.1f", dValue);
		m_pMainDlg->SendData(strData);
	}
}

int CDHSocket::DoSomething(void* message,void* wParam,void* lParam)
{
	CString  strMessage;
	strMessage.Format("%s",(char*)message);
	RECTIFICATIONINIT * pInitParam;
	if (strMessage == "INITIALIZE" || strMessage == "Initialize")
	{
		if (wParam == NULL)
		{
			pInitParam = NULL;
		}
		else
		{
			pInitParam = (RECTIFICATIONINIT*)wParam;
		}
		return Init(pInitParam);
	}
	else if (strMessage == "SHOW" || strMessage == "Show")
	{
		return DisplayTestDlg();
	}
	else if (strMessage == "ReadOffset")
	{
		*(double*)wParam = ReadOffset(*(int*)lParam);
		return TRUE;
	}
	else if (strMessage == "WriteDiff")
	{
		if (wParam == NULL || lParam == NULL)
		{
			return TRUE;
		}
		else
		{
			double dDiff = *(double*)wParam;
			if (TRUE == m_tParam.tDif.bEnable && abs(dDiff) < m_tParam.dOffsetMax)//只接受30mm以内的偏差值
			{
				WriteDiff(dDiff,*(int*)lParam);
			}
		}	
	}
	else if (strMessage == "WriteOffset")
	{
		if (wParam == NULL)
		{
			return TRUE;
		}
		else
		{
			double dOffset = *(double*)wParam;
			if (TRUE == m_PID.m_bEnable)
			{
				dOffset = m_PID.PIDWork(dOffset);
			}
			WriteOffset(dOffset,*(int*)lParam);
		}	
	}else if (strMessage == "WritePosition")
	{
		if (wParam == NULL)
		{
			return TRUE;
		}
		else
		{	
			double dLeft,dRight;
			dLeft = *(double*)wParam;
			dRight = *(double*)lParam;
			
			int nSuccess;
			if (0 == m_tParam.nMode)//居中纠偏
			{
				if (dLeft + dRight > m_tParam.dMin && dLeft+dRight < m_tParam.dMax)
				{
					WritePositionLR(*(double*)wParam,*(double*)lParam,nSuccess);
				}
				
				//WritePositionLR(iRand,iRand,nSuccess);
			}
			else if (1 == m_tParam.nMode)//左侧纠偏
			{
				if (dLeft > m_tParam.dMin && dLeft < m_tParam.dMax)
				{
					WritePositionLR(*(double*)wParam, *(double*)wParam, nSuccess);
				}
				//WritePositionL(*(double*)wParam, nSuccess);
			}
			else if (2 == m_tParam.nMode)//右侧纠偏
			{
				if (dRight > m_tParam.dMin && dRight < m_tParam.dMax)
				{
					WritePositionLR(*(double*)lParam, *(double*)lParam, nSuccess);
				}
				//WritePositionR(*(double*)lParam, nSuccess);
			}
		}	
	}else if (strMessage == "WriteCommand")
	{
		if (wParam == NULL)
		{
			return FALSE;
		}
		else
		{
			WriteCommand(*(int*)wParam);
		}	
	}
	else if (strMessage == "GetMode")
	{
		if (wParam == NULL)
		{
			return FALSE;
		}
		else
		{
			*(int*)wParam = m_tParam.nMode;
		}
		return TRUE;
	}
	else if (strMessage == "WriteMode")
	{
		if (wParam == NULL)
		{
			return FALSE;
		}
		else
		{
			m_tParam.nMode = *(int*)wParam;
			WriteParam();
		}
		return TRUE;
	}
	else if (strMessage == "WriteMove")
	{
		if (wParam == NULL)
		{
			return FALSE;
		}
		else
		{
			WriteMove(*(int*)wParam);
		}	
	}
	else if(strMessage == "GETCONNECTION" || strMessage == "GetConnection")
	{
		if (lParam != NULL)
		{
			*(int*)lParam = m_bConnect;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

int CDHSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{

	// TODO: 在此添加专用代码和/或调用基类
	SetTimeOut(10000);
	int nRecv = CSocket::Receive(lpBuf, nBufLen, nFlags);
	KillTimeOut();
	return nRecv;
}


int CDHSocket::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: 在此添加专用代码和/或调用基类
	SetTimeOut(10000);
	int nSend = CSocket::Send(lpBuf, nBufLen, nFlags);
	KillTimeOut();
	return nSend; 
}
int CDHSocket::SendTo(const void* lpBuf, int nBufLen,UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
	RECTIFICATIONCALL tCall;
	tCall.nType = 0;//代表SendMessage
	m_InitParam.RectficaionCall(&tCall);
	return CSocket::SendTo(lpBuf,nBufLen,nHostPort,lpszHostAddress,nFlags);
}

BOOL CDHSocket::OnMessagePending()
{
	MSG msg;
	if(::PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_NOREMOVE))
	{
		if (msg.wParam == (UINT) m_nTimerID)
		{
			// Remove the message and call CancelBlockingCall.
			::PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE);
			CancelBlockingCall();
			return FALSE;  // No need for idle time processing.
		};
	};
	return CSocket::OnMessagePending();
}
BOOL CDHSocket::SetTimeOut(int nTimeOut)
{
	m_nTimerID = SetTimer(NULL,0,nTimeOut,NULL);
	return m_nTimerID;
}

BOOL CDHSocket::KillTimeOut()
{
	return KillTimer(NULL,m_nTimerID);
}


void CDHSocket::WritePositionR(double dRightValue, int& bSuccess)
{
	static char sData[44] = {0x55,12,0x1F,0,0xD3,0x09,0x84,0x0C,0x2E,0x27,0x3F,
		                     0xA2,0, 0,0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,   
							 0,0,0,0,0,0,1,1,1,0};//前4为固定初始化
	
	int nMax = int(m_tParam.dMax*1000);		//上限值 可以自己设置  类似测量量程
	int nMin = int(m_tParam.dMin*1000);		//下限值
	int nValue = int(dRightValue * 1000);			// 发送值，可能输入的是mm为单位，发送的是以um 为单位，
	//发送右边的数据转
	sData[24] = nValue & 0xFF;
	sData[25] = (nValue & 0xFFFF) >> 8;
	sData[26] = (nValue >> 16) & 0xFF;
	sData[27] = nValue >> 24;
	//下限值
	sData[28] = nMin & 0xFF;
	sData[29] = (nMin & 0xFFFF) >> 8;
	sData[30] = (nMin >> 16) & 0xFF;
	sData[31] = nMin >> 24;
	//上限值
	sData[32] = nMax& 0xFF;
	sData[33] = (nMax & 0xFFFF) >> 8;
	sData[34] = (nMax >> 16) & 0xFF;
	sData[35] = nMax >> 24;
	if (SOCKET_ERROR == SendTo(sData, 43, m_tParam.nPositionDestPort, m_tParam.strPositionDestIP))
	{
		bSuccess = FALSE;
	}
	else
	{
		bSuccess = TRUE;
	}
}
void CDHSocket::WritePositionL(double dLeftValue, int& bSuccess)
{
	static char sData[44] = {0x55,12,0x1F,0,0xF4,0x09,0x84,0x0C,0x2E,0x27,0x3F,
		                     0xA2,0, 0,0,0,0,0,0,0,0,0,0x02,0,0,0,0,0,0,0,0,0,0,0,   
							 0,0,0,0,0,0,1,2,1,0};//前4为固定初始化
	
	int nMax = int(m_tParam.dMax*1000);		//上限值 可以自己设置  类似测量量程
	int nMin = int(m_tParam.dMin*1000);		//下限值
	int nValue = int(dLeftValue * 1000);			// 发送值，可能输入的是mm为单位，发送的是以um 为单位，
	//发送右边的数据转
	sData[24] = nValue & 0xFF;
	sData[25] = (nValue & 0xFFFF) >> 8;
	sData[26] = (nValue >> 16) & 0xFF;
	sData[27] = nValue >> 24;
	//下限值
	sData[28] = nMin & 0xFF;
	sData[29] = (nMin & 0xFFFF) >> 8;
	sData[30] = (nMin >> 16) & 0xFF;
	sData[31] = nMin >> 24;
	//上限值
	sData[32] = nMax& 0xFF;
	sData[33] = (nMax & 0xFFFF) >> 8;
	sData[34] = (nMax >> 16) & 0xFF;
	sData[35] = nMax >> 24;
	if (SOCKET_ERROR == SendTo(sData, 43, m_tParam.nPositionDestPort, m_tParam.strPositionDestIP))
	{
		bSuccess = FALSE;
	}
	else
	{
		bSuccess = TRUE;
	}
}

void CDHSocket::WritePositionLR(double dLeftValue, double dRightValue, int& bSuccess)
{
	WritePositionL(dLeftValue, bSuccess);
	WritePositionR(dRightValue, bSuccess);
}