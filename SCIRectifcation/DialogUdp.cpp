// DialogUdp.cpp : 实现文件
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "DHInterfaceEX.h"
#include "DialogUdp.h"
//#include "halcon/halconcpp/HTuple.h"

// DialogUdp 对话框
#define WM_UPDATE_MESSAGE (WM_USER+200)

IMPLEMENT_DYNAMIC(DialogUdp, CBCGPDialog)

DialogUdp::DialogUdp(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(DialogUdp::IDD, pParent)
	, m_valR00(0)
	, m_valR01(0)
	, m_valR02(0)
	, m_valR03(0)
	, m_valR04(0)
	, m_valR05(0)
	, m_valR06(0)
	, m_valR07(0)
	, m_valR08(0)
	, m_valR09(0)
	, m_valR10(0)
	, m_valR11(0)
	, m_valR12(0)
	, m_valR13(0)
	, m_valR14(0)
	, m_valR15(0)
	, m_valR16(0)
	, m_valR17(0)
	, m_valR18(0)
	, m_valR19(0)
	, m_valR20(0)
	, m_valR21(0)
	, m_valR22(0)
	, m_valR23(0)
	, m_valR24(0)
	, m_valR25(0)
	, m_valR26(0)
	, m_valR27(0)
	, m_valR28(0)
	, m_valT00(0)
	, m_valT01(0)
	, m_valT02(0)
	, m_valT03(0)
	, m_valT04(0)
	, m_valT05(0)
	, m_valT06(0)
	, m_valT07(0)
	, m_valT08(0)
	, m_valT09(0)
	, m_valT10(0)
	, m_valT11(0)
	, m_valT12(0)
	, m_valT13(0)
	, m_valT14(0)
	, m_valT15(0)
	, m_valT16(0)
	, m_valT17(0)
	, m_valT18(0)
	, m_valT19(0)
	, m_valT20(0)
	, m_valT24(0)
	, m_valT25(0)
	, m_valT26(0)
	, m_valT27(0)
	, m_valT28(0)
	, m_bSendMSG(FALSE)
	, m_bRecvMSG(FALSE)
	, m_sendMSGSN(0)
	, m_recvMSGSN(0)
	, m_srcPort(0)
	, m_destPort(0)
	,m_pInterfaceEX(NULL)
	, m_RectifcationSN(0)
	, m_nUse(0)
	,m_pInitParam(NULL)
	, m_bEdit(FALSE)
	, m_bTest(FALSE)
	, m_iDelay(0)
	, m_nThreshValue(0)
	, m_nKp(0)
	, m_SendStatus(0)
{
	GetLocalTime(&m_sendTime);
	GetLocalTime(&m_recvTime);
	memset(m_srcIP,0,sizeof(char)*MAX_PATH);
	memset(m_destIP,0,sizeof(char)*MAX_PATH);
	memset(m_seData,0,sizeof(short)*29);
	memset(m_reData,0,sizeof(short)*29);
	memset(m_lpSeData,0,sizeof(char)*29);
	memset(m_lpReData,0,sizeof(char)*29);
}

DialogUdp::~DialogUdp()
{
}

void DialogUdp::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_valR00);
	DDX_Text(pDX, IDC_EDIT4, m_valR01);
	DDX_Text(pDX, IDC_EDIT5, m_valR02);
	DDX_Text(pDX, IDC_EDIT6, m_valR03);
	DDX_Text(pDX, IDC_EDIT7, m_valR04);
	DDX_Text(pDX, IDC_EDIT8, m_valR05);
	DDX_Text(pDX, IDC_EDIT9, m_valR06);
	DDX_Text(pDX, IDC_EDIT10, m_valR07);
	DDX_Text(pDX, IDC_EDIT11, m_valR08);
	DDX_Text(pDX, IDC_EDIT12, m_valR09);
	DDX_Text(pDX, IDC_EDIT13, m_valR10);
	DDX_Text(pDX, IDC_EDIT14, m_valR11);
	DDX_Text(pDX, IDC_EDIT15, m_valR12);
	DDX_Text(pDX, IDC_EDIT16, m_valR13);
	DDX_Text(pDX, IDC_EDIT17, m_valR14);
	DDX_Text(pDX, IDC_EDIT18, m_valR15);
	DDX_Text(pDX, IDC_EDIT19, m_valR16);
	DDX_Text(pDX, IDC_EDIT20, m_valR17);
	DDX_Text(pDX, IDC_EDIT21, m_valR18);
	DDX_Text(pDX, IDC_EDIT22, m_valR19);
	DDX_Text(pDX, IDC_EDIT23, m_valR20);
	DDX_Text(pDX, IDC_EDIT24, m_valR21);
	DDX_Text(pDX, IDC_EDIT25, m_valR22);
	DDX_Text(pDX, IDC_EDIT26, m_valR23);
	DDX_Text(pDX, IDC_EDIT27, m_valR24);
	DDX_Text(pDX, IDC_EDIT28, m_valR25);
	DDX_Text(pDX, IDC_EDIT29, m_valR26);
	DDX_Text(pDX, IDC_EDIT30, m_valR27);
	DDX_Text(pDX, IDC_EDIT31, m_valR28);
	DDX_Text(pDX, IDC_EDIT32, m_valT00);
	DDX_Text(pDX, IDC_EDIT33, m_valT01);
	DDX_Text(pDX, IDC_EDIT34, m_valT02);
	DDX_Text(pDX, IDC_EDIT35, m_valT03);
	DDX_Text(pDX, IDC_EDIT36, m_valT04);
	DDX_Text(pDX, IDC_EDIT37, m_valT05);
	DDX_Text(pDX, IDC_EDIT38, m_valT06);
	DDX_Text(pDX, IDC_EDIT39, m_valT07);
	DDX_Text(pDX, IDC_EDIT40, m_valT08);
	DDX_Text(pDX, IDC_EDIT41, m_valT09);
	DDX_Text(pDX, IDC_EDIT42, m_valT10);
	DDX_Text(pDX, IDC_EDIT43, m_valT11);
	DDX_Text(pDX, IDC_EDIT44, m_valT12);
	DDX_Text(pDX, IDC_EDIT45, m_valT13);
	DDX_Text(pDX, IDC_EDIT46, m_valT14);
	DDX_Text(pDX, IDC_EDIT47, m_valT15);
	DDX_Text(pDX, IDC_EDIT48, m_valT16);
	DDX_Text(pDX, IDC_EDIT49, m_valT17);
	DDX_Text(pDX, IDC_EDIT50, m_valT18);
	DDX_Text(pDX, IDC_EDIT51, m_valT19);
	DDX_Text(pDX, IDC_EDIT52, m_valT20);
	DDX_Text(pDX, IDC_EDIT53, m_valT24);
	DDX_Text(pDX, IDC_EDIT54, m_valT25);
	DDX_Text(pDX, IDC_EDIT55, m_valT26);
	DDX_Text(pDX, IDC_EDIT56, m_valT27);
	DDX_Text(pDX, IDC_EDIT57, m_valT28);
	DDX_Control(pDX, IDC_CHECK_USE, m_checkUse);
}


