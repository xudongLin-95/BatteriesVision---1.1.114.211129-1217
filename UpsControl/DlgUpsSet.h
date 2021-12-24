#pragma once
#include "SerialPortExt.h"
#include <vector>
#include "Resource.h"
#include "BCGCBProInc.h"//背景头文件
#include "DH_HalconEx.h"
#include "UpsSerial.h"
// CDlgUpsSet 对话框

class CDlgUpsSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgUpsSet)

public:
	CDlgUpsSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgUpsSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UPS_SET };
#endif

	CUpsSerial *m_pOwner;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString CurPort;//当前串口号
	CBCGPComboBox m_SerialPortList;
	CString m_Record;
	std::vector<CString> m_PortList; //串口列表	
	CSerialPortExt serialPortExt;//串口类对象
	int m_checkUpsStatus;//检测UPS的状态

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
