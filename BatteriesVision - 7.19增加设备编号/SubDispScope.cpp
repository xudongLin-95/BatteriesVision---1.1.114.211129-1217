// SubDispScope.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "SubDispScope.h"
#include "afxdialogex.h"
#include "AboutDlg.h"


// CSubDispScope 对话框

IMPLEMENT_DYNAMIC(CSubDispScope, CBCGPDialog)

CSubDispScope::CSubDispScope(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CSubDispScope::IDD, pParent)
	, m_hvecMainImageHandle(1)
	, m_hvecTmpHandle(1)
{
	m_fontStatic.CreateFontA(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");
}

CSubDispScope::~CSubDispScope()
{
	m_fontStatic.DeleteObject();
}

void CSubDispScope::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PRODUCT, m_STC_CurrentProduct);
	DDX_Control(pDX, IDC_STATIC_CURRENT_BATCH, m_STC_CurrentBatch);
	DDX_Control(pDX, IDC_STATIC_CURRENT_SPEED, m_STC_CurrentSpeed);
	DDX_Control(pDX, IDC_STATIC_CURRENT_METERCOUNT, m_STC_CurrentMeterCount);
	DDX_Control(pDX, IDC_STATIC_YIELD_TOTAL, m_STC_YieldTotal);
	DDX_Control(pDX, IDC_STATIC_CURRENT_STATUS, m_STC_CurrentStatus);
}


BEGIN_MESSAGE_MAP(CSubDispScope, CBCGPDialog)
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(WM_REFRESH_DISP, &CSubDispScope::OnRefreshDisp)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSubDispScope 消息处理程序
BOOL CSubDispScope::PreTranslateMessage(MSG* pMsg)
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
	else if (pMsg->message == WM_LBUTTONDOWN)
	{
		CRect rcVersion;
		GetDlgItem(IDC_STATIC_YIELD_TOTAL)->GetWindowRect(&rcVersion);
		if (PtInRect(rcVersion, pMsg->pt))
		{
			CAboutDlg dlg;
			dlg.DoModal();
		}
		return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


BOOL CSubDispScope::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	theGlobal.m_pSubDispScope = this;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSubDispScope::InitUI()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CRect rcClient, rcOscpShow;
	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(rcClient);
	ScreenToClient(rcClient);
	rcOscpShow = rcClient;

	//第一排，基材左边界,涂膜宽
	//第二排，极耳宽
	//第二排，凹版/at11宽
	//第三排，涂膜错位+错位均值
	

	//一共几排折线图
	int ROW_NUM = 1
		+ 1 
		+ ((tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)==TRUE ? 1 : 0)
		+ (tPdInfo.nCoatingType == TwoSide ? 1 : 0);

	SCOPE_COL_NUM[0] = tPdInfo.tParam.vecCoatWidth.size();
	SCOPE_COL_NUM[1] = tPdInfo.tParam.vecFoilWidth.size();
	SCOPE_COL_NUM[2] = ((tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat) ? tPdInfo.nPDType * 2 : 0);
	SCOPE_COL_NUM[3] = (tPdInfo.nCoatingType == TwoSide ? tPdInfo.nPDType * 2 + 1 : 0);

	//横向折线图间距
	int COL_GAP = 4;
	//纵向折线图间距
	int ROW_GAP = 4;
	//折线图高
	int SCOPE_HEIGHT = ROW_NUM == 1 ? rcClient.Height() : (rcClient.Height() - (ROW_NUM - 1) * ROW_GAP) / ROW_NUM;
	//折线图宽
	int SCOPE_WIDTH = (rcClient.Width() - (SCOPE_COL_NUM[0] - 1) * COL_GAP) / SCOPE_COL_NUM[0];
	int nRowScopeCount = 0;
	//第一排折线图初始化
	for (int i = 0; i < SCOPE_COL_NUM[0]; i++)
	{
		rcOscpShow.top = rcClient.top;
		rcOscpShow.bottom = rcOscpShow.top + SCOPE_HEIGHT;
		rcOscpShow.left = rcClient.left + (SCOPE_WIDTH + COL_GAP) * i;
		rcOscpShow.right = rcOscpShow.left + SCOPE_WIDTH;

		m_Oscope1[i].Create(WS_VISIBLE | WS_CHILD, rcOscpShow, this);
		SetScopeColor(m_Oscope1[i]);
	}

	//第二排折线图初始化
	nRowScopeCount++;
	SCOPE_WIDTH = (rcClient.Width() - (SCOPE_COL_NUM[1] - 1) * COL_GAP) / SCOPE_COL_NUM[1];
	for (int i = 0; i < SCOPE_COL_NUM[1]; i++)
	{
		rcOscpShow.top = rcClient.top + (SCOPE_HEIGHT + ROW_GAP) * nRowScopeCount;
		rcOscpShow.bottom = rcOscpShow.top + SCOPE_HEIGHT;
		rcOscpShow.left = rcClient.left + (SCOPE_WIDTH + COL_GAP) * i;
		rcOscpShow.right = rcOscpShow.left + SCOPE_WIDTH;

		m_Oscope2[i].Create(WS_VISIBLE | WS_CHILD, rcOscpShow, this);
		SetScopeColor(m_Oscope2[i]);
	}

	if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
	{
		nRowScopeCount++;
		SCOPE_WIDTH = (rcClient.Width() - (SCOPE_COL_NUM[2] - 1) * COL_GAP) / SCOPE_COL_NUM[2];
		//第三排折线图初始化
		for (int i = 0; i < SCOPE_COL_NUM[2]; i++)
		{
			rcOscpShow.top = rcClient.top + (SCOPE_HEIGHT + ROW_GAP) * nRowScopeCount;
			rcOscpShow.bottom = rcOscpShow.top + SCOPE_HEIGHT;
			rcOscpShow.left = rcClient.left + (SCOPE_WIDTH + COL_GAP) * i;
			rcOscpShow.right = rcOscpShow.left + SCOPE_WIDTH;

			m_Oscope3[i].Create(WS_VISIBLE | WS_CHILD, rcOscpShow, this);
			SetScopeColor(m_Oscope3[i]);
		}
	}

	if (tPdInfo.nCoatingType == TwoSide)
	{
		nRowScopeCount++;
		SCOPE_WIDTH = (rcClient.Width() - (SCOPE_COL_NUM[3] - 1) * COL_GAP) / SCOPE_COL_NUM[3];
		//第四排折线图初始化
		for (int i = 0; i < SCOPE_COL_NUM[3]; i++)
		{
			rcOscpShow.top = rcClient.top + (SCOPE_HEIGHT + ROW_GAP) * nRowScopeCount;
			rcOscpShow.bottom = rcOscpShow.top + SCOPE_HEIGHT;
			rcOscpShow.left = rcClient.left + (SCOPE_WIDTH + COL_GAP) * i;
			rcOscpShow.right = rcOscpShow.left + SCOPE_WIDTH;

			m_Oscope4[i].Create(WS_VISIBLE | WS_CHILD, rcOscpShow, this);
			SetScopeColor(m_Oscope4[i]);
			m_Oscope4[i].SetCpkSigShow(FALSE, TRUE);
		}
	}

	//示意图
	CRect rcDsp;
	GetDlgItem(IDC_PICTURE_MAIN)->GetWindowRect(rcDsp);
	ScreenToClient(rcDsp);

	m_CoatSketch.Create(WS_VISIBLE | WS_CHILD, rcDsp, this);
	CCoatSketchStatic::tagInit tInit;
	tInit.bExistAT11 = tPdInfo.bExistAT11;
	tInit.bExistBaseCoat = tPdInfo.bExistBaseCoat;
	tInit.nCoatNum = tPdInfo.nPDType;
	tInit.nCoatType = tPdInfo.nCoatingType;
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && tPdInfo.nCoatingType == OneSide)
		tInit.nTitleType = 0;
	else
		tInit.nTitleType = 1;
	m_CoatSketch.Init(tInit);

	InitStatic();
	ResetScope();
	OpenTimer();
	//InitWindowShow();
}

