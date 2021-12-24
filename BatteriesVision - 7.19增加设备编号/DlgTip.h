#pragma once
#include "afxwin.h"

#define IDC_BTN_BASE 3000

// CDlgTip �Ի���
enum eTipType
{
	TIP_CheckProduct = 0,	//����䷽
	TIP_StopPosReason,		//ͣ��λԭ��
	TIP_FirstArticleNG,		//֪ͨ���׼�������,NG
	TIP_FirstArticleOK,		//֪ͨ���׼�������,OK
};
struct tagTipInfo
{
	eTipType				eType;			//����
	CString					strDiscribe;	//����
	std::vector<CString>	vecOption;		//ѡ����
};

class CDlgTip : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgTip)

public:
	CDlgTip(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTip();

// �Ի�������
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIP };
//#endif

	void ShowTip(eTipType eTip);

private:
	std::vector<tagTipInfo> m_vecInfoList;
	tagTipInfo	m_tCurrentInfo;
	CRITICAL_SECTION m_csLock;
	std::vector<CBCGPButton*> m_vecBtnList;
private:
	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CxStatic m_STC_Info;
	CFont m_ftInfo;
	afx_msg void OnNcDestroy();
	afx_msg void OnButtonClick(UINT uID);
};
