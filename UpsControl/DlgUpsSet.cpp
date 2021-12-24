// DlgUpsSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UpsControl.h"
#include "DlgUpsSet.h"
#include "afxdialogex.h"


// CDlgUpsSet �Ի���

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


// CDlgUpsSet ��Ϣ�������
BOOL CDlgUpsSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
void CDlgUpsSet::InitUI()
{
	CSerialPortExt Port;
	Port.GetComList(m_PortList);//�����ʼ�����Զ���ȡ���ô���
	for (int i = 0; i < m_PortList.size(); i++)
	{
		m_SerialPortList.AddString(TEXT(m_PortList[i]));
	}

	int index = m_SerialPortList.GetCurSel();
	m_SerialPortList.SetCurSel(index);//�����±�

	((CButton*)GetDlgItem(IDC_CHECK_UPS_STATUS))->SetCheck((bool)g_This->m_checkUpsStatus);

	UpdateData(FALSE);
}
BOOL CDlgUpsSet:: OpenSerialPort()
{
	try
	{	
		if (m_SerialPortList.GetCurSel() == -1)
		{
			BCGPMessageBox("��ѡ��Ҫ�򿪵Ĵ��ڣ�");
			return FALSE;
		}
		m_SerialPortList.GetLBText(m_SerialPortList.GetCurSel(), CurPort);
		g_This->GetLightParam().nComName = CurPort;

		if (g_This->Open())
		{
			//�༭����ʾ�򿪽��
			CString str;
			str.Format("�ɹ��򿪴���%s��\r\n", CurPort);
			m_Record += TEXT(str);
		}
		else
		{
			m_Record += TEXT("�򿪴���ʧ��%s��\r\n", CurPort);
		}

		UpdateData(FALSE); // ���������ݣ�ͬ�����ؼ��У�
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
			m_Record += TEXT("���ڹرճɹ�\r\n");
		}
		else
		{
			m_Record += TEXT("���ڹر�ʧ��\r\n");
		}

		UpdateData(FALSE); // ���������ݣ�ͬ�����ؼ��У�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	try
	{
		CString sContent;
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->GetWindowTextA(sContent);
		if (sContent == "�򿪴���")
		{
			if (OpenSerialPort())
			{
				((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetWindowTextA("�رմ���");
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
				((CBCGPButton*)GetDlgItem(IDC_BUTTON_OPEN))->SetWindowTextA("�򿪴���");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	try
	{
		//1.��������
		CString recv;
		CLockHelper lk(&CUpsSerial::m_csLock);
		bool ret = g_This->SetUpsConnect(recv);

		if (ret == true)
		{
			m_Record += TEXT("�������ӳɹ�" + recv + "\r\n");
		}
		else
		{
			m_Record += TEXT("��������ʧ��" + recv + "\r\n");
		}

		UpdateData(FALSE); // ���������ݣ�ͬ�����ؼ��У�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	try
	{
		//2.�����ֶ����ָ��
		CString statue;
		CLockHelper lk(&CUpsSerial::m_csLock);
		g_This->GetUpsStates(statue);
		m_Record += TEXT(statue + "\r\n");

		UpdateData(FALSE); // ���������ݣ�ͬ�����ؼ��У�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	OnCancel();
}