void CSubDispScope::SetScopeColor(COScopeCtrl& Scop)
{
	Scop.SetBackgroundColor(RGB(48, 48, 48));
	Scop.SetTitleBackgroundColor(RGB(58, 58, 58));
	Scop.SetGridColor(RGB(85, 85, 85));
	//Scop.SetLimitLineColor(RGB(85, 85, 90));
	//Scop.SetPlotColor(RGB(85, 85, 90));
	Scop.SetTitleTextColor(RGB(200, 200, 200));
	Scop.SetWarnLineShow(TRUE);
	Scop.SetCpkLimit(1.33);
	Scop.SetCpkSigShow(TRUE, TRUE);
}

void CSubDispScope::ResetScope()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	CString strName;
	std::vector<std::vector<CString>>	vecNameList;
	std::vector<CString>				vecTmp;
	std::vector<double>					vecStd;
	std::vector<double>					vecTol;
	std::vector<double>					vecWarnTol;
	std::vector<double>					vecMarginTol;

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		vecTmp.push_back("A面");
		if (tPdInfo.nCoatingType == TwoSide)
			vecTmp.push_back("B面");
	}
	else
	{
		vecTmp.push_back("B面");
		if (tPdInfo.nCoatingType == TwoSide)
			vecTmp.push_back("A面");
	}

#pragma region 涂膜宽
	vecNameList.clear();
	strArrCode.RemoveAll();
	//膜宽
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		strName.Format("涂膜-%d", i + 1);
		strArrCode.Add(strName);
		vecNameList.push_back(vecTmp);
	}
	vecStd.clear();
	vecTol.clear();
	vecWarnTol.clear();
	vecMarginTol.clear();
	//涂膜
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		vecStd.push_back(tPdInfo.tParam.vecCoatWidth[0].dStandard);
		vecTol.push_back(abs(tPdInfo.tParam.vecCoatWidth[0].dTolerance));
		vecWarnTol.push_back(abs(tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance));
		vecMarginTol.push_back(max(abs(tPdInfo.tParam.vecCoatWidth[0].dTolerance), abs(tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance)) * 1.5);
	}
	//设置折线图参数
	for (int i = 0; i < SCOPE_COL_NUM[0]; i++)
	{
		if (NULL == m_Oscope1[i]) continue;

		m_Oscope1[i].SetLimit(vecStd[i], vecTol[i], vecWarnTol[i], vecMarginTol[i], 2);
		m_Oscope1[i].SetTitleText(strArrCode[i]);
		m_Oscope1[i].SetPlotName(vecNameList[i]);
		m_Oscope1[i].Reset();
	}
#pragma endregion

#pragma region 极耳宽
	vecNameList.clear();
	strArrCode.RemoveAll();
	//极耳宽
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		strName.Format("极耳-%d", i + 1);
		strArrCode.Add(strName);
		vecNameList.push_back(vecTmp);
	}
	vecStd.clear();
	vecTol.clear();
	vecWarnTol.clear();
	vecMarginTol.clear();
	//极耳宽
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		vecStd.push_back(tPdInfo.tParam.vecFoilWidth[i].dStandard);
		vecTol.push_back(abs(tPdInfo.tParam.vecFoilWidth[i].dTolerance));
		vecWarnTol.push_back(abs(tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance));
		vecMarginTol.push_back(max(abs(tPdInfo.tParam.vecFoilWidth[i].dTolerance), abs(tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance)) * 1.5);
	}
	//设置折线图参数
	for (int i = 0; i < SCOPE_COL_NUM[1]; i++)
	{
		if (NULL == m_Oscope2[i]) continue;

		m_Oscope2[i].SetLimit(vecStd[i], vecTol[i], vecWarnTol[i], vecMarginTol[i], 2);
		m_Oscope2[i].SetTitleText(strArrCode[i]);
		m_Oscope2[i].SetPlotName(vecNameList[i]);
		m_Oscope2[i].Reset();
	}
