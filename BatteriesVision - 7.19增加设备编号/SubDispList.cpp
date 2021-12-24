// DlgDispList.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "SubDispList.h"
#include "afxdialogex.h"


// CSubDispList 对话框

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


// CSubDispList 消息处理程序


BOOL CSubDispList::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	theGlobal.m_pSubDispList = this;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CSubDispList::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CSubDispList::InitUI()
{
	BOOL bChinese = theGlobal.m_Language.IsChinese();
	m_TAB_List.InsertItem(0, bChinese ? "测宽数据" : "Measure Data");
	m_TAB_List.InsertItem(1, bChinese ? "瑕疵图像" : "Statistics Data");
	m_TAB_List.InsertItem(2, bChinese ? "点检数据" : "SpotInspection Data");
	m_TAB_List.InsertItem(3, bChinese ? "首件数据" : "FAI Data");

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
	CPoint OldTLPoint,TLPoint; //左上角  
	CPoint OldBRPoint,BRPoint; //右下角  
	HWND  hwndChild=::GetWindow(m_hWnd,GW_CHILD);  //列出所有控件 
	CWnd* pWnd = CWnd::FromHandle(m_hWnd);
	int woc; 
	CRect Rect;
	while(hwndChild)      
	{      
		woc=::GetDlgCtrlID(hwndChild);//取得ID  
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
	// TODO: 在此添加专用代码和/或调用基类
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
		strExcp.Format("PageDispMeasureWidth_UpdateList: 显示异常 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}
	

	try
	{
		m_pPageDispDefect->UpdateList(tOutput);
	}
	catch (HException& e)
	{
		strExcp.Format("PageDispDefect_UpdateList: 显示异常 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}

	try
	{
		m_pPageDispSpotInspection->UpdateList(tOutput);
	}
	catch (HException& e)
	{
		strExcp.Format("PageDispSpotInspection_UpdateList: 显示异常 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}

	try
	{
		m_pPageDispFAI->UpdateList(tOutput);
	}
	catch (HException& e)
	{
		strExcp.Format("PageDispFAI_UpdateList: 显示异常 %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strExcp);
	}
}

void CSubDispList::OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}
