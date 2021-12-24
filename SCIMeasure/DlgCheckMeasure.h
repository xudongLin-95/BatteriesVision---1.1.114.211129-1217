#pragma once
#include "SCIMeasure.in.h"

// CDlgCheckMeasure 对话框

class CDlgCheckMeasure : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgCheckMeasure)

public:
	CDlgCheckMeasure(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCheckMeasure();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CHECK_MEASURE };
#endif

	void InitUI();

public:
	CMeasure*		m_pTemp;		//模拟测试类
	CXmvImageWnd m_ImageWnd[30];
	MeasureInput	m_tInput;		//检测输入
	MeasureOutput	m_tOutput;		//检测输出
	CFont			m_ftTitle;

	CBCGPDialog* m_pParent;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
};
