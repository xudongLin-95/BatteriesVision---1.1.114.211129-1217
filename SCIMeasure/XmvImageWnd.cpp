// XmvImageWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "XmvImageWnd.h"


// CXmvImageWnd

IMPLEMENT_DYNAMIC(CXmvImageWnd, CStatic)

CXmvImageWnd::CXmvImageWnd()
{
	m_vec_hoCurrentDispRegion.clear();
	GenEmptyObj(&m_hoCurrentDispImage);
	m_hvRegionColor="red";
	m_bLeftButtonDown = FALSE;
	m_bDrawing = FALSE;
}

CXmvImageWnd::~CXmvImageWnd()
{
}


BEGIN_MESSAGE_MAP(CXmvImageWnd, CStatic)
	ON_COMMAND(IDR_ZOOM_P,OnMenuZoomP)
	ON_COMMAND(IDR_ZOOM_IN,OnMenuZoomIn)
	ON_COMMAND(IDR_ZOOM_OUT,OnMenuZoomOut)
	ON_COMMAND(IDR_ZOOM_MOVE,OnMenuZoomMove)
	ON_COMMAND(IDR_ZOOM_CENTER,OnMenuZoomCenter)
	ON_COMMAND(IDR_ZOOM_REVERT,OnMenuZoomRevert)
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CXmvImageWnd 消息处理程序

void CXmvImageWnd::SetPart(const HTuple& Row1, const HTuple& Column1, const HTuple& Row2, const HTuple& Column2)
{
	try
	{
		::SetPart(m_hDispWnd,Row1,Column1,Row2,Column2);
	}catch(HException e)
	{

	}
}
void CXmvImageWnd::DispRegion()
{
	for (int i=0;i<m_hvRegionColor.Length();i++)
	{
		SetColor(m_hDispWnd,m_hvRegionColor[i]);
		SetDraw(m_hDispWnd,m_hvRegionMode[i]);
		SetLineWidth(m_hDispWnd,m_hvLineWidth[i].I());
		HALCON_TRY(DispObj(m_vec_hoCurrentDispRegion[i],m_hDispWnd));
	}

	for (int i = 0; i < m_vec_strMsg.size(); i++)
	{
		HALCON_TRY(disp_message(m_hDispWnd, HTuple(m_vec_strMsg[i]), HTuple(m_vec_strCoord[i]), HTuple(m_vec_nY[i]), HTuple(m_vec_nX[i]), HTuple(m_vec_strMsgColor[i]), "false"));
	}
}
void CXmvImageWnd::DispImage(BOOL bAdaptive)
{
	try
	{
		HTuple hvWidth, hvHeight;
		GetImageSize(m_hoCurrentDispImage, &hvWidth, &hvHeight);
		if (TRUE == bAdaptive)
		{
			SetPart(0, 0, hvHeight - 1, hvWidth - 1);
			m_dDispImagePartRow0 = 0;
			m_dDispImagePartCol0 = 0;
			m_dDispImagePartRow1 = hvHeight - 1;
			m_dDispImagePartCol1 = hvWidth - 1;
		}
		DispObj(m_hoCurrentDispImage, m_hDispWnd);
	}
	catch (HException e)
	{

	}
}
void CXmvImageWnd::ClearImage()
{
	try
	{
		ClearWindow(m_hDispWnd);
		m_hoCurrentDispImage.GenEmptyObj();
	}catch(HException e)
	{

	}
	
}
void CXmvImageWnd::ClearRegion()
{
	try
	{
		m_hvRegionColor.Clear();
		m_hvRegionMode.Clear();
		m_hvLineWidth.Clear();
		m_vec_hoCurrentDispRegion.clear();
	}catch(HException e)
	{

	}

}

void CXmvImageWnd::ClearMessage()
{
	try
	{
		m_vec_strMsg.clear();
		m_vec_nX.clear();
		m_vec_nY.clear();
		m_vec_strMsgColor.clear();
	}
	catch (...)
	{

	}
}

void CXmvImageWnd::GetMPosition(HTuple* hvRow, HTuple* hvCol)
{
	GetMposition(m_hDispWnd, hvRow, hvCol, &HTuple());
}

