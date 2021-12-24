// DlgParam.cpp : 实现文件
//

#include "stdafx.h"
#include "SCISeparate.h"
#include "DlgParam.h"
#include "afxdialogex.h"
#include "Product.in.h"

// CDlgParam 对话框

int WM_TEST = RegisterWindowMessage(_T("WM_TEST"));

IMPLEMENT_DYNAMIC(CDlgParam, CBCGPDialog)

CDlgParam::CDlgParam(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_PARAM, pParent)
	, m_COM_bAT9(0)
	, m_COM_nProType(0)
	, m_CHECK_bShowAdaptedImage(FALSE)
{
	m_pTemp = new CMeasure;

	m_ftButton.CreatePointFont(100, "Microsoft YaHei");
	m_nSearchLineSN = 0;
	m_bDrawLineHor = FALSE;
	m_bConfirmMeasure = FALSE;
}

CDlgParam::~CDlgParam()
{
	m_ftButton.DeleteObject();
}

void CDlgParam::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_AT9, m_COM_bAT9);
	DDX_CBIndex(pDX, IDC_COMBO_PROTYPE, m_COM_nProType);
	DDX_Control(pDX, IDC_TAB_LIST, m_TAB_List);
	DDX_Check(pDX, IDC_CHECK_SHOW_ADAPTED_IMAGE, m_CHECK_bShowAdaptedImage);
}


BEGIN_MESSAGE_MAP(CDlgParam, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDlgParam::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_TESTALL, &CDlgParam::OnBnClickedButtonTestall)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgParam::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SAVEIMAGE, &CDlgParam::OnBnClickedButtonSaveimage)
	ON_BN_CLICKED(IDC_BUTTON_OPENIMAGE, &CDlgParam::OnBnClickedButtonOpenimage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_LIST, &CDlgParam::OnTcnSelchangeTabList)
	ON_BN_CLICKED(IDC_CHECK_SHOW_ADAPTED_IMAGE, &CDlgParam::OnBnClickedCheckShowAdaptedImage)
	ON_BN_CLICKED(IDC_BUTTON_DISP_NEWIMAGE, &CDlgParam::OnBnClickedButtonDispNewimage)
END_MESSAGE_MAP()


// CDlgParam 消息处理程序

void CDlgParam::InitWindow()
{
	CRect rcClient;
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(rcClient);
	ScreenToClient(rcClient);
	m_Wnd_Image.Create("", GetDlgItem(IDC_STATIC_IMAGE)->GetStyle(),
		rcClient, this, IDC_STATIC_IMAGE);
	GetDlgItem(IDC_STATIC_IMAGE)->ShowWindow(SW_HIDE);
	SetDraw(m_Wnd_Image.GetDispWnd(), "margin");


	//这里要对图像做合成
	HTuple Width, Height;
	if (m_pTemp->m_tInit.hvCamSNList.Length() != m_tInput.hoGrabImage.CountObj())
	{
		//BCGPMessageBox("每面的相机个数与传入的图像数量不一致");
	}
	if (m_tInput.hoGrabImage.CountObj() > 1)
	{
		TileImages(m_tInput.hoGrabImage, &m_tInput.hoGrabImage, m_tInput.hoGrabImage.CountObj(), "horizontal");
	}
	GetImageSize(m_tInput.hoGrabImage, &Width, &Height);

	m_Wnd_Image.DispImage(m_tInput.hoGrabImage);
}


BOOL CDlgParam::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	FullScreenTaskbarAdaptive(GetSafeHwnd());

	// TODO:  在此添加额外的初始化
	InitUI();

	if (m_bConfirmMeasure)
	{
		OnBnClickedButtonTestall();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgParam::InitUI()
{
	InitWindow();

	ePOWERLIMITMODULE eP = PM_Detect;
	BOOL bEnable = m_pTemp->m_tInit.pMain((eModuleType)-1, "CheckPermission", "", &eP, NULL);

	m_COM_nProType = m_pTemp->m_tInit.tProductInfo.nPDType;
	if (m_pTemp->m_tInit.tProductInfo.bExistAT11)
		m_COM_bAT9 = 1;
	else if (m_pTemp->m_tInit.tProductInfo.bExistBaseCoat)
		m_COM_bAT9 = 2;
	else
		m_COM_bAT9 = 0;

	NameList vecList = m_pTemp->m_tInit.tProductInfo.vecNameList;

	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_LIST)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_listInfo.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_listInfo, theme);
	m_listInfo.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_listInfo.SetColorTheme(theme);
	m_listInfo.SetReadOnly();//可编辑
	m_listInfo.EnableMarkSortedColumn(FALSE);//使用列排序
	m_listInfo.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_listInfo.SetWholeRowSel(TRUE);
	m_listInfo.EnableRowHeader(TRUE);//使用行表头
	m_listInfo.EnableLineNumbers(TRUE);//使用行序号
	m_listInfo.SetClearInplaceEditOnEnter(FALSE);
	m_listInfo.EnableInvertSelOnCtrl(FALSE);
	m_listInfo.InsertColumn(0, "项目", 120);
	for (int i = 0; i < vecList.size(); i++)
	{
		m_listInfo.InsertColumn(i + 1, vecList[i], 80);
	}

	m_listInfo.LoadState(_T("BasicGrid"));
	m_listInfo.AdjustLayout();//相当于数据显示在表格上
	m_listInfo.ShowWindow(SW_NORMAL);
	m_listInfo.SetFont(&m_ftButton);

	UpdateData(FALSE);

	//权限不够禁用部分控件
	/*if (!bEnable)
	{
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	}*/

	InitTab();

	GenSearchRegion();

	if (m_bConfirmMeasure)
	{
		::EnableMenuItem(::GetSystemMenu(this->m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_TESTALL)->SetWindowTextA("确认寻边");
	}
}

