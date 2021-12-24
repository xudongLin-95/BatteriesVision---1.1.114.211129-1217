#pragma once
#include "TabCtrlExt.h"
#include "PageDispList.h"
#include "PageDispList2.h"
#include "PageDispList3.h"
#include "PageDispList4.h"
#include "CPageDispList5.h"

#define MAX_LIST_COUNT 50
// CSubDispList 对话框

class CSubDispList : public CBCGPDialog
{
	DECLARE_DYNAMIC(CSubDispList)

public:
	CSubDispList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubDispList();

	// 对话框数据
	enum { IDD = IDD_SUB_DISP_LIST };


	CPageDispList* m_pPageDispMeasureWidth;
	CPageDispList2* m_pPageDispDefect;
	CPageDispList3* m_pPageDispSpotInspection;
	CPageDispList4* m_pPageDispFAI;//首件
	CPageDispList5*m_pPageDispInquire;

	std::vector<CBCGPDialog*> m_vecPages;

	void InitUI();
	void ResetSize();
	void UpdateData(C_MergeWork& tOutput);
private:
	void SwitchDispPage(const int& nSN);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CBCGPGridCtrl m_LST_Grid1;
	CBCGPGridCtrl m_LST_Grid2;
	CBCGPGridCtrl m_LST_Grid3;
	virtual void PostNcDestroy();
	CTabCtrlExt m_TAB_List;
	afx_msg void OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnRefreshDisp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
