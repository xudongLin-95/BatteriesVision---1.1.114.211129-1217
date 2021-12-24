// DlgAlarmSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgAlarmSet.h"
#include "afxdialogex.h"


// CDlgAlarmSet 对话框

IMPLEMENT_DYNAMIC(CDlgAlarmSet, CBCGPDialog)

CDlgAlarmSet::CDlgAlarmSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_ALARM_SET, pParent)
	, m_EDIT_dIngoreMeter(0)
	, m_EDIT_dMeanShiftFactor(0)
{

}

CDlgAlarmSet::~CDlgAlarmSet()
{
}

void CDlgAlarmSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INGORE_METER, m_EDIT_dIngoreMeter);
	DDX_Text(pDX, IDC_EDIT_MEANSHIFT_FACTOR, m_EDIT_dMeanShiftFactor);
}


BEGIN_MESSAGE_MAP(CDlgAlarmSet, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgAlarmSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAlarmSet 消息处理程序


BOOL CDlgAlarmSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CBCGPDialog::PreTranslateMessage(pMsg);
}


BOOL CDlgAlarmSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);

	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgAlarmSet::InitUI()
{
	int nCount = 0;
	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_Grid_SizeAlarm.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_SizeAlarm, theme);
	m_Grid_SizeAlarm.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_SizeAlarm.SetColorTheme(theme);
	m_Grid_SizeAlarm.SetReadOnly(FALSE);//可编辑
	m_Grid_SizeAlarm.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_SizeAlarm.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_SizeAlarm.EnableRowHeader(TRUE);//使用行表头
	m_Grid_SizeAlarm.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_SizeAlarm.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_SizeAlarm.EnableInvertSelOnCtrl(FALSE);
	nCount = 0;
	m_Grid_SizeAlarm.InsertColumn(nCount++, "测量类型", 120);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "报警判断", 80);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "NG米数(报警)", 100);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "", 40);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "停机判断", 80);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "NG米数(停机)", 100);
	m_Grid_SizeAlarm.LoadState(_T("BasicGrid"));
	m_Grid_SizeAlarm.AdjustLayout();//相当于数据显示在表格上
	m_Grid_SizeAlarm.ShowWindow(SW_NORMAL);

	RefreshSizeList();

	GetDlgItem(IDC_STATIC_GRID2)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_Grid_DefectAlarm.Create(WS_CHILD, rcGridBase, this, 1001);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_DefectAlarm, theme);
	m_Grid_DefectAlarm.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_DefectAlarm.SetColorTheme(theme);
	m_Grid_DefectAlarm.SetReadOnly(FALSE);//可编辑
	m_Grid_DefectAlarm.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_DefectAlarm.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_DefectAlarm.EnableRowHeader(TRUE);//使用行表头
	m_Grid_DefectAlarm.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_DefectAlarm.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_DefectAlarm.EnableInvertSelOnCtrl(FALSE);
	nCount = 0;
	m_Grid_DefectAlarm.InsertColumn(nCount++, "瑕疵类型", 120);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "报警判断", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG面积(报警)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG个数(报警)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG米数(报警)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "", 40);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "停机判断", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG面积(停机)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG个数(停机)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG米数(停机)", 90);
	m_Grid_DefectAlarm.LoadState(_T("BasicGrid"));
	m_Grid_DefectAlarm.AdjustLayout();//相当于数据显示在表格上
	m_Grid_DefectAlarm.ShowWindow(SW_NORMAL);

	RefreshDefectList();

	((CButton*)GetDlgItem(IDC_CHECK_USE_ALARM))->SetCheck(theGlobal.m_Judge.m_tParam.bUseAlarm);
	((CButton*)GetDlgItem(IDC_CHECK_USE_STOP_MACHINE))->SetCheck(theGlobal.m_Judge.m_tParam.bUseStopMachine);
	((CButton*)GetDlgItem(IDC_CHECK_USE_NG_MACHINE))->SetCheck(theGlobal.m_Judge.m_tParam.bUseNGAlarm);
	m_EDIT_dIngoreMeter = theGlobal.m_Judge.m_tParam.dIngoreMeter;
	m_EDIT_dMeanShiftFactor = theGlobal.m_Judge.m_tParam.dMeanShiftLimitFactor;

	UpdateData(FALSE);
}

void CDlgAlarmSet::RefreshSizeList()
{
	CBCGPGridRow* pRow;
	m_Grid_SizeAlarm.RemoveAll();
	for (int i = 0; i < NG_ABMisalign+1; i++)
	{
		pRow = m_Grid_SizeAlarm.CreateRow(m_Grid_SizeAlarm.GetColumnCount());
		CString strName = theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth + i);

		pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
		pRow->GetItem(1)->AddOption("关闭");
		pRow->GetItem(1)->AddOption("开启");
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse ? "开启" : "关闭"), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters.D()), FALSE);

		pRow->GetItem(4)->AddOption("关闭");
		pRow->GetItem(4)->AddOption("开启");
		pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse ? "开启" : "关闭"),FALSE);
		pRow->GetItem(5)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters.D()), FALSE);
		m_Grid_SizeAlarm.AddRow(pRow, FALSE);
	}
	//AB面基材互校准
	pRow = m_Grid_SizeAlarm.CreateRow(m_Grid_SizeAlarm.GetColumnCount());
	CString strName = "AB面基材宽互校";
	pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
	pRow->GetItem(1)->AddOption("关闭");
	pRow->GetItem(1)->AddOption("开启");
	pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].bUse ? "开启" : "关闭"), FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].hvMeters.D()), FALSE);
	pRow->GetItem(4)->AddOption("关闭");
	pRow->GetItem(4)->AddOption("开启");
	pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].bUse ? "开启" : "关闭"), FALSE);
	pRow->GetItem(5)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].hvMeters.D()), FALSE);
	m_Grid_SizeAlarm.AddRow(pRow, FALSE);

	//Mean Shift
	for (int i = 0; i < NG_ABMisalign + 1; i++)
	{
		pRow = m_Grid_SizeAlarm.CreateRow(m_Grid_SizeAlarm.GetColumnCount());
		CString strName = "Mean Shift " + theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth + i);

		pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
		pRow->GetItem(1)->AddOption("关闭");
		pRow->GetItem(1)->AddOption("开启");
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].bUse ? "开启" : "关闭"), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].hvMeters.D()), FALSE);

		pRow->GetItem(4)->AddOption("关闭");
		pRow->GetItem(4)->AddOption("开启");
		pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].bUse ? "开启" : "关闭"), FALSE);
		pRow->GetItem(5)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].hvMeters.D()), FALSE);
		m_Grid_SizeAlarm.AddRow(pRow, FALSE);
	}


	m_Grid_SizeAlarm.AdjustLayout();
}

