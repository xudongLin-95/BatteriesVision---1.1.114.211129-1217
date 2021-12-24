// DlgSemiAutoMatic.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgSemiAutoMatic.h"
#include "afxdialogex.h"


// CDlgSemiAutoMatic 对话框

IMPLEMENT_DYNAMIC(CDlgSemiAutoMatic, CBCGPDialog)

CDlgSemiAutoMatic::CDlgSemiAutoMatic(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_SEMIAUTOMATIC, pParent)
	, m_COM_nSelectMethod(1)
{

}

CDlgSemiAutoMatic::~CDlgSemiAutoMatic()
{
}

void CDlgSemiAutoMatic::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GANMO, m_STC_GM);
	DDX_Control(pDX, IDC_STATIC_SM, m_STC_SM);
	DDX_Control(pDX, IDC_STATIC_BC, m_STC_BC);
	DDX_CBIndex(pDX, IDC_COMBO_SELECT_METHOD, m_COM_nSelectMethod);
}


BEGIN_MESSAGE_MAP(CDlgSemiAutoMatic, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CDlgSemiAutoMatic::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA, &CDlgSemiAutoMatic::OnBnClickedButtonSavedata)
END_MESSAGE_MAP()


// CDlgSemiAutoMatic 消息处理程序


BOOL CDlgSemiAutoMatic::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	AlignData();
	InitUI();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CDlgSemiAutoMatic::AlignData()
{
	m_tWetAndDry[0] = theGlobal.m_DataCalculate.m_tWetAndDry[0];
	m_tWetAndDry[1] = theGlobal.m_DataCalculate.m_tWetAndDry[1];
	for (int i=0;i<2;i++)
	{
		//湿膜的长度大于干膜
		int nLengthDifference = m_tWetAndDry[i].Wet.Length() - m_tWetAndDry[i].Dry.Length();
		for (int j = 0; j < nLengthDifference;j++)
		{
			m_tWetAndDry[i].Wet.Remove(0);
		}
		for (int j =0; j<-nLengthDifference; j++)
		{
			m_tWetAndDry[i].Dry.Remove(0);
		}
	}
}
void CDlgSemiAutoMatic::UpdateListDry()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	//插入湿料的数值
	for (int i = 0; i < m_tWetAndDry[0].Dry.Length(); i++)
	{
		int nCount = 0;
		CBCGPGridRow* pRow = m_LST_Dry.CreateRow(m_LST_Dry.GetColumnCount());
		//正面尺寸
		for (int j = 0; j < m_tWetAndDry[0].Dry[i].T().Length(); j++)
		{
			CString strVal;
			if (m_tWetAndDry[0].Dry[i].T()[j] != NO_DATA)
			{
				strVal.Format("%0.4f", m_tWetAndDry[0].Dry[i].T()[j].D());
				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
				nCount++;
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			for (int j = 0; j < m_tWetAndDry[1].Dry[i].T().Length(); j++)
			{
				CString strVal;
				if (m_tWetAndDry[1].Dry[i].T()[j] != NO_DATA)
				{
					strVal.Format("%0.4f", m_tWetAndDry[1].Dry[i].T()[j].D());
					pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
					nCount++;
				}
				else
				{
					pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
				}
			}
		}
		//干料的画需要倒着向里面放，保证第一行是最近的
		m_LST_Dry.InsertRowAfter(m_LST_Dry.GetRowCount() - 1, pRow, FALSE);
		m_LST_Dry.AdjustLayout();
	}
}
void CDlgSemiAutoMatic::UpdateListOffset()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	for (int i=0;i<m_tWetAndDry[0].Dry.Length();i++)
	{
		int nCount = 0;
		CBCGPGridRow* pRow = m_LST_Offset.CreateRow(m_LST_Offset.GetColumnCount());
		//正面尺寸
		for (int j = 0; j < m_tWetAndDry[0].Dry[i].T().Length(); j++)
		{
			CString strVal;
			if (m_tWetAndDry[0].Wet[i].T()[j] != NO_DATA && m_tWetAndDry[0].Dry[i].T()[j] != NO_DATA)
			{
				strVal.Format("%0.4f", m_tWetAndDry[0].Dry[i].T()[j].D() - m_tWetAndDry[0].Wet[i].T()[j].D());//干的减去湿料
				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
				nCount++;
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			for (int j = 0; j < m_tWetAndDry[1].Dry[i].T().Length(); j++)
			{
				CString strVal;
				if (m_tWetAndDry[1].Wet[i].T()[j] != NO_DATA
					&& m_tWetAndDry[1].Dry[i].T()[j] != NO_DATA)
				{
					strVal.Format("%0.4f", m_tWetAndDry[1].Dry[i].T()[j].D() - m_tWetAndDry[1].Wet[i].T()[j].D());//干的减去湿料
					pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
					nCount++;
				}
				else
				{
					pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
				}
			}
		}
		m_LST_Offset.InsertRowAfter(m_LST_Offset.GetRowCount() - 1, pRow, FALSE);
		m_LST_Offset.AdjustLayout();
	}
	
	int nColumnCount = m_LST_Offset.GetColumnCount();
	int nRowCount = m_LST_Offset.GetRowCount();
	for (int k=0;k<2;k++)
	{
		CBCGPGridRow* pRow = m_LST_Offset.CreateRow(m_LST_Offset.GetColumnCount());
		//求出每一列的中值和均值放到最后两行
		for (int i = 0; i < nColumnCount; i++)//列的和
		{
			HTuple hvTuple;
			for (int j = 0; j < nRowCount; j++)
			{
				CString strValue = m_LST_Offset.GetRow(j)->GetItem(i)->GetValue();
				if (strValue != "-")
				{
					hvTuple.Append(atof(strValue));
				}
			}
			if (hvTuple.Length() <=0)
			{
				continue;
			}
			HTuple hvMean, hvMedian;
			TupleMean(hvTuple, &hvMean);
			TupleMedian(hvTuple, &hvMedian);
			CString strValue;
			
			if (0 == k && hvMean.Length()>0)
			{
				strValue.Format("%.4f", hvMean.D());
				pRow->GetItem(i)->SetValue(_variant_t(strValue), FALSE);
			}
			else if (1 == k && hvMedian.Length()>0)
			{
				strValue.Format("%.4f", hvMedian.D());
				pRow->GetItem(i)->SetValue(_variant_t(strValue), FALSE);
			}
		}
		m_LST_Offset.InsertRowAfter(m_LST_Offset.GetRowCount() - 1, pRow, FALSE);
		m_LST_Offset.AdjustLayout();
	}
	
}
void CDlgSemiAutoMatic::UpdateListWet()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	//插入湿料的数值
	for (int i=0;i<m_tWetAndDry[0].Wet.Length();i++)
	{
		int nCount = 0;
		CBCGPGridRow* pRow = m_LST_Wet.CreateRow(m_LST_Wet.GetColumnCount());
		//正面尺寸
		for (int j = 0; j < m_tWetAndDry[0].Wet[i].T().Length(); j++)
		{
			CString strVal;
			if (m_tWetAndDry[0].Wet[i].T()[j] != NO_DATA)
			{
				strVal.Format("%0.4f", m_tWetAndDry[0].Wet[i].T()[j].D());
				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
				nCount++;
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			for (int j = 0; j < m_tWetAndDry[1].Wet[i].T().Length(); j++)
			{
				CString strVal;
				if (m_tWetAndDry[1].Wet[i].T()[j] != NO_DATA)
				{
					strVal.Format("%0.4f", m_tWetAndDry[1].Wet[i].T()[j].D());
					pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
					nCount++;
				}
				else
				{
					pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
				}
			}
		}
		m_LST_Wet.InsertRowAfter(m_LST_Wet.GetRowCount() - 1, pRow, FALSE);
		m_LST_Wet.AdjustLayout();
	}
}
void CDlgSemiAutoMatic::ResetList(CBCGPGridCtrl& lst)
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
		lst.InsertColumn(nCount++, CString("正面" + tPdInfo.vecNameList[i]), 100);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			lst.InsertColumn(nCount++, CString("反面" + tPdInfo.vecNameList[i]), 100);
		}
	}
	lst.LoadState(_T("BasicGrid"));
	lst.AdjustLayout();//相当于数据显示在表格上
	lst.ShowWindow(SW_NORMAL);
}
void CDlgSemiAutoMatic::InitUI()
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


	m_STC_GM.SetFont("Microsoft Yahei", 50, 300);
	m_STC_GM.SetBkColor(RGB(37, 37, 37));
	m_STC_GM.SetTextColor(RGB(0, 255, 0));
	m_STC_SM.SetFont("Microsoft Yahei", 50, 300);
	m_STC_SM.SetBkColor(RGB(37, 37, 37));
	m_STC_SM.SetTextColor(RGB(0, 255, 0));
	m_STC_BC.SetFont("Microsoft Yahei", 50, 300);
	m_STC_BC.SetBkColor(RGB(37, 37, 37));
	m_STC_BC.SetTextColor(RGB(0, 255, 0));

	UpdateListWet();
	UpdateListDry();
	UpdateListOffset();
}

