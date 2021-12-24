// LoggerManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCILogView.h"
#include "LoggerManagerDlg.h"
#include "afxdialogex.h"


// CLoggerManagerDlg 对话框

IMPLEMENT_DYNAMIC(CLoggerManagerDlg, CBCGPDialog)

CLoggerManagerDlg::CLoggerManagerDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_LOGGERMANAGERDLG, pParent)
{
	m_ftProp.CreateFontA(17, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");
}

CLoggerManagerDlg::~CLoggerManagerDlg()
{
	m_ftProp.DeleteObject();
}

void CLoggerManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_STATIC_PROPLIST, m_Static_PropList);
}


BEGIN_MESSAGE_MAP(CLoggerManagerDlg, CBCGPDialog)
//	ON_WM_CLOSE()
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BUTTON_VIEWLOG, &CLoggerManagerDlg::OnBnClickedButtonViewlog)
END_MESSAGE_MAP()


// CLoggerManagerDlg 消息处理程序


BOOL CLoggerManagerDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	
	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);

	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLoggerManagerDlg::InitUI()
{
	CRect rectPropList, rcDlg;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	rectPropList.left = rcDlg.left + 15;
	rectPropList.right = rcDlg.right - 15;
	rectPropList.top = rcDlg.top + 35;
	rectPropList.bottom = rcDlg.bottom - 50;
//	m_Static_PropList.MapWindowPoints(this, rectPropList);
	m_PropList.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectPropList, this, 2000);
	m_PropList.EnableToolBar(TRUE);//设置工具栏
	m_PropList.EnableSearchBox(TRUE, "Search");//设置搜索框
	m_PropList.EnableContextMenu(FALSE);
	m_PropList.EnableHeaderCtrl();//表头
	m_PropList.EnableDesciptionArea();//描述区域
	m_PropList.SetVSDotNetLook(FALSE);
	m_PropList.MarkModifiedProperties(TRUE);
	m_PropList.SetNameAlign(DT_LEFT);
	m_PropList.SetAlphabeticMode(FALSE);//按照字母次序的方式,目录
	m_PropList.SetShowDragContext(TRUE);//显示上下文
	m_PropList.SetBooleanPropertiesStyle(CBCGPPropList::BOOL_PROPS_CHECK_BOX);//bool类型选择开关
	m_PropList.SetFont(&m_ftProp);
	m_PropList.SetDescriptionRows(4);
	m_PropList.SetPropertyColumnWidth(200);
	
	int nLoggerCount = m_vecPLogger.size();
	for (int i = 0; i < nLoggerCount; i++)
	{
		CBCGPProp* pGroupGeneralLog = new CBCGPProp(m_vecPLogger[i]->m_cstrLogName);
		CBCGPProp* pProp = new CBCGPProp(_T("是否保存日志"), (_variant_t)m_vecPLogger[i]->m_bWriteLog, _T("是否将日志保存到磁盘"));
		pProp->SetSwitchLabels("Yes", "No");
		pGroupGeneralLog->AddSubItem(pProp);
		pProp = new CBCGPProp(_T("保存天数"), (_variant_t)m_vecPLogger[i]->m_nSaveTime, _T("日志保存天数"));
		pGroupGeneralLog->AddSubItem(pProp);
		pProp = new CBCGPFileProp(_T("日志输出路径"), m_vecPLogger[i]->m_cstrFilePath, 0, "选择日志输出文件夹路径");
		pGroupGeneralLog->AddSubItem(pProp);
		m_PropList.AddProperty(pGroupGeneralLog);
	}
	CenterWindow();
	UpdateData(FALSE);
}
void CLoggerManagerDlg::SaveParam()
{
	int nCount = m_PropList.GetPropertyCount();
	for (int i = 0; i < nCount; i++)
	{
		CBCGPProp* pProp = m_PropList.GetProperty(i);
		int nSubCount = pProp->GetSubItemsCount();
		m_vecPLogger[i]->m_bWriteLog = (bool)(pProp->GetSubItem(0)->GetValue());
		m_vecPLogger[i]->m_nSaveTime = (int)(pProp->GetSubItem(1)->GetValue()) < 0 ? 30 : (int)(pProp->GetSubItem(1)->GetValue());
	//	m_vecPLogger[i]->m_cstrFilePath = (CString)(pProp->GetSubItem(2)->GetValue());
		m_vecPLogger[i]->SaveParam(FALSE);
	}
}


//void CLoggerManagerDlg::OnClose()
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CBCGPDialog::OnClose();
//}


void CLoggerManagerDlg::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	int nCount = m_PropList.GetPropertyCount();
	for (int i = nCount -1; i >= 0; i--)
	{
		int nSubCount = m_PropList.GetProperty(i)->GetSubItemsCount();
		for (int j = nSubCount -1 ; j >= 0 ; j--)
		{
			CBCGPProp* pPropSub = m_PropList.GetProperty(i)->GetSubItem(j);
			m_PropList.GetProperty(i)->RemoveSubItem(pPropSub);
		}
		CBCGPProp* pProp = m_PropList.GetProperty(i);
		m_PropList.DeleteProperty(pProp);
	}
}


void CLoggerManagerDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	SaveParam();
	CBCGPDialog::OnOK();
}


void CLoggerManagerDlg::OnBnClickedButtonViewlog()
{
	CLogViewDlg* plogViewDlg;
	plogViewDlg = new CLogViewDlg();
	plogViewDlg->m_vecLogger = m_vecPLogger;
	plogViewDlg->Create(IDD_LOGVIEWDLG, this);
	plogViewDlg->ShowWindow(SW_SHOW);
}


void CLoggerManagerDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}
