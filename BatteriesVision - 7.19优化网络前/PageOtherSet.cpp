// PageOtherSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageOtherSet.h"
#include "afxdialogex.h"
#include "AboutDlg.h"


// CPageOtherSet 对话框

IMPLEMENT_DYNAMIC(CPageOtherSet, CBCGPDialog)

CPageOtherSet::CPageOtherSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageOtherSet::IDD, pParent)
{

}

CPageOtherSet::~CPageOtherSet()
{
}

void CPageOtherSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageOtherSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CPageOtherSet::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_VERIFY_INFO, &CPageOtherSet::OnBnClickedButtonVerifyInfo)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CPageOtherSet::OnBnClickedButtonAbout)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_USER_MANAGE, &CPageOtherSet::OnBnClickedButtonUserManage)
	ON_BN_CLICKED(IDC_BUTTON_USER_POWER, &CPageOtherSet::OnBnClickedButtonUserPower)
END_MESSAGE_MAP()


// CPageOtherSet 消息处理程序


BOOL CPageOtherSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageOtherSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageOtherSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageOtherSet::ResetSize()
{
	InitUI();
}

void CPageOtherSet::InitUI()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_RESTART))->SetTooltip("Restart this software\r\n重启此软件");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_VERIFY_INFO))->SetTooltip("Check verify info of this software\r\n检查此软件的验证信息");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_ABOUT))->SetTooltip("About this software\r\n关于此软件");

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_OTHER_SET");
}

void CPageOtherSet::OnBnClickedButtonRestart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (theGlobal.m_Language.MessageboxQuestion("是否重启此程序?", "Whether to restart this program?", ""))return;


	theGlobal.PostMessageToMainDlg(WM_RESTARTPROCESS);
}


void CPageOtherSet::OnBnClickedButtonVerifyInfo()
{
	theGlobal.ShowVerifyInfo();
}


void CPageOtherSet::OnBnClickedButtonAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


BOOL CPageOtherSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageOtherSet::OnBnClickedButtonUserManage()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Manage, NULL)) return;
	theGlobal.m_Permission.ShowUserManage();
}


void CPageOtherSet::OnBnClickedButtonUserPower()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Manage, NULL)) return;
	theGlobal.m_Permission.ShowUserPower();
}
