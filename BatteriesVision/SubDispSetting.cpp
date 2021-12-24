// SubDispSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "SubDispSetting.h"
#include "afxdialogex.h"


// CSubDispSetting 对话框

IMPLEMENT_DYNAMIC(CSubDispSetting, CBCGPDialog)

CSubDispSetting::CSubDispSetting(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CSubDispSetting::IDD, pParent)
{
	m_vecPages.clear();
}

CSubDispSetting::~CSubDispSetting()
{

}

void CSubDispSetting::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SET, m_Tab_Set);
}


BEGIN_MESSAGE_MAP(CSubDispSetting, CBCGPDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SET, &CSubDispSetting::OnTcnSelchangeTabSet)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CSubDispSetting 消息处理程序


BOOL CSubDispSetting::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSubDispSetting::InitUI()
{
	BOOL bChinese = theGlobal.m_Language.IsChinese();
	m_Tab_Set.InsertItem(0, bChinese ? "相机设置" : "Camera Settings");
	m_Tab_Set.InsertItem(1, bChinese ? "算法设置" : "Algorithm Settings");
	m_Tab_Set.InsertItem(2, bChinese ? "系统设置" : "System Settings");
	m_Tab_Set.InsertItem(3, bChinese ? "存储设置" : "Storage Settings");
	m_Tab_Set.InsertItem(4, bChinese ? "通信设置" : "Comm Settings");
	m_Tab_Set.InsertItem(5, bChinese ? "配方设置" : "Product Settings"); 
	m_Tab_Set.InsertItem(6, bChinese ? "点检设置" : "Check Settings");
	m_Tab_Set.InsertItem(7, bChinese ? "其他设置" : "Other Settings");

	m_Tab_Set.SetTextFont("Microsoft Yahei",19,0);

	//m_Tab_Set.SetColor(RGB(58, 58, 63), RGB(80, 80, 85), RGB(80, 80, 85), RGB(80, 80, 85), RGB(200, 200, 200), RGB(255, 255, 255));
	m_Tab_Set.SetColor(RGB(48, 48, 48), RGB(87, 87, 87), RGB(87, 87, 87), RGB(87, 87, 87), RGB(200, 200, 200), RGB(255, 255, 255));
	m_Tab_Set.SetItemSize(CSize(140,30));
	m_Tab_Set.SetMinTabWidth(bChinese?100:135);

	CRect rcSubDlg,rcTabItem;
	GetDlgItem(IDC_TAB_SET)->GetClientRect(rcSubDlg);
	m_Tab_Set.GetItemRect(0,rcTabItem);
	rcSubDlg.top = rcSubDlg.top + rcTabItem.Height() + 3;
	rcSubDlg.left += 1;
	rcSubDlg.right -= 1;
	rcSubDlg.bottom -= 1;

	m_pPageCameraSet = new CPageCameraSet;
	m_pPageCameraSet->Create(CPageCameraSet::IDD,this);
	m_pPageCameraSet->MoveWindow(rcSubDlg);
	m_pPageCameraSet->ShowWindow(SW_NORMAL);
	m_pPageCameraSet->ResetSize();
	m_vecPages.push_back(m_pPageCameraSet);

	m_pPageAlgSet = new CPageAlgSet;
	m_pPageAlgSet->Create(CPageAlgSet::IDD,this);
	m_pPageAlgSet->MoveWindow(rcSubDlg);
	m_pPageAlgSet->ShowWindow(SW_HIDE);
	m_pPageAlgSet->ResetSize();
	m_vecPages.push_back(m_pPageAlgSet);

	m_pPageSystemSet = new CPageSystemSet;
	m_pPageSystemSet->Create(CPageSystemSet::IDD,this);
	m_pPageSystemSet->MoveWindow(rcSubDlg);
	m_pPageSystemSet->ShowWindow(SW_HIDE);
	m_pPageSystemSet->ResetSize();
	m_vecPages.push_back(m_pPageSystemSet);

	m_pPageStorageSet = new CPageStorageSet;
	m_pPageStorageSet->Create(CPageStorageSet::IDD,this);
	m_pPageStorageSet->MoveWindow(rcSubDlg);
	m_pPageStorageSet->ShowWindow(SW_HIDE);
	m_pPageStorageSet->ResetSize();
	m_vecPages.push_back(m_pPageStorageSet);

	m_pPageCommunicationSet = new CPageCommunicationSet;
	m_pPageCommunicationSet->Create(CPageCommunicationSet::IDD,this);
	m_pPageCommunicationSet->MoveWindow(rcSubDlg);
	m_pPageCommunicationSet->ShowWindow(SW_HIDE);
	m_pPageCommunicationSet->ResetSize();
	m_vecPages.push_back(m_pPageCommunicationSet);

	m_pPageProductSet = new CPageProductSet;
	m_pPageProductSet->Create(CPageProductSet::IDD,this);
	m_pPageProductSet->MoveWindow(rcSubDlg);
	m_pPageProductSet->ShowWindow(SW_HIDE);
	m_pPageProductSet->ResetSize();
	m_vecPages.push_back(m_pPageProductSet);

	m_pPageCheckSet = new CPageCheckSet;
	m_pPageCheckSet->Create(CPageCheckSet::IDD, this);
	m_pPageCheckSet->MoveWindow(rcSubDlg);
	m_pPageCheckSet->ShowWindow(SW_HIDE);
	m_pPageCheckSet->ResetSize();
	m_vecPages.push_back(m_pPageCheckSet);

	m_pPageOtherSet = new CPageOtherSet;
	m_pPageOtherSet->Create(CPageOtherSet::IDD,this);
	m_pPageOtherSet->MoveWindow(rcSubDlg);
	m_pPageOtherSet->ShowWindow(SW_HIDE);
	m_pPageOtherSet->ResetSize();
	m_vecPages.push_back(m_pPageOtherSet);

	m_Tab_Set.SetCurSel(CameraSet);
}


BOOL CSubDispSetting::PreTranslateMessage(MSG* pMsg)
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


void CSubDispSetting::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CSubDispSetting::ResetSize()
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


void CSubDispSetting::OnTcnSelchangeTabSet(NMHDR *pNMHDR, LRESULT *pResult)
{
	//if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, CPermission::Engineer, NULL)) return;
	*pResult = 0;
	SwitchDispPage(m_Tab_Set.GetCurSel());
}

void CSubDispSetting::SwitchDispPage(const int& nSN)
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

BOOL CSubDispSetting::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}
