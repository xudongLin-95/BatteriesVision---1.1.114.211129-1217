#pragma once
#include "PLC.h"
#include "resource.h"
#include "afxwin.h"
#include "BCGCBProInc.h"
// CDlgParamSet 对话框

#define  UM_REFRESH_LIST WM_USER+1002
#define  UM_HEART_BEAT WM_USER+1003

class CDlgParamSet : public CBCGPDialog
{
public:
	enum eValueType//bool;short;int32;float;int64;double;
	{
		VT_bool = 0,
		VT_short,
		VT_int32,
		VT_float,
		VT_double,
		VT_string,
	};

	DECLARE_DYNAMIC(CDlgParamSet)

public:
	CDlgParamSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamSet();

	// 对话框数据
	enum { IDD = IDD_DIALOG_PLC_PARAMSET };

	CPLC *m_pOwner;
private:
	void InitUI();

	/************************************
	*@Access:    private 
	*@Returns:   void
	*@Author: 	 Fowindy
	*@Created:   2021/03/18 10:22
	*@Describe:	 PLC连接按钮刷新状态
	*************************************/
	void RefreshButton();
	void RefreshAddressValueList();
private:
	CBCGPGridCtrl m_LST_Grid_List;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	int m_EDIT_nPort;
	CString m_EDIT_strIP;
	int m_EDIT_nSingleCorrectAddress;	//单面纠偏地址
	int m_EDIT_nDoubleCorrectAddress;	//双面纠偏地址
	int m_EDIT_nPlcHeartBeatAddress;	//PLC心跳地址
	int m_EDIT_nDecimal;
	double m_EDIT_dAlertDiff;
	int m_EDIT_nFilterSize;	//数据滤波尺寸
	double m_EDIT_dCorrectPercent;	//单次纠偏比例（0-1）
	double m_EDIT_dCorrectInterval;	//单次纠偏间距
	double m_EDIT_dCorrectIntervalLong;//单次长纠偏间距
	/************************************
	*@Access:    public 
	*@Returns:   void
	*@Author: 	 Fowindy
	*@Created:   2021/03/18 10:44
	*@Describe:	 PLC设置界面_保存按钮
	*************************************/
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonClose();

	/*int m_Edit_Add;
	int m_Edit_Value;*/

	afx_msg void OnBnClickedBtnWritetest();
	afx_msg void OnBnClickedBtnRead();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CBCGPComboBox m_combCommuType;
	CBCGPComboBox m_combCorrectType;
	CBCGPComboBox m_combCorrectLogicType;
	CBCGPComboBox m_combFitterType;
	CBCGPComboBox m_Combo_CoorectDirection;
	int m_EDIT_nBatchAddress;

public:
	void EnableCorrectPowerLimit(BOOL Enable);
	void ManageCorrectPowerLimit();
	int m_EDT_nGuidFinishAddr;
	int m_EDT_nDoCorrectAddr;
	int m_EDT_nCCDEnableAddress;
	int m_EDT_nPLCRunDirectionAddress;
	double m_EDIT_dCamDistance;
	int m_EDT_nPLCDieHeadDirectionAddress;
	int m_EDIT_nBatchAddressVerify;
	int m_EDT_nPLCStartCoatAddress;
	int m_EDT_nPLCDieHeadPositionAddress;
	int m_EDT_nPLCAlarmAddress;
};
