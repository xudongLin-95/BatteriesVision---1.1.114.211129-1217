// DlgSwitchBatch.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgSwitchBatch.h"
#include "afxdialogex.h"


// CDlgSwitchBatch �Ի���

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


// CDlgSwitchBatch ��Ϣ�������


BOOL CDlgSwitchBatch::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strBatch="";
	GetDlgItem(IDC_EDIT_NEW_BATCH)->GetWindowTextA(strBatch);
	if (""==strBatch)
	{
		theGlobal.m_Language.MessageboxError("��Ų���Ϊ�գ����顣", "The roll Number cannot be empty, please check it.", "");
		return;
	}
	if (strBatch.GetLength() > 20)
	{
		theGlobal.m_Language.MessageboxError("����ַ����Ȳ��ܳ���20�����顣", "The roll number character length cannot larger than 20, please check it.", "");
		return;
	}
	if (BaseFunction::CheckIllageChar(strBatch) == TRUE)
	{
		theGlobal.m_Language.MessageboxError("��Ų��ܰ����Ƿ��ַ������顣", "The roll number cannot include illegal character, please check it.", "");
		return;
	}
	theGlobal.m_DataCalculate.FinishCalculate();
	theGlobal.m_Counter.EndBatch();
	theGlobal.m_Counter.StartBatch(strBatch);
	theGlobal.m_Storage.GenDataFileSavePath();
	theGlobal.m_Counter.WriteParam();
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
		theGlobal.m_Upload.SendBatchToClient(strBatch);

	//�����뻻��
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Marking, "SetCurrentBatch", &strBatch, NULL);

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head || theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
	{
		//��PLCд���ţ�������֤
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "WriteBatchNameVerify", &strBatch, NULL);
	}
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.bCheckProductInfoByChangeBatch)
	{
		//�๤λ�䷽У��
		//theGlobal.m_Upload.SendProductInfoToClient();
		theGlobal.SetTipInfo(TIP_CheckProduct);
	}
	
	OnOK();
}

void CDlgSwitchBatch::OnBnClickedButtonGetbatch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strBatchName = "", strBatch;
	//��ȡ���
	BOOL bRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "ReadBatchName", NULL, &strBatchName);
	if (!bRet)
	{
		BCGPMessageBox("��ȡĤ���ʧ��,����");
		return;
	}
	if ("" == strBatchName)
	{
		BCGPMessageBox("Ĥ���Ϊ��,����");
		return;
	}

	//��plc��ȡ��Ĥ�����Ҫ�ߵ�λ������,����ǰ15λ
	strBatch = strBatchName.Left(15);

	GetDlgItem(IDC_EDIT_NEW_BATCH)->SetWindowTextA(strBatch);
}