void CDlgParam::GenSearchRegion()
{
	GenEmptyObj(&m_hoSearchRegion);
	m_hvDispX.Clear();
	m_hvDispY.Clear();
	m_vecDispString.clear();
	for (int i = 0; i < m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam.size(); i++)
	{
		HObject hoTmp;
		double dStartRow = min(m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dStartRow, m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dEndRow);
		double dEndRow = max(m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dStartRow, m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dEndRow);
		double dStartCol = (m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dStartCol + m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dEndCol) / 2 - 25;
		double dEndCol = (m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dStartCol + m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dEndCol) / 2 + 25;
		if (m_tInput.hoGrabImage.CountObj() > 0)
		{
			HTuple hvHeight, hvWidth;
			GetImageSize(m_tInput.hoGrabImage, &hvWidth, &hvHeight);
			dStartRow = min(max(hvHeight.D() / 8, dStartRow), hvHeight.D() / 8 * 7);
			dEndRow = max(min(hvHeight.D() / 8 * 7, dEndRow), hvHeight.D() / 8);
			double dMin, dMax;
			dMin = min(dStartRow, dEndRow);
			dMax = max(dStartRow, dEndRow);
			dStartRow = dMin;
			dEndRow = dMax;
		}
		GenRectangle1(&hoTmp, dStartRow, dStartCol, dEndRow, dEndCol);

		ConcatObj(m_hoSearchRegion, hoTmp, &m_hoSearchRegion);

		m_hvDispX.Append((m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dStartCol + m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[i].dEndCol) / 2 - 50);
		if (i % 2 == 0)
			m_hvDispY.Append(dStartRow - 25);
		else
			m_hvDispY.Append(dEndRow + 25);
		CString strDispName;
		strDispName.Format("SN:%d", i);
		m_vecDispString.push_back(strDispName);
	}
}

void CDlgParam::InitTab()
{
	m_TAB_List.InsertItem(0, "测宽参数");
	m_TAB_List.InsertItem(1, "瑕疵参数");
	m_TAB_List.InsertItem(2, "标定参数");
	m_TAB_List.InsertItem(3, "点检参数");

	//m_TAB_List.SetColor(RGB(58, 58, 63), RGB(80, 80, 85), RGB(80, 80, 85), RGB(80, 80, 85), RGB(200, 200, 200), RGB(255, 255, 255));
	m_TAB_List.SetColor(RGB(48, 48, 48), RGB(87, 87, 87), RGB(87, 87, 87), RGB(87, 87, 87), RGB(200, 200, 200), RGB(255, 255, 255));
	m_TAB_List.SetTextFont("Microsoft Yahei", 19, 0);
	m_TAB_List.SetItemSize(CSize(120, 30));
	m_TAB_List.SetMinTabWidth(100);

	CRect rcSubDlg, rcTabItem;
	GetDlgItem(IDC_TAB_LIST)->GetWindowRect(rcSubDlg);

	m_TAB_List.GetItemRect(0, rcTabItem);
	rcSubDlg.top += 10;
	rcSubDlg.left += 2;
	rcSubDlg.right -= 2;
	rcSubDlg.bottom -= 25;

	m_pPageMeasureSet = new CPageMeasureSet;
	m_pPageMeasureSet->m_pOwners = m_pTemp;
	m_pPageMeasureSet->m_pParentDlg = this;
	m_pPageMeasureSet->Create(CPageMeasureSet::IDD, this);
	m_pPageMeasureSet->MoveWindow(rcSubDlg);
	m_pPageMeasureSet->ShowWindow(SW_NORMAL);
	m_vecPages.push_back(m_pPageMeasureSet);

	m_pPageDefectSet = new CPageDefectSet;
	m_pPageDefectSet->m_pOwners = m_pTemp;
	m_pPageDefectSet->m_pParentDlg = this;
	m_pPageDefectSet->Create(CPageDefectSet::IDD, this);
	m_pPageDefectSet->MoveWindow(rcSubDlg);
	m_pPageDefectSet->ShowWindow(SW_HIDE);
	m_vecPages.push_back(m_pPageDefectSet);

	m_pPageCalibrationSet = new CPageCalibrationSet;
	m_pPageCalibrationSet->m_pOwners = m_pTemp;
	m_pPageCalibrationSet->m_pParentDlg = this;
	m_pPageCalibrationSet->Create(CPageCalibrationSet::IDD, this);
	m_pPageCalibrationSet->MoveWindow(rcSubDlg);
	m_pPageCalibrationSet->ShowWindow(SW_HIDE);
	m_vecPages.push_back(m_pPageCalibrationSet);

	m_pPageSpotInspectSet = new CPageSpotInspectSet;
	m_pPageSpotInspectSet->m_pOwners = m_pTemp;
	m_pPageSpotInspectSet->m_pParentDlg = this;
	m_pPageSpotInspectSet->Create(CPageSpotInspectSet::IDD, this);
	m_pPageSpotInspectSet->MoveWindow(rcSubDlg);
	m_pPageSpotInspectSet->ShowWindow(SW_HIDE);
	m_vecPages.push_back(m_pPageSpotInspectSet);

	//SwitchDispPage(MeasureSet);
}

