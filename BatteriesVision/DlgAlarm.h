#pragma once
#include "ErrorInfo.h"
#include "afxwin.h"
// CDlgAlarm �Ի���

class CDlgAlarm : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAlarm)

public:
	CDlgAlarm(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAlarm();

// �Ի�������
	enum { IDD = IDD_DIALOG_ALARM };

	void ShowAlarm(CString strAlarmInfo);
	void ShowAlarm(int nErrorCode);
	void ShowAlarm(CErrorInfo::tagErrorInfo tInfo);

private:
	CFont m_ftInfo;
	CFont m_ftDiscribe;
	CFont m_ftSolution;
	CFont m_ftObject;

	std::vector<CErrorInfo::tagErrorInfo> m_vecErrorInfoList;//ʵʱ�����洢
	CRITICAL_SECTION m_csLock;

	CErrorInfo::tagErrorInfo m_tInfo;
private:
	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	//XML�ļ����
	std::vector<CErrorInfo::tagErrorInfo> m_vecAllErrorInfos;//���ؼ����б�
	CErrorInfo m_cErrorInfo;
	CString m_strErrorCodeFilePath;       //·����
	BOOL GetErrorCodeListFromXml();       //��Xml�ļ���ȡ�������б�
	
	CListCtrl m_LIST_Show;
};
