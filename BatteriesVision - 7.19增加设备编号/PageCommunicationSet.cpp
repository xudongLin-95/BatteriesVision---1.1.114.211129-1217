// PageCommunicationSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageCommunicationSet.h"
#include "afxdialogex.h"


// CPageCommunicationSet �Ի���

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


// CPageCommunicationSet ��Ϣ�������


BOOL CPageCommunicationSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CPageCommunicationSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//����ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//����ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageCommunicationSet::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageCommunicationSet::ResetSize()
{
	InitUI();
}

void CPageCommunicationSet::InitUI()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_IO))->SetTooltip("Set labeling and IO params\r\n���ô���IO����");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_PLC))->SetTooltip("Set communication params width PLC\r\n������PLCͨ�Ų���");

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
		theGlobal.m_Language.MessageboxError("��IO����ʧ�ܡ�", "Fail to open IO settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetPlc()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_PLC, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::PLC))
	{
		theGlobal.m_Language.MessageboxError("��PLC����ʧ�ܡ�", "Fail to open PLC settings.", "");
	}
}

void CPageCommunicationSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CBCGPDialog::OnPaint()
	ShowHiddenSetting();
}


void CPageCommunicationSet::OnBnClickedButtonSetLight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_LightControl, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::LightController))
	{
		theGlobal.m_Language.MessageboxError("�򿪹�Դ����ʧ�ܡ�", "Fail to open light settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetMes()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Mes, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::MES))
	{
		theGlobal.m_Language.MessageboxError("��MES����ʧ�ܡ�", "Fail to open MES settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetMarking()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Marking, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::Marking))
	{
		theGlobal.m_Language.MessageboxError("����������ʧ�ܡ�", "Fail to open Marking settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetUps()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_UPS, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::UPS))
	{
		theGlobal.m_Language.MessageboxError("��UPS����ʧ�ܡ�", "Fail to open UPS settings.", "");
	}
}


void CPageCommunicationSet::OnBnClickedButtonSetTemperature()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Temperature, NULL)) return;
	if (!theGlobal.m_CommunicationTool.ShowParamDlg(CCommunicationTool::Temperature))
	{
		theGlobal.m_Language.MessageboxError("���¿�������ʧ�ܡ�", "Fail to open Temperature settings.", "");
	}
}
