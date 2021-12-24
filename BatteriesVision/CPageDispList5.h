#pragma once


// CPageDispList5 对话框

class CPageDispList5 : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDispList5)

public:
	CPageDispList5(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageDispList5();

// 对话框数据
	enum { IDD = IDD_PAGE_DISP_LIST5 };

	enum eDefectType
	{
		LeakFoil = 0,	//漏金属
		Drop,		//脱碳
	};

	void ResetSize();
	void UpdateList();
private:
	void InitUI();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CString m_EDIT_sCurrentBatch;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnCbnSelchangeCombo1();
	int m_Combo_nDefectType;
	afx_msg void OnBnClickedButton1();
	std::vector<CString> m_vDefectImageFilePathList;
	HTupleVector m_hvecWndHandle;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
