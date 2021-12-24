// PageProductSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageProductSet.h"
#include "afxdialogex.h"
#include "CustomProperties.h"

// CPageProductSet 对话框

IMPLEMENT_DYNAMIC(CPageProductSet, CBCGPDialog)

CPageProductSet::CPageProductSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageProductSet::IDD, pParent)
{
	m_nProductSel = 0;
	m_nCurrentProductSel = 0;
	m_ftProp.CreateFontA(17,0,0,0,0,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS
		,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,"Microsoft Yahei");
}

CPageProductSet::~CPageProductSet()
{
	m_ftProp.DeleteObject();
}

void CPageProductSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageProductSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_SET, &CPageProductSet::OnBnClickedButtonProductSet)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_ENDINPLACEEDIT, &CPageProductSet::OnEndEditGridProduct)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_BEGININPLACEEDIT, &CPageProductSet::OnBeginEditGridProduct)
	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, &CPageProductSet::OnItemChangedGridProduct)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_NEW, &CPageProductSet::OnBnClickedButtonProductNew)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_LOAD, &CPageProductSet::OnBnClickedButtonProductLoad)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_DELETE, &CPageProductSet::OnBnClickedButtonProductDelete)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_SAVE, &CPageProductSet::OnBnClickedButtonProductSave)
END_MESSAGE_MAP()


// CPageProductSet 消息处理程序


BOOL CPageProductSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageProductSet::PreTranslateMessage(MSG* pMsg)
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


void CPageProductSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageProductSet::ResetSize()
{
	InitUI();
}