#pragma endregion

#pragma region 凹版/AT11
	if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
	{
		vecNameList.clear();
		strArrCode.RemoveAll();
		//凹版/AT11
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size() * 2; i++)
		{
			if (tPdInfo.bExistAT11)
			{
				strName.Format("涂膜%d%sAT11", i / 2 + 1, i % 2 == 0 ? "左" : "右");
			}
			else
			{
				strName.Format("涂膜%d%s凹版", i / 2 + 1, i % 2 == 0 ? "左" : "右");
			}

			strArrCode.Add(strName);
			vecNameList.push_back(vecTmp);
		}
		vecStd.clear();
		vecTol.clear();
		vecWarnTol.clear();
		vecMarginTol.clear();
		//涂膜
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size() * 2; i++)
		{
			if (tPdInfo.bExistAT11)
			{
				vecStd.push_back(tPdInfo.tParam.vecAT11Width[0].dStandard);
				vecTol.push_back(abs(tPdInfo.tParam.vecAT11Width[0].dTolerance));
				vecWarnTol.push_back(abs(tPdInfo.tParam.vecAT11Width[0].dWarnTolerance));
				vecMarginTol.push_back(max(abs(tPdInfo.tParam.vecAT11Width[0].dTolerance), abs(tPdInfo.tParam.vecAT11Width[0].dWarnTolerance)) * 1.5);
			}
			else
			{
				vecStd.push_back(tPdInfo.tParam.vecBaseCoatWidth[0].dStandard);
				vecTol.push_back(abs(tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance));
				vecWarnTol.push_back(abs(tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance));
				vecMarginTol.push_back(max(abs(tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance), abs(tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance)) * 1.5);
			}
		}
		//设置折线图参数
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size() * 2; i++)
		{
			if (NULL == m_Oscope3[i]) continue;

			m_Oscope3[i].SetLimit(vecStd[i], vecTol[i], vecWarnTol[i], vecMarginTol[i], 2);
			m_Oscope3[i].SetTitleText(strArrCode[i]);
			m_Oscope3[i].SetPlotName(vecNameList[i]);
			m_Oscope3[i].Reset();
		}
	}
#pragma endregion

#pragma region 错位
	if (tPdInfo.nCoatingType == TwoSide)
	{
		vecTmp.clear();
		vecTmp.push_back("错位");
		vecNameList.clear();
		strArrCode.RemoveAll();
		strArrCode.Add("纠偏差值");
		vecNameList.push_back(vecTmp);
		//错位
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			strName.Format("涂膜%d%s错位", i / 2 + 1, i % 2 == 0 ? "左" : "右");
			strArrCode.Add(strName);
			vecNameList.push_back(vecTmp);
		}
		vecStd.clear();
		vecTol.clear();
		vecWarnTol.clear();
		vecMarginTol.clear();
		//涂膜
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size()+1; i++)
		{
			vecStd.push_back(tPdInfo.tParam.vecMisalignment[0].dStandard);
			vecTol.push_back(abs(tPdInfo.tParam.vecMisalignment[0].dTolerance));
			vecWarnTol.push_back(abs(tPdInfo.tParam.vecMisalignment[0].dWarnTolerance));
			vecMarginTol.push_back(max(abs(tPdInfo.tParam.vecMisalignment[0].dTolerance), abs(tPdInfo.tParam.vecMisalignment[0].dWarnTolerance)) * 1.5);
		}
		//设置折线图参数
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size()+1; i++)
		{
			if (NULL == m_Oscope4[i]) continue;

			m_Oscope4[i].SetLimit(vecStd[i], vecTol[i], vecWarnTol[i], vecMarginTol[i], 2);
			m_Oscope4[i].SetTitleText(strArrCode[i]);
			m_Oscope4[i].SetPlotName(vecNameList[i]);
			m_Oscope4[i].Reset();
		}
	}
#pragma endregion
}

void CSubDispScope::ResetSize()
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

LRESULT CSubDispScope::OnRefreshDisp(WPARAM wParam, LPARAM lParam)
{
	ResetScope();
	RefreshCountStatic();
	return 0L;
}

void CSubDispScope::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CSubDispScope::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
}

void CSubDispScope::UpdateData(C_MergeWork& tOutput)
{
	DispCoatSketch(tOutput);
	DispScope(tOutput);
	
	/*if (IsWindowVisible())
	{
		DispOutput(tOutput);
	}*/
}

BOOL CSubDispScope::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}

