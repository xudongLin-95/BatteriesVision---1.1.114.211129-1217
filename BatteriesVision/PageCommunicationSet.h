#pragma once


// CPageCommunicationSet 对话框

class CPageCommunicationSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageCommunicationSet)

public:
	CPageCommunicationSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageCommunicationSet();

// 对话框数据
	enum { IDD = IDD_PAGE_COMMUNICATION_SET };

	void ResetSize();
private:
	void InitUI();
	void ShowHiddenSetting();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonSetIo();
	afx_msg void OnBnClickedButtonSetPlc();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonSetLight();
	afx_msg void OnBnClickedButtonSetMes();
	afx_msg void OnBnClickedButtonSetMarking();
	afx_msg void OnBnClickedButtonSetUps();
	afx_msg void OnBnClickedButtonSetTemperature();
	afx_msg void OnBnClickedButtonSetBetaray();
};
