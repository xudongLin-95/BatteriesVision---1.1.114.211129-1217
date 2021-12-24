#pragma once
#include "afxwin.h"


// CDlgInfo �Ի���
#define WM_UPDATE_MESSAGE (WM_USER+200)
class CDlgInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInfo)

public:
	CDlgInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgInfo();

// �Ի�������
	enum { IDD = IDD_DLGINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg LRESULT OnUpdateMessage(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
public:
	CString m_strUploadInfo;
	int m_iCount;
	int m_iComplete;
	CStatic m_staticInfo;
	SYSTEMTIME m_beginTime;
	SYSTEMTIME m_EndTime;
	
};
__int64 TimeDiff(SYSTEMTIME left, SYSTEMTIME right);

BOOL MillisecondToHours(__int64 const& Millisecond,CString& strHours);