void CSubDispScope::InitStatic()
{
	m_STC_CurrentProduct.SetBkColor(RGB(58, 58, 58), RGB(114, 114, 114));
	m_STC_CurrentBatch.SetBkColor(RGB(58, 58, 58));
	m_STC_CurrentSpeed.SetBkColor(RGB(58, 58, 58));
	m_STC_CurrentMeterCount.SetBkColor(RGB(58, 58, 58));

	m_STC_CurrentProduct.SetTextColor(RGB(255, 255, 255));
	m_STC_CurrentBatch.SetTextColor(RGB(255, 255, 255));
	m_STC_CurrentSpeed.SetTextColor(RGB(255, 255, 255));
	m_STC_CurrentMeterCount.SetTextColor(RGB(255, 255, 255));

	m_STC_CurrentProduct.SetFont(&m_fontStatic);
	m_STC_CurrentBatch.SetFont(&m_fontStatic);
	m_STC_CurrentSpeed.SetFont(&m_fontStatic);

	COLORREF colTitleBK(RGB(58, 58, 58)), colBK(RGB(48, 48, 48)), colHigh(RGB(85, 85, 85)), colText(RGB(255, 255, 255)), colRed(RGB(230, 0, 0)), colGreen(RGB(112, 168, 0));
	int nTitlePointSize(19), nTitlePointWeight(400), nTextPointSize(20), nTextPointWeight(600);

	BOOL bChinese = theGlobal.m_Language.IsChinese();
	m_STC_CurrentProduct.SetBkColorNormal(colTitleBK, colBK);
	m_STC_CurrentProduct.SetTextColorAll(colText, colText);
	m_STC_CurrentProduct.SetTitleSize(35);
	m_STC_CurrentProduct.Format(" ");
	m_STC_CurrentProduct.SetStaticHorizontal(FALSE);
	m_STC_CurrentProduct.SetTitleText(bChinese ? "当前配方" : "Current Product");
	m_STC_CurrentProduct.SetFontAll("Microsoft Yahei", nTitlePointSize, nTitlePointWeight, "Microsoft Yahei", 28, 500);

	m_STC_CurrentBatch.SetBkColorNormal(colTitleBK, colBK);
	m_STC_CurrentBatch.SetTextColorAll(colText, colText);
	m_STC_CurrentBatch.SetTitleSize(35);
	m_STC_CurrentBatch.Format(" ");
	m_STC_CurrentBatch.SetStaticHorizontal(FALSE);
	m_STC_CurrentBatch.SetTitleText(bChinese ? "当前卷号" : "Current Roll NO.");
	m_STC_CurrentBatch.SetFontAll("Microsoft Yahei", nTitlePointSize, nTitlePointWeight, "Microsoft Yahei", 28, 500);

	m_STC_CurrentSpeed.SetBkColorNormal(colTitleBK, colBK);
	m_STC_CurrentSpeed.SetTextColorAll(colText, colText);
	m_STC_CurrentSpeed.SetTitleSize(35);
	m_STC_CurrentSpeed.Format(" ");
	m_STC_CurrentSpeed.SetStaticHorizontal(FALSE);
	m_STC_CurrentSpeed.SetTitleText(bChinese ? "生产速度" : "Production Speed");
	m_STC_CurrentSpeed.SetFontAll("Microsoft Yahei", nTitlePointSize, nTitlePointWeight, "Microsoft Yahei", 28, 500);

	m_STC_CurrentMeterCount.SetBkColorNormal(colTitleBK, colBK);
	m_STC_CurrentMeterCount.SetTextColorAll(colText, colText);
	m_STC_CurrentMeterCount.SetTitleSize(35);
	m_STC_CurrentMeterCount.Format(" ");
	m_STC_CurrentMeterCount.SetStaticHorizontal(FALSE);
	m_STC_CurrentMeterCount.SetTitleText(bChinese ? "计米" : "Meter Count");
	m_STC_CurrentMeterCount.SetFontAll("Microsoft Yahei", nTitlePointSize, nTitlePointWeight, "Microsoft Yahei", 28, 500);

	m_STC_YieldTotal.SetBkColorNormal(colTitleBK, colBK);
	m_STC_YieldTotal.SetTextColorAll(colText, colText);
	m_STC_YieldTotal.SetTitleSize(35);
	m_STC_YieldTotal.Format(theGlobal.GetVersion());
	m_STC_YieldTotal.SetStaticHorizontal(FALSE);
	m_STC_YieldTotal.SetTitleText("软件版本");
	m_STC_YieldTotal.SetFontAll("Microsoft Yahei", nTitlePointSize, nTitlePointWeight, "Microsoft Yahei", 28, 500);

	m_STC_CurrentStatus.SetBkColorNormal(colTitleBK, colBK);
	m_STC_CurrentStatus.SetTextColorAll(colText, colText);
	m_STC_CurrentStatus.SetTitleSize(35);
	m_STC_CurrentStatus.Format(theGlobal.GetVersion());
	m_STC_CurrentStatus.SetStaticHorizontal(FALSE);
	m_STC_CurrentStatus.SetTitleText("当前状态");
	m_STC_CurrentStatus.SetFontAll("Microsoft Yahei", nTitlePointSize, nTitlePointWeight, "Microsoft Yahei", 28, 500);

	RefreshCountStatic();
	RefreshBatchDisp();
}

void CSubDispScope::RefreshBatchDisp()
{
	m_STC_CurrentBatch.Format("%s", theGlobal.GetCurrentBatch());
	m_STC_CurrentProduct.Format("%s", theGlobal.m_ProductTool.GetCurrentProductName());
}

void CSubDispScope::RefreshCountStatic()
{
	if (CGlobal::SN_Head == theGlobal.m_tSystemSet.eSN)
	{
		m_STC_CurrentMeterCount.Format("%0.3f 米 （工位一）", theGlobal.m_Counter.GetCurrentMeterCount());
	}
	else if (CGlobal::SN_2nd == theGlobal.m_tSystemSet.eSN)
	{
		m_STC_CurrentMeterCount.Format("%0.3f 米 （工位二）", theGlobal.m_Counter.GetCurrentMeterCount());
	}
	else if (CGlobal::SN_3rd == theGlobal.m_tSystemSet.eSN)
	{
		m_STC_CurrentMeterCount.Format("%0.3f 米 （工位三）", theGlobal.m_Counter.GetCurrentMeterCount());
	}
	else if (CGlobal::SN_4th == theGlobal.m_tSystemSet.eSN)
	{
		m_STC_CurrentMeterCount.Format("%0.3f 米 （工位四）", theGlobal.m_Counter.GetCurrentMeterCount());
	}
	else
	{
		m_STC_CurrentMeterCount.Format("%0.3f 米 （工位五）", theGlobal.m_Counter.GetCurrentMeterCount());
	}
	
	m_STC_CurrentSpeed.Format("%.1f 米/分钟", theGlobal.m_Speed.GetSpeed());

	if (theGlobal.GetSystemSet().nActionMode == RunMode)
	{
		m_STC_CurrentStatus.Format("%s", "生产模式");
		m_STC_CurrentStatus.SetTextColor(RGB(255, 255, 255));
	}
	else if (theGlobal.GetSystemSet().nActionMode == DebugMode)
	{
		m_STC_CurrentStatus.Format("%s", "调试模式");
		m_STC_CurrentStatus.SetTextColor(RGB(255, 191, 0));
	}
	else if (theGlobal.GetSystemSet().nActionMode == SimulateMode)
	{
		m_STC_CurrentStatus.Format("%s", "仿真模式");
		m_STC_CurrentStatus.SetTextColor(RGB(255, 191, 0));
	}
	else
	{
		m_STC_CurrentStatus.Format("%s", "GRR验证模式");
		m_STC_CurrentStatus.SetTextColor(RGB(255, 191, 0));
	}

	//
}

