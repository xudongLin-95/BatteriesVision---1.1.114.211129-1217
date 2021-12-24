#pragma once
#include "DHSocket.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "SCIRectifcation.h"
#include "stdafx.h"


// CDlgDHSocket 对话框
class CDHSocket;

class CDlgDHSocket : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgDHSocket)

public:
	CDlgDHSocket(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDHSocket();

// 对话框数据
	enum { IDD = IDD_DIALOG_DHSOCKET };

	void RecvData(CString strText);
	void SendData(CString strText);
	void OnSocketSend();

	CDHSocket* m_pSocket;//当前加载的Socket
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_LST_Recive;
	afx_msg void OnBnClickedButtonOffset();
	double m_EDT_dOffset;
	
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	CIPAddressCtrl m_Address_SrcIP;
	CIPAddressCtrl m_Address_Offset;
	int m_EDT_nSrcPort;
	int m_EDT_nOffsetPort;
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedButtonSave();
	int m_EDT_nDelay;
	double m_EDT_dThreshValue;
	double m_EDT_dKP;
	int m_COM_nPIDEnable;
	afx_msg void OnBnClickedButtonOffget();
	int m_EDT_nFrameSeg;
	int m_EDT_nFrameUse;
	int m_EDT_bEnableDiff;
	CListBox m_LST_Send;
	afx_msg void OnBnClickedButtonCancel();
	double m_EDT_dOffsetMax;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnDispList(WPARAM wParam, LPARAM lParam);
};
