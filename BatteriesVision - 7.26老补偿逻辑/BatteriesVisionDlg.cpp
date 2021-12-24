
// BatteriesVisionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "BatteriesVisionDlg.h"
#include "afxdialogex.h"
#include "DlgSwitchBatch.h"
#include "DlgViewLog.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CBatteriesVisionDlg::CBatteriesVisionDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CBatteriesVisionDlg::IDD, pParent),
	m_hvecHistoryImageHandle(1),
	m_hvecTmpHandle(1),
	m_hovecHistroy(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MAIN);
	m_fontStatic.CreateFontA(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");
	m_nSubDlgDispSN = 0;
	m_ftButton.CreateFontA(21, 0, 0, 0, 500, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");
	m_ftCountGrid.CreateFontA(18, 0, 0, 0, 500, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");
}

CBatteriesVisionDlg::~CBatteriesVisionDlg()
{
	m_fontStatic.DeleteObject();
	m_ftButton.DeleteObject();
	m_ftCountGrid.DeleteObject();
}

void CBatteriesVisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE_LOGO, m_STC_Logo);
}

BEGIN_MESSAGE_MAP(CBatteriesVisionDlg, CBCGPDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CBatteriesVisionDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDCANCEL, &CBatteriesVisionDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(WM_GRABBER, &CBatteriesVisionDlg::OnGrabber)
	ON_REGISTERED_MESSAGE(WM_DISP, &CBatteriesVisionDlg::OnDisp)
	ON_REGISTERED_MESSAGE(WM_DETECT, &CBatteriesVisionDlg::OnDetect)
	ON_REGISTERED_MESSAGE(WM_RESTARTPROCESS, &CBatteriesVisionDlg::OnRestartProcess)
	ON_REGISTERED_MESSAGE(WM_CHECK_PMSN, &CBatteriesVisionDlg::OnCheckPermission)
	ON_REGISTERED_MESSAGE(WM_CHANGEBATCH, &CBatteriesVisionDlg::OnChangeBatch)
	ON_REGISTERED_MESSAGE(WM_UPDATE_PRODUCT, &CBatteriesVisionDlg::OnUpdateProduct)
	ON_REGISTERED_MESSAGE(WM_UPDATE_ONLINEOFFSET, &CBatteriesVisionDlg::OnUpdateOnlineOffset)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_LANGUAGE, &CBatteriesVisionDlg::OnBnClickedButtonLanguage)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_VIEW, &CBatteriesVisionDlg::OnBnClickedButtonSwitchView)
	ON_BN_CLICKED(IDC_BUTTON_BATCH, &CBatteriesVisionDlg::OnBnClickedButtonBatch)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CBatteriesVisionDlg::OnBnClickedButtonLogin)
	ON_COMMAND(ID_MENU_LOG, &CBatteriesVisionDlg::OnMenuLog)
	ON_COMMAND(ID_MENU_RESTART, &CBatteriesVisionDlg::OnMenuRestart)
	ON_COMMAND(ID_MENU_ABOUT, &CBatteriesVisionDlg::OnMenuAbout)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CBatteriesVisionDlg::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_LIST, &CBatteriesVisionDlg::OnBnClickedButtonList)
	ON_WM_CTLCOLOR()
	ON_REGISTERED_MESSAGE(WM_REFRESH_DISP, &CBatteriesVisionDlg::OnRefreshDisp)
	ON_COMMAND(ID_LANGUAGE_CN, &CBatteriesVisionDlg::OnLanguageCn)
	ON_COMMAND(ID_LANGUAGE_EN, &CBatteriesVisionDlg::OnLanguageEn)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_CHANGE, &CBatteriesVisionDlg::OnBnClickedButtonProductChange)
END_MESSAGE_MAP()


// CBatteriesVisionDlg ��Ϣ�������
void CBatteriesVisionDlg::InitLogo()
{
	CPaintDC   dc(GetDlgItem(IDC_PICTURE_LOGO));
	CString strLogo;
	strLogo.Format("%s\\res\\logo.png", GetCurrentAppPath());
	CRect rcLogo;
	GetDlgItem(IDC_PICTURE_LOGO)->GetClientRect(&rcLogo);
	CImage Pic;
	Pic.Load(strLogo);
	if (Pic.IsNull())
	{
		return;
	}
	if (Pic.GetBPP() == 32)
	{
		for (int i = 0; i < Pic.GetWidth(); i++)
		{
			for (int j = 0; j < Pic.GetHeight(); j++)
			{
				unsigned char* pucColor = reinterpret_cast<unsigned char *>(Pic.GetPixelAddress(i, j));
				pucColor[0] = pucColor[0] * pucColor[3] / 255;
				pucColor[1] = pucColor[1] * pucColor[3] / 255;
				pucColor[2] = pucColor[2] * pucColor[3] / 255;
			}
		}
	}
	::SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);
	::SetBrushOrgEx(dc.GetSafeHdc(), 0, 0, NULL);
	Pic.Draw(dc.GetSafeHdc(), rcLogo);
}


