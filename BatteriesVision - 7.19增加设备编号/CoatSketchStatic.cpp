// CoatSketchStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "CoatSketchStatic.h"


// CCoatSketchStatic

IMPLEMENT_DYNAMIC(CCoatSketchStatic, CWnd)

CCoatSketchStatic::CCoatSketchStatic()
{
	m_crBg = RGB(58, 58, 58);
	m_crBaseFoil = RGB(220, 174, 0);
	m_crCoat = RGB(150, 150, 150);

	m_crAT11 = RGB(200, 200, 200);
	m_crBaseCoat = RGB(100, 100, 100);

	m_crDefect = RGB(248, 43, 43);

	m_crOK = RGB(0, 255, 0);
	m_crNG = RGB(255, 0, 0);
	m_crWarn = RGB(248, 182, 43);

	m_crValueName = RGB(200, 200, 200);
	m_crBigTitle = RGB(220, 220, 220);
}

CCoatSketchStatic::~CCoatSketchStatic()
{
}


BEGIN_MESSAGE_MAP(CCoatSketchStatic, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CCoatSketchStatic 消息处理程序
void CCoatSketchStatic::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	//GetWindowRect(m_rcClient);
	//ScreenToClient(m_rcClient);
	GetClientRect(m_rcClient);

	m_nClientHeight = m_rcClient.Height();
	m_nClientWidth = m_rcClient.Width();
}

void CCoatSketchStatic::Init(tagInit tInit)
{
	m_tInit = tInit;
	m_tWidth.Clear();
}

void CCoatSketchStatic::InvalidateCtrl()
{
	CClientDC dc(this);

	//显示箔材
	DispBaseFoil(dc);
	//计算坐标，显示涂布
	DispCoat(dc);
	//显示标题
	DispTitle(dc);
	//显示测量值
	try
	{
		DispValue(dc);
	}
	catch (std::out_of_range)
	{

	}
	catch (...)
	{

	}
	//显示缺陷
	try
	{
		DispDefects(dc);
	}
	catch (std::out_of_range)
	{

	}
	catch (...)
	{

	}
}

void CCoatSketchStatic::DispBaseFoil(CClientDC& dc)
{
	if (m_tInit.nCoatType == 1)
	{
		m_rcBaseFoil.top = m_rcClient.top + m_rcClient.Height() / 2 - 5 - 20;
		m_rcBaseFoil.bottom = m_rcClient.top + m_rcClient.Height() / 2 + 5 - 20;
	}
	else
	{
		m_rcBaseFoil.top = m_rcClient.top + m_rcClient.Height() / 2 - 5;
		m_rcBaseFoil.bottom = m_rcClient.top + m_rcClient.Height() / 2 + 5;
	}
	
	m_rcBaseFoil.left = m_rcClient.left + 70;
	m_rcBaseFoil.right = m_rcClient.right - 30;

	CBrush brushBack;
	brushBack.CreateSolidBrush(m_crBaseFoil);
	dc.SetBkColor(m_crBg);
	dc.FillRect(m_rcBaseFoil, &brushBack);

	/*CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_crBigTitle);
	dc.MoveTo(m_rcClient.left, m_rcClient.top);
	dc.LineTo(m_rcClient.right, m_rcClient.top);
	dc.LineTo(m_rcClient.right, m_rcClient.bottom);
	dc.LineTo(m_rcClient.left, m_rcClient.bottom);
	dc.LineTo(m_rcClient.left, m_rcClient.top);*/
}

