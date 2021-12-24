// DlgSemiAutoMatic.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgK.h"
#include "afxdialogex.h"


// CDlgK �Ի���

IMPLEMENT_DYNAMIC(CDlgK, CBCGPDialog)

CDlgK::CDlgK(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_MANUALK, pParent)
	, m_COM_nInternOffset(0)
{
	m_dInternOffset[0] = 0;
	m_dInternOffset[1] = 0;
}

CDlgK::~CDlgK()
{
}

void CDlgK::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GANMO, m_STC_GM);
	DDX_Control(pDX, IDC_STATIC_SM, m_STC_SM);
	DDX_Control(pDX, IDC_STATIC_BC, m_STC_BC);
	DDX_CBIndex(pDX, IDC_COMBO_INTERNOFFSET, m_COM_nInternOffset);
}


BEGIN_MESSAGE_MAP(CDlgK, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CDlgK::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA, &CDlgK::OnBnClickedButtonSavedata)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA2, &CDlgK::OnBnClickedButtonCompute)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CDlgK ��Ϣ�������


BOOL CDlgK::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE, TRUE);
	m_tInitVecK[0] = m_tVecK[0];
	m_tInitVecK[1] = m_tVecK[1];
	AlignData();
	InitUI();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
void CDlgK::AlignData()
{
	m_tWetAndDry[0] = theGlobal.m_DataCalculate.m_tWetAndDry[0];
	m_tWetAndDry[1] = theGlobal.m_DataCalculate.m_tWetAndDry[1];
}
void CDlgK::UpdateListErcy()
{
	//tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//COLORREF colTmp[3];
	//colTmp[OK] = RGB(255, 255, 255);
	//colTmp[NG] = RGB(255, 0, 0);
	//colTmp[WARN] = RGB(248, 182, 43);
	////����ʪ�ϵ���ֵ
	//int nCount = 0;
	//CBCGPGridRow* pRow = m_LST_Ercy.CreateRow(m_LST_Ercy.GetColumnCount());
	////����ߴ�
	//for (int j = 0; j < m_tWetAndDry[0].Wet[0].T().Length(); j++)
	//{
	//	pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	//}
	//if (tPdInfo.nCoatingType == TwoSide)
	//{
	//	for (int j = 0; j < m_tWetAndDry[1].Wet[0].T().Length(); j++)
	//	{

	//		pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	//	}
	//}
	////���ϵĻ���Ҫ����������ţ���֤��һ���������
	//m_LST_Ercy.InsertRowAfter(m_LST_Ercy.GetRowCount() - 1, pRow, FALSE);
	//m_LST_Ercy.AdjustLayout();
}
void CDlgK::UpdateListOffset()
{
	//tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//COLORREF colTmp[3];
	//colTmp[OK] = RGB(255, 255, 255);
	//colTmp[NG] = RGB(255, 0, 0);
	//colTmp[WARN] = RGB(248, 182, 43);
	////����ʪ�ϵ���ֵ
	//int nCount = 0;
	//CBCGPGridRow* pRow = m_LST_K.CreateRow(m_LST_K.GetColumnCount());
	////����ߴ�
	//for (int j = 0; j < m_tWetAndDry[0].Wet[0].T().Length(); j++)
	//{
	//	pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	//}
	//if (tPdInfo.nCoatingType == TwoSide)
	//{
	//	for (int j = 0; j < m_tWetAndDry[1].Wet[0].T().Length(); j++)
	//	{

	//		pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
	//	}
	//}
	////���ϵĻ���Ҫ����������ţ���֤��һ���������
	//m_LST_K.InsertRowAfter(m_LST_K.GetRowCount() - 1, pRow, FALSE);
	//m_LST_K.AdjustLayout();

}
void CDlgK::UpdateListCCD()
{
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

void CDlgK::ResetListK(CBCGPGridCtrl& lst)
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
	lst.InsertColumn(nCount++, "λ��", 100);
	for (int i = 0; i < m_tVecK->size(); i++)
	{
		CString strInsert;
		strInsert.Format("K%d", i + 1);
		lst.InsertColumn(nCount++, strInsert, 100);
	}
	lst.InsertColumn(nCount++, "���紦����", 100);
	nCount = 0;
	CBCGPGridRow* pRow = lst.CreateRow(lst.GetColumnCount());
	for (int i = 0; i < lst.GetColumnCount(); i++)
	{
		if (0 == i)
		{
			pRow->GetItem(nCount++)->SetValue(_variant_t("����"), FALSE);
		}
		else if(lst.GetColumnCount()-1 !=i)
		{
			CString strInsert;
			strInsert.Format("%.7f", m_tVecK[0].at(i - 1));
			pRow->GetItem(nCount++)->SetValue(_variant_t(strInsert), FALSE);
		}
		else
		{
			CString strInsert;
			strInsert.Format("%.4f", m_dInternOffset[0]);
			pRow->GetItem(nCount++)->SetValue(_variant_t(strInsert), FALSE);
		}
	}
	lst.InsertRowAfter(lst.GetRowCount() - 1, pRow, FALSE);
	lst.AdjustLayout();
	if (tPdInfo.nCoatingType == TwoSide)
	{
		nCount = 0;
		CBCGPGridRow* pRow = lst.CreateRow(lst.GetColumnCount());
		for (int i = 0; i < lst.GetColumnCount(); i++)
		{
			if (0 == i)
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("����"), FALSE);
			}
			else if (lst.GetColumnCount() - 1 != i)
			{
				CString strInsert;
				strInsert.Format("%.7f", m_tVecK[1].at(i - 1));
				pRow->GetItem(nCount++)->SetValue(_variant_t(strInsert), FALSE);
			}
			else
			{
				CString strInsert;
				strInsert.Format("%.4f", m_dInternOffset[1]);
				pRow->GetItem(nCount++)->SetValue(_variant_t(strInsert), FALSE);
			}
		}
		lst.InsertRowAfter(lst.GetRowCount() - 1, pRow, FALSE);
		lst.AdjustLayout();
	}
	lst.LoadState(_T("BasicGrid"));
	lst.AdjustLayout();//�൱��������ʾ�ڱ����
	lst.ShowWindow(SW_NORMAL);
}
void CDlgK::ResetList(CBCGPGridCtrl& lst)
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
	lst.InsertColumn(nCount++, "λ��", 100);
	for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
	{
		lst.InsertColumn(nCount++, tPdInfo.vecNameList[i], 100);
	}
	nCount = 0;
	CBCGPGridRow* pRow = lst.CreateRow(lst.GetColumnCount());
	for (int i=0;i<lst.GetColumnCount();i++)
	{
		if (0 == i)
		{
			pRow->GetItem(nCount++)->SetValue(_variant_t("����"), FALSE);
		}
		else
		{
			pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
		}
	}
	lst.InsertRowAfter(lst.GetRowCount() - 1, pRow, FALSE);
	lst.AdjustLayout();
	if (tPdInfo.nCoatingType == TwoSide)
	{
		nCount = 0;
		CBCGPGridRow* pRow = lst.CreateRow(lst.GetColumnCount());
		for (int i = 0; i < lst.GetColumnCount(); i++)
		{
			if (0 == i)
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("����"), FALSE);
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
		lst.InsertRowAfter(lst.GetRowCount() - 1, pRow, FALSE);
		lst.AdjustLayout();
	}
	lst.LoadState(_T("BasicGrid"));
	lst.AdjustLayout();//�൱��������ʾ�ڱ����
	lst.ShowWindow(SW_NORMAL);
}
void CDlgK::InitUI()
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
	m_LST_K.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_K, theme);
	m_LST_K.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_K.SetColorTheme(theme);
	ResetList(m_LST_CCD);
	ResetList(m_LST_Ercy);
	ResetListK(m_LST_K);

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	int nCount = 0;
	((CComboBox*)GetDlgItem(IDC_COMBO_INTERNOFFSET))->AddString("��");
	for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_INTERNOFFSET))->AddString(tPdInfo.vecNameList[i]);
	}
	if (1== m_tInitVecK[0].size())//ֻ��һ��Kֵ,��ô��Ϊ��1�����
	{
		m_COM_nInternOffset = 0;
	}
	else
	{
		m_COM_nInternOffset = tPdInfo.vecNameList.size() / 2 + 1;
	}





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

