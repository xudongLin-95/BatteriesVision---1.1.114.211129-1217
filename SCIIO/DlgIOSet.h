#pragma once
#include "stdafx.h"
#include "resource.h"
#include "BCGCBProInc.h"
#include "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\include\afxwin.h"

// CDlgIOSet 对话框

enum eFenPinMoShi
{
	LineStype,//行模式
	FrameAddLine,//帧加行
};

class CDlgIOSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIOSet)

public:
	CDlgIOSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgIOSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_IOSET };

	CIOCommunicate *m_pCommuicate;//类指针

	CImage m_Image;
	 
private:
	void InitUI();
	void InitValue();
	void Save();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int m_EDT_nIOCardSN;
	int m_nRejectType;
	afx_msg void OnBnClickedButtonCalliocardparamset();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	BOOL m_CHECK_bLXTB0;
	BOOL m_CHECK_bLXTB1;
	afx_msg void OnBnClickedButtonIoGreen();
	afx_msg void OnBnClickedButtonIoRed();
	afx_msg void OnBnClickedButtonIoBuzeer();
	CBCGPComboBox m_ChoseIO_Green;
	CBCGPComboBox m_ChoseIO_Red;
	CBCGPComboBox m_ChoseIO_Buzzer;
	afx_msg void OnBnClickedButtonIoYellow();
	CBCGPComboBox m_ChoseIO_Yellow;
};