BEGIN_MESSAGE_MAP(DialogUdp, CBCGPDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_MESSAGE, OnUpdateMessage)
	ON_BN_CLICKED(IDC_BUTTON_EDITIP, &DialogUdp::OnBnClickedButtonEditip)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &DialogUdp::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &DialogUdp::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &DialogUdp::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_CHECK_USE, &DialogUdp::OnBnClickedCheckUse)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &DialogUdp::OnBnClickedButtonTest)
END_MESSAGE_MAP()


// DialogUdp 消息处理程序


BOOL DialogUdp::Bind(char * lpszIp,int iPort)
{
	 m_sock = socket(AF_INET,SOCK_DGRAM,0);
	if (INVALID_SOCKET == m_sock)
	{
		MessageBox(_T("sock创建失败"));
		return FALSE;
	}
	sockaddr_in addrSock = {0};
	addrSock.sin_family = AF_INET;
	addrSock.sin_addr.S_un.S_addr = inet_addr(m_srcIP);
	addrSock.sin_port = htons(m_srcPort);
	if(0 != bind(m_sock,(sockaddr*)&addrSock,sizeof(addrSock)))
	{
		DWORD error =  GetLastError();
		CString strMsg;
		strMsg.Format(_T("SCIRetification: bind失败,错误码;%d,bind IP：%s,端口：%d"),error,m_srcIP,m_srcPort);
		if(m_pInitParam->Log !=NULL)
		{
			
			m_pInitParam->Log(strMsg,ExcpLog,FatalExcp);
		}
		else
		{
			MessageBox(strMsg);
		}
		closesocket(m_sock);
		return FALSE;
	}
	::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ThreadProc,this,NULL,NULL);
	return TRUE;
}
void DialogUdp::AddText(char* lpszText,char*lpszCaption)
{
	SYSTEMTIME sTime;
	char szText[1024*20] = {0};
	CString strText;
	CString strtext,strTemp;
	int iLenght;
	GetLocalTime(&sTime);
	HWND hTextWnd = ::GetDlgItem(this->m_hWnd,IDC_EDIT_TEXT);
	CBCGPEdit *pEdit = (CBCGPEdit *)GetDlgItem(IDC_EDIT_TEXT);
	iLenght = pEdit->GetLineCount();
	::GetWindowText(hTextWnd,szText,1024*1024);
	pEdit->GetWindowText(strText);
	if (iLenght>=43)
	{
		strText.Empty();
	}
	strtext.Format("%s [%d:%d:%d:%d]",strText,sTime.wHour,sTime.wMinute,sTime.wSecond,sTime.wMilliseconds);
	strtext.Append(lpszCaption);
	strtext.Append("\n");
	strtext.Append(lpszText);
	strtext.Append("\r\n\n");
	::lstrcat(szText,lpszCaption);
	::lstrcat(szText,lpszText);
	::lstrcat(szText,"\r\n\n");
	::SetWindowText(hTextWnd,strtext);

}
UINT DialogUdp::ThreadProc(LPVOID lpVoid)
{
	DialogUdp *lpThis = (DialogUdp*)lpVoid;
	lpThis->RecvFrom();
	return	0;
}
void DialogUdp::RecvFrom()
{
	
	char szBuf[256] = {0};
	PRECTIFICATIONCALL pBack = (PRECTIFICATIONCALL)malloc(sizeof(RECTIFICATIONCALL));
	while(TRUE)
	{
		char szReData[256] = {0};
		sockaddr_in addSender = {0};
		memset(szReData,0,256);
		memset(m_lpSeData,0,58);
		int iLenght = sizeof(addSender);
		recvfrom(m_sock,m_lpReData,256,0,(sockaddr*)&addSender,&iLenght);
		GetLocalTime(&m_recvTime);
		m_bRecvMSG = TRUE;
		m_recvMSGSN++;
		CharToShort();
		UpdataTval();
		PostMessage(WM_UPDATE_MESSAGE ,NULL,NULL);
		if(NULL != m_pInitParam && NULL !=m_pInitParam->RectficaionCall)
		{
			 ;
			pBack->bReLoad = FALSE;
			pBack->nType = 1;
	//		m_pInitParam->RectficaionCall(pBack);
		}
		for (int i = 0; i < 29; i++)
		{
			sprintf(&szReData[4*i],"%04x",m_reData[i]);
		}
		char lpszTem[MAX_PATH];
		sprintf(lpszTem,"%s(%02d:%02d:%02d:%02d)",szReData,m_recvTime.wHour,m_recvTime.wMinute,m_recvTime.wSecond,m_recvTime.wMilliseconds);
		AddText(lpszTem,"[recvfrom]");

	}
	free(pBack);
	pBack = NULL;

}
BOOL DialogUdp::SendTo(char*lpszIp,int iport)
{
	if (m_nUse == 0)
	{
		return FALSE;
	}
	sockaddr_in addrTo={0};
	int iLenght;
	addrTo.sin_family = AF_INET;
	addrTo.sin_addr.S_un.S_addr = inet_addr(lpszIp);
	addrTo.sin_port = htons(iport);
	iLenght = sizeof(addrTo);
	updataRVal();
	ShortToChar();
	int iret = sendto(m_sock,m_lpSeData,sizeof(short)*29,0,(sockaddr*)&addrTo,sizeof(addrTo));
	if (iret == SOCKET_ERROR)
	{
		if(m_pInitParam->Log !=NULL)
		{
			CString strMes ;
			strMes.Format("SCIRetification:sendTo失败！",m_sendMSGSN,m_recvMSGSN);
			int error = 1;
			m_pInitParam->Log(strMes,ExcpLog,SlightExcp);
		}
		else
		{
			MessageBox("sendTo失败！");
		}
		
		m_SendStatus = -1;
		return FALSE;
	}
	else
	{
		m_SendStatus = 1;
	}
	GetLocalTime(&m_sendTime);
	m_bSendMSG = TRUE;
	m_sendMSGSN ++;
	SetTimer(1,20,NULL);
	char seData1[256]={0};
	for (int i =0;i<sizeof(short)*29;i++)
	{
		sprintf(&seData1[2*i],"%02x",m_lpSeData[i]);
	}
	char lpszTem[MAX_PATH];
	sprintf(lpszTem,"%s(%02d:%02d:%02d:%02d)",seData1,m_sendTime.wHour,m_sendTime.wMinute,m_sendTime.wSecond,m_sendTime.wMilliseconds);

	AddText(lpszTem,"[sendto]");
	return TRUE;

}
void DialogUdp::updataRVal(void)
{
	
	memset(m_seData,0,29);
	m_seData[0] = (short)abs(m_valR00);
	m_seData[1] = m_valR01 > 0 ? (short)(m_valR01*10+0.5) : (short)(m_valR01*10-0.5);
	m_seData[2] = m_valR02;
	m_seData[4] = m_valR04;

	m_seData[6] = m_valR06 > 0 ? (unsigned short)(m_valR06*10+0.5) : (unsigned short)(m_valR06*10-0.5);
	m_seData[7] = m_valR07 > 0 ? (short)(m_valR07*10+0.5) : (short)(m_valR07*10-0.5);
	m_seData[8] = m_valR08 > 0 ? (short)(m_valR08*10+0.5) : (short)(m_valR08*10-0.5);
	m_seData[9] = m_valR09 > 0 ? (short)(m_valR09*10+0.5) : (short)(m_valR09*10-0.5);
	m_seData[10] = m_valR10 > 0 ? (short)(m_valR10*10+0.5) : (short)(m_valR10*10-0.5);
	m_seData[11] = m_valR11 > 0 ? (short)(m_valR11*10+0.5) : (short)(m_valR11*10-0.5);
	m_seData[12] = m_valR12 > 0 ? (short)(m_valR12*10+0.5) : (short)(m_valR12*10-0.5);
	m_seData[20] = (short)(m_valR20);
	m_seData[21] = (short)abs((short) m_valR21);
	m_seData[22] = (short)abs((short) m_valR22);

}
void DialogUdp::ShortToChar(void)
{
	BYTE bHIGH, bLow;
	for (int i = 0; i < 29; i++)
	{
		short temp;
		temp = *(m_seData + i);
		bHIGH = temp >> 8;
		bLow = temp & 0xFF;	
		sprintf(&m_lpSeData[2*i],"%c",bHIGH);
		sprintf(&m_lpSeData[2*i+1],"%c",bLow);
	}
}
void DialogUdp::CharToShort(void)
{
	BYTE bHIGH, bLow;
	for ( int i =0;i< 29;i++)
	{
		short tem = 0;
		bHIGH = m_lpReData[2*i];
		bLow = m_lpReData[2*i+1];
		tem = (short)(bHIGH<<8|bLow);
		m_reData[i] = tem;
	}
	unsigned long long bHIGH1,bHIGH2,bHIGH3,bHIGH4,bLow1,bLow2,bLow3,bLow4;
	bHIGH1 = (unsigned char)m_lpReData[40];
	bHIGH2 = (unsigned char)m_lpReData[41];
	bHIGH3 = (unsigned char)m_lpReData[42];
	bHIGH4 = (unsigned char)m_lpReData[43];
	bLow1 = (unsigned char)m_lpReData[44];
	bLow2 = (unsigned char)m_lpReData[45];
	bLow3 = (unsigned char)m_lpReData[46];
	bLow4 = (unsigned char)m_lpReData[47];
	m_valT20 = (unsigned long long)(bHIGH1<<(8*7)|bHIGH2<<(8*6)|bHIGH3<<(8*5)|bHIGH4<<(8*4)|bLow1<<(8*3)|bLow2<<(8*2)|bLow3<<8|bLow4);

}
void DialogUdp::UpdataTval(void)
{
	m_valT00 = m_reData[0] ;
	m_valT01 = ((double)m_reData[1])/10;
	m_valT02 = m_reData[2];
	m_valT04 = m_reData[4];
	m_valT05 = m_reData[5];
	m_valT06 = m_reData[6];
	m_valT07 = m_reData[7];
	m_valT08 = ((double)m_reData[8]/10);
	m_valT09 = ((double)m_reData[9]/10);
	m_valT10 = ((double)m_reData[10]/10);
	m_valT11 = ((double)m_reData[11]/10);
	m_valT12 = ((double)m_reData[12]/10);
	m_valT13 = ((double)m_reData[13]/10);
	m_valT14 = ((double)m_reData[14]/10);
	m_valT15 = ((double)m_reData[15]/10);
	m_valT16 = ((double)m_reData[16]/10);
	m_valT17 = ((double)m_reData[17]/10);
	m_valT18 = ((double)m_reData[18]/10);
	m_valT19 = ((double)m_reData[19]/10);
	m_valT25 = m_reData[25];
	m_valT26 = ((double)m_reData[26]/10);
	m_valT27 = ((double)m_reData[27]/10);
	m_valT28 = ((double)m_reData[28]/10);
}