void CDlgK::OnBnClickedButtonWrite()
{
	//�Ȼ�ȡ�����offsetֵ
	UpdateData(TRUE);
	int nColumnCount = m_LST_K.GetColumnCount();
	HTuple hvAllColumn;
	for (int i = 0; i < m_LST_K.GetRowCount(); i++)//�еĺ�
	{
		CBCGPGridRow* pRow = m_LST_K.GetRow(i);
		HTuple hvMean(0), hvMedian(0);
		HTuple hvTuple;
		for (int j = 1; j < nColumnCount; j++)
		{
			CString strValue = pRow->GetItem(j)->GetValue();
			if (j !=  nColumnCount -1)//���һ�����ٽ粹��ֵ
			{
				m_tVecK[i].at(j - 1) = atof(strValue);
			}
			else
			{
			   m_dInternOffset[i] = atof(strValue);
			}
		}
	}
	return OnOK();
}
void CDlgK::OnBnClickedButtonSavedata()
{
	CString strFile;
	strFile.Format("D://�ֶ���������.xls");
	CStdioFile file;
	if (TRUE != file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		CString strException;
		strException.Format("Counter_WriteBatchRecord: ���������ļ�ʧ�� (%s)", strFile);
		theGlobal.m_Log.Exception(strException);
		return;
	}
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
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
	for (int i = 0; i < m_LST_K.GetColumnCount(); i++)
	{
		strLines += m_LST_K.GetColumnName(i);
		strLines += "\t";
	}
	strLines += "\n";
	file.WriteString(strLines);
	for (int i = 0; i < m_LST_K.GetRowCount(); i++)
	{
		CString strLines;
		for (int j = 0; j < m_LST_K.GetColumnCount(); j++)
		{
			CString strValue = m_LST_K.GetRow(i)->GetItem(j)->GetValue();
			strLines += strValue;
			strLines += "\t";
		}
		strLines += "\n";
		file.WriteString(strLines);
	}
	file.Close();
	AfxMessageBox("�����ѱ��浽 D:\\���Զ�����.xls");
}


