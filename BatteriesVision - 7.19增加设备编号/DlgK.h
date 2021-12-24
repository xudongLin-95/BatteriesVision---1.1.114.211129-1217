#pragma once
#include "afxwin.h"
#include "CxStatic.h"

// CDlgK 对话框

class CDlgK : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgK)

public:
	CDlgK(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgK();
	std::vector<double> m_tVecK[2];

	std::vector<double> m_tInitVecK[2];
	double m_dInternOffset[2];
private:
	CBCGPGridCtrl m_LST_CCD;			//干料
	CBCGPGridCtrl m_LST_Ercy;			//二次元测量数据
	CBCGPGridCtrl m_LST_K;			//补偿值数据列表
	void InitUI();
	void ResetList(CBCGPGridCtrl& lst);
	void ResetListK(CBCGPGridCtrl& lst);
	void UpdateListCCD();
	void UpdateListErcy();
	void UpdateListOffset();
	void AlignData();
	CDataCalculate::tagWetAndDry m_tWetAndDry[2];                //干湿膜的数据 0---A面；1---B面
	HTuple CopyDataToTuple(CString strData);															 // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANUALK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CxStatic m_STC_SM;
	CxStatic m_STC_GM;
	CxStatic m_STC_BC;
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonSavedata();
	afx_msg void OnBnClickedButtonCompute();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int m_COM_nInternOffset;
};
