#pragma once
#include "afxwin.h"
#include "CxStatic.h"

// CDlgSemiAutoMatic �Ի���

class CDlgSemiAutoMatic : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSemiAutoMatic)

public:
	CDlgSemiAutoMatic(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSemiAutoMatic();

private:
	CBCGPGridCtrl m_LST_Dry;			//����
	CBCGPGridCtrl m_LST_Wet;			//ʪ��
	CBCGPGridCtrl m_LST_Offset;			//����ֵ�����б�
	void InitUI();
	void ResetList(CBCGPGridCtrl& lst);
	void UpdateListWet();
	void UpdateListDry();
	void UpdateListOffset();
	void AlignData();
	CDataCalculate::tagWetAndDry m_tWetAndDry[2];                //��ʪĤ������ 0---A�棻1---B��
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SEMIAUTOMATIC };
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
	int m_COM_nSelectMethod;
	afx_msg void OnBnClickedButtonSavedata();
};