void CSubDispScope::OpenTimer()
{
	SetTimer(UM_TIMER_TIME, 1000, NULL);
}

void CSubDispScope::CloseTimer()
{
	KillTimer(UM_TIMER_TIME);
}

void CSubDispScope::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (UM_TIMER_TIME == nIDEvent)//1秒钟一次
	{
		RefreshCountStatic();
		RefreshBatchDisp();
	}
	CBCGPDialog::OnTimer(nIDEvent);
}

void CSubDispScope::InitWindowShow()
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

void CSubDispScope::DispScope(C_MergeWork& tOutput)
{
	if (theGlobal.m_Speed.GetSpeed() > 50 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 3 != 0) return;
	if (theGlobal.m_Speed.GetSpeed() > 30 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 2 != 0) return;

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	std::vector<COScopeCtrl::tagPoint> vecVal;
	std::vector<BOOL> vecIsShow;

	COScopeCtrl::tagPoint tPtTmp;//多点值

	BOOL bHasCoat = tOutput.vecB[0].tMeasure.hv_HasCoat.I();
	if (tOutput.vecB.size() == 2 && tPdInfo.nCoatingType == TwoSide)
	{
		bHasCoat = bHasCoat && tOutput.vecB[1].tMeasure.hv_HasCoat.I();
	}
	CDataCalculate::tagData tCpkData = theGlobal.m_DataCalculate.GetCurrentMeasureQuota(0);
#pragma region 涂膜宽度
	tPtTmp.vecPt.clear();
	vecIsShow.clear();
	vecVal.clear();
	//涂膜宽
	tPtTmp.vecPt.clear();
	for (int i = 0; i < tOutput.vecB[0].tMeasure.tCoat.hv_Width.Length(); i++)
	{
		vecIsShow.push_back(bHasCoat);
		COScopeCtrl::tagPoint tPt;
		tPt.vecPt.clear();
		tPt.vecPt.push_back(tOutput.vecB[0].tMeasure.tCoat.hv_Width[i].D());
		tPt.dCpk = theGlobal.m_DataCalculate.GetCurrentCoatCpk(0, i).D();
		tPt.dSig = theGlobal.m_DataCalculate.GetCurrentCoatSig(0, i).D();
		if (tPdInfo.nCoatingType == TwoSide)
			tPt.vecPt.push_back(tOutput.vecB[1].tMeasure.tCoat.hv_Width[i].D());
		vecVal.push_back(tPt);
	}

	
	for (int i = 0; i < MAX_SCOPE; i++)
	{
		if (vecVal.size() >= SCOPE_COL_NUM[0]) break;
		tPtTmp.vecPt.clear();
		tPtTmp.vecPt.push_back(0);
		tPtTmp.dCpk = 1.34;
		tPtTmp.dSig = 0;
		tPtTmp.dMean = 0;
		vecVal.push_back(tPtTmp);
		vecIsShow.push_back(false);
	}

	for (int i = 0; i < SCOPE_COL_NUM[0]; i++)
	{
		if (!vecIsShow[i]) continue;
		m_Oscope1[i].AppendPoint(vecVal[i]);
	}
#pragma endregion

#pragma region 极耳宽度
	tPtTmp.vecPt.clear();
	vecIsShow.clear();
	vecVal.clear();
	//极耳宽度
	tPtTmp.vecPt.clear();
	for (int i = 0; i < tOutput.vecB[0].tMeasure.tFoil.hv_Width.Length(); i++)
	{
		vecIsShow.push_back(bHasCoat);
		tPtTmp.vecPt.clear();
		tPtTmp.vecPt.push_back(tOutput.vecB[0].tMeasure.tFoil.hv_Width[i].D());
		if (tPdInfo.nCoatingType == TwoSide)
			tPtTmp.vecPt.push_back(tOutput.vecB[1].tMeasure.tFoil.hv_Width[i].D());
		tPtTmp.dCpk = theGlobal.m_DataCalculate.GetCurrentFoilCpk(0, i).D();
		tPtTmp.dSig = theGlobal.m_DataCalculate.GetCurrentFoilSig(0, i).D();
		vecVal.push_back(tPtTmp);
	}

	for (int i = 0; i < MAX_SCOPE; i++)
	{
		if (vecVal.size() >= SCOPE_COL_NUM[1]) break;
		tPtTmp.vecPt.clear();
		tPtTmp.vecPt.push_back(0);
		tPtTmp.dCpk = 1.34;
		tPtTmp.dSig = 0;
		vecVal.push_back(tPtTmp);
		vecIsShow.push_back(false);
	}

	for (int i = 0; i < SCOPE_COL_NUM[1]; i++)
	{
		if (!vecIsShow[i]) continue;
		m_Oscope2[i].AppendPoint(vecVal[i]);
	}
#pragma endregion

#pragma region 凹版/AT11
	if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
	{
		vecIsShow.clear();
		vecVal.clear();
		//凹版/AT11宽
		tPtTmp.vecPt.clear();
		if (tPdInfo.bExistAT11)
		{
			for (int i = 0; i < tOutput.vecB[0].tMeasure.tAT11.hv_Width.Length(); i++)
			{
				vecIsShow.push_back(bHasCoat);
				tPtTmp.vecPt.clear();
				tPtTmp.vecPt.push_back(tOutput.vecB[0].tMeasure.tAT11.hv_Width[i].D());
				if (tPdInfo.nCoatingType == TwoSide)
					tPtTmp.vecPt.push_back(tOutput.vecB[1].tMeasure.tAT11.hv_Width[i].D());
				tPtTmp.dCpk = theGlobal.m_DataCalculate.GetCurrentAT11BasecoatCpk(0, i).D();
				tPtTmp.dSig = theGlobal.m_DataCalculate.GetCurrentAT11BasecoatSig(0, i).D();
				vecVal.push_back(tPtTmp);
			}
		}
		else
		{
			for (int i = 0; i < tOutput.vecB[0].tMeasure.tBaseCoat.hv_Width.Length(); i++)
			{
				vecIsShow.push_back(bHasCoat);
				tPtTmp.vecPt.clear();
				tPtTmp.vecPt.push_back(tOutput.vecB[0].tMeasure.tBaseCoat.hv_Width[i].D());
				if (tPdInfo.nCoatingType == TwoSide)
					tPtTmp.vecPt.push_back(tOutput.vecB[1].tMeasure.tBaseCoat.hv_Width[i].D());
				tPtTmp.dCpk = theGlobal.m_DataCalculate.GetCurrentAT11BasecoatCpk(0, i).D();
				tPtTmp.dSig = theGlobal.m_DataCalculate.GetCurrentAT11BasecoatSig(0, i).D();
				vecVal.push_back(tPtTmp);
			}
		}


		for (int i = 0; i < MAX_SCOPE; i++)
		{
			if (vecVal.size() >= SCOPE_COL_NUM[2]) break;
			tPtTmp.vecPt.clear();
			tPtTmp.vecPt.push_back(0);
			tPtTmp.dCpk = 1.34;
			tPtTmp.dSig = 0;
			vecVal.push_back(tPtTmp);
			vecIsShow.push_back(false);
		}

		for (int i = 0; i < SCOPE_COL_NUM[2]; i++)
		{
			if (!vecIsShow[i]) continue;
			m_Oscope3[i].AppendPoint(vecVal[i]);
		}
	}
#pragma endregion

#pragma region 错位
	if (tPdInfo.nCoatingType == TwoSide)
	{
		vecIsShow.clear();
		vecVal.clear();
		//错位均值
		vecIsShow.push_back(bHasCoat);
		tPtTmp.vecPt.clear();
		tPtTmp.vecPt.push_back(tOutput.tMerge.hv_Misalignment.TupleMean().D());
		tPtTmp.dSig = 0;
		vecVal.push_back(tPtTmp);
		//错位值
		for (int i = 0; i < tOutput.tMerge.hv_Misalignment.Length(); i++)
		{
			vecIsShow.push_back(bHasCoat);
			tPtTmp.vecPt.clear();
			tPtTmp.vecPt.push_back(tOutput.tMerge.hv_Misalignment[i].D());
			tPtTmp.dSig = theGlobal.m_DataCalculate.GetCurrentMisalignmentSig(0, i).D();
			vecVal.push_back(tPtTmp);
		}


		for (int i = 0; i < MAX_SCOPE; i++)
		{
			if (vecVal.size() >= SCOPE_COL_NUM[3]) break;
			tPtTmp.vecPt.clear();
			tPtTmp.vecPt.push_back(0);
			vecVal.push_back(tPtTmp);
			vecIsShow.push_back(false);
		}

		for (int i = 0; i < SCOPE_COL_NUM[3]; i++)
		{
			if (!vecIsShow[i]) continue;
			m_Oscope4[i].AppendPoint(vecVal[i]);
		}
	}
#pragma endregion
}

