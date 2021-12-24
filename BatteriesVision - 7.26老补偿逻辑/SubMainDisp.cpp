// SubMianDisp.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "SubMainDisp.h"
#include "afxdialogex.h"


// CSubMainDisp 对话框

IMPLEMENT_DYNAMIC(CSubMainDisp, CBCGPDialog)

CSubMainDisp::CSubMainDisp(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_SUB_MAIN_DISP, pParent)
	,m_hvecMainImageHandle(1)
	,m_hvecTmpHandle(1)
{
	m_ftCountGrid.CreateFontA(18, 0, 0, 0, 550, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");
}

CSubMainDisp::~CSubMainDisp()
{
	m_ftCountGrid.DeleteObject();
}

void CSubMainDisp::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSubMainDisp, CBCGPDialog)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CSubMainDisp 消息处理程序


BOOL CSubMainDisp::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CSubMainDisp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if (pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CSubMainDisp::InitUI()
{
	InitList();
	InitWindowShow();
	InitSubDlg();

	OpenTimer();
}

void CSubMainDisp::ResetSize()
{
	CRect rtSize;
	this->GetWindowRect(rtSize);
	ScreenToClient(rtSize);

	LONG nSrcX(1917), nSrcY(946);
	LONG nDstX = rtSize.Width();
	LONG nDstY = rtSize.Height();
	float fsp[2];
	fsp[0] = (float)nDstX / nSrcX;
	fsp[1] = (float)nDstY / nSrcY;
	CPoint OldTLPoint, TLPoint; //左上角  
	CPoint OldBRPoint, BRPoint; //右下角  
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件 
	CWnd* pWnd = CWnd::FromHandle(m_hWnd);
	int woc;
	CRect Rect;
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//取得ID  
		pWnd->GetDlgItem(woc)->GetWindowRect(Rect);
		pWnd->ScreenToClient(Rect);
		OldTLPoint = Rect.TopLeft();
		TLPoint.x = long(OldTLPoint.x*fsp[0]);
		TLPoint.y = long(OldTLPoint.y*fsp[1]);
		OldBRPoint = Rect.BottomRight();
		BRPoint.x = long(OldBRPoint.x *fsp[0]);
		BRPoint.y = long(OldBRPoint.y *fsp[1]);
		Rect.SetRect(TLPoint, BRPoint);
		pWnd->GetDlgItem(woc)->MoveWindow(Rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}

	InitUI();
}

void CSubMainDisp::InitList()
{
	CBCGPGridColors theme;

	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID_BASE)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_Grid_Base.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Base, theme);
	m_Grid_Base.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Base.SetColorTheme(theme);
	m_Grid_Base.SetReadOnly();//可编辑
	m_Grid_Base.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_Base.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_Base.EnableRowHeader(FALSE);//使用行表头
	m_Grid_Base.EnableLineNumbers(FALSE);//使用行序号
	m_Grid_Base.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_Base.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Base.InsertColumn(0, theGlobal.m_Language.IsChinese() ? "日志" : "Log", rcGridBase.Width() - 40);
	m_Grid_Base.LoadState(_T("BasicGrid"));
	m_Grid_Base.AdjustLayout();//相当于数据显示在表格上
	m_Grid_Base.ShowWindow(SW_NORMAL);

	//GetDlgItem(IDC_STATIC_GRID_INFO)->GetWindowRect(rcGridBase);
	//ScreenToClient(rcGridBase);
	//m_Grid_Info.Create(WS_CHILD, rcGridBase, this, 1001);
	//CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Info, theme);
	//m_Grid_Info.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	//m_Grid_Info.SetColorTheme(theme);
	//m_Grid_Info.SetReadOnly();//可编辑
	//m_Grid_Info.EnableMarkSortedColumn(FALSE);//使用列排序
	//m_Grid_Info.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	//m_Grid_Info.EnableRowHeader(FALSE);//使用行表头
	//m_Grid_Info.EnableLineNumbers(FALSE);//使用行序号
	//m_Grid_Info.SetClearInplaceEditOnEnter(FALSE);
	//m_Grid_Info.EnableInvertSelOnCtrl(FALSE);
	//m_Grid_Info.SetFont(&m_ftCountGrid);
	//m_Grid_Info.InsertColumn(0, "常用信息", 150);
	//m_Grid_Info.InsertColumn(1, "详情", rcGridBase.Width() - 150 - 60);
	//m_Grid_Info.LoadState(_T("BasicGrid"));
	//m_Grid_Info.AdjustLayout();//相当于数据显示在表格上
	//m_Grid_Info.ShowWindow(SW_NORMAL);

	//int nCount = 0;
	//m_Grid_Info.RemoveAll();
	//CBCGPGridRow* pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("工厂编号"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.GetSystemSet().strWorkshopName), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("设备编号"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.GetSystemSet().strDeviceID), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("工号"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.GetCurrentJobNum()), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("上卷开始时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetLastBatch().GetStartTime()), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("上卷结束时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetLastBatch().GetEndTime()), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("上卷耗时"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetLastBatch().GetTimeSpanS()), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("当前卷开始时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetCurrentBatch().GetStartTime()), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//pRow = m_Grid_Info.CreateRow(m_Grid_Info.GetColumnCount());
	//pRow->GetItem(0)->SetValue(_variant_t("持续时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetCurrentBatch().GetTimeSpanSForNow()), FALSE);
	//m_Grid_Info.AddRow(pRow, FALSE);
	//m_Grid_Info.AdjustLayout();

	//RefreshBatchInfoList();

	//GetDlgItem(IDC_STATIC_GRID_INFO2)->GetWindowRect(rcGridBase);
	//ScreenToClient(rcGridBase);
	//m_Grid_Count.Create(WS_CHILD, rcGridBase, this, 1001);
	//CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Count, theme);
	//m_Grid_Count.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	//m_Grid_Count.SetColorTheme(theme);
	//m_Grid_Count.SetReadOnly();//可编辑
	//m_Grid_Count.EnableMarkSortedColumn(FALSE);//使用列排序
	//m_Grid_Count.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	//m_Grid_Count.EnableRowHeader(FALSE);//使用行表头
	//m_Grid_Count.EnableLineNumbers(FALSE);//使用行序号
	//m_Grid_Count.SetClearInplaceEditOnEnter(FALSE);
	//m_Grid_Count.EnableInvertSelOnCtrl(FALSE);
	//m_Grid_Count.SetFont(&m_ftCountGrid);
	//m_Grid_Count.InsertColumn(0, "统计", 150);
	//m_Grid_Count.InsertColumn(1, "当前卷", (rcGridBase.Width() - 150 - 60) / 2);
	//m_Grid_Count.InsertColumn(2, "上一卷", (rcGridBase.Width() - 150 - 60) / 2);
	//m_Grid_Count.LoadState(_T("BasicGrid"));
	//m_Grid_Count.AdjustLayout();//相当于数据显示在表格上
	//m_Grid_Count.ShowWindow(SW_NORMAL);

	//RefreshErrorCountList();
}