void DialogUdp::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		{
			if (m_bSendMSG)
			{
				if (m_bRecvMSG )
				{
					if (m_sendMSGSN == m_recvMSGSN)
					{
						KillTimer(1);
						m_bSendMSG = FALSE;
						m_bRecvMSG = FALSE;
						if (abs(m_sendTime.wMilliseconds - m_recvTime.wMilliseconds)<20)
						{
							m_SendStatus = 1;
							return;
						}
						else
						{
							CString strMes ;
							strMes.Format("SCIRetification:发送第%lld次消息，收到第%lld次消息,接收超时！",m_sendMSGSN,m_recvMSGSN);
							if (NULL != m_pInitParam->Log)
							{
								m_pInitParam->Log(strMes,ExcpLog,SlightExcp);
							}
							else
							{
								MessageBox(strMes);
							}
							m_SendStatus = 0;
							return; 
						}

					}else
					{
						KillTimer(1);
						m_bSendMSG = FALSE;
						m_bRecvMSG = FALSE;
						if (m_recvTime.wSecond == m_recvTime.wSecond && abs(m_recvTime.wMilliseconds - m_recvTime.wMilliseconds)<20)
						{
							m_recvMSGSN = m_sendMSGSN;
							m_SendStatus = 0;
							return;
						}
						CString strMes ;
						strMes.Format("SCIRetification:发送第%lld次消息，收到第%lld次消息！",m_sendMSGSN,m_recvMSGSN);
						if (NULL != m_pInitParam->Log)
						{
							m_pInitParam->Log(strMes,ExcpLog,SlightExcp);
						}
						else
						{
							MessageBox(strMes);
						}

					}

				}else
				{
					KillTimer(1);
					m_bSendMSG = FALSE;
					m_bRecvMSG = FALSE;
					CString strMes ;
					strMes.Format("SCIRetification:第%lld次消息已发送，但未收到回复！",m_sendMSGSN);
					if (NULL != m_pInitParam->Log)
					{
						m_pInitParam->Log(strMes,ExcpLog,SlightExcp);
					}
					else
					{
						MessageBox(strMes);
					}

				}
			}
		}
	default:
		KillTimer(1);
		break;
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


