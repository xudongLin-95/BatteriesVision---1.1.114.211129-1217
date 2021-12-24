#pragma once


// DlgUserPower �Ի���

class DlgUserPower : public CBCGPDialog
{
	DECLARE_DYNAMIC(DlgUserPower)

public:
	DlgUserPower(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgUserPower();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_USERPOWER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

private:
	void InitList();
	void LoadList();
public:
	CBCGPGridCtrl m_LST_Grid_List;
	PowerLimitManage* m_pOwner;
};