void CSubMainDisp::RefreshErrorCountList()
{
	//HWND hwndList;
	//hwndList = m_Grid_Count.GetSafeHwnd();
	//SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);//SetWindowRedraw
	/*m_Grid_Count.RemoveAll();
	CCounter::tagCount tCurCnt = theGlobal.m_Counter.GetCurrentCount();
	CCounter::tagCount tLstCnt = theGlobal.m_Counter.GetLastCount();
	CBCGPGridRow* pRow = m_Grid_Count.CreateRow(m_Grid_Count.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("总米数"), FALSE);
	pRow->GetItem(1)->SetValue(_variant_t(tCurCnt.dTotalMeter), FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(tLstCnt.dTotalMeter), FALSE);
	m_Grid_Count.AddRow(pRow, FALSE);
	pRow = m_Grid_Count.CreateRow(m_Grid_Count.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("总NG数"), FALSE);
	pRow->GetItem(1)->SetValue(_variant_t(tCurCnt.nNGCount), FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(tLstCnt.nNGCount), FALSE);
	m_Grid_Count.AddRow(pRow, FALSE);
	for (int i = 0; i < tCurCnt.vecNG.size(); i++)
	{
		if (!theGlobal.m_Counter.GetCountEnable(i)) continue;
		if (tCurCnt.vecNG.size() - 1 < i) continue;
		if (tCurCnt.vecNG[i].nCount == 0 &&
			tLstCnt.vecNG[i].nCount == 0) continue;

		pRow = m_Grid_Count.CreateRow(m_Grid_Count.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t(tCurCnt.vecNG[i].strName + "数"), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t(tCurCnt.vecNG[i].nCount), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(tLstCnt.vecNG[i].nCount), FALSE);
		m_Grid_Count.AddRow(pRow, FALSE);
	}*/
	m_Grid_Count.AdjustLayout();
	//SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
}

