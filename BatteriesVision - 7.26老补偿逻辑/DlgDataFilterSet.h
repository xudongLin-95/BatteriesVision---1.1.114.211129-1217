#pragma once


// CDlgDataFilterSet �Ի���

class CDlgDataFilterSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgDataFilterSet)

public:
	CDlgDataFilterSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDataFilterSet();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DATA_FILTER_SET };
#endif

	void InitUI();
private:
	void RefreshList(CDataFilter::tagTFilterParam &tParam);
	void GetList();
private:
	CBCGPGridCtrl m_Grid_Set;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