void CPageProductSet::InitUI()
{
	BOOL bChinese = theGlobal.m_Language.IsChinese();
	CBCGPGridColors theme;
	CRect rcGridBase, rcProp, rcGridParam, rcDlg;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	GetDlgItem(IDC_STATIC_CH_PRODUCTPARAM)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	rcGridBase.top = rcGridBase.bottom + 15;
	rcGridBase.bottom = rcDlg.bottom - 50;
	rcGridBase.right = 350;

	m_Grid_ProductList.Create (WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme(&m_Grid_ProductList, theme);
	m_Grid_ProductList.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_ProductList.SetColorTheme(theme);
	m_Grid_ProductList.SetReadOnly(TRUE);//不可编辑
	m_Grid_ProductList.SetWholeRowSel(TRUE);
	m_Grid_ProductList.EnableMarkSortedColumn (FALSE);//使用列排序
	m_Grid_ProductList.EnableHeader (TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_ProductList.EnableRowHeader (TRUE);//使用行表头
	m_Grid_ProductList.EnableLineNumbers (TRUE);//使用行序号
	m_Grid_ProductList.SetClearInplaceEditOnEnter (FALSE);
	m_Grid_ProductList.EnableInvertSelOnCtrl (FALSE);
	m_Grid_ProductList.InsertColumn (0, bChinese?"配方名":"Product Name", 100);
	m_Grid_ProductList.InsertColumn (1, bChinese?"备注":"Note", rcGridBase.Width()-160);
	m_Grid_ProductList.LoadState (_T("BasicGrid"));
	m_Grid_ProductList.AdjustLayout ();//相当于数据显示在表格上
	m_Grid_ProductList.ShowWindow(SW_NORMAL);
	m_Grid_ProductList.SetFont(&m_ftProp);

	RefeshProductList();
	m_Grid_ProductList.SelectRow(m_nCurrentProductSel);
	m_nProductSel = m_nCurrentProductSel;

	CRect rcBtn, rcMove;
	GetDlgItem(IDC_BUTTON_PRODUCT_NEW)->GetClientRect(rcBtn);
	int BTN_GAP = 5;
	rcMove.left = rcGridBase.left;
	rcMove.right = rcMove.left + rcBtn.Width();
	rcMove.top = rcGridBase.bottom + 15;
	rcMove.bottom = rcMove.top + rcBtn.Height();
	GetDlgItem(IDC_BUTTON_PRODUCT_NEW)->MoveWindow(rcMove);
	rcMove.left = rcGridBase.left + rcBtn.Width() + BTN_GAP;
	rcMove.right = rcMove.left + rcBtn.Width();
	rcMove.top = rcGridBase.bottom + 15;
	rcMove.bottom = rcMove.top + rcBtn.Height();
	GetDlgItem(IDC_BUTTON_PRODUCT_LOAD)->MoveWindow(rcMove);
	rcMove.left = rcGridBase.left + rcBtn.Width() * 2 + BTN_GAP * 2;
	rcMove.right = rcMove.left + rcBtn.Width();
	rcMove.top = rcGridBase.bottom + 15;
	rcMove.bottom = rcMove.top + rcBtn.Height();
	GetDlgItem(IDC_BUTTON_PRODUCT_DELETE)->MoveWindow(rcMove);

	//PropList
	rcGridParam = rcGridBase;
	rcGridParam.left = 360;
	rcGridParam.right = rcDlg.right - 10;
	rcGridParam.bottom = rcDlg.bottom - 50;

	m_Grid_ProductParam.Create(WS_CHILD, rcGridParam, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_ProductParam, theme);
	m_Grid_ProductParam.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_ProductParam.SetColorTheme(theme);
	m_Grid_ProductParam.SetReadOnly(FALSE);//不可编辑
	m_Grid_ProductParam.SetWholeRowSel(FALSE);
	m_Grid_ProductParam.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_ProductParam.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_ProductParam.EnableCheckBoxes(FALSE);
	m_Grid_ProductParam.EnableRowHeader(TRUE);//使用行表头
	m_Grid_ProductParam.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_ProductParam.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_ProductParam.EnableInvertSelOnCtrl(FALSE);
	m_Grid_ProductParam.EnableDragHeaderItems(FALSE);
	m_Grid_ProductParam.InsertColumn(0, "项目", 100);
	m_Grid_ProductParam.InsertColumn(1, "规格", rcGridParam.Width() - 160);
	m_Grid_ProductParam.LoadState(_T("BasicGrid"));
	m_Grid_ProductParam.AdjustLayout();//相当于数据显示在表格上
	m_Grid_ProductParam.ShowWindow(SW_NORMAL);
	m_Grid_ProductParam.SetFont(&m_ftProp);

	RefreshProductParam();

	rcMove.left = rcGridParam.left;
	rcMove.right = rcMove.left + rcBtn.Width();
	rcMove.top = rcGridParam.bottom + 15;
	rcMove.bottom = rcMove.top + rcBtn.Height();
	GetDlgItem(IDC_BUTTON_PRODUCT_SAVE)->MoveWindow(rcMove);


	UpdateData(FALSE);
	
	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_PRODUCT_SET");
}


void CPageProductSet::OnBnClickedButtonProductSet()
{
	CList<CBCGPGridItem*, CBCGPGridItem*> sel;
	m_Grid_ProductList.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0 >= nCount || nCount > 2)
	{
		theGlobal.m_Language.MessageboxWarn("请选择配方列表中的一个配方。", "Please select the product from the list.", "");
		return;
	}
	CBCGPGridItem* pItem = sel.GetHead();
	CBCGPGridRow* pRow = pItem->GetParentRow();
	int nRow = pRow->GetRowId();
	theGlobal.m_ProductTool.BackUp(nRow);
	RefeshProductList();
}

void CPageProductSet::RefeshProductList()
{
	ProductList vecList = theGlobal.m_ProductTool.GetProductList();
	if(vecList.size()<=0)
		return;

	HWND hwndList = m_Grid_ProductList.GetSafeHwnd();
	SNDMSG(hwndList,WM_SETREDRAW,FALSE,0);//SetWindowRedraw
	m_Grid_ProductList.RemoveAll();
	
	for (int i=0;i<vecList.size();i++)
	{
		CBCGPGridRow* pRow = m_Grid_ProductList.CreateRow (m_Grid_ProductList.GetColumnCount ());

		pRow->GetItem (0)->SetValue (_variant_t (vecList[i].strPDName));
		pRow->GetItem (1)->SetValue (_variant_t (vecList[i].strNote));
		
		if(i%2==1)
		{
			pRow->GetItem(0)->SetBackgroundColor(RGB(58,58,58));
			pRow->GetItem(1)->SetBackgroundColor(RGB(58,58,58));
		}
		if(theGlobal.m_ProductTool.GetCurrentProductName()==vecList[i].strPDName)
		{
			m_nCurrentProductSel = i;
			pRow->GetItem(0)->SetTextColor(RGB(0,153,223));
			pRow->GetItem(1)->SetTextColor(RGB(0,153,223));
		}
		else
		{
			pRow->GetItem(0)->SetTextColor(RGB(134,134,134));
			pRow->GetItem(1)->SetTextColor(RGB(134,134,134));
		}
		m_Grid_ProductList.InsertRowAfter(i,pRow, FALSE);
	}
	SNDMSG(hwndList,WM_SETREDRAW,TRUE,0);
	m_Grid_ProductList.AdjustLayout();
}

void CPageProductSet::RenameProductList()
{
	int nCount = m_Grid_ProductList.GetRowCount();
	if (nCount<=0)
	{
		theGlobal.m_Language.MessageboxError("产品列表不能为空。", "Product list cannot be empty.", "");
		RefeshProductList();
		return;
	}
	for (int i=0;i<nCount;i++)
	{
		CBCGPGridRow* pRow = m_Grid_ProductList.GetRow(i);
		if (""==(CString)pRow->GetItem(0)->GetValue())
		{
			theGlobal.m_Language.MessageboxError("产品名称不能为空。", "Product name cannot be empty.", "");
			continue;
		}
		if (i==m_nCurrentProductSel && 
			theGlobal.m_ProductTool.GetCurrentProductName()!=(CString)pRow->GetItem(0)->GetValue())
		{
			theGlobal.m_Language.MessageboxError("当前产品正在使用，不能改名。", "Current product in use, cannot be renamed.", "");
			theGlobal.m_ProductTool.Rename(i,(CString)theGlobal.m_ProductTool.GetCurrentProductName(),(CString)pRow->GetItem(1)->GetValue());
			continue;
		}
		theGlobal.m_ProductTool.Rename(i,(CString)pRow->GetItem(0)->GetValue(),(CString)pRow->GetItem(1)->GetValue());
	}

	RefeshProductList();
}

void CPageProductSet::EditRefreshProductParam()
{
	ProductList vecList = theGlobal.m_ProductTool.GetProductList();
	if (vecList.size() <= 0)
		return;
	if (m_nProductSel >= vecList.size())
		return;
	tagPDInfo tPdInfo = vecList[m_nProductSel];

	CString strValue;
	//参数规格
	CBCGPGridRow* pRow;
	//产品名称
	pRow = m_Grid_ProductParam.GetRow(0);
	tPdInfo.strPDName = pRow->GetItem(1)->GetValue();
	//产品备注
	pRow = m_Grid_ProductParam.GetRow(1);
	tPdInfo.strNote = pRow->GetItem(1)->GetValue();
	//产品类型
	pRow = m_Grid_ProductParam.GetRow(2);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "一出三")
		tPdInfo.nPDType = ThreeCavity;
	else if (strValue == "一出二")
		tPdInfo.nPDType = TwoCavity;
	else if (strValue == "一出四")
		tPdInfo.nPDType = FourCavity;
	else if (strValue == "一出六")
		tPdInfo.nPDType = SixCavity;
	else if (strValue == "一出八")
		tPdInfo.nPDType = EightCavity;
	else
		tPdInfo.nPDType = TenCavity;
	//有无AT11/凹版
	pRow = m_Grid_ProductParam.GetRow(3);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "有AT11")
	{
		tPdInfo.bExistAT11 = TRUE;
		tPdInfo.bExistBaseCoat = FALSE;
	}
	else if (strValue == "有凹版")
	{
		tPdInfo.bExistAT11 = FALSE;
		tPdInfo.bExistBaseCoat = TRUE;
	}
	else
	{
		tPdInfo.bExistAT11 = FALSE;
		tPdInfo.bExistBaseCoat = FALSE;
	}
	//有无间歇
	pRow = m_Grid_ProductParam.GetRow(4);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "有间歇")
		tPdInfo.bExistIntermittent = TRUE;
	else
		tPdInfo.bExistIntermittent = FALSE;
	//涂布类型
	pRow = m_Grid_ProductParam.GetRow(5);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "单面")
		tPdInfo.nCoatingType = OneSide;
	else
		tPdInfo.nCoatingType = TwoSide;

	int nSideCount = theGlobal.m_Project.GetProjectConfig().nSideCount;

	//涂布数量
	int nCount = 0;
	nCount = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType);
	tPdInfo.tParam.vecCoatWidth.resize(nCount, tPdInfo.tParam.vecCoatWidth[0]);
	//极耳数量
	nCount = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType + 1);
	tPdInfo.tParam.vecFoilWidth.resize(nCount, tPdInfo.tParam.vecFoilWidth[0]);
	
	if (tPdInfo.bExistAT11)
	{
		//AT11数量
		nCount = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType * 2);
		tagTolerance tTol;
		if (tPdInfo.tParam.vecAT11Width.size() > 0)
		{
			tTol = tPdInfo.tParam.vecAT11Width[0];
		}
		else
		{
			tTol.dStandard = 20;
			tTol.dTolerance = 0.1;
			tTol.dWarnTolerance = 0.07;
			tTol.OffsetList.clear();
			for (int i = 0; i < nSideCount; i++)
			{
				tTol.OffsetList.push_back(0);
			}
		}
		tPdInfo.tParam.vecAT11Width.resize(nCount, tTol);
	}
	else if (tPdInfo.bExistBaseCoat)
	{
		//凹版数量
		nCount = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType * 2);
		tagTolerance tTol;
		if (tPdInfo.tParam.vecBaseCoatWidth.size() > 0)
		{
			tTol = tPdInfo.tParam.vecBaseCoatWidth[0];
		}
		else
		{
			tTol.dStandard = 20;
			tTol.dTolerance = 0.1;
			tTol.dWarnTolerance = 0.07;
			tTol.OffsetList.clear();
			for (int i = 0; i < nSideCount; i++)
			{
				tTol.OffsetList.push_back(0);
			}
		}
		tPdInfo.tParam.vecBaseCoatWidth.resize(nCount, tTol);
	}

	//A/B面错位值
	if (tPdInfo.nCoatingType == TwoSide)
	{
		if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
			nCount = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 4 + 1);
		else
			nCount = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 2 + 1);

		tagTolerance tTol;
		if (tPdInfo.tParam.vecMisalignment.size() > 0)
		{
			tTol = tPdInfo.tParam.vecMisalignment[0];
		}
		else
		{
			tTol.dStandard = 0;
			tTol.dTolerance = 0.1;
			tTol.dWarnTolerance = 0.2;
			tTol.OffsetList.clear();
			for (int i = 0; i < nSideCount; i++)
			{
				tTol.OffsetList.push_back(0);
			}
		}
		tPdInfo.tParam.vecMisalignment.resize(nCount, tTol);
	}

	//测量线数量
	if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
		tPdInfo.nMeasureLineNum = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 4 + 1) + 1;
	else
		tPdInfo.nMeasureLineNum = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 2 + 1) + 1;

	RefreshProductParam(tPdInfo);
}

