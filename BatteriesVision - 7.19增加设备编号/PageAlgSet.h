#pragma once


// CPageAlgSet �Ի���

class CPageAlgSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageAlgSet)
	void ResetSize();
private:
	void InitUI();
public:
	CPageAlgSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageAlgSet();

// �Ի�������
	enum { IDD = IDD_PAGE_ALG_SET };

	void ShowHiddenSetting();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonSetMeasure0();
	afx_msg void OnBnClickedButtonSetMeasure1();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonSetDataFilter();
	afx_msg void OnBnClickedButtonCalibrate();

	void ShowMesageJudgeByPowerLimit();
};
