// DlgLogin.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PowerLimit.h"
#include "DlgUser.h"
#include "afxdialogex.h"


// DlgLogin �Ի���

IMPLEMENT_DYNAMIC(DlgUser, CBCGPDialog)

DlgUser::DlgUser(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_USER, pParent)
{
	m_pOwner = NULL;
	m_eDlgType = LOGIN_TYPE;
}

DlgUser::~DlgUser()
{
}

void DlgUser::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_POWERLIMIT, m_COMBO_User);
}


BEGIN_MESSAGE_MAP(DlgUser, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &DlgUser::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &DlgUser::OnBnClickedCancel)
END_MESSAGE_MAP()


// DlgLogin ��Ϣ�������
void DlgUser::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPWD, strJobNum;
	GetDlgItem(IDC_EDIT_JOBNUMBER)->GetWindowText(strJobNum);
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(strPWD);
	int nCurSel = m_COMBO_User.GetCurSel()+1;//    YS:Ϊ��ƥ��
	switch (m_eDlgType)
	{
		case LOGIN_TYPE:
		{
			PowerLimit data;
			data.setPowerType(ePOWERLIMITTYPE(nCurSel));
			data.setJobNumber(strJobNum);
			data.setPassword(strPWD);

			/*strCardNum = getCardNumberByJobNumber(strJobNum); //���ݹ��Ų�ѯ������
			data.setCardNumber(strCardNum);*/

			if (m_pOwner->getPowerLimitList().find(data))
			{
				data.setLogin(TRUE);
				if (data.getPowerType() == PT_SUPER_ADMINISTRATOR)// ö�ٵ�0����Ȩ�����ͣ�Ϊ�� 
					data.setJobNumber("��������Ա");
				m_pOwner->setCurPowerLimit(data);
				m_pOwner->setCurPowerType(data.getPowerType());
			}
			else
			{
				BCGPMessageBox("��¼ʧ�ܣ����������룡");
				return;
			}
			break;
		} 
		case ADD_TYPE:
		{
			PowerLimit data;
			CString strCardNum;
			data.setPowerType(ePOWERLIMITTYPE(nCurSel));
			data.setJobNumber(strJobNum);
			data.setPassword(strPWD);
			//��ȡ���ű༭�������
			GetDlgItem(IDC_EDIT_CARDNUMBER)->GetWindowText(strCardNum);
			data.setCardNumber(strCardNum);

			if (strJobNum.IsEmpty() || strPWD.IsEmpty() || strCardNum.IsEmpty())
			{
				BCGPMessageBox("�벹ȫ�û���Ϣ��");
				return;
			}
			else if (m_pOwner->getPowerLimitList().isExit(data))
			{
				BCGPMessageBox("�û��Ѵ��ڣ����������룡");
				return;
			}
			else
			{
				m_pOwner->getPowerLimitList().addData(data);
			}
			break;
		}
		case CHANGE_TYPE:
		{
			CString strCardNum;
			//��ȡ���ű༭�������
			GetDlgItem(IDC_EDIT_CARDNUMBER)->GetWindowText(strCardNum);

			if (strPWD.IsEmpty())
			{
				BCGPMessageBox("�������޸����룡");
				return;
			}
			if (strCardNum.IsEmpty())
			{
				BCGPMessageBox("�������޸Ŀ��ţ�");
				return;
			}
			PowerLimit* pData = m_pOwner->getPowerLimitList().getPowerLimitByJN(strJobNum);
			if ( pData )
			{
				pData->setPowerType(ePOWERLIMITTYPE(nCurSel));
				pData->setPassword(strPWD);
				pData->setCardNumber(strCardNum);
			}
			break;
		}
		default:
			break;
	}

	OnOK();
}


void DlgUser::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	OnCancel();
}


BOOL DlgUser::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//EnableVisualManagerStyle(TRUE,TRUE);
	m_COMBO_User.AddString("����Ա");
	m_COMBO_User.AddString("����Ա");
	m_COMBO_User.AddString("����ʦ");
	m_COMBO_User.AddString("����Ա");
	m_COMBO_User.AddString("��������Ա");
	switch (m_eDlgType)
	{
	case LOGIN_TYPE:
	{
		SetWindowText("�û���¼");
		GetDlgItem(IDC_STATIC_CARD)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CARDNUMBER)->ShowWindow(FALSE);
		m_COMBO_User.SetCurSel(0);
	}
	    break;
	case ADD_TYPE:
	{
		SetWindowText("�û����");
		GetDlgItem(IDC_STATIC_CARD)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_CARDNUMBER)->ShowWindow(TRUE);
		m_COMBO_User.SetCurSel(0);
		GetDlgItem(IDOK)->SetWindowTextA("��Ӳ��˳�");
	}
	    break;
	case CHANGE_TYPE:
	{
		SetWindowText("�û��޸�");
		GetDlgItem(IDC_STATIC_CARD)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_CARDNUMBER)->ShowWindow(TRUE);
		m_COMBO_User.SetCurSel(m_changePL.getPowerType()-1);
		GetDlgItem(IDC_EDIT_JOBNUMBER)->SetWindowText(m_changePL.getJobNumber());
		GetDlgItem(IDC_EDIT_JOBNUMBER)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->SetWindowTextA("�޸Ĳ��˳�");
	}
	    break;
	default:
		break;
	}
	((CBCGPEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->EnablePasswordPreview();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


CString DlgUser:: getCardNumberByJobNumber(CString JobNumber)
{
	PowerLimit data = *(m_pOwner->getPowerLimitList().getPowerLimitByJN(JobNumber));
	return data.getCardNumber();

}