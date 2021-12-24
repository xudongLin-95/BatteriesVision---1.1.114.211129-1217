// DlgSimulateSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgSimulateSet.h"
#include "afxdialogex.h"


// CDlgSimulateSet 对话框

IMPLEMENT_DYNAMIC(CDlgSimulateSet, CBCGPDialog)

CDlgSimulateSet::CDlgSimulateSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_SIMUATE_SET, pParent)
{

}

CDlgSimulateSet::~CDlgSimulateSet()
{
}

void CDlgSimulateSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SIMULATE_PATH, m_EDT_SimulatePath);
}


BEGIN_MESSAGE_MAP(CDlgSimulateSet, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgSimulateSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSimulateSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgSimulateSet 消息处理程序


BOOL CDlgSimulateSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CBCGPDialog::PreTranslateMessage(pMsg);
}


BOOL CDlgSimulateSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgSimulateSet::InitUI()
{
	m_EDT_SimulatePath.EnableFolderBrowseButton("仿真图像路径");
	GetDlgItem(IDC_EDIT_SIMULATE_PATH)->SetWindowTextA(theGlobal.m_Storage.GetImageSaveFolderPath());
	GetDlgItem(IDC_EDIT_SIMULATE_TIME)->SetWindowTextA("150");
}

void CDlgSimulateSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTmp;
	GetDlgItem(IDOK)->GetWindowTextA(strTmp);
	if (strTmp == "开始仿真")
	{
		GetDlgItem(IDOK)->SetWindowTextA("停止仿真");


		((CBCGPEdit*)GetDlgItem(IDC_EDIT_SIMULATE_PATH))->GetWindowText(strTmp);
		theGlobal.m_Process.m_strSimulateGrabPath = strTmp;
		GetDlgItem(IDC_EDIT_SIMULATE_TIME)->GetWindowTextA(strTmp);
		theGlobal.m_Process.m_nSimulateGrabTime = atoi(strTmp);

		theGlobal.m_Process.StartSimulateGrab();
	}
	else
	{
		GetDlgItem(IDOK)->SetWindowTextA("开始仿真");
		theGlobal.m_Process.StopSimulateGrab();
	}
}


void CDlgSimulateSet::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	theGlobal.m_Process.StopSimulateGrab();
	OnCancel();
}