void CDlgK::OnBnClickedButtonCompute()
{
	//�Ȼ�ȡ�����offsetֵ
	UpdateData(TRUE);
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	double hvSelectCCD[2] = { 0 }, hvUnSelectCCD[2][2] = { 0 };
	for (int i=0;i<m_LST_CCD.GetRowCount();i++)
	{
		CBCGPGridRow *pRow1 = m_LST_CCD.GetRow(i);
		int nCount = 0;
		for (int j=1;j<m_LST_CCD.GetColumnCount();j++)
		{
			CBCGPGridItem* item1 = pRow1->GetItem(j);
			CString str1, str2;
			str1 = item1->GetValue();
			double d1 = atof(str1);
			if (0 != m_COM_nInternOffset)//����������紦
			{
				if (j == m_COM_nInternOffset)
				{
					hvSelectCCD[i] += d1;
				}
				else if (j < m_COM_nInternOffset)
				{
					hvUnSelectCCD[i][0] += d1;
				}
				else if (j > m_COM_nInternOffset)
				{
					hvUnSelectCCD[i][1] += d1;
				}
			}
			else//û��
			{
				hvUnSelectCCD[i][0] += d1;
			}
		}
	}
	double hvSelectErcy[2] = { 0 }, hvUnSelectErcy[2][2] = { 0 };
	for (int i = 0; i < m_LST_Ercy.GetRowCount(); i++)
	{
		CBCGPGridRow *pRow1 = m_LST_Ercy.GetRow(i);
		int nCount = 0;
		for (int j = 1; j < m_LST_Ercy.GetColumnCount(); j++)
		{
			CBCGPGridItem* item1 = pRow1->GetItem(j);
			CString str1, str2;
			str1 = item1->GetValue();
			double d1 = atof(str1);
			if (0 != m_COM_nInternOffset)//����������紦
			{
				if (j == m_COM_nInternOffset)
				{
					hvSelectErcy[i] += d1;
				}
				else if (j < m_COM_nInternOffset)
				{
					hvUnSelectErcy[i][0] += d1;
				}
				else if (j > m_COM_nInternOffset)
				{
					hvUnSelectErcy[i][1] += d1;
				}
			}
			else//û��
			{
				hvUnSelectErcy[i][0] += d1;
			}
		}
	}
	
	for (int i=0;i<2;i++)//��������
	{
		for (int j = 0;j<m_tInitVecK[i].size();j++)//����1���漸��K
		{
			double dK = hvUnSelectErcy[i][j] / (hvUnSelectCCD[i][j] / m_tInitVecK[i].at(j));
			m_tVecK[i].at(j) = min(1.2, max(0.8, dK));
			if (dK < 0.9)
			{
				AfxMessageBox("Kֵ < 0.9,��ע���������");
			}
			if (dK > 1.1)
			{
				AfxMessageBox("Kֵ > 1.1,��ע���������");
			}
		}
		double dInternOffset = hvSelectErcy[i] - hvSelectCCD[i];
		m_dInternOffset[i] = dInternOffset;
	}
	ResetListK(m_LST_K);
}


