#pragma once
#include "TabCtrlExt.h"
#include <vector>
#include "PageCameraSet.h"
#include "PageSystemSet.h"
#include "PageAlgSet.h"
#include "PageStorageSet.h"
#include "PageCommunicationSet.h"
#include "PageProductSet.h"
#include "PageOtherSet.h"
#include "PageCheckSet.h"


// CSubDispSetting 对话框

class CSubDispSetting : public CBCGPDialog
{
	DECLARE_DYNAMIC(CSubDispSetting)
public:
	enum eTabItem
	{
		CameraSet = 0,
		AlgSet,
		HistoryReview,
		SystemSet,
		StorageSet,
		CommunicationSet,
		ProductSet,
		OtherSet,
	};
public:
	CSubDispSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubDispSetting();

// 对话框数据
	enum { IDD = IDD_SUB_DISP_SETTING };

	void ResetSize();
	CPageProductSet* m_pPageProductSet;
private:
	std::vector<CBCGPDialog*> m_vecPages;
	CPageCameraSet* m_pPageCameraSet;
	CPageSystemSet* m_pPageSystemSet;
	CPageAlgSet*	m_pPageAlgSet;
	CPageStorageSet* m_pPageStorageSet;
	CPageCommunicationSet* m_pPageCommunicationSet;
	CPageCheckSet* m_pPageCheckSet;
	CPageOtherSet* m_pPageOtherSet;

private:
	void InitUI();
	void SwitchDispPage(const int& nSN);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	CTabCtrlExt m_Tab_Set;
	afx_msg void OnTcnSelchangeTabSet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
