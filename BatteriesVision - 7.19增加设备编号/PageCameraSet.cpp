// PageCameraSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageCameraSet.h"
#include "afxdialogex.h"


// CPageCameraSet 对话框

IMPLEMENT_DYNAMIC(CPageCameraSet, CBCGPDialog)

CPageCameraSet::CPageCameraSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageCameraSet::IDD, pParent)
{

}

CPageCameraSet::~CPageCameraSet()
{
}

void CPageCameraSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageCameraSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_CAM0, &CPageCameraSet::OnBnClickedButtonSetCam0)
	ON_BN_CLICKED(IDC_BUTTON_SET_CAM1, &CPageCameraSet::OnBnClickedButtonSetCam1)
	ON_BN_CLICKED(IDC_BUTTON_SET_CAM2, &CPageCameraSet::OnBnClickedButtonSetCam2)
	ON_BN_CLICKED(IDC_BUTTON_SET_CAM3, &CPageCameraSet::OnBnClickedButtonSetCam3)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPageCameraSet 消息处理程序


BOOL CPageCameraSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageCameraSet::PreTranslateMessage(MSG* pMsg)
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


void CPageCameraSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageCameraSet::ResetSize()
{
	InitUI();
}

void CPageCameraSet::InitUI()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM0))->SetTooltip("Set camera 0 params\r\n设置相机0参数");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM1))->SetTooltip("Set camera 1 params\r\n设置相机1参数");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM2))->SetTooltip("Set camera 2 params\r\n设置相机2参数");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM3))->SetTooltip("Set camera 3 params\r\n设置相机3参数");

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_CAMERA_SET");

	if (theGlobal.m_Project.GetProjectConfig().nCamCount <= 3)
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM3))->ShowWindow(SW_HIDE);
	if (theGlobal.m_Project.GetProjectConfig().nCamCount <= 2)
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM2))->ShowWindow(SW_HIDE);
	if (theGlobal.m_Project.GetProjectConfig().nCamCount <= 1)
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM1))->ShowWindow(SW_HIDE);
}


void CPageCameraSet::OnBnClickedButtonSetCam0()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Camera, NULL)) return;
	theGlobal.m_GrabberTool.ShowParamDlg(0);
}


void CPageCameraSet::OnBnClickedButtonSetCam1()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Camera, NULL)) return;
	theGlobal.m_GrabberTool.ShowParamDlg(1);
}

void CPageCameraSet::OnBnClickedButtonSetCam2()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Camera, NULL)) return;
	theGlobal.m_GrabberTool.ShowParamDlg(2);
}


void CPageCameraSet::OnBnClickedButtonSetCam3()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Camera, NULL)) return;
	theGlobal.m_GrabberTool.ShowParamDlg(3);
}


void CPageCameraSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBCGPDialog::OnPaint()

	ShowMesageJudgeByPowerLimit();
}

void CPageCameraSet::ShowMesageJudgeByPowerLimit()
{
	try
	{
		if (theGlobal.m_Permission.CheckPermission(PM_Camera))
		{
			GetDlgItem(IDC_BUTTON_SET_CAM0)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SET_CAM1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SET_CAM2)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SET_CAM3)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_SET_CAM0)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SET_CAM1)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SET_CAM2)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SET_CAM3)->EnableWindow(FALSE);
		}
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}
}