void CDlgSemiAutoMatic::OnBnClickedButtonWrite()
{
	//先获取计算的offset值
	UpdateData(TRUE);
	if (m_LST_Offset.GetRowCount() <=2)
	{
		AfxMessageBox("没有足够的数据");
		return;
	}
	if (0 == m_LST_Offset.GetSelectionCount())
	{
		m_LST_Offset.SelectRow(0);
	}
	int nColumnCount = m_LST_Offset.GetColumnCount();
	HTuple hvAllColumn;
	for (int i = 0; i < nColumnCount; i++)//列的和
	{
		HTuple hvMean(0), hvMedian(0);
		HTuple hvTuple;
		for (int j = 0; j < m_LST_Offset.GetRowCount(); j++)
		{
			CBCGPGridRow *pRow = m_LST_Offset.GetRow(j);
			if (FALSE == pRow->IsSelected())
			{
				continue;
			}
			//求出每一列的中值和均值放到最后两行
			CString strValue = pRow->GetItem(i)->GetValue();
			if (strValue != "-")
			{
				hvTuple.Append(atof(strValue));
			}
			if (hvTuple.Length() <= 0)
			{
				continue;
			}
			TupleMean(hvTuple, &hvMean);
			TupleMedian(hvTuple, &hvMedian);
		}
		if (0 == m_COM_nSelectMethod)
		{
			hvAllColumn.Append(hvMean);
		}
		else if (1 == m_COM_nSelectMethod)
		{
			hvAllColumn.Append(hvMedian);
		}
	}
	
	HTuple hvAT11,hvCoat,hvFoil,hvBaseCoat;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (OneSide == tPdInfo.nCoatingType)
	{
		for (int i = 0; i < hvAllColumn.Length(); i++)
		{
			CString strValue;
			strValue.Format("%f", hvAllColumn[i].D());
			if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)//是否存在AT11
			{
				if (i % 2 == 1)
				{
					if (tPdInfo.bExistAT11)
					{
						hvAT11.Append(atof(strValue));
					}
					else
					{
						hvBaseCoat.Append(atof(strValue));
					}
				}
				else if (i % 4 == 0)
				{
					hvFoil.Append(atof(strValue));
				}
				else if (i % 4 == 2)
				{
					hvCoat.Append(atof(strValue));
				}
			}
			else//只有涂布和极耳
			{
				if (i % 2 == 0)
				{
					hvFoil.Append(atof(strValue));
				}
				else
				{
					hvCoat.Append(atof(strValue));
				}
			}
		}
	}else if (TwoSide == tPdInfo.nCoatingType)
	{
		for (int i = 0; i < hvAllColumn.Length() / 2; i++)
		{
			CString strValue;
			strValue.Format("%f", hvAllColumn[i].D());
			if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)//是否存在AT11
			{
				if (i % 2 == 1)
				{
					if (tPdInfo.bExistAT11)
					{
						hvAT11.Append(atof(strValue));
					}
					else
					{
						hvBaseCoat.Append(atof(strValue));
					}
				}
				else if (i % 4 == 0)
				{
					hvFoil.Append(atof(strValue));
				}
				else if (i % 4 == 2)
				{
					hvCoat.Append(atof(strValue));
				}
			}
			else//只有涂布和极耳
			{
				if (i % 2 == 0)
				{
					hvFoil.Append(atof(strValue));
				}
				else
				{
					hvCoat.Append(atof(strValue));
				}
			}
		}
		for (int i = hvAllColumn.Length() / 2; i < hvAllColumn.Length(); i++)
		{
			CString strValue;
			strValue.Format("%f", hvAllColumn[i].D());
			if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)//是否存在AT11
			{
				if ((i - 1) % 2 == 1)
				{
					if (tPdInfo.bExistAT11)
					{
						hvAT11.Append(atof(strValue));
					}
					else
					{
						hvBaseCoat.Append(atof(strValue));
					}
				}
				else if ((i - 1) % 4 == 0)
				{
					hvFoil.Append(atof(strValue));
				}
				else if ((i - 1) % 4 == 2)
				{
					hvCoat.Append(atof(strValue));
				}
			}
			else//只有涂布和极耳
			{
				if ((i - 1) % 2 == 0)
				{
					hvFoil.Append(atof(strValue));
				}
				else
				{
					hvCoat.Append(atof(strValue));
				}
			}
		}
	}

	int nEnd = 1;
	/*if (tPdInfo.nCoatingType == TwoSide)
	{
		nEnd = 2;
	}*/
	for (int i = 0; i < nEnd; i++)
	{
		int nNum = tPdInfo.tParam.vecCoatWidth.size();
		if (nNum*nEnd == hvCoat.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecCoatWidth[j].OffsetList[i] = hvCoat[i*nNum + j].D();
			}
		}
		nNum = tPdInfo.tParam.vecFoilWidth.size();
		if (nNum*nEnd == hvFoil.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecFoilWidth[j].OffsetList[i] = hvFoil[i*nNum + j].D();
			}
		}
		nNum = tPdInfo.tParam.vecAT11Width.size();
		if (nNum*nEnd == hvAT11.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecAT11Width[j].OffsetList[i] = hvAT11[i*nNum + j].D();
			}
		}
		nNum = tPdInfo.tParam.vecBaseCoatWidth.size();
		if (nNum*nEnd == hvBaseCoat.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecBaseCoatWidth[j].OffsetList[i] = hvBaseCoat[i*nNum + j].D();
			}
		}
	}

	theGlobal.m_ProductTool.SetCurrentProductInfo(tPdInfo);
	return OnOK();
}



void CDlgSemiAutoMatic::OnBnClickedButtonSavedata()
{
	CString strFile;
	strFile.Format("D://半自动补偿数据.xls");
	CStdioFile file;
	if (TRUE != file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		CString strException;
		strException.Format("Counter_WriteBatchRecord: 创建报表文件失败 (%s)", strFile);
		theGlobal.m_Log.Exception(strException);
		return ;
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
	for (int i=0;i<m_LST_Wet.GetRowCount();i++)
	{
		CString strLines;
		for (int j=0;j<m_LST_Wet.GetColumnCount();j++)
		{
			CString strValue = m_LST_Wet.GetRow(i)->GetItem(j)->GetValue();
			strLines += strValue;
			strLines += "\t";
		}
		strLines += "\n";
		file.WriteString(strLines);
	}
	
	file.WriteString("干膜数据\n");
	strLines="";
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
	AfxMessageBox("数据已保存到 D:\\半自动补偿.xls");
}