void CBatteriesVisionDlg::InitWindowShow()
{
	CRect rcMainPicture, rcPart;
	//GetDlgItem(IDC_PICTURE_MAIN)->GetClientRect(rcMainPicture);

	int nCamNum = theGlobal.m_GrabberTool.GetGrabberCount();
	int nGap = 2;

	SetCheck("~father");
	///////��������//
	SetWindowAttr("background_color", "#252525");//#252525

	CRect rcHistoryCtrl, rcHistory;
	GetDlgItem(IDC_PICTURE_HISTORY)->GetClientRect(rcHistoryCtrl);
	GetDlgItem(IDC_PICTURE_HISTORY)->GetClientRect(rcHistory);
	//rcHistory.right = rcHistoryCtrl.right;

	if (theGlobal.m_GrabberTool.GetGrabberCount() == 1)
	{
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[0].T());
	}
	else if (theGlobal.m_GrabberTool.GetGrabberCount() == 2)
	{
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height() / 2,
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[0].T());
		OpenWindow(rcHistory.Height() / 2 + 1, rcHistory.left, rcHistory.Width(), rcHistory.Height() / 2,
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[1].T());
	}
	else if (theGlobal.m_GrabberTool.GetGrabberCount() == 3)
	{
		rcHistory.right = (rcHistoryCtrl.right - 20) / 2;
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height() / 2 - 1,
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[0].T());
		OpenWindow(rcHistory.Height() / 2 + 1, rcHistory.left, rcHistory.Width(), rcHistory.Height() / 2,
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[1].T());
		rcHistory.left = (rcHistoryCtrl.right - 20) / 2 + 10;
		rcHistory.right = rcHistoryCtrl.right;
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height() / 2 - 1,
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[2].T());
	}
	else
	{
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height() / 2,
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[0].T());
		OpenWindow(rcHistory.Height() / 2 + 1, rcHistory.left, rcHistory.Width(), rcHistory.Height() / 2,
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[1].T());
		/*rcHistory.left = rcHistoryCtrl.left;
		rcHistory.right = rcHistoryCtrl.right / 2 - 10;
		rcHistory.bottom = rcHistoryCtrl.top + rcHistoryCtrl.Height() / 2 - 1;
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[0].T());
		rcHistory.OffsetRect(rcHistoryCtrl.Width() / 2 + 10, 0);
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[1].T());
		rcHistory.OffsetRect(-rcHistoryCtrl.Width() / 2 - 10, rcHistoryCtrl.Height() / 2 + 1);
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[2].T());
		rcHistory.OffsetRect(rcHistoryCtrl.Width() / 2 + 10, 0);
		OpenWindow(rcHistory.top, rcHistory.left, rcHistory.Width(), rcHistory.Height(),
			(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecHistoryImageHandle[3].T());*/
	}

	OpenWindow(rcHistoryCtrl.top, rcHistoryCtrl.left, rcHistoryCtrl.Width(), rcHistoryCtrl.Height(),
		(Hlong)GetDlgItem(IDC_PICTURE_HISTORY)->m_hWnd, "visible", "", &m_hvecTmpHandle[0].T());
}

void CBatteriesVisionDlg::OpenTimer()
{
	SetTimer(UM_TIMER_TIME, 1000, NULL);
	SetTimer(UM_TIMER_QUEUEINFO, 2000, NULL);
	SetTimer(UM_TIMER_STARTGRAB, 2000, NULL);
}

void CBatteriesVisionDlg::CloseTimer()
{
	KillTimer(UM_TIMER_TIME);
	KillTimer(UM_TIMER_QUEUEINFO);
}

CString CBatteriesVisionDlg::GetCurTime()
{
	CTime time = CTime::GetCurrentTime();
	CString strTime;
	if (theGlobal.m_Language.IsChinese())
	{
		strTime.Format("%d��%d��  %02d:%02d:%2d", time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	}
	else
	{
		CStringArray strArr, strSuffix;
		strArr.Add("January");
		strArr.Add("February");
		strArr.Add("March");
		strArr.Add("April");
		strArr.Add("May");
		strArr.Add("June");
		strArr.Add("July");
		strArr.Add("August");
		strArr.Add("September");
		strArr.Add("October");
		strArr.Add("November");
		strArr.Add("December");
		strSuffix.Add("st");
		strSuffix.Add("nd");
		strSuffix.Add("rd");
		CString strDay;
		strTime.Format("%s %d%s  %02d:%02d:%2d", strArr.GetAt(time.GetMonth() - 1), time.GetDay(), time.GetDay() > 3 ? "th" : strSuffix.GetAt(time.GetDay() - 1),
			time.GetHour(), time.GetMinute(), time.GetSecond());
	}

	return strTime;
}

BOOL CBatteriesVisionDlg::OnInitDialog()
{
	theGlobal.Verify();
	theGlobal.m_pMainDlg = this;
	theApp.SetAppTheme(theGlobal.GetTheme());
	CBCGPDialog::OnInitDialog();
	FullScreenTaskbarAdaptive(GetSafeHwnd());
	ShowWindow(SW_HIDE);
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	EnableVisualManagerStyle(TRUE, TRUE);
	SetWindowText("���ͼ��Ϳ���Ӿ����ϵͳ");
	ModifyStyle(0, WS_SYSMENU | WS_MINIMIZE | WS_MINIMIZEBOX);

	theGlobal.InitLoadingDlg();
	theGlobal.InitAll();
	theGlobal.SetLoadingDlgInfo("���������...", "Loading UI...", 90);
	InitUI();
	theGlobal.SetLoadingDlgInfo("��ʱ��������...", "Loading timer...", 95);
	OpenTimer();
	theGlobal.SetLoadingDlgInfo("�������.", "Loading complete.", 100);

	Sleep(200);
	theGlobal.CloseLoadingDlg();
	theGlobal.InitAlarmDlg();
	theGlobal.InitTipDlg();

	ShowWindow(SW_SHOWNORMAL);

	//AfxBeginThread((AFX_THREADPROC)T_ListenOffsetSocket, NULL);//�������Ķ˿ڴ�

	theGlobal.m_Log.Std(theGlobal.m_Language.IsChinese() ? "......�����......" : "......The program is open......");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CBatteriesVisionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CBCGPDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBatteriesVisionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBCGPDialog::OnPaint();
		CPaintDC   dc(this);
		CRect   rcCtl;

		COLORREF col;
		if (theGlobal.GetTheme() == 19)
			col = RGB(64, 64, 64);
		else if (theGlobal.GetTheme() == 18)
			col = RGB(162, 162, 162);
		else if (theGlobal.GetTheme() == 17)
			col = RGB(64, 64, 64);
		else if (theGlobal.GetTheme() == 11)
			col = RGB(120, 120, 120);
		else
			col = RGB(64, 64, 64);

		//CPen pen, *pOld;
		//pen.CreatePen(PS_SOLID, 1, col);
		//GetDlgItem(IDC_PICTURE_MAIN)->GetWindowRect(rcCtl);
		//ScreenToClient(rcCtl);
		//pOld = dc.SelectObject(&pen);
		//dc.MoveTo(rcCtl.left - 1, rcCtl.top - 1);
		//dc.LineTo(rcCtl.right + 1, rcCtl.top - 1);
		//dc.LineTo(rcCtl.right + 1, rcCtl.bottom + 1);
		////dc.LineTo(rcCtl.left-1,rcCtl.bottom+1);
		//dc.LineTo(rcCtl.left - 1, rcCtl.top - 1);
		//dc.SelectObject(pOld);

		/*GetDlgItem(IDC_PICTURE_HISTORY)->GetWindowRect(rcCtl);
		dc.FillSolidRect(rcCtl,RGB(37,37,37));*/
	}
	InitLogo();
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBatteriesVisionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBatteriesVisionDlg::InitUI()
{
	InitButton();
	InitStatusBar();
	InitSubDlg();
	InitWindowShow();

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_BATTERIESVISION_DIALOG");
}

void CBatteriesVisionDlg::InitStatusBar()
{
	static UINT indicators[] =
	{
		IDS_STRING_USER,
		IDS_STRING_GRAB,
		IDS_STRING_DETECT,
		IDS_STRING_QUEUE,
		IDS_STRING_PLC,
		IDS_STRING_IO,
		IDS_STRING_RECTIFY,
		IDS_STRING_NET,
		IDS_STRING_REMAIN_DISK,
		IDS_STRING_RAM,
		IDS_STRING_TIME,
	};
	CRect RectStatusBar;
	GetClientRect(RectStatusBar);
	RectStatusBar.top = RectStatusBar.bottom - 25; //����״̬���ľ�������

	m_wndStatusBar.Create(this);
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.SetWindowPos(&wndTop, RectStatusBar.left, RectStatusBar.top,
		RectStatusBar.Width(), RectStatusBar.Height(), SWP_NOACTIVATE);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_USER, IDS_STRING_USER, SBPS_NORMAL, RectStatusBar.Width() / 9 / 2);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_GRAB, IDS_STRING_GRAB, SBPS_NORMAL, RectStatusBar.Width() / 9 );
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_DETECT, IDS_STRING_DETECT, SBPS_NORMAL, RectStatusBar.Width() / 9);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_QUEUE, IDS_STRING_QUEUE, SBPS_NORMAL, RectStatusBar.Width() / 9 * 2);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_PLC, IDS_STRING_PLC, SBPS_NORMAL, RectStatusBar.Width() / 9 * 2 / 3 - 20);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_IO, IDS_STRING_IO, SBPS_NORMAL, RectStatusBar.Width() / 9 * 2 / 3 - 20);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_RECTIFY, IDS_STRING_RECTIFY, SBPS_NORMAL, RectStatusBar.Width() / 9 * 2 / 3 - 20);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_NET, IDS_STRING_NET, SBPS_NORMAL, RectStatusBar.Width() / 9 * 2 / 3 - 20);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_OFFSET, IDS_STRING_OFFSET, SBPS_NORMAL, RectStatusBar.Width() / 9 * 2 / 3 - 20);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_REMAIN_DISK, IDS_STRING_REMAIN_DISK, SBPS_NORMAL, RectStatusBar.Width() / 9 * 2 / 3 - 20);
	m_wndStatusBar.SetPaneInfo(STATUS_BAR_TIME, IDS_STRING_TIME, SBPS_NORMAL, RectStatusBar.Width() / 9 / 2 + 25);//SBPS_STRETCH

	CString strTime;
	m_wndStatusBar.SetPaneText(STATUS_BAR_TIME, GetCurTime());
	strTime.Format("δ��¼(%s)", theGlobal.GetCurrentJobNum());
	m_wndStatusBar.SetPaneText(STATUS_BAR_USER, strTime);
	m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_USER, RGB(85, 85, 85));
}

