#pragma once
#include "resource.h"
#include "afxwin.h"

// CPageSpotInspectSet 对话框
class CDlgParam;
class CPageSpotInspectSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageSpotInspectSet)

public:
	CPageSpotInspectSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageSpotInspectSet();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SPOTINSPECT_SET };
//#endif

	CMeasure* m_pOwners;
	CDlgParam* m_pParentDlg;

private:
	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonGenParm();
	afx_msg void OnCbnEditchangeComboSpotinsptype();
	afx_msg void OnCbnEditchangeComboSpotinspnum();
	afx_msg void OnEnChangeEditMinsize();
	afx_msg void OnEnChangeEditSpthreshold();
	CBCGPComboBox m_combType;
	CBCGPComboBox m_combObjNum;
	double m_edit_dMinSizePixel;
	int m_edit_nThreshold;
	void RefreshUIData();
	afx_msg void OnBnClickedButtonGenSpotinspectRegion();
	CBCGPComboBox m_comSpotInspColor;
	afx_msg void OnCbnSelchangeComboSpotinspColor();
	double m_edit_dMaxSizePixel;
	afx_msg void OnEnChangeEditMaxsize();
};
