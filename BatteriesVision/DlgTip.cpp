// DlgTip.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgTip.h"
#include "afxdialogex.h"


// CDlgTip �Ի���

IMPLEMENT_DYNAMIC(CDlgTip, CBCGPDialog)

CDlgTip::CDlgTip(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_TIP, pParent)
{
	m_ftInfo.CreatePointFont(210, "����");

	InitializeCriticalSection(&m_csLock);
}

CDlgTip::~CDlgTip()
{
	m_ftInfo.DeleteObject();
}

void CDlgTip::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INFO, m_STC_Info);
}


BEGIN_MESSAGE_MAP(CDlgTip, CBCGPDialog)
	ON_WM_TIMER()
	ON_WM_NCDESTROY()
	ON_COMMAND_RANGE(IDC_BTN_BASE, IDC_BTN_BASE + 100, OnButtonClick)
END_MESSAGE_MAP()


// CDlgTip ��Ϣ�������


BOOL CDlgTip::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	::EnableMenuItem(::GetSystemMenu(this->m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);//���ùرհ�ť
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();
	SetTimer(NULL, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CDlgTip::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)//����ESC
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

void CDlgTip::ShowTip(eTipType eTip)
{
	tagTipInfo tInfo;
	if (TIP_CheckProduct == eTip)
	{
		tInfo.eType = eTip;
		tInfo.strDiscribe.Format("�Ƿ���к�λ�䷽ͬ��? ͬ��ǰ����ȷ�ϻ�ͷ�䷽����Ƿ���ȷ?");
		tInfo.vecOption.push_back("�䷽ͬ��");
		tInfo.vecOption.push_back("ȡ��");
	}
	else if (TIP_StopPosReason == eTip)
	{
		tInfo.eType = eTip;
		tInfo.strDiscribe.Format("�豸����ͣ��λ����ѡ��ͣ��ԭ��?");
		tInfo.vecOption.push_back("����־�");
		tInfo.vecOption.push_back("��������/������");
		tInfo.vecOption.push_back("��ͷ/�Ӵ�");
		tInfo.vecOption.push_back("�������/У��");
		tInfo.vecOption.push_back("����/©����");
		tInfo.vecOption.push_back("�δ�");
		tInfo.vecOption.push_back("�ϴ�");
		tInfo.vecOption.push_back("����ͣ��λ");
		tInfo.vecOption.push_back("����");
		tInfo.vecOption.push_back("�����쳣");
		tInfo.vecOption.push_back("˫ǿ��");
		tInfo.vecOption.push_back("�Զ�����ʧ��");
		tInfo.vecOption.push_back("����");
		tInfo.vecOption.push_back("������");
		tInfo.vecOption.push_back("������");
		tInfo.vecOption.push_back("����");
	}
	else if (TIP_FirstArticleNG == eTip)
	{
		tInfo.eType = eTip;
		tInfo.strDiscribe.Format("�׼�����ɣ�����ж�ΪNG");
		tInfo.vecOption.push_back("ȷ��");
	}
	else if (TIP_FirstArticleOK == eTip)
	{
		tInfo.eType = eTip;
		tInfo.strDiscribe.Format("�׼�����ɣ�����ж�ΪOK");
		tInfo.vecOption.push_back("ȷ��");
	}
	else if (TIP_FirstArticleCompenFinsh == eTip)
	{
		tInfo.eType = eTip;
		tInfo.strDiscribe.Format("�׼���ʼ�������������");
		tInfo.vecOption.push_back("ȷ��");
	}

	CLockHelper lk(&m_csLock);
	m_vecInfoList.push_back(tInfo);

	if (m_vecInfoList.size() > 10)
	{
		CString strExcp;
		strExcp.Format("CDlgTip_ShowTip: ��ʾ��Ϣ�������%d, �Ѻ��Բ�����Ϣ", 10);
		theGlobal.m_Log.Exception(strExcp);
	}
}

void CDlgTip::InitUI()
{
	m_STC_Info.SetBkColor(RGB(37, 37, 37));
	m_STC_Info.SetTextColor(RGB(255, 255, 255));//248, 182, 43
	m_STC_Info.SetFont(&m_ftInfo);
}

void CDlgTip::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (this->IsWindowVisible() || m_vecInfoList.size() == 0) return;

	m_tCurrentInfo = m_vecInfoList.front();

	int BTN_GAP_HEIGHT = 5;
	if (m_tCurrentInfo.eType == TIP_FirstArticleOK)
	{
		m_STC_Info.SetTextColor(RGB(0, 255, 0));
	}
	else if (m_tCurrentInfo.eType == TIP_FirstArticleNG)
	{
		m_STC_Info.SetTextColor(RGB(255, 0, 0));
	}
	else
	{
		m_STC_Info.SetTextColor(RGB(255, 255, 255));
	}
	m_STC_Info.Format(m_tCurrentInfo.strDiscribe);
	//m_STC_Info.Format("%s", m_tCurrentInfo.strDiscribe);
	CRect rcBtn, rcBtnTmp;
	GetDlgItem(IDC_BUTTON_CANCEL)->GetWindowRect(rcBtn);
	ScreenToClient(rcBtn);
	m_vecBtnList.clear();
	if (m_tCurrentInfo.vecOption.size() <= 8)
	{
		int nTop = rcBtn.bottom - rcBtn.Height()*m_tCurrentInfo.vecOption.size() + BTN_GAP_HEIGHT*(m_tCurrentInfo.vecOption.size()-1);
		for (int i = 0; i < m_tCurrentInfo.vecOption.size(); i++)
		{
			rcBtnTmp = rcBtn;
			rcBtnTmp.top = i*(rcBtn.Height() + BTN_GAP_HEIGHT) + nTop;
			rcBtnTmp.bottom = rcBtnTmp.top + rcBtn.Height();

			CBCGPButton* btn = new CBCGPButton;
			btn->Create(_T(m_tCurrentInfo.vecOption[i]), WS_CHILD | WS_VISIBLE /*| BS_PUSHBUTTON*/, rcBtnTmp, this, IDC_BTN_BASE + i);
			btn->SetFaceColor(RGB(48, 48, 48));
			btn->SetButtonStyle(CBCGPButton::BUTTONSTYLE_FLAT);
			m_vecBtnList.push_back(btn);
		}
	}
	else
	{
		int nTop = rcBtn.top - rcBtn.Height()*m_tCurrentInfo.vecOption.size()/2 + BTN_GAP_HEIGHT*(m_tCurrentInfo.vecOption.size()/2-1);
		int BTN_WIDTH = rcBtn.Width() / 2 - 4;
		int BTN_GAP_WIDTH = 8;
		for (int i = 0; i < m_tCurrentInfo.vecOption.size(); i++)
		{
			rcBtnTmp = rcBtn;
			rcBtnTmp.top = i/2*(rcBtn.Height() + BTN_GAP_HEIGHT) + nTop;
			rcBtnTmp.bottom = rcBtnTmp.top + rcBtn.Height();

			rcBtnTmp.left = rcBtn.left + i % 2 * (BTN_WIDTH + BTN_GAP_WIDTH);
			rcBtnTmp.right = rcBtnTmp.left + BTN_WIDTH;

			CBCGPButton* btn = new CBCGPButton;
			btn->Create(_T(m_tCurrentInfo.vecOption[i]), WS_CHILD | WS_VISIBLE /*| BS_PUSHBUTTON*/, rcBtnTmp, this, IDC_BTN_BASE + i);
			btn->SetFaceColor(RGB(48, 48, 48));
			btn->SetButtonStyle(CBCGPButton::BUTTONSTYLE_FLAT);
			m_vecBtnList.push_back(btn);
		}
	}

	CLockHelper lk(&m_csLock);
	m_vecInfoList.erase(m_vecInfoList.begin());

	ShowWindow(SW_NORMAL);
	CBCGPDialog::OnTimer(nIDEvent);
}