void CSubMainDisp::RefreshBatchInfoList()
{
	//CBCGPGridRow* pRow;
	//pRow = m_Grid_Info.GetRow(0);
	////pRow->GetItem(0)->SetValue(_variant_t("工厂编号"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.GetSystemSet().strWorkshopName), FALSE);
	//pRow = m_Grid_Info.GetRow(1);
	////pRow->GetItem(0)->SetValue(_variant_t("设备编号"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.GetSystemSet().strDeviceID), FALSE);
	//pRow = m_Grid_Info.GetRow(2);
	////pRow->GetItem(0)->SetValue(_variant_t("工号"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.GetCurrentJobNum()), FALSE);
	//pRow = m_Grid_Info.GetRow(3);
	////pRow->GetItem(0)->SetValue(_variant_t("上卷开始时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetLastBatch().GetStartTime()), FALSE);
	//pRow = m_Grid_Info.GetRow(4);
	////pRow->GetItem(0)->SetValue(_variant_t("上卷结束时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetLastBatch().GetEndTime()), FALSE);
	//pRow = m_Grid_Info.GetRow(5);
	////pRow->GetItem(0)->SetValue(_variant_t("上卷耗时"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetLastBatch().GetTimeSpanS()), FALSE);
	//pRow = m_Grid_Info.GetRow(6);
	////pRow->GetItem(0)->SetValue(_variant_t("当前卷开始时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetCurrentBatch().GetStartTime()), FALSE);
	//pRow = m_Grid_Info.GetRow(7);
	////pRow->GetItem(0)->SetValue(_variant_t("持续时间"), FALSE);
	//pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Counter.GetCurrentBatch().GetTimeSpanSForNow()), FALSE);

	//m_Grid_Info.AdjustLayout();
}

void CSubMainDisp::RefreshLogList()
{
	static CString s_strTmp = "";
	CLog::LogList vec = theGlobal.m_Log.GetDispLogList();
	if (vec.size() <= 0) return;
	if (s_strTmp == vec[vec.size() - 1].strLog) return;

	HWND hwndList = m_Grid_Base.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);//SetWindowRedraw
	m_Grid_Base.RemoveAll();

	for (int nRow = 0; nRow < vec.size(); nRow++)
	{
		CBCGPGridRow* pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());

		pRow->GetItem(0)->SetValue(_variant_t(vec[nRow].strLog), FALSE);
		if (vec[nRow].type == ExcpLog)
			pRow->GetItem(0)->SetTextColor(RGB(255, 191, 0), FALSE);
		else if (vec[nRow].type == DebugLog)
			pRow->GetItem(0)->SetTextColor(RGB(37, 218, 111), FALSE);
		else if (vec[nRow].type == OperateLog)
			pRow->GetItem(0)->SetTextColor(RGB(0, 103, 173), FALSE);
		else if (vec[nRow].type == AlarmLog)
			pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0), FALSE);
		else if (vec[nRow].type == CommunicationLog)
			pRow->GetItem(0)->SetTextColor(RGB(204, 153, 255), FALSE);

		if (nRow % 2 == 1)
			pRow->GetItem(0)->SetBackgroundColor(RGB(58, 58, 58), FALSE);

		m_Grid_Base.InsertRowBefore(0, pRow, FALSE);
	}
	s_strTmp = vec[vec.size() - 1].strLog;
	SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
	m_Grid_Base.AdjustLayout();
}

void CSubMainDisp::OpenTimer()
{
	SetTimer(UM_TIMER_TIME, 1000, NULL);
}


void CSubMainDisp::CloseTimer()
{
	KillTimer(UM_TIMER_TIME);
}

void CSubMainDisp::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (UM_TIMER_TIME == nIDEvent)//1秒钟一次
	{
		if (IsWindowVisible())
		{
			RefreshLogList();
		}
		
		/*static int nCount = 0;
		if (nCount++ % 60 == 0 && IsWindowVisible())
		{
			RefreshBatchInfoList();
			RefreshErrorCountList();
		}*/
	}
	CBCGPDialog::OnTimer(nIDEvent);
}

