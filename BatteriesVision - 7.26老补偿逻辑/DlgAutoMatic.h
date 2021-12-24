#pragma once
#include "afxwin.h"
#include "CxStatic.h"

// CDlgAutoMatic �Ի���

class CDlgAutoMatic : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAutoMatic)

public:
	CDlgAutoMatic(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAutoMatic();

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
	HTuple       m_hvCompensation;
	HTupleVector m_vecRawData;//�ڲ������ʪĤ������
	HTupleVector m_vecDataCollect;	//�ռ�������,���ڷ��͸�����Ϊ��Ĥ����
	HTuple       m_hvBiasRecv;//�յ��ĸ�Ĥ������
																 // �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AUTOMATIC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CxStatic m_STC_SM;
	CxStatic m_STC_GM;
	CxStatic m_STC_BC;
	afx_msg void OnBnClickedButtonSavedata();
};
