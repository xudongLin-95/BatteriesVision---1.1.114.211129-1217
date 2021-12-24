// DlgIOSet.cpp : 实现文件
//

#include "stdafx.h"
#include "SCIIO.h"
#include "DlgIOSet.h"
#include "afxdialogex.h"

// CDlgIOSet 对话框
IMPLEMENT_DYNAMIC(CDlgIOSet, CBCGPDialog)
int m_BtnClickNum_Red = 0;
int m_BtnClickNum_Green = 0;
int m_BtnClickNum_Buzeer = 0;
int m_BtnClickNum_Yellow = 0;

CDlgIOSet::CDlgIOSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgIOSet::IDD, pParent)
	, m_CHECK_bLXTB0(FALSE)
	, m_CHECK_bLXTB1(FALSE)
{
	m_EDT_nIOCardSN = 0;
	m_nRejectType = 0;
	CString strBmp;
	strBmp.Format("%s//Config//Public//SCIIO//DelayModel.jpg", GetCurrentAppPath());
	if (!PathFileExistsA(strBmp))
	{
		return;
	}
	m_Image.Load(strBmp);//加载 
}

CDlgIOSet::~CDlgIOSet()
{
}

void CDlgIOSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IOCARDSN, m_EDT_nIOCardSN);
	DDV_MinMaxInt(pDX, m_EDT_nIOCardSN, 0, 99999);
	DDX_Radio(pDX, IDC_RADIO_REJECT, m_nRejectType);
	DDX_Check(pDX, IDC_CHECK_LXTB0, m_CHECK_bLXTB0);
	DDX_Check(pDX, IDC_CHECK_LXTB1, m_CHECK_bLXTB1);
	DDX_Control(pDX, IDC_COMBO_IO_GREEN, m_ChoseIO_Green);
	DDX_Control(pDX, IDC_COMBO_IO_RED, m_ChoseIO_Red);
	DDX_Control(pDX, IDC_COMBO_IO_BUZEER, m_ChoseIO_Buzzer);
	DDX_Control(pDX, IDC_COMBO_IO_YELLOW, m_ChoseIO_Yellow);
}


BEGIN_MESSAGE_MAP(CDlgIOSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_CALLIOCARDPARAMSET, &CDlgIOSet::OnBnClickedButtonCalliocardparamset)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgIOSet::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgIOSet::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_IO_GREEN, &CDlgIOSet::OnBnClickedButtonIoGreen)
	ON_BN_CLICKED(IDC_BUTTON_IO_RED, &CDlgIOSet::OnBnClickedButtonIoRed)
	ON_BN_CLICKED(IDC_BUTTON_IO_BUZEER, &CDlgIOSet::OnBnClickedButtonIoBuzeer)
	ON_BN_CLICKED(IDC_BUTTON_IO_YELLOW, &CDlgIOSet::OnBnClickedButtonIoYellow)
END_MESSAGE_MAP()


// CDlgIOSet 消息处理程序


BOOL CDlgIOSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);

	InitUI();
	m_ChoseIO_Green.SetCurSel(m_pCommuicate->m_Commuicate.nAlarmGreenLight);
	m_ChoseIO_Red.SetCurSel(m_pCommuicate->m_Commuicate.nAlarmRedLight);
	m_ChoseIO_Buzzer.SetCurSel(m_pCommuicate->m_Commuicate.nAlarmBuzzer);
	m_ChoseIO_Yellow.SetCurSel(m_pCommuicate->m_Commuicate.nAlarmYellowLight);
	InitValue();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CDlgIOSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CDlgIOSet::InitUI()
{

}

void CDlgIOSet::InitValue()
{
	m_nRejectType =  m_pCommuicate->m_Commuicate.nRejectType;//剔废类型 如果为1是间隔剔废 如果为2为连续剔废 默认为0 根据结果好坏进行剔废
	m_CHECK_bLXTB0 = m_pCommuicate->m_bLXTB[0]; //4个带路是否进行连续贴标测试
	m_CHECK_bLXTB1 = m_pCommuicate->m_bLXTB[1];
	m_pCommuicate->m_Lang.SetDlgLanguage(GetSafeHwnd(), "IDD_DIALOG_IOSET");
	UpdateData(FALSE);
}