tagPDInfo CPageProductSet::GetProductParam()
{
	ProductList vecList = theGlobal.m_ProductTool.GetProductList();
	if (vecList.size() <= 0)
		return tagPDInfo();
	if (m_nProductSel >= vecList.size())
		return tagPDInfo();
	tagPDInfo tPdInfo = vecList[m_nProductSel];

	int nSideCount = theGlobal.m_Project.GetProjectConfig().nSideCount;
	int nCount = 0;

	CString strValue;
	//参数规格
	CBCGPGridRow* pRow;
	//产品名称
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	tPdInfo.strPDName = pRow->GetItem(1)->GetValue();
	//产品备注
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	tPdInfo.strNote = pRow->GetItem(1)->GetValue();
	//产品类型
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "一出三")
		tPdInfo.nPDType = ThreeCavity;
	else if (strValue == "一出二")
		tPdInfo.nPDType = TwoCavity;
	else if (strValue == "一出四")
		tPdInfo.nPDType = FourCavity;
	else if (strValue == "一出六")
		tPdInfo.nPDType = SixCavity;
	else if (strValue == "一出八")
		tPdInfo.nPDType = EightCavity;
	else
		tPdInfo.nPDType = TenCavity;
	//有无AT11/凹版
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "有AT11")
	{
		tPdInfo.bExistAT11 = TRUE;
		tPdInfo.bExistBaseCoat = FALSE;
	}
	else if (strValue == "有凹版")
	{
		tPdInfo.bExistAT11 = FALSE;
		tPdInfo.bExistBaseCoat = TRUE;
	}
	else
	{
		tPdInfo.bExistAT11 = FALSE;
		tPdInfo.bExistBaseCoat = FALSE;
	}
	//有无间歇
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "有间歇")
		tPdInfo.bExistIntermittent = TRUE;
	else
		tPdInfo.bExistIntermittent = FALSE;
	//涂布类型
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	strValue = pRow->GetItem(1)->GetValue();
	if (strValue == "单面")
		tPdInfo.nCoatingType = OneSide;
	else
		tPdInfo.nCoatingType = TwoSide;
	nCount++;