void CSubMainDisp::InitWindowShow()
{
	CRect rcMainPicture, rcPart;
	GetDlgItem(IDC_PICTURE_MAIN)->GetClientRect(rcMainPicture);

	int nCamNum = theGlobal.m_GrabberTool.GetGrabberCount();
	int nGap = 2;

	SetCheck("~father");
	///////两个窗体//
	SetWindowAttr("background_color", "#252525");//#252525

	if (theGlobal.m_GrabberTool.GetGrabberCount() == 1 ||
		(theGlobal.m_GrabberTool.GetGrabberCount() == 2 && theGlobal.m_Project.GetProjectConfig().nSideCount == 1))
	{
		rcPart = rcMainPicture;
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[0].T());
	}
	else if (theGlobal.m_GrabberTool.GetGrabberCount() == 2 && theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
	{
		rcPart.right = rcMainPicture.left + rcMainPicture.Width() - 1;
		rcPart.bottom = rcMainPicture.top + rcMainPicture.Height() / 2 - nGap;
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[0].T());
		rcPart.OffsetRect(0, rcMainPicture.Height() / 2 + nGap);
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[1].T());
	}
	else  if (theGlobal.m_GrabberTool.GetGrabberCount() == 3)
	{
		rcPart.right = rcMainPicture.left + rcMainPicture.Width() - 1;
		rcPart.bottom = rcMainPicture.top + rcMainPicture.Height() / 3 - nGap * 2;
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[0].T());
		rcPart.OffsetRect(0, rcMainPicture.Height() / 3 + nGap);
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[1].T());
		rcPart.OffsetRect(0, rcMainPicture.Height() / 3 + nGap);
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[2].T());
	}
	else if (theGlobal.m_GrabberTool.GetGrabberCount() == 4)
	{
		rcPart.right = rcMainPicture.left + rcMainPicture.Width() - 1;
		rcPart.bottom = rcMainPicture.top + rcMainPicture.Height() / 2 - nGap;
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[0].T());
		rcPart.OffsetRect(0, rcMainPicture.Height() / 2 + nGap);
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[1].T());
		/*rcPart.right = rcMainPicture.left + rcMainPicture.Width() / 2 - nGap;
		rcPart.bottom = rcMainPicture.top + rcMainPicture.Height() / 2 - nGap;
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
		(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[0].T());
		rcPart.OffsetRect(rcMainPicture.Width() / 2 + nGap, 0);
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
		(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[1].T());
		rcPart.OffsetRect(-rcMainPicture.Width() / 2 - nGap, rcMainPicture.Height() / 2 + nGap);
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
		(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[2].T());
		rcPart.OffsetRect(rcMainPicture.Width() / 2 + nGap, 0);
		OpenWindow(rcPart.top, rcPart.left, rcPart.Width(), rcPart.Height(),
		(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecMainImageHandle[3].T());*/
	}
	else
	{
		AfxMessageBox("未实现4个相机以上的显示设置，程序将强制退出");
		exit(0);
	}

	OpenWindow(rcMainPicture.top, rcMainPicture.left, rcMainPicture.Width(), rcMainPicture.Height(),
		(Hlong)GetDlgItem(IDC_PICTURE_MAIN)->m_hWnd, "visible", "", &m_hvecTmpHandle[0].T());

	//rcHistory.right = rcHistoryCtrl.right;

	for (int i = 0; i < m_hvecMainImageHandle.Length(); i++)
	{
		SetDraw(m_hvecMainImageHandle[i].T(), "margin");
		SetColor(m_hvecMainImageHandle[i].T(), "red");
		set_display_font(m_hvecMainImageHandle[i].T(), 16, "mono", "true", "false");
	}

	SetCheck("father");

	CString strBG;
	strBG.Format("%s\\res\\bg.png", GetCurrentAppPath());
	if (!PathFileExists(strBG))
		return;
	HObject hoBG;
	HTuple hvW, hvH, hvNum;
	ReadImage(&hoBG, (HTuple)strBG);
	CountObj(hoBG, &hvNum);
	if (hvNum == 0)
	{
		return;
	}

	GetImageSize(hoBG, &hvW, &hvH);
	for (int i = 0; i < m_hvecMainImageHandle.Length(); i++)
	{
		SetPart(m_hvecMainImageHandle[i].T(), 0, 0, hvH, hvW);
		DispObj(hoBG, m_hvecMainImageHandle[i].T());
	}
}