void CCoatSketchStatic::DispCoat(CClientDC& dc)
{
	if (m_tInit.nCoatNum < 0) return;
	//显示涂布，涂布宽是极耳宽的3.5倍
	int FOIL_WIDTH = m_rcBaseFoil.Width() / (4.5 * m_tInit.nCoatNum + 1);
	int COAT_WIDTH = FOIL_WIDTH * 3.5;
	int AT11_WIDTH = 7;
	
	CBrush brushCoat, brushAT11, brushBaseCoat;
	brushCoat.CreateSolidBrush(m_crCoat);
	brushAT11.CreateSolidBrush(m_crAT11);
	brushBaseCoat.CreateSolidBrush(m_crBaseCoat);
	dc.SetBkColor(m_crBg);

	m_vecRtCoat.clear();
	m_vecRtCoat2.clear();
	m_vecRtFoil.clear();
	m_vecRtFoil2.clear();
	m_vecRtAT11BaseCoat.clear();
	m_vecRtAT11BaseCoat2.clear();

	CRect rcCoat, rcCoat2, rcAT11BaseCoat, rcAT11BaseCoat2, rcFoil, rcFoil2;
	rcCoat.top = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 18;
	rcCoat.bottom = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 5;
	if (m_tInit.bExistBaseCoat)
		rcCoat.bottom = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 9;

	rcAT11BaseCoat.top = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 9;
	rcAT11BaseCoat.bottom = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 5;

	rcFoil.top = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 9;
	rcFoil.bottom = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 5;

	rcCoat2.top = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 5;
	rcCoat2.bottom = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 18;
	if (m_tInit.bExistBaseCoat)
		rcCoat2.top = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 9;

	rcAT11BaseCoat2.top = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 5;
	rcAT11BaseCoat2.bottom = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 9;

	rcFoil2.top = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 5;
	rcFoil2.bottom = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 9;

	for (int i = 0; i < m_tInit.nCoatNum; i++)
	{
		rcCoat.left = m_rcBaseFoil.left + COAT_WIDTH * i + FOIL_WIDTH * (i+1);
		rcCoat.right = rcCoat.left + COAT_WIDTH;
		dc.FillRect(rcCoat, &brushCoat);
		m_vecRtCoat.push_back(rcCoat);

		if (m_tInit.bExistAT11)
		{
			rcAT11BaseCoat.left = rcCoat.left - AT11_WIDTH;
			rcAT11BaseCoat.right = rcCoat.left;
			dc.FillRect(rcAT11BaseCoat, &brushAT11);

			rcAT11BaseCoat.left = rcCoat.right;
			rcAT11BaseCoat.right = rcCoat.right + AT11_WIDTH;
			dc.FillRect(rcAT11BaseCoat, &brushAT11);
		}
		else if (m_tInit.bExistBaseCoat)
		{
			rcAT11BaseCoat.left = rcCoat.left - AT11_WIDTH;
			rcAT11BaseCoat.right = rcCoat.right + AT11_WIDTH;
			dc.FillRect(rcAT11BaseCoat, &brushBaseCoat);
		}
		rcAT11BaseCoat.left = rcCoat.left - AT11_WIDTH;
		rcAT11BaseCoat.right = rcCoat.left;
		m_vecRtAT11BaseCoat.push_back(rcAT11BaseCoat);
		rcAT11BaseCoat.left = rcCoat.right;
		rcAT11BaseCoat.right = rcCoat.right + AT11_WIDTH;
		m_vecRtAT11BaseCoat.push_back(rcAT11BaseCoat);

		if (m_tInit.nCoatType == 1)
		{
			rcCoat2.left = m_rcBaseFoil.left + COAT_WIDTH * i + FOIL_WIDTH * (i + 1);
			rcCoat2.right = rcCoat2.left + COAT_WIDTH;
			dc.FillRect(rcCoat2, &brushCoat);
			m_vecRtCoat2.push_back(rcCoat2);

			if (m_tInit.bExistAT11)
			{
				rcAT11BaseCoat2.left = rcCoat2.left - AT11_WIDTH;
				rcAT11BaseCoat2.right = rcCoat2.left;
				dc.FillRect(rcAT11BaseCoat2, &brushAT11);
				m_vecRtAT11BaseCoat2.push_back(rcAT11BaseCoat2);

				rcAT11BaseCoat2.left = rcCoat2.right;
				rcAT11BaseCoat2.right = rcCoat2.right + AT11_WIDTH;
				dc.FillRect(rcAT11BaseCoat2, &brushAT11);
				m_vecRtAT11BaseCoat2.push_back(rcAT11BaseCoat2);
			}
			else if (m_tInit.bExistBaseCoat)
			{
				rcAT11BaseCoat2.left = rcCoat2.left - AT11_WIDTH;
				rcAT11BaseCoat2.right = rcCoat2.right + AT11_WIDTH;
				dc.FillRect(rcAT11BaseCoat2, &brushBaseCoat);

				rcAT11BaseCoat2.left = rcCoat2.left - AT11_WIDTH;
				rcAT11BaseCoat2.right = rcCoat2.left;
				m_vecRtAT11BaseCoat2.push_back(rcAT11BaseCoat2);

				rcAT11BaseCoat2.left = rcCoat2.right;
				rcAT11BaseCoat2.right = rcCoat2.right + AT11_WIDTH;
				m_vecRtAT11BaseCoat2.push_back(rcAT11BaseCoat2);
			}
		}
	}

	for (int i = 0; i < m_tInit.nCoatNum+1; i++)
	{
		rcFoil.left = m_rcBaseFoil.left + (COAT_WIDTH + FOIL_WIDTH) * i;
		rcFoil.right = rcFoil.left + FOIL_WIDTH;
		m_vecRtFoil.push_back(rcFoil);

		if (m_tInit.nCoatType == 1)
		{
			rcFoil2.left = m_rcBaseFoil.left + (COAT_WIDTH + FOIL_WIDTH) * i;
			rcFoil2.right = rcFoil2.left + FOIL_WIDTH;
			m_vecRtFoil2.push_back(rcFoil2);
		}
	}
}