void CSubDispScope::DispOutput(C_MergeWork& tOutput)
{
	HTuple Width, Height;
	CString strMessage;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	try
	{
		for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
		{
			/*HObject hoImage;
			GenEmptyObj(&hoImage);
			for (int nAWorkSN = 0; nAWorkSN < tOutput.vecB[nBWorkSN].vecA.size(); nAWorkSN++)
			{
				ConcatObj(hoImage, tOutput.vecB[nBWorkSN].vecA[nAWorkSN].hoGrabImage, &hoImage);
			}
			if (hoImage.CountObj() > 1)*/
			{
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

					if (theGlobal.GetSystemSet().nActionMode == RunMode)
					{
						if (nGrabberSN == 0 && tOutput.tMerge.hv_NGType.Length() > 0)
						{
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-16-*-0-*-*-1-"));
							strMessage.Format("%s", theGlobal.m_Counter.GetNGTypeName(tOutput.tMerge.hv_NGType.I()));
							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 10,
								10, "red", "false");
						}
						else if (nGrabberSN == 0 && tOutput.tMerge.hv_WarnType.Length() > 0)
						{
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-16-*-0-*-*-1-"));
							strMessage.Format("%s", theGlobal.m_Counter.GetNGTypeName(tOutput.tMerge.hv_WarnType.I()));
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

					if ((theGlobal.m_Project.GetProjectConfig().nSideCount == 1 && theGlobal.m_Project.GetProjectConfig().nCamCount == 1)
						|| (theGlobal.m_Project.GetProjectConfig().nSideCount == 2 && theGlobal.m_Project.GetProjectConfig().nCamCount == 2))
					{
						if (nBWorkSN == 0)
							strMessage = "正面";
						else
							strMessage = "反面";
						HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-30-*-0-*-*-1-"));
						CString strColor = "green";
						disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", Height.I() / 3,
							10, HTuple(strColor), "false");
					}
					else
					{
						if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
						{
							if (nBWorkSN == 0)
								strMessage = "A面";
							else
								strMessage = "B面";
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-30-*-0-*-*-1-"));
							CString strColor = "green";
							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", Height.I() / 3,
								10, HTuple(strColor), "false");
						}
						else
						{
							if (nBWorkSN == 0)
								strMessage = "B面";
							else
								strMessage = "A面";
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-30-*-0-*-*-1-"));
							CString strColor = "green";
							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", Height.I() / 3,
								10, HTuple(strColor), "false");
						}
					}

					if ((nBWorkSN == 0 && theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType == OneSide) ||
						theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType == TwoSide)
					{
						for (int nWidthSN = 0; nWidthSN < tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width.Length(); nWidthSN++)
						{
							if((tPdInfo.bExistBaseCoat|| tPdInfo.bExistAT11) && nWidthSN%2==1)
								HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-22-*-0-*-*-1-"));
							else
								HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-22-*-0-*-*-1-"));

							if (tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width[nWidthSN] != NO_DATA)
								strMessage.Format("%.2f", tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width[nWidthSN].D());
							else
								strMessage = "NA";
							CString strColor = "green";
							if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_AllWidthFlagList[nWidthSN].I() == NG)
								strColor = "red";
							else if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_AllWidthFlagList[nWidthSN].I() == WARN)
								strColor = "#f8b62b";
							if ((tPdInfo.bExistBaseCoat || tPdInfo.bExistAT11) && nWidthSN % 4 == 1)
							{
								disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", nWidthSN % 2 == 0 ? Height.I() / 4 : Height.I() / 3 * 2 - 400,
									(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN] + tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN + 1]) / 2 - 250, HTuple(strColor), "false");
							}
							else if ((tPdInfo.bExistBaseCoat || tPdInfo.bExistAT11) && nWidthSN % 4 == 3)
							{
								disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", nWidthSN % 2 == 0 ? Height.I() / 4 : Height.I() / 3 * 2 + 200,
									(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN] + tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN + 1]) / 2 - 250, HTuple(strColor), "false");
							}
							else
							{
								disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", nWidthSN % 2 == 0 ? Height.I() / 4 : Height.I() / 3 * 2 -1000,
									(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN] + tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nWidthSN + 1]) / 2 - 250, HTuple(strColor), "false");
							}
						}
						if (tOutput.vecB.size() > 1 && tOutput.vecB[0].tMeasure.hv_HasCoat.I() && tOutput.vecB[1].tMeasure.hv_HasCoat.I())
						{
							HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-16-*-0-*-*-1-"));
							for (int nSN = 0; nSN < tOutput.tMerge.hv_Misalignment.Length(); nSN++)
							{
								if (tOutput.tMerge.hv_Misalignment[nSN] != NO_DATA)
									strMessage.Format("%.2f", tOutput.tMerge.hv_Misalignment[nSN].D());
								else
									strMessage = "NA";
								CString strColor = "green";
								if (tOutput.tMerge.hv_MisalignmentFlagList[nSN].I() == NG)
									strColor = "red";
								else if (tOutput.tMerge.hv_MisalignmentFlagList[nSN].I() == WARN)
									strColor = "#f8b62b";

								//涂膜增加错位
								if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
								{
									if (nSN % 2 == 0)
										disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 0,
											tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[(nSN + 1) * 2] - 800, HTuple(strColor), "false");
									else
										disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 0,
											tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nSN * 2 + 1] + 200, HTuple(strColor), "false");
								}
								else
								{
									/*if (nSN % 2 == 0)
										disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 0,
											tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nSN + 1] - 800, HTuple(strColor), "false");
									else*/
										disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 0,
											tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn[nSN + 1] - 250, HTuple(strColor), "false");
								}
							}
							//涂膜错位均值
							if (tOutput.tMerge.hv_Misalignment != NO_DATA)
								strMessage.Format("%.2f", tOutput.tMerge.hv_Misalignment.TupleMean().D());
							else
								strMessage = "NA";

							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("错位均值"), "image", 0,
								(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn.TupleMax().D() + Width.I()) / 2, HTuple("yellow"), "false");
							disp_message(m_hvecMainImageHandle[nGrabberSN].T(), HTuple(strMessage), "image", 200,
								(tOutput.vecB[nBWorkSN].tMeasure.tLine.hv_StartColumn.TupleMax().D() + Width.I()) / 2, HTuple("yellow"), "false");
						}
					}
				}

				//显示耗时
				HTuple hv_MaxWidth, hv_MaxHeight, hvW, hvH;
				strMessage.Format("采图:%0.1fms  检测:%0.1fms  综合:%0.1fms", tOutput.vecB[nBWorkSN].vecA[0].hvTimeStamp.D(), tOutput.vecB[nBWorkSN].hv_TimeStamp.D(), tOutput.hv_TimeStamp.D());
				HalconCpp::SetFont(m_hvecMainImageHandle[nGrabberSN].T(), HTuple("-Consolas-15-*-0-*-*-1-"));
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

