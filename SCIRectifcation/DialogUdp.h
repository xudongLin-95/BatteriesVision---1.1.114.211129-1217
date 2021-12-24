#pragma once
#include "afxwin.h"
#include "BCGCBProInc.h"
class DHInterfaceEX;
#define WM_UPDATE_MESSAGE (WM_USER+200)

//CBCGPDialog
// DialogUdp 对话框
class DialogUdp : public CBCGPDialog// // CBCGPDialog
{
	DECLARE_DYNAMIC(DialogUdp)

public:
	DialogUdp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DialogUdp();

// 对话框数据
	enum { IDD = IDD_DIALOGUDP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnUpdateMessage(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()
public:
	short m_valR00;
	double m_valR01;
	short m_valR02;
	short m_valR03;
	short m_valR04;
	short m_valR05;
	double m_valR06;
	double m_valR07;
	double m_valR08;
	double m_valR09;
	double m_valR10;
	double m_valR11;
	double m_valR12;
	short m_valR13;
	short m_valR14;
	short m_valR15;
	short m_valR16;
	short m_valR17;
	short m_valR18;
	short m_valR19;
	double m_valR20;
	double m_valR21;
	double m_valR22;
	short m_valR23;
	short m_valR24;
	short m_valR25;
	short m_valR26;
	short m_valR27;
	short m_valR28;
	short m_valT00;
	double m_valT01;
	short m_valT02;
	short m_valT03;
	short m_valT04;
	short m_valT05;
	short m_valT06;
	short m_valT07;
	double m_valT08;
	double m_valT09;
	double m_valT10;
	double m_valT11;
	double m_valT12;
	double m_valT13;
	double m_valT14;
	double m_valT15;
	double m_valT16;
	double m_valT17;
	double m_valT18;
	double m_valT19;
	LONGLONG m_valT20;
	short m_valT24;
	short m_valT25;
	double m_valT26;
	double m_valT27;
	double m_valT28;
	int m_srcPort;
	int m_destPort;
	char m_srcIP[MAX_PATH];
	char m_destIP[MAX_PATH];
	SOCKET m_sock;
	BOOL m_bSendMSG;
	BOOL m_bRecvMSG;
	unsigned long long m_sendMSGSN;
	unsigned long long m_recvMSGSN;
	SYSTEMTIME m_sendTime;
	SYSTEMTIME m_recvTime;
	short m_seData[29];
	short m_reData[29];
	char  m_lpSeData[59];
	char  m_lpReData[59];
	int m_RectifcationSN;
	int m_iDelay;
	double m_nThreshValue;
	double m_nKp;
	CBCGPButton m_checkUse;
	int m_nUse;
	DHInterfaceEX *m_pInterfaceEX;

	PRECTIFICATIONINIT m_pInitParam;

	BOOL Bind(char * lpszIp,int iPort);
	void AddText(char* lpszText,char*lpszCaption);
	static UINT ThreadProc(LPVOID lpVoid);
	void RecvFrom();
	BOOL SendTo(char*lpszIp,int iport);
	void updataRVal(void);
	void ShortToChar(void);
	void CharToShort(void);
	void UpdataTval(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonEditip();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedCheckUse();
	BOOL SendMSG(void);
	void SetContrl(BOOL bFlag);
	BOOL m_bEdit;
	void SendTo1(char*lpszIp,int iport);
	afx_msg void OnBnClickedButtonTest();
	BOOL m_bTest;
	int m_SendStatus;
};