#pragma region 修正涂布AT11测量线等数量
	//涂布数量
	int nNum = 0;
	nNum = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType);
	tPdInfo.tParam.vecCoatWidth.resize(nNum, tPdInfo.tParam.vecCoatWidth[0]);
	//极耳数量
	nNum = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType + 1);
	tPdInfo.tParam.vecFoilWidth.resize(nNum, tPdInfo.tParam.vecFoilWidth[0]);

	if (tPdInfo.bExistAT11)
	{
		//AT11数量
		nNum = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType * 2);
		tagTolerance tTol;
		if (tPdInfo.tParam.vecAT11Width.size() > 0)
		{
			tTol = tPdInfo.tParam.vecAT11Width[0];
		}
		else
		{
			tTol.dStandard = 20;
			tTol.dTolerance = 0.1;
			tTol.dWarnTolerance = 0.07;
			tTol.OffsetList.clear();
			for (int i = 0; i < nSideCount; i++)
			{
				tTol.OffsetList.push_back(0);
			}
		}
		tPdInfo.tParam.vecAT11Width.resize(nNum, tTol);
	}
	else if (tPdInfo.bExistBaseCoat)
	{
		//凹版数量
		nNum = (tPdInfo.nPDType == ThreeCavity ? 2 : tPdInfo.nPDType * 2);
		tagTolerance tTol;
		if (tPdInfo.tParam.vecBaseCoatWidth.size() > 0)
		{
			tTol = tPdInfo.tParam.vecBaseCoatWidth[0];
		}
		else
		{
			tTol.dStandard = 20;
			tTol.dTolerance = 0.1;
			tTol.dWarnTolerance = 0.07;
			tTol.OffsetList.clear();
			for (int i = 0; i < nSideCount; i++)
			{
				tTol.OffsetList.push_back(0);
			}
		}
		tPdInfo.tParam.vecBaseCoatWidth.resize(nNum, tTol);
	}

	//A/B面错位值
	if (tPdInfo.nCoatingType == TwoSide)
	{
		if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
			nNum = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 4 + 1);
		else
			nNum = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 2 + 1);

		tagTolerance tTol;
		if (tPdInfo.tParam.vecMisalignment.size() > 0)
		{
			tTol = tPdInfo.tParam.vecMisalignment[0];
		}
		else
		{
			tTol.dStandard = 0;
			tTol.dTolerance = 0.1;
			tTol.dWarnTolerance = 0.2;
			tTol.OffsetList.clear();
			for (int i = 0; i < nSideCount; i++)
			{
				tTol.OffsetList.push_back(0);
			}
		}
		tPdInfo.tParam.vecMisalignment.resize(nNum, tTol);
	}

	//测量线数量
	if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
		tPdInfo.nMeasureLineNum = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 4 + 1) + 1;
	else
		tPdInfo.nMeasureLineNum = (tPdInfo.nPDType == ThreeCavity ? 7 : tPdInfo.nPDType * 2 + 1) + 1;