void CDlgParam::MoveSearchReion()
{
	if (m_hoSearchRegion.CountObj() == m_pTemp->m_tInit.tProductInfo.nMeasureLineNum && m_nSearchLineSN >= 0)
	{
		m_Wnd_Image.SetDrawing(TRUE);
		m_Wnd_Image.DispImage(FALSE);
		m_Wnd_Image.ClearMessage();

		HObject hoSearchRegion = m_hoSearchRegion.SelectObj(m_nSearchLineSN + 1);
		HTuple hvRow1, hvCol1, hvRow2, hvCol2;
		SmallestRectangle1(hoSearchRegion, &hvRow1, &hvCol1, &hvRow2, &hvCol2);

		CString strMsg;
		strMsg.Format("测量SN%d, 拖动红色区域移动搜索线, 点击右键完成", m_nSearchLineSN);
		m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");
		SetColor(m_Wnd_Image.GetDispWnd(), "green");
		DrawRectangle1Mod(m_Wnd_Image.GetDispWnd(), hvRow1, hvCol1, hvRow2, hvCol2, &hvRow1, &hvCol1, &hvRow2, &hvCol2);
		if (m_nSearchLineSN < m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam.size())
		{
			m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dStartCol = (hvCol1.D() + hvCol2.D()) / 2;
			m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dEndCol = (hvCol1.D() + hvCol2.D()) / 2;
			if (m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].nDirection == CLineMeasure::MD_Left2Right)
			{
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dStartRow = hvRow2.D();
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dEndRow = hvRow1.D();
			}
			else
			{
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dStartRow = hvRow1.D();
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dEndRow = hvRow2.D();
			}
		}

		DispSearchLine(TRUE);
		m_Wnd_Image.SetDrawing(FALSE);
	}
}

void CDlgParam::MoveSearchRegionStep(BOOL bLeft)
{
	if (m_hoSearchRegion.CountObj() == m_pTemp->m_tInit.tProductInfo.nMeasureLineNum)
	{
		m_Wnd_Image.DispImage(FALSE);
		m_Wnd_Image.ClearMessage();

		HObject hoSearchRegion = m_hoSearchRegion.SelectObj(m_nSearchLineSN + 1);
		HTuple hvRow1, hvCol1, hvRow2, hvCol2;
		SmallestRectangle1(hoSearchRegion, &hvRow1, &hvCol1, &hvRow2, &hvCol2);
		HTuple hvImageWidth, hvImageHeight;
		GetImageSize(m_Wnd_Image.GetDispImage(), &hvImageWidth, &hvImageHeight);

		hvCol1 += bLeft ? (-5) : (5);
		hvCol2 += bLeft ? (-5) : (5);
		hvCol1 = min(max(0, hvCol1), hvImageWidth.I() - 1);
		hvCol2 = min(max(0, hvCol2), hvImageWidth.I() - 1);
		if (m_nSearchLineSN < m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam.size())
		{
			m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dStartCol = (hvCol1.D() + hvCol2.D()) / 2;
			m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dEndCol = (hvCol1.D() + hvCol2.D()) / 2;
			if (m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].nDirection == CLineMeasure::MD_Left2Right)
			{
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dStartRow = hvRow2.D();
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dEndRow = hvRow1.D();
			}
			else
			{
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dStartRow = hvRow1.D();
				m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nSearchLineSN].dEndRow = hvRow2.D();
			}
		}

		DispSearchLine(TRUE);
	}
}

