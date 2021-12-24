#pragma once
#include "Control.h"
#include "SerialPortExt.h"
// CDlgTemperature 对话框

class CDlgTemperature : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgTemperature)

public:
	CDlgTemperature(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTemperature();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TEMPERATURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void InitUI();
	DECLARE_MESSAGE_MAP()

public:
	CControl *m_pOwner;
	std::vector<CString> m_PortList; //串口列表		
	CBCGPComboBox m_SerialPortList;
	CBCGPListCtrl m_ListCtrl_Temperature;//温度显示列表
	int m_Edit_TempNum;      //设置温控器个数
	int m_Edit_IntervalTime; //设置获取温度间隔时间
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