#pragma endregion

	//涂布宽
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	strValue = pRow->GetItem(1)->GetValue();
	tPdInfo.tParam.bUseCoatWidth = strValue == "启用" ? TRUE : FALSE;
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		tPdInfo.tParam.vecCoatWidth[i].dStandard = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		tPdInfo.tParam.vecCoatWidth[i].dTolerance = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		tPdInfo.tParam.vecCoatWidth[i].dWarnTolerance = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		tPdInfo.tParam.vecCoatWidth[i].OffsetList[0] = pRow->GetItem(2 + i)->GetValue();
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			tPdInfo.tParam.vecCoatWidth[i].OffsetList[1] = pRow->GetItem(2 + i)->GetValue();
		}
	}
	nCount++;
	//极耳宽
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	strValue = pRow->GetItem(1)->GetValue();
	tPdInfo.tParam.bUseFoilWidth = strValue == "启用" ? TRUE : FALSE;
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		tPdInfo.tParam.vecFoilWidth[i].dStandard = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		tPdInfo.tParam.vecFoilWidth[i].dTolerance = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		tPdInfo.tParam.vecFoilWidth[i].OffsetList[0] = pRow->GetItem(2 + i)->GetValue();
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			tPdInfo.tParam.vecFoilWidth[i].OffsetList[1] = pRow->GetItem(2 + i)->GetValue();
		}
	}
	nCount++;
	//AT11/凹版宽
	if (tPdInfo.bExistAT11)
	{
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		strValue = pRow->GetItem(1)->GetValue();
		tPdInfo.tParam.bUseAT11Width = strValue == "启用" ? TRUE : FALSE;
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			tPdInfo.tParam.vecAT11Width[i].dStandard = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			tPdInfo.tParam.vecAT11Width[i].dTolerance = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			tPdInfo.tParam.vecAT11Width[i].dWarnTolerance = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			tPdInfo.tParam.vecAT11Width[i].OffsetList[0] = pRow->GetItem(2 + i)->GetValue();
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			pRow = m_Grid_ProductParam.GetRow(nCount++);
			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				tPdInfo.tParam.vecAT11Width[i].OffsetList[1] = pRow->GetItem(2 + i)->GetValue();
			}
		}
	}
	else if (tPdInfo.bExistBaseCoat)
	{
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		strValue = pRow->GetItem(1)->GetValue();
		tPdInfo.tParam.bUseBaseCoatWidth = strValue == "启用" ? TRUE : FALSE;
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			tPdInfo.tParam.vecBaseCoatWidth[i].dStandard = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			tPdInfo.tParam.vecBaseCoatWidth[i].dTolerance = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			tPdInfo.tParam.vecBaseCoatWidth[i].dWarnTolerance = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[0] = pRow->GetItem(2 + i)->GetValue();
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			pRow = m_Grid_ProductParam.GetRow(nCount++);
			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[1] = pRow->GetItem(2 + i)->GetValue();
			}
		}
	}
	nCount++;

	//错位值
	if (tPdInfo.nCoatingType == TwoSide && theGlobal.m_Project.GetProjectConfig().nSideCount > 1)
	{
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		strValue = pRow->GetItem(1)->GetValue();
		tPdInfo.tParam.bUseMisalignment = strValue == "启用" ? TRUE : FALSE;
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			tPdInfo.tParam.vecMisalignment[i].dStandard = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			tPdInfo.tParam.vecMisalignment[i].dTolerance = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			tPdInfo.tParam.vecMisalignment[i].dWarnTolerance = pRow->GetItem(2 + i)->GetValue();
		}
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			tPdInfo.tParam.vecMisalignment[i].OffsetList[0] = pRow->GetItem(2 + i)->GetValue();
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			pRow = m_Grid_ProductParam.GetRow(nCount++);
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				tPdInfo.tParam.vecMisalignment[i].OffsetList[1] = pRow->GetItem(2 + i)->GetValue();
			}
		}
	}
	nCount++;

	//点检
	pRow = m_Grid_ProductParam.GetRow(23);
	strValue = pRow->GetItem(1)->GetValue();
	tPdInfo.tParam.bUseChecking = strValue == "启用" ? TRUE : FALSE;
	for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
	{
		tPdInfo.tParam.vecChecking[i].dStandard = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
	{
		tPdInfo.tParam.vecChecking[i].dTolerance = pRow->GetItem(2 + i)->GetValue();
	}
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
	{
		tPdInfo.tParam.vecChecking[i].OffsetList[0] = pRow->GetItem(2 + i)->GetValue();
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		pRow = m_Grid_ProductParam.GetRow(nCount++);
		for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
		{
			tPdInfo.tParam.vecChecking[i].OffsetList[1] = pRow->GetItem(2 + i)->GetValue();
		}
	}
	nCount++;

	//表面瑕疵
	pRow = m_Grid_ProductParam.GetRow(nCount++);
	strValue = pRow->GetItem(1)->GetValue();
	tPdInfo.tParam.bUseSurfaceDetect = strValue == "启用" ? TRUE : FALSE;

	return tPdInfo;
}