void CBatteriesVisionDlg::InitButton()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_START))->SetTooltip("Start inspect\r\n��ʼ���");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_LANGUAGE))->SetTooltip("Set language\r\n��������");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SWITCH_VIEW))->SetTooltip("Switch line-chart dialog\r\n�л�����ͼ");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_BATCH))->SetTooltip("Change roll NO.\r\n�������");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_LIST))->SetTooltip("Switch data list dialog\r\n�л����ݱ�");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SETTING))->SetTooltip("Switch setting dialog\r\n�л�����");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_LOGIN))->SetTooltip("Login\r\n��¼");
	((CBCGPButton*)GetDlgItem(IDCANCEL))->SetTooltip("Exit the program\r\n�˳�����");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_PRODUCT_CHANGE))->SetTooltip("Product Change\r\n�����䷽");

	((CBCGPButton*)GetDlgItem(IDC_BUTTON_START))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SWITCH_VIEW))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_START))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_LANGUAGE))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SWITCH_VIEW))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_BATCH))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_LIST))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SETTING))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_LOGIN))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDCANCEL))->SetFont(&m_ftButton);
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_PRODUCT_CHANGE))->SetFont(&m_ftButton);
}


void CBatteriesVisionDlg::OnBnClickedButtonStart()
{
	if (theGlobal.IsStart())
	{
		
		if (theGlobal.m_GrabberTool.GetGrabberType(0) != CGrabberTool::Simulate
			&& !theGlobal.m_Permission.IsLogin())
		{
			LoginOrLogout();//���µ�¼
		}

		theGlobal.m_Log.Std(theGlobal.m_Language.IsChinese() ? "......���ֹͣ......" : "......Inspection stop......");
		theGlobal.m_CommunicationTool.EnableIOCard(FALSE);
		theGlobal.m_GrabberTool.StopSimGrab();
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextA(theGlobal.m_Language.IsChinese() ? "��ʼ" : "Start");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_START))->SetTooltip("Start inspect\r\n��ʼ���");
		theGlobal.SetStart(false);
		theGlobal.m_CommunicationTool.PLCCCDEnable(FALSE);
		theGlobal.m_CommunicationTool.UploadDeviceInfoToMes();
	}
	else
	{
		theGlobal.m_Log.Std(theGlobal.m_Language.IsChinese() ? "......��⿪ʼ......" : "......Inspection start......");
		theGlobal.m_CommunicationTool.EnableIOCard();
		theGlobal.m_GrabberTool.StartGrab();
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextA(theGlobal.m_Language.IsChinese() ? "ֹͣ" : "Stop");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_START))->SetTooltip("Stop inspect\r\nֹͣ���");
		theGlobal.SetStart(true);
		theGlobal.m_CommunicationTool.PLCCCDEnable(TRUE);
		theGlobal.m_CommunicationTool.UploadDeviceInfoToMes();
	}
}


void CBatteriesVisionDlg::OnBnClickedCancel()
{
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("�Ƿ�Ҫ�˳��˳���?", "Whether to exit this program?", ""))return;

	ExitDo();
	return CBCGPDialog::OnCancel();
}

void CBatteriesVisionDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (UM_TIMER_TIME == nIDEvent)//1����һ��
	{
		m_wndStatusBar.SetPaneText(STATUS_BAR_TIME, GetCurTime());

		//if (nCount % 300 == 0)//ÿ5���Ӵ�һ��ͳ�����ݣ���ֹ��ʧ
		//	theGlobal.m_Counter.WriteParam();
	}
	else if (UM_TIMER_QUEUEINFO == nIDEvent)
	{
		RefreshStatusBar();
	}
	else if (UM_TIMER_STARTGRAB == nIDEvent)
	{
		if(theGlobal.m_GrabberTool.GetGrabberType(0) != CGrabberTool::Simulate)
			OnBnClickedButtonStart();
		KillTimer(nIDEvent);
	}
	CBCGPDialog::OnTimer(nIDEvent);
}

void CBatteriesVisionDlg::DispOutput(const HTuple& hv_DispQueue)
{
	HTuple hv_MessageRecv, hv_StopProcInfo;
	HTuple Width, Height;
	HTuple hvMessagenum;
	GetMessageQueueParam(hv_DispQueue, "message_num", &hvMessagenum);
	for (int i = 0; i < hvMessagenum.I(); i++)
	{
		DequeueMessage(hv_DispQueue, "timeout", "infinite", &hv_MessageRecv);
		GetMessageParam(hv_MessageRecv, "key_exists", "stop_processing", &hv_StopProcInfo);
		if (0 != (HTuple(hv_StopProcInfo[0])))
		{
			return;
		}

		C_MergeWork tOutput;
		tOutput.ReadMessage(hv_MessageRecv);
		ClearMessage(hv_MessageRecv);
		CString strMessage;
		//��ʾ������static
		//DispStatic(tOutput);
		//��ʷͼ��
		DispHistory(tOutput);
		//�Ӵ���
		DispSubDlg(tOutput);
		//����
		//DispCount(tOutput);
	}
}
LRESULT CBatteriesVisionDlg::OnDisp(WPARAM wParam, LPARAM lParam)
{
	HTuple hv_TimeStart, hv_TimeEnd;
	CountSeconds(&hv_TimeStart);//��ʼ��ʱ

	try
	{
		DispOutput(theGlobal.m_Process.GetDisplayQueue(0));
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("OnDisp exceptional(OnDisp�쳣):%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
	CountSeconds(&hv_TimeEnd);

	CString strTime;
	strTime.Format("OnDisp:Consume%05.03fms", (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000);
	theGlobal.m_Log.Debug(strTime);
	return 0l;
}

void CBatteriesVisionDlg::DispSubDlg(C_MergeWork& tOutput)
{
	//if (theGlobal.GetSystemSet().nActionMode > DebugMode) return;
	m_pSubDispScope->UpdateData(tOutput);
	//m_pSubDispList->UpdateData(tOutput);

	m_pSubMainDisp->UpdateData(tOutput);
}

void CBatteriesVisionDlg::DispStatic(C_MergeWork& tOutput)
{
	
}

LRESULT CBatteriesVisionDlg::OnGrabber(WPARAM wParam, LPARAM lParam)
{
	static LONGLONG nCount[4] = { 0 };
	int nGrabberSN = LOWORD(wParam);
	int nStatus = HIWORD(wParam);
	int nFrameSN = LOWORD(lParam);
	int nImageSN = HIWORD(lParam);
	nCount[nGrabberSN]++;

	CString strDisp = "[�ɼ�] ֡��:", strTmp;
	if (theGlobal.m_GrabberTool.GetGrabberCount() > 0)
	{
		strTmp.Format("%d", nCount[0]);
		strDisp += strTmp;
	}
	if (theGlobal.m_GrabberTool.GetGrabberCount() > 1)
	{
		strTmp.Format("%d", nCount[1]);
		strDisp += "-" + strTmp;
	}
	if (theGlobal.m_GrabberTool.GetGrabberCount() > 2)
	{
		strTmp.Format("%d", nCount[2]);
		strDisp += "-" + strTmp;
	}
	if (theGlobal.m_GrabberTool.GetGrabberCount() > 3)
	{
		strTmp.Format("%d", nCount[3]);
		strDisp += "-" + strTmp;
	}

	if (theGlobal.m_GrabberTool.GetGrabberCount() > 1 && abs(nCount[0] - nCount[1]) > 3)
	{
		HTuple hvTuple;
		hvTuple.Append(120);
		hvTuple.Append(9);
		CString strErrorMessage;
		strErrorMessage.Format("BatteriesVisionDlg_OnGrabber:����ɼ�ͼ��֡����һ��");
		hvTuple.Append(HTuple(strErrorMessage));
		theGlobal.m_Log.Exception(strErrorMessage);
	}

	m_wndStatusBar.SetPaneText(STATUS_BAR_GRAB, strDisp);
	return 0l;
}
LRESULT CBatteriesVisionDlg::OnChangeBatch(WPARAM wParam, LPARAM lParam)
{
	m_pSubDispScope->RefreshBatchDisp();

	return 0;
}

LRESULT CBatteriesVisionDlg::OnUpdateProduct(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CBatteriesVisionDlg::OnUpdateOnlineOffset(WPARAM wParam, LPARAM lParam)
{
	//m_pSubMainDisp->m_pSubDispSetting->m_pPageProductSet->UpdateOnlineOffset(theGlobal.m_Upload.m_hvCompensation);
	return 0l;
}


LRESULT CBatteriesVisionDlg::OnDetect(WPARAM wParam, LPARAM lParam)
{
	static int nMeasureCount[4] = { 0 };
	int nDetectInfoType = LOWORD(wParam);
	int nSideSN = HIWORD(wParam);

	nMeasureCount[nSideSN]++;

	CString strDisp = "[����] ������:", strTmp;
	if (theGlobal.m_Project.GetProjectConfig().nSideCount > 0)
	{
		strTmp.Format("%d", nMeasureCount[0]);
		strDisp += strTmp;
	}
	if (theGlobal.m_Project.GetProjectConfig().nSideCount > 1)
	{
		strTmp.Format("%d", nMeasureCount[1]);
		strDisp += "-" + strTmp;
	}

	m_wndStatusBar.SetPaneText(STATUS_BAR_DETECT, strDisp);

	return 0l;
}

LRESULT CBatteriesVisionDlg::OnRestartProcess(WPARAM wParam, LPARAM lParam)
{
	ExitDo();

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset((void*)&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	CString strApp;
	strApp.Format("%s\\BatteriesVision.exe", GetCurrentAppPath());
	CreateProcess(NULL, strApp.GetBuffer(0), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	::TerminateProcess(::GetCurrentProcess(), NULL);//��ֹԭ�ȳ���

	return 0l;
}

LRESULT CBatteriesVisionDlg::OnRefreshDisp(WPARAM wParam, LPARAM lParam)
{
	return 0l;
}

LRESULT CBatteriesVisionDlg::OnCheckPermission(WPARAM wParam, LPARAM lParam)
{
CHECK:
	//if (!theGlobal.m_Permission.IsLogin())
	//{
	//	LoginOrLogout();//���µ�¼
	//	if (!theGlobal.m_Permission.IsLogin())return FALSE;
	//}
	if (!theGlobal.m_Permission.CheckPermission((ePOWERLIMITMODULE)(int)wParam))
	{
		if (!theGlobal.m_Permission.IsLogin())
		{
			LoginOrLogout();//���µ�¼
			if (!theGlobal.m_Permission.IsLogin())return FALSE;
			else goto CHECK;
		}
		else
		{
			if (IDYES == theGlobal.m_Language.MessageboxQuestion("��ǰ�û�Ȩ�޲�����������Ƿ�ע�������µ�¼��",
				"The permission of current user is not allow. Whether to logout and login with other user again?", ""))
			{
				LoginOrLogout();//ע��
				LoginOrLogout();//���µ�¼
				if (!theGlobal.m_Permission.IsLogin())return FALSE;
				else goto CHECK;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CBatteriesVisionDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT uID = (UINT)wParam;//��ѡ������ID
	return CBCGPDialog::OnCommand(wParam, lParam);
}


BOOL CBatteriesVisionDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CBatteriesVisionDlg::OnBnClickedButtonLanguage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CRect rct;
	GetDlgItem(IDC_BUTTON_LANGUAGE)->GetWindowRect(&rct);
	CPoint point(rct.left, rct.bottom);

	CBCGPVisualManager::GetInstance()->SetMenuShadowDepth(5);
	CMenu menu;
	menu.LoadMenu(IDR_MENU_LANGUAGE);
	theApp.GetContextMenuManager()->ShowPopupMenu(menu.GetSubMenu(0)->GetSafeHmenu(), point.x, point.y, this, TRUE);
}

void CBatteriesVisionDlg::InitSubDlg()
{
	CRect rcBtn, rcSubMain;
	GetDlgItem(IDC_BUTTON_START)->GetWindowRect(rcBtn);
	GetWindowRect(rcSubMain);
	rcSubMain.top = rcBtn.bottom + 6;
	rcSubMain.bottom = rcSubMain.bottom - 33;

	m_pSubDispScope = new CSubDispScope;
	m_pSubDispScope->Create(CSubDispScope::IDD, this);
	m_pSubDispScope->MoveWindow(rcSubMain);
	m_pSubDispScope->ShowWindow(SW_NORMAL);
	m_pSubDispScope->ResetSize();

	m_pSubMainDisp = new CSubMainDisp;
	m_pSubMainDisp->Create(CSubMainDisp::IDD, this);
	m_pSubMainDisp->MoveWindow(rcSubMain);
	m_pSubMainDisp->ShowWindow(SW_HIDE);
	m_pSubMainDisp->ResetSize();
}

BOOL CBatteriesVisionDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//����ESC
			return TRUE;
		else if (pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//����ALT+F4
			return TRUE;
	}
	else if (pMsg->message == WM_LBUTTONDBLCLK)//˫��������
	{
		return LDoubleClkAlgDlg(pMsg);
	}
	//else if (pMsg->message == WM_RBUTTONDBLCLK)//˫������Ҽ�
	//{
	//	return RDoubleClkShowFindMarkDlg(pMsg);
	//}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

BOOL CBatteriesVisionDlg::RDoubleClkShowFindMarkDlg(MSG* pMsg)
{
	BOOL bRet = FALSE;
	CRect rcHistory;
	GetDlgItem(IDC_PICTURE_HISTORY)->GetWindowRect(&rcHistory);
	try
	{
		if (PtInRect(rcHistory, pMsg->pt) && m_hvecHistoryImageHandle.Length() > 3)
		{
			HTuple Button, Row, Column;
			for (int i = 3; i < m_hvecHistoryImageHandle.Length(); i++)
			{
				try
				{
					GetMposition(m_hvecHistoryImageHandle[i].T(), &Row, &Column, &Button);
					if (4 == Button)//�Ҽ�
					{
						HObject ObjectSelected;
						HObject ImageSelected;
						HTuple Inside;
						for (int j = 0; j < m_hovecHistroy[i].O().CountObj(); j++)
						{
							SelectObj(m_hovecHistroy[i].O(), &ObjectSelected, j + 1);
							TestRegionPoint(ObjectSelected, Row, Column, &Inside);
							if (1 == Inside)
							{
								SelectObj(theGlobal.m_hovecHistroyImage[i].O(), &ImageSelected, j + 1);
								theGlobal.m_AlgTool.ShowParamDlg(i - 3, CAlgorithmTool::AlgFindMark, &ImageSelected);
								bRet = TRUE;
								return bRet;
							}
						}
					}
				}
				catch (HException&) {}
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	catch (HException)
	{
		bRet = FALSE;
	}
	return bRet;
}

BOOL CBatteriesVisionDlg::LDoubleClkAlgDlg(MSG* pMsg)
{
	BOOL bRet = FALSE;
	CRect rcHistory;
	GetDlgItem(IDC_PICTURE_HISTORY)->GetWindowRect(&rcHistory);
	try
	{
		if (PtInRect(rcHistory, pMsg->pt))
		{
			HTuple Button, Row, Column;
			for (int i = 0; i < m_hvecHistoryImageHandle.Length(); i++)
			{
				try
				{
					GetMposition(m_hvecHistoryImageHandle[i].T(), &Row, &Column, &Button);
					if (1 == Button)//���
					{
						HObject ObjectSelected;
						HObject ImageSelected;
						HTuple Inside;
						for (int j = 0; j < m_hovecHistroy[i].O().CountObj(); j++)
						{
							SelectObj(m_hovecHistroy[i].O(), &ObjectSelected, j + 1);
							TestRegionPoint(ObjectSelected, Row, Column, &Inside);
							if (1 == Inside/* && TRUE == OnCheckPermission(PM_Product, NULL)*/)
							{
								MeasureInput tInput;
								tInput.nMode = DebugMode;
								GenEmptyObj(&tInput.hoGrabImage);
								HTuple hvGrabSNList = theGlobal.m_Project.GetSideToCamSNList(i);
								for (int k = 0; k < hvGrabSNList.Length(); k++)
								{
									HObject hoTmp;
									SelectObj(theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O(), &hoTmp, j + 1);
									ConcatObj(tInput.hoGrabImage, hoTmp, &tInput.hoGrabImage);
								}

								theGlobal.m_AlgTool.ShowParamDlg(i, CAlgorithmTool::AlgMeasure, &tInput);

								bRet = TRUE;
								return bRet;
							}
						}
					}
				}
				catch (HException&) {}
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	catch (HException)
	{
		bRet = FALSE;
	}
	return TRUE;

}

void CBatteriesVisionDlg::SwitchDispSubDlg()
{
	switch (m_nSubDlgDispSN)
	{
	case 0:
		m_pSubDispScope->ShowWindow(SW_NORMAL);
		m_pSubMainDisp->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pSubDispScope->ShowWindow(SW_HIDE);
		m_pSubMainDisp->ShowWindow(SW_NORMAL);
		m_pSubMainDisp->SwitchDispSubDlg(0);
		break;
	case 2:
		m_pSubDispScope->ShowWindow(SW_HIDE);
		m_pSubMainDisp->ShowWindow(SW_NORMAL);
		m_pSubMainDisp->SwitchDispSubDlg(1);
		break;
	default:
		m_pSubDispScope->ShowWindow(SW_NORMAL);
		m_pSubMainDisp->ShowWindow(SW_HIDE);
		break;
	}
}


void CBatteriesVisionDlg::OnBnClickedButtonSwitchView()
{
	m_nSubDlgDispSN = 0;
	SwitchDispSubDlg();
}

void CBatteriesVisionDlg::RefreshStatusBar()
{
	try
	{
		CString strInfo = "[����] ͼ��:", strTmp;
		HTuple Tmp;
		if (theGlobal.m_GrabberTool.GetGrabberCount() > 0)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetImageQueue(0), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += strTmp;
		}
		if (theGlobal.m_GrabberTool.GetGrabberCount() > 1)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetImageQueue(1), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += "-" + strTmp;
		}
		if (theGlobal.m_GrabberTool.GetGrabberCount() > 2)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetImageQueue(2), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += "-" + strTmp;
		}
		if (theGlobal.m_GrabberTool.GetGrabberCount() > 3)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetImageQueue(3), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += "-" + strTmp;
		}
		strInfo += " ���:";

		if (theGlobal.m_Project.GetProjectConfig().nSideCount > 0)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetResultQueue(0), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += strTmp;
		}
		if (theGlobal.m_Project.GetProjectConfig().nSideCount > 1)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetResultQueue(1), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += "-" + strTmp;
		}
		/*if (theGlobal.m_GrabberTool.GetGrabberCount() > 2)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetResultQueue(2), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += "-" + strTmp;
		}
		if (theGlobal.m_GrabberTool.GetGrabberCount() > 3)
		{
			GetMessageQueueParam(theGlobal.m_Process.GetResultQueue(3), "message_num", &Tmp);
			strTmp.Format("%d", Tmp.I());
			strInfo += "-" + strTmp;
		}*/
		strInfo += " ��ʾ:";
		GetMessageQueueParam(theGlobal.m_Process.GetDisplayQueue(0), "message_num", &Tmp);
		strTmp.Format("%d", Tmp.I());
		strInfo += strTmp;

		strInfo += " �洢:";
		GetMessageQueueParam(theGlobal.m_Storage.GetStorageQueue(0), "message_num", &Tmp);
		strTmp.Format("%d", Tmp.I());
		strInfo += strTmp;

		strInfo += " IO:";
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::IO, "GetParam", "message_num", &Tmp);
		strTmp.Format("%d", Tmp.I());
		strInfo += strTmp;

		m_wndStatusBar.SetPaneText(STATUS_BAR_QUEUE, strInfo);

		COLORREF clNomal(RGB(85, 85, 85)), clExcp(RGB(248, 182, 43)), clGreen(RGB(132, 228, 43));

		int nConnected = FALSE;
		int nConnected2 = FALSE;
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetConnection", NULL, &nConnected);
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetCorrectingEnable", NULL, &nConnected2);
		strInfo.Format("[PLC] %s [�ջ�] %s", (nConnected == TRUE ? "����" : "�쳣"), (nConnected2 == TRUE ? "����" : "�쳣"));
		m_wndStatusBar.SetPaneText(STATUS_BAR_PLC, strInfo);
		m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_PLC, (nConnected&&nConnected2) == TRUE ? clNomal : clExcp);

		nConnected = FALSE;
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::IO, "GetConnection", NULL, &nConnected);
		strInfo.Format("[IO] %s", (nConnected == TRUE ? "����" : "�쳣"));
		m_wndStatusBar.SetPaneText(STATUS_BAR_IO, strInfo);
		m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_IO, nConnected == TRUE ? clNomal : clExcp);

		nConnected = FALSE;
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Marking, "GetConnection", NULL, &nConnected);
		strInfo.Format("[����] %s", nConnected == TRUE ? "����" : "�쳣");
		m_wndStatusBar.SetPaneText(STATUS_BAR_RECTIFY, strInfo);
		m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_RECTIFY, nConnected == TRUE ? clNomal : clExcp);

		//��������
		nConnected = theGlobal.m_Upload.IsNetConnect();
		strInfo.Format("[����] %s", nConnected == TRUE ? "����" : "�쳣");
		m_wndStatusBar.SetPaneText(STATUS_BAR_NET, strInfo);
		m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_NET, nConnected == TRUE ? clNomal : clExcp);

		//��ʪĤ״̬
		nConnected = theGlobal.m_Upload.m_tParam.bUseCompensationAuto;
		strInfo.Format("[��ʪĤ����] %s", nConnected == TRUE ? "��̬" : "��̬");
		m_wndStatusBar.SetPaneText(STATUS_BAR_OFFSET, strInfo);
		m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_OFFSET, nConnected == TRUE ? clGreen : clNomal);

		//Ӳ��ʣ��
		nConnected = TRUE;
		double dRemain = theGlobal.m_Storage.GetRemainDiskGB();
		if (dRemain < theGlobal.m_Storage.GetMinRemainStorageGB()) nConnected = FALSE;
		strInfo.Format("[Ӳ��] ʣ��:%0.3fGB", dRemain);
		m_wndStatusBar.SetPaneText(STATUS_BAR_REMAIN_DISK, strInfo);
		m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_REMAIN_DISK, nConnected == TRUE ? clNomal : clExcp);
	}
	catch (HException& e) {}

	try
	{
		if (theGlobal.m_Permission.GetUserInfo().bLogin  && theGlobal.m_Permission.GetUserInfo().ePowerType > PT_OPERATOR)
		{
			SYSTEMTIME nowSysTime;
			GetLocalTime(&nowSysTime);
			if (GetDiffSeconds(theGlobal.m_Permission.GetUserInfo().tLoginTime, nowSysTime) > 300)//��ʱû����
			{
				theGlobal.m_Permission.Logout();

				if (!theGlobal.m_Permission.IsLogin())
				{
					GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowTextA(theGlobal.m_Language.IsChinese() ? "��¼" : "Login");
					((CBCGPButton*)GetDlgItem(IDC_BUTTON_LOGIN))->SetTooltip("Login\r\n��¼");
				}
				else
				{
					GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowTextA(theGlobal.m_Language.IsChinese() ? "ע��" : "Logout");
					((CBCGPButton*)GetDlgItem(IDC_BUTTON_LOGIN))->SetTooltip("Logout\r\nע��");
				}
				COLORREF col(RGB(0, 255, 0));
				CString strInfo;

				strInfo.Format("δ��¼");
				col = RGB(85, 85, 85);
				m_wndStatusBar.SetPaneText(STATUS_BAR_USER, strInfo);
				m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_USER, col);

				if (!theGlobal.m_Permission.IsLogin())
					OnBnClickedButtonSwitchView();
			}
		}
	}
	catch (const std::exception&)
	{
	}
	catch (HException& e)
	{
	}
	catch (...)
	{
	}

}

