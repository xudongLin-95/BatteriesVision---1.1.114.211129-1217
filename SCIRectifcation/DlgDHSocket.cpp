// DlgDHSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "SCIRectifcation.h"
#include "DlgDHSocket.h"
#include "afxdialogex.h"


// CDlgDHSocket 对话框

IMPLEMENT_DYNAMIC(CDlgDHSocket, CBCGPDialog)

CDlgDHSocket::CDlgDHSocket(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgDHSocket::IDD, pParent)
	, m_EDT_dOffset(0)
	, m_EDT_nSrcPort(0)
	, m_EDT_nOffsetPort(0)
	, m_EDT_nDelay(0)
	, m_EDT_dThreshValue(0)
	, m_EDT_dKP(0)
	, m_COM_nPIDEnable(0)
	, m_EDT_nFrameSeg(0)
	, m_EDT_nFrameUse(0)
	, m_EDT_bEnableDiff(0)
	, m_EDT_dOffsetMax(0)
{

}

CDlgDHSocket::~CDlgDHSocket()
{
}

void CDlgDHSocket::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECIVE, m_LST_Recive);
	DDX_Text(pDX, IDC_EDIT_OFFSET, m_EDT_dOffset);
	DDV_MinMaxDouble(pDX, m_EDT_dOffset, -100, 100);
	DDX_Control(pDX, IDC_IPADDRESS_SRCPORT, m_Address_SrcIP);
	DDX_Control(pDX, IDC_IPADDRESS_OFFSET, m_Address_Offset);
	DDX_Text(pDX, IDC_EDIT_SRCPORT, m_EDT_nSrcPort);
	DDX_Text(pDX, IDC_EDIT_OFFSETPORT, m_EDT_nOffsetPort);
	DDX_Text(pDX, IDC_EDIT_DELAYFRAME, m_EDT_nDelay);
	DDX_Text(pDX, IDC_EDIT_THRESHVALUE, m_EDT_dThreshValue);
	DDX_Text(pDX, IDC_EDIT_KP2, m_EDT_dKP);
	DDX_CBIndex(pDX, IDC_COMBO_PIDENABLE, m_COM_nPIDEnable);
	DDX_Text(pDX, IDC_EDIT_DIFFFRAME, m_EDT_nFrameSeg);
	DDX_Text(pDX, IDC_EDIT_USEFRAME, m_EDT_nFrameUse);
	DDX_CBIndex(pDX, IDC_COMBO_DIFFENABLE, m_EDT_bEnableDiff);
	DDX_Control(pDX, IDC_LIST_SEND, m_LST_Send);
	DDX_Text(pDX, IDC_EDIT_MAXOFFSET, m_EDT_dOffsetMax);
}


BEGIN_MESSAGE_MAP(CDlgDHSocket, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_OFFSET, &CDlgDHSocket::OnBnClickedButtonOffset)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CDlgDHSocket::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgDHSocket::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_OFFGET, &CDlgDHSocket::OnBnClickedButtonOffget)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgDHSocket::OnBnClickedButtonCancel)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CDlgDHSocket 消息处理程序
void CDlgDHSocket::RecvData(CString strText)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strText.Format("%2d:%2d:%2d::%2dms->%s",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds,strText);
	m_LST_Recive.InsertString(0,strText);
	if (m_LST_Recive.GetCount() > 30)
		m_LST_Recive.DeleteString(m_LST_Recive.GetCount() - 1);
}

void CDlgDHSocket::SendData(CString strText)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strText.Format("%2d:%2d:%2d::%2dms->%s", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strText);
	m_LST_Send.InsertString(0, strText);
	if (m_LST_Send.GetCount() > 30)
		m_LST_Send.DeleteString(m_LST_Send.GetCount() - 1);
}

void CDlgDHSocket::OnSocketSend()
{

}

void CDlgDHSocket::OnBnClickedButtonOffset()
{
	UpdateData(TRUE);
	double dOffset = m_EDT_dOffset;
	BOOL bSuccess;
	m_pSocket->DoSomething("WriteOffset", &dOffset, &bSuccess);
}


void CDlgDHSocket::PostNcDestroy()
{
	m_pSocket->m_pMainDlg = NULL;
	CBCGPDialog::PostNcDestroy();
}