void CPageProductSet::RefreshProductParam(tagPDInfo& tPdInfo)
{
	int nCount = 0;
	if (tPdInfo.nCoatingType == TwoSide)
	{
		nCount = tPdInfo.tParam.vecMisalignment.size();
	}
	else
	{
		if (tPdInfo.bExistAT11 == TRUE)
			nCount = tPdInfo.tParam.vecAT11Width.size();
		else if (tPdInfo.bExistBaseCoat == TRUE)
			nCount = tPdInfo.tParam.vecBaseCoatWidth.size();
		else
			nCount = tPdInfo.tParam.vecFoilWidth.size();
	}

	CString strValue;
	//参数规格
	CBCGPGridRow* pRow;
	m_Grid_ProductParam.RemoveAll();
	m_Grid_ProductParam.DeleteAllColumns();
	m_Grid_ProductParam.InsertColumn(0, "产品信息", 130);
	m_Grid_ProductParam.InsertColumn(1, "设置", 70);
	for (int i = 0; i < nCount; i++)
	{
		strValue.Format("规格%d", i + 1);
		m_Grid_ProductParam.InsertColumn(i + 2, strValue, 50);
	}
	//产品名称
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("产品名称"), FALSE);
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.strPDName), FALSE);
	pRow->GetItem(1)->AllowEdit(!m_nProductSel == m_nCurrentProductSel);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	//产品备注
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("产品备注"), FALSE);
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.strNote), FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	//产品类型
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("产品类型"), FALSE);
	pRow->GetItem(1)->AddOption("一出三");
	pRow->GetItem(1)->AddOption("一出二");
	pRow->GetItem(1)->AddOption("一出四");
	pRow->GetItem(1)->AddOption("一出六");
	pRow->GetItem(1)->AddOption("一出八");
	pRow->GetItem(1)->AddOption("一出十");
	if (tPdInfo.nPDType == ThreeCavity)
		strValue = "一出三";
	else if (tPdInfo.nPDType == TwoCavity)
		strValue = "一出二";
	else if (tPdInfo.nPDType == FourCavity)
		strValue = "一出四";
	else if (tPdInfo.nPDType == SixCavity)
		strValue = "一出六";
	else if (tPdInfo.nPDType == EightCavity)
		strValue = "一出八";
	else
		strValue = "一出十";
	pRow->GetItem(1)->SetValue(_variant_t(strValue), FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	//有无AT11/凹版
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("AT11/凹版"), FALSE);
	pRow->GetItem(1)->AddOption("无");
	pRow->GetItem(1)->AddOption("有AT11");
	pRow->GetItem(1)->AddOption("有凹版");
	if (tPdInfo.bExistAT11)
		strValue = "有AT11";
	else if (tPdInfo.bExistBaseCoat)
		strValue = "有凹版";
	else
		strValue = "无";
	pRow->GetItem(1)->SetValue(_variant_t(strValue), FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	//有无间歇
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("有无间歇"), FALSE);
	pRow->GetItem(1)->AddOption("无");
	pRow->GetItem(1)->AddOption("有间歇");
	if (tPdInfo.bExistIntermittent)
		strValue = "有间歇";
	else
		strValue = "无";
	pRow->GetItem(1)->SetValue(_variant_t(strValue), FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	//涂布类型
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("涂布类型"), FALSE);
	pRow->GetItem(1)->AddOption("单面");
	pRow->GetItem(1)->AddOption("双面");
	if (tPdInfo.nCoatingType == OneSide)
		strValue = "单面";
	else
		strValue = "双面";
	pRow->GetItem(1)->SetValue(_variant_t(strValue), FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->Enable(FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);

	//涂布宽
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'涂布'宽标准值(mm)"), FALSE);
	pRow->GetItem(1)->AddOption("不启用");
	pRow->GetItem(1)->AddOption("启用");
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseCoatWidth ? "启用" : "不启用"), FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecCoatWidth[i].dStandard), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'涂布'宽公差(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecCoatWidth[i].dTolerance), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'涂布'宽预警公差(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecCoatWidth[i].dWarnTolerance), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'涂布'宽A面补偿(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecCoatWidth[i].OffsetList[0]), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	if (tPdInfo.nCoatingType == TwoSide)
	{
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'涂布'宽B面补偿(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecCoatWidth[i].OffsetList[1]), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
	}
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->Enable(FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	//极耳宽
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'极耳'宽标准值(mm)"), FALSE);
	pRow->GetItem(1)->AddOption("不启用");
	pRow->GetItem(1)->AddOption("启用");
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseFoilWidth ? "启用" : "不启用"), FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecFoilWidth[i].dStandard), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'极耳'宽公差(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecFoilWidth[i].dTolerance), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'极耳'宽预警公差(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'极耳'宽A面补偿(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecFoilWidth[i].OffsetList[0]), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	if (tPdInfo.nCoatingType == TwoSide)
	{
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'极耳'宽B面补偿(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecFoilWidth[i].OffsetList[1]), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
	}
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->Enable(FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	//AT11/凹版宽
	if (tPdInfo.bExistAT11)
	{
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'AT11'宽标准值(mm)"), FALSE);
		pRow->GetItem(1)->AddOption("不启用");
		pRow->GetItem(1)->AddOption("启用");
		pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseAT11Width ? "启用" : "不启用"), FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecAT11Width[i].dStandard), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'AT11'宽公差(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecAT11Width[i].dTolerance), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'AT11'宽预警公差(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecAT11Width[i].dWarnTolerance), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'AT11'宽A面补偿(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecAT11Width[i].OffsetList[0]), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		if (tPdInfo.nCoatingType == TwoSide)
		{
			pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
			pRow->GetItem(0)->SetValue(_variant_t("'AT11'宽B面补偿(mm)"), FALSE);
			pRow->GetItem(1)->AllowEdit(FALSE);
			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecAT11Width[i].OffsetList[1]), FALSE);
			}
			m_Grid_ProductParam.AddRow(pRow, FALSE);
		}
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->Enable(FALSE);
		m_Grid_ProductParam.AddRow(pRow, FALSE);
	}
	else if (tPdInfo.bExistBaseCoat)
	{
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'凹版'宽标准值(mm)"), FALSE);
		pRow->GetItem(1)->AddOption("不启用");
		pRow->GetItem(1)->AddOption("启用");
		pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseBaseCoatWidth ? "启用" : "不启用"), FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecBaseCoatWidth[i].dStandard), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'凹版'宽公差(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecBaseCoatWidth[i].dTolerance), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'凹版'宽预警公差(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecBaseCoatWidth[i].dWarnTolerance), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'凹版'宽A面补偿(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[0]), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		if (tPdInfo.nCoatingType == TwoSide)
		{
			pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
			pRow->GetItem(0)->SetValue(_variant_t("'凹版'宽B面补偿(mm)"), FALSE);
			pRow->GetItem(1)->AllowEdit(FALSE);
			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[1]), FALSE);
			}
			m_Grid_ProductParam.AddRow(pRow, FALSE);
		}
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->Enable(FALSE);
		m_Grid_ProductParam.AddRow(pRow, FALSE);
	}

	//错位值
	if (tPdInfo.nCoatingType == TwoSide && theGlobal.m_Project.GetProjectConfig().nSideCount > 1)
	{
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'错位'标准值(mm)"), FALSE);
		pRow->GetItem(1)->AddOption("不启用");
		pRow->GetItem(1)->AddOption("启用");
		pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseMisalignment ? "启用" : "不启用"), FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecMisalignment[i].dStandard), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'错位'公差(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecMisalignment[i].dTolerance), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'错位'预警公差(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecMisalignment[i].dWarnTolerance), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'错位'宽A面补偿(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecMisalignment[i].OffsetList[0]), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
		if (tPdInfo.nCoatingType == TwoSide)
		{
			pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
			pRow->GetItem(0)->SetValue(_variant_t("'错位'宽B面补偿(mm)"), FALSE);
			pRow->GetItem(1)->AllowEdit(FALSE);
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecMisalignment[i].OffsetList[1]), FALSE);
			}
			m_Grid_ProductParam.AddRow(pRow, FALSE);
		}
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->Enable(FALSE);
		m_Grid_ProductParam.AddRow(pRow, FALSE);
	}

	//点检
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'点检'宽标准值(mm)"), FALSE);
	pRow->GetItem(1)->AddOption("不启用");
	pRow->GetItem(1)->AddOption("启用");
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseChecking ? "启用" : "不启用"), FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[i].dStandard), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'点检'宽公差(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[i].dTolerance), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("'点检'宽A面补偿(mm)"), FALSE);
	pRow->GetItem(1)->AllowEdit(FALSE);
	for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
	{
		pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[i].OffsetList[0]), FALSE);
	}
	m_Grid_ProductParam.AddRow(pRow, FALSE);
	if (tPdInfo.nCoatingType == TwoSide)
	{
		pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("'点检'宽B面补偿(mm)"), FALSE);
		pRow->GetItem(1)->AllowEdit(FALSE);
		for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
		{
			pRow->GetItem(2 + i)->SetValue(_variant_t(tPdInfo.tParam.vecChecking[i].OffsetList[1]), FALSE);
		}
		m_Grid_ProductParam.AddRow(pRow, FALSE);
	}
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->Enable(FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);

	//表面瑕疵
	pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("表面瑕疵"), FALSE);
	pRow->GetItem(1)->AddOption("不启用");
	pRow->GetItem(1)->AddOption("启用");
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.tParam.bUseSurfaceDetect ? "启用" : "不启用"), FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);

	//测试
	/*pRow = m_Grid_ProductParam.CreateRow(m_Grid_ProductParam.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("测试"), FALSE);
	pRow->GetItem(1)->AddOption("启用");
	pRow->GetItem(1)->AddOption("不启用");
	pRow->GetItem(1)->SetValue(_variant_t(tPdInfo.bExistAT11 ? "启用" : "不启用"), FALSE);
	pRow->GetItem(2)->AddOption("启用");
	pRow->GetItem(2)->AddOption("不启用");
	pRow->GetItem(2)->SetValue(_variant_t(tPdInfo.bExistAT11 ? "启用" : "不启用"), FALSE);
	m_Grid_ProductParam.AddRow(pRow, FALSE);*/

	m_Grid_ProductParam.AdjustLayout();
}

