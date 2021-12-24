// DlgSemiAutoMatic.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgAutoMatic.h"
#include "afxdialogex.h"


// CDlgAutoMatic 对话框

IMPLEMENT_DYNAMIC(CDlgAutoMatic, CBCGPDialog)

CDlgAutoMatic::CDlgAutoMatic(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_AUTOMATIC, pParent)
	, m_vecRawData(1)
	, m_vecDataCollect(1)
{

}

CDlgAutoMatic::~CDlgAutoMatic()
{
}

void CDlgAutoMatic::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GANMO, m_STC_GM);
	DDX_Control(pDX, IDC_STATIC_SM, m_STC_SM);
	DDX_Control(pDX, IDC_STATIC_BC, m_STC_BC);
}


BEGIN_MESSAGE_MAP(CDlgAutoMatic, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA, &CDlgAutoMatic::OnBnClickedButtonSavedata)
END_MESSAGE_MAP()


// CDlgAutoMatic 消息处理程序


BOOL CDlgAutoMatic::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	AlignData();
	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CDlgAutoMatic::AlignData()
{
	m_vecRawData = theGlobal.m_Upload.m_vecRawData;//延迟到后工位的数据组
	m_vecDataCollect = theGlobal.m_Upload.m_vecDataCollect;
	m_hvBiasRecv = theGlobal.m_Upload.m_hvBiasRecv;
	m_hvCompensation = theGlobal.m_Upload.m_hvCompensation;
}
void CDlgAutoMatic::UpdateListDry()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	//插入湿料的数值
	for (int i = 0; i < m_vecDataCollect.Length(); i++)
	{
		int nCount = 0;
		CBCGPGridRow* pRow = m_LST_Dry.CreateRow(m_LST_Dry.GetColumnCount());
		//正面尺寸
		for (int j = 0; j < m_vecDataCollect[i].T().Length(); j++)
		{
			CString strVal;
			if (m_vecDataCollect[i].T()[j] != NO_DATA)
			{
				strVal.Format("%0.4f", m_vecDataCollect[i].T()[j].D());
				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
				nCount++;
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
		//干料的画需要倒着向里面放，保证第一行是最近的
		m_LST_Dry.InsertRowAfter(m_LST_Dry.GetRowCount() - 1, pRow, FALSE);
		m_LST_Dry.AdjustLayout();
	}
}
void CDlgAutoMatic::UpdateListOffset()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_Offset.CreateRow(m_LST_Offset.GetColumnCount());
	//正面尺寸
	for (int j = 0; j < m_hvBiasRecv.Length(); j++)
	{
		CString strVal;
		if (m_hvBiasRecv[j] != NO_DATA)
		{
			strVal.Format("%0.4f", m_hvBiasRecv[j].D());
			pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
			nCount++;
		}
		else
		{
			pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
		}
	}
	m_LST_Offset.InsertRowAfter(m_LST_Offset.GetRowCount() - 1, pRow, FALSE);



	nCount = 0;
	pRow = m_LST_Offset.CreateRow(m_LST_Offset.GetColumnCount());
	//正面尺寸
	for (int j = 0; j < m_hvCompensation.Length(); j++)
	{
		CString strVal;
		if (m_hvCompensation[j] != NO_DATA)
		{
			strVal.Format("%0.4f", m_hvCompensation[j].D());
			pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
			nCount++;
		}
		else
		{
			pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
		}
	}
	m_LST_Offset.InsertRowAfter(m_LST_Offset.GetRowCount() - 1, pRow, FALSE);

	m_LST_Offset.AdjustLayout();
}
void CDlgAutoMatic::UpdateListWet()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	//插入湿料的数值
	for (int i = 0; i < m_vecRawData.Length(); i++)
	{
		int nCount = 0;
		CBCGPGridRow* pRow = m_LST_Wet.CreateRow(m_LST_Wet.GetColumnCount());
		//正面尺寸
		for (int j = 0; j < m_vecRawData[i].T().Length(); j++)
		{
			CString strVal;
			if (m_vecRawData[i].T()[j] != NO_DATA)
			{
				strVal.Format("%0.4f", m_vecRawData[i].T()[j].D());
				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
				nCount++;
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
		m_LST_Wet.InsertRowAfter(m_LST_Wet.GetRowCount() - 1, pRow, FALSE);
		m_LST_Wet.AdjustLayout();
	}
}
void CDlgAutoMatic::ResetList(CBCGPGridCtrl& lst)
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//正面测量值
	lst.RemoveAll();
	lst.DeleteAllColumns();
	lst.SetReadOnly(FALSE);//可编辑
	lst.EnableMarkSortedColumn(FALSE);//使用列排序
	lst.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	lst.EnableRowHeader(TRUE);//使用行表头
	lst.EnableLineNumbers(TRUE);//使用行序号
	lst.SetClearInplaceEditOnEnter(FALSE);
	lst.EnableInvertSelOnCtrl(FALSE);
	//m_LST_Grid_List.SetScalingRange(0.1, 2.0);//设置缩放比例，按Ctrl键和鼠标滚轮可缩放
	int nCount = 0;
	for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
	{
		lst.InsertColumn(nCount++, CString(tPdInfo.vecNameList[i]), 100);
	}
	lst.LoadState(_T("BasicGrid"));
	lst.AdjustLayout();//相当于数据显示在表格上
	lst.ShowWindow(SW_NORMAL);
}
void CDlgAutoMatic::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg;
	GetDlgItem(IDC_STATIC_LIST)->GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	//正转数据
	rcGridBase = rcDlg;
	rcGridBase.top = rcDlg.top;
	rcGridBase.bottom = rcDlg.bottom * 1 / 3;
	m_LST_Wet.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Wet, theme);
	m_LST_Wet.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Wet.SetColorTheme(theme);

	//反转数据
	rcGridBase.top = rcDlg.bottom * 1 / 3;
	rcGridBase.bottom = rcDlg.bottom * 2 / 3;
	m_LST_Dry.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Dry, theme);
	m_LST_Dry.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Dry.SetColorTheme(theme);
	//补偿值数据
	rcGridBase.top = rcDlg.bottom * 2 / 3;
	rcGridBase.bottom = rcDlg.bottom;
	m_LST_Offset.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Offset, theme);
	m_LST_Offset.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Offset.SetColorTheme(theme);
	ResetList(m_LST_Wet);
	ResetList(m_LST_Dry);
	ResetList(m_LST_Offset);


	m_STC_GM.SetFont("Microsoft Yahei", 25, 150);
	m_STC_GM.SetBkColor(RGB(37, 37, 37));
	m_STC_GM.SetTextColor(RGB(0, 255, 0));
	m_STC_SM.SetFont("Microsoft Yahei", 25, 150);
	m_STC_SM.SetBkColor(RGB(37, 37, 37));
	m_STC_SM.SetTextColor(RGB(0, 255, 0));
	m_STC_BC.SetFont("Microsoft Yahei", 25, 150);
	m_STC_BC.SetBkColor(RGB(37, 37, 37));
	m_STC_BC.SetTextColor(RGB(0, 255, 0));

	UpdateListWet();
	UpdateListDry();
	UpdateListOffset();
}