void CDlgIOSet::Save()
{
	UpdateData(TRUE);
	m_pCommuicate->m_Commuicate.nRejectType = m_nRejectType;
	//m_pCommuicate->m_Alarms.bEffective = ((CButton*)GetDlgItem(IDC_BUTTON_CONTINUOUSERRORSET))->GetCheck();
	m_pCommuicate->m_bLXTB[0] = m_CHECK_bLXTB0;
	m_pCommuicate->m_bLXTB[1] = m_CHECK_bLXTB1;
	WritePrivateProfileInt("IO","RejectType",m_nRejectType,m_pCommuicate->m_strConfigCommunicateFile);//IO结果信号


	//获取选中通道
	m_pCommuicate->m_Commuicate.nAlarmGreenLight = m_ChoseIO_Green.GetCurSel();
	m_pCommuicate->m_Commuicate.nAlarmRedLight = m_ChoseIO_Red.GetCurSel();
	m_pCommuicate->m_Commuicate.nAlarmBuzzer = m_ChoseIO_Buzzer.GetCurSel();
	m_pCommuicate->m_Commuicate.nAlarmYellowLight = m_ChoseIO_Yellow.GetCurSel();

	WritePrivateProfileInt("IO", "AlarmGreenLight", m_pCommuicate->m_Commuicate.nAlarmGreenLight, m_pCommuicate->m_strConfigCommunicateFile);
	WritePrivateProfileInt("IO", "AlarmRedLight", m_pCommuicate->m_Commuicate.nAlarmRedLight, m_pCommuicate->m_strConfigCommunicateFile);
	WritePrivateProfileInt("IO", "AlarmBuzzer", m_pCommuicate->m_Commuicate.nAlarmBuzzer, m_pCommuicate->m_strConfigCommunicateFile);
	WritePrivateProfileInt("IO", "AlarmYellowLight", m_pCommuicate->m_Commuicate.nAlarmYellowLight, m_pCommuicate->m_strConfigCommunicateFile);
	WritePrivateProfileInt("IO", "AlarmHeatBeat", m_pCommuicate->m_Commuicate.nAlarmHeatBeat, m_pCommuicate->m_strConfigCommunicateFile);

	UpdateData(FALSE);
}

void CDlgIOSet::OnBnClickedButtonCalliocardparamset()
{
	if(IDYES == m_pCommuicate->m_Lang.MessageboxQuestion("IO卡内参数十分重要!错误的更改会引起软件异常运行!您已经和厂家联系过要更改此参数了吗?",
		"The parameters in the IO card are very important! Wrong changes will cause the software to run abnormally! Have you contacted the manufacturer to change this parameter?",""))
	{
		if (UpdateData(TRUE))
		{
			if (m_pCommuicate->m_IOCard[m_EDT_nIOCardSN].hasCard)
			{
				m_pCommuicate->m_IOCard[m_EDT_nIOCardSN].Show_PIO24B_DebugDialog(NULL);
			}else
			{
				m_pCommuicate->m_Lang.MessageboxError("无法打开，没有该接口卡，请检查。", "Cannot open, there is no IO card, please check.", "");
			}
		}
	}
}


void CDlgIOSet::OnBnClickedButtonOk()
{
	Save();
	CBCGPDialog::OnOK();
}


void CDlgIOSet::OnBnClickedCancel()
{
	CBCGPDialog::OnCancel();
}


void CDlgIOSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if (m_Image.IsNull())
	{
		return;
	}
	CRect rcBmp;
	GetDlgItem(IDC_STATIC_DELAY_DIAGRAM)->GetWindowRect(rcBmp);
	ScreenToClient(rcBmp);
	CPaintDC dcc(GetDlgItem(IDC_STATIC_DELAY_DIAGRAM));

	::SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);
	::SetBrushOrgEx(dc.GetSafeHdc(), 0, 0, NULL);
	m_Image.Draw(dc.GetSafeHdc(),rcBmp);

}
void CDlgIOSet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
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

