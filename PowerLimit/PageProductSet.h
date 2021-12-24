#pragma once

// CPageProductSet �Ի���

class CPageProductSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageProductSet)

public:
	CPageProductSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageProductSet();

// �Ի�������
	enum { IDD = IDD_PAGE_PRODUCT_SET };

	void ResetSize();
private:
	CBCGPGridCtrl m_Grid_ProductList;
	CBCGPGridCtrl m_Grid_ProductParam;			//��Ʒ���
	int m_nCurrentProductSel;//��ǰʹ��Ʒ�ֵ����
	int m_nProductSel;//��ǰѡ����ʾ������Ʒ�����
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
