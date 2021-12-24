
// BatteriesVision.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "BatteriesVisionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBatteriesVisionApp

BEGIN_MESSAGE_MAP(CBatteriesVisionApp, CBCGPWinApp)
	ON_COMMAND(ID_HELP, &CBCGPWinApp::OnHelp)
END_MESSAGE_MAP()


// CBatteriesVisionApp 构造

CBatteriesVisionApp::CBatteriesVisionApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBatteriesVisionApp 对象

CBatteriesVisionApp theApp;


// CBatteriesVisionApp 初始化
void CBatteriesVisionApp::SetAppTheme(int nTheme)
{
	CWaitCursor wait;

	//m_nTheme = nTheme;

	WriteInt(_T("VisualManager"), nTheme);

	switch (nTheme)
	{
	case 0:
	default:
		SetVisualTheme(BCGP_VISUAL_THEME_DEFAULT);
		break;

	case 1:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2007_BLUE);
		break;

	case 2:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2007_BLACK);
		break;

	case 3:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2007_SILVER);
		break;

	case 4:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2007_AQUA);
		break;

	case 5:
		SetVisualTheme(BCGP_VISUAL_THEME_VS_2010);
		break;

	case 6:
		SetVisualTheme(BCGP_VISUAL_THEME_CARBON);
		break;

	case 7:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_BLUE);
		break;

	case 8:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_BLACK);
		break;

	case 9:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_SILVER);
		break;

	case 10:
		SetVisualTheme(BCGP_VISUAL_THEME_VS_2013_LIGHT);
		break;

	case 11:
		SetVisualTheme(BCGP_VISUAL_THEME_VS_2013_DARK);
		break;

	case 12:
		SetVisualTheme(BCGP_VISUAL_THEME_VS_2013_BLUE);
		break;

	case 13:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2013_WHITE);
		break;

	case 14:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2013_GRAY);
		break;

	case 15:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2013_DARK_GRAY);
		break;

	case 16:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2016_COLORFUL);
		break;

	case 17:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2016_DARK_GRAY);
		break;

	case 18:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2016_WHITE);
		break;

	case 19:
		SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2016_BLACK);
		break;
	}

	if (m_pMainWnd != NULL)
	{
		m_pMainWnd->SendMessage(BCGM_CHANGEVISUALMANAGER, 0, 0);
	}
}
BOOL CBatteriesVisionApp::InitInstance()
{
	//进程唯一性
	HANDLE hCurInstance = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 16, "CBatteriesVisionApp");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hCurInstance);
		AfxMessageBox("BatteriesVision is opened.\r\nBatteriesVision已经打开");
		return FALSE;
	}

	/*try
	{
		SetSystem("width", 12100);
		SetSystem("height", 12100);
		SetSystem("parallelize_operators","true");
		SetSystem("use_window_thread", "true");
	}catch(HException& e)
	{
		CString strMessageBox;
		strMessageBox.Format("Operating license has expired, please contact the supplier.\r\n运行许可已到期,请联系供应商\r\n%s  [ %s() ]",
			e.ErrorMessage().Text(),e.ProcName().Text());
		AfxMessageBox(strMessageBox);
		return FALSE;
	}*/

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CBCGPWinApp::InitInstance();

	AfxOleInit();
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	AfxEnableControlContainer();

	AfxInitRichEdit2();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	CBatteriesVisionDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CBatteriesVisionApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	GdiplusShutdown(m_gdiplusToken);
	BCGCBProCleanUp();
	//return __super::ExitInstance();

	return CBCGPWinApp::ExitInstance();
}