void CXmvImageWnd::DispImage(const HObject& hoImage,BOOL bAdaptive)
{
	try
	{
		HTuple hvWidth,hvHeight;
		GetImageSize(hoImage,&hvWidth,&hvHeight);
		if (TRUE == bAdaptive)
		{
			SetPart(0,0,hvHeight-1,hvWidth-1);
			m_dDispImagePartRow0 = 0;
			m_dDispImagePartCol0 = 0;
			m_dDispImagePartRow1 = hvHeight - 1;
			m_dDispImagePartCol1 = hvWidth - 1;
		}
		DispObj(hoImage,m_hDispWnd);
		m_hoCurrentDispImage = hoImage;
	}catch(HException e)
	{

	}
}
void CXmvImageWnd::DispRegion(const HObject& hoRegion,const CString& strColor,const CString& strMode,const int nLineWidth,BOOL bClear)
{
	try
	{
		if (TRUE == bClear)
		{
			m_vec_hoCurrentDispRegion.clear();
			m_hvRegionColor.Clear();
			m_hvRegionMode.Clear();	
			m_hvLineWidth.Clear();
		}
		m_hvRegionColor.Append(HTuple(strColor));
		m_hvRegionMode.Append(HTuple(strMode));
		m_hvLineWidth.Append(nLineWidth);
	    m_vec_hoCurrentDispRegion.push_back(hoRegion);
		DispRegion();
	}catch(HException e)
	{

	}
}

void CXmvImageWnd::DispMessage(const CString& strMsg, const int& nRow, const int& nColumn, const CString& strColor)
{
	try
	{
		SetSystem("flush_graphic", "false");
		ClearWindow(m_hDispWnd);
		DispObj(m_hoCurrentDispImage, m_hDispWnd);
		SetSystem("flush_graphic", "true");
		
		disp_message(m_hDispWnd, HTuple(strMsg), "window", nRow, nColumn, HTuple(strColor), "false");
		
		DispRegion();
		
	}
	catch (HException e)
	{

	}
}

void CXmvImageWnd::DispMessage(const CString& strMsg, const eXmvDispLocate& eLocate, const CString& strColor)
{
	try
	{
		SetSystem("flush_graphic", "false");
		ClearWindow(m_hDispWnd);
		DispObj(m_hoCurrentDispImage, m_hDispWnd);
		SetSystem("flush_graphic", "true");
		HTuple hvR, hvC, hvW, hvH, hv_MaxAscent, hv_MaxDescent, hv_MaxWidth, hv_MaxHeight;
		GetWindowExtents(m_hDispWnd, &hvR, &hvC, &hvW, &hvH);
		GetStringExtents(m_hDispWnd, HTuple(strMsg), &hv_MaxAscent, &hv_MaxDescent, &hv_MaxWidth, &hv_MaxHeight);

		int nRow, nColumn;
		if (eLocate == XDL_LeftTop)
		{
			nRow = 2;
			nColumn = 2;
		}
		else if (eLocate == XDL_RightTop)
		{
			nRow = 2;
			nColumn = hvW - hv_MaxWidth.I() - 1;
		}
		else if (eLocate == XDL_RightButtom)
		{
			nRow = hvH - hv_MaxHeight.I() - 1;
			nColumn = hvW - hv_MaxWidth.I() - 1;
		}
		else if (eLocate == XDL_LeftButtom)
		{
			nRow = hvH - hv_MaxHeight.I() - 1;
			nColumn = 2;
		}
		else
		{
			nRow = hvH.I() / 2 - hv_MaxHeight.I() / 2;
			nColumn = hvW.I() / 2 - hv_MaxWidth.I() / 2;
		}

		disp_message(m_hDispWnd, HTuple(strMsg), "window", nRow, nColumn, HTuple(strColor), "false");
	}
	catch (HException e)
	{

	}
}

void CXmvImageWnd::DispMessage(const CString& strMsg, const int& nRow, const int& nColumn, const CString& strColor, const eDispCoordSystem& eCoord, BOOL bClear)
{
	try
	{
		if (TRUE == bClear)
		{
			m_vec_strMsg.clear();
			m_vec_nX.clear();
			m_vec_nY.clear();
			m_vec_strMsgColor.clear();
			m_vec_strCoord.clear();
		}
		m_vec_strMsg.push_back(strMsg);
		m_vec_nX.push_back(nColumn);
		m_vec_nY.push_back(nRow);
		m_vec_strMsgColor.push_back(strColor);
		m_vec_strCoord.push_back(eCoord == DCS_Image ? "image" : "window");

		DispRegion();
	}
	catch (HException& e)
	{

	}

}

