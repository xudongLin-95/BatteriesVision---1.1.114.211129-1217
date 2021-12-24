#pragma once
#include "afxwin.h"


// CDlgViewLog 对话框

class CDlgViewLog : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgViewLog)

public:
	CDlgViewLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgViewLog();

// 对话框数据
	enum { IDD = IDD_DIALOG_VIEW_LOG };

	CBCGPGridCtrl m_Grid_Log;

	void InitUI();

	void RefreshLogList();
	void ReadLogFile();
private:
	HTuple	m_hvLog;
	HTuple  m_hvLogLevel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CBCGPDateTimeCtrl m_wndDateTimeLog;
	afx_msg void OnBnClickedDatetimeLog();
	afx_msg void OnCbnSelchangeComboLogDisptype();
	CBCGPComboBox m_COMBO_LogLevel;
	afx_msg void OnCbnSelchangeComboLogLevel();
	CBCGPEdit m_EDIT_SearchLog;
	afx_msg void OnEnChangeEditSearch();
	CString m_EDIT_strSearch;
};
