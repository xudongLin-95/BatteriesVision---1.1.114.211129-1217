// DlgTemperature.cpp : 实现文件
//

#include "stdafx.h"
#include "Temperature.h"
#include "afxdialogex.h"
#include "DlgTemperature.h"

// CDlgTemperature 对话框

IMPLEMENT_DYNAMIC(CDlgTemperature, CBCGPDialog)

CDlgTemperature::CDlgTemperature(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_TEMPERATURE, pParent)
	, m_Edit_TempNum(0)
	, m_Edit_IntervalTime(0)
	, m_Edit_ResponseTimeOut(0)
{

}

CDlgTemperature::~CDlgTemperature()
{
}

void CDlgTemperature::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SERIAL_PORT_LIST, m_SerialPortList);
	DDX_Control(pDX, IDC_LIST_TEMPERATURE, m_ListCtrl_Temperature);
	DDX_Text(pDX, IDC_EDIT_NUMBER, m_Edit_TempNum);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_Edit_IntervalTime);
	DDX_Text(pDX, IDC_EDIT_RESPONS_TIMEPUT, m_Edit_ResponseTimeOut);
}


BEGIN_MESSAGE_MAP(CDlgTemperature, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_SERIAL, &CDlgTemperature::OnBnClickedButtonOpenSerial)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_EXIT, &CDlgTemperature::OnBnClickedButtonSaveExit)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDlgTemperature::OnBnClickedButtonExit)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GET_TEMPERATURE, &CDlgTemperature::OnBnClickedButtonGetTemperature)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgTemperature::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgTemperature 消息处理程序

void CDlgTemperature::OnBnClickedButtonOpenSerial()
{
	// TODO: 在此添加控件通知处理程序代码
	
	UpdateData(TRUE);
	int index = m_SerialPortList.GetCurSel();
	if (index < 0)
	{
		BCGPMessageBox("串口选择不能为空!");
		return;
	}

	CString CurPort;
	m_SerialPortList.GetLBText(index, CurPort);
	g_This->m_tParam.nComIndex = index;
	g_This->m_tParam.sComName = CurPort;

	CString sContent;
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->GetWindowTextA(sContent);
	if (sContent == "打开串口")
	{
		if (g_This->m_nCurrentStatus)
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->SetWindowTextA("关闭串口");
			return;
		}
		if (g_This->Open())
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->SetWindowTextA("关闭串口");
		}
	}
	else if (sContent == "关闭串口")
	{
		if (g_This->Close())
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->SetWindowTextA("打开串口");
		}
	}
}

BOOL CDlgTemperature::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle(TRUE, TRUE);

	// TODO:  在此添加额外的初始化
	InitUI();

	SetTimer(NULL, g_This->m_tParam.nIntervalTime * 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgTemperature::InitUI()
{
	CSerialPortExt Port;
	Port.GetComList(m_PortList);//程序初始化，自动获取可用串口
	for (int i = 0; i < m_PortList.size(); i++)
	{
		m_SerialPortList.AddString(TEXT(m_PortList[i]));
	}

	((CButton*)GetDlgItem(IDC_CHECK_USE))->SetCheck((bool)g_This->m_tParam.nUse);

	if (m_SerialPortList.GetCount() > 0)
		m_SerialPortList.SetCurSel(g_This->m_tParam.nComIndex);//串口下标
	
	m_Edit_TempNum = g_This->m_tParam.nTempNum;
	m_Edit_IntervalTime = g_This->m_tParam.nIntervalTime;
	m_Edit_ResponseTimeOut = g_This->m_tParam.nResponseTimeout;

	InitListCrtl();

	UpdateData(FALSE);//变量到控件
}
//初始化温度列表控件
void CDlgTemperature::InitListCrtl()
{
	// 获取编程语言列表视图控件的位置和大小
	CRect rect;
	m_ListCtrl_Temperature.GetClientRect(&rect);
	// 为列表视图控件添加全行选中和栅格风格   
	m_ListCtrl_Temperature.SetExtendedStyle(m_ListCtrl_Temperature.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 为列表视图控件添加列   
	m_ListCtrl_Temperature.InsertColumn(0, _T("温控器编号"), LVCFMT_CENTER, rect.Width() / 2, 0);
	m_ListCtrl_Temperature.InsertColumn(1, _T("温度"), LVCFMT_CENTER, rect.Width() / 2, 1);
	//添加成员
	CString strTempNo;
	for (int i = 0; i < g_This->m_tParam.nTempNum; i++)
	{
		strTempNo.Format("温控器%d", i+1);
		m_ListCtrl_Temperature.InsertItem(i, _T(strTempNo));
		m_ListCtrl_Temperature.SetItemText(i, 1, _T("0"));
	}	
}

void CDlgTemperature::OnBnClickedButtonSaveExit()
{
	// TODO: 在此添加控件通知处理程序代码
	g_This->m_tParam.nUse = ((CButton*)GetDlgItem(IDC_CHECK_USE))->GetCheck() == TRUE;//是否使用温度模块

	UpdateData(TRUE);
	g_This->m_tParam.nComIndex = m_SerialPortList.GetCurSel();//串口下标
	if (m_SerialPortList.GetCurSel() == -1)
	{
		g_This->m_tParam.sComName = "";
	}
	m_SerialPortList.GetLBText(g_This->m_tParam.nComIndex, g_This->m_tParam.sComName);//串口名字
	g_This->m_tParam.nResponseTimeout = m_Edit_ResponseTimeOut;//主机响应超时时间
	g_This->m_tParam.nTempNum = m_Edit_TempNum;//温控器个数
	g_This->m_tParam.nIntervalTime = m_Edit_IntervalTime;//读温度间隔时间

	if (m_Edit_TempNum > 9)
	{
		BCGPMessageBox("温控器个数需小于10个");
		return;
	}
	g_This->SaveParam();
	CBCGPDialog::OnOK();
}


void CDlgTemperature::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码

	CBCGPDialog::OnCancel();
}


void CDlgTemperature::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!this->IsWindowVisible() || g_This->m_vTempList.size() == 0) return;

	CLockHelper lk(&CControl::m_csLock);	
	CString strResult;
	for (int i = 0; i < g_This->m_vTempList.size();i++)
	{
		strResult.Format("%d", g_This->m_vTempList[i]);
		m_ListCtrl_Temperature.SetItemText(i, 1, _T(strResult));
	}
	//m_Edit_TempNum = g_This->m_vTempList.size();

	//UpdateData(FALSE);
	ShowWindow(SW_NORMAL);

	CBCGPDialog::OnTimer(nIDEvent);
}


void CDlgTemperature::OnBnClickedButtonGetTemperature()
{
	// TODO: 在此添加控件通知处理程序代码

	
}


void CDlgTemperature::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	g_This->GetTemp(g_This->m_vTempList);

	CString strResult;
	strResult.Format("%d", g_This->m_vTempList[0]);

	m_ListCtrl_Temperature.SetItemText(0, 1, _T(strResult));
	UpdateData(FALSE);
}