void CDlgParam::OnBnClickedButtonExit()
{
	CBCGPDialog::OnCancel();
}
BOOL CDlgParam::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE)//
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_LEFT)
		{
			m_pPageCalibrationSet->m_bFlag = FALSE;
			m_pPageCalibrationSet->m_hvGrayTuple.Clear();

			return TRUE;
		}
		else if (pMsg->wParam == VK_RIGHT)
		{
			m_pPageCalibrationSet->m_hvGrayTuple.Clear();
			m_pPageCalibrationSet->m_bFlag = TRUE;
			m_pPageCalibrationSet->m_hvGrayTuple.Append(0.7).Append(1).Append(1.5).Append(2);
		}
	}

	if (pMsg->message == WM_MOUSEMOVE)
	{
		try
		{
			HTuple Width, Height;
			GetImageSize(m_Wnd_Image.GetDispImage(), &Width, &Height);
			CString strDisp;
			//strDisp.Format("图片宽: %04d   图片高: %04d   极耳高: %03d   REMAIN: %d", Width.I(), Height.I(), m_pTemp->m_tParameters.nFoilHeight, REMAIN_LINS);
			//m_STC_ImageInfo.SetWindowTextA(strDisp);

			HTuple Row, Column, Value, Area, Circularity;
			GetMposition(m_Wnd_Image.GetDispWnd(), &Row, &Column, NULL);

			m_nMouseX = Column.I();
			m_nMouseY = Row.I();

			GetGrayval(m_Wnd_Image.GetDispImage(), Row, Column, &Value);

			HTuple hvCol;
			m_pTemp->m_Calibration.CalPoint(Column, hvCol);

			CString strRealX;
			if(hvCol != NO_DATA && hvCol != -1)
				strRealX.Format("%0.3f",hvCol.D());
			else
				strRealX = "-";

			strDisp.Format("宽: %d  高:% d  X: %d Y: %d   灰度值: %3d  物理坐标X: %s", Width.I(), Height.I(), Column.I(), Row.I(), Value.I(), strRealX);
			//m_STC_MouseInfo.SetWindowTextA(strDisp);

			m_Wnd_Image.DispMessage(strDisp, XDL_RightButtom);
			m_Wnd_Image.DispRegion();

			return TRUE;
		}
		catch (HException e)
		{
			//CString strDisp;
			//strDisp.Format("坐标:  X:-- Y:--\r\n \r\n颜色值: --");
			//			m_STC_GrayValue.SetWindowTextA(strDisp);
		}

	}
	if (pMsg->message == WM_RBUTTONUP)
	{

	}
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		GetLButtonLineSN(pMsg);
		m_pPageMeasureSet->DispSearchLineParam(m_nSearchLineSN);
		DispLineHor(pMsg);
	}
	if (pMsg->wParam == VK_F1)
	{
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CDlgParam::DispSearchLine(BOOL bDisp)
{
	GenSearchRegion();
	if (bDisp && m_hoSearchRegion.CountObj() == m_pTemp->m_tInit.tProductInfo.nMeasureLineNum)
	{
		m_Wnd_Image.DispImage(FALSE);
		m_Wnd_Image.ClearRegion();
		m_Wnd_Image.ClearMessage();

		HObject hoHover, hoUnhover;
		GenEmptyObj(&hoHover);
		GenEmptyObj(&hoUnhover);
		for (int i = 0; i < m_hoSearchRegion.CountObj(); i++)
		{
			if (m_nSearchLineSN == i)
				ConcatObj(hoHover, m_hoSearchRegion.SelectObj(i + 1), &hoHover);
			else
				ConcatObj(hoUnhover, m_hoSearchRegion.SelectObj(i + 1), &hoUnhover);

			if (m_nSearchLineSN == i)
				m_Wnd_Image.DispMessage(m_vecDispString[i], (int)m_hvDispY[i].D(), (int)m_hvDispX[i].D(), "green", DCS_Image, FALSE);
			else
				m_Wnd_Image.DispMessage(m_vecDispString[i], (int)m_hvDispY[i].D(), (int)m_hvDispX[i].D(), "blue", DCS_Image, FALSE);
		}

		m_Wnd_Image.DispRegion(hoUnhover, "blue", "margin", 2, TRUE);
		m_Wnd_Image.DispRegion(hoHover, "red", "margin", 4, FALSE);
	}
	else
	{
		m_Wnd_Image.ClearRegion();
		m_Wnd_Image.ClearMessage();
		m_Wnd_Image.DispImage(FALSE);
	}
}

BOOL CDlgParam::DispLineHor(MSG* pMsg)
{
	if (!m_bDrawLineHor) return TRUE;
	CRect rgn;
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(rgn);
	ScreenToClient(rgn);
	if (!rgn.PtInRect(pMsg->pt)) return FALSE;

	try
	{
		HTuple hvWidth, hvHeight;
		GetImageSize(m_Wnd_Image.GetDispImage(), &hvWidth, &hvHeight);
		if (m_nMouseY >= 0 && m_nMouseY <= hvHeight - 1)
		{
			HObject hoLine;
			GenRegionLine(&hoLine, m_nMouseY,0, m_nMouseY, hvWidth - 1);
			m_Wnd_Image.DispRegion(hoLine, "red", "margin", 2, 1);
		}
	}
	catch (HException& e)
	{

	}
	return TRUE;
}

BOOL CDlgParam::GetLButtonLineSN(MSG* pMsg)
{
	CRect rgn;
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(rgn);
	ScreenToClient(rgn);
	if (!rgn.PtInRect(pMsg->pt))
	{
		m_nSearchLineSN = -1;
		return FALSE;
	}

	try
	{
		for (int i = 0; i < m_hoSearchRegion.CountObj(); i++)
		{
			HObject hoRgn = m_hoSearchRegion.SelectObj(i + 1);
			if (hoRgn.CountObj() > 0)
			{
				HTuple hvInside;
				TestRegionPoint(hoRgn, m_nMouseY, m_nMouseX, &hvInside);
				if (hvInside == TRUE)
				{
					m_nSearchLineSN = i;

					/*CString strMessage;
					strMessage.Format("SN:%d", m_nSearchLineSN);
					m_Wnd_Image.DispMessage(strMessage, m_nMouseY/6, m_nMouseX, "green", TRUE);*/

					CLineMeasure::LineMeasureInput tIn;
					CLineMeasure::LineMeasureOutput tOut;
					tIn.hoImage = m_Wnd_Image.GetDispImage();
					tIn.nMode = DebugMode;
					m_pTemp->m_LineMeasure.FindSingleLine(m_nSearchLineSN, tIn, tOut);
					HObject holine;
					holine = tOut.hoDisp;
					m_Wnd_Image.DispRegion(tOut.hoDisp, "green", "margin", 2, 0);
					break;
				}

			}
		}
	}
	catch (HException& e)
	{

	}
	return true;
}

void CDlgParam::RefreshTestList()
{
	//测量结果
	CString strVal;
	int nCount = 0;
	m_listInfo.RemoveAll();
	CBCGPGridRow* pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
	if (m_tOutput.hv_HasCoat == TRUE)
	{
		pRow->GetItem(nCount++)->SetValue(_variant_t("测量结果(mm)"), FALSE);
		for (int i = 0; i < m_tOutput.tSize.hv_Width.Length(); i++)
		{
			strVal.Format("%0.4f", m_tOutput.tSize.hv_Width[i].D());
			pRow->GetItem(nCount++)->SetValue(_variant_t(strVal), FALSE);
		}
	}
	else
	{
		pRow->GetItem(nCount++)->SetValue(_variant_t("测量结果(mm)"), FALSE);
		pRow->GetItem(nCount++)->SetValue(_variant_t("无涂膜"), FALSE);
	}
	m_listInfo.AddRow(pRow, FALSE);
	//错位距离结果
	if (m_tOutput.hv_HasCoat == TRUE && m_pTemp->m_tInit.tProductInfo.nCoatingType == TwoSide)
	{
		nCount = 0;
		pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
		pRow->GetItem(nCount++)->SetValue(_variant_t("错位距离值(mm)"), FALSE);
		for (int i = 0; i < m_tOutput.tSize.hv_MisalignedDist.Length(); i++)
		{
			if(m_tOutput.tSize.hv_MisalignedDist[i]!=NO_DATA)
				strVal.Format("%0.4f", m_tOutput.tSize.hv_MisalignedDist[i].D());
			else
				strVal = "-";
			pRow->GetItem(nCount++)->SetValue(_variant_t(strVal), FALSE);
		}
		m_listInfo.AddRow(pRow, FALSE);
	}
	

	//测试凹版结果
	if (m_pTemp->m_tInit.tProductInfo.bExistBaseCoat == TRUE && m_tOutput.tSize.hv_WholeBaseCoat.Length()>0)
	{
		nCount = 0;
		pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
		pRow->GetItem(nCount++)->SetValue(_variant_t("凹版全尺寸(mm)"), FALSE);
		for (int i = 0; i < m_tOutput.tSize.hv_WholeBaseCoat.Length(); i++)
		{
			strVal.Format("%0.4f", m_tOutput.tSize.hv_WholeBaseCoat[i].D());
			pRow->GetItem(nCount++)->SetValue(_variant_t(strVal), FALSE);
		}
		m_listInfo.AddRow(pRow, FALSE);
	}

	//测试基材宽
	if (m_tOutput.tSize.hv_WholeWidth.Length()>1)
	{
		CString strItem, strTemp;
		pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("基材尺寸(mm)"), FALSE);
		strItem += "基材尺寸(mm)";
		strVal.Format("%0.4f", m_tOutput.tSize.hv_WholeWidth.D());
		strTemp.Format(",%s", strVal);
		strItem += strTemp;
		pRow->GetItem(1)->SetValue(_variant_t(strVal), FALSE);
		double dSum = 0;
		if (m_pTemp->m_tMeasureAdvanceParam.eFoilSelect == CMeasure::FS_FoilBaseCoat)
		{
			if (m_pTemp->m_tInit.tProductInfo.bExistBaseCoat == TRUE)
				dSum = m_tOutput.tBaseCoat.hv_Width.TupleSum().D();
			else if (m_pTemp->m_tInit.tProductInfo.bExistAT11 == TRUE)
				dSum = m_tOutput.tAT11.hv_Width.TupleSum().D();
		}

		strVal.Format("%0.4f", m_tOutput.tSize.hv_Width.TupleSum().D() - dSum);
		strTemp.Format(",%s", strVal);
		strItem += strTemp;
		pRow->GetItem(2)->SetValue(_variant_t(strVal), FALSE);
		strItem += "\n";
		m_listInfo.AddRow(pRow, FALSE);
	}

	//添加未找到边的情况
	AddErrorLineIndex();

	//瑕疵检测结果
	for (int i = 0; i < m_tOutput.tDefect.ho_Region.CountObj(); i++)
	{
		AddDefectList(i);
	}

	//点检检测结果，是2的倍数就输出结果
	if (m_tOutput.tSpotInspection.hv_Width.Length() >= 5);
	{
		for (int i = 0; i < m_tOutput.tSpotInspection.hv_Width.Length(); i = i + 2)
		{
			if (i + 1 >= m_tOutput.tSpotInspection.hv_Width.Length())
			{
				//if (m_pTemp->m_tInit.hvCamSNList.Length() == 2)
				{
					pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
					pRow->GetItem(0)->SetValue(_variant_t("两点检目标间距(mm)"), FALSE);
					if (m_tOutput.tSpotInspection.hv_Width[i] == NO_DATA)
						strVal = "-";
					else
						strVal.Format("%0.4f", m_tOutput.tSpotInspection.hv_Width[i].D());
					pRow->GetItem(1)->SetValue(_variant_t(strVal), FALSE);
					m_listInfo.AddRow(pRow, FALSE);
				}
				continue;
			}
			pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
			pRow->GetItem(0)->SetValue(_variant_t("点检结果[宽,高](mm)"), FALSE);
			if (m_tOutput.tSpotInspection.hv_Width[i] == NO_DATA)
				strVal = "-";
			else
				strVal.Format("%0.4f", m_tOutput.tSpotInspection.hv_Width[i].D());
			pRow->GetItem(1)->SetValue(_variant_t(strVal), FALSE);
			if (m_tOutput.tSpotInspection.hv_Width[i + 1] == NO_DATA)
				strVal = "-";
			else
				strVal.Format("%0.4f", m_tOutput.tSpotInspection.hv_Width[i + 1].D());
			pRow->GetItem(2)->SetValue(_variant_t(strVal), FALSE);
			m_listInfo.AddRow(pRow, FALSE);
		}
	}

	pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("检测耗时(ms)"), FALSE);
	pRow->GetItem(1)->SetValue(_variant_t(m_SpendTime.GetMillisecondInt()), FALSE);
	m_listInfo.AddRow(pRow, FALSE);

	m_listInfo.AdjustLayout();
}

