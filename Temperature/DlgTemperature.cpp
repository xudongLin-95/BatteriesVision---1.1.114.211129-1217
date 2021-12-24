// DlgTemperature.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Temperature.h"
#include "afxdialogex.h"
#include "DlgTemperature.h"

// CDlgTemperature �Ի���

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


// CDlgTemperature ��Ϣ�������

void CDlgTemperature::OnBnClickedButtonOpenSerial()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	UpdateData(TRUE);
	int index = m_SerialPortList.GetCurSel();
	if (index < 0)
	{
		BCGPMessageBox("����ѡ����Ϊ��!");
		return;
	}

	CString CurPort;
	m_SerialPortList.GetLBText(index, CurPort);
	g_This->m_tParam.nComIndex = index;
	g_This->m_tParam.sComName = CurPort;

	CString sContent;
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->GetWindowTextA(sContent);
	if (sContent == "�򿪴���")
	{
		if (g_This->m_nCurrentStatus)
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->SetWindowTextA("�رմ���");
			return;
		}
		if (g_This->Open())
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->SetWindowTextA("�رմ���");
		}
	}
	else if (sContent == "�رմ���")
	{
		if (g_This->Close())
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN_SERIAL))->SetWindowTextA("�򿪴���");
		}
	}
}

BOOL CDlgTemperature::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle(TRUE, TRUE);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();

	SetTimer(NULL, g_This->m_tParam.nIntervalTime * 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgTemperature::InitUI()
{
	CSerialPortExt Port;
	Port.GetComList(m_PortList);//�����ʼ�����Զ���ȡ���ô���
	for (int i = 0; i < m_PortList.size(); i++)
	{
		m_SerialPortList.AddString(TEXT(m_PortList[i]));
	}

	((CButton*)GetDlgItem(IDC_CHECK_USE))->SetCheck((bool)g_This->m_tParam.nUse);

	if (m_SerialPortList.GetCount() > 0)
		m_SerialPortList.SetCurSel(g_This->m_tParam.nComIndex);//�����±�
	
	m_Edit_TempNum = g_This->m_tParam.nTempNum;
	m_Edit_IntervalTime = g_This->m_tParam.nIntervalTime;
	m_Edit_ResponseTimeOut = g_This->m_tParam.nResponseTimeout;

	InitListCrtl();

	UpdateData(FALSE);//�������ؼ�
}
//��ʼ���¶��б�ؼ�
void CDlgTemperature::InitListCrtl()
{
	// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С
	CRect rect;
	m_ListCtrl_Temperature.GetClientRect(&rect);
	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
	m_ListCtrl_Temperature.SetExtendedStyle(m_ListCtrl_Temperature.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// Ϊ�б���ͼ�ؼ������   
	m_ListCtrl_Temperature.InsertColumn(0, _T("�¿������"), LVCFMT_CENTER, rect.Width() / 2, 0);
	m_ListCtrl_Temperature.InsertColumn(1, _T("�¶�"), LVCFMT_CENTER, rect.Width() / 2, 1);
	//��ӳ�Ա
	CString strTempNo;
	for (int i = 0; i < g_This->m_tParam.nTempNum; i++)
	{
		strTempNo.Format("�¿���%d", i+1);
		m_ListCtrl_Temperature.InsertItem(i, _T(strTempNo));
		m_ListCtrl_Temperature.SetItemText(i, 1, _T("0"));
	}	
}

void CDlgTemperature::OnBnClickedButtonSaveExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_This->m_tParam.nUse = ((CButton*)GetDlgItem(IDC_CHECK_USE))->GetCheck() == TRUE;//�Ƿ�ʹ���¶�ģ��

	UpdateData(TRUE);
	g_This->m_tParam.nComIndex = m_SerialPortList.GetCurSel();//�����±�
	if (m_SerialPortList.GetCurSel() == -1)
	{
		g_This->m_tParam.sComName = "";
	}
	m_SerialPortList.GetLBText(g_This->m_tParam.nComIndex, g_This->m_tParam.sComName);//��������
	g_This->m_tParam.nResponseTimeout = m_Edit_ResponseTimeOut;//������Ӧ��ʱʱ��
	g_This->m_tParam.nTempNum = m_Edit_TempNum;//�¿�������
	g_This->m_tParam.nIntervalTime = m_Edit_IntervalTime;//���¶ȼ��ʱ��

	if (m_Edit_TempNum > 9)
	{
		BCGPMessageBox("�¿���������С��10��");
		return;
	}
	g_This->SaveParam();
	CBCGPDialog::OnOK();
}


void CDlgTemperature::OnBnClickedButtonExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CBCGPDialog::OnCancel();
}


void CDlgTemperature::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	
}


void CDlgTemperature::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_This->GetTemp(g_This->m_vTempList);

	CString strResult;
	strResult.Format("%d", g_This->m_vTempList[0]);

	m_ListCtrl_Temperature.SetItemText(0, 1, _T(strResult));
	UpdateData(FALSE);
}
