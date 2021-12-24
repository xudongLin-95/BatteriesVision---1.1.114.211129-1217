// DlgSemiAutoMatic.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgManualMatic.h"
#include "afxdialogex.h"


// CDlgManualMatic �Ի���

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


// CDlgManualMatic ��Ϣ�������


BOOL CDlgManualMatic::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE, TRUE);
	AlignData();
	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

HTupleVector CDlgManualMatic::CopyDataToTuple(CString strData)
{
	HTuple hvData = strData;
	HTupleVector hvReturn(1);
	HTuple Length, Position, Substring;
	TupleStrlen(hvData, &Length);
	try
	{
		while (true)//����\n�ֿ�
		{
			TupleStrstr(hvData, "\n", &Position);
			TupleStrlen(hvData, &Length);
			if (-1 != Position.I())//�ҵ���\r\n
			{
				TupleStrFirstN(hvData, Position, &Substring);
				
				HTuple hvLines = Substring;
				HTuple hvAllData;
				while (TRUE)//����\t�ֿ�
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
			if (Substring == hvData)//�Ѿ������һ����
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
	//����ʪ�ϵ���ֵ
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_Ercy.CreateRow(m_LST_Ercy.GetColumnCount());
	//����ߴ�
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
	//���ϵĻ���Ҫ����������ţ���֤��һ���������
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
	//����ʪ�ϵ���ֵ
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_Offset.CreateRow(m_LST_Offset.GetColumnCount());
	//����ߴ�
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
	//���ϵĻ���Ҫ����������ţ���֤��һ���������
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
	//����ʪ�ϵ���ֵ
	int nCount = 0;
	CBCGPGridRow* pRow = m_LST_CCD.CreateRow(m_LST_CCD.GetColumnCount());
	//����ߴ�
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
	//���ϵĻ���Ҫ����������ţ���֤��һ���������
	m_LST_CCD.InsertRowAfter(m_LST_CCD.GetRowCount() - 1, pRow, FALSE);
	m_LST_CCD.AdjustLayout();
	//tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//COLORREF colTmp[3];
	//colTmp[OK] = RGB(255, 255, 255);
	//colTmp[NG] = RGB(255, 0, 0);
	//colTmp[WARN] = RGB(248, 182, 43);
	////����ʪ�ϵ���ֵ
	//for (int i = 0; i < m_tWetAndDry[0].Wet.Length(); i++)
	//{
	//	int nCount = 0;
	//	CBCGPGridRow* pRow = m_LST_CCD.CreateRow(m_LST_CCD.GetColumnCount());
	//	//����ߴ�
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
	//�������ֵ
	lst.RemoveAll();
	lst.DeleteAllColumns();
	lst.SetReadOnly(FALSE);//�ɱ༭
	lst.EnableMarkSortedColumn(FALSE);//ʹ��������
	lst.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	lst.EnableRowHeader(TRUE);//ʹ���б�ͷ
	lst.EnableLineNumbers(TRUE);//ʹ�������
	lst.SetClearInplaceEditOnEnter(FALSE);
	lst.EnableInvertSelOnCtrl(FALSE);
	//m_LST_Grid_List.SetScalingRange(0.1, 2.0);//�������ű�������Ctrl���������ֿ�����
	int nCount = 0;
	for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
	{
		lst.InsertColumn(nCount++, CString("����" + tPdInfo.vecNameList[i]), 100);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			lst.InsertColumn(nCount++, CString("����" + tPdInfo.vecNameList[i]), 100);
		}
	}
	lst.LoadState(_T("BasicGrid"));
	lst.AdjustLayout();//�൱��������ʾ�ڱ����
	lst.ShowWindow(SW_NORMAL);
}
void CDlgManualMatic::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg;
	GetDlgItem(IDC_STATIC_LIST)->GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	//��ת����
	rcGridBase = rcDlg;
	rcGridBase.top = rcDlg.top;
	rcGridBase.bottom = rcDlg.bottom * 1 / 3;
	m_LST_CCD.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_CCD, theme);
	m_LST_CCD.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_CCD.SetColorTheme(theme);

	//��ת����
	rcGridBase.top = rcDlg.bottom * 1 / 3;
	rcGridBase.bottom = rcDlg.bottom * 2 / 3;
	m_LST_Ercy.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Ercy, theme);
	m_LST_Ercy.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Ercy.SetColorTheme(theme);
	//����ֵ����
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
	//�Ȼ�ȡ�����offsetֵ
	UpdateData(TRUE);
	SaveData();
	if (0 == m_LST_Offset.GetSelectionCount())
	{
		m_LST_Offset.SelectRow(0);
	}
	int nColumnCount = m_LST_Offset.GetColumnCount();
	HTuple hvAllColumn;
	for (int i = 0; i < nColumnCount; i++)//�еĺ�
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
			//���ÿһ�е���ֵ�;�ֵ�ŵ��������
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
			if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)//�Ƿ����AT11
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
			else//ֻ��Ϳ���ͼ���
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
			if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)//�Ƿ����AT11
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
			else//ֻ��Ϳ���ͼ���
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
			if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)//�Ƿ����AT11
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
			else//ֻ��Ϳ���ͼ���
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
	if (CGlobal::SN_Head == theGlobal.m_tSystemSet.eSN)//��ͷ�ĳߴ���Ҫ��һ������ľ���ͺ���ı���һ��
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
	strFile.Format("D:\\�ֶ�����(%s)%04d-%02d-%02d-%02d.xls", tPdInfo.strPDName,hvYear.I(),hvMonth.I(),hvDay.I(),hvHour.I());
	CStdioFile file;
	if (TRUE != file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		CString strException;
		strException.Format("Counter_WriteBatchRecord: ���������ļ�ʧ�� (%s)", strFile);
		theGlobal.m_Log.Exception(strException);
		return strFile;
	}

	file.WriteString("CCD��������\n");
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

	file.WriteString("����Ԫ��������\n");
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

	file.WriteString("��������\n");
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
	strMessageBox.Format("�����ѱ������!\r\n�ļ���:%s", strFileName);
	AfxMessageBox(strMessageBox);
}


