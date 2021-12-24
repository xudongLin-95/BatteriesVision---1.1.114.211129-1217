#pragma once

// CPageProductSet 对话框

class CPageProductSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageProductSet)

public:
	CPageProductSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageProductSet();

// 对话框数据
	enum { IDD = IDD_PAGE_PRODUCT_SET };

	void ResetSize();
private:
	CBCGPGridCtrl m_Grid_ProductList;
	CBCGPGridCtrl m_Grid_ProductParam;			//产品规格
	int m_nCurrentProductSel;//当前使用品种的序号
	int m_nProductSel;//当前选择显示参数的品种序号
	CFont m_ftProp;

	int m_nGridRowSN, m_nGridOldRowSN;
	int m_nGridColSN, m_nGridOldColSN;
private:
	void InitUI();
	void RefeshProductList();
	void RenameProductList();
	void EditRefreshProductParam();
	void RefreshProductParam();
	void RefreshProductParam(tagPDInfo& tPdInfo);
	tagPDInfo GetProductParam();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonProductSet();
	afx_msg LRESULT OnEndEditGridProduct(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBeginEditGridProduct(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemChangedGridProduct(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonProductNew();
	afx_msg void OnBnClickedButtonProductLoad();
	afx_msg void OnBnClickedButtonProductDelete();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonProductSave();
};