void CDlgIOSet::OnBnClickedButtonIoGreen()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_pCommuicate->m_IOCard[0].hasCard)//红0 绿1 黄2 蜂鸣器3  心跳信号OUT7 
	{
		MessageBox("没找到IO卡！");
		return;
	}
	int index = m_ChoseIO_Green.GetCurSel();
	if (m_BtnClickNum_Green == 0)
	{
		GetDlgItem(IDC_BUTTON_IO_GREEN)->SetWindowText("绿灯关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}
	else if (m_BtnClickNum_Green == 1)
	{
		GetDlgItem(IDC_BUTTON_IO_GREEN)->SetWindowText("绿灯开");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 0);
	}
	else if (m_BtnClickNum_Green == 2)
	{
		m_BtnClickNum_Green = 0;
		GetDlgItem(IDC_BUTTON_IO_GREEN)->SetWindowText("绿灯关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}
		
	m_BtnClickNum_Green++;
}


void CDlgIOSet::OnBnClickedButtonIoRed()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_pCommuicate->m_IOCard[0].hasCard)//红0 绿1 黄2 蜂鸣器3  心跳信号OUT7 
	{
		MessageBox("没找到IO卡！");
		return;
	}	

	int index = m_ChoseIO_Red.GetCurSel();

	if (m_BtnClickNum_Red == 0)
	{
		GetDlgItem(IDC_BUTTON_IO_RED)->SetWindowText("红灯关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}
	else if (m_BtnClickNum_Red == 1)
	{
		GetDlgItem(IDC_BUTTON_IO_RED)->SetWindowText("红灯开");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 0);
	}
	else if (m_BtnClickNum_Red == 2)
	{
		m_BtnClickNum_Red = 0;
		GetDlgItem(IDC_BUTTON_IO_RED)->SetWindowText("红灯关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}

	m_BtnClickNum_Red++;
}


void CDlgIOSet::OnBnClickedButtonIoBuzeer()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_pCommuicate->m_IOCard[0].hasCard)//红0 绿1 黄2 蜂鸣器3  心跳信号OUT7 
	{
		MessageBox("没找到IO卡！");
		return;
	}
	int index = m_ChoseIO_Buzzer.GetCurSel();
	if (m_BtnClickNum_Buzeer == 0)
	{
		GetDlgItem(IDC_BUTTON_IO_BUZEER)->SetWindowText("蜂鸣器关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}
	else if (m_BtnClickNum_Buzeer == 1)
	{
		GetDlgItem(IDC_BUTTON_IO_BUZEER)->SetWindowText("蜂鸣器开");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 0);
	}
	else if (m_BtnClickNum_Buzeer == 2)
	{
		m_BtnClickNum_Buzeer = 0;
		GetDlgItem(IDC_BUTTON_IO_BUZEER)->SetWindowText("蜂鸣器关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}	
	m_BtnClickNum_Buzeer++;
}

void CDlgIOSet::OnBnClickedButtonIoYellow()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_pCommuicate->m_IOCard[0].hasCard)//红0 绿1 黄2 蜂鸣器3  心跳信号OUT7 
	{
		MessageBox("没找到IO卡！");
		return;
	}
	int index = m_ChoseIO_Yellow.GetCurSel();
	if (m_BtnClickNum_Yellow == 0)
	{
		GetDlgItem(m_BtnClickNum_Yellow)->SetWindowText("黄灯关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}
	else if (m_BtnClickNum_Yellow == 1)
	{
		GetDlgItem(m_BtnClickNum_Yellow)->SetWindowText("黄灯开");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 0);
	}
	else if (m_BtnClickNum_Yellow == 2)
	{
		m_BtnClickNum_Yellow = 0;
		GetDlgItem(m_BtnClickNum_Yellow)->SetWindowText("黄灯关");
		m_pCommuicate->m_IOCard[0].setCardOutput(index, 1);
	}

	m_BtnClickNum_Yellow++;
}
