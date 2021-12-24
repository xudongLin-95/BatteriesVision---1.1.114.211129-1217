#pragma once
#include "resource.h"

// CPageMeasureSet 对话框
class CDlgParam;
class CPageMeasureSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageMeasureSet)

public:
	CPageMeasureSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageMeasureSet();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MEASURE_SET };
//#endif

	CMeasure* m_pOwners;
	CDlgParam* m_pParentDlg;

private:
	void InitUI();
	void RefreshUIData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonMeasurelineReset();
	afx_msg void OnBnClickedCheckShowMeasureline();
	void DispSearchLineParam(int nSN = 0);
	int m_nMeasureLineSN;
	double m_dSearchLineLength;
	int m_nThreshold;
	double m_dSlide;
	int m_nSearchDir;
	int m_nJumpDir;
	int m_nMeasurePointSelect;
	afx_msg void OnEnChangeEditMeasureLength();
	afx_msg void OnEnChangeEditMeasureThres();
	afx_msg void OnEnChangeEditMeasureSmooth();
	afx_msg void OnCbnSelchangeComboMeasureDirection();
	afx_msg void OnCbnSelchangeComboMeasureTransition();
	afx_msg void OnCbnSelchangeComboMeasureSort();
	afx_msg void OnBnClickedButtonSingleLineTest();
	afx_msg void OnBnClickedButtonSingleLineMove();
	double m_EDT_dUncoatAreaRate;
	int m_EDT_nUncoatThresLight;
	int m_EDT_nUncoatThresDark;
	afx_msg void OnBnClickedCheckJudgeUncoat();
	afx_msg void OnEnChangeEditUncoatThresLight();
	afx_msg void OnEnChangeEditUncoatAreaRate();
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnBnClickedButtonRight();
	afx_msg void OnBnClickedButtonMoveLeft();
	afx_msg void OnBnClickedButtonMoveRight();
	afx_msg void OnCbnSelchangeComboFoilSelect();
	afx_msg void OnCbnSelchangeComboMisalignmentSelect();
	int m_Combo_nFoilSelect;
	int m_Combo_nMisalignmentSelect;
	int m_Combo_nCoatSelect;
	afx_msg void OnCbnSelchangeComboCoatSelect();
	afx_msg void OnBnClickedButtonMeasurelineReset2();
	int m_EDIT_nMeasureLeanDiff;
	afx_msg void OnEnChangeEditMeasureLeanDiff();
	afx_msg void OnEnChangeEditUncoatThresDark();
	int m_Combo_nPreproc;
	afx_msg void OnCbnSelchangeComboMeasurePreproc();
};