void CCoatSketchStatic::DispDefects(CClientDC& dc)
{
	if (m_tWidth.vecDefects.size() == 0 && m_tWidth.vecDefects2.size() == 0) return;

	int Y_BASEFOIL = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2;

	int BASEFOIL_WIDTH = m_rcBaseFoil.Width();

	CRect rcDefect,rcDefect2;
	rcDefect.top = Y_BASEFOIL - 18 - 4;
	rcDefect.bottom = Y_BASEFOIL - 18 + 4;

	rcDefect2.top = Y_BASEFOIL + 18 - 4;
	rcDefect2.bottom = Y_BASEFOIL + 18 + 4;

	CBrush brushDefect;
	brushDefect.CreateSolidBrush(m_crDefect);

	CFont ftValue;
	ftValue.CreateFontA(20, 0, 0, 0, 600,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");// 

	m_pOldFont = dc.SelectObject(&ftValue);
	dc.SetBkMode(TRANSPARENT);
	//dc.SetBkColor(m_crBg);

	//正面缺陷
	for (int i = 0; i < m_tWidth.vecDefects.size(); i++)
	{
		if (i > 7) break;
		rcDefect.left = (m_tWidth.vecDefects[i].dCenterX - m_tWidth.vecDefects[i].dWidth / 2) / m_tWidth.dWholeWidth * BASEFOIL_WIDTH;
		rcDefect.right = (m_tWidth.vecDefects[i].dCenterX + m_tWidth.vecDefects[i].dWidth / 2) / m_tWidth.dWholeWidth * BASEFOIL_WIDTH;

		//画缺陷区域
		dc.FillRect(rcDefect, &brushDefect);
		//显示缺陷名称
		dc.SetTextColor(RGB(244,244,244));
		dc.SetTextAlign(TA_CENTER | TA_BOTTOM);
		dc.TextOut((rcDefect.left + rcDefect.right) / 2, Y_BASEFOIL+2, m_tWidth.vecDefects[i].strName);
	}

	//反面缺陷
	for (int i = 0; i < m_tWidth.vecDefects2.size(); i++)
	{
		if (i > 7) break;
		rcDefect2.left = (m_tWidth.vecDefects2[i].dCenterX - m_tWidth.vecDefects2[i].dWidth / 2) / m_tWidth.dWholeWidth2 * BASEFOIL_WIDTH;
		rcDefect2.right = (m_tWidth.vecDefects2[i].dCenterX + m_tWidth.vecDefects2[i].dWidth / 2) / m_tWidth.dWholeWidth2 * BASEFOIL_WIDTH;

		//画缺陷区域
		dc.FillRect(rcDefect2, &brushDefect);
		//显示缺陷名称
		dc.SetTextColor(RGB(244, 244, 244));
		dc.SetTextAlign(TA_CENTER | TA_TOP);
		dc.TextOut((rcDefect2.left + rcDefect2.right) / 2, Y_BASEFOIL-4, m_tWidth.vecDefects2[i].strName);
	}

	dc.SelectObject(m_pOldFont);
}

void CCoatSketchStatic::DispValue(CClientDC& dc)
{
	if (m_tWidth.vecCoatWidth.size() == 0 && m_tWidth.vecCoatWidth2.size() == 0) return;

	int Y_BASEFOIL = (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2;

	CFont ftValue;
	ftValue.CreateFontA(23, 0, 0, 0, 600,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");// 

	m_pOldFont = dc.SelectObject(&ftValue);

	//显示涂布值
	for (int i = 0; i < m_vecRtCoat.size(); i++)
	{
		//正面涂布宽
		CString strVal;
		if (i < m_tWidth.vecCoatWidth.size())
		{
			if (m_tWidth.vecCoatWidth[i] != NO_DATA)
				strVal.Format("%0.2f", m_tWidth.vecCoatWidth[i]);
			else
				strVal = "-";
			if (m_tWidth.vecCoatWidthFlag[i] == F_OK)
				dc.SetTextColor(m_crOK);
			else if (m_tWidth.vecCoatWidthFlag[i] == F_NG)
				dc.SetTextColor(m_crNG);
			else
				dc.SetTextColor(m_crWarn);
			dc.SetTextAlign(TA_CENTER | TA_TOP);
			dc.TextOut((m_vecRtCoat[i].left + m_vecRtCoat[i].right) / 2, Y_BASEFOIL - 60, strVal);
		}
		

		/*if (m_tWidth.vecCoatWidth2.size() == 0) continue;
		if (i > m_tWidth.vecCoatWidth2.size() - 1 ) continue;*/
		//反面涂布宽
		if (i < m_tWidth.vecCoatWidth2.size())
		{
			if (m_tInit.nCoatType == 1)
			{
				if (m_tWidth.vecCoatWidth2[i] != NO_DATA)
					strVal.Format("%0.2f", m_tWidth.vecCoatWidth2[i]);
				else
					strVal = "-";
				if (m_tWidth.vecCoatWidthFlag2[i] == F_OK)
					dc.SetTextColor(m_crOK);
				else if (m_tWidth.vecCoatWidthFlag2[i] == F_NG)
					dc.SetTextColor(m_crNG);
				else
					dc.SetTextColor(m_crWarn);
				dc.SetTextAlign(TA_CENTER | TA_BOTTOM);
				dc.TextOut((m_vecRtCoat2[i].left + m_vecRtCoat2[i].right) / 2, Y_BASEFOIL + 60, strVal);
			}
		}
	}

	//显示极耳值
	for (int i = 0; i < m_vecRtFoil.size(); i++)
	{
		CString strVal;
		//正面极耳宽
		if (i < m_tWidth.vecFoilWidth.size())
		{
			if (m_tWidth.vecFoilWidth[i] != NO_DATA)
				strVal.Format("%0.2f", m_tWidth.vecFoilWidth[i]);
			else
				strVal = "-";
			if (m_tWidth.vecFoilWidthFlag[i] == F_OK)
				dc.SetTextColor(m_crOK);
			else if (m_tWidth.vecFoilWidthFlag[i] == F_NG)
				dc.SetTextColor(m_crNG);
			else
				dc.SetTextColor(m_crWarn);
			dc.SetTextAlign(TA_CENTER | TA_TOP);
			dc.TextOut((m_vecRtFoil[i].left + m_vecRtFoil[i].right) / 2, Y_BASEFOIL - 60, strVal);
		}
		
		//反面极耳宽
		if (i < m_tWidth.vecFoilWidth2.size())
		{
			if (m_tInit.nCoatType == 1)
			{
				if (m_tWidth.vecFoilWidth2[i] != NO_DATA)
					strVal.Format("%0.2f", m_tWidth.vecFoilWidth2[i]);
				else
					strVal = "-";
				if (m_tWidth.vecFoilWidthFlag2[i] == F_OK)
					dc.SetTextColor(m_crOK);
				else if (m_tWidth.vecFoilWidthFlag2[i] == F_NG)
					dc.SetTextColor(m_crNG);
				else
					dc.SetTextColor(m_crWarn);
				dc.SetTextAlign(TA_CENTER | TA_BOTTOM);
				dc.TextOut((m_vecRtFoil2[i].left + m_vecRtFoil2[i].right) / 2, Y_BASEFOIL + 60, strVal);
			}
		}
	}

	//显示AT11/凹版
	if (m_tInit.bExistAT11 || m_tInit.bExistBaseCoat)
	{
		for (int i = 0; i < m_vecRtAT11BaseCoat.size(); i++)
		{
			CString strVal;
			//正面AT11/凹版
			if (i < m_tWidth.vecAT11BaseCoatWidth.size())
			{
				if (m_tWidth.vecAT11BaseCoatWidth[i] != NO_DATA)
					strVal.Format("%0.2f", m_tWidth.vecAT11BaseCoatWidth[i]);
				else
					strVal = "-";
				if (m_tWidth.vecAT11BaseCoatWidthFlag[i] == F_OK)
					dc.SetTextColor(m_crOK);
				else if (m_tWidth.vecAT11BaseCoatWidthFlag[i] == F_NG)
					dc.SetTextColor(m_crNG);
				else
					dc.SetTextColor(m_crWarn);

				dc.SetTextAlign(TA_CENTER | TA_TOP);
				dc.TextOut((m_vecRtAT11BaseCoat[i].left + m_vecRtAT11BaseCoat[i].right) / 2, Y_BASEFOIL - 45, strVal);
			}
			
			//反面AT11/凹版
			if (i < m_tWidth.vecAT11BaseCoatWidth2.size())
			{
				if (m_tInit.nCoatType == 1)
				{
					if (m_tWidth.vecAT11BaseCoatWidth2[i] != NO_DATA)
						strVal.Format("%0.2f", m_tWidth.vecAT11BaseCoatWidth2[i]);
					else
						strVal = "-";
					if (m_tWidth.vecAT11BaseCoatWidthFlag2[i] == F_OK)
						dc.SetTextColor(m_crOK);
					else if (m_tWidth.vecAT11BaseCoatWidthFlag2[i] == F_NG)
						dc.SetTextColor(m_crNG);
					else
						dc.SetTextColor(m_crWarn);

					dc.SetTextAlign(TA_CENTER | TA_BOTTOM);
					dc.TextOut((m_vecRtAT11BaseCoat2[i].left + m_vecRtAT11BaseCoat2[i].right) / 2, Y_BASEFOIL + 45, strVal);
				}
			}
		}
	}

	//错位值
	if (m_tInit.nCoatType == 1)
	{
		double dMs = 0;
		int nNoDataCount = 0;
		CString strVal;
		for (int i = 0; i < m_vecRtAT11BaseCoat.size(); i++)
		{
			if (m_tWidth.vecMisalignment.size() == 0) continue;
			if (i > m_tWidth.vecMisalignment.size() - 1) continue;
			
			if (m_tWidth.vecMisalignment[i] != NO_DATA)
				strVal.Format("%0.2f", m_tWidth.vecMisalignment[i]);
			else
			{
				nNoDataCount++;
				strVal = "-";
			}	
			dMs += m_tWidth.vecMisalignment[i];
			if (m_tWidth.vecMisalignmentFlag[i] ==F_OK)
				dc.SetTextColor(m_crOK);
			else if (m_tWidth.vecMisalignmentFlag[i] ==F_NG)
				dc.SetTextColor(m_crNG);
			else
				dc.SetTextColor(m_crWarn);

			if (i % 2 == 0)
				dc.SetTextAlign(TA_LEFT | TA_BOTTOM);
			else
				dc.SetTextAlign(TA_RIGHT | TA_BOTTOM);
			dc.TextOut((m_vecRtAT11BaseCoat[i].left + m_vecRtAT11BaseCoat[i].right) / 2, m_rcClient.bottom - 30, strVal);
		}

		//错位均值
		if (nNoDataCount == m_tWidth.vecMisalignment.size() || m_tWidth.vecMisalignment.size()==0)
			strVal = "-";
		else
			strVal.Format("%0.2f", (dMs - nNoDataCount*NO_DATA) / (m_tWidth.vecMisalignment.size() - nNoDataCount));
		dc.SetTextColor(m_crOK);
		dc.SetTextAlign(TA_LEFT | TA_BOTTOM);
		dc.TextOut(m_rcBaseFoil.left, m_rcClient.bottom - 30, strVal);
	}
	
	dc.SelectObject(m_pOldFont);
	//m_tWidth.Clear();
}

void CCoatSketchStatic::DispTitle(CClientDC& dc)
{
	//正反面标题
	CFont ftBigTitle;
	ftBigTitle.CreateFontA(40, 0, 0, 0, 800,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");// 
	dc.SetBkMode(OPAQUE);
	dc.SetBkColor(m_crBg);
	CString strTitle = "正面";
	/*if (m_tInit.nTitleType == 1)
		strTitle = "反面";*/
	//正面
	m_pOldFont = dc.SelectObject(&ftBigTitle);
	dc.SetTextColor(m_crBigTitle);
	dc.SetTextAlign(TA_LEFT | TA_BOTTOM);
	dc.TextOut(m_rcClient.left + 8, (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 - 40, strTitle);
	dc.SelectObject(m_pOldFont);

	//反面
	if (m_tInit.nCoatType == 1)
	{
		strTitle = "反面";
		/*if (m_tInit.nTitleType == 1)
			strTitle = "A面";*/
		m_pOldFont = dc.SelectObject(&ftBigTitle);
		dc.SetTextColor(m_crBigTitle);
		dc.SetTextAlign(TA_LEFT | TA_TOP);
		dc.TextOut(m_rcClient.left + 8, (m_rcBaseFoil.top + m_rcBaseFoil.bottom) / 2 + 40, strTitle);
		dc.SelectObject(m_pOldFont);
	}

	if (m_tInit.nCoatNum <= 0) return;
	//涂膜，极耳名称
	CFont ftValueName;
	ftValueName.CreateFontA(26, 0, 0, 0, 800,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");
	//涂膜名称
	for (int i = 0; i < m_tInit.nCoatNum; i++)
	{
		if (i > m_vecRtCoat.size() - 1) continue;
		CString strName;
		strName.Format("涂膜%d",i+1);
		m_pOldFont = dc.SelectObject(&ftValueName);
		dc.SetTextColor(m_crValueName);
		dc.SetTextAlign(TA_CENTER | TA_TOP);
		dc.TextOut((m_vecRtCoat[i].left+ m_vecRtCoat[i].right)/2, m_rcClient.top+5, strName);
		dc.SelectObject(m_pOldFont);
	}
	//极耳名称
	for (int i = 0; i < m_tInit.nCoatNum+1; i++)
	{
		if (i > m_vecRtFoil.size() - 1) continue;
		CString strName;
		strName.Format("极耳%d", i + 1);
		m_pOldFont = dc.SelectObject(&ftValueName);
		dc.SetTextColor(m_crValueName);
		dc.SetTextAlign(TA_CENTER | TA_TOP);
		dc.TextOut((m_vecRtFoil[i].left + m_vecRtFoil[i].right) / 2, m_rcClient.top+5, strName);
		dc.SelectObject(m_pOldFont);
	}
	//AT11/凹版
	/*if (m_tInit.bExistAT11)
	{
		for (int i = 0; i < m_tInit.nCoatNum*2; i++)
		{
			if (i > m_vecRtAT11BaseCoat.size() - 1) continue;
			CString strName;
			strName.Format("AT%d", i + 1);
			m_pOldFont = dc.SelectObject(&ftValueName);
			dc.SetTextColor(m_crValueName);
			if (i % 2 == 0)
				dc.SetTextAlign(TA_LEFT | TA_TOP);
			else
				dc.SetTextAlign(TA_RIGHT | TA_TOP);
			dc.TextOut((m_vecRtAT11BaseCoat[i].left + m_vecRtAT11BaseCoat[i].right) / 2, m_rcClient.top, strName);
			dc.SelectObject(m_pOldFont);
		}
	}
	else if (m_tInit.bExistBaseCoat)
	{
		for (int i = 0; i < m_tInit.nCoatNum*2; i++)
		{
			if (i > m_vecRtAT11BaseCoat.size() - 1) continue;
			CString strName;
			strName.Format("凹版%d", i + 1);
			m_pOldFont = dc.SelectObject(&ftValueName);
			dc.SetTextColor(m_crValueName);
			if (i % 2 == 0)
				dc.SetTextAlign(TA_LEFT | TA_TOP);
			else
				dc.SetTextAlign(TA_RIGHT | TA_TOP);
			dc.TextOut((m_vecRtAT11BaseCoat[i].left + m_vecRtAT11BaseCoat[i].right) / 2, m_rcClient.top, strName);
			dc.SelectObject(m_pOldFont);
		}
	}*/

	//错位
	if (m_tInit.nCoatType == 1)
	{
		for (int i = 0; i < m_tInit.nCoatNum * 2; i++)
		{
			if (i > m_vecRtAT11BaseCoat.size() - 1) continue;
			CString strName;
			strName.Format("错位%d", i + 1);
			m_pOldFont = dc.SelectObject(&ftValueName);
			dc.SetTextColor(m_crValueName);
			if (i % 2 == 0)
				dc.SetTextAlign(TA_LEFT | TA_BOTTOM);
			else
				dc.SetTextAlign(TA_RIGHT | TA_BOTTOM);
			dc.TextOut((m_vecRtAT11BaseCoat[i].left + m_vecRtAT11BaseCoat[i].right) / 2, m_rcClient.bottom-5, strName);
			dc.SelectObject(m_pOldFont);
		}
		//错位均值
		m_pOldFont = dc.SelectObject(&ftValueName);
		dc.SetTextColor(m_crValueName);
		dc.SetTextAlign(TA_LEFT | TA_BOTTOM);
		dc.TextOut(m_rcBaseFoil.left, m_rcClient.bottom-5, "纠偏差值");
		dc.SelectObject(m_pOldFont);

	}
}

BOOL CCoatSketchStatic::OnEraseBkgnd(CDC* pDC)
{
	CBrush brushBack;
	brushBack.CreateSolidBrush(m_crBg);
	pDC->SetBkColor(m_crBg);
	pDC->FillRect(m_rcClient, &brushBack);
	return TRUE;
	//CWnd::OnEraseBkgnd(pDC);
}

BOOL CCoatSketchStatic::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL result;
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);

	result = CWnd::CreateEx(WS_EX_CLIENTEDGE/* | WS_EX_STATICEDGE*/,
		className, NULL, dwStyle&(~WS_CAPTION),
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID);

	//设置边框
	ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, SWP_DRAWFRAME);
	ModifyStyleEx(WS_EX_WINDOWEDGE, NULL, SWP_DRAWFRAME);

	if (result != 0)
		InvalidateCtrl();

	return result;
}

void CCoatSketchStatic::OnNcPaint()
{
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnNcPaint()
	CWnd::OnNcPaint();

	//CDC* pDc = GetWindowDC();

	//// Paint into this DC

	//RECT rcWin;

	//GetWindowRect(&rcWin);

	//OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

	//HBRUSH hbr = CreateSolidBrush(m_crBg);
	//FrameRect(pDc->m_hDC, &rcWin, hbr);
	//InflateRect(&rcWin, -1, -1);


	//ReleaseDC(pDc);
}

void CCoatSketchStatic::OnPaint()
{
	CClientDC dc(this);
	CWnd::OnPaint();

	InvalidateCtrl();
}

void CCoatSketchStatic::ShowValue(tagWidth tValue)
{
	m_tWidth = tValue;
	Invalidate();
}