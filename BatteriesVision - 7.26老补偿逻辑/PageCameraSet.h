#pragma once


// CPageCameraSet 对话框

class CPageCameraSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageCameraSet)
	void ResetSize();
private:
	void InitUI();
public:
	CPageCameraSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageCameraSet();

// 对话框数据
	enum { IDD = IDD_PAGE_CAMERA_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonSetCam0();
	afx_msg void OnBnClickedButtonSetCam1();
	afx_msg void OnBnClickedButtonSetCam2();
	afx_msg void OnBnClickedButtonSetCam3();
	afx_msg void OnPaint();
	void ShowMesageJudgeByPowerLimit();
};
