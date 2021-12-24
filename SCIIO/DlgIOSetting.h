#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "BCGCBProInc.h"

// CDlgIOSetting �Ի���
enum eFenPinMoShi
{
	LineStype,//��ģʽ
	FrameAddLine,//֡����
};


class CDlgIOSetting : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIOSetting)

public:

	CDlgIOSetting(CWnd* pParent = NULL);   // ��׼���캯��

	virtual ~CDlgIOSetting();

// �Ի�������
	enum { IDD = IDD_DIALOG_IOSETTING };

public:

	CFont  m_ftButton;//��ť������
	CFont  m_ftEdit;

	CIOCommunicate *m_pCommuicate;//��ָ��

private:
	void InitUI();
	void InitValue();
	void Save();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	int m_nContinuousErrorAlarmsIO;
	int m_nReadyIO;

	int m_nIOCardSN;
	int m_nRejectType;
	int m_nRejectDelay;
	int m_nPulseWidth;
	int m_nContinuesErrorCountMax;

	double m_dMarkerLengh;
	
	virtual BOOL OnInitDialog();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnBnClickedButtonTestReady();

	afx_msg void OnBnClickedButtonTestContinuouserroralarms();

	afx_msg void OnBnClickedButtonContinuouserrorset();

	afx_msg void OnBnClickedButtonCalliocardparamset();

	afx_msg void OnBnClickedButtonOk();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedCancel();

	afx_msg void OnBnClickedButtonClose();

	int m_EDT_DabiaoDelay;
	int m_nRejectDelay1;//�ڶ�·�������ӳ�����
	int m_EDT_nTiFeiPulse;
	int m_EDT_nBuChangHangShu;
	CBCGPComboBox m_COM_CLabelType;
	int m_COM_nLabelType;
	int m_EDT_nATotalDistannce;
	int m_EDT_nBTotalDistannce;
	int m_EDT_AFrameNum;
	int m_EDT_BFrameNum;
	afx_msg void OnCbnSelchangeComLabeltype();
	afx_msg void OnEnChangeEdtAtotaldistance();
	void Calculate();
	int m_EDT_nImageHeight;
	afx_msg void OnEnChangeEdtBtotaldistance();
};
