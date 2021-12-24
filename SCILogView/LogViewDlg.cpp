// LogViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCILogView.h"
#include "LogViewDlg.h"
#include "afxdialogex.h"


// CLogViewDlg 对话框

IMPLEMENT_DYNAMIC(CLogViewDlg, CBCGPDialog)

CLogViewDlg::CLogViewDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_LOGVIEWDLG, pParent)
{
	m_vecCstrLog.clear();
	m_vecCstrSearch.clear();
}

CLogViewDlg::~CLogViewDlg()
{
}

void CLogViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIME_LOG, m_wndDateTimeLog);
	DDX_Control(pDX, IDC_COMBO_LOG_LEVEL, m_COMBO_LogLevel);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_EDIT_SearchLog);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_EDIT_strSearch);
}


BEGIN_MESSAGE_MAP(CLogViewDlg, CBCGPDialog)
	ON_BN_CLICKED(IDC_DATETIME_LOG, &CLogViewDlg::OnBnClickedDatetimeLog)
	ON_CBN_SELCHANGE(IDC_COMBO_LOG_LEVEL, &CLogViewDlg::OnCbnSelchangeComboLogLevel)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CLogViewDlg::OnEnChangeEditSearch)
END_MESSAGE_MAP()


// CLogViewDlg 消息处理程序


BOOL CLogViewDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	InitUI();
	ReadLog();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLogViewDlg::InitUI()
{
	m_wndDateTimeLog.m_monthFormat = 2;

	const UINT stateMask = CBCGPDateTimeCtrl::DTM_SPIN |
		CBCGPDateTimeCtrl::DTM_DROPCALENDAR |
		CBCGPDateTimeCtrl::DTM_DATE |
		CBCGPDateTimeCtrl::DTM_TIME24H |
		CBCGPDateTimeCtrl::DTM_CHECKBOX |
		CBCGPDateTimeCtrl::DTM_TIME |
		CBCGPDateTimeCtrl::DTM_TIME24HBYLOCALE |
		CBCGPDateTimeCtrl::DTM_SECONDS;

	m_wndDateTimeLog.SetState(CBCGPDateTimeCtrl::DTM_DROPCALENDAR | CBCGPDateTimeCtrl::DTM_DATE, stateMask);
	int nSize = m_vecLogger.size();
	for (int i = 0; i < nSize;i++)
	{
		m_COMBO_LogLevel.AddString(m_vecLogger[i]->m_cstrLogName);
	}
	
	m_EDIT_SearchLog.EnableSearchMode(TRUE, _T("搜索日志"));

	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	rcGridBase.left = rcDlg.left + 10;
	rcGridBase.right = rcDlg.right - 10;
	rcGridBase.top = rcDlg.top + 65;
	rcGridBase.bottom = rcDlg.bottom - 50;
	
	m_Grid_Log.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Log, theme);
	m_Grid_Log.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Log.SetColorTheme(theme);
	m_Grid_Log.SetReadOnly();//可编辑
	m_Grid_Log.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_Log.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_Log.EnableRowHeader(TRUE);//使用行表头
	m_Grid_Log.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_Log.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_Log.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Log.InsertColumn(0, _T("日志") , rcGridBase.Width() - 30);
	m_Grid_Log.LoadState(_T("BasicGrid"));
	m_Grid_Log.AdjustLayout();//相当于数据显示在表格上
	m_Grid_Log.ShowWindow(SW_NORMAL);
	m_COMBO_LogLevel.SetCurSel(0);
	CenterWindow();
}
void CLogViewDlg::OnBnClickedDatetimeLog()
{
	ReadLog();
	
}
void CLogViewDlg::OnCbnSelchangeComboLogDisptype()
{
	ReadLog();
}
void CLogViewDlg::OnCbnSelchangeComboLogLevel()
{
	ReadLog();
}
void CLogViewDlg::OnEnChangeEditSearch()
{
	CString cstrSearch;
	m_EDIT_SearchLog.GetWindowText(cstrSearch);
	if(_T("") == cstrSearch)
	{
		UpdateList();
		return;
	}
	SearchLog(cstrSearch,m_vecCstrLog,m_vecCstrSearch);
	UpdateList(m_vecCstrSearch);
}

void CLogViewDlg::ReadLog()
{
	m_vecCstrLog.clear();
	CString cstrDay;
	COleDateTime dt = m_wndDateTimeLog.GetDate();
	cstrDay.Format("%04d-%02d-%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
	m_vecLogger[m_COMBO_LogLevel.GetCurSel()]->ReadLogFile(cstrDay, m_vecCstrLog);
	UpdateList();
}


void CLogViewDlg::UpdateList()
{
	HWND hwndList = m_Grid_Log.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);//SetWindowRedraw
	try
	{
		m_Grid_Log.RemoveAll();
		int nSize = m_vecCstrLog.size();
		if (nSize <= 0)
		{
			SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
			return;
		}
		for (int i = 0; i < nSize; i++)
		{
			CBCGPGridRow* pRow = m_Grid_Log.CreateRow(m_Grid_Log.GetColumnCount());
			pRow->GetItem(0)->SetValue(_variant_t(m_vecCstrLog[i]), FALSE);
			m_Grid_Log.InsertRowBefore(0, pRow, FALSE);
		}
	}
	catch (...)
	{
	}
	SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
	m_Grid_Log.AdjustLayout();
}

void CLogViewDlg::UpdateList(std::vector<CString>& vecCstr)
{
	HWND hwndList = m_Grid_Log.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);//SetWindowRedraw
	try
	{
		int nSize = vecCstr.size();
		if (nSize <= 0)
		{
			SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
			return;
		}
		m_Grid_Log.RemoveAll();
		for (int i = 0; i < nSize; i++)
		{
			CBCGPGridRow* pRow = m_Grid_Log.CreateRow(m_Grid_Log.GetColumnCount());
			pRow->GetItem(0)->SetValue(_variant_t(vecCstr[i]), FALSE);
			m_Grid_Log.InsertRowBefore(0, pRow, FALSE);
		}
	}
	catch (...)
	{
	}
	SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
	m_Grid_Log.AdjustLayout();
}

void CLogViewDlg::SearchLog(const CString & cstrSearch ,const std::vector<CString>& vecLogRead,std::vector<CString>& vecLogSearch)
{
	vecLogSearch.clear();
	int nSize = vecLogRead.size();
	if(nSize <= 0 || cstrSearch == _T("") || cstrSearch == _T(" "))
	{
		return;
	}
	for( std::vector<CString>::const_iterator iter = vecLogRead.begin(); iter != vecLogRead.end(); iter++)
	{
		if(-1 !=(*iter).Find(cstrSearch))
		{
			vecLogSearch.push_back(*iter);
		}
	}
	
}
void CLogViewDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}