void CPageProductSet::RefreshProductParam()
{
	ProductList vecList = theGlobal.m_ProductTool.GetProductList();
	if (vecList.size() <= 0)
		return;
	if (m_nProductSel >= vecList.size())
		return;
	tagPDInfo tPdInfo = vecList[m_nProductSel];

	RefreshProductParam(tPdInfo);
}

void CPageProductSet::OnBnClickedButtonProductNew()
{
	if (-1!=theGlobal.m_ProductTool.FindName("New"))
	{
		theGlobal.m_Language.MessageboxError("请先将之前新建的产品改名。", "Please rename the newly created product first.", "");
		return;
	}
	theGlobal.m_ProductTool.AddNew("New");
	RefeshProductList();
}


void CPageProductSet::OnBnClickedButtonProductLoad()
{
	CList<CBCGPGridItem*,CBCGPGridItem*> sel;
	m_Grid_ProductList.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0>=nCount || nCount>2)
	{
		theGlobal.m_Language.MessageboxError("请选择产品列表中的一个产品。", "Please select a product in the list.", "");
		return;
	}

	CBCGPGridItem* pItem = sel.GetHead();
	CBCGPGridRow* pRow = pItem->GetParentRow();
	int nRow = pRow->GetRowId();
	if (nRow==m_nCurrentProductSel)
	{
		theGlobal.m_Language.MessageboxError("不能加载当前正在使用的产品。", "Cannot load the product currently in use.", "");
		return;
	}

	theGlobal.m_ProductTool.Load(nRow);
	theGlobal.m_Language.Messagebox("加载产品成功，点击确定将重启。", "Successfully load the product, click OK to restart.", "");
	theGlobal.PostMessageToMainDlg(WM_RESTARTPROCESS);
}

void CPageProductSet::OnBnClickedButtonProductDelete()
{
	CList<CBCGPGridItem*,CBCGPGridItem*> sel;
	m_Grid_ProductList.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0>=nCount || nCount>2)
	{
		theGlobal.m_Language.MessageboxError("请选择产品列表中的一个产品。", "Please select a product in the list.", "");
		return;
	}

	CBCGPGridItem* pItem = sel.GetHead();
	CBCGPGridRow* pRow = pItem->GetParentRow();
	int nRow = pRow->GetRowId();
	if (nRow==m_nCurrentProductSel)
	{
		theGlobal.m_Language.MessageboxError("不能删除当前正在使用的产品。", "Cannot delete the product currently in use.", "");
		return;
	}

	theGlobal.m_ProductTool.Delete(nRow);
	RefeshProductList();
}

