// DlgDetectSet.cpp : 实现文件
//

#include "stdafx.h"
#include "SCISeparate.h"
#include "DlgDetectSet.h"
#include "afxdialogex.h"


// CDlgDetectSet 对话框

IMPLEMENT_DYNAMIC(CDlgDetectSet, CDialogEx)

CDlgDetectSet::CDlgDetectSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDetectSet::IDD, pParent)
{

	m_dCoatShrink = 0.0;
}

CDlgDetectSet::~CDlgDetectSet()
{
}

void CDlgDetectSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COAT, m_LST_Coat);
	DDX_Text(pDX, IDC_EDIT_COAT_SHRINK, m_dCoatShrink);
	DDV_MinMaxDouble(pDX, m_dCoatShrink, 0, 999);
}


BEGIN_MESSAGE_MAP(CDlgDetectSet, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgDetectSet::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgDetectSet::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// CDlgDetectSet 消息处理程序
void CDlgDetectSet::InitUI()
{
	m_LST_Coat.SetColorHeader(TRUE);
	m_LST_Coat.SetHeaderBKColor(255, 255, 255, 1);
	//m_LST_Coat.SetHeaderFontHW(18, 0);

	m_LST_Coat.SetGridLines();
	m_LST_Coat.SetCheckboxeStyle();
	m_LST_Coat.SetEditable(TRUE);

	CString strVal;

	m_LST_Coat.DeleteAllItems();
	m_LST_Coat.DeleteAllColumns();
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "检测项目", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "灰度阈值", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "宽度(mm)", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "高度(mm)", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "面积(mm2)", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertItem(1,"涂布漏箔");
	m_LST_Coat.SetCheck(0,m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.bDetect);
	strVal.Format("%d",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.nGray);
	m_LST_Coat.SetItemText(0,1,strVal);
	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.dWidth);
	m_LST_Coat.SetItemText(0,2,strVal);
	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.dHeight);
	m_LST_Coat.SetItemText(0,3,strVal);
	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.dArea);
	m_LST_Coat.SetItemText(0,4,strVal);

	m_dCoatShrink = m_pOwner->m_Surface.m_SurfaceParam.CoatParam.dShrinkWidth;

	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.Reslution.dColumnReslution);
	GetDlgItem(IDC_EDIT_RESLUTION)->SetWindowTextA(strVal);

	UpdateData(FALSE);
}

BOOL CDlgDetectSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgDetectSet::OnBnClickedButtonCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}


void CDlgDetectSet::OnBnClickedButtonOk()
{
	// TODO: 在此添加控件通知处理程序代码

}
