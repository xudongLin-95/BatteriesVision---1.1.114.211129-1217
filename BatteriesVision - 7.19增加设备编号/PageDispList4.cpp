// PageDispList4.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageDispList4.h"
#include "afxdialogex.h"


// CPageDispList4 �Ի���

IMPLEMENT_DYNAMIC(CPageDispList4, CBCGPDialog)

CPageDispList4::CPageDispList4(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageDispList4::IDD, pParent)
	, m_EDIT_dFirstArticleMeter(0)
{

}

CPageDispList4::~CPageDispList4()
{
}

void CPageDispList4::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SPOTINSPECTION_TIME, m_EDIT_dFirstArticleMeter);
}


BEGIN_MESSAGE_MAP(CPageDispList4, CBCGPDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_FAI, &CPageDispList4::OnBnClickedButtonFai)
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_TIME, &CPageDispList4::OnEnChangeEditSpotinspectionTime)
END_MESSAGE_MAP()


// CPageDispList4 ��Ϣ�������


BOOL CPageDispList4::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CPageDispList4::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//����ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//����ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageDispList4::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageDispList4::ResetSize()
{
	InitUI();
}

void CPageDispList4::InitUI()
{
	/*GetDlgItem(IDC_BUTTON_FAI)->ShowWindow(SW_NORMAL);
	GetDlgItem(IDC_STATIC_CH_PRODUCTPARAM)->ShowWindow(SW_NORMAL);
	GetDlgItem(IDC_STATIC_CH_SPOTINSPECTION_TIME)->ShowWindow(SW_NORMAL);
	GetDlgItem(IDC_EDIT_SPOTINSPECTION_TIME)->ShowWindow(SW_NORMAL);*/

	CBCGPGridColors theme;
	CRect rcGridBase,rcDlg,rcButton;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	GetDlgItem(IDC_BUTTON_FAI)->GetWindowRect(rcButton);
	ScreenToClient(rcButton);
	rcGridBase = rcDlg;
	rcGridBase.top = rcButton.bottom + 15;
	m_LST_Grid_List.Create (WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme(&m_LST_Grid_List, theme);
	m_LST_Grid_List.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Grid_List.SetColorTheme(theme);
	ResetList();

	m_EDIT_dFirstArticleMeter = theGlobal.m_Judge.m_tParam.dFirstArticleInspMeter;
	UpdateData(FALSE);
}

void CPageDispList4::ResetList()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//�������ֵ
	m_LST_Grid_List.RemoveAll();
	m_LST_Grid_List.DeleteAllColumns();
	m_LST_Grid_List.SetReadOnly();//�ɱ༭
	m_LST_Grid_List.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_LST_Grid_List.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_LST_Grid_List.EnableRowHeader(FALSE);//ʹ���б�ͷ
	m_LST_Grid_List.EnableLineNumbers(FALSE);//ʹ�������
	m_LST_Grid_List.SetClearInplaceEditOnEnter(FALSE);
	m_LST_Grid_List.EnableInvertSelOnCtrl(FALSE);
	//m_LST_Grid_List.SetScalingRange(0.1, 2.0);//�������ű�������Ctrl���������ֿ�����
	int nCount = 0;
	m_LST_Grid_List.InsertColumn(nCount++, "ʱ��", 140);
	m_LST_Grid_List.InsertColumn(nCount++, "ͼ�����", 70);

	for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
	{
		m_LST_Grid_List.InsertColumn(nCount++, CString("����" + tPdInfo.vecNameList[i]), 70);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			m_LST_Grid_List.InsertColumn(nCount++, CString("����" + tPdInfo.vecNameList[i]), 70);
		}
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			CString strName;
			strName.Format("��λֵ%d", i + 1);
			m_LST_Grid_List.InsertColumn(nCount++, strName, 70);
		}
	}

	m_LST_Grid_List.LoadState(_T("BasicGrid"));
	m_LST_Grid_List.AdjustLayout();//�൱��������ʾ�ڱ����
	m_LST_Grid_List.ShowWindow(SW_NORMAL);
}

