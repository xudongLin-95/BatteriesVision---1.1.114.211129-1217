#pragma once
#include "afxwin.h"

#define IDC_BTN_BASE 3000

// CDlgTip 对话框
enum eTipType
{
	TIP_CheckProduct = 0,	//检查配方
	TIP_StopPosReason,		//停机位原因
	TIP_FirstArticleNG,		//通知，首件完成情况,NG
	TIP_FirstArticleOK,		//通知，首件完成情况,OK
};
struct tagTipInfo
{
	eTipType				eType;			//类型
	CString					strDiscribe;	//描述
	std::vector<CString>	vecOption;		//选项名
};

class CDlgTip : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgTip)

public:
	CDlgTip(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTip();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIP };
//#endif

	void ShowTip(eTipType eTip);

private:
	std::vector<tagTipInfo> m_vecInfoList;
	tagTipInfo	m_tCurrentInfo;
	CRITICAL_SECTION m_csLock;
	std::vector<CBCGPButton*> m_vecBtnList;
private:
	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CxStatic m_STC_Info;
	CFont m_ftInfo;
	afx_msg void OnNcDestroy();
	afx_msg void OnButtonClick(UINT uID);
};
