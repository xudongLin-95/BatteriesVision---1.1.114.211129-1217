// PageCheckSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageCheckSet.h"
#include "afxdialogex.h"


// CPageCheckSet 对话框

IMPLEMENT_DYNAMIC(CPageCheckSet, CBCGPDialog)

CPageCheckSet::CPageCheckSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_PAGE_CHECK_SET, pParent)
{

}

CPageCheckSet::~CPageCheckSet()
{
}

void CPageCheckSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageCheckSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_SPOTINSPECTION, &CPageCheckSet::OnBnClickedButtonSpotinspection)
END_MESSAGE_MAP()


// CPageCheckSet 消息处理程序


BOOL CPageCheckSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

BOOL CPageCheckSet::PreTranslateMessage(MSG* pMsg)
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


void CPageCheckSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageCheckSet::ResetSize()
{
	InitUI();
}

void CPageCheckSet::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcProp, rcGridParam, rcDlg;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	GetDlgItem(IDC_STATIC_CH_SPOTINSPECTION)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	rcGridBase.left = rcDlg.left + 15;
	rcGridBase.top = rcGridBase.bottom + 15;
	rcGridBase.bottom = rcGridBase.top + 150;
	rcGridBase.right = rcDlg.right - 15;

	m_Grid_SpotInspectionParam.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_SpotInspectionParam, theme);
	m_Grid_SpotInspectionParam.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_SpotInspectionParam.SetColorTheme(theme);
	m_Grid_SpotInspectionParam.SetReadOnly(FALSE);//不可编辑
	m_Grid_SpotInspectionParam.SetWholeRowSel(FALSE);
	m_Grid_SpotInspectionParam.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_SpotInspectionParam.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_SpotInspectionParam.EnableCheckBoxes(FALSE);
	m_Grid_SpotInspectionParam.EnableRowHeader(TRUE);//使用行表头
	m_Grid_SpotInspectionParam.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_SpotInspectionParam.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_SpotInspectionParam.EnableInvertSelOnCtrl(FALSE);
	m_Grid_SpotInspectionParam.EnableDragHeaderItems(FALSE);
	m_Grid_SpotInspectionParam.InsertColumn(0, "项目", 100);
	m_Grid_SpotInspectionParam.InsertColumn(1, "选项", 60);
	m_Grid_SpotInspectionParam.InsertColumn(2, "直径11", 80);
	m_Grid_SpotInspectionParam.InsertColumn(3, "直径12", 80);
	m_Grid_SpotInspectionParam.InsertColumn(4, "圆心距1", 80);
	m_Grid_SpotInspectionParam.InsertColumn(5, "直径21", 80);
	m_Grid_SpotInspectionParam.InsertColumn(6, "直径22", 80);
	m_Grid_SpotInspectionParam.InsertColumn(7, "圆心距2", 80);
	m_Grid_SpotInspectionParam.InsertColumn(8, "圆心距3", 80);
	m_Grid_SpotInspectionParam.LoadState(_T("BasicGrid"));
	m_Grid_SpotInspectionParam.AdjustLayout();//相当于数据显示在表格上
	m_Grid_SpotInspectionParam.ShowWindow(SW_NORMAL);

	RefeshSpotInspectionParam();

	CRect rcBtn, rcMove;
	GetDlgItem(IDC_BUTTON_SPOTINSPECTION)->GetClientRect(rcBtn);
	int BTN_GAP = 5;
	rcMove.left = rcGridBase.left;
	rcMove.right = rcMove.left + rcBtn.Width();
	rcMove.top = rcGridBase.bottom + 15;
	rcMove.bottom = rcMove.top + rcBtn.Height();
	GetDlgItem(IDC_BUTTON_SPOTINSPECTION)->MoveWindow(rcMove);

}

