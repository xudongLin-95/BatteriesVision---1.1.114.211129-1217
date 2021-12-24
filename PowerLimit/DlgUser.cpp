// DlgLogin.cpp : 实现文件
//

#include "stdafx.h"
#include "PowerLimit.h"
#include "DlgUser.h"
#include "afxdialogex.h"


// DlgLogin 对话框

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


// DlgLogin 消息处理程序
void DlgUser::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPWD, strJobNum;
	GetDlgItem(IDC_EDIT_JOBNUMBER)->GetWindowText(strJobNum);
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(strPWD);
	int nCurSel = m_COMBO_User.GetCurSel()+1;//    YS:为了匹配
	switch (m_eDlgType)
	{
		case LOGIN_TYPE:
		{
			PowerLimit data;
			data.setPowerType(ePOWERLIMITTYPE(nCurSel));
			data.setJobNumber(strJobNum);
			data.setPassword(strPWD);

			/*strCardNum = getCardNumberByJobNumber(strJobNum); //根据工号查询到卡号
			data.setCardNumber(strCardNum);*/

			if (m_pOwner->getPowerLimitList().find(data))
			{
				data.setLogin(TRUE);
				if (data.getPowerType() == PT_SUPER_ADMINISTRATOR)// 枚举的0不是权限类型，为空 
					data.setJobNumber("超级管理员");
				m_pOwner->setCurPowerLimit(data);
				m_pOwner->setCurPowerType(data.getPowerType());
			}
			else
			{
				BCGPMessageBox("登录失败，请重新输入！");
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
			//获取卡号编辑框的内容
			GetDlgItem(IDC_EDIT_CARDNUMBER)->GetWindowText(strCardNum);
			data.setCardNumber(strCardNum);

			if (strJobNum.IsEmpty() || strPWD.IsEmpty() || strCardNum.IsEmpty())
			{
				BCGPMessageBox("请补全用户信息！");
				return;
			}
			else if (m_pOwner->getPowerLimitList().isExit(data))
			{
				BCGPMessageBox("用户已存在，请重新输入！");
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
			//获取卡号编辑框的内容
			GetDlgItem(IDC_EDIT_CARDNUMBER)->GetWindowText(strCardNum);

			if (strPWD.IsEmpty())
			{
				BCGPMessageBox("请输入修改密码！");
				return;
			}
			if (strCardNum.IsEmpty())
			{
				BCGPMessageBox("请输入修改卡号！");
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
	// TODO: 在此添加控件通知处理程序代码

	OnCancel();
}


BOOL DlgUser::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	//EnableVisualManagerStyle(TRUE,TRUE);
	m_COMBO_User.AddString("操作员");
	m_COMBO_User.AddString("技术员");
	m_COMBO_User.AddString("工程师");
	m_COMBO_User.AddString("管理员");
	m_COMBO_User.AddString("超级管理员");
	switch (m_eDlgType)
	{
	case LOGIN_TYPE:
	{
		SetWindowText("用户登录");
		GetDlgItem(IDC_STATIC_CARD)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CARDNUMBER)->ShowWindow(FALSE);
		m_COMBO_User.SetCurSel(0);
	}
	    break;
	case ADD_TYPE:
	{
		SetWindowText("用户添加");
		GetDlgItem(IDC_STATIC_CARD)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_CARDNUMBER)->ShowWindow(TRUE);
		m_COMBO_User.SetCurSel(0);
		GetDlgItem(IDOK)->SetWindowTextA("添加并退出");
	}
	    break;
	case CHANGE_TYPE:
	{
		SetWindowText("用户修改");
		GetDlgItem(IDC_STATIC_CARD)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_CARDNUMBER)->ShowWindow(TRUE);
		m_COMBO_User.SetCurSel(m_changePL.getPowerType()-1);
		GetDlgItem(IDC_EDIT_JOBNUMBER)->SetWindowText(m_changePL.getJobNumber());
		GetDlgItem(IDC_EDIT_JOBNUMBER)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->SetWindowTextA("修改并退出");
	}
	    break;
	default:
		break;
	}
	((CBCGPEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->EnablePasswordPreview();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


CString DlgUser:: getCardNumberByJobNumber(CString JobNumber)
{
	PowerLimit data = *(m_pOwner->getPowerLimitList().getPowerLimitByJN(JobNumber));
	return data.getCardNumber();

}