void CDlgParam::AddDefectList(int i)
{
	if (i < 0) return;

	HObject ho_ObjectSelected;
	HTuple hv_InMean, hv_InDeviation;

	SelectObj(m_tOutput.tDefect.ho_Region, &ho_ObjectSelected, i + 1);

	//区域灰度均值和偏差
	Intensity(ho_ObjectSelected, m_tInput.hoGrabImage, &hv_InMean, &hv_InDeviation);

	CString strStripe, strErrorName, strErrorKey, strInfo;
	strErrorKey.Format("Error%dName", m_tOutput.tDefect.hv_Type[i].I());
	strStripe.Format("区域%d", m_tOutput.tDefect.hv_StripeSN[i].I());
	strErrorName.Format("%s", m_pTemp->m_Defect.m_hvErrorName[m_tOutput.tDefect.hv_Type[i].I()].S().Text());
	CBCGPGridRow* pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
	strInfo.Format("表面缺陷%d", i);
	pRow->GetItem(0)->SetValue(_variant_t(strInfo), FALSE);
	pRow->GetItem(1)->SetValue(_variant_t(_T(strStripe)), FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(_T(strErrorName)), FALSE);
	strInfo.Format("面积:%.3fmm2, 最大直径 : %.3fmm, 宽度：%.3fmm, 高度 : %.3fmm, 灰度均值 : %.2f, 灰度偏差 : %.2f",
		m_tOutput.tDefect.tFeature.hvArea[i].D(),
		m_tOutput.tDefect.tFeature.hvDiameter[i].D(),
		m_tOutput.tDefect.tFeature.hvWidth[i].D(),
		m_tOutput.tDefect.tFeature.hvHeight[i].D(),
		hv_InMean.D(), 
		hv_InDeviation.D());
	pRow->GetItem(3)->SetValue(_variant_t(strInfo), FALSE);
	m_listInfo.AddRow(pRow, FALSE);
}

