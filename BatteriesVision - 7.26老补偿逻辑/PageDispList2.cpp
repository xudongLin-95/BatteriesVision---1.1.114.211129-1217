// PageDispList2.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageDispList2.h"
#include "afxdialogex.h"


// CPageDispList2 对话框

IMPLEMENT_DYNAMIC(CPageDispList2, CBCGPDialog)

CPageDispList2::CPageDispList2(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageDispList2::IDD, pParent)
	, m_hvecWndHandle(1)
{
	m_hvecWndHandle.Clear();
	GenEmptyObj(&m_tDefects.hoCropImageList);
	m_tDefects.hvCamSNList.Clear();
	m_tDefects.hvErrorNameList.Clear();
	m_tDefects.hvImageSNList.Clear();
	GenEmptyObj(&m_tDefects.hoDefectImageList);
	m_tDefects.hvSideSNList.Clear();
	m_tDefects.hvDefectXList.Clear();
	m_tDefects.hvDefectYList.Clear();
}

CPageDispList2::~CPageDispList2()
{
}

void CPageDispList2::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageDispList2, CBCGPDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CPageDispList2 消息处理程序


BOOL CPageDispList2::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageDispList2::PreTranslateMessage(MSG* pMsg)
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
	else if (pMsg->message == WM_LBUTTONDBLCLK)//双击鼠标左键
	{
		return LDoubleClkAlgDlg(pMsg);
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

BOOL CPageDispList2::LDoubleClkAlgDlg(MSG* pMsg)
{
	BOOL bRet = FALSE;
	try
	{
		HTuple Button, Row, Column;
		for (int i = 0; i < m_hvecWndHandle.Length(); i++)
		{
			try
			{
				GetMposition(m_hvecWndHandle[i].T(), &Row, &Column, &Button);
				if (1 == Button)//左键
				{
					HObject ObjectSelected;
					HObject ImageSelected;
					HTuple Inside;
					for (int j = 0; j < m_tDefects.hoDefectImageList.CountObj(); j++)
					{
						SelectObj(m_tDefects.hoDefectImageList, &ObjectSelected, j + 1);
						TestRegionPoint(ObjectSelected, Row, Column, &Inside);

						if (1 == Inside)
						{
							WPARAM wParam = MAKEWPARAM(theGlobal.m_Permission.GetUserInfo().ePowerType, 0);
							LPARAM lParam = MAKELPARAM(0, 0);
							theGlobal.PostMessageToMainDlg(WM_CHECK_PMSN, wParam, lParam);
							MeasureInput tInput;
							tInput.nMode = DebugMode;
							GenEmptyObj(&tInput.hoGrabImage);
							ConcatObj(tInput.hoGrabImage, m_tDefects.hoDefectImageList.SelectObj(j + 1), &tInput.hoGrabImage);
							theGlobal.m_AlgTool.ShowParamDlg(m_tDefects.hvSideSNList[j].I(), CAlgorithmTool::AlgMeasure, &tInput);
							bRet = TRUE;
							return bRet;
						}
					}
				}
			}
			catch (HException&) {}
		}
	}
	catch (HException)
	{
		bRet = FALSE;
	}
	return TRUE;
}

void CPageDispList2::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageDispList2::ResetSize()
{
	InitUI();
}

void CPageDispList2::InitUI()
{

	ResetList();
}

void CPageDispList2::ResetList()
{
	CRect rcPic, rcDlg;
	GetClientRect(rcDlg);
	rcDlg.left += 2;
	rcDlg.top += 3;
	rcDlg.bottom -= 1;
	rcDlg.right -= 2;

	int ROW_NUM = 4;
	int COL_NUM = 5;
	int GAP = 1;

	SetCheck("~father");
	///////两个窗体//
	SetWindowAttr("background_color", "#353539");//#252525

	for (int i = 0; i < ROW_NUM*COL_NUM; i++)
	{
		int nRowSN = i / COL_NUM;
		int nColSN = i % COL_NUM;

		int nWidth = (rcDlg.Width() - (COL_NUM - 1) * GAP) / COL_NUM;
		int nHeight = (rcDlg.Height() - (ROW_NUM - 1) * GAP) / ROW_NUM;

		rcPic.left = (nWidth + (COL_NUM - 1) * GAP)*nColSN;
		rcPic.right = (nWidth + (COL_NUM - 1) * GAP)*nColSN + nWidth;
		
		rcPic.top = (nHeight + (ROW_NUM - 1) * GAP)*nRowSN;
		rcPic.bottom = (nHeight + (ROW_NUM - 1) * GAP)*nRowSN + nHeight;

		HTuple hvTmp;
		OpenWindow(rcPic.top, rcPic.left, rcPic.Width(), rcPic.Height(),
			(Hlong)this->GetSafeHwnd(), "visible", "", &m_hvecWndHandle[i].T());
	}

	SetCheck("father");

	CString strBG;
	strBG.Format("%s\\res\\bg.png", GetCurrentAppPath());
	if (!PathFileExists(strBG))
		return;
	HObject hoBG;
	HTuple hvW, hvH, hvNum;
	ReadImage(&hoBG, (HTuple)strBG);
	GetImageSize(hoBG, &hvW, &hvH);
	for (int i = 0; i < m_hvecWndHandle.Length(); i++)
	{
		SetPart(m_hvecWndHandle[i].T(), 0, 0, hvH-1, hvW-1);
		DispObj(hoBG, m_hvecWndHandle[i].T());
	}
}

void CPageDispList2::UpdateList(C_MergeWork& tOutput)
{
	/*HTuple hvDisp;
	SetPart(m_hvecWndHandle[0].T(), 0, 0, 100, 100);
	DispObj(tOutput.vecB[0].vecA[0].hoGrabImage, m_hvecWndHandle[0].T());*/

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	int ROW_NUM = 4;
	int COL_NUM = 5;
	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	CString strTime;
	strTime.Format("%02d:%02d:%02d.%03dms", tTm.wHour, tTm.wMinute, tTm.wSecond, tTm.wMilliseconds);
	for (int nBWorkSN = 0;nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
	{
		for (int i = 0; i < tOutput.vecB[nBWorkSN].tMeasure.tDefect.hv_Type.Length(); i++)
		{
			if (tOutput.vecB[nBWorkSN].tMeasure.tDefect.hv_Type[i].I() >= NG_ExposeFoil &&
				i <= tOutput.vecB[nBWorkSN].tMeasure.tDefect.ho_Region.CountObj() - 1)
			{
				HObject hoRgn, hoRect, hoCrop;
				SelectObj(tOutput.vecB[nBWorkSN].tMeasure.tDefect.ho_Region, &hoRgn, i + 1);
				HTuple hvRow1, hvRow2, hvCol1, hvCol2, hvWidth, hvHeight, hvCenterRow, hvCenterCol, hvRegionWidth, hvRegionHeight;
				GetImageSize(tOutput.vecB[nBWorkSN].tMeasure.ho_DispImage, &hvWidth, &hvHeight);
				SmallestRectangle1(hoRgn, &hvRow1, &hvCol1, &hvRow2, &hvCol2);
				RegionFeatures(hoRgn, "width", &hvRegionWidth);
				RegionFeatures(hoRgn, "height", &hvRegionHeight);
				AreaCenter(hoRgn, &HTuple(), &hvCenterRow, &hvCenterCol);
				int r0 = (int)max(0, hvCenterRow.D() - hvRegionHeight.D() * 2);
				int c0 = (int)max(0, hvCenterCol.D() - hvRegionWidth.D() * 2);
				int r1 = (int)min(hvHeight.D() - 1, hvCenterRow.D() + hvRegionHeight.D() * 2);
				int c1 = (int)min(hvWidth.D() - 1, hvCenterCol.D() + hvRegionWidth.D() * 2);
				CropRectangle1(tOutput.vecB[nBWorkSN].tMeasure.ho_DispImage, &hoCrop, r0, c0, r1, c1);
				
				//瑕疵的坐标
				double dX = tOutput.vecB[nBWorkSN].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Column[i].D();
				/*double dY = tOutput.vecB[nBWorkSN].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Row[i].D() / 1000
					+ tOutput.hv_MeterCount.D() - hvHeight.D()*theGlobal.m_vecResolution[0].dRow / 1000;*/
				double dY = tOutput.hv_MeterCount.D();

				ConcatObj(m_tDefects.hoCropImageList, hoCrop, &m_tDefects.hoCropImageList);
				m_tDefects.hvCamSNList.Append(tOutput.vecB[nBWorkSN].vecA[0].hvGrabberSN.I());
				m_tDefects.hvErrorNameList.Append((HTuple)theGlobal.m_Counter.GetNGTypeName(tOutput.vecB[nBWorkSN].tMeasure.tDefect.hv_Type[i].I()));
				m_tDefects.hvImageSNList.Append(tOutput.vecB[nBWorkSN].vecA[0].hvImageSN.I());
				//2021-4-18 存储有瑕疵的原图(拼接后的图像)
				ConcatObj(m_tDefects.hoDefectImageList, tOutput.vecB[nBWorkSN].tMeasure.ho_DispImage, &m_tDefects.hoDefectImageList);
				m_tDefects.hvSideSNList.Append(nBWorkSN);
				m_tDefects.hvDefectXList.Append(dX);
				m_tDefects.hvDefectYList.Append(dY);
				m_tDefects.hvTimeList.Append(HTuple(strTime));

				if (m_tDefects.hoCropImageList.CountObj() > ROW_NUM*COL_NUM)
				{
					m_tDefects.hoCropImageList = m_tDefects.hoCropImageList.RemoveObj(1);
					m_tDefects.hvCamSNList = m_tDefects.hvCamSNList.TupleRemove(0);
					m_tDefects.hvErrorNameList = m_tDefects.hvErrorNameList.TupleRemove(0);
					m_tDefects.hvImageSNList = m_tDefects.hvImageSNList.TupleRemove(0);
					m_tDefects.hoDefectImageList = m_tDefects.hoDefectImageList.RemoveObj(1);
					m_tDefects.hvSideSNList = m_tDefects.hvSideSNList.TupleRemove(0);

					m_tDefects.hvDefectXList = m_tDefects.hvDefectXList.TupleRemove(0);
					m_tDefects.hvDefectYList = m_tDefects.hvDefectYList.TupleRemove(0);
				}
			}
		}
	}
	
	if (m_hvecWndHandle.Length() == m_tDefects.hoCropImageList.CountObj())
	{
		for (int i = 0; i < m_hvecWndHandle.Length(); i++)
		{
			HObject hoCrop;
			SelectObj(m_tDefects.hoCropImageList, &hoCrop, i + 1);
			HTuple hvHeight, hvWidth;
			GetImageSize(hoCrop, &hvWidth, &hvHeight);
			SetPart(m_hvecWndHandle[i].T(), 0, 0, hvHeight - 1, hvWidth - 1);
			DispObj(hoCrop, m_hvecWndHandle[i].T());

			CString strName;
			strName.Format("%s\n%s %s\nX:%0.3fmm\nY:%0.3fm",
				m_tDefects.hvTimeList[i].S().Text(), m_tDefects.hvSideSNList[i].I() == 0 ? "正面" : "反面",
				m_tDefects.hvErrorNameList[i].S().Text(), m_tDefects.hvDefectXList[i].D(), m_tDefects.hvDefectYList[i].D());
			disp_message(m_hvecWndHandle[i].T(), HTuple(strName), "window", 5, 5, "red", "false");
		}
	}
}

BOOL CPageDispList2::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}
