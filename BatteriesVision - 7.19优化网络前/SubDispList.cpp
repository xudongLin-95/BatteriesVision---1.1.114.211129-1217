// DlgDispList.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "SubDispList.h"
#include "afxdialogex.h"


// CSubDispList �Ի���

IMPLEMENT_DYNAMIC(CSubDispList, CBCGPDialog)

CSubDispList::CSubDispList(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CSubDispList::IDD, pParent)
{
	m_vecPages.clear();
}

CSubDispList::~CSubDispList()
{
}

void CSubDispList::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_LIST, m_TAB_List);
}


BEGIN_MESSAGE_MAP(CSubDispList, CBCGPDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_LIST, &CSubDispList::OnTcnSelchangeTabList)
	ON_REGISTERED_MESSAGE(WM_REFRESH_DISP,&CSubDispList::OnRefreshDisp)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CSubDispList ��Ϣ�������


BOOL CSubDispList::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	theGlobal.m_pSubDispList = this;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CSubDispList::PreTranslateMessage(MSG* pMsg)
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

void CSubDispList::InitUI()
{
	BOOL bChinese = theGlobal.m_Language.IsChinese();
	m_TAB_List.InsertItem(0, bChinese ? "�������" : "Measure Data");
	m_TAB_List.InsertItem(1, bChinese ? "覴�ͼ��" : "Statistics Data");
	m_TAB_List.InsertItem(2, bChinese ? "�������" : "SpotInspection Data");
	m_TAB_List.InsertItem(3, bChinese ? "�׼�����" : "FAI Data");

	//m_TAB_List.SetColor(RGB(58, 58, 63), RGB(80, 80, 85), RGB(80, 80, 85), RGB(80, 80, 85), RGB(200, 200, 200), RGB(255, 255, 255));
	m_TAB_List.SetColor(RGB(48, 48, 48), RGB(87, 87, 87), RGB(87, 87, 87), RGB(87, 87, 87), RGB(200, 200, 200), RGB(255, 255, 255));
	m_TAB_List.SetTextFont("Microsoft Yahei", 19, 0);
	m_TAB_List.SetItemSize(CSize(120, 30));
	m_TAB_List.SetMinTabWidth(bChinese ? 100 : 150);

	CRect rcSubDlg,rcTabItem;
	GetDlgItem(IDC_TAB_LIST)->GetClientRect(rcSubDlg);
	m_TAB_List.GetItemRect(0,rcTabItem);
	rcSubDlg.top = rcSubDlg.top + rcTabItem.Height() + 3;
	rcSubDlg.left += 1;
	rcSubDlg.right -= 1;
	rcSubDlg.bottom -= 1;

	m_pPageDispMeasureWidth = new CPageDispList;
	m_pPageDispMeasureWidth->Create(CPageDispList::IDD,this);
	m_pPageDispMeasureWidth->MoveWindow(rcSubDlg);
	m_pPageDispMeasureWidth->ShowWindow(SW_NORMAL);
	m_pPageDispMeasureWidth->ResetSize();
	m_vecPages.push_back(m_pPageDispMeasureWidth);

	m_pPageDispDefect = new CPageDispList2;
	m_pPageDispDefect->Create(CPageDispList2::IDD,this);
	m_pPageDispDefect->MoveWindow(rcSubDlg);
	m_pPageDispDefect->ShowWindow(SW_HIDE);
	m_pPageDispDefect->ResetSize();
	m_vecPages.push_back(m_pPageDispDefect);

	m_pPageDispSpotInspection = new CPageDispList3;
	m_pPageDispSpotInspection->Create(CPageDispList3::IDD, this);
	m_pPageDispSpotInspection->MoveWindow(rcSubDlg);
	m_pPageDispSpotInspection->ShowWindow(SW_HIDE);
	m_pPageDispSpotInspection->ResetSize();
	m_vecPages.push_back(m_pPageDispSpotInspection);

	m_pPageDispFAI = new CPageDispList4;
	m_pPageDispFAI->Create(CPageDispList4::IDD, this);
	m_pPageDispFAI->MoveWindow(rcSubDlg);
	m_pPageDispFAI->ShowWindow(SW_HIDE);
	m_pPageDispFAI->ResetSize();
	m_vecPages.push_back(m_pPageDispFAI);
}

