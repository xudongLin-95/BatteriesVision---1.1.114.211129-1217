#pragma once


// CPageCheckSet 对话框

class CPageCheckSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageCheckSet)

public:
	CPageCheckSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageCheckSet();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_CHECK_SET };
//#endif

	void ResetSize();

private:
	void InitUI();
	void RefeshSpotInspectionParam();
	tagPDInfo GetSpotInspectionParam();

private:
	CBCGPGridCtrl m_Grid_SpotInspectionParam;			//点检参数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonSpotinspection();
};