HObject CXmvImageWnd::GetDispImage()
{
	return m_hoCurrentDispImage;
}
HTuple CXmvImageWnd::GetDispWnd()
{
	return m_hDispWnd;
}
void CXmvImageWnd::SetDrawing(BOOL bDrawing)
{
	m_bDrawing = bDrawing;
}
BOOL CXmvImageWnd::GetDrawing()
{
	return m_bDrawing;
}

BOOL CXmvImageWnd::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
  	m_Menu.CreatePopupMenu();//新建菜单
  	m_Menu.AppendMenu(MF_STRING, IDR_ZOOM_P, "指示");//新建子菜单的菜单项1
  	m_Menu.AppendMenu(MF_STRING, IDR_ZOOM_IN, "放大");//新建子菜单的菜单项2
  	m_Menu.AppendMenu(MF_STRING, IDR_ZOOM_OUT, "缩小");//新建子菜单的菜单项3 
  	m_Menu.AppendMenu(MF_STRING, IDR_ZOOM_MOVE, "拖动");//新建子菜单的菜单项3 
  	m_Menu.AppendMenu(MF_STRING, IDR_ZOOM_CENTER, "居中");//新建子菜单的菜单项3 
  	m_Menu.AppendMenu(MF_STRING, IDR_ZOOM_REVERT, "还原");//新建子菜单的菜单项3 
   
	
	CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);

	CRect rcClient;
	GetClientRect(rcClient);
	SetCheck("~father");
	SetWindowAttr("background_color","#46464B");
	OpenWindow(rcClient.top,rcClient.left,rcClient.Width(),rcClient.Height(),(Hlong)(m_hWnd),"visible","",&m_hDispWnd);	
	set_display_font(m_hDispWnd, 16, "mono", "true", "false");
	SetCheck("father");
	SetSystem("flush_graphic", "true");
	//disp_message(m_hDispWnd,"ImageVision","window",250,256,"red","true");
	return TRUE;
}


void CXmvImageWnd::OnMenuZoomIn()
{
	m_nCurrentMode = IDR_ZOOM_IN;
}
void CXmvImageWnd::OnMenuZoomOut()
{
	m_nCurrentMode = IDR_ZOOM_OUT;
}
void CXmvImageWnd::OnMenuZoomP()
{
	m_nCurrentMode = IDR_ZOOM_P;
}
void CXmvImageWnd::OnMenuZoomMove()
{
	m_nCurrentMode = IDR_ZOOM_MOVE;
}
void CXmvImageWnd::OnMenuZoomCenter()
{
	try
	{
		m_nCurrentMode = IDR_ZOOM_P;
		HTuple hvWidth,hvHeight;
		GetImageSize(m_hoCurrentDispImage,&hvWidth,&hvHeight);
		SetPart(0,0,hvHeight-1,hvWidth-1);
		m_dDispImagePartRow0 = 0;
		m_dDispImagePartCol0 = 0;
		m_dDispImagePartRow1 = hvHeight - 1;
		m_dDispImagePartCol1 = hvWidth - 1;
		DispObj(m_hoCurrentDispImage,m_hDispWnd);
		DispRegion();
	}catch(HException e)
	{

	}
	
}
void CXmvImageWnd::OnMenuZoomRevert()
{
	m_nCurrentMode = IDR_ZOOM_P;
}

void CXmvImageWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(FALSE == m_bDrawing)
	{
		ClientToScreen(&point);
		m_Menu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
	}
	
	CStatic::OnRButtonUp(nFlags, point);
}