void CSubDispList::ResetSize()
{
	CRect rtSize;
	this->GetWindowRect(rtSize);
	ScreenToClient(rtSize);

	LONG nSrcX(587),nSrcY(367);
	LONG nDstX = rtSize.Width();  
	LONG nDstY = rtSize.Height();  
	float fsp[2];
	fsp[0]=(float)nDstX/nSrcX;  
	fsp[1]=(float)nDstY/nSrcY;  
	CPoint OldTLPoint,TLPoint; //���Ͻ�  
	CPoint OldBRPoint,BRPoint; //���½�  
	HWND  hwndChild=::GetWindow(m_hWnd,GW_CHILD);  //�г����пؼ� 
	CWnd* pWnd = CWnd::FromHandle(m_hWnd);
	int woc; 
	CRect Rect;
	while(hwndChild)      
	{      
		woc=::GetDlgCtrlID(hwndChild);//ȡ��ID  
		pWnd->GetDlgItem(woc)->GetWindowRect(Rect);    
		pWnd->ScreenToClient(Rect);    
		OldTLPoint = Rect.TopLeft();    
		TLPoint.x = long(OldTLPoint.x*fsp[0]);    
		TLPoint.y = long(OldTLPoint.y*fsp[1]);    
		OldBRPoint = Rect.BottomRight();    
		BRPoint.x = long(OldBRPoint.x *fsp[0]);    
		BRPoint.y = long(OldBRPoint.y *fsp[1]);    
		Rect.SetRect(TLPoint,BRPoint);    
		pWnd->GetDlgItem(woc)->MoveWindow(Rect,TRUE);  
		hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);      
	}

	InitUI();
}

LRESULT CSubDispList::OnRefreshDisp(WPARAM wParam, LPARAM lParam)
{
	m_pPageDispMeasureWidth->ResetList();
	m_pPageDispDefect->ResetList();
	m_pPageDispSpotInspection->ResetList();
	//m_pPageDispRightFoil->ResetList();
	return 0L;
}

void CSubDispList::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CSubDispList::UpdateData(C_MergeWork& tOutput)
{
	CString strExcp;
	try
	{
		m_pPageDispMeasureWidth->UpdateList(tOutput);
	}
	catch (HException& e)
	{
		strExcp.Format("PageDispMeasureWidth_UpdateList: ��ʾ�쳣 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}
	

	try
	{
		m_pPageDispDefect->UpdateList(tOutput);
	}
	catch (HException& e)
	{
		strExcp.Format("PageDispDefect_UpdateList: ��ʾ�쳣 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}

	try
	{
		m_pPageDispSpotInspection->UpdateList(tOutput);
	}
	catch (HException& e)
	{
		strExcp.Format("PageDispSpotInspection_UpdateList: ��ʾ�쳣 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}

	try
	{
		m_pPageDispFAI->UpdateList(tOutput);
	}
	catch (HException& e)
	{
		strExcp.Format("PageDispFAI_UpdateList: ��ʾ�쳣 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}
}

void CSubDispList::OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	SwitchDispPage(m_TAB_List.GetCurSel());
}

void CSubDispList::SwitchDispPage(const int& nSN)
{
	if(nSN >= m_vecPages.size())return;

	for (int i=0;i<m_vecPages.size();i++)
	{
		if(nSN == i)
			m_vecPages[i]->ShowWindow(SW_NORMAL);
		else
			m_vecPages[i]->ShowWindow(SW_HIDE);
	}
}


BOOL CSubDispList::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}
