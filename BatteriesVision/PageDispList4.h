#pragma once


// CPageDispList4 对话框

class CPageDispList4 : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDispList4)

public:
	CPageDispList4(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageDispList4();

// 对话框数据
	enum { IDD = IDD_PAGE_DISP_LIST4 };

	void ResetSize();
	void ResetList();
	void UpdateList(C_MergeWork& tOutput);

	void UIPowerLimit();
private:
	void InitUI();
private:
	CBCGPGridCtrl m_LST_Grid_List;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonFai();
	double m_EDIT_dFirstArticleIntervalTime;
	afx_msg void OnEnChangeEditSpotinspectionTime();
	afx_msg void OnPaint();
	int m_EDIT_nFAIIngoreNum;
	int m_EDIT_nFAINum;
	afx_msg void OnEnChangeEditFaiIgnorenum();
	afx_msg void OnEnChangeEditFaiNum();
	afx_msg void OnEnChangeEditFaiTime();
	afx_msg void OnBnClickedButtonFaiCompen();
};
