#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "Find.h"
#include "afxwin.h"
#include "BCGCBProInc.h"
// CDlgParamSet 对话框

class CDlgParamSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamSet)

public:
	CDlgParamSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_PARAM };

	CImageList m_TreeImages;

	void DispImage(const HObject& hoImage);
	void DispMark(const SINGLEMARKINFO&  MarkDisp);
	void SetOwner(CFindMark* pOwner);
	BOOL Create_ShapeModel(HObject& ho_ImageReduced);
	BOOL Create_NCCModel(HObject& ho_ImageReduced);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
//	virtual void PostNcDestroy();
public:
	CBCGPTreeCtrl m_MarkTree;
	virtual BOOL OnInitDialog();
	BOOL IsDrawing();
	void Draw(CString strDrawType,HObject* Region);
private:
	// 滚轮缩放
	CFont  m_ftButton;
	HTuple m_hvDispImageWidth;		// 显示图像宽
	HTuple m_hvDispImageHeight;		// 显示图像高
	double m_dDispImagePartRow0;
	double m_dDispImagePartCol0;
	double m_dDispImagePartRow1;
	double m_dDispImagePartCol1;
private:
	
	void InitUI();
	void SetButton(CButtonExt& tButton);
	void InitTree();
	BOOL m_bDrawing;
	//
public:
	CFindMark* m_pRealDetect;
	CFindMark* m_pOwner;
	HObject m_hoDispRegion;
	void SetButtonStaus(BOOL bStaus);
public:
	HObject m_hoImage;
	void MyMouseWheel(short zDelta);
	void MyMouseMove();
	void MyFitImage();
	HObject  m_hoCurrentDispImage;
	HTuple   m_hv_WindowID;
	afx_msg void OnBnClickedButtonAdd();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	int m_nSearchCoefficient_Column;
	CString m_strMarkType;
	CString m_strDrawType;
	CString m_strMarkName;
	afx_msg void OnBnClickedButtonOpenimage();
	afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuDelete();
	afx_msg void OnMenuRename();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonAddline();
	afx_msg void OnBnClickedButtonTest();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int m_COM_nType;
	afx_msg void OnCbnSelchangeComboType();
	double m_EDT_dMiniScore;
	afx_msg void OnBnClickedBtnSaveparam();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
