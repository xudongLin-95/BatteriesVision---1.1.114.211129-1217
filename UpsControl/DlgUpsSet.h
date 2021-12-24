#pragma once
#include "SerialPortExt.h"
#include <vector>
#include "Resource.h"
#include "BCGCBProInc.h"//����ͷ�ļ�
#include "DH_HalconEx.h"
#include "UpsSerial.h"
// CDlgUpsSet �Ի���

class CDlgUpsSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgUpsSet)

public:
	CDlgUpsSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgUpsSet();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UPS_SET };
#endif

	CUpsSerial *m_pOwner;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CString CurPort;//��ǰ���ں�
	CBCGPComboBox m_SerialPortList;
	CString m_Record;
	std::vector<CString> m_PortList; //�����б�	
	CSerialPortExt serialPortExt;//���������
	int m_checkUpsStatus;//���UPS��״̬

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonCon();
	afx_msg void OnBnClickedButtonCheck();
	afx_msg void OnBnClickedButtonSaveexit();
	afx_msg void OnBnClickedButtonExit();

public:	
	BOOL OpenSerialPort();
	BOOL CloseSerialPort();
	void InitUI();
};