void CDlgParam::AddErrorLineIndex()
{
	if (m_pTemp->m_hvErrorLineIndex.Length() == 0 || m_pTemp->m_hvErrorLineIndex == -1) return;
	CBCGPGridRow* pRow;
	CString strInfo;
	HTuple hvErrorIndex = m_pTemp->m_hvErrorLineIndex;
	for (int i=0;i<hvErrorIndex.Length();i++)
	{
		pRow = m_listInfo.CreateRow(m_listInfo.GetColumnCount());
		strInfo.Format("%d边未找到边", hvErrorIndex[i].I());
		pRow->GetItem(0)->SetValue(_variant_t(strInfo), FALSE);
		pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
		m_listInfo.AddRow(pRow, FALSE);
	}
}

void CDlgParam::OnBnClickedButtonTestall()
{
	try
	{
		CString strMessage;
		m_SpendTime.Start();
		MeasureOutput tOut;
		m_pTemp->DoSomething("Action", &m_tInput, &tOut);
		m_SpendTime.End();
		//strMessage.Format("SpendTime:%d ms", m_SpendTime.GetMillisecondInt());

		m_tOutput = tOut;
		m_Wnd_Image.ClearMessage();
		if (m_CHECK_bShowAdaptedImage)
		{
			//显示灰度拉伸/自适应图像
			HObject hoAdaptedImage;
			m_pTemp->DoSomething("GetAdaptedImage", NULL, &hoAdaptedImage);
			m_Wnd_Image.DispImage(hoAdaptedImage, FALSE);
		}
		else
		{
			//显示原图
			m_Wnd_Image.DispImage(m_tInput.hoGrabImage, FALSE);
		}
		//m_Wnd_Image.DispRegion(tOut.ho_Disp, "green", "margin", 2, TRUE);
		m_Wnd_Image.DispRegion(tOut.tCoat.ho_Region, "yellow", "margin", 1, TRUE);
		m_Wnd_Image.DispRegion(tOut.tAT11.ho_Region, "green", "margin", 1, FALSE);
		m_Wnd_Image.DispRegion(tOut.tBaseCoat.ho_Region, "green", "margin", 1, FALSE);
		m_Wnd_Image.DispRegion(tOut.tFoil.ho_Region, "orange", "margin", 1, FALSE);
		m_Wnd_Image.DispRegion(tOut.tDefect.ho_Region, "red", "margin", 2, FALSE);

		if (m_pTemp->m_tInit.hvCamSNList.Length() == 2 && m_pTemp->m_Calibration.GetStripeParam().hvColumnOver.Length() == 2)
		{
			HTuple hvHeight;
			GetImageSize(m_tInput.hoGrabImage, &HTuple(), &hvHeight);
			HObject hoRegion;
			GenRectangle1(&hoRegion, 0, m_pTemp->m_Calibration.GetStripeParam().hvColumnOver.TupleMin(), hvHeight - 1, m_pTemp->m_Calibration.GetStripeParam().hvColumnOver.TupleMax());
			m_Wnd_Image.DispRegion(hoRegion, "violet", "margin", 2, FALSE);
		}
		if (m_pTemp->m_tInit.hvCamSNList.Length() == 2 && m_pTemp->m_Calibration.GetStripeParam().hvLeftRightColumnOver.Length() == 2)
		{
			HTuple hvHeight;
			GetImageSize(m_tInput.hoGrabImage, &HTuple(), &hvHeight);
			HObject hoRegion;
			GenRectangle1(&hoRegion, 0, m_pTemp->m_Calibration.GetStripeParam().hvLeftRightColumnOver.TupleMin(), hvHeight - 1, m_pTemp->m_Calibration.GetStripeParam().hvLeftRightColumnOver.TupleMax());
			m_Wnd_Image.DispRegion(hoRegion, "blue", "margin", 2, FALSE);
		}

		//点检输出
		m_Wnd_Image.DispRegion(tOut.tSpotInspection.ho_Region, "blue", "margin", 2, FALSE);
		/*if (tOut.tSpotInspection.hv_Width.Length() % 2 == 0)
		{
			strMessage = "";
			for (int i = 0; i < tOut.tSpotInspection.hv_Width.Length(); i = i + 2)
			{
				CString strItem;
				strItem.Format("点检参数输出%d：[宽,高]=[%f,%f] mm;", i / 2 + 1, tOut.tSpotInspection.hv_Width[i].D(), tOut.tSpotInspection.hv_Width[i + 1].D());
				strMessage += strItem;
			}
		}*/
		m_Wnd_Image.DispMessage(strMessage);

		RefreshTestList();

		if (m_bConfirmMeasure)
		{
			CDlgCheckMeasure dlg;
			dlg.m_tInput = m_tInput;
			dlg.m_tOutput = m_tOutput;
			dlg.m_pTemp = m_pTemp;
			dlg.m_pParent = this;
			dlg.DoModal();
		}
	}
	catch (HException e)
	{
		CString strMessageBox;
		strMessageBox.Format("检测出现异常:%s", e.ErrorMessage().Text());
		BCGPMessageBox(strMessageBox);
	}
}

