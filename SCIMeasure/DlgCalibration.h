#pragma once
#include "Measure.h"
#include "BCGPGridCtrl.h"
// CDlgJiaoZhun 对话框

class CDlgJiaoZhun : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgJiaoZhun)

public:
	CDlgJiaoZhun(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgJiaoZhun();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_JIAOZHUN };
#endif
	CXmvImageWnd m_Wnd_Image;
	HObject m_hoImage;
	void InitWindow();
	void InitList();
	void UpdateList(BOOL bFirst = TRUE);
	void SetButtonStaus(BOOL bStaus);
	HTuple m_hvWidth, m_hvHeight;
	HTuple m_hMeasureHandle;
	HTuple m_hvRowEdge, m_hvColumnEdge, m_hvAmplitude, m_hvDistance;
	HTuple m_hvPix, m_hvReal;
	CMeasure* m_pOwners;
	CBCGPGridCtrl m_LST_JyDistance;
	int m_nSideSN;
	double DistancePP(double p1, double p2);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonCreate();
	int m_EDT_nThres;
	double m_EDT_dSigma;
	afx_msg void OnBnClickedButtonSave();
	double m_EDT_dSWidth;
	afx_msg void OnBnClickedButtonJy();
	int m_EDT_nJysl;
	afx_msg void OnEnChangeEditJysw();
};
