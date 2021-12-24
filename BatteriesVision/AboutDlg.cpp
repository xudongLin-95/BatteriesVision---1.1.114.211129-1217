#include "stdafx.h"
#include "AboutDlg.h"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

CAboutDlg::CAboutDlg() : CBCGPDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBatteriesVisionDlg 对话框
BOOL CAboutDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAboutDlg::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg;
	GetClientRect(rcDlg);
	rcGridBase.top = rcDlg.top + 80;
	rcGridBase.bottom = rcDlg.bottom - 50;
	rcGridBase.left = rcDlg.left + 12;
	rcGridBase.right = rcDlg.right / 2 - 4;
	m_Grid_VersionList.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_VersionList, theme);
	m_Grid_VersionList.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_VersionList.SetColorTheme(theme);
	m_Grid_VersionList.SetReadOnly(TRUE);//可编辑
	m_Grid_VersionList.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_VersionList.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_VersionList.EnableRowHeader(TRUE);//使用行表头
	m_Grid_VersionList.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_VersionList.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_VersionList.EnableInvertSelOnCtrl(FALSE);
	m_Grid_VersionList.InsertColumn(0, "Module", 150);
	m_Grid_VersionList.InsertColumn(1, "Version", rcGridBase.Width() - 210);
	m_Grid_VersionList.LoadState(_T("BasicGrid"));
	m_Grid_VersionList.AdjustLayout();//相当于数据显示在表格上
	m_Grid_VersionList.ShowWindow(SW_NORMAL);

	RefreshModuleVersion();

	rcGridBase.top = rcDlg.top + 10;
	rcGridBase.bottom = rcDlg.bottom - 50;
	rcGridBase.left = rcDlg.right / 2 + 4;
	rcGridBase.right = rcDlg.right - 12;
	m_EDIT_UpdateInfo.Create(WS_CHILD, rcGridBase, this, 2000);
	m_EDIT_UpdateInfo.EnableAutoOutlining();
	m_EDIT_UpdateInfo.EnableOutlining();
	m_EDIT_UpdateInfo.SetReadOnly();
	m_EDIT_UpdateInfo.ShowWindow(SW_NORMAL);

	RefreshVersionInfo();
}

