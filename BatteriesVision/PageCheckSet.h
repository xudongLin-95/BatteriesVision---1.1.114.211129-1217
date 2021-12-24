#pragma once


// CPageCheckSet �Ի���

class CPageCheckSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageCheckSet)

public:
	CPageCheckSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageCheckSet();

// �Ի�������
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_CHECK_SET };
//#endif

	void ResetSize();

private:
	void InitUI();
	void RefeshSpotInspectionParam();
	tagPDInfo GetSpotInspectionParam();

private:
	CBCGPGridCtrl m_Grid_SpotInspectionParam;			//������

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonSpotinspection();
};