void CPageCheckSet::RefeshSpotInspectionParam()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	m_Grid_SpotInspectionParam.RemoveAll();
	CString strValue;
	CBCGPGridRow* pRow;
	//正面规格
	pRow = m_Grid_SpotInspectionParam.CreateRow(m_Grid_SpotInspectionParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("点检正面规格"), FALSE);
	pRow->GetItem(1)->AddOption("不启用");
	pRow->GetItem(1)->AddOption("启用");
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseChecking ? "启用" : "不启用"), FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[0].dStandard), FALSE);
	pRow->GetItem(3)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[1].dStandard), FALSE);
	pRow->GetItem(4)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[2].dStandard), FALSE);
	pRow->GetItem(5)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[3].dStandard), FALSE);
	pRow->GetItem(6)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[4].dStandard), FALSE);
	pRow->GetItem(7)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[5].dStandard), FALSE);
	pRow->GetItem(8)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[6].dStandard), FALSE);
	m_Grid_SpotInspectionParam.AddRow(pRow, FALSE);
	//反面规格
	pRow = m_Grid_SpotInspectionParam.CreateRow(m_Grid_SpotInspectionParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("点检反面规格"), FALSE);
	pRow->GetItem(1)->Enable(FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[0].dStandard2), FALSE);
	pRow->GetItem(3)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[1].dStandard2), FALSE);
	pRow->GetItem(4)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[2].dStandard2), FALSE);
	pRow->GetItem(5)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[3].dStandard2), FALSE);
	pRow->GetItem(6)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[4].dStandard2), FALSE);
	pRow->GetItem(7)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[5].dStandard2), FALSE);
	pRow->GetItem(8)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[6].dStandard2), FALSE);
	m_Grid_SpotInspectionParam.AddRow(pRow, FALSE);
	//公差
	pRow = m_Grid_SpotInspectionParam.CreateRow(m_Grid_SpotInspectionParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("点检公差"), FALSE);
	pRow->GetItem(1)->Enable(FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[0].dTolerance), FALSE);
	pRow->GetItem(3)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[1].dTolerance), FALSE);
	pRow->GetItem(4)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[2].dTolerance), FALSE);
	pRow->GetItem(5)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[3].dTolerance), FALSE);
	pRow->GetItem(6)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[4].dTolerance), FALSE);
	pRow->GetItem(7)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[5].dTolerance), FALSE);
	pRow->GetItem(8)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[6].dTolerance), FALSE);
	m_Grid_SpotInspectionParam.AddRow(pRow, FALSE);
	//正面补偿
	pRow = m_Grid_SpotInspectionParam.CreateRow(m_Grid_SpotInspectionParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("点检正面补偿"), FALSE);
	pRow->GetItem(1)->Enable(FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[0].OffsetList[0]), FALSE);
	pRow->GetItem(3)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[1].OffsetList[0]), FALSE);
	pRow->GetItem(4)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[2].OffsetList[0]), FALSE);
	pRow->GetItem(5)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[3].OffsetList[0]), FALSE);
	pRow->GetItem(6)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[4].OffsetList[0]), FALSE);
	pRow->GetItem(7)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[5].OffsetList[0]), FALSE);
	pRow->GetItem(8)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[6].OffsetList[0]), FALSE);
	m_Grid_SpotInspectionParam.AddRow(pRow, FALSE);
	//反面补偿
	pRow = m_Grid_SpotInspectionParam.CreateRow(m_Grid_SpotInspectionParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("点检反面补偿"), FALSE);
	pRow->GetItem(1)->Enable(FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[0].OffsetList[1]), FALSE);
	pRow->GetItem(3)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[1].OffsetList[1]), FALSE);
	pRow->GetItem(4)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[2].OffsetList[1]), FALSE);
	pRow->GetItem(5)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[3].OffsetList[1]), FALSE);
	pRow->GetItem(6)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[4].OffsetList[1]), FALSE);
	pRow->GetItem(7)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[5].OffsetList[1]), FALSE);
	pRow->GetItem(8)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[6].OffsetList[1]), FALSE);
	m_Grid_SpotInspectionParam.AddRow(pRow, FALSE);

	m_Grid_SpotInspectionParam.AdjustLayout();
}

