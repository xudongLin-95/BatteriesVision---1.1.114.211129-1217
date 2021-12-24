#pragma once


// DlgLogin �Ի���

class DlgUser : public CBCGPDialog
{
	DECLARE_DYNAMIC(DlgUser)

public:
	DlgUser(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgUser();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_USER };
#endif

	enum DLGTYPE//�Ի�������
	{
		LOGIN_TYPE = 0,//��¼
		ADD_TYPE,//���
		CHANGE_TYPE,//�޸�

		DLG_COUNT
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CString getCardNumberByJobNumber(CString JobNumber);
public:
	PowerLimitManage* m_pOwner;
	DLGTYPE m_eDlgType;
	PowerLimit m_changePL;
private:
	CBCGPComboBox m_COMBO_User;
public:
	virtual BOOL OnInitDialog();
	
};
