// PageDispList3.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageDispList3.h"
#include "afxdialogex.h"


// CPageDispList3 �Ի���

IMPLEMENT_DYNAMIC(CPageDispList3, CBCGPDialog)

CPageDispList3::CPageDispList3(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageDispList3::IDD, pParent)
{

}

CPageDispList3::~CPageDispList3()
{
}

void CPageDispList3::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageDispList3, CBCGPDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CPageDispList3 ��Ϣ�������


BOOL CPageDispList3::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CPageDispList3::PreTranslateMessage(MSG* pMsg)
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


void CPageDispList3::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageDispList3::ResetSize()
{
	InitUI();
}

void CPageDispList3::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase,rcDlg;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	rcGridBase = rcDlg;
	m_LST_Grid_List.Create (WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme(&m_LST_Grid_List, theme);
	m_LST_Grid_List.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Grid_List.SetColorTheme(theme);
	ResetList();
}

void CPageDispList3::ResetList()
{
	BOOL bChinese = theGlobal.m_Language.IsChinese();
	m_LST_Grid_List.RemoveAll();
	m_LST_Grid_List.DeleteAllColumns();
	m_LST_Grid_List.SetReadOnly();//�ɱ༭
	m_LST_Grid_List.EnableMarkSortedColumn (FALSE);//ʹ��������
	m_LST_Grid_List.EnableHeader (TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_LST_Grid_List.EnableRowHeader (FALSE);//ʹ���б�ͷ
	m_LST_Grid_List.EnableLineNumbers (FALSE);//ʹ�������
	m_LST_Grid_List.SetClearInplaceEditOnEnter (FALSE);
	m_LST_Grid_List.EnableInvertSelOnCtrl (FALSE);
	//m_LST_Grid_List.SetScalingRange (0.1, 2.0);//�������ű�������Ctrl���������ֿ�����
	m_LST_Grid_List.InsertColumn(0, "���/ʱ��", 120);
	//���
	int nCount = 1;
	for (int i = 0; i < theGlobal.m_Project.GetProjectConfig().nSideCount; i++)
	{
		CString strName;
		strName.Format("%s���(mm)", i == 0 ? "��" : "��");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s���(mm)", i == 0 ? "��" : "��");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s�ҿ�(mm)", i == 0 ? "��" : "��");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s�Ҹ�(mm)", i == 0 ? "��" : "��");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s���(mm)", i == 0 ? "��" : "��");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
	}
	m_LST_Grid_List.LoadState(_T("BasicGrid"));
	m_LST_Grid_List.AdjustLayout();//�൱��������ʾ�ڱ����
	m_LST_Grid_List.ShowWindow(SW_NORMAL);
}

void CPageDispList3::UpdateList(C_MergeWork& tOutput)
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (!tPdInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode) return;

	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	CString strTime, strVal;
	strTime.Format("%2dʱ%2d��%2d��%3d����", tTm.wHour, tTm.wMinute, tTm.wSecond, tTm.wMilliseconds);

	/*HWND hwndList = m_LST_Grid_List.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);*/
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);

	//�ж������Ƿ���Ч
	HTuple hvSize;
	for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
	{
		hvSize.Append(tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
	}
	HTuple hvInc;
	TupleFind(hvSize, NO_DATA, &hvInc);
	if ((hvInc.Length() >= 1 && hvInc != -1) || hvInc.Length() == 0 || hvSize.Length() != 10)
	{
		return;
	}

	//���
	if (tOutput.vecB[0].tMeasure.tSpotInspection.hv_Width.Length() == 5)
	{
		CBCGPGridRow* pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t(strTime), FALSE);
		int nCount = 1;
		for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
		{
			HTuple hv = tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width;
			for (int i = 0; i < 5; i++)
			{
				if (tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width.Length() > i
					&& tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width[i] != NO_DATA)
				{
					pRow->GetItem(nCount)->SetValue(_variant_t((tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width[i].D())), FALSE);
					if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_SpotInspectionFlagList[i].I() == NG)
						pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
				}
				else
				{
					pRow->GetItem(nCount)->SetValue(_variant_t("-"), FALSE);
				}
				nCount++;
			}
		}
		m_LST_Grid_List.InsertRowBefore(0, pRow, FALSE);
	}

	if (m_LST_Grid_List.GetRowCount() > MAX_LIST_COUNT)
		m_LST_Grid_List.RemoveRow(m_LST_Grid_List.GetRowCount() - 1, FALSE);

	m_LST_Grid_List.AdjustLayout();
}

BOOL CPageDispList3::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}
