#pragma once
#include "stdafx.h"
#include "resource.h"
#include "BCGCBProInc.h"
#include "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\include\afxwin.h"

// CDlgIOSet �Ի���

enum eFenPinMoShi
{
	LineStype,//��ģʽ
	FrameAddLine,//֡����
};

class CDlgIOSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIOSet)

public:
	CDlgIOSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgIOSet();

// �Ի�������
	enum { IDD = IDD_DIALOG_IOSET };

	CIOCommunicate *m_pCommuicate;//��ָ��

	CImage m_Image;
	 
private:
	void InitUI();
	void InitValue();
	void Save();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
