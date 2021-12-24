#pragma once


// CPageCommunicationSet �Ի���

class CPageCommunicationSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageCommunicationSet)

public:
	CPageCommunicationSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageCommunicationSet();

// �Ի�������
	enum { IDD = IDD_PAGE_COMMUNICATION_SET };

	void ResetSize();
private:
	void InitUI();
	void ShowHiddenSetting();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
