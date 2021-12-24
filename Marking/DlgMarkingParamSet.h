#pragma once

#include "MarkingCtr.h"
#include "resource.h"
#include "afxwin.h"
#include "BCGCBProInc.h"

// CDlgMarkingParamSet �Ի���

class CDlgMarkingParamSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgMarkingParamSet)

public:
	CDlgMarkingParamSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMarkingParamSet();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PLC_MARKINGSET };
#endif
	CMarkingCtr *m_pOwner;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	void InitUI();
	void RefreshButton();

	DECLARE_MESSAGE_MAP()
public:
	int m_EDIT_nPort;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSenddata();
	afx_msg void OnBnClickedButtonReceivedata();
	afx_msg void OnBnClickedButtonIsconnected();
	afx_msg void OnBnClickedButtonCancel();
	CString m_EDIT_Diff;           //�洢����ͼ����
	CString m_Edit_DivImageNum;    //ÿ��ͼ����200mmһ�ݻ��ֵĸ���
	CString m_Edit_MarkingWidth;   //��������Ϳ�Ŀ��[����ಬ��Ϊ��׼�����Ҳ���󲬲Ľ���] 
	CString m_Edit_MarkingHeight;  //��������Ϳ�ĸ߶�[Ĭ��200mmһ��]
};
