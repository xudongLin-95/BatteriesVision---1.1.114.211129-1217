#pragma once


// CPageDispList �Ի���

class CPageDispList : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDispList)

public:
	CPageDispList(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageDispList();

// �Ի�������
	enum { IDD = IDD_PAGE_DISP_LIST };

	void ResetSize();
	void ResetList();
	void UpdateList(C_MergeWork& tOutput);
private:
	void InitUI();
private:
	CBCGPGridCtrl m_LST_Grid_List;			//�����б�
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