BOOL CXmvImageWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(FALSE == m_bDrawing)
	{
		try
		{
			if(m_hDispWnd!=NULL && m_hoCurrentDispImage.CountObj())
			{
				CRect WndRect;
				GetClientRect(&WndRect);
				ScreenToClient(&pt);
				if (WndRect.PtInRect(pt))
				{			
					double	ImagePtX,ImagePtY;
					double	Row0_1,Col0_1,Row1_1,Col1_1;
					double	Scale(0);
					if (zDelta>0)
					{
						Scale = 0.1;	
					}
					else
					{
						Scale = -0.1;	
					}
					ImagePtX=m_dDispImagePartCol0+(pt.x-WndRect.left)/(WndRect.Width()-1.0)*(m_dDispImagePartCol1-m_dDispImagePartCol0);
					ImagePtY=m_dDispImagePartRow0+(pt.y-WndRect.top)/(WndRect.Height()-1.0)*(m_dDispImagePartRow1-m_dDispImagePartRow0);
					Row0_1=ImagePtY-1/(1-Scale)*(ImagePtY-m_dDispImagePartRow0);
					Row1_1=ImagePtY-1/(1-Scale)*(ImagePtY-m_dDispImagePartRow1);
					Col0_1=ImagePtX-1/(1-Scale)*(ImagePtX-m_dDispImagePartCol0);
					Col1_1=ImagePtX-1/(1-Scale)*(ImagePtX-m_dDispImagePartCol1);
					m_dDispImagePartRow0=Row0_1;
					m_dDispImagePartCol0=Col0_1;
					m_dDispImagePartRow1=Row1_1;
					m_dDispImagePartCol1=Col1_1;

					SetPart(m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1, m_dDispImagePartCol1);
					SetSystem("flush_graphic","false");
					ClearWindow(m_hDispWnd);
					SetSystem("flush_graphic","true");
					DispObj(m_hoCurrentDispImage, m_hDispWnd);	
					DispRegion();
				}
			}
		}catch(HException )
		{

		}
	}
	
	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CXmvImageWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MBUTTONDOWN)
	{
		if(m_hDispWnd != NULL)
		{
			CPoint   pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			CRect WndRect;
			GetClientRect(&WndRect);

			// 还原显示（不缩放）
			if (WndRect.PtInRect(pt))
			{
				HTuple tmpWidth;
				HTuple tmpHeight;

				GetImageSize(m_hoCurrentDispImage, &tmpWidth, &tmpHeight);

				m_dDispImagePartRow0 = 0;
				m_dDispImagePartCol0 = 0;
				m_dDispImagePartRow1 = tmpHeight - 1;
				m_dDispImagePartCol1 = tmpWidth - 1;
				SetPart(m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1, m_dDispImagePartCol1);

				ClearWindow(m_hDispWnd);
				DispObj(m_hoCurrentDispImage, m_hDispWnd);
			}
		}
		return TRUE;
	}
	return CStatic::PreTranslateMessage(pMsg);
}


void CXmvImageWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(FALSE == m_bDrawing)
	{
		HTuple Row(0),Column(0),Value(0),PeakGray(0);
		CString strDisp;
		try
		{
			CRect rcRect;
			GetWindowRect(&rcRect);
			ClientToScreen(&point);
			if (PtInRect(rcRect,point))
			{
				if (this != GetFocus())//如果当前未处于焦点，设置成焦点
				{
					SetFocus();	
				}
				if(IDR_ZOOM_MOVE == m_nCurrentMode && TRUE == m_bLeftButtonDown)
				{
					try
					{
						CRect rcDisplay;
						GetWindowRect(&rcDisplay);
						HTuple Width, Height;
						GetImageSize(m_hoCurrentDispImage, &Width, &Height);			
						double dZoomCoffC = rcDisplay.Width() / (m_dDispImagePartCol1 - m_dDispImagePartCol0);
						double dZoomCoffR = rcDisplay.Height() / (m_dDispImagePartRow1 - m_dDispImagePartRow0);
						double dRow0, dRow1, dCol0, dCol1;
						int iMoveX, iMoveY;
						iMoveX = m_ptBtnDown.x - point.x;
						iMoveY = m_ptBtnDown.y - point.y;
						dCol0 = m_dDispImagePartCol0 + (int)(iMoveX / dZoomCoffC + 0.5);
						dRow0 = m_dDispImagePartRow0 + (int)(iMoveY / dZoomCoffR + 0.5);
						dRow1 = dRow0 + m_dDispImagePartRow1 - m_dDispImagePartRow0;
						dCol1 = dCol0 + m_dDispImagePartCol1 - m_dDispImagePartCol0;
						SetPart(dRow0, dCol0, dRow1, dCol1);
						SetSystem("flush_graphic","false");
						ClearWindow(m_hDispWnd);
						SetSystem("flush_graphic","true");
						DispObj(m_hoCurrentDispImage, m_hDispWnd);	
						DispRegion();
					}
					catch (HException&)
					{

					}
				}
			}
		}catch(HException e)
		{
		}
	}
	
	CStatic::OnMouseMove(nFlags, point);
}