time_t CBatteriesVisionDlg::FileTimeToTime_t(const FILETIME &ft)
{
	
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

int CBatteriesVisionDlg::GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2)
{
	FILETIME fTime1 = { 0, 0 };
	FILETIME fTime2 = { 0, 0 };
	SystemTimeToFileTime(&t1, &fTime1);
	SystemTimeToFileTime(&t2, &fTime2);

	time_t tt1 = FileTimeToTime_t(fTime1);
	time_t tt2 = FileTimeToTime_t(fTime2);

	return (int)(tt2 - tt1);
}

void CBatteriesVisionDlg::DispHistory(C_MergeWork& tOutput)
{
	HTuple hvHandle;
	const int HISTORT_COUNT = theGlobal.GetHistoryCount();
	const int HISTORY_TYPE = theGlobal.GetHistoryType();
	const int REC_WIDTH = 80;
	HTuple hvGrabberSNList;
	for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
	{
		//ȥ���Ǳ���ѭ��
		B_DetectWork* pDetectOutput = &tOutput.vecB[nBWorkSN];
		if (NULL == pDetectOutput)
			continue;

		hvHandle = m_hvecHistoryImageHandle[nBWorkSN].T();
		SetPart(hvHandle, 0, 0, 100, HISTORT_COUNT * 100);

		HObject Rectangle;
		if (HISTORY_TYPE != CGlobal::NoHistory)
		{
			if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_Flag == NG)
				GenRectangle1(&Rectangle, 16, (HISTORT_COUNT + 1) * 100 - REC_WIDTH, 84, (HISTORT_COUNT + 1) * 100);//NG
			else if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_Flag == WARN)
				GenRectangle1(&Rectangle, 16, (HISTORT_COUNT + 1) * 100 - REC_WIDTH, 83, (HISTORT_COUNT + 1) * 100);//Ԥ��
			else
				GenRectangle1(&Rectangle, 16, (HISTORT_COUNT + 1) * 100 - REC_WIDTH, 85, (HISTORT_COUNT + 1) * 100);//OK
			if ((HISTORY_TYPE == CGlobal::ErrorHistory) || HISTORY_TYPE == CGlobal::AllHistory)//ȫ���������󣬻����ǲ��ֱ���
			{
				for (int nAWorkSN = 0; nAWorkSN < pDetectOutput->vecA.size(); nAWorkSN++)
				{
					ConcatObj(theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O(), pDetectOutput->vecA[nAWorkSN].hoGrabImage, &theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O());
				}
				
				//ÿ�������ֻ��һ����������ʾ
				{
					ConcatObj(m_hovecHistroy[nBWorkSN].O(), Rectangle, &m_hovecHistroy[nBWorkSN].O());
					if (m_hovecHistroy[nBWorkSN].O().CountObj() == HISTORT_COUNT + 1)
					{
						HObject TempRegion; TempRegion.GenEmptyObj();
						for (int i = 2; i <= m_hovecHistroy[nBWorkSN].O().CountObj(); i++)
						{
							ConcatObj(TempRegion, m_hovecHistroy[nBWorkSN].O().SelectObj(i), &TempRegion);
						}
						m_hovecHistroy[nBWorkSN].O().GenEmptyObj();
						ConcatObj(m_hovecHistroy[nBWorkSN].O(), TempRegion, &m_hovecHistroy[nBWorkSN].O());

						//image
						for (int nAWorkSN = 0; nAWorkSN < pDetectOutput->vecA.size(); nAWorkSN++)
						{
							HObject TempImage; TempImage.GenEmptyObj();
							for (int i = 2; i <= theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O().CountObj(); i++)
							{
								ConcatObj(TempImage, theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O().SelectObj(i), &TempImage);
							}
							theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O().GenEmptyObj();
							ConcatObj(theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O(), TempImage, 
								&theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O());
						}
					}
					else if (m_hovecHistroy[nBWorkSN].O().CountObj() > HISTORT_COUNT + 1)//��������Ѿ�����
					{
						m_hovecHistroy[nBWorkSN].O().GenEmptyObj();
						for (int nAWorkSN = 0; nAWorkSN < pDetectOutput->vecA.size(); nAWorkSN++)
						{
							theGlobal.m_hovecHistroyImage[pDetectOutput->vecA[nAWorkSN].hvGrabberSN.I()].O().GenEmptyObj();
						}
					}
					SetSystem("flush_graphic", "false");
					ClearWindow(hvHandle);

					MoveRegion(m_hovecHistroy[nBWorkSN].O(), &m_hovecHistroy[nBWorkSN].O(), 0, -100);//�޸�2016.5.18

					for (int i = 0; i < m_hovecHistroy[nBWorkSN].O().CountObj(); i++)
					{
						HTuple Row1, Row2, Column1, Column2;
						SmallestRectangle1(m_hovecHistroy[nBWorkSN].O().SelectObj(i + 1), &Row1, &Column1, &Row2, &Column2);

						if (Row2 - Row1 == 99 || Row2 - Row1 == 69)//OK
						{
							SetColor(hvHandle, "green");
						}
						else if (Row2 - Row1 == 98 || Row2 - Row1 == 68)//NG
						{
							SetColor(hvHandle, "red");
						}
						else//Ԥ��
						{
							SetColor(hvHandle, "#f8b62b");
						}

						DispObj(m_hovecHistroy[nBWorkSN].O().SelectObj(i + 1), hvHandle);
					}
					SetSystem("flush_graphic", "true");
					DispPolygon(hvHandle, -1, -1);//��ֹ��˸
				}
			}
		}
	}
}

