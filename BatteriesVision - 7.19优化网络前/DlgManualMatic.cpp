// DlgSemiAutoMatic.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgManualMatic.h"
#include "afxdialogex.h"


// CDlgManualMatic 对话框

IMPLEMENT_DYNAMIC(CDlgManualMatic, CBCGPDialog)

CDlgManualMatic::CDlgManualMatic(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_MANUALMATIC, pParent)
{

}

CDlgManualMatic::~CDlgManualMatic()
{
}

void CDlgManualMatic::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GANMO, m_STC_GM);
	DDX_Control(pDX, IDC_STATIC_SM, m_STC_SM);
	DDX_Control(pDX, IDC_STATIC_BC, m_STC_BC);
}


BEGIN_MESSAGE_MAP(CDlgManualMatic, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CDlgManualMatic::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA, &CDlgManualMatic::OnBnClickedButtonSavedata)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA2, &CDlgManualMatic::OnBnClickedButtonCompute)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CDlgManualMatic 消息处理程序


BOOL CDlgManualMatic::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	AlignData();
	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

HTupleVector CDlgManualMatic::CopyDataToTuple(CString strData)
{
	HTuple hvData = strData;
	HTupleVector hvReturn(1);
	HTuple Length, Position, Substring;
	TupleStrlen(hvData, &Length);
	try
	{
		while (true)//按照\n分开
		{
			TupleStrstr(hvData, "\n", &Position);
			TupleStrlen(hvData, &Length);
			if (-1 != Position.I())//找到了\r\n
			{
				TupleStrFirstN(hvData, Position, &Substring);
				
				HTuple hvLines = Substring;
				HTuple hvAllData;
				while (TRUE)//按照\t分开
				{
					HTuple Length, Position, Substring;
					TupleStrstr(hvLines, "\t", &Position);
					TupleStrlen(hvLines, &Length);
					if (-1 != Position.I())
					{
						TupleStrFirstN(hvLines, Position, &Substring);
						TupleSubstr(hvLines, Position + 1, Length - 1, &hvLines);

						TupleStrlen(Substring, &Length);
						TupleStrFirstN(Substring, Length - 2, &Substring);
						hvAllData.Append(Substring);
					}
					else
					{
						TupleStrlen(hvLines, &Length);
						TupleStrFirstN(hvLines, Length - 3, &hvLines);
						hvAllData.Append(hvLines);
						break;
					}
				}
				if (hvAllData.Length() > 0)
				{
					hvReturn.Insert(hvReturn.Length(), HTupleVector(hvAllData));
				}
			}
			else
			{
				break;
			}
			if (Substring == hvData)//已经是最后一行了
			{
				break;
			}
			TupleSubstr(hvData, Position + 1, Length - 1, &hvData);
		}
		
	}
	catch (HException e)
	{

	}
	
	return hvReturn;
}
void CDlgManualMatic::AlignData()
{
	m_tWetAndDry[0] = theGlobal.m_DataCalculate.m_tWetAndDry[0];
	m_tWetAndDry[1] = theGlobal.m_DataCalculate.m_tWetAndDry[1];
}
void CDlgManualMatic::UpdateListErcy()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	//插入湿料的数值
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_Ercy.CreateRow(m_LST_Ercy.GetColumnCount());
	//正面尺寸
	for (int j = 0; j < m_tWetAndDry[0].Wet[0].T().Length(); j++)
	{
		pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		for (int j = 0; j < m_tWetAndDry[1].Wet[0].T().Length(); j++)
		{
			
			pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
		}
	}
	//干料的画需要倒着向里面放，保证第一行是最近的
	m_LST_Ercy.InsertRowAfter(m_LST_Ercy.GetRowCount() - 1, pRow, FALSE);
	m_LST_Ercy.AdjustLayout();
}
void CDlgManualMatic::UpdateListOffset()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	//插入湿料的数值
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_Offset.CreateRow(m_LST_Offset.GetColumnCount());
	//正面尺寸
	for (int j = 0; j < m_tWetAndDry[0].Wet[0].T().Length(); j++)
	{
		pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		for (int j = 0; j < m_tWetAndDry[1].Wet[0].T().Length(); j++)
		{

			pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
		}
	}
	//干料的画需要倒着向里面放，保证第一行是最近的
	m_LST_Offset.InsertRowAfter(m_LST_Offset.GetRowCount() - 1, pRow, FALSE);
	m_LST_Offset.AdjustLayout();

}
void CDlgManualMatic::UpdateListCCD()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	//插入湿料的数值
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_CCD.CreateRow(m_LST_CCD.GetColumnCount());
	//正面尺寸
	for (int j = 0; j < m_tWetAndDry[0].Wet[0].T().Length(); j++)
	{
		pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		for (int j = 0; j < m_tWetAndDry[1].Wet[0].T().Length(); j++)
		{

			pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
		}
	}
	//干料的画需要倒着向里面放，保证第一行是最近的
	m_LST_CCD.InsertRowAfter(m_LST_CCD.GetRowCount() - 1, pRow, FALSE);
	m_LST_CCD.AdjustLayout();
	//tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//COLORREF colTmp[3];
	//colTmp[OK] = RGB(255, 255, 255);
	//colTmp[NG] = RGB(255, 0, 0);
	//colTmp[WARN] = RGB(248, 182, 43);
	////插入湿料的数值
	//for (int i = 0; i < m_tWetAndDry[0].Wet.Length(); i++)
	//{
	//	int nCount = 0;
	//	CBCGPGridRow* pRow = m_LST_CCD.CreateRow(m_LST_CCD.GetColumnCount());
	//	//正面尺寸
	//	for (int j = 0; j < m_tWetAndDry[0].Wet[i].T().Length(); j++)
	//	{
	//		CString strVal;
	//		if (m_tWetAndDry[0].Wet[i].T()[j] != NO_DATA)
	//		{
	//			strVal.Format("%0.4f", m_tWetAndDry[0].Wet[i].T()[j].D());
	//			pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
	//			nCount++;
	//		}
	//		else
	//		{
	//			pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	//		}
	//	}
	//	if (tPdInfo.nCoatingType == TwoSide)
	//	{
	//		for (int j = 0; j < m_tWetAndDry[1].Wet[i].T().Length(); j++)
	//		{
	//			CString strVal;
	//			if (m_tWetAndDry[1].Wet[i].T()[j] != NO_DATA)
	//			{
	//				strVal.Format("%0.4f", m_tWetAndDry[1].Wet[i].T()[j].D());
	//				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
	//				nCount++;
	//			}
	//			else
	//			{
	//				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	//			}
	//		}
	//	}
	//	m_LST_CCD.InsertRowAfter(m_LST_CCD.GetRowCount() - 1, pRow, FALSE);
	//	m_LST_CCD.AdjustLayout();
	//}
}
void CDlgManualMatic::ResetList(CBCGPGridCtrl& lst)
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
void CDlgManualMatic::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg;
	GetDlgItem(IDC_STATIC_LIST)->GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	//正转数据
	rcGridBase = rcDlg;
	rcGridBase.top = rcDlg.top;
	rcGridBase.bottom = rcDlg.bottom * 1 / 3;
	m_LST_CCD.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_CCD, theme);
	m_LST_CCD.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_CCD.SetColorTheme(theme);

	//反转数据
	rcGridBase.top = rcDlg.bottom * 1 / 3;
	rcGridBase.bottom = rcDlg.bottom * 2 / 3;
	m_LST_Ercy.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Ercy, theme);
	m_LST_Ercy.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Ercy.SetColorTheme(theme);
	//补偿值数据
	rcGridBase.top = rcDlg.bottom * 2 / 3;
	rcGridBase.bottom = rcDlg.bottom;
	m_LST_Offset.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Offset, theme);
	m_LST_Offset.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Offset.SetColorTheme(theme);
	ResetList(m_LST_CCD);
	ResetList(m_LST_Ercy);
	ResetList(m_LST_Offset);


	m_STC_GM.SetFont("Microsoft Yahei", 30, 150);
	m_STC_GM.SetBkColor(RGB(37, 37, 37));
	m_STC_GM.SetTextColor(RGB(0, 255, 0));
	m_STC_SM.SetFont("Microsoft Yahei", 30, 150);
	m_STC_SM.SetBkColor(RGB(37, 37, 37));
	m_STC_SM.SetTextColor(RGB(0, 255, 0));
	m_STC_BC.SetFont("Microsoft Yahei", 30, 150);
	m_STC_BC.SetBkColor(RGB(37, 37, 37));
	m_STC_BC.SetTextColor(RGB(0, 255, 0));

	UpdateListCCD();
	UpdateListErcy();
	UpdateListOffset();
}