BOOL DialogUdp::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE);

	HWND hDestIpWnd  = ::GetDlgItem(this->m_hWnd,IDC_IPADDRESS_DEST);
	HWND hDestPortWnd = ::GetDlgItem(this->m_hWnd,IDC_EDIT_DESTPORT);
	HWND hSrcIpWnd  = ::GetDlgItem(this->m_hWnd,IDC_IPADDRESS_SRC);
	HWND hSrcPortWnd = ::GetDlgItem(this->m_hWnd,IDC_EDIT_SRCPORT);
	
	HWND hTestWnd = ::GetDlgItem(this->m_hWnd,IDC_BUTTON_TEST);
	::ShowWindow(hTestWnd,SW_HIDE);

	char lpsrcPort[MAX_PATH];
	char lpdestPort[MAX_PATH];
	memset(lpsrcPort,0,sizeof(char)*MAX_PATH);
	memset(lpdestPort,0,sizeof(char)*MAX_PATH);
	itoa(m_srcPort,lpsrcPort,10);
	itoa(m_destPort,lpdestPort,10);
	::SetWindowText(hSrcIpWnd,m_srcIP);
	::SetWindowText(hSrcPortWnd,lpsrcPort);
	::SetWindowText(hDestIpWnd,m_destIP);
	::SetWindowText(hDestPortWnd,lpdestPort);
	HWND hCheckUse = ::GetDlgItem(this->m_hWnd,IDC_CHECK_USE);
	char lpDelay[MAX_PATH] = {0};
	char lpThresh[MAX_PATH] = {0};
	char lpKp[MAX_PATH] = {0};
	HWND hDelay = ::GetDlgItem(this->m_hWnd,IDC_EDIT_DELAYFRAME);
	HWND hActionThresh = ::GetDlgItem(this->m_hWnd,IDC_EDIT_THRESHVALUE);
	HWND hKp = ::GetDlgItem(this->m_hWnd,IDC_EDIT_KP);
	
	itoa(m_iDelay,lpDelay,10);
	sprintf(lpThresh,"%.4f",m_nThreshValue);
	sprintf(lpKp,"%.4f",m_nKp);
	::SetWindowText(hDelay,lpDelay);
	::SetWindowTextA(hActionThresh,lpThresh);
	::SetWindowText(hKp,lpKp);


	if (m_nUse == 0)
	{
		m_checkUse.SetCheck(FALSE);
	}
	else
	{
		m_checkUse.SetCheck(TRUE);
	}
	// TODO:  在此添加额外的初始化
	Bind(m_srcIP,m_srcPort);
	AddText("Bind OK","[bind]");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