void CSubDispScope::DispCoatSketch(C_MergeWork& tOutput)
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CCoatSketchStatic::tagWidth tWidth;
	tWidth.Clear();
	BOOL bHasCoat = tOutput.vecB[0].tMeasure.hv_HasCoat.I();
	BOOL bHasCoat2 = FALSE;
	if (tOutput.vecB.size() == 2 && tPdInfo.nCoatingType == TwoSide)
	{
		bHasCoat2 = tOutput.vecB[1].tMeasure.hv_HasCoat.I();
	}

	//涂膜宽
	for (int i = 0; i < tOutput.vecB[0].tMeasure.tCoat.hv_Width.Length(); i++)
	{
		if (bHasCoat)
		{
			tWidth.vecCoatWidth.push_back(tOutput.vecB[0].tMeasure.tCoat.hv_Width[i].D());
			tWidth.vecCoatWidthFlag.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[0].hv_CoatWidthFlagList[i].I());
		}
		if (tPdInfo.nCoatingType == TwoSide && bHasCoat2)
		{
			tWidth.vecCoatWidth2.push_back(tOutput.vecB[1].tMeasure.tCoat.hv_Width[i].D());
			tWidth.vecCoatWidthFlag2.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[1].hv_CoatWidthFlagList[i].I());
		}
	}

	//极耳宽
	for (int i = 0; i < tOutput.vecB[0].tMeasure.tFoil.hv_Width.Length(); i++)
	{
		if (bHasCoat)
		{
			tWidth.vecFoilWidth.push_back(tOutput.vecB[0].tMeasure.tFoil.hv_Width[i].D());
			tWidth.vecFoilWidthFlag.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[0].hv_FoilWidthFlagList[i].I());
		}
		if (tPdInfo.nCoatingType == TwoSide && bHasCoat2)
		{
			tWidth.vecFoilWidth2.push_back(tOutput.vecB[1].tMeasure.tFoil.hv_Width[i].D());
			tWidth.vecFoilWidthFlag2.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[1].hv_FoilWidthFlagList[i].I());
		}
	}

	//AT11/凹版
	if (tPdInfo.bExistAT11)
	{
		for (int i = 0; i < tOutput.vecB[0].tMeasure.tAT11.hv_Width.Length(); i++)
		{
			if (bHasCoat)
			{
				tWidth.vecAT11BaseCoatWidth.push_back(tOutput.vecB[0].tMeasure.tAT11.hv_Width[i].D());
				tWidth.vecAT11BaseCoatWidthFlag.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[0].hv_AT11WidthFlagList[i].I());
			}
			if (tPdInfo.nCoatingType == TwoSide && bHasCoat2)
			{
				tWidth.vecAT11BaseCoatWidth2.push_back(tOutput.vecB[1].tMeasure.tAT11.hv_Width[i].D());
				tWidth.vecAT11BaseCoatWidthFlag2.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[1].hv_AT11WidthFlagList[i].I());
			}
		}
	}
	else if (tPdInfo.bExistBaseCoat)
	{
		for (int i = 0; i < tOutput.vecB[0].tMeasure.tBaseCoat.hv_Width.Length(); i++)
		{
			if (bHasCoat)
			{
				tWidth.vecAT11BaseCoatWidth.push_back(tOutput.vecB[0].tMeasure.tBaseCoat.hv_Width[i].D());
				tWidth.vecAT11BaseCoatWidthFlag.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[0].hv_BaseCoatWidthFlagList[i].I());
			}
			if (tPdInfo.nCoatingType == TwoSide && bHasCoat2)
			{
				tWidth.vecAT11BaseCoatWidth2.push_back(tOutput.vecB[1].tMeasure.tBaseCoat.hv_Width[i].D());
				tWidth.vecAT11BaseCoatWidthFlag2.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.vecSideResult[1].hv_BaseCoatWidthFlagList[i].I());
			}
		}
	}

	//错位
	if (tPdInfo.nCoatingType == TwoSide && bHasCoat && bHasCoat2)
	{
		for (int i = 0; i < tOutput.tMerge.hv_Misalignment.Length(); i++)
		{
			tWidth.vecMisalignment.push_back(tOutput.tMerge.hv_Misalignment[i].D());
			tWidth.vecMisalignmentFlag.push_back((CCoatSketchStatic::eFlag)tOutput.tMerge.hv_MisalignmentFlagList[i].I());
		}
	}

	//基材总宽
	if (bHasCoat)
	{
		tWidth.dWholeWidth = tOutput.vecB[0].tMeasure.tSize.hv_WholeWidth.D();
	}
	if (tPdInfo.nCoatingType == TwoSide && bHasCoat2)
	{
		tWidth.dWholeWidth2 = tOutput.vecB[1].tMeasure.tSize.hv_WholeWidth.D();
	}

	//瑕疵
	if (bHasCoat)
	{
		for (int i = 0; i < tOutput.vecB[0].tMeasure.tDefect.tFeature.tCenter.hv_Column.Length(); i++)
		{
			CCoatSketchStatic::tagDefect tDefect;
			tDefect.dCenterX = tOutput.vecB[0].tMeasure.tDefect.tFeature.tCenter.hv_Column[i].D();
			tDefect.dWidth = tOutput.vecB[0].tMeasure.tDefect.tFeature.hvWidth[i].D();
			tDefect.dHeight = tOutput.vecB[0].tMeasure.tDefect.tFeature.hvHeight[i].D();
			tDefect.strName = theGlobal.m_Counter.GetNGTypeName(tOutput.vecB[0].tMeasure.tDefect.hv_Type[i].I());

			tWidth.vecDefects.push_back(tDefect);
		}
	}
	if (tPdInfo.nCoatingType == TwoSide && bHasCoat2)
	{
		for (int i = 0; i < tOutput.vecB[1].tMeasure.tDefect.tFeature.tCenter.hv_Column.Length(); i++)
		{
			CCoatSketchStatic::tagDefect tDefect;
			tDefect.dCenterX = tOutput.vecB[1].tMeasure.tDefect.tFeature.tCenter.hv_Column[i].D();
			tDefect.dWidth = tOutput.vecB[1].tMeasure.tDefect.tFeature.hvWidth[i].D();
			tDefect.dHeight = tOutput.vecB[1].tMeasure.tDefect.tFeature.hvHeight[i].D();
			tDefect.strName = theGlobal.m_Counter.GetNGTypeName(tOutput.vecB[1].tMeasure.tDefect.hv_Type[i].I());

			tWidth.vecDefects2.push_back(tDefect);
		}
	}

	m_CoatSketch.ShowValue(tWidth);
}