#pragma once
#include "LoggerManager.h"
#include "BCGCBProInc.h"

// CLogViewDlg 对话框

class CLogViewDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CLogViewDlg)

public:
	CLogViewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogViewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGVIEWDLG };
#endif

private:
	CBCGPGridCtrl m_Grid_Log;
	CBCGPDateTimeCtrl m_wndDateTimeLog;
	CBCGPComboBox m_COMBO_LogLevel;
	CBCGPEdit m_EDIT_SearchLog;
	CString m_EDIT_strSearch;

public:
	std::vector<CLogger*> m_vecLogger;
	std::vector<CString> m_vecCstrLog;
	std::vector<CString> m_vecCstrSearch;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDatetimeLog();
	afx_msg void OnCbnSelchangeComboLogDisptype();
	afx_msg void OnCbnSelchangeComboLogLevel();
	afx_msg void OnEnChangeEditSearch();
	virtual BOOL OnInitDialog();
	void InitUI();
	void ReadLog();
	void UpdateList();
	void UpdateList(std::vector<CString>& vecCstr);
	void SearchLog(const CString & cstrSearch ,const std::vector<CString>& vecLogRead,std::vector<CString>& vecLogSearch);
	virtual void PostNcDestroy();
};