void CBatteriesVisionDlg::DispCount(C_MergeWork& tOutput)
{
	//RefreshCountStatic();
	// 	if (tOutput.tResults.tCurrentResult[0].hv_HasMark.I()
	// 		|| tOutput.tResults.tCurrentResult[1].hv_HasMark.I())
	// 	{
	// 		RefreshErrorCountList();
	// 		RefreshCountList();
	// 	}
}

void CBatteriesVisionDlg::OnBnClickedButtonBatch()
{
	CDlgSwitchBatch dlg;
	if (IDOK == dlg.DoModal())
	{
		//RefreshCountStatic();
	}
}

void CBatteriesVisionDlg::OnBnClickedButtonLogin()
{
	LoginOrLogout();
	if (!theGlobal.m_Permission.IsLogin())
		OnBnClickedButtonSwitchView();
}

void CBatteriesVisionDlg::LoginOrLogout()
{
	if (!theGlobal.m_Permission.IsLogin())
	{
		/*CDlgLogin dlg;
		if (IDOK != dlg.DoModal())*/
		if(!theGlobal.m_Permission.Login())
			OnBnClickedButtonSwitchView();
	}
	else
		theGlobal.m_Permission.Logout();

	if (!theGlobal.m_Permission.IsLogin())
	{
		GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowTextA(theGlobal.m_Language.IsChinese() ? "��¼" : "Login");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_LOGIN))->SetTooltip("Login\r\n��¼");
	}
	else
	{
		GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowTextA(theGlobal.m_Language.IsChinese() ? "ע��" : "Logout");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_LOGIN))->SetTooltip("Logout\r\nע��");
	}

	ePOWERLIMITTYPE ePmsn = theGlobal.m_Permission.GetPermission();
	COLORREF col(RGB(0, 255, 0));
	CString strInfo;
	if (PT_OPERATOR == ePmsn)
		strInfo.Format("����Ա(%s)", theGlobal.GetCurrentJobNum());
	else if (PT_TECHNICIAN == ePmsn)
		strInfo.Format("����Ա(%s)", theGlobal.GetCurrentJobNum());
	else if (PT_ENGINEER == ePmsn)
		strInfo.Format("����ʦ(%s)", theGlobal.GetCurrentJobNum());
	else if (PT_ADMINISTRATOR == ePmsn)
		strInfo.Format("����Ա(%s)", theGlobal.GetCurrentJobNum());
	else if (PT_SUPER_ADMINISTRATOR == ePmsn)
		strInfo.Format("��������Ա(%s)", theGlobal.GetCurrentJobNum());
	else if (PT_FACTORY == ePmsn)
	{
		col = RGB(255, 0, 0);
		strInfo.Format("����");
	}
	else  if (PT_NONE == ePmsn)
	{
		strInfo.Format("δ��¼");
		col = RGB(85, 85, 85);
	}
	else
	{
		return;
	}
	m_wndStatusBar.SetPaneText(STATUS_BAR_USER, strInfo);
	m_wndStatusBar.SetPaneBackgroundColor(STATUS_BAR_USER, col);
}

