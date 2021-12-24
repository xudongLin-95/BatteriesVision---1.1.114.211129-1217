#pragma once
#include "afxwin.h"


// CPageSystemSet 对话框

class CPageSystemSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageSystemSet)

public:
	CPageSystemSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageSystemSet();

// 对话框数据
	enum { IDD = IDD_PAGE_SYSTEM_SET };
	void ResetSize();
private:
	void InitUI();
	void EnableWidgetsWithPermenssion();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	CBCGPListCtrl m_LST_Resolution;
	CBCGPComboBox m_COMBO_HistoryType;
	int m_EDIT_n2CamDistance;
	int m_EDIT_nHistoryNum;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeEditHistorynum();
	afx_msg void OnCbnSelchangeComboHistorytype();
	afx_msg void OnBnClickedCheckGrabAlign();
	afx_msg void OnEnChangeEdit2camDistance();
	afx_msg void OnBnClickedButtonClearBatchCount();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int m_COM_nActionMode;
	afx_msg void OnCbnSelchangeComboActionMode();
	CString m_EDIT_strMachineID;
	CString m_EDIT_strWorkShopID;
	afx_msg void OnEnChangeEditWorkshopId();
	afx_msg void OnEnChangeEditMachineId();
	double m_EDIT_dSelectMeter;
	afx_msg void OnBnClickedCheckDataCalculate();
	afx_msg void OnEnChangeEditSelectMeter();
	afx_msg void OnBnClickedCheckChangeBatchAuto();
	afx_msg void OnEnChangeEditBatchDelayDistance();
	double m_EDIT_dBatchDelayDistance;
	double m_EDIT_dToNextStationMeter;
	afx_msg void OnBnClickedCheckUseNet();
	afx_msg void OnBnClickedCheckDataCompensationAtuo();
	afx_msg void OnEnChangeEditToNextMeter();
	afx_msg void OnCbnSelchangeComboStation();
	int m_Combo_nStationSN;
	afx_msg void OnBnClickedButtonIpSet();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ShowMesageJudgeByPowerLimit();
	double m_EDT_dScale;
	afx_msg void OnEnChangeEditScale();
	int m_EDIT_nSpotInspectionTime;
	afx_msg void OnEnChangeEditSpotinspectionTime();
	afx_msg void OnEnChangeEditCompenInterval();
	double m_EDIT_dCompenInterval;
	afx_msg void OnBnClickedCheckDataCompensationInverse();
	double m_EDIT_dStartIgnoreMeter;
	afx_msg void OnEnChangeEditStartIgnoreMeter();
	BOOL m_CHECK_bUseDataFilte;
	afx_msg void OnBnClickedCheckUseDataFilter();
	afx_msg void OnBnClickedButtonProductCheck();
	afx_msg void OnBnClickedCheckUseProductCheck();
	double m_EDIT_dCompenLimit;
	afx_msg void OnEnChangeEditCompLimit();
	int m_Combo_nDeviceType;
	afx_msg void OnCbnSelchangeComboDeviceType();
};
