// DlgLoading.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgLoading.h"
#include "afxdialogex.h"



// CDlgLoading 对话框

IMPLEMENT_DYNAMIC(CDlgLoading, CBCGPDialog)

CDlgLoading::CDlgLoading(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgLoading::IDD, pParent)
{
}

CDlgLoading::~CDlgLoading()
{
}

void CDlgLoading::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_STC_Title);
	DDX_Control(pDX, IDC_STATIC_LOAD_INFO, m_STC_LoadInfo);
	DDX_Control(pDX, IDC_PROGRESS_LOADING, m_Progress_Loading);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_STC_Version);
}


BEGIN_MESSAGE_MAP(CDlgLoading, CBCGPDialog)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
//	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CDlgLoading 消息处理程序


BOOL CDlgLoading::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgLoading::InitUI()
{
	m_STC_Title.SetBkColor(RGB(37, 37, 37));
	m_STC_Title.SetFont("Microsoft YaHei", 45, FW_NORMAL);
	m_STC_Title.SetTextColor(RGB(255,255,255));

	m_STC_LoadInfo.SetBkColor(RGB(37, 37, 37));
	m_STC_LoadInfo.SetTextColor(RGB(180,180,180));

	m_STC_Version.SetBkColor(RGB(37, 37, 37));
	m_STC_Version.SetTextColor(RGB(180, 180, 180));

	GetDlgItem(IDC_STATIC_TITLE)->SetWindowTextA(theGlobal.m_Language.IsChinese() ? "涂布视觉检测系统" : "Batteries Vision Inspection System");

	InitProgressBar();
	ShowProgramVersion();
}

void CDlgLoading::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBCGPDialog::OnPaint()
	CRect   rcDlg,rcTitle,rcClient;  
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);

	CString strLogo;
	strLogo.Format("%s\\res\\titleLogo.png",GetCurrentAppPath());
	CRect rcLogo;
	rcLogo.left = 11;
	rcLogo.top = 11;
	rcLogo.bottom = 47;
	
	CImage Pic;
	Pic.Load(strLogo);
	if (Pic.IsNull())
	{
		return;
	}
	rcLogo.right = rcLogo.left + rcLogo.Height() * Pic.GetWidth() / Pic.GetHeight();

	if ( Pic.GetBPP() == 32 ) 
	{ 
		for(int i = 0; i < Pic.GetWidth(); i++) 
		{ 
			for(int j = 0; j < Pic.GetHeight(); j++) 
			{ 
				unsigned char* pucColor = reinterpret_cast<unsigned char *>(Pic.GetPixelAddress(i , j)); 
				pucColor[0] = pucColor[0] * pucColor[3] / 255; 
				pucColor[1] = pucColor[1] * pucColor[3] / 255; 
				pucColor[2] = pucColor[2] * pucColor[3] / 255; 
			} 
		} 
	} 
	::SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);  
	::SetBrushOrgEx(dc.GetSafeHdc(), 0, 0, NULL);  
	Pic.Draw(dc.GetSafeHdc(),rcLogo);

	//画边框线
	CPen pen;
	pen.CreatePen(PS_SOLID,1,RGB(64,64,64));//154,222,106
	dc.SelectObject(pen);
	dc.MoveTo(rcDlg.left,rcDlg.top);
	dc.LineTo(rcDlg.right-1,rcDlg.top);
	dc.LineTo(rcDlg.right-1,rcDlg.bottom-1);
	dc.LineTo(rcDlg.left,rcDlg.bottom-1);
	dc.LineTo(rcDlg.left,rcDlg.top);
}

CString CDlgLoading::GetFileVersion(CString strFileName)
{
#pragma comment(lib, "Version.lib ")
	//获取当前文件的版本信息
	DWORD dwLen = GetFileVersionInfoSize(strFileName, NULL);
	char *pszAppVersion = new char[dwLen + 1];
	CString strVersion;
	if (pszAppVersion)
	{
		memset(pszAppVersion, 0, sizeof(char)*(dwLen + 1));
		GetFileVersionInfo(strFileName, NULL, dwLen, pszAppVersion);

		UINT nLen(0);
		VS_FIXEDFILEINFO *pFileInfo(NULL);
		VerQueryValue(pszAppVersion, "\\", (LPVOID*)&pFileInfo, &nLen);
		if (pFileInfo)
		{
			//获取版本号
			strVersion.Format("%d.%d.%d.%d", HIWORD(pFileInfo->dwFileVersionMS),
				LOWORD(pFileInfo->dwFileVersionMS),
				HIWORD(pFileInfo->dwFileVersionLS),
				LOWORD(pFileInfo->dwFileVersionLS));
		}
	}
	delete pszAppVersion;
	return strVersion;
}

void CDlgLoading::ShowProgramVersion()
{
	CString strFile,strVersion;
	strFile.Format("%s\\BatteriesVision.exe", GetCurrentAppPath());
	strVersion.Format("Version:  %s", theGlobal.GetVersion());
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextA(strVersion);
}

void CDlgLoading::SetLoadingInfo(const CString& strInfoCN,const CString& strInfoEN)
{
	if (CLanguage::Chinese == theGlobal.m_Language.GetLanguageType())
		m_STC_LoadInfo.SetWindowText(strInfoCN);
	else
		m_STC_LoadInfo.SetWindowText(strInfoEN);

	UpdateWindow();
}

void CDlgLoading::SetLoadingInfo(const CString& strInfoCN,const CString& strInfoEN,double dPercent)
{
	m_STC_LoadInfo.SetWindowText(theGlobal.m_Language.IsChinese() ? strInfoCN : strInfoEN);
	SetProgressPercent(dPercent);
	UpdateWindow();
}

LRESULT CDlgLoading::OnNcHitTest(CPoint point)
{
	CRect rcRgn;
	GetWindowRect(rcRgn);
	return rcRgn.PtInRect(point)
		? HTCAPTION : CBCGPDialog::OnNcHitTest(point);
}

void CDlgLoading::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CDlgLoading::InitProgressBar()
{
	CRect rcRgn,rcDlg;
	GetWindowRect(rcDlg);
	m_Progress_Loading.GetWindowRect(rcRgn);
	rcRgn.top+=3;
	rcRgn.left = rcDlg.left + 1;
	rcRgn.right = rcDlg.right - 1;
	rcRgn.bottom = rcDlg.bottom - 1;
	m_Progress_Loading.MoveWindow(rcRgn);

	m_Progress_Loading.SetRange(0,100);
	m_Progress_Loading.SetStep(1);
	m_Progress_Loading.ShowPercent(0);
	//m_Progress_Loading.SetColor(RGB(37,37,37),RGB(37,37,37),RGB(37,37,37),RGB(120,120,120),RGB(255,255,255),1);
	m_Progress_Loading.SetColor(RGB(53, 53, 57), RGB(53, 53, 57), RGB(53, 53, 57), RGB(0, 122, 204), RGB(255, 255, 255), 1);
	m_Progress_Loading.SetPos(0);
}


void CDlgLoading::SetProgressPercent(double& dPercent)
{
	m_Progress_Loading.SetPos(min(dPercent,100));
	m_Progress_Loading.StepIt();
}