void CDlgParam::OnBnClickedButtonSave()
{
	ePOWERLIMITMODULE ePm = PM_Detect;
	if (!m_pReal->m_tInit.pMain(MT_Detect, "CheckPermission", "", &ePm, NULL))
	{
		m_pReal->m_tInit.pMain(MT_Detect, "Login", "", &ePm, NULL);
		if (!m_pReal->m_tInit.pMain(MT_Detect, "CheckPermission", "", &ePm, NULL))
			return;
	}
	UpdateData(TRUE);

	m_pTemp->m_LineMeasure.m_hvOverColumn = m_pTemp->m_Calibration.GetStripeParam().hvColumnOver;
	m_pReal->DoSomething("CopyObjects", m_pTemp, NULL);

	m_pReal->WriteParam();

	//OnBnClickedButtonTestall();
	BCGPMessageBox("保存成功");
}


void CDlgParam::OnBnClickedButtonSaveimage()
{
	try
	{
		SYSTEMTIME CurrentSystime;
		GetLocalTime(&CurrentSystime);
		//保存图片
		CString strPath;
		CString strImageName;
		CreateDirectory("D:\\SaveImage", NULL);
		strPath.Format("D:\\SaveImage\\%dm%dd\\", CurrentSystime.wMonth, CurrentSystime.wDay);
		CreateDirectory(strPath, NULL);
		if (m_pTemp->m_tInit.hvCamSNList.Length() > 1)
		{
			strImageName.Format("%dh%dm%ds_Algo%d_Tiled.bmp", CurrentSystime.wHour, CurrentSystime.wMinute, CurrentSystime.wSecond, m_pTemp->m_tInit.nSN);
		}
		else
		{
			strImageName.Format("%dh%dm%ds_Algo%d.bmp", CurrentSystime.wHour, CurrentSystime.wMinute, CurrentSystime.wSecond, m_pTemp->m_tInit.nSN);
		}
		
		WriteImage(m_Wnd_Image.GetDispImage(), "bmp", NULL, HTuple(strPath + strImageName));

		CString strMessageBox;
		strMessageBox.Format("图片保存完毕\r\n%s%s\r\n", strPath, strImageName);
		BCGPMessageBox(strMessageBox);
	}
	catch (HException e)
	{
		BCGPMessageBox("保存图片失败");
	}
}


