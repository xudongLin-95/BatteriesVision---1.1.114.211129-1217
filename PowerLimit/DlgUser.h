#pragma once


// DlgLogin 对话框

class DlgUser : public CBCGPDialog
{
	DECLARE_DYNAMIC(DlgUser)

public:
	DlgUser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgUser();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_USER };
#endif

	enum DLGTYPE//对话框类型
	{
		LOGIN_TYPE = 0,//登录
		ADD_TYPE,//添加
		CHANGE_TYPE,//修改

		DLG_COUNT
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