tagPDInfo CPageCheckSet::GetSpotInspectionParam()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//正面规格
	CString strValue = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(1)->GetValue();
	tPdInfo.tParam.bUseChecking = strValue == "启用" ? TRUE : FALSE;
	tPdInfo.tParam.vecChecking[0].dStandard = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(2)->GetValue();
	tPdInfo.tParam.vecChecking[1].dStandard = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(3)->GetValue();
	tPdInfo.tParam.vecChecking[2].dStandard = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(4)->GetValue();
	tPdInfo.tParam.vecChecking[3].dStandard = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(5)->GetValue();
	tPdInfo.tParam.vecChecking[4].dStandard = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(6)->GetValue();
	tPdInfo.tParam.vecChecking[5].dStandard = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(7)->GetValue();
	tPdInfo.tParam.vecChecking[6].dStandard = m_Grid_SpotInspectionParam.GetRow(0)->GetItem(8)->GetValue();
	//反面规格
	tPdInfo.tParam.vecChecking[0].dStandard2 = m_Grid_SpotInspectionParam.GetRow(1)->GetItem(2)->GetValue();
	tPdInfo.tParam.vecChecking[1].dStandard2 = m_Grid_SpotInspectionParam.GetRow(1)->GetItem(3)->GetValue();
	tPdInfo.tParam.vecChecking[2].dStandard2 = m_Grid_SpotInspectionParam.GetRow(1)->GetItem(4)->GetValue();
	tPdInfo.tParam.vecChecking[3].dStandard2 = m_Grid_SpotInspectionParam.GetRow(1)->GetItem(5)->GetValue();
	tPdInfo.tParam.vecChecking[4].dStandard2 = m_Grid_SpotInspectionParam.GetRow(1)->GetItem(6)->GetValue();
	tPdInfo.tParam.vecChecking[5].dStandard2 = m_Grid_SpotInspectionParam.GetRow(1)->GetItem(7)->GetValue();
	tPdInfo.tParam.vecChecking[6].dStandard2 = m_Grid_SpotInspectionParam.GetRow(1)->GetItem(8)->GetValue();
	//公差
	tPdInfo.tParam.vecChecking[0].dTolerance = m_Grid_SpotInspectionParam.GetRow(2)->GetItem(2)->GetValue();
	tPdInfo.tParam.vecChecking[1].dTolerance = m_Grid_SpotInspectionParam.GetRow(2)->GetItem(3)->GetValue();
	tPdInfo.tParam.vecChecking[2].dTolerance = m_Grid_SpotInspectionParam.GetRow(2)->GetItem(4)->GetValue();
	tPdInfo.tParam.vecChecking[3].dTolerance = m_Grid_SpotInspectionParam.GetRow(2)->GetItem(5)->GetValue();
	tPdInfo.tParam.vecChecking[4].dTolerance = m_Grid_SpotInspectionParam.GetRow(2)->GetItem(6)->GetValue();
	tPdInfo.tParam.vecChecking[5].dTolerance = m_Grid_SpotInspectionParam.GetRow(2)->GetItem(7)->GetValue();
	tPdInfo.tParam.vecChecking[6].dTolerance = m_Grid_SpotInspectionParam.GetRow(2)->GetItem(8)->GetValue();
	//正面补偿
	tPdInfo.tParam.vecChecking[0].OffsetList[0] = m_Grid_SpotInspectionParam.GetRow(3)->GetItem(2)->GetValue();
	tPdInfo.tParam.vecChecking[1].OffsetList[0] = m_Grid_SpotInspectionParam.GetRow(3)->GetItem(3)->GetValue();
	tPdInfo.tParam.vecChecking[2].OffsetList[0] = m_Grid_SpotInspectionParam.GetRow(3)->GetItem(4)->GetValue();
	tPdInfo.tParam.vecChecking[3].OffsetList[0] = m_Grid_SpotInspectionParam.GetRow(3)->GetItem(5)->GetValue();
	tPdInfo.tParam.vecChecking[4].OffsetList[0] = m_Grid_SpotInspectionParam.GetRow(3)->GetItem(6)->GetValue();
	tPdInfo.tParam.vecChecking[5].OffsetList[0] = m_Grid_SpotInspectionParam.GetRow(3)->GetItem(7)->GetValue();
	tPdInfo.tParam.vecChecking[6].OffsetList[0] = m_Grid_SpotInspectionParam.GetRow(3)->GetItem(8)->GetValue();
	//反面补偿
	tPdInfo.tParam.vecChecking[0].OffsetList[1] = m_Grid_SpotInspectionParam.GetRow(4)->GetItem(2)->GetValue();
	tPdInfo.tParam.vecChecking[1].OffsetList[1] = m_Grid_SpotInspectionParam.GetRow(4)->GetItem(3)->GetValue();
	tPdInfo.tParam.vecChecking[2].OffsetList[1] = m_Grid_SpotInspectionParam.GetRow(4)->GetItem(4)->GetValue();
	tPdInfo.tParam.vecChecking[3].OffsetList[1] = m_Grid_SpotInspectionParam.GetRow(4)->GetItem(5)->GetValue();
	tPdInfo.tParam.vecChecking[4].OffsetList[1] = m_Grid_SpotInspectionParam.GetRow(4)->GetItem(6)->GetValue();
	tPdInfo.tParam.vecChecking[5].OffsetList[1] = m_Grid_SpotInspectionParam.GetRow(4)->GetItem(7)->GetValue();
	tPdInfo.tParam.vecChecking[6].OffsetList[1] = m_Grid_SpotInspectionParam.GetRow(4)->GetItem(8)->GetValue();

	return tPdInfo;
}

void CPageCheckSet::OnBnClickedButtonSpotinspection()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Product, NULL)) return;
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("是否保存点检参数?", "Whether to save spot inspection params?", ""))
		return;

	tagPDInfo tPdInfo = GetSpotInspectionParam();

	theGlobal.m_ProductTool.SetCurrentProductInfo(tPdInfo);
	theGlobal.m_ProductTool.SetCurrentProductInfoTmp(tPdInfo);

	//给算法更新产品信息
	for (int nSideSN = 0; nSideSN < theGlobal.m_Project.GetProjectConfig().nSideCount; nSideSN++)
	{
		theGlobal.m_AlgTool.DoSomething(nSideSN, CAlgorithmTool::AlgMeasure, "SetProductInfo", &tPdInfo, NULL);
	}

	theGlobal.m_ProductTool.WriteSpotInspectionParam();
}
