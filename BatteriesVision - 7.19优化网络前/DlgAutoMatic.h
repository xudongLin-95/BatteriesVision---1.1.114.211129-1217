#pragma once
#include "afxwin.h"
#include "CxStatic.h"

// CDlgAutoMatic 对话框

class CDlgAutoMatic : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAutoMatic)

public:
	CDlgAutoMatic(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAutoMatic();

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
	HTuple       m_hvCompensation;
	HTupleVector m_vecRawData;//内部缓冲的湿膜的数据
	HTupleVector m_vecDataCollect;	//收集的数据,用于发送该数据为干膜数据
	HTuple       m_hvBiasRecv;//收到的干膜的数据
																 // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AUTOMATIC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CxStatic m_STC_SM;
	CxStatic m_STC_GM;
	CxStatic m_STC_BC;
	afx_msg void OnBnClickedButtonSavedata();
};