void CDlgManualMatic::OnBnClickedButtonCompute()
{
	//�Ȼ�ȡ�����offsetֵ
	UpdateData(TRUE);
	int nColumnCount = m_LST_CCD.GetColumnCount();
	HTuple hvAllColumn;
	for (int i = 0; i < nColumnCount; i++)//�еĺ�
	{
		HTuple hvMean(0);
		HTuple hvTuple;
		for (int j = 0; j < m_LST_CCD.GetRowCount(); j++)
		{
			CBCGPGridRow *pRow = m_LST_CCD.GetRow(j);
			//���ÿһ�е���ֵ�;�ֵ�ŵ��������
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
	//����ߴ�
	for (int i=0;i<hvAllColumn.Length();i++)
	{
		CString strErcy = m_LST_Ercy.GetRow(0)->GetItem(i)->GetValue();
		CString strVal;
		strVal.Format("%0.4f", atof(strErcy) - hvAllColumn[i].D());//����Ԫ�ĳߴ��ȥCCD�������еľ�ֵ
		pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
		nCount++;
	}
	m_LST_Offset.InsertRowAfter(m_LST_Offset.GetRowCount() - 1, pRow, FALSE);
	m_LST_Offset.AdjustLayout();
}


BOOL CDlgManualMatic::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)//������������Ϣ
	{
		if (GetKeyState(VK_CONTROL) & 0x80)//�ж�CTRL���Ƿ��Ǳ����µ�״̬
		{
			SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);//����������Ϣ
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CDlgManualMatic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
						//����ߴ�
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
						//����ߴ�
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
			AfxMessageBox("ճ����û������");
			exit(0);
		}
	}

	break;
	default:
		break;
	}
	CBCGPDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}
