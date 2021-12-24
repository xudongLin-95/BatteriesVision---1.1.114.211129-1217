// DlgViewLog.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgViewLog.h"
#include "afxdialogex.h"


// CDlgViewLog 对话框

IMPLEMENT_DYNAMIC(CDlgViewLog, CBCGPDialog)

CDlgViewLog::CDlgViewLog(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgViewLog::IDD, pParent)
	, m_EDIT_strSearch(_T(""))
{

}

CDlgViewLog::~CDlgViewLog()
{
}

void CDlgViewLog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIME_LOG, m_wndDateTimeLog);
	DDX_Control(pDX, IDC_COMBO_LOG_LEVEL, m_COMBO_LogLevel);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_EDIT_SearchLog);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_EDIT_strSearch);
}


BEGIN_MESSAGE_MAP(CDlgViewLog, CBCGPDialog)
	ON_BN_CLICKED(IDC_DATETIME_LOG, &CDlgViewLog::OnBnClickedDatetimeLog)
	ON_CBN_SELCHANGE(IDC_COMBO_LOG_LEVEL, &CDlgViewLog::OnCbnSelchangeComboLogLevel)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CDlgViewLog::OnEnChangeEditSearch)
END_MESSAGE_MAP()


// CDlgViewLog 消息处理程序


BOOL CDlgViewLog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	InitUI();
	ReadLogFile();
	RefreshLogList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgViewLog::InitUI()
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

	m_wndDateTimeLog.SetState(CBCGPDateTimeCtrl::DTM_DROPCALENDAR | CBCGPDateTimeCtrl::DTM_DATE,stateMask);

	m_COMBO_LogLevel.AddString("运行日志");
	m_COMBO_LogLevel.AddString("操作日志");
	m_COMBO_LogLevel.AddString("异常日志");
	m_COMBO_LogLevel.AddString("调试日志");
	m_COMBO_LogLevel.AddString("所有日志");
	

	m_EDIT_SearchLog.EnableSearchMode(TRUE,theGlobal.m_Language.IsChinese()?"搜索日志":"Search Log");

	CBCGPGridColors theme;
	CRect rcGridBase;
	rcGridBase.top = 41;
	rcGridBase.bottom = 488;
	rcGridBase.left = 11;
	rcGridBase.right = 589;
	m_Grid_Log.Create (WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme(&m_Grid_Log, theme);
	m_Grid_Log.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Log.SetColorTheme(theme);
	m_Grid_Log.SetReadOnly();//可编辑
	m_Grid_Log.EnableMarkSortedColumn (FALSE);//使用列排序
	m_Grid_Log.EnableHeader (TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_Log.EnableRowHeader (TRUE);//使用行表头
	m_Grid_Log.EnableLineNumbers (TRUE);//使用行序号
	m_Grid_Log.SetClearInplaceEditOnEnter (FALSE);
	m_Grid_Log.EnableInvertSelOnCtrl (FALSE);
	m_Grid_Log.InsertColumn (0, theGlobal.m_Language.IsChinese() ? "日志":"Logs", rcGridBase.Width()-60);
	m_Grid_Log.LoadState (_T("BasicGrid"));
	m_Grid_Log.AdjustLayout ();//相当于数据显示在表格上
	m_Grid_Log.ShowWindow(SW_NORMAL);

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_DIALOG_VIEW_LOG");
	m_COMBO_LogLevel.SetCurSel(4);
}

void CDlgViewLog::OnBnClickedDatetimeLog()
{
	ReadLogFile();
	RefreshLogList();
}

void CDlgViewLog::ReadLogFile()
{
	m_hvLog.Clear();
	m_hvLogLevel.Clear();
	COleDateTime dt = m_wndDateTimeLog.GetDate();

	CString strLogName;
	strLogName.Format("%d-%d-%d",dt.GetYear(),dt.GetMonth(),dt.GetDay());

	theGlobal.m_Log.ReadLogFile(strLogName,&m_hvLog,&m_hvLogLevel);
}

void CDlgViewLog::RefreshLogList()
{
	HWND hwndList = m_Grid_Log.GetSafeHwnd();
	SNDMSG(hwndList,WM_SETREDRAW,FALSE,0);//SetWindowRedraw
	int nLevelSel = m_COMBO_LogLevel.GetCurSel();

	m_Grid_Log.RemoveAll();
	if(m_hvLog.Length()==0)
	{
		SNDMSG(hwndList,WM_SETREDRAW,TRUE,0);
		return;
	}

	HTuple hvDspLog,hvDspLogLevel,Inc,Pos,Greater;
	try
	{
		if(nLevelSel!=4)
		{
			TupleFind(m_hvLogLevel,nLevelSel,&Inc);
			if (Inc!=-1)
			{
				TupleSelect(m_hvLog,Inc,&hvDspLog);
				TupleSelect(m_hvLogLevel,Inc,&hvDspLogLevel);
			}
		}
		else
		{
			hvDspLog=m_hvLog;
			hvDspLogLevel=m_hvLogLevel;
		}
		if(m_EDIT_strSearch!="")
		{
			TupleStrstr(hvDspLog,HTuple(m_EDIT_strSearch),&Pos);
			TupleGreaterEqualElem(Pos,0,&Greater);
			TupleFind(Greater,1,&Inc);
			if (Inc!=-1)
			{
				TupleSelect(hvDspLog,Inc,&hvDspLog);
				TupleSelect(hvDspLogLevel,Inc,&hvDspLogLevel);
			}
			else
			{
				hvDspLog.Clear();
				hvDspLogLevel.Clear();
			}
		}
		if(hvDspLogLevel.Length()!=hvDspLog.Length())
		{
			theGlobal.m_Language.MessageboxError("读取日志异常，日志类型和等级无法对应。", "Read log failed, log type and level cannot correspond.", "");
			return;
		}

		COLORREF clTxt[4]={RGB(255,255,255),RGB(0,103,173),RGB(255,191,0),RGB(37,218,111)};
		for (int i=0;i<hvDspLogLevel.Length();i++)
		{
			CBCGPGridRow* pRow = m_Grid_Log.CreateRow (m_Grid_Log.GetColumnCount());
			pRow->GetItem (0)->SetValue (_variant_t (hvDspLog[i].S().Text()),FALSE);
			pRow->GetItem(0)->SetTextColor(clTxt[hvDspLogLevel[i].I()],FALSE);
			if(i%2==1)
				pRow->GetItem(0)->SetBackgroundColor(RGB(58,58,58),FALSE);

			m_Grid_Log.InsertRowBefore(0,pRow, FALSE);
		}
		SNDMSG(hwndList,WM_SETREDRAW,TRUE,0);
		m_Grid_Log.AdjustLayout ();
	}
	catch(CFileException&)
	{

	}
}


void CDlgViewLog::OnCbnSelchangeComboLogLevel()
{
	RefreshLogList();
}


void CDlgViewLog::OnEnChangeEditSearch()
{
	UpdateData(TRUE);
	RefreshLogList();
}
