// DlgMarkingParamSet.cpp : 实现文件
//

#include "stdafx.h"
#include "Marking.h"
#include "DlgMarkingParamSet.h"
#include "afxdialogex.h"


// CDlgMarkingParamSet 对话框

IMPLEMENT_DYNAMIC(CDlgMarkingParamSet, CBCGPDialog)

CDlgMarkingParamSet::CDlgMarkingParamSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_PLC_MARKINGSET, pParent)
	, m_EDIT_nPort(0)
	, m_EDIT_Diff(_T(""))
	, m_Edit_DivImageNum(_T(""))
	, m_Edit_MarkingWidth(_T(""))
	, m_Edit_MarkingHeight(_T(""))
{

}

CDlgMarkingParamSet::~CDlgMarkingParamSet()
{
}

void CDlgMarkingParamSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MARKINGPORT, m_EDIT_nPort);

	DDX_Text(pDX, IDC_EDIT_IMGDIFF, m_EDIT_Diff);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_DivImageNum);
	DDX_Text(pDX, IDC_EDIT4, m_Edit_MarkingWidth);
	DDX_Text(pDX, IDC_EDIT5, m_Edit_MarkingHeight);
}


BEGIN_MESSAGE_MAP(CDlgMarkingParamSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgMarkingParamSet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SENDDATA, &CDlgMarkingParamSet::OnBnClickedButtonSenddata)
	ON_BN_CLICKED(IDC_BUTTON_RECEIVEDATA, &CDlgMarkingParamSet::OnBnClickedButtonReceivedata)
	ON_BN_CLICKED(IDC_BUTTON_ISCONNECTED, &CDlgMarkingParamSet::OnBnClickedButtonIsconnected)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgMarkingParamSet::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CDlgMarkingParamSet 消息处理程序

BOOL CDlgMarkingParamSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
		{
			return TRUE;
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CDlgMarkingParamSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CBCGPDialog::OnPaint()
}


BOOL CDlgMarkingParamSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CDlgMarkingParamSet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
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


HBRUSH CDlgMarkingParamSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


LRESULT CDlgMarkingParamSet::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CBCGPDialog::OnNcHitTest(point);
}

BOOL CDlgMarkingParamSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgMarkingParamSet::InitUI()
{
	((CButton*)GetDlgItem(IDC_CHECK_ISCOMMU))->SetCheck((bool)m_pOwner->m_tSocketSet.nUseCommu);
	((CButton*)GetDlgItem(IDC_CHECK_DEFEAT))->SetCheck((bool)m_pOwner->m_tSocketSet.nUseCheckDefeat);
	((CButton*)GetDlgItem(IDC_CHECK_SIZE))->SetCheck((bool)m_pOwner->m_tSocketSet.nUseCheckSize);
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_MARKING))->SetWindowTextA(m_pOwner->m_tSocketSet.strIP);
	m_EDIT_nPort = m_pOwner->m_tSocketSet.nPort;

	CString strDiff;
	strDiff.Format(_T("%d"), m_pOwner->m_tSocketSet.nImgDiff);
	m_EDIT_Diff = strDiff;

	CString strDivImageNum;
	strDivImageNum.Format(_T("%d"), m_pOwner->m_tSocketSet.nDivImageNum);
	m_Edit_DivImageNum = strDivImageNum;

	CString strMarkingWidth;
	strMarkingWidth.Format(_T("%d"), m_pOwner->m_tSocketSet.nMarkingWidth);
	m_Edit_MarkingWidth = strMarkingWidth;

	CString strMarkingHeight;
	strMarkingHeight.Format(_T("%d"), m_pOwner->m_tSocketSet.nMarkingHeight);
	m_Edit_MarkingHeight = strMarkingHeight;

	UpdateData(FALSE);
	RefreshButton();

}

void CDlgMarkingParamSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_pOwner->m_tSocketSet.nPort= m_EDIT_nPort;
	m_pOwner->m_tSocketSet.nUseCommu=  ((CButton*)GetDlgItem(IDC_CHECK_ISCOMMU))->GetCheck() == TRUE;
	m_pOwner->m_tSocketSet.nUseCheckDefeat = ((CButton*)GetDlgItem(IDC_CHECK_DEFEAT))->GetCheck() == TRUE;
	m_pOwner->m_tSocketSet.nUseCheckSize = ((CButton*)GetDlgItem(IDC_CHECK_SIZE))->GetCheck() == TRUE;

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_MARKING))->GetWindowTextA(m_pOwner->m_tSocketSet.strIP);
	m_pOwner->m_tSocketSet.nImgDiff = _ttoi(m_EDIT_Diff);
	m_pOwner->m_tSocketSet.nDivImageNum = _ttoi(m_Edit_DivImageNum);
	m_pOwner->m_tSocketSet.nMarkingWidth = _ttoi(m_Edit_MarkingWidth);
	m_pOwner->m_tSocketSet.nMarkingHeight = _ttoi(m_Edit_MarkingHeight);
	m_pOwner->WriteParam();
	CBCGPDialog::OnOK();
}

void CDlgMarkingParamSet::OnBnClickedButtonSenddata()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sSendData;
	((CEdit*)GetDlgItem(IDC_EDIT_SENDDATA))->GetWindowTextA(sSendData);
	m_pOwner->DoSomething("SENDSTR",(void*)(&sSendData),NULL);

	//double dTotalMeter0;
	//m_pOwner->m_sInit.Main(MT_Marking, "ReadTotalMeter", NULL, &dTotalMeter0, NULL);

	//m_pOwner->testConnection();

}


void CDlgMarkingParamSet::OnBnClickedButtonReceivedata()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sReceiveData;
	m_pOwner->DoSomething("RECEIVEDATA", NULL, (void*)&sReceiveData);
	((CEdit*)GetDlgItem(IDC_EDIT_SENDDATA))->SetWindowTextA(sReceiveData);
}

void CDlgMarkingParamSet::RefreshButton()
{
	BOOL bRet = FALSE;
	m_pOwner->DoSomething("GetConnection", NULL, &bRet);
	if (bRet)
	{
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_ISCONNECTED))->SetWindowTextA("已连接");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_ISCONNECTED))->SetFaceColor(RGB(0, 255, 0));
	}
	else
	{
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_ISCONNECTED))->SetWindowTextA("未连接" );
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_ISCONNECTED))->SetFaceColor(RGB(255, 0, 0));
	}
}

void CDlgMarkingParamSet::OnBnClickedButtonIsconnected()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sContent;
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_ISCONNECTED))->GetWindowTextA(sContent);
	if (sContent == "已连接")
	{
		m_pOwner->DoSomething("RECONNECT", NULL, NULL);
		RefreshButton();
	}
	else
	{
		m_pOwner->DoSomething("RECONNECT", NULL, NULL);
		RefreshButton();
	}
}


void CDlgMarkingParamSet::OnBnClickedButtonCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
