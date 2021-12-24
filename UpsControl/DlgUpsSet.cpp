// DlgUpsSet.cpp : 实现文件
//

#include "stdafx.h"
#include "UpsControl.h"
#include "DlgUpsSet.h"
#include "afxdialogex.h"


// CDlgUpsSet 对话框

IMPLEMENT_DYNAMIC(CDlgUpsSet, CBCGPDialog)

CDlgUpsSet::CDlgUpsSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_UPS_SET, pParent)
	, m_Record(_T(""))
{

}

CDlgUpsSet::~CDlgUpsSet()
{
}

void CDlgUpsSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SERIAL_PORT_LIST, m_SerialPortList);
	DDX_Text(pDX, IDC_EDIT_RECORD, m_Record);
}

BEGIN_MESSAGE_MAP(CDlgUpsSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDlgUpsSet::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CON, &CDlgUpsSet::OnBnClickedButtonCon)
	ON_BN_CLICKED(IDC_BUTTON_CHECK, &CDlgUpsSet::OnBnClickedButtonCheck)
	ON_BN_CLICKED(IDC_BUTTON_SAVEEXIT, &CDlgUpsSet::OnBnClickedButtonSaveexit)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDlgUpsSet::OnBnClickedButtonExit)
END_MESSAGE_MAP()


// CDlgUpsSet 消息处理程序
BOOL CDlgUpsSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化

	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CDlgUpsSet::InitUI()
{
	CSerialPortExt Port;
	Port.GetComList(m_PortList);//程序初始化，自动获取可用串口
	for (int i = 0; i < m_PortList.size(); i++)
	{
		m_SerialPortList.AddString(TEXT(m_PortList[i]));
	}

	int index = m_SerialPortList.GetCurSel();
	m_SerialPortList.SetCurSel(index);//串口下标

	((CButton*)GetDlgItem(IDC_CHECK_UPS_STATUS))->SetCheck((bool)g_This->m_checkUpsStatus);

	UpdateData(FALSE);
}
BOOL CDlgUpsSet:: OpenSerialPort()
{
	try
	{	
		if (m_SerialPortList.GetCurSel() == -1)
		{
			BCGPMessageBox("请选择要打开的串口！");
			return FALSE;
		}
		m_SerialPortList.GetLBText(m_SerialPortList.GetCurSel(), CurPort);
		g_This->GetLightParam().nComName = CurPort;

		if (g_This->Open())
		{
			//编辑框显示打开结果
			CString str;
			str.Format("成功打开串口%s！\r\n", CurPort);
			m_Record += TEXT(str);
		}
		else
		{
			m_Record += TEXT("打开串口失败%s！\r\n", CurPort);
		}

		UpdateData(FALSE); // 将变量内容，同步到控件中；
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CDlgUpsSet:: CloseSerialPort()
{
	try
	{
		if (g_This->Close())
		{
			m_Record += TEXT("串口关闭成功\r\n");
		}
		else
		{
			m_Record += TEXT("串口关闭失败\r\n");
		}

		UpdateData(FALSE); // 将变量内容，同步到控件中；
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
void CDlgUpsSet::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	try
	{
		CString sContent;
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->GetWindowTextA(sContent);
		if (sContent == "打开串口")
		{
			if (OpenSerialPort())
			{
				((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetWindowTextA("关闭串口");
				((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetFaceColor(RGB(0, 255, 0));
			}
			else
			{
				((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetFaceColor(RGB(255, 0, 0));
			}
		}
		else
		{
			if (CloseSerialPort())
			{
				((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetWindowTextA("打开串口");
				((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetFaceColor(RGB(255, 0, 0));
			}		
		}		
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}	
}

void CDlgUpsSet::OnBnClickedButtonCon()
{
	// TODO: 在此添加控件通知处理程序代码

	try
	{
		//1.建立连接
		CString recv;
		CLockHelper lk(&CUpsSerial::m_csLock);
		bool ret = g_This->SetUpsConnect(recv);

		if (ret == true)
		{
			m_Record += TEXT("建立连接成功" + recv + "\r\n");
		}
		else
		{
			m_Record += TEXT("建立连接失败" + recv + "\r\n");
		}

		UpdateData(FALSE); // 将变量内容，同步到控件中；
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}
}


void CDlgUpsSet::OnBnClickedButtonCheck()
{
	// TODO: 在此添加控件通知处理程序代码

	try
	{
		//2.发送手动监测指令
		CString statue;
		CLockHelper lk(&CUpsSerial::m_csLock);
		g_This->GetUpsStates(statue);
		m_Record += TEXT(statue + "\r\n");

		UpdateData(FALSE); // 将变量内容，同步到控件中；
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}
}


void CDlgUpsSet::OnBnClickedButtonSaveexit()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	g_This->m_checkUpsStatus = ((CButton*)GetDlgItem(IDC_CHECK_UPS_STATUS))->GetCheck() == TRUE;

	int index = m_SerialPortList.GetCurSel();
	if(index>0)
	{ 
		m_SerialPortList.GetLBText(index, CurPort);
		g_This->m_tUpsParam.nComIndex = index;
		g_This->m_tUpsParam.nComName = CurPort;
	}
	else
	{
		g_This->m_tUpsParam.nComIndex = index;
		g_This->m_tUpsParam.nComName = "";
	}
	
	g_This->SaveParams();

	CBCGPDialog::OnOK();
}


void CDlgUpsSet::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码

	OnCancel();
}
