#pragma once


// DlgDefctImageDisp 对话框

class DlgDefctImageDisp : public CBCGPDialog
{
	DECLARE_DYNAMIC(DlgDefctImageDisp)

public:
	DlgDefctImageDisp(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgDefctImageDisp();

// 对话框数据
	enum { IDD = IDD_DIALOG_DEFECT_DISP };
	void ShowAlarm(C_MergeWork& tOutput);
	void UpdateList(C_MergeWork& tOutput);
	void ResetList();

private:
	HTuple m_hWndHandle;
	//tagDefectInfo m_tDefects;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