void CDlgManualMatic::OnBnClickedButtonWrite()
{
	//先获取计算的offset值
	UpdateData(TRUE);
	SaveData();
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
		hvAllColumn.Append(hvMean);
	}
	HTuple hvAT11, hvCoat, hvFoil, hvBaseCoat;
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
	}
	else if (TwoSide == tPdInfo.nCoatingType)
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
	if (tPdInfo.nCoatingType == TwoSide)
	{
		nEnd = 2;
	}
	if (CGlobal::SN_Head == theGlobal.m_tSystemSet.eSN)//机头的尺寸需要做一个横向的镜像和后面的保持一致
	{
		TupleInverse(hvCoat, &hvCoat);
		TupleInverse(hvFoil, &hvFoil);
		TupleInverse(hvAT11, &hvAT11);
		TupleInverse(hvBaseCoat, &hvBaseCoat);
	}
	for (int i = 0; i < nEnd; i++)
	{
		int nNum = tPdInfo.tParam.vecCoatWidth.size();
		if (nNum*nEnd == hvCoat.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecCoatWidth[j].OffsetList[i] += hvCoat[i*nNum + j].D();
			}
		}
		nNum = tPdInfo.tParam.vecFoilWidth.size();
		if (nNum*nEnd == hvFoil.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecFoilWidth[j].OffsetList[i] += hvFoil[i*nNum + j].D();
			}
		}
		nNum = tPdInfo.tParam.vecAT11Width.size();
		if (nNum*nEnd == hvAT11.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecAT11Width[j].OffsetList[i] += hvAT11[i*nNum + j].D();
			}
		}
		nNum = tPdInfo.tParam.vecBaseCoatWidth.size();
		if (nNum*nEnd == hvBaseCoat.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				tPdInfo.tParam.vecBaseCoatWidth[j].OffsetList[i] += hvBaseCoat[i*nNum + j].D();
			}
		}
	}

	theGlobal.m_ProductTool.SetCurrentProductInfo(tPdInfo);
	return OnOK();
}
CString CDlgManualMatic::SaveData()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	HTuple hvYear, hvMonth, hvDay, hvHour;
	GetSystemTime(NULL, NULL, NULL, &hvHour, &hvDay, NULL, &hvMonth, &hvYear);
	CString strFile;
	strFile.Format("D:\\手动补偿(%s)%04d-%02d-%02d-%02d.xls", tPdInfo.strPDName,hvYear.I(),hvMonth.I(),hvDay.I(),hvHour.I());
	CStdioFile file;
	if (TRUE != file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		CString strException;
		strException.Format("Counter_WriteBatchRecord: 创建报表文件失败 (%s)", strFile);
		theGlobal.m_Log.Exception(strException);
		return strFile;
	}

	file.WriteString("CCD测量数据\n");
	CString strLines("");
	for (int i = 0; i < m_LST_CCD.GetColumnCount(); i++)
	{
		strLines += m_LST_CCD.GetColumnName(i);
		strLines += "\t";
	}
	strLines += "\n";
	file.WriteString(strLines);
	for (int i = 0; i < m_LST_CCD.GetRowCount(); i++)
	{
		CString strLines;
		for (int j = 0; j < m_LST_CCD.GetColumnCount(); j++)
		{
			CString strValue = m_LST_CCD.GetRow(i)->GetItem(j)->GetValue();
			strLines += strValue;
			strLines += "\t";
		}
		strLines += "\n";
		file.WriteString(strLines);
	}

	file.WriteString("二次元测量数据\n");
	strLines = "";
	for (int i = 0; i < m_LST_Ercy.GetColumnCount(); i++)
	{
		strLines += m_LST_Ercy.GetColumnName(i);
		strLines += "\t";
	}
	strLines += "\n";
	file.WriteString(strLines);
	for (int i = 0; i < m_LST_Ercy.GetRowCount(); i++)
	{
		CString strLines;
		for (int j = 0; j < m_LST_Ercy.GetColumnCount(); j++)
		{
			CString strValue = m_LST_Ercy.GetRow(i)->GetItem(j)->GetValue();
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
	return strFile;
}
void CDlgManualMatic::OnBnClickedButtonSavedata()
{
	CString strFileName;
	strFileName = SaveData();
	CString strMessageBox;
	strMessageBox.Format("数据已保存完毕!\r\n文件名:%s", strFileName);
	AfxMessageBox(strMessageBox);
}


void CDlgManualMatic::OnBnClickedButtonCompute()
{
	//先获取计算的offset值
	UpdateData(TRUE);
	int nColumnCount = m_LST_CCD.GetColumnCount();
	HTuple hvAllColumn;
	for (int i = 0; i < nColumnCount; i++)//列的和
	{
		HTuple hvMean(0);
		HTuple hvTuple;
		for (int j = 0; j < m_LST_CCD.GetRowCount(); j++)
		{
			CBCGPGridRow *pRow = m_LST_CCD.GetRow(j);
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
		}
		hvAllColumn.Append(hvMean);
	}
	m_LST_Offset.RemoveAll();
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_Offset.CreateRow(m_LST_Offset.GetColumnCount());
	//正面尺寸
	for (int i=0;i<hvAllColumn.Length();i++)
	{
		CString strErcy = m_LST_Ercy.GetRow(0)->GetItem(i)->GetValue();
		CString strVal;
		strVal.Format("%0.4f", atof(strErcy) - hvAllColumn[i].D());//二次元的尺寸减去CCD测量的列的均值
		pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
		nCount++;
	}
	m_LST_Offset.InsertRowAfter(m_LST_Offset.GetRowCount() - 1, pRow, FALSE);
	m_LST_Offset.AdjustLayout();
}


BOOL CDlgManualMatic::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)//处理按键按下消息
	{
		if (GetKeyState(VK_CONTROL) & 0x80)//判断CTRL键是否是被按下的状态
		{
			SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);//发出按键消息
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CDlgManualMatic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nChar)
	{
	case 'V':
	{
		CString StrData;
		if (OpenClipboard())
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			char *buffer = (char*)GlobalLock(hData);
			StrData = buffer;
			GlobalUnlock(hData);
			CloseClipboard();
			HTupleVector hvData = CopyDataToTuple(StrData);
			BOOL bUpdateErcy(FALSE), bUpdateCCD(FALSE);
			for (int i = 0; i < m_LST_Ercy.GetRowCount(); i++)
			{
				CBCGPGridRow* pRow = m_LST_Ercy.GetRow(i);
				if (pRow->IsSelected())
				{
					bUpdateErcy = TRUE;
					break;
				}
			}
			for (int i = 0; i < m_LST_CCD.GetRowCount(); i++)
			{
				CBCGPGridRow* pRow = m_LST_CCD.GetRow(i);
				if (pRow->IsSelected())
				{
					bUpdateCCD = TRUE;
					break;
				}
			}
			if (hvData.Length() > 0)
			{
				if (TRUE == bUpdateCCD)
				{
					m_LST_CCD.RemoveAll();
					for (int i = 0; i < hvData.Length(); i++)
					{
						CBCGPGridRow* pRow = m_LST_CCD.CreateRow(m_LST_CCD.GetColumnCount());
						//正面尺寸
						for (int j = 0; j < min(m_LST_CCD.GetColumnCount(),hvData[i].T().Length()); j++)
						{
							HALCON_TRY(pRow->GetItem(j)->SetValue(hvData[i].T()[j].S().Text()));
						}
						m_LST_CCD.InsertRowAfter(m_LST_CCD.GetRowCount() - 1, pRow, FALSE);
						m_LST_CCD.AdjustLayout();
					}
				}
				if (TRUE == bUpdateErcy)
				{
					m_LST_Ercy.RemoveAll();
					for (int i = 0; i < hvData.Length(); i++)
					{
						int nCount = 0;
						CBCGPGridRow* pRow = m_LST_Ercy.CreateRow(m_LST_Ercy.GetColumnCount());
						//正面尺寸
						for (int j = 0; j < min(m_LST_Ercy.GetColumnCount(), hvData[i].T().Length()); j++)
						{
							HALCON_TRY(pRow->GetItem(j)->SetValue(hvData[i].T()[j].S().Text()));
						}
						m_LST_Ercy.InsertRowAfter(m_LST_Ercy.GetRowCount() - 1, pRow, FALSE);
						m_LST_Ercy.AdjustLayout();
					}
				}
			}
			
		}
		else
		{
			AfxMessageBox("粘贴板没有内容");
			exit(0);
		}
	}

	break;
	default:
		break;
	}
	CBCGPDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}
