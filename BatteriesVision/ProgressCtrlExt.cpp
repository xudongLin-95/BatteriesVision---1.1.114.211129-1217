// GradientProgressCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressCtrlExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressCtrlExt

CProgressCtrlExt::CProgressCtrlExt()
{
	m_nLower=0;
	m_nUpper=100;
	m_nCurrentPosition=0;
	m_nStep=10;

	//Initial Color of show
	m_clrStart=	COLORREF(RGB(37,37,37));
	m_clrEnd=	COLORREF(RGB(58,58,58));
	m_clrBkGround=::GetSysColor(COLOR_3DFACE);
	m_clrBorder=COLORREF(RGB(37,37,37));
	m_clrText=COLORREF(RGB(255,255,255));
	//Not show Word
	m_bShowPercent=FALSE;
	m_bCalPercentAuto=FALSE;
	m_bUseGradient=FALSE;
	m_bUseBorder=TRUE;

	m_ftPercent.CreateFontA(17,0,0,0,500,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS
		,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,"Microsoft Yahei");
	m_strPercentShow="";
}

CProgressCtrlExt::~CProgressCtrlExt()
{
	m_ftPercent.DeleteObject();
}


BEGIN_MESSAGE_MAP(CProgressCtrlExt, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressCtrlExt)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressCtrlExt message handlers

void CProgressCtrlExt::SetRange(int nLower,int nUpper)
{
	//This Function is to Set Range of the progress 
	m_nLower=nLower;
	m_nUpper=nUpper;
	m_nCurrentPosition=nLower;
	
	CProgressCtrl::SetRange(nLower,nUpper);
}

int CProgressCtrlExt::SetStep(int nStep)
{
	m_nStep=nStep;
	return (CProgressCtrl::SetStep(nStep));
}

BOOL CProgressCtrlExt::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;//CProgressCtrl::OnEraseBkgnd(pDC);
}

void CProgressCtrlExt::DrawColor(CPaintDC *pDC,const RECT& rectClient,const int & nMaxWidth)
{
	RECT	rectFill;		//��ʾ����
	float	fStep;			//ÿһ���ķ���
	CBrush	brush;			//��ʾ����ɫ��ˢ


	::SetRect(&rectFill,
		//����Ϊ��������������Ͻ�x,y�����½�x,y
		rectClient.top,
		rectClient.left,
		rectClient.right,
		rectClient.bottom);

	VERIFY(brush.CreateSolidBrush(m_clrStart));
	pDC->FillRect(&rectFill,&brush);
	VERIFY(brush.DeleteObject());
	//�ڽ�������֮ǰ��ʹ�ñ���ɫ�����µĵĿͻ�����
	if(rectFill.right>nMaxWidth)
	{
		::SetRect(&rectFill,nMaxWidth,rectClient.top,rectClient.right,rectClient.bottom);
		VERIFY(brush.CreateSolidBrush(m_clrBkGround));
		pDC->FillRect(&rectFill,&brush);
		VERIFY(brush.DeleteObject());
		return;
	}
}

void CProgressCtrlExt::DrawGradient(CPaintDC *pDC, const RECT &rectClient, const int &nMaxWidth)
{
	RECT	rectFill;		//��ʾ����
	float	fStep;			//ÿһ���ķ���
	CBrush	brush;			//��ʾ����ɫ��ˢ

	int		r,g,b;
	float	rStep,gStep,bStep;
	//�õ���ͬ��ɫ�������������ɫ֮�������ֵ
	r=(GetRValue(m_clrEnd)-GetRValue(m_clrStart));
	g=(GetGValue(m_clrEnd)-GetGValue(m_clrStart));
	b=(GetBValue(m_clrEnd)-GetBValue(m_clrStart));
	//ʹ��������ʾ������ ����������ɫ��ֵ
	int		nSteps=max(abs(r),max(abs(g),abs(b)));
	//ȷ��ÿһ��ɫ�����ľ�������
	fStep=(float)rectClient.right/(float)nSteps;
	//����ÿһ��ɫ���Ĳ���
	rStep=r/(float)nSteps;
	gStep=g/(float)nSteps;
	bStep=b/(float)nSteps;

	r=GetRValue(m_clrStart);
	g=GetGValue(m_clrStart);
	b=GetBValue(m_clrStart);
	//������ɫ����Ľ�����
	for(int iOnBand=0;iOnBand<nSteps;iOnBand++)
	{
		::SetRect(&rectFill,
			//����Ϊ��������������Ͻ�x,y�����½�x,y
							(int)(iOnBand*fStep),
							0,
							(int)((iOnBand+1)*fStep),
							rectClient.bottom+1);
		
		VERIFY(brush.CreateSolidBrush(RGB(r+rStep*iOnBand,g+gStep*iOnBand,b+bStep*iOnBand)));
		pDC->FillRect(&rectFill,&brush);
		VERIFY(brush.DeleteObject());
		//�ڽ�������֮ǰ��ʹ�ñ���ɫ�����µĵĿͻ�����
		if(rectFill.right>nMaxWidth)
		{
			::SetRect(&rectFill,rectFill.right,0,rectClient.right,rectClient.bottom);
			VERIFY(brush.CreateSolidBrush(m_clrBkGround));
			pDC->FillRect(&rectFill,&brush);
			VERIFY(brush.DeleteObject());
			return;
		}
	}
}


void CProgressCtrlExt::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	/*if(m_nCurrentPosition<=m_nLower||m_nCurrentPosition>=m_nUpper)
	{
		CRect rect;
		GetClientRect(rect);
		CBrush brush;
		brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		dc.FillRect(&rect,&brush);
		VERIFY(brush.DeleteObject());
		return;
	}*/
	CRect rectClient;
	GetClientRect(rectClient);
	float maxWidth((float)m_nCurrentPosition/(float)m_nUpper*(float)rectClient.right);
	//����
	if(m_bUseGradient)
		DrawGradient(&dc,rectClient,(int)maxWidth);
	else
		DrawColor(&dc,rectClient,(int)maxWidth);

	//��ʾ��������������
	if(m_bShowPercent)
	{
		CString percent;
		if(m_bCalPercentAuto)
			percent.Format("%d%%",(int)(100*(float)m_nCurrentPosition/m_nUpper));
		else
			percent = m_strPercentShow;
		dc.SetTextColor(m_clrText);
		CFont *ftOld;
		ftOld = dc.SelectObject(&m_ftPercent);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(percent,&rectClient,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
		dc.SelectObject(ftOld);
	}

	// Do not call CProgressCtrl::OnPaint() for painting messages
}

int CProgressCtrlExt::SetPos(int nPos)
{
	//Set the Position of the Progress
	m_nCurrentPosition=nPos;
	return (CProgressCtrl::SetPos(nPos));
}


void CProgressCtrlExt::OnNcPaint()
{
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CProgressCtrl::OnNcPaint()
	if (!m_bUseBorder)return;
	
	CDC* pDc = GetWindowDC();
	RECT rcWin;
	GetWindowRect(&rcWin);
	OffsetRect( &rcWin, -rcWin.left, -rcWin.top );
	HBRUSH hbr = CreateSolidBrush(m_clrBorder);
	FrameRect(pDc->m_hDC,&rcWin, hbr);
	InflateRect(&rcWin, -1, -1);
	ReleaseDC(pDc);
	
}
