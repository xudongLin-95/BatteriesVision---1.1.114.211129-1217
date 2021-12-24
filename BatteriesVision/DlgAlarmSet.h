#pragma once


// CDlgAlarmSet 对话框

class CDlgAlarmSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAlarmSet)

public:
	CDlgAlarmSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAlarmSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALARM_SET };
#endif

	void InitUI();

	void RefreshSizeList();
	void RefreshDefectList();
public:
	CBCGPGridCtrl m_Grid_SizeAlarm;
	CBCGPGridCtrl m_Grid_DefectAlarm;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	double m_EDIT_dIngoreMeter;
	double m_EDIT_dMeanShiftFactor;
};