void CAboutDlg::RefreshModuleVersion()
{
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextA("Version: " + theGlobal.GetVersion());


	HWND hwndList = m_Grid_VersionList.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);//SetWindowRedraw
	m_Grid_VersionList.RemoveAll();
	int nCount = 0;
	CBCGPGridRow* pRow;
	CString strTmp;

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("System Main Program"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("BatteriesVision.exe"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("Vision Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("halconxl.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("Measure Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIMeasure.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("FindMark Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIFindMark.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("GrabberDS Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIGrabberDS.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("IO Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIIO.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("PLC Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIPLC.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("Rectification Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIRectification.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("PowerLimit Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("PowerLimit.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("LightControl Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("LightControl.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("CrashRecord Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("CrashRecord.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	/*pRow = m_Grid_VersionList.CreateRow (m_Grid_VersionList.GetColumnCount());
	pRow->GetItem (0)->SetValue (_variant_t ("MES Libary"));
	pRow->GetItem (1)->SetValue (_variant_t (GetFileVersion(GetModulePath("SCIMES.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);*/


	m_Grid_VersionList.AdjustLayout();
	SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
}

void CAboutDlg::RefreshVersionInfo()
{
	CString strInfo = "\n版本更新内容:\n\n";

	strInfo += "---------------Version 1.1.114.211129---------------\n"
		"\t1.修改点检显示列表，修改点检判定列表；\n"
		"\t2.增加首件自动计算补偿功能；\n"
		"\t3.优化存储、PLC、喷码等消息入队逻辑，队列满时自动清空队列；\n\n";

	strInfo += "---------------Version 1.1.113.211125---------------\n"
		"\t1.修改点检算法，一面输出4个圆直径，3个圆心距；\n"
		"\t2.点检设置参数单独增加一页；\n"
		"\t3.点检输出文件格式、判定NG、显示逻辑重新修改；\n\n";

	strInfo += "---------------Version 1.1.111.210902---------------\n"
		"\t1.优化刀距闭环逻辑；\n"
		"\t2.增加首件公差设置；\n\n";

	strInfo += "---------------Version 1.1.109.210831---------------\n"
		"\t1.单独修改测厚仪通信为一个模块；\n"
		"\t2.优化算法寻边逻辑,优化凹版涂膜边找边；\n\n";

	strInfo += "---------------Version 1.1.107.210819---------------\n"
		"\t1.增加测厚仪通信，获取重量数据；\n"
		"\t2.优化算法寻边逻辑,优化凹版涂膜边找边；\n"
		"\t3.优化极耳和凹版找边，过滤极耳拉丝干扰；\n\n";

	strInfo += "---------------Version 1.1.105.210809---------------\n"
		"\t1.优化湿膜和凹版边测量；\n"
		"\t2.增加刀距闭环逻辑；\n\n";

	strInfo += "---------------Version 1.1.103.210729---------------\n"
		"\t1.优化算法搜索框移动,减少搜索宽度；\n"
		"\t2.优化点检逻辑；\n"
		"\t3.修改一键首件逻辑；\n\n";

	strInfo += "---------------Version 1.1.101.210723---------------\n"
		"\t1.优化算法结构，改为检测单张图；\n"
		"\t2.按照客户要求修改点检逻辑，修改点检算法为检测双圆心；\n";

	strInfo += "---------------Version 1.1.96.210717---------------\n"
		"\t1.优化闭环逻辑；\n"
		"\t2.修复日志不显示的问题；\n\n";

	strInfo += "---------------Version 1.1.94.210716---------------\n"
		"\t1.修复统计类出现的vector异常；\n"
		"\t2.优化点检和首件逻辑；\n\n";

	strInfo += "---------------Version 1.1.92.210713---------------\n"
		"\t1.增加一键首件功能；\n"
		"\t2.修复停机位统计设置；\n"
		"\t3.修改点检逻辑；\n\n";

	strInfo += "---------------Version 1.1.90.210709---------------\n"
		"\t1.增加算法连续寻边失败的报警防呆；\n"
		"\t2.修复工位之间网络连接优化；\n"
		"\t3.增加闭环连续数据异常或者料带跑偏的报警；\n"
		"\t4.修复停机位原因选择频繁弹框问题；\n\n";

	strInfo += "---------------Version 1.1.87.210701---------------\n"
		"\t1.保存湿膜数据替换为保存全部数据；\n"
		"\t2.后工位与机头报警联动；\n"
		"\t3.各个工位之间通信；\n"
		"\t4.各工位配方校验；\n\n";

	strInfo += "---------------Version 1.1.84.210627---------------\n"
		"\t1.优化喷码逻辑；\n"
		"\t2.修改示意图显示缺陷；\n"
		"\t3.优化双强开，增加连续NG报警和停机判断逻辑；\n"
		"\t4.优化动态补偿，开机可设置米数不做动态补偿；\n\n";

	strInfo += "---------------Version 1.1.81.210617---------------\n"
		"\t1.优化检测耗时，修改存储瑕疵图片；\n"
		"\t2.修改PLC交互，增加读取涂布机进退刀信号，起涂信号，模头位置，停机位数据过滤等；\n"
		"\t3.增加尺寸、瑕疵报警和停机判断逻辑；\n\n";

	strInfo += "---------------Version 1.1.71.210519---------------\n"
		"\t1.修复干湿膜补偿问题，增加正反面发送补偿值设置；\n"
		"\t2.修改PLC读取卷号写入卷号；\n\n";

	strInfo += "---------------Version 1.0.69.210419---------------\n"
		"\t1.修复CCD数据存储错位问题/文件存储问题；\n"
		"\t2.开启瑕疵双击弹窗参数界面；\n"
		"\t3.产品参数界面：新加K补偿/手动补偿/半自动补偿功能；\n\n";

	strInfo += "---------------Version 1.0.69.211217---------------\n"
		"\t1.配方中分离点检模块；\n"
		"\t2.增加数据表走带速度；\n"
		"\t3.增加膜卷号、品种号、版本号写入上传；\n\n";

	m_EDIT_UpdateInfo.SetWindowTextA(strInfo);
}

CString CAboutDlg::GetFileVersion(CString strFileName)
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

CString CAboutDlg::GetModulePath(CString strModuleName)
{
	CString strPath;
	strPath.Format("%s//%s", GetCurrentAppPath(), strModuleName);

	return strPath;
}


void CAboutDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CBCGPDialog::PostNcDestroy();
}


void CAboutDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CBCGPDialog::OnTimer(nIDEvent);
}


BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CBCGPDialog::PreTranslateMessage(pMsg);
}
