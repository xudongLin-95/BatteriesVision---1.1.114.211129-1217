// DlgUploadAlarm.cpp: 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgUploadAlarm.h"
#include "afxdialogex.h"


// DlgUploadAlarm 对话框

IMPLEMENT_DYNAMIC(DlgUploadAlarm, CBCGPDialog)

DlgUploadAlarm::DlgUploadAlarm(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_DIALOG_UPLOAD_ALARM, pParent)
{
}

DlgUploadAlarm::~DlgUploadAlarm()
{
}

void DlgUploadAlarm::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_UPLOAD_ALARM, m_LIST_cUploadAlarm);
}


BEGIN_MESSAGE_MAP(DlgUploadAlarm, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &DlgUploadAlarm::OnBnClickedOk)
//	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// DlgUploadAlarm 消息处理程序

void DlgUploadAlarm::ShowAlarm(CString strAlarmInfo)
{	
	m_LIST_cUploadAlarm.InsertItem(0, strAlarmInfo);
	ShowWindow(SW_NORMAL);
}

void DlgUploadAlarm::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}


BOOL DlgUploadAlarm::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	m_LIST_cUploadAlarm.SetBkColor(RGB(58, 58, 63));
	m_LIST_cUploadAlarm.SetSelectionMark(RGB(58, 58, 63));
	m_LIST_cUploadAlarm.SetTextBkColor(RGB(255, 0, 0));
	m_LIST_cUploadAlarm.SetExtendedStyle(m_LIST_cUploadAlarm.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	CString  str[] = { "请注意！通信连接异常" };
	m_LIST_cUploadAlarm.InsertColumn(0, str[0], LVCFMT_LEFT, 1200);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void DlgUploadAlarm::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}


BOOL DlgUploadAlarm::PreTranslateMessage(MSG* pMsg)
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
		else if (pMsg->wParam == VK_SPACE)//
		{
			return TRUE;
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}