BOOL CDlgK::PreTranslateMessage(MSG* pMsg)
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

HTuple CDlgK::CopyDataToTuple(CString strData)
{
	HTuple hvData = strData;
	HTuple hvAllData;
	HTuple Length, Position, Substring;
	TupleStrlen(hvData, &Length);
	try
	{
		while (TRUE)
		{
			TupleStrstr(hvData, "\t", &Position);
			TupleStrlen(hvData, &Length);
			if (-1 != Position.I())
			{
				TupleStrFirstN(hvData, Position, &Substring);
				TupleSubstr(hvData, Position + 1, Length - 1, &hvData);

				TupleStrlen(Substring, &Length);
				TupleStrFirstN(Substring, Length - 2, &Substring);
				hvAllData.Append(Substring);
			}
			else
			{
				TupleStrlen(hvData, &Length);
				TupleStrFirstN(hvData, Length - 3, &hvData);
				hvAllData.Append(hvData);
				break;
			}
		}
	}
	catch (HException e)
	{

	}

	return hvAllData;
}
void CDlgK::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
			HTuple hvData = CopyDataToTuple(StrData);
			for (int i=0;i<m_LST_CCD.GetRowCount();i++)
			{
				CBCGPGridRow* pRow = m_LST_CCD.GetRow(i);
				if (pRow->IsSelected())
				{
					for (int j=1;j<m_LST_CCD.GetColumnCount();j++)
					{
						HALCON_TRY(pRow->GetItem(j)->SetValue(hvData[j - 1].S().Text()));
					}
				}
			}
			for (int i = 0; i < m_LST_Ercy.GetRowCount(); i++)
			{
				CBCGPGridRow* pRow = m_LST_Ercy.GetRow(i);
				if (pRow->IsSelected())
				{
					for (int j = 1; j < m_LST_Ercy.GetColumnCount(); j++)
					{
						HALCON_TRY(pRow->GetItem(j)->SetValue(hvData[j - 1].S().Text()));
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