void CBatteriesVisionDlg::OnMenuLog()
{
	// TODO: �ڴ���������������
	CDlgViewLog dlg;
	dlg.DoModal();
}

void CBatteriesVisionDlg::ExitDo()
{
	theGlobal.m_bExitThread = TRUE;
	theGlobal.m_CommunicationTool.EnableIOCard(FALSE);
	CloseTimer();
	theGlobal.m_GrabberTool.CloseGrab();
	theGlobal.CloseAlarmDlg();
	theGlobal.CloseTipDlg();
	theGlobal.WriteParamAll();
	theGlobal.m_Log.Std(theGlobal.m_Language.IsChinese() ? "......����ر�......" : "......Program exits......");
}

void CBatteriesVisionDlg::OnMenuRestart()
{
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("�Ƿ������˳���?", "Whether to restart this program?", ""))return;

	OnRestartProcess(NULL, NULL);
}

void CBatteriesVisionDlg::OnMenuAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


void CBatteriesVisionDlg::OnBnClickedButtonSetting()
{
	//δ��¼���ܽ������ý���
	/*if (PT_NONE == theGlobal.m_Permission.GetPermission())
	{
		BCGPMessageBox("���ȵ�¼!");
		return;
	}*/
	if (!theGlobal.m_Permission.IsLogin())
	{
		LoginOrLogout();//���µ�¼
		if (!theGlobal.m_Permission.IsLogin())return;
	}
	//��ʱ���ε�
	//if (TRUE != OnCheckPermission(PM_Detect, NULL) || TRUE != OnCheckPermission(PM_Product, NULL))return;
	m_nSubDlgDispSN = 2;
	SwitchDispSubDlg();
}


void CBatteriesVisionDlg::OnBnClickedButtonList()
{
	m_nSubDlgDispSN = 1;
	SwitchDispSubDlg();
}


HBRUSH CBatteriesVisionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void CBatteriesVisionDlg::OnLanguageCn()
{
	// TODO: �ڴ���������������
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("�Ƿ����Ϊ����(Chinese)?", "Whether to change to Chinese language?", ""))return;

	theGlobal.m_Language.SetLanguage(CLanguage::Chinese);
	OnRestartProcess(NULL, NULL);
}


void CBatteriesVisionDlg::OnLanguageEn()
{
	// TODO: �ڴ���������������
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("�Ƿ����ΪӢ��(English)?", "Whether to change to English language?", ""))return;
	theGlobal.m_Language.SetLanguage(CLanguage::English);
	OnRestartProcess(NULL, NULL);
}


void CBatteriesVisionDlg::OnBnClickedButtonProductChange()
{
	theGlobal.m_Language.MessageboxWarn("��δ����", "", "");
}
