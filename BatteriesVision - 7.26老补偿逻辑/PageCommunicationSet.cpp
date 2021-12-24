// PageCommunicationSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageCommunicationSet.h"
#include "afxdialogex.h"


// CPageCommunicationSet 对话框

IMPLEMENT_DYNAMIC(CPageCommunicationSet, CBCGPDialog)

CPageCommunicationSet::CPageCommunicationSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageCommunicationSet::IDD, pParent)
{

}

CPageCommunicationSet::~CPageCommunicationSet()
{
}

void CPageCommunicationSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageCommunicationSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_IO, &CPageCommunicationSet::OnBnClickedButtonSetIo)
	ON_BN_CLICKED(IDC_BUTTON_SET_PLC, &CPageCommunicationSet::OnBnClickedButtonSetPlc)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_SET_LIGHT, &CPageCommunicationSet::OnBnClickedButtonSetLight)
	ON_BN_CLICKED(IDC_BUTTON_SET_MES, &CPageCommunicationSet::OnBnClickedButtonSetMes)
	ON_BN_CLICKED(IDC_BUTTON_SET_MARKING, &CPageCommunicationSet::OnBnClickedButtonSetMarking)
	ON_BN_CLICKED(IDC_BUTTON_SET_UPS, &CPageCommunicationSet::OnBnClickedButtonSetUps)
	ON_BN_CLICKED(IDC_BUTTON_SET_TEMPERATURE, &CPageCommunicationSet::OnBnClickedButtonSetTemperature)
END_MESSAGE_MAP()


// CPageCommunicationSet 消息处理程序


BOOL CPageCommunicationSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageCommunicationSet::PreTranslateMessage(MSG* pMsg)
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


void CPageCommunicationSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageCommunicationSet::ResetSize()
{
	InitUI();
}

void CPageCommunicationSet::InitUI()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_IO))->SetTooltip("Set labeling and IO params\r\n设置打标和IO参数");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_PLC))->SetTooltip("Set communication params width PLC\r\n设置与PLC通信参数");

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_COMMUNICATION_SET");
}

void CPageCommunicationSet::ShowHiddenSetting()
{
	/*if (theGlobal.m_Permission.CheckPermission(CPermission::Root))
	{
	}
	else
	{
	}*/
}

void CPageCommunicationSet::OnBnClickedButtonSetIo()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_IO, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::IO))
	{
		theGlobal.m_Language.MessageboxError("打开IO设置失败。", "Fail to open IO settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetPlc()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_PLC, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::PLC))
	{
		theGlobal.m_Language.MessageboxError("打开PLC设置失败。", "Fail to open PLC settings.", "");
	}
}

void CPageCommunicationSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CBCGPDialog::OnPaint()
	ShowHiddenSetting();
}


void CPageCommunicationSet::OnBnClickedButtonSetLight()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_LightControl, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::LightController))
	{
		theGlobal.m_Language.MessageboxError("打开光源设置失败。", "Fail to open light settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetMes()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Mes, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::MES))
	{
		theGlobal.m_Language.MessageboxError("打开MES设置失败。", "Fail to open MES settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetMarking()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Marking, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::Marking))
	{
		theGlobal.m_Language.MessageboxError("打开喷码设置失败。", "Fail to open Marking settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetUps()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_UPS, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::UPS))
	{
		theGlobal.m_Language.MessageboxError("打开UPS设置失败。", "Fail to open UPS settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetTemperature()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Temperature, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::Temperature))
	{
		theGlobal.m_Language.MessageboxError("打开温控器设置失败。", "Fail to open Temperature settings.", "");
	}
}