BOOL CDlgDHSocket::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	DWORD  dwIP; 
	dwIP = inet_addr(m_pSocket->m_tParam.strSrcIP); 
	unsigned  char   *pIP=(unsigned   char*)&dwIP; 
	m_Address_SrcIP.SetAddress(*pIP,*(pIP+1),*(pIP+2),*(pIP+3));

	dwIP = inet_addr(m_pSocket->m_tParam.strOffsetDestIP); 
	pIP = (unsigned   char*)&dwIP; 
	m_Address_Offset.SetAddress(*pIP,*(pIP+1),*(pIP+2),*(pIP+3));

	m_EDT_nSrcPort = m_pSocket->m_tParam.nSrcPort;
	m_EDT_nOffsetPort = m_pSocket->m_tParam.nOffsetDestPort;

	m_EDT_dOffsetMax = m_pSocket->m_tParam.dOffsetMax;

	m_EDT_nDelay = m_pSocket->m_PID.m_nDelay;
	m_EDT_dThreshValue = m_pSocket->m_PID.m_dActionThresh;
	m_EDT_dKP         = m_pSocket->m_PID.m_dKp;
	m_COM_nPIDEnable  = m_pSocket->m_PID.m_bEnable;


	m_EDT_nFrameSeg = m_pSocket->m_tParam.tDif.nFrameSeg;
	m_EDT_nFrameUse = m_pSocket->m_tParam.tDif.nUseFrame;
	m_EDT_bEnableDiff = m_pSocket->m_tParam.tDif.bEnable;

	m_pSocket->m_Lang.SetDlgLanguage(GetSafeHwnd(), "IDD_DIALOG_DHSOCKET");
	CString strTitle;
	if (m_pSocket->m_Lang.IsChinese())
		strTitle.Format("纠偏闭环设置%d", m_pSocket->m_InitParam.nSN);
	else
		strTitle.Format("CPC Loop Settings %d", m_pSocket->m_InitParam.nSN);
	SetWindowTextA(strTitle);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgDHSocket::OnBnClickedButtonSet()
{
	UpdateData(TRUE);

	BYTE nf1,nf2,nf3,nf4;
	m_Address_SrcIP.GetAddress(nf1,nf2,nf3,nf4);
	m_pSocket->m_tParam.strSrcIP.Format("%d.%d.%d.%d",nf1,nf2,nf3,nf4);
	m_Address_Offset.GetAddress(nf1,nf2,nf3,nf4);
	m_pSocket->m_tParam.strOffsetDestIP.Format("%d.%d.%d.%d",nf1,nf2,nf3,nf4);

	m_pSocket->m_tParam.nSrcPort          = m_EDT_nSrcPort;
	m_pSocket->m_tParam.nOffsetDestPort   = m_EDT_nOffsetPort;
	m_pSocket->m_tParam.dOffsetMax = m_EDT_dOffsetMax;


	m_pSocket->m_PID.m_nDelay = m_EDT_nDelay;
	m_pSocket->m_PID.m_dActionThresh = m_EDT_dThreshValue;
	m_pSocket->m_PID.m_dKp = m_EDT_dKP;
	m_pSocket->m_PID.m_bEnable = m_COM_nPIDEnable;

	m_pSocket->m_tParam.tDif.nFrameSeg = m_EDT_nFrameSeg ;
	m_pSocket->m_tParam.tDif.nUseFrame = m_EDT_nFrameUse ;
	m_pSocket->m_tParam.tDif.bEnable = m_EDT_bEnableDiff;
}


void CDlgDHSocket::OnBnClickedButtonSave()
{
	OnBnClickedButtonSet();
	m_pSocket->WriteParam();
	m_pSocket->m_Lang.Messagebox("保存成功", "Saved successfully", "");
}



void CDlgDHSocket::OnBnClickedButtonOffget()
{
	double dOffset = 0.0;
	int    nMainThread = TRUE;
	m_pSocket->DoSomething("ReadOffset",&dOffset, &nMainThread);
	m_EDT_dOffset = dOffset;
	CString strMessageBox, strMessageBox1;
	strMessageBox.Format("读取到offset值:%.1f",m_EDT_dOffset);
	strMessageBox1.Format("Read the offset:%.1f", m_EDT_dOffset);
	m_pSocket->m_Lang.Messagebox(strMessageBox, strMessageBox1, "");
	UpdateData(FALSE);
}

void CDlgDHSocket::OnBnClickedButtonCancel()
{
	OnCancel();
}

void CDlgDHSocket::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CBCGPDialog::OnActivate(nState, pWndOther, bMinimized);

	if (WA_INACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
		this->SetLayeredWindowAttributes(0, (255 * 90) / 100, LWA_ALPHA);
	}
	else if (WA_CLICKACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}
	else if (WA_ACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}
}