void CDlgAlarmSet::RefreshDefectList()
{
	CBCGPGridRow* pRow;
	m_Grid_DefectAlarm.RemoveAll();
	for (int i = NG_ABMisalign + 1; i < NG_Count; i++)
	{
		pRow = m_Grid_DefectAlarm.CreateRow(m_Grid_DefectAlarm.GetColumnCount());
		CString strName = theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth + i);

		pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
		pRow->GetItem(1)->AddOption("关闭");
		pRow->GetItem(1)->AddOption("开启");
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse ? "开启" : "关闭"), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvArea.D()), FALSE);
		pRow->GetItem(3)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvNum.I()), FALSE);
		pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters.D()), FALSE);

		pRow->GetItem(6)->AddOption("关闭");
		pRow->GetItem(6)->AddOption("开启");
		pRow->GetItem(6)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse ? "开启" : "关闭"), FALSE);
		pRow->GetItem(7)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvArea.D()), FALSE);
		pRow->GetItem(8)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvNum.I()), FALSE);
		pRow->GetItem(9)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters.D()), FALSE);
		m_Grid_DefectAlarm.AddRow(pRow, FALSE);
	}

	m_Grid_DefectAlarm.AdjustLayout();
}

void CDlgAlarmSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CBCGPGridRow* pRow;
	theGlobal.m_Judge.m_tParam.bUseAlarm = ((CButton*)GetDlgItem(IDC_CHECK_USE_ALARM))->GetCheck();
	theGlobal.m_Judge.m_tParam.bUseStopMachine = ((CButton*)GetDlgItem(IDC_CHECK_USE_STOP_MACHINE))->GetCheck();
	theGlobal.m_Judge.m_tParam.bUseNGAlarm = ((CButton*)GetDlgItem(IDC_CHECK_USE_NG_MACHINE))->GetCheck();
	theGlobal.m_Judge.m_tParam.dIngoreMeter = m_EDIT_dIngoreMeter;
	theGlobal.m_Judge.m_tParam.dMeanShiftLimitFactor = m_EDIT_dMeanShiftFactor;

	//尺寸
	for (int i = 0; i < NG_ABMisalign + 1; i++)
	{
		pRow = m_Grid_SizeAlarm.GetRow(i);

		theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse = (CString)pRow->GetItem(1)->GetValue() == "开启" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters = (double)pRow->GetItem(2)->GetValue();

		theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse = (CString)pRow->GetItem(4)->GetValue() == "开启" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters = (double)pRow->GetItem(5)->GetValue();
	}
	//AB尺寸互校
	pRow = m_Grid_SizeAlarm.GetRow(NG_ABMisalign+1);
	theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].bUse = (CString)pRow->GetItem(1)->GetValue() == "开启" ? 1 : 0;
	theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].hvMeters = (double)pRow->GetItem(2)->GetValue();

	theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].bUse = (CString)pRow->GetItem(4)->GetValue() == "开启" ? 1 : 0;
	theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].hvMeters = (double)pRow->GetItem(5)->GetValue();
	//Mean Shift
	for (int i = 0; i < NG_ABMisalign + 1; i++)
	{
		pRow = m_Grid_SizeAlarm.GetRow(NG_ABMisalign + 2 + i);

		theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].bUse = (CString)pRow->GetItem(1)->GetValue() == "开启" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].hvMeters = (double)pRow->GetItem(2)->GetValue();

		theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].bUse = (CString)pRow->GetItem(4)->GetValue() == "开启" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].hvMeters = (double)pRow->GetItem(5)->GetValue();
	}

	//瑕疵
	for (int i = NG_ExposeFoil; i < NG_Count; i++)
	{
		pRow = m_Grid_DefectAlarm.GetRow(i - NG_ExposeFoil);

		theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse = (CString)pRow->GetItem(1)->GetValue() == "开启" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvArea = (double)pRow->GetItem(2)->GetValue();
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvNum = (int)pRow->GetItem(3)->GetValue();
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters = (double)pRow->GetItem(4)->GetValue();


		theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse = (CString)pRow->GetItem(6)->GetValue() == "开启" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvArea = (double)pRow->GetItem(7)->GetValue();
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvNum = (int)pRow->GetItem(8)->GetValue();
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters = (double)pRow->GetItem(9)->GetValue();
	}

	theGlobal.m_Judge.WriteParam();

	OnOK();
}