void CDlgAutoMatic::OnBnClickedButtonSavedata()
{
	CString strFile;
	strFile.Format("D://自动补偿数据.xls");
	CStdioFile file;
	if (TRUE != file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		CString strException;
		strException.Format("Counter_WriteBatchRecord: 创建报表文件失败 (%s)", strFile);
		theGlobal.m_Log.Exception(strException);
		return;
	}
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	file.WriteString("湿膜数据\n");
	CString strLines("");
	for (int i = 0; i < m_LST_Wet.GetColumnCount(); i++)
	{
		strLines += m_LST_Wet.GetColumnName(i);
		strLines += "\t";
	}
	strLines += "\n";
	file.WriteString(strLines);
	for (int i = 0; i < m_LST_Wet.GetRowCount(); i++)
	{
		CString strLines;
		for (int j = 0; j < m_LST_Wet.GetColumnCount(); j++)
		{
			CString strValue = m_LST_Wet.GetRow(i)->GetItem(j)->GetValue();
			strLines += strValue;
			strLines += "\t";
		}
		strLines += "\n";
		file.WriteString(strLines);
	}

	file.WriteString("干膜数据\n");
	strLines = "";
	for (int i = 0; i < m_LST_Dry.GetColumnCount(); i++)
	{
		strLines += m_LST_Dry.GetColumnName(i);
		strLines += "\t";
	}
	strLines += "\n";
	file.WriteString(strLines);
	for (int i = 0; i < m_LST_Dry.GetRowCount(); i++)
	{
		CString strLines;
		for (int j = 0; j < m_LST_Dry.GetColumnCount(); j++)
		{
			CString strValue = m_LST_Dry.GetRow(i)->GetItem(j)->GetValue();
			strLines += strValue;
			strLines += "\t";
		}
		strLines += "\n";
		file.WriteString(strLines);
	}

	file.WriteString("补偿数据\n");
	strLines = "";
	for (int i = 0; i < m_LST_Offset.GetColumnCount(); i++)
	{
		strLines += m_LST_Offset.GetColumnName(i);
		strLines += "\t";
	}
	strLines += "\n";
	file.WriteString(strLines);
	for (int i = 0; i < m_LST_Offset.GetRowCount(); i++)
	{
		CString strLines;
		for (int j = 0; j < m_LST_Offset.GetColumnCount(); j++)
		{
			CString strValue = m_LST_Offset.GetRow(i)->GetItem(j)->GetValue();
			strLines += strValue;
			strLines += "\t";
		}
		strLines += "\n";
		file.WriteString(strLines);
	}
	file.Close();
	AfxMessageBox("数据已保存到 D:\\自动补偿.xls");
}
