#pragma once
#include "SubDispList.h"
#include "SubDispSetting.h"

// CSubMainDisp 对话框

class CSubMainDisp : public CBCGPDialog
{
	DECLARE_DYNAMIC(CSubMainDisp)

public:
	CSubMainDisp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubMainDisp();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUB_MAIN_DISP };
//#endif

	void ResetSize();
	void UpdateData(C_MergeWork& tOutput);
	void SwitchDispSubDlg(int nSubDlgDispSN);
	CSubDispSetting* m_pSubDispSetting;
private:
	void InitUI();
	void InitList();
	void InitSubDlg();
	void RefreshBatchInfoList();
	void RefreshErrorCountList();
	void RefreshLogList();

	void OpenTimer();
	void CloseTimer();

	void InitWindowShow();

	void DispOutput(C_MergeWork& tOutput);
private:
	CBCGPGridCtrl m_Grid_Base;
	CBCGPGridCtrl m_Grid_Info;
	CBCGPGridCtrl m_Grid_Count;

	CFont m_ftCountGrid;

	HTupleVector m_hvecMainImageHandle;
	HTupleVector m_hvecTmpHandle;

	CSubDispList* m_pSubDispList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PostNcDestroy();
};