void CDlgTip::OnNcDestroy()
{
	CBCGPDialog::OnNcDestroy();

	// TODO: �ڴ˴������Ϣ����������
	delete this;
}

void CDlgTip::OnButtonClick(UINT uID)
{
	if (uID - IDC_BTN_BASE >= m_tCurrentInfo.vecOption.size()) return;

	if (TIP_CheckProduct == m_tCurrentInfo.eType)
	{
		if (m_tCurrentInfo.vecOption[uID - IDC_BTN_BASE] == "�䷽ͬ��"
			|| IDC_BTN_BASE == uID)
		{
			//�䷽ͬ��
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
			{
				theGlobal.m_Upload.SendProductInfoToClient();
			}
		}
	}
	else if (TIP_StopPosReason == m_tCurrentInfo.eType)
	{
		//ͳ��ͣ��λԭ��
		CString strReason = m_tCurrentInfo.vecOption[uID - IDC_BTN_BASE];
		theGlobal.m_Counter.SetCurrentStopDiscribe(strReason);
	}

	//ɾ����̬�����İ�ť
	CLockHelper lk(&m_csLock);
	for (int i = 0; i < m_vecBtnList.size(); i++)
	{
		m_vecBtnList[i]->DestroyWindow();
		delete m_vecBtnList[i];
	}

	m_vecBtnList.clear();

	ShowWindow(SW_HIDE);
}
