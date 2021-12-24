#pragma once
#include "Control.h"
#include "SerialPortExt.h"
// CDlgTemperature �Ի���

class CDlgTemperature : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgTemperature)

public:
	CDlgTemperature(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTemperature();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TEMPERATURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void InitUI();
	DECLARE_MESSAGE_MAP()

public:
	CControl *m_pOwner;
	std::vector<CString> m_PortList; //�����б�		
	CBCGPComboBox m_SerialPortList;
	CBCGPListCtrl m_ListCtrl_Temperature;//�¶���ʾ�б�
	int m_Edit_TempNum;      //�����¿�������
	int m_Edit_IntervalTime; //���û�ȡ�¶ȼ��ʱ��
public:
	virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonOpenSerial();
	afx_msg void OnBnClickedButtonSaveExit();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	afx_msg void OnBnClickedButtonGetTemperature();
    afx_msg void OnBnClickedButton1();

public:
	void InitListCrtl();	
	int m_Edit_ResponseTimeOut;
};