void CSubMainDisp::DispOutput(C_MergeWork& tOutput)
{
	HTuple Width, Height;
	CString strMessage;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	try
	{
		for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
		{
		/*	HObject hoImage;
			GenEmptyObj(&hoImage);
			for (int nAWorkSN = 0; nAWorkSN < tOutput.vecB[nBWorkSN].vecA.size(); nAWorkSN++)
			{
				ConcatObj(hoImage, tOutput.vecB[nBWorkSN].vecA[nAWorkSN].hoGrabImage, &hoImage);
			}*/

			//if (hoImage.CountObj() > 1)				
			{
				//从输出参数中获取
				//TileImages(hoImage, &hoImage, hoImage.CountObj(), "horizontal");

				int nGrabberSN = nBWorkSN;
				SetSystem("flush_graphic", "false");

				ClearWindow(m_hvecMainImageHandle[nGrabberSN].T());
				GetImageSize(tOutput.vecB[nBWorkSN].tMeasure.ho_DispImage, &Width, &Height);
				SetPart(m_hvecMainImageHandle[nGrabberSN].T(), 0, 0, Height - 1, Width - 1);
				DispObj(tOutput.vecB[nBWorkSN].tMeasure.ho_DispImage, m_hvecMainImageHandle[nGrabberSN].T());

				if ((nBWorkSN == 0 && theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType == OneSide) ||
					theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType == TwoSide)
				{
					SetLineWidth(m_hvecMainImageHandle[nGrabberSN].T(), 1);
					SetColor(m_hvecMainImageHandle[nGrabberSN].T(), "green");
					DispObj(tOutput.vecB[nBWorkSN].tMeasure.ho_Disp, m_hvecMainImageHandle[nGrabberSN].T());

					SetColor(m_hvecMainImageHandle[nGrabberSN].T(), "medium slate blue");
					SetLineWidth(m_hvecMainImageHandle[nGrabberSN].T(), 2);
					HObject hoLine;
					GenRegionLine(&hoLine, tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartRow, tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn,
						tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_EndRow, tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_EndColumn);
					HALCON_TRY(DispObj(hoLine, m_hvecMainImageHandle[nGrabberSN].T()));

					SetColor(m_hvecMainImageHandle[nGrabberSN].T(), "red");
					DispObj(tOutput.vecB[nBWorkSN].tMeasure.tDefect.ho_Region, m_hvecMainImageHandle[nGrabberSN].T());

					if (theGlobal.GetSystemSet().nActionMode == DebugMode || theGlobal.GetSystemSet().nActionMode == RunMode)
					{
						SetColor(m_hvecMainImageHandle[nGrabberSN].T(), "green");
						HALCON_TRY(DispObj(tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.ho_Region, m_hvecMainImageHandle[nGrabberSN].T()));
					}

					if (theGlobal.GetSystemSet().nActionMode == RunMode)
					{
						if (nGrabberSN == 0 && tOutput.tMerge.hv_NGType.Length() > 0)
						{
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-16-*-0-*-*-1-"));
							CString strItem;
							for (int i = 0; i < tOutput.tMerge.hv_NGType.Length(); i++)
							{
								strItem.Format("%s", theGlobal.m_Counter.GetNGTypeName(tOutput.tMerge.hv_NGType[i].I()));
								if(strMessage.Find(strItem) == -1)
									strMessage += strItem + ",";
							}
							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 10,
								10, "red", "false");
						}
						else if (nGrabberSN == 0 && tOutput.tMerge.hv_WarnType.Length() > 0)
						{
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-16-*-0-*-*-1-"));
							CString strItem;
							for (int i = 0; i < tOutput.tMerge.hv_WarnType.Length(); i++)
							{
								strItem.Format("%s", theGlobal.m_Counter.GetNGTypeName(tOutput.tMerge.hv_WarnType[i].I()));
								if (strMessage.Find(strItem) == -1)
									strMessage += strItem + ",";
							}
							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 10,
								10, "#f8b62b", "false");
						}
						else if (nGrabberSN == 0)
						{
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-16-*-0-*-*-1-"));
							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("OK"), "image", 10,
								10, "green", "false");
						}
					}
					else
					{
						CString strName = "调试模式";
						if (theGlobal.GetSystemSet().nActionMode == SimulateMode)
							strName = "仿真模式";
						else if (theGlobal.GetSystemSet().nActionMode == GRRMode)
							strName = "GRR验证模式";

						HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-16-*-0-*-*-1-"));
						disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strName), "image", 10,
							10, "blue", "false");
					}

					if ((nBWorkSN == 0 && theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType == OneSide) ||
						theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType == TwoSide)
					{
						for (int nWidthSN = 0; nWidthSN < tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width.Length(); nWidthSN++)
						{
							if ((tPdInfo.bExistBaseCoat || tPdInfo.bExistAT11) && nWidthSN % 2 == 1)
								HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-17-*-0-*-*-1-"));
							else
								HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-17-*-0-*-*-1-"));
							if (tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width[nWidthSN] != NO_DATA)
								strMessage.Format("%.4f", tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width[nWidthSN].D());
							else
								strMessage = "NA";
							CString strColor = "green";
							if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_AllWidthFlagList[nWidthSN].I() == NG)
								strColor = "red";
							else if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_AllWidthFlagList[nWidthSN].I() == WARN)
								strColor = "#f8b62b";
							if ((tPdInfo.bExistBaseCoat || tPdInfo.bExistAT11) && nWidthSN % 4 == 1)
							{
								disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", nWidthSN % 2 == 0 ? Height.I() / 4 : Height.I() / 3 * 2 - 100,
									(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN] + tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN + 1]) / 2 - 250, HTuple(strColor), "false");
							}
							else if ((tPdInfo.bExistBaseCoat || tPdInfo.bExistAT11) && nWidthSN % 4 == 3)
							{
								disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", nWidthSN % 2 == 0 ? Height.I() / 4 : Height.I() / 3 * 2 + 100,
									(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN] + tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN + 1]) / 2 - 250, HTuple(strColor), "false");
							}
							else
							{
								disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", nWidthSN % 2 == 0 ? Height.I() / 4 : Height.I() / 3 * 2,
									(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN] + tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN + 1]) / 2 - 250, HTuple(strColor), "false");
							}
							
						}
					}
				}

				//显示耗时
				HTuple hv_MaxWidth, hv_MaxHeight, hvW, hvH;
				strMessage.Format("采图:%0.1fms  检测:%0.1fms  综合:%0.1fms", tOutput.vecB[nBWorkSN].vecA[0].hvTimeStamp.D(), tOutput.vecB[nBWorkSN].hv_TimeStamp.D(), tOutput.hv_TimeStamp.D());
				HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-14-*-0-*-*-1-"));
				GetWindowExtents(m_hvecMainImageHandle[nGrabberSN].T(), &HTuple(), &HTuple(), &hvW, &hvH);
				GetStringExtents(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), &HTuple(), &HTuple(), &hv_MaxWidth, &hv_MaxHeight);//获取字体属性
				disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "window", hvH - hv_MaxHeight - 1,
					1, "blue", "false");

				SetSystem("flush_graphic", "true");
				DispPolygon(m_hvecMainImageHandle[nGrabberSN].T(), -1, -1);//防止闪烁
			}
		}
	}
	catch (HException&e)
	{

	}
}

