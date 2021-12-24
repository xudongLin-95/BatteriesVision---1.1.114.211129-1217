#pragma once
#include "afxwin.h"
#include "CxStatic.h"

// CDlgManualMatic 对话框

class CDlgManualMatic : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgManualMatic)

public:
	CDlgManualMatic(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgManualMatic();

private:
	CBCGPGridCtrl m_LST_CCD;			//干料
	CBCGPGridCtrl m_LST_Ercy;			//二次元测量数据
	CBCGPGridCtrl m_LST_Offset;			//补偿值数据列表
	void InitUI();
	void ResetList(CBCGPGridCtrl& lst);
	void UpdateListCCD();
	void UpdateListErcy();
	void UpdateListOffset();
	void AlignData();
	CString SaveData();
	CDataCalculate::tagWetAndDry m_tWetAndDry[2];                //干湿膜的数据 0---A面；1---B面
	HTupleVector CopyDataToTuple(CString strData);															 // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANUALMATIC };
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
};
