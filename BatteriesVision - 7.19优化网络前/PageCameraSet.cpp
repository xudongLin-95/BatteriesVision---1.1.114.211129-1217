// PageCameraSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageCameraSet.h"
#include "afxdialogex.h"


// CPageCameraSet �Ի���

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


// CPageCameraSet ��Ϣ�������


BOOL CPageCameraSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CPageCameraSet::PreTranslateMessage(MSG* pMsg)
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


void CPageCameraSet::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageCameraSet::ResetSize()
{
	InitUI();
}

void CPageCameraSet::InitUI()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM0))->SetTooltip("Set camera 0 params\r\n�������0����");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM1))->SetTooltip("Set camera 1 params\r\n�������1����");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM2))->SetTooltip("Set camera 2 params\r\n�������2����");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_CAM3))->SetTooltip("Set camera 3 params\r\n�������3����");

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
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CBCGPDialog::OnPaint()

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