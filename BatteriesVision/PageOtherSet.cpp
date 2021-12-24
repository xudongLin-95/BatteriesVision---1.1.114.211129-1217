// PageOtherSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageOtherSet.h"
#include "afxdialogex.h"
#include "AboutDlg.h"


// CPageOtherSet �Ի���

IMPLEMENT_DYNAMIC(CPageOtherSet, CBCGPDialog)

CPageOtherSet::CPageOtherSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageOtherSet::IDD, pParent)
	, m_EDIT_nLoginDelayTime(0)
{

}

CPageOtherSet::~CPageOtherSet()
{
}

void CPageOtherSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGIN_DELAY_TIME, m_EDIT_nLoginDelayTime);
}


BEGIN_MESSAGE_MAP(CPageOtherSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CPageOtherSet::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_VERIFY_INFO, &CPageOtherSet::OnBnClickedButtonVerifyInfo)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CPageOtherSet::OnBnClickedButtonAbout)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_USER_MANAGE, &CPageOtherSet::OnBnClickedButtonUserManage)
	ON_BN_CLICKED(IDC_BUTTON_USER_POWER, &CPageOtherSet::OnBnClickedButtonUserPower)
	ON_EN_CHANGE(IDC_EDIT_LOGIN_DELAY_TIME, &CPageOtherSet::OnEnChangeEditLoginDelayTime)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPageOtherSet ��Ϣ�������


BOOL CPageOtherSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CPageOtherSet::PreTranslateMessage(MSG* pMsg)
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


void CPageOtherSet::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageOtherSet::ResetSize()
{
	InitUI();
}

void CPageOtherSet::InitUI()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_RESTART))->SetTooltip("Restart this software\r\n���������");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_VERIFY_INFO))->SetTooltip("Check verify info of this software\r\n�����������֤��Ϣ");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_ABOUT))->SetTooltip("About this software\r\n���ڴ����");

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_OTHER_SET");

	m_EDIT_nLoginDelayTime = theGlobal.m_Permission.m_nLoginDelayTime;

	UpdateData(FALSE);
}

void CPageOtherSet::ShowMesageJudgeByPowerLimit()
{
	if (theGlobal.m_Permission.CheckPermission(PM_Manage))
	{
		//Ȩ������
		GetDlgItem(IDC_BUTTON_USER_MANAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_USER_POWER)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LOGIN_DELAY_TIME)->EnableWindow(TRUE);
	}
	else
	{
		//Ȩ������
		GetDlgItem(IDC_BUTTON_USER_MANAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_USER_POWER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LOGIN_DELAY_TIME)->EnableWindow(FALSE);
	}
}

void CPageOtherSet::OnBnClickedButtonRestart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (theGlobal.m_Language.MessageboxQuestion("�Ƿ������˳���?", "Whether to restart this program?", ""))return;


	theGlobal.PostMessageToMainDlg(WM_RESTARTPROCESS);
}


void CPageOtherSet::OnBnClickedButtonVerifyInfo()
{
	theGlobal.ShowVerifyInfo();
}


void CPageOtherSet::OnBnClickedButtonAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


BOOL CPageOtherSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageOtherSet::OnBnClickedButtonUserManage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Manage, NULL)) return;
	theGlobal.m_Permission.ShowUserManage();
}


void CPageOtherSet::OnBnClickedButtonUserPower()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Manage, NULL)) return;
	theGlobal.m_Permission.ShowUserPower();
}


void CPageOtherSet::OnEnChangeEditLoginDelayTime()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Permission.m_nLoginDelayTime, m_EDIT_nLoginDelayTime, "ע���ӳ�ʱ��(s)");
	theGlobal.m_Permission.m_nLoginDelayTime = m_EDIT_nLoginDelayTime;
	theGlobal.m_Permission.WriteParam();
}


void CPageOtherSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CBCGPDialog::OnPaint()

	ShowMesageJudgeByPowerLimit();
}
