#pragma once
#include "afxwin.h"
#include "CxStatic.h"

// CDlgSemiAutoMatic 对话框

class CDlgSemiAutoMatic : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSemiAutoMatic)

public:
	CDlgSemiAutoMatic(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSemiAutoMatic();

private:
	CBCGPGridCtrl m_LST_Dry;			//干料
	CBCGPGridCtrl m_LST_Wet;			//湿料
	CBCGPGridCtrl m_LST_Offset;			//补偿值数据列表
	void InitUI();
	void ResetList(CBCGPGridCtrl& lst);
	void UpdateListWet();
	void UpdateListDry();
	void UpdateListOffset();
	void AlignData();
	CDataCalculate::tagWetAndDry m_tWetAndDry[2];                //干湿膜的数据 0---A面；1---B面
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SEMIAUTOMATIC };
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
	int m_COM_nSelectMethod;
	afx_msg void OnBnClickedButtonSavedata();
};
