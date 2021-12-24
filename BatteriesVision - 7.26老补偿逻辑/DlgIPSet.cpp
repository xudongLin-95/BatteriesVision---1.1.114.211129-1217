// DlgIPSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgIPSet.h"
#include "afxdialogex.h"


// CDlgIPSet 对话框

IMPLEMENT_DYNAMIC(CDlgIPSet, CBCGPDialog)

CDlgIPSet::CDlgIPSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_IP_SET, pParent)
{

}

CDlgIPSet::~CDlgIPSet()
{
}

void CDlgIPSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgIPSet, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgIPSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgIPSet 消息处理程序


BOOL CDlgIPSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgIPSet::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);

	m_Grid_Base.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Base, theme);
	m_Grid_Base.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Base.SetColorTheme(theme);
	m_Grid_Base.SetReadOnly(FALSE);//不可编辑
	m_Grid_Base.SetWholeRowSel(FALSE);
	m_Grid_Base.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_Base.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_Base.EnableCheckBoxes(FALSE);
	m_Grid_Base.EnableRowHeader(TRUE);//使用行表头
	m_Grid_Base.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_Base.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_Base.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Base.EnableDragHeaderItems(FALSE);
	m_Grid_Base.LoadState(_T("BasicGrid"));
	m_Grid_Base.AdjustLayout();//相当于数据显示在表格上
	m_Grid_Base.ShowWindow(SW_NORMAL);

	//IP和端口设置
	CBCGPGridRow* pRow;
	m_Grid_Base.RemoveAll();
	m_Grid_Base.DeleteAllColumns();

	m_Grid_Base.InsertColumn(0, "名称", 100);
	m_Grid_Base.InsertColumn(1, "IP", 130);
	m_Grid_Base.InsertColumn(2, "端口", 80);
	m_Grid_Base.InsertColumn(3, "启用", 80);

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		//工位1服务器
		pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("工位2(监听)"), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t("-"), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.nPortListen), FALSE);
		pRow->GetItem(3)->AddOption("关闭");
		pRow->GetItem(3)->AddOption("启用");
		pRow->GetItem(3)->SetValue(_variant_t(TRUE ? "启用" : "关闭"), FALSE);
		m_Grid_Base.AddRow(pRow, FALSE);
	}
	else if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
	{
		//工位2客户端
		pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("工位1(连接)"), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.strAddresConnect), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.nPortConnect), FALSE);
		pRow->GetItem(3)->AddOption("关闭");
		pRow->GetItem(3)->AddOption("启用");
		pRow->GetItem(3)->SetValue(_variant_t(TRUE ? "启用" : "关闭"), FALSE);
		m_Grid_Base.AddRow(pRow, FALSE);

		//双层二涂也作为服务器
		if (theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
		{
			//工位2服务器
			pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());
			pRow->GetItem(0)->SetValue(_variant_t("工位3(监听)"), FALSE);
			pRow->GetItem(1)->SetValue(_variant_t("-"), FALSE);
			pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.nPortListen), FALSE);
			pRow->GetItem(3)->AddOption("关闭");
			pRow->GetItem(3)->AddOption("启用");
			pRow->GetItem(3)->SetValue(_variant_t(TRUE ? "启用" : "关闭"), FALSE);
			m_Grid_Base.AddRow(pRow, FALSE);
		}
	}
	else
	{
		//工位3及其他工位都暂时作为客户端
		pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("工位2(连接)"), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.strAddresConnect), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.nPortConnect), FALSE);
		pRow->GetItem(3)->AddOption("关闭");
		pRow->GetItem(3)->AddOption("启用");
		pRow->GetItem(3)->SetValue(_variant_t(TRUE ? "启用" : "关闭"), FALSE);
		m_Grid_Base.AddRow(pRow, FALSE);
	}
	m_Grid_Base.AdjustLayout();
}


void CDlgIPSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CBCGPGridRow* pRow;
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		//工位2，端口监听
		pRow = m_Grid_Base.GetRow(0);
		theGlobal.m_Upload.m_tParam.nPortListen = (_variant_t)pRow->GetItem(2)->GetValue();
		theGlobal.m_Upload.m_tParam.bListenClient = ((CString)pRow->GetItem(3)->GetValue()) == "启用" ? TRUE : FALSE;
	}
	else if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
	{
		//工位1的IP和端口
		pRow = m_Grid_Base.GetRow(0);
		theGlobal.m_Upload.m_tParam.strAddresConnect = (_variant_t)pRow->GetItem(1)->GetValue();
		theGlobal.m_Upload.m_tParam.nPortConnect = (_variant_t)pRow->GetItem(2)->GetValue();
		if (theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
		{
			//工位3，端口监听
			pRow = m_Grid_Base.GetRow(1);
			theGlobal.m_Upload.m_tParam.nPortListen = (_variant_t)pRow->GetItem(2)->GetValue();
			theGlobal.m_Upload.m_tParam.bListenClient = ((CString)pRow->GetItem(3)->GetValue()) == "启用" ? TRUE : FALSE;
		}
	}
	else
	{
		//工位2的IP和端口
		pRow = m_Grid_Base.GetRow(0);
		theGlobal.m_Upload.m_tParam.strAddresConnect = (_variant_t)pRow->GetItem(1)->GetValue();
		theGlobal.m_Upload.m_tParam.nPortConnect = (_variant_t)pRow->GetItem(2)->GetValue();
	}

	theGlobal.m_Upload.WriteParam();

	OnOK();
}
