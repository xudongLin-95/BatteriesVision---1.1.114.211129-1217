// PageAlgSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageAlgSet.h"
#include "afxdialogex.h"
#include "DlgDataFilterSet.h"


// CPageAlgSet 对话框

IMPLEMENT_DYNAMIC(CPageAlgSet, CBCGPDialog)

CPageAlgSet::CPageAlgSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageAlgSet::IDD, pParent)
{

}

CPageAlgSet::~CPageAlgSet()
{
}

void CPageAlgSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageAlgSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_MEASURE0, &CPageAlgSet::OnBnClickedButtonSetMeasure0)
	ON_BN_CLICKED(IDC_BUTTON_SET_MEASURE1, &CPageAlgSet::OnBnClickedButtonSetMeasure1)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_SET_DATA_FILTER, &CPageAlgSet::OnBnClickedButtonSetDataFilter)
	ON_BN_CLICKED(IDC_BUTTON_CALIBRATE, &CPageAlgSet::OnBnClickedButtonCalibrate)
END_MESSAGE_MAP()


// CPageAlgSet 消息处理程序


BOOL CPageAlgSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageAlgSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageAlgSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageAlgSet::ResetSize()
{
	InitUI();
}

void CPageAlgSet::InitUI()
{
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_MEASURE0))->SetTooltip("Set camera 0 algorithm params\r\n设置0面算法参数");
	((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_MEASURE1))->SetTooltip("Set camera 1 algorithm params\r\n设置1面算法参数");
	
	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_ALG_SET");

	if (theGlobal.m_Project.GetProjectConfig().nSideCount == 1)
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_SET_MEASURE1))->ShowWindow(SW_HIDE);
}


void CPageAlgSet::OnBnClickedButtonSetMeasure0()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Detect, NULL)) return;
	int nSN = 0;
	
	MeasureInput tInput;
	tInput.nMode = DebugMode;
	GenEmptyObj(&tInput.hoGrabImage);
	HTuple hvGrabSNList = theGlobal.m_Project.GetSideToCamSNList(nSN);
	for (int k = 0; k < hvGrabSNList.Length(); k++)
	{
		HObject hoTmp;
		if (theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O().CountObj() > 0)
			SelectObj(theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O(), &hoTmp, theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O().CountObj());
		else
			GenImageConst(&hoTmp, "byte", 800, 600);
		ConcatObj(tInput.hoGrabImage, hoTmp, &tInput.hoGrabImage);
	}

	theGlobal.m_AlgTool.ShowParamDlg(nSN, CAlgorithmTool::AlgMeasure, &tInput);
}


void CPageAlgSet::OnBnClickedButtonSetMeasure1()
{
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Detect, NULL)) return;
	int nSN = 1;
	
	MeasureInput tInput;
	tInput.nMode = DebugMode;
	GenEmptyObj(&tInput.hoGrabImage);
	HTuple hvGrabSNList = theGlobal.m_Project.GetSideToCamSNList(nSN);
	for (int k = 0; k < hvGrabSNList.Length(); k++)
	{
		HObject hoTmp;
		if (theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O().CountObj() > 0)
			SelectObj(theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O(), &hoTmp, theGlobal.m_hovecHistroyImage[hvGrabSNList[k]].O().CountObj());
		else
			GenImageConst(&hoTmp, "byte", 800, 600);
		ConcatObj(tInput.hoGrabImage, hoTmp, &tInput.hoGrabImage);
	}

	theGlobal.m_AlgTool.ShowParamDlg(nSN, CAlgorithmTool::AlgMeasure, &tInput);
}


void CPageAlgSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBCGPDialog::OnPaint()
	ShowMesageJudgeByPowerLimit();

	ShowHiddenSetting();
}

void CPageAlgSet::ShowMesageJudgeByPowerLimit()
{
	try
	{
		if (theGlobal.m_Permission.CheckPermission(PM_Detect))
		{
			GetDlgItem(IDC_BUTTON_SET_MEASURE0)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SET_MEASURE1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SET_DATA_FILTER)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_SET_MEASURE0)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SET_MEASURE1)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SET_DATA_FILTER)->EnableWindow(FALSE);
		}
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}
}

void CPageAlgSet::ShowHiddenSetting()
{
	if (theGlobal.m_Permission.GetPermission() == PT_FACTORY || theGlobal.m_Permission.GetPermission() == PT_SUPER_ADMINISTRATOR)
	{
		GetDlgItem(IDC_BUTTON_SET_DATA_FILTER)->ShowWindow(SW_NORMAL);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SET_DATA_FILTER)->ShowWindow(SW_HIDE);
	}
}

void CPageAlgSet::OnBnClickedButtonSetDataFilter()
{
	CDlgDataFilterSet dlg;
	dlg.DoModal();
}


void CPageAlgSet::OnBnClickedButtonCalibrate()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!theGlobal.m_pMainDlg->SendMessage(WM_CHECK_PMSN, PM_Detect, NULL)) return;
}