BOOL CXmvImageWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	HCURSOR hcur = LoadCursor(NULL,IDC_ARROW);
	switch (m_nCurrentMode)
	{

	case IDR_ZOOM_P:
		hcur = LoadCursor(NULL,IDC_ARROW);
		break;
	case IDR_ZOOM_IN:

		hcur = LoadCursor(NULL,IDC_ARROW);
		break;
	case IDR_ZOOM_OUT:
		hcur = LoadCursor(NULL,IDC_ARROW);
		break;
	case IDR_ZOOM_MOVE:
		hcur = LoadCursor(NULL,IDC_SIZEALL);
		break;
	case IDR_ZOOM_CENTER:
		hcur = LoadCursor(NULL,IDC_ARROW);
		break;
	case IDR_ZOOM_REVERT:
		hcur = LoadCursor(NULL,IDC_ARROW);
		break;
	}
	::SetCursor(hcur);
	return TRUE;
	
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


void CXmvImageWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(FALSE == m_bDrawing)
	{
		if (IDR_ZOOM_MOVE == m_nCurrentMode)
		{
			m_bLeftButtonDown = TRUE;
			m_ptBtnDown = point;
			ClientToScreen(&m_ptBtnDown);
		}else if (IDR_ZOOM_IN == m_nCurrentMode || IDR_ZOOM_OUT == m_nCurrentMode)//如果是放大模式点击之后，要进行放大操作
		{
			try
			{
				if(m_hDispWnd!=NULL && m_hoCurrentDispImage.CountObj())
				{
					CRect WndRect;
					GetClientRect(&WndRect);
					if (WndRect.PtInRect(point))
					{
						double	ImagePtX,ImagePtY;
						double	Row0_1,Col0_1,Row1_1,Col1_1;
						double	Scale(0);
						if (IDR_ZOOM_OUT == m_nCurrentMode)
						{
							Scale = 0.1;	
						}
						else
						{
							Scale = -0.1;	
						}
						ImagePtX=m_dDispImagePartCol0+(point.x-WndRect.left)/(WndRect.Width()-1.0)*(m_dDispImagePartCol1-m_dDispImagePartCol0);
						ImagePtY=m_dDispImagePartRow0+(point.y-WndRect.top)/(WndRect.Height()-1.0)*(m_dDispImagePartRow1-m_dDispImagePartRow0);
						Row0_1=ImagePtY-1/(1-Scale)*(ImagePtY-m_dDispImagePartRow0);
						Row1_1=ImagePtY-1/(1-Scale)*(ImagePtY-m_dDispImagePartRow1);
						Col0_1=ImagePtX-1/(1-Scale)*(ImagePtX-m_dDispImagePartCol0);
						Col1_1=ImagePtX-1/(1-Scale)*(ImagePtX-m_dDispImagePartCol1);
						m_dDispImagePartRow0=Row0_1;
						m_dDispImagePartCol0=Col0_1;
						m_dDispImagePartRow1=Row1_1;
						m_dDispImagePartCol1=Col1_1;

						SetPart(m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1, m_dDispImagePartCol1);
						SetSystem("flush_graphic","false");
						ClearWindow(m_hDispWnd);
						SetSystem("flush_graphic","true");
						DispObj(m_hoCurrentDispImage, m_hDispWnd);	
						DispRegion();
					}
				}
			}catch(HException )
			{

			}
		}
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}


void CXmvImageWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(FALSE == m_bDrawing)
	{
		if (IDR_ZOOM_MOVE == m_nCurrentMode)
		{
			if (TRUE == m_bLeftButtonDown)
			{	
				try
				{
					HTuple hvRow1, hvRow2, hvCol1, hvCol2;
					GetPart(m_hDispWnd, &hvRow1, &hvCol1, &hvRow2, &hvCol2);
					m_dDispImagePartRow0 = hvRow1.D();
					m_dDispImagePartRow1 = hvRow2.D();
					m_dDispImagePartCol0 = hvCol1.D();
					m_dDispImagePartCol1 = hvCol2.D();
				}
				catch (HException e)
				{
				}	
			}
			m_bLeftButtonDown = FALSE;
		}
	}

	
	CStatic::OnLButtonUp(nFlags, point);
}
