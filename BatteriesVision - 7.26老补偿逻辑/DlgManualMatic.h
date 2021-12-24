#pragma once
#include "afxwin.h"
#include "CxStatic.h"

// CDlgManualMatic �Ի���

class CDlgManualMatic : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgManualMatic)

public:
	CDlgManualMatic(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgManualMatic();

private:
	CBCGPGridCtrl m_LST_CCD;			//����
	CBCGPGridCtrl m_LST_Ercy;			//����Ԫ��������
	CBCGPGridCtrl m_LST_Offset;			//����ֵ�����б�
	void InitUI();
	void ResetList(CBCGPGridCtrl& lst);
	void UpdateListCCD();
	void UpdateListErcy();
	void UpdateListOffset();
	void AlignData();
	CString SaveData();
	CDataCalculate::tagWetAndDry m_tWetAndDry[2];                //��ʪĤ������ 0---A�棻1---B��
	HTupleVector CopyDataToTuple(CString strData);															 // �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANUALMATIC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CxStatic m_STC_SM;
	CxStatic m_STC_GM;
	CxStatic m_STC_BC;
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonSavedata();
	afx_msg void OnBnClickedButtonCompute();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