LRESULT DialogUdp::OnUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return 0;
}

void DialogUdp::OnBnClickedButtonEditip()
{
	// TODO: 在此添加控件通知处理程序代码
	SetContrl(!m_bEdit);
	m_bEdit = !m_bEdit;
}


void DialogUdp::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	HWND hDestIpWnd  = ::GetDlgItem(this->m_hWnd,IDC_IPADDRESS_DEST);
	HWND hDestPortWnd = ::GetDlgItem(this->m_hWnd,IDC_EDIT_DESTPORT);
	HWND hSrcIpWnd  = ::GetDlgItem(this->m_hWnd,IDC_IPADDRESS_SRC);
	HWND hSrcPortWnd = ::GetDlgItem(this->m_hWnd,IDC_EDIT_SRCPORT);

	HWND hDelay = ::GetDlgItem(this->m_hWnd,IDC_EDIT_DELAYFRAME);
	HWND hActionThresh = ::GetDlgItem(this->m_hWnd,IDC_EDIT_THRESHVALUE);
	HWND hKp = ::GetDlgItem(this->m_hWnd,IDC_EDIT_KP);
	//	HWND hSendWnd = ::GetDlgItem(this->m_hWnd,IDC_EDIT_SEND);

	char szSrcIp[MAX_PATH] = {0};
	char szSrcPort[MAX_PATH] = {0};
	char szDestIp[MAX_PATH] = {0};
	char szDestPort[MAX_PATH] = {0};

	char lpDelay[MAX_PATH] = {0};
	char lpThresh[MAX_PATH] = {0};
	char lpKp[MAX_PATH] = {0};
	::GetWindowText(hSrcIpWnd,m_srcIP,MAX_PATH);
	::GetWindowText(hSrcPortWnd,szSrcPort,MAX_PATH);
	::GetWindowText(hDestIpWnd,m_destIP,MAX_PATH);
	::GetWindowText(hDestPortWnd,szDestPort,MAX_PATH);
	::GetWindowText(hDelay,lpDelay,MAX_PATH);
	::GetWindowText(hActionThresh,lpThresh,MAX_PATH);
	::GetWindowText(hKp,lpKp,MAX_PATH);
	m_srcPort = atoi(szSrcPort);
	m_destPort = atoi(szDestPort);

	m_iDelay = atoi(lpDelay);
	m_nThreshValue = atof(lpThresh);
	m_nKp = atof(lpKp);

	m_pInterfaceEX->m_readParam.nSrcPort = m_srcPort;
	m_pInterfaceEX->m_readParam.nDestPort = m_destPort;
	m_pInterfaceEX->m_readParam.strSrcIP = m_srcIP;
	m_pInterfaceEX->m_readParam.strDestIP = m_destIP;
	m_pInterfaceEX->m_readParam.nUse = m_nUse;
	
	m_pInterfaceEX->m_readParam.nDelay = m_iDelay;
	m_pInterfaceEX->m_readParam.nActionThresh = m_nThreshValue;
	m_pInterfaceEX->m_readParam.nKp = m_nKp;

	if (m_pInterfaceEX->WriteParam())
	{
		m_pInterfaceEX->m_CPIDController->m_iDelay = m_iDelay;
		m_pInterfaceEX->m_CPIDController->m_ActionThresh = m_nThreshValue;
		m_pInterfaceEX->m_CPIDController->m_Kp = m_nKp;
		MessageBox(_T("参数保存成功,新参数已经生效！"));
	}
	else
	{
		MessageBox(_T("参数保存失败！"));
	}
	
	
}


