#pragma once


// CPageDispList 对话框

class CPageDispList : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDispList)

public:
	CPageDispList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageDispList();

// 对话框数据
	enum { IDD = IDD_PAGE_DISP_LIST };

	void ResetSize();
	void ResetList();
	void UpdateList(C_MergeWork& tOutput);
private:
	void InitUI();
private:
	CBCGPGridCtrl m_LST_Grid_List;			//数据列表
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
