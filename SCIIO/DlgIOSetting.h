#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "BCGCBProInc.h"

// CDlgIOSetting 对话框
enum eFenPinMoShi
{
	LineStype,//行模式
	FrameAddLine,//帧加行
};


class CDlgIOSetting : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIOSetting)

public:

	CDlgIOSetting(CWnd* pParent = NULL);   // 标准构造函数

	virtual ~CDlgIOSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_IOSETTING };

public:

	CFont  m_ftButton;//按钮的字体
	CFont  m_ftEdit;

	CIOCommunicate *m_pCommuicate;//类指针

private:
	void InitUI();
	void InitValue();
	void Save();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
	int m_nRejectDelay1;//第二路打标队列延迟行数
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