void DialogUdp::OnBnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	
	UpdateData(TRUE);
	SendTo(m_destIP,m_destPort);
	
}


void DialogUdp::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	HWND hTextWnd = ::GetDlgItem(this->m_hWnd,IDC_EDIT_TEXT);
	::SetWindowText(hTextWnd,"");
}


void DialogUdp::OnBnClickedCheckUse()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nUse = m_checkUse.GetCheck();
}


BOOL DialogUdp::SendMSG()
{
	SendTo(m_destIP,m_destPort);
	return TRUE;
}


void DialogUdp::SetContrl(BOOL bFlag)
{
	HWND  hwndChild=::GetWindow(this->m_hWnd,GW_CHILD);  //列出所有控件 
	CWnd* pWnd = CWnd::FromHandle(this->m_hWnd);
	int woc; 
	CRect Rect;
	while(hwndChild)      
	{      
		woc=::GetDlgCtrlID(hwndChild);//取得ID  
		pWnd->GetDlgItem(woc)->EnableWindow(bFlag);    
		hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);      
	}
	if (bFlag == FALSE)
	{
		GetDlgItem(IDC_BUTTON_EDITIP)->EnableWindow(TRUE);
	}
}


void DialogUdp::SendTo1(char*lpszIp,int iport)
{
	if (m_nUse == 0)
	{
		return;
	}
	sockaddr_in addrTo={0};
	int iLenght;
	addrTo.sin_family = AF_INET;
	addrTo.sin_addr.S_un.S_addr = inet_addr("224.3.0.0");
	addrTo.sin_port = htons(57007);
	iLenght = sizeof(addrTo);
//	updataRVal();
//	ShortToChar();
	char lpSend[44] = {0};
	BYTE bHIGH1, bHIGH2, bLow1, bLow2;
	unsigned short sTemp;
	int iTemp;
	sprintf(lpSend, "%c", 0x55);
	sprintf(lpSend + 1, "%01c", 12);
	sTemp = 31;
	bHIGH1 = sTemp >> 8;
	bLow1 = sTemp & 0xFF;
	sprintf(lpSend + 2, "%c", bLow1);
	sprintf(lpSend + 3, "%c", bHIGH1);
	iTemp = 0xC8409D3;
	bHIGH1 = iTemp >> 24;
	bHIGH2 = (iTemp >> 16) & 0xFF;
	bLow1 = (iTemp & 0xFFFF) >> 8;
	bLow2 = iTemp & 0xFF;
	sprintf(lpSend + 4, "%c", bLow2);
	sprintf(lpSend + 5, "%c", bLow1);
	sprintf(lpSend + 6, "%c", bHIGH2);
	sprintf(lpSend + 7, "%c", bHIGH1);
	iTemp = 0xA23F272E;
	bHIGH1 = iTemp >> 24;
	bHIGH2 = (iTemp >> 16) & 0xFF;
	bLow1 = (iTemp & 0xFFFF) >> 8;
	bLow2 = iTemp & 0xFF;
	sprintf(lpSend + 8, "%c", bLow2);
	sprintf(lpSend + 9, "%c", bLow1);
	sprintf(lpSend + 10, "%c", bHIGH2);
	sprintf(lpSend + 11, "%c", bHIGH1);
	sprintf(lpSend + 12, "%c", 0);
	sprintf(lpSend + 13, "%c", 0);
	sprintf(lpSend + 14, "%c", 0);
	sprintf(lpSend + 15, "%c", 0);
	sprintf(lpSend + 16, "%c", 0);
	sprintf(lpSend + 17, "%c", 0);
	sprintf(lpSend + 18, "%c", 0);
	sprintf(lpSend + 19, "%c", 0);

	sTemp = 0;// (short)iCount;
	bHIGH1 = sTemp >> 8;
	bLow1 = sTemp & 0xFF;
	sprintf(lpSend + 20, "%c", bLow1);
	sprintf(lpSend + 21, "%c", bHIGH1);
	iTemp = 2;
	bHIGH1 = iTemp >> 24;
	bHIGH2 = (iTemp >> 16) & 0xFF;
	bLow1 = (iTemp & 0xFFFF) >> 8;
	bLow2 = iTemp & 0xFF;

	sprintf(lpSend + 22, "%c", bLow2);
	sprintf(lpSend + 23, "%c", bLow1);

	//	iRand = 20000;
	iTemp = 0.2;
	bHIGH1 = iTemp >> 24;
	bHIGH2 = (iTemp >> 16) & 0xFF;
	bLow1 = (iTemp & 0xFFFF) >> 8;
	bLow2 = iTemp & 0xFF;


	sprintf(lpSend + 24, "%c", bLow2);
	sprintf(lpSend + 25, "%c", bLow1);

	sprintf(lpSend + 26, "%c", bHIGH2);;
	sprintf(lpSend + 27, "%c", bHIGH1);;


	iTemp = -50000;
	bHIGH1 = iTemp >> 24;
	bHIGH2 = (iTemp >> 16) & 0xFF;
	bLow1 = (iTemp & 0xFFFF) >> 8;
	bLow2 = iTemp & 0xFF;
	sprintf(lpSend + 28, "%c", bLow2);
	sprintf(lpSend + 29, "%c", bLow1);
	sprintf(lpSend + 30, "%c", bHIGH2);
	sprintf(lpSend + 31, "%c", bHIGH1);
	iTemp = 50000;
	bHIGH1 = iTemp >> 24;
	bHIGH2 = (iTemp >> 16) & 0xFF;
	bLow1 = (iTemp & 0xFFFF) >> 8;
	bLow2 = iTemp & 0xFF;
	sprintf(lpSend + 32, "%c", bLow2);
	sprintf(lpSend + 33, "%c", bLow1);
	sprintf(lpSend + 34, "%c", bHIGH2);
	sprintf(lpSend + 35, "%c", bHIGH1);


	iTemp = 3000;
	bHIGH1 = iTemp >> 24;
	bHIGH2 = (iTemp >> 16) & 0xFF;
	bLow1 = (iTemp & 0xFFFF) >> 8;
	bLow2 = iTemp & 0xFF;
	sprintf(lpSend + 36, "%c", bLow2);
	sprintf(lpSend + 37, "%c", bLow1);
	sprintf(lpSend + 38, "%c", bHIGH2);
	sprintf(lpSend + 39, "%c", bHIGH1);
	sprintf(lpSend + 40, "%c", 1);
	sprintf(lpSend + 41, "%c", 2);

	sprintf(lpSend + 42, "%c", 1);
/*
	char lpSend[43] = {0};
//	lpSend[0] = 0x55;					//1位
	sprintf(lpSend,"%c",0x55);
//	lpSend[1] = 12;						//1位
	sprintf(lpSend+1,"%01c",12);
//	lpSend[2] = unsigned short(31);		//2位
	sprintf(lpSend+2,"%02c",31);
//	lpSend[4] = int (0xC8409F4);		//4位
	sprintf(lpSend+4,"%04c",0xC8409F4);
//	lpSend[8] = int (0xA23F272E);		//4位
	sprintf(lpSend+8,"%04c",0xA23F272E);//空8位										
//	lpSend[20] = unsigned short(1);		//2位
	sprintf(lpSend+20,"%02c",1);
//	lpSend[22] = int(3000);				//4位   value
	sprintf(lpSend+22,"%04c",3000);
//	lpSend[26] = 1;						//1位   valid
	sprintf(lpSend+26,"%01c",1);
//	lpSend[27] = 2;						//1位   status
	sprintf(lpSend+27,"%01c",2);
//	lpSend[28] = int(-5000);			//4位 minrange
	sprintf(lpSend+28,"%04c",-5000);
//	lpSend[32] = int(5000);				//4位 maxrange
	sprintf(lpSend+32,"%04c",5000);
//	lpSend[36] = 1;						//1位 mod
	sprintf(lpSend+36,"%01c",1);
//	lpSend[37] = unsigned short(3);		//2位 sensor num
	sprintf(lpSend+37,"%02c",3);
*/

	int iret = sendto(m_sock,lpSend,sizeof(char)*43,0,(sockaddr*)&addrTo,sizeof(addrTo));
	if (iret == SOCKET_ERROR)
	{
		MessageBox("sendTo失败！");
		return;
	}
	/*
	GetLocalTime(&m_sendTime);
	m_bSendMSG = TRUE;
	m_sendMSGSN ++;
	SetTimer(1,20,NULL);
	*/
	char seData1[256]={0};
	for (int i =0;i<sizeof(char)*43;i++)
	{
		sprintf(&seData1[i],"%02x",lpSend[i]);
	}
	char lpszTem[MAX_PATH];
	sprintf(lpszTem,"%s(%02d:%02d:%02d:%02d)",seData1,m_sendTime.wHour,m_sendTime.wMinute,m_sendTime.wSecond,m_sendTime.wMilliseconds);

	AddText(lpszTem,"[sendto]");
}


void DialogUdp::OnBnClickedButtonTest()			//电眼地址发送数据
{
	// TODO: 在此添加控件通知处理程序代码

	m_bTest = !m_bTest;
	int count = 0;
	char destIP[MAX_PATH] = {0};
	sprintf(destIP,"224.3.0.0");
	int port = 57007;
	while(count < 200)
	{
		SendTo1(destIP,port);
		count++;
		Sleep(100);
	}
}