LRESULT CPageProductSet::OnEndEditGridProduct(WPARAM wParam, LPARAM lParam)
{
	//RenameProductList();
	if (m_Grid_ProductParam.IsFocused())
	{
		if (m_nGridOldRowSN < 6 && m_nGridOldColSN == 1)
		{
			EditRefreshProductParam();
		}
		
	}
	return 0L;
}

LRESULT CPageProductSet::OnBeginEditGridProduct(WPARAM wParam, LPARAM lParam)
{
	if (m_Grid_ProductParam.IsFocused())
	{
		CList<CBCGPGridItem*, CBCGPGridItem*> sel;
		m_Grid_ProductParam.GetSelectedItems(sel);
		int nCount = sel.GetCount();
		if (0 >= nCount || nCount > 2)
		{
			return 0L;
		}

		m_nGridOldRowSN = m_nGridRowSN;
		m_nGridOldColSN = m_nGridColSN;
		CBCGPGridItemID tID = m_Grid_ProductParam.GetCurSelItemID();
		m_nGridColSN = tID.m_nColumn;
		m_nGridRowSN = tID.m_nRow;
	}
	else
	{
		m_nGridRowSN = -1;
		m_nGridColSN = -1;
	}
	return 0L;
}

LRESULT CPageProductSet::OnItemChangedGridProduct(WPARAM wParam, LPARAM lParam)
{
	if (!m_Grid_ProductList.IsFocused()) return 0L;

	static int s_nSN = 0;

	CList<CBCGPGridItem*,CBCGPGridItem*> sel;
	m_Grid_ProductList.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0>=nCount || nCount>2)
	{
		return 0L;
	}

	CBCGPGridItem* pItem = sel.GetHead();
	CBCGPGridRow* pRow = pItem->GetParentRow();
	m_nProductSel = pRow->GetRowId();
	if (m_nProductSel == s_nSN) return 0L;
	s_nSN = m_nProductSel;
	RefreshProductParam();
	return 0L;
}


BOOL CPageProductSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageProductSet::OnBnClickedButtonProductSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("是否保存配方参数?", "Whether to save product params?", ""))
		return;

	tagPDInfo tPdInfo = GetProductParam();

	ProductList vecList = theGlobal.m_ProductTool.GetProductList();
	if (vecList.size() <= 0)
	{
		theGlobal.m_Language.MessageboxError("产品列表为空，请检查!","","");
		return;
	}
	if (m_nProductSel >= vecList.size())
	{
		theGlobal.m_Language.MessageboxError("当前所选产品超出产品列表范围，请检查!","","");
			return;
	}
	//判断非法字符
	if (BaseFunction::CheckIllageChar(tPdInfo.strPDName) == TRUE)
	{
		theGlobal.m_Language.MessageboxError("当前名字包含非法字符，请重新修改。", "Current product name includes illegal character, please rename again.", "");
		return;
	}
	for (int i = 0; i < vecList.size(); i++)
	{
		if (vecList[i].strPDName == tPdInfo.strPDName && i != m_nProductSel)
		{
			theGlobal.m_Language.MessageboxError("当前名字和已存在的产品名称重复，请重新修改。", "Current product in use, The current name and the existing product name are duplicated, please rename again.", "");

			return;
		}
	}

	//重命名
	if (vecList[m_nProductSel].strPDName != tPdInfo.strPDName)
	{
		if (m_nProductSel == m_nCurrentProductSel)
			theGlobal.m_ProductTool.Rename(m_nProductSel, vecList[m_nCurrentProductSel].strPDName, tPdInfo.strNote);
		else
			theGlobal.m_ProductTool.Rename(m_nProductSel, tPdInfo.strPDName, tPdInfo.strNote);
	}

	theGlobal.m_ProductTool.SetProductInfo(m_nProductSel, tPdInfo);
	theGlobal.m_ProductTool.WriteProductParam(m_nProductSel);

	//给算法更新产品信息
	for (int nSideSN = 0; nSideSN < theGlobal.m_Project.GetProjectConfig().nSideCount; nSideSN++)
	{
		theGlobal.m_AlgTool.DoSomething(nSideSN, CAlgorithmTool::AlgMeasure, "SetProductInfo", &tPdInfo, NULL);
	}

	RefeshProductList();

	if (m_nProductSel == m_nCurrentProductSel)
	{
		if (vecList[m_nProductSel].nCoatingType != tPdInfo.nCoatingType
			|| vecList[m_nProductSel].bExistAT11 != tPdInfo.bExistAT11
			|| vecList[m_nProductSel].bExistBaseCoat != tPdInfo.bExistBaseCoat)
		{
			//theGlobal.m_pSubDispList->PostMessageA(WM_REFRESH_DISP);
		}
		if (vecList[m_nProductSel].nCoatingType != tPdInfo.nCoatingType
			|| vecList[m_nProductSel].bExistAT11 != tPdInfo.bExistAT11
			|| vecList[m_nProductSel].bExistBaseCoat != tPdInfo.bExistBaseCoat)
		{
			theGlobal.m_Language.Messagebox("保存完成，产品类型改变，程序将重启。", "Product type changed, program will restart.", "");
			theGlobal.PostMessageToMainDlg(WM_RESTARTPROCESS);
		}
		//如果参数范围改变，也重新显示下折线图
		//theGlobal.m_pSubDispScope->PostMessageA(WM_REFRESH_DISP);
		//theGlobal.m_pMainDlg->PostMessageA(WM_REFRESH_DISP);
	}
	else
	{
		theGlobal.m_Language.Messagebox("保存完成", "Product type changed.", "");
	}
}