void CDlgParam::OnBnClickedButtonOpenimage()
{
	TCHAR szFilter[MAX_PATH * 2] = _T("所有文件(*.*)|*.*||");
	//CFileDialog FileDlg(TRUE, _T("*.*"), NULL, 0, szFilter, this);
	CFileDialog FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, _T("所有文件(*.*)|*.*||"));
	FileDlg.m_ofn.lpstrFile = szFilter;
	FileDlg.m_ofn.nMaxFile = MAX_PATH * 2;
	FileDlg.m_ofn.lpstrFile[0] = '\0';
	if (IDOK == FileDlg.DoModal())
	{
		HObject hoImage;
		GenEmptyObj(&m_tInput.hoGrabImage);
		CString strFilename = FileDlg.GetPathName();
		POSITION pos = FileDlg.GetStartPosition();
		while (pos != NULL)
		{
			strFilename = FileDlg.GetNextPathName(pos);//取得文件路径
			try
			{
				ReadImage(&hoImage, (HTuple)strFilename);
				if (m_tInput.hoGrabImage.CountObj() < m_pTemp->m_tInit.hvCamSNList.Length())
					ConcatObj(m_tInput.hoGrabImage, hoImage, &m_tInput.hoGrabImage);
			}
			catch (HException&)
			{
			}
		}
		try
		{
			TileImages(m_tInput.hoGrabImage, &m_tInput.hoGrabImage, m_tInput.hoGrabImage.CountObj(), "horizontal");
			m_Wnd_Image.ClearRegion();
			m_Wnd_Image.ClearMessage();
			m_Wnd_Image.DispImage(m_tInput.hoGrabImage);
			//m_Wnd_Image.DispImage(m_tInput.hoGrabImage, FALSE);
		}
		catch (HException)
		{
			BCGPMessageBox("读取图片失败");
		}
	}
}


void CDlgParam::OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	SwitchDispPage(m_TAB_List.GetCurSel());
}

void CDlgParam::SwitchDispPage(const int& nSN)
{
	if (nSN >= m_vecPages.size())return;

	for (int i = 0; i < m_vecPages.size(); i++)
	{
		if (nSN == i)
			m_vecPages[i]->ShowWindow(SW_NORMAL);
		else
			m_vecPages[i]->ShowWindow(SW_HIDE);
	}
}

LRESULT CDlgParam::OnTest(WPARAM wParam, LPARAM lParam)
{

	return 0L;
}


void CDlgParam::OnBnClickedCheckShowAdaptedImage()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
}


void CDlgParam::OnBnClickedButtonDispNewimage()
{
	// TODO: 在此添加控件通知处理程序代码
	try
	{
		HObject hoImage;
		GenEmptyObj(&hoImage);
		CString strSide;
		strSide.Format("Side%d", m_pTemp->m_tInit.nSN);
		m_pTemp->m_tInit.pMain(MT_Measure, "GetNewImage", strSide, NULL, &hoImage);
		if (hoImage.CountObj() == 0)
		{
			BCGPMessageBox("获取新图像失败");
			return;
		}

		TileImages(hoImage, &m_tInput.hoGrabImage, hoImage.CountObj(), "horizontal");
		m_Wnd_Image.ClearRegion();
		m_Wnd_Image.ClearMessage();
		m_Wnd_Image.DispImage(m_tInput.hoGrabImage);
		//m_Wnd_Image.DispImage(m_tInput.hoGrabImage, FALSE);
	}
	catch (HException)
	{
		BCGPMessageBox("读取图片失败");
	}
}
