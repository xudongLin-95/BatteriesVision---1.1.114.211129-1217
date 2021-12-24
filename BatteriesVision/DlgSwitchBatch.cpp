// DlgSwitchBatch.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgSwitchBatch.h"
#include "afxdialogex.h"


// CDlgSwitchBatch 对话框

IMPLEMENT_DYNAMIC(CDlgSwitchBatch, CBCGPDialog)

CDlgSwitchBatch::CDlgSwitchBatch(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSwitchBatch::IDD, pParent)
{

}

CDlgSwitchBatch::~CDlgSwitchBatch()
{
}

void CDlgSwitchBatch::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSwitchBatch, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgSwitchBatch::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GETBATCH, &CDlgSwitchBatch::OnBnClickedButtonGetbatch)
END_MESSAGE_MAP()


// CDlgSwitchBatch 消息处理程序


BOOL CDlgSwitchBatch::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSwitchBatch::InitUI()
{
	GetDlgItem(IDC_EDIT_CURRENT_BATCH)->SetWindowTextA(theGlobal.GetCurrentBatch());

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_DIALOG_SWITCH_BATCH");

	if (theGlobal.m_tSystemSet.eSN != CGlobal::SN_Head)
	{
		GetDlgItem(IDC_BUTTON_GETBATCH)->EnableWindow(FALSE);
	}
}


void CDlgSwitchBatch::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strBatch="";
	GetDlgItem(IDC_EDIT_NEW_BATCH)->GetWindowTextA(strBatch);
	if (""==strBatch)
	{
		theGlobal.m_Language.MessageboxError("卷号不能为空，请检查。", "The roll Number cannot be empty, please check it.", "");
		return;
	}
	if (strBatch.GetLength() > 20)
	{
		theGlobal.m_Language.MessageboxError("卷号字符长度不能超过20，请检查。", "The roll number character length cannot larger than 20, please check it.", "");
		return;
	}
	if (BaseFunction::CheckIllageChar(strBatch) == TRUE)
	{
		theGlobal.m_Language.MessageboxError("卷号不能包含非法字符，请检查。", "The roll number cannot include illegal character, please check it.", "");
		return;
	}
	theGlobal.m_DataCalculate.FinishCalculate();
	theGlobal.m_Counter.EndBatch();
	theGlobal.m_Counter.StartBatch(strBatch);
	theGlobal.m_Storage.GenDataFileSavePath();
	theGlobal.m_Counter.WriteParam();
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
		theGlobal.m_Upload.SendBatchToClient(strBatch);

	//给喷码换卷
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Marking, "SetCurrentBatch", &strBatch, NULL);

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head || theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
	{
		//往PLC写入卷号，用于验证
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "WriteBatchNameVerify", &strBatch, NULL);
	}
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.bCheckProductInfoByChangeBatch)
	{
		//多工位配方校验
		//theGlobal.m_Upload.SendProductInfoToClient();
		theGlobal.SetTipInfo(TIP_CheckProduct);
	}
	
	OnOK();
}

void CDlgSwitchBatch::OnBnClickedButtonGetbatch()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strBatchName = "", strBatch;
	//获取卷号
	BOOL bRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "ReadBatchName", NULL, &strBatchName);
	if (!bRet)
	{
		BCGPMessageBox("获取膜卷号失败,请检查");
		return;
	}
	if ("" == strBatchName)
	{
		BCGPMessageBox("膜卷号为空,请检查");
		return;
	}

	//从plc获取的膜卷号需要高低位反过来,保留前15位
	strBatch = strBatchName.Left(15);

	GetDlgItem(IDC_EDIT_NEW_BATCH)->SetWindowTextA(strBatch);
}
