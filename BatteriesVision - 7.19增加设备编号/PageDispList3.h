#pragma once


// CPageDispList3 �Ի���

class CPageDispList3 : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDispList3)

public:
	CPageDispList3(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageDispList3();

// �Ի�������
	enum { IDD = IDD_PAGE_DISP_LIST3 };

	void ResetSize();
	void ResetList();
	void UpdateList(C_MergeWork& tOutput);
private:
	void InitUI();
private:
	CBCGPGridCtrl m_LST_Grid_List;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
