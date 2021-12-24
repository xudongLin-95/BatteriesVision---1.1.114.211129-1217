#pragma once
#include "ErrorInfo.h"
#include "afxwin.h"
// CDlgAlarm 对话框

class CDlgAlarm : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAlarm)

public:
	CDlgAlarm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAlarm();

// 对话框数据
	enum { IDD = IDD_DIALOG_ALARM };

	void ShowAlarm(CString strAlarmInfo);
	void ShowAlarm(int nErrorCode);
	void ShowAlarm(CErrorInfo::tagErrorInfo tInfo);

private:
	CFont m_ftInfo;
	CFont m_ftDiscribe;
	CFont m_ftSolution;
	CFont m_ftObject;

	std::vector<CErrorInfo::tagErrorInfo> m_vecErrorInfoList;//实时报警存储
	CRITICAL_SECTION m_csLock;

	CErrorInfo::tagErrorInfo m_tInfo;
private:
	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	/*CxStatic m_STC_Discribe;
	CxStatic m_STC_Info;
	CxStatic m_STC_Solution;*/
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	//XML文件相关
	std::vector<CErrorInfo::tagErrorInfo> m_vecAllErrorInfos;//本地加载列表
	CErrorInfo m_cErrorInfo;
	CString m_strErrorCodeFilePath;       //路径名
	BOOL GetErrorCodeListFromXml();       //从Xml文件获取错误码列表
	
	CListCtrl m_LIST_Show;
};
