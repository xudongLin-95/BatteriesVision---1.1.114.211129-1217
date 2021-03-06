#pragma once


// CPageDispList3 对话框

class CPageDispList3 : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDispList3)

public:
	CPageDispList3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageDispList3();

// 对话框数据
	enum { IDD = IDD_PAGE_DISP_LIST3 };

	CJudge::tagSpotInspCount m_tSpotInspCount;

	void ResetSize();
	void ResetList();
	void UpdateList(C_MergeWork& tOutput);
	void RefreshList();

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
	int m_EDIT_nSpotInspectionInterval;
	int m_EDIT_nSpotInspDataNum;
	int m_EDIT_nSpotInspNoDataNum;
	afx_msg void OnEnChangeEditSpotinspectionInterval();
	afx_msg void OnEnChangeEditSpotinspectionDatanum();
	afx_msg void OnEnChangeEditSpotinspectionNodatanum();
	afx_msg void OnBnClickedButtonSpotinspection();
	afx_msg void OnPaint();
	double m_EDIT_dMaxTemp;
	afx_msg void OnEnChangeEditMaxTemp();
	afx_msg void OnBnClickedCheckUseTemp();
	afx_msg LRESULT OnSpotInspectionRefresh(WPARAM wParam, LPARAM lParam);
};