void CSubMainDisp::UpdateData(C_MergeWork& tOutput)
{
	if (IsWindowVisible())
	{
		DispOutput(tOutput);
		m_pSubDispList->UpdateData(tOutput);
	}
}

void CSubMainDisp::InitSubDlg()
{
	CRect rcSubDlg;
	GetDlgItem(IDC_STATIC_SWITCH_VIEW)->GetWindowRect(rcSubDlg);
	ScreenToClient(rcSubDlg);

	m_pSubDispList = new CSubDispList;
	m_pSubDispList->Create(CSubDispList::IDD, this);
	m_pSubDispList->MoveWindow(rcSubDlg);
	m_pSubDispList->ShowWindow(SW_NORMAL);
	m_pSubDispList->ResetSize();

	m_pSubDispSetting = new CSubDispSetting;
	m_pSubDispSetting->Create(CSubDispSetting::IDD, this);
	m_pSubDispSetting->MoveWindow(rcSubDlg);
	m_pSubDispSetting->ShowWindow(SW_HIDE);
	m_pSubDispSetting->ResetSize();
}

BOOL CSubMainDisp::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}

void CSubMainDisp::SwitchDispSubDlg(int nSubDlgDispSN)
{
	switch (nSubDlgDispSN)
	{
	case 0:
		m_pSubDispList->ShowWindow(SW_NORMAL);
		m_pSubDispSetting->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pSubDispList->ShowWindow(SW_HIDE);
		m_pSubDispSetting->ShowWindow(SW_NORMAL);
		break;
	default:
		m_pSubDispList->ShowWindow(SW_NORMAL);
		m_pSubDispSetting->ShowWindow(SW_HIDE);
		break;
	}
}

void CSubMainDisp::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}
