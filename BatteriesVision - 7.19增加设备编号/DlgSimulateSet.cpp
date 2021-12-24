// DlgSimulateSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgSimulateSet.h"
#include "afxdialogex.h"


// CDlgSimulateSet �Ի���

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


// CDlgSimulateSet ��Ϣ�������


BOOL CDlgSimulateSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CBCGPDialog::PreTranslateMessage(pMsg);
}


BOOL CDlgSimulateSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgSimulateSet::InitUI()
{
	m_EDT_SimulatePath.EnableFolderBrowseButton("����ͼ��·��");
	GetDlgItem(IDC_EDIT_SIMULATE_PATH)->SetWindowTextA(theGlobal.m_Storage.GetImageSaveFolderPath());
	GetDlgItem(IDC_EDIT_SIMULATE_TIME)->SetWindowTextA("150");
}

void CDlgSimulateSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTmp;
	GetDlgItem(IDOK)->GetWindowTextA(strTmp);
	if (strTmp == "��ʼ����")
	{
		GetDlgItem(IDOK)->SetWindowTextA("ֹͣ����");


		((CBCGPEdit*)GetDlgItem(IDC_EDIT_SIMULATE_PATH))->GetWindowText(strTmp);
		theGlobal.m_Process.m_strSimulateGrabPath = strTmp;
		GetDlgItem(IDC_EDIT_SIMULATE_TIME)->GetWindowTextA(strTmp);
		theGlobal.m_Process.m_nSimulateGrabTime = atoi(strTmp);

		theGlobal.m_Process.StartSimulateGrab();
	}
	else
	{
		GetDlgItem(IDOK)->SetWindowTextA("��ʼ����");
		theGlobal.m_Process.StopSimulateGrab();
	}
}


void CDlgSimulateSet::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	theGlobal.m_Process.StopSimulateGrab();
	OnCancel();
}