void CPageDispList4::UpdateList(C_MergeWork& tOutput)
{
	if (!theGlobal.m_Judge.m_tParam.bUseFirstArticleInsp) return;

	/*if (theGlobal.m_Project.GetProjectConfig().nCamCount == 4 && theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
		return;
	if (theGlobal.m_Project.GetProjectConfig().nCamCount == 2 && theGlobal.m_Project.GetProjectConfig().nSideCount == 1)
		return;*/

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	CString strTime, strVal;
	strTime.Format("%2dʱ%2d��%2d��%3d����", tTm.wHour, tTm.wMinute, tTm.wSecond, tTm.wMilliseconds);

	if (theGlobal.m_Speed.GetSpeed() > 39 && theGlobal.m_Speed.GetSpeed() <= 50 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 2 != 0) return;
	if (theGlobal.m_Speed.GetSpeed() > 50 && theGlobal.m_Speed.GetSpeed() <= 70 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 3 != 0) return;
	if (theGlobal.m_Speed.GetSpeed() > 70 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 4 != 0) return;
	if (!IsWindowVisible()) return;


	/*HWND hwndList = m_LST_Grid_List.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);*/
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	int nCount = 0;

	CBCGPGridRow* pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
	pRow->GetItem(nCount++)->SetValue(_variant_t(strTime), FALSE);
	//���
	pRow->GetItem(nCount++)->SetValue(_variant_t(tOutput.vecB[0].vecA[0].hvImageSN.I()), FALSE);

	//����ߴ�
	if (tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE)
	{

		for (int i = 0; i < tOutput.vecB[0].tMeasure.tSize.hv_Width.Length(); i++)
		{
			if (tOutput.vecB[0].tMeasure.tSize.hv_Width[i] != NO_DATA)
			{
				strVal.Format("%0.4f", tOutput.vecB[0].tMeasure.tSize.hv_Width[i].D());
				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
				if (tOutput.tMerge.vecSideResult[0].hv_AllWidthFlagList[i].I() == NG)
					pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
				nCount++;
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
	}

	if (tPdInfo.nCoatingType == TwoSide)
	{
		//����ߴ�
		if (tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE && theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
		{
			for (int i = 0; i < tOutput.vecB[1].tMeasure.tSize.hv_Width.Length(); i++)
			{
				if (tOutput.vecB[1].tMeasure.tSize.hv_Width[i] != NO_DATA)
				{
					strVal.Format("%0.4f", tOutput.vecB[1].tMeasure.tSize.hv_Width[i].D());
					pRow->GetItem(nCount)->SetValue(_variant_t(strVal), FALSE);
					if (tOutput.tMerge.vecSideResult[1].hv_AllWidthFlagList[i].I() == NG)
						pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
					nCount++;
				}
				else
				{
					pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
				}
			}
		}

		//A/B���λ
		if (tOutput.tMerge.hv_Misalignment.Length() > 0 && theGlobal.m_Project.GetProjectConfig().nSideCount == 2
			&& tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE && tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE)
		{
			for (int i = 0; i < tOutput.tMerge.hv_Misalignment.Length(); i++)
			{
				if (tOutput.tMerge.hv_Misalignment[i] != NO_DATA)
				{
					strVal.Format("%0.4f", tOutput.tMerge.hv_Misalignment[i].D());
					pRow->GetItem(nCount)->SetValue(_variant_t(strVal), FALSE);
					if (tOutput.tMerge.hv_MisalignmentFlagList[i].I() == NG)
						pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
					nCount++;
				}
				else
				{
					pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
				}
			}
		}
	}

	m_LST_Grid_List.InsertRowBefore(0, pRow, FALSE);

	if (m_LST_Grid_List.GetRowCount() > MAX_LIST_COUNT)
		m_LST_Grid_List.RemoveRow(m_LST_Grid_List.GetRowCount() - 1, FALSE);


	m_LST_Grid_List.AdjustLayout();
}

BOOL CPageDispList4::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//return FALSE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageDispList4::OnBnClickedButtonFai()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	theGlobal.m_Judge.m_tParam.bUseFirstArticleInsp = TRUE;

	while (m_LST_Grid_List.GetRowCount() > 0)
	{
		m_LST_Grid_List.RemoveRow(0, FALSE);
	}
	m_LST_Grid_List.AdjustLayout();
}


void CPageDispList4::OnEnChangeEditSpotinspectionTime()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_Judge.m_tParam.dFirstArticleInspMeter = m_EDIT_dFirstArticleMeter;
	theGlobal.m_Judge.WriteParam();
}
