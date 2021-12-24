#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "SerialPortExt.h"
#include <vector>
#include "LightSerial.h"
#include "Resource.h"
#include "BCGCBProInc.h"//背景头文件
#include "DH_HalconEx.h"
// CDlgSet 对话框
class CDlgSet : public CBCGPDialog //CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSet)

public:
	CDlgSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSet();
	void InitUI();
    // 对话框数据
	enum { IDD = IDD_DIALOG_SET };

	CLightSerial *m_pOwner;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()
public:
	//控件关联变量
	CBCGPComboBox m_SerialPortList;
//	CBCGPComboBox m_BoteRateList;
	CBCGPComboBox m_ChannelList;
	CBCGPButton m_Open;
	CBCGPButton m_Close;
	CBCGPSliderCtrl m_SliderV;
	//获取当前选中变量
	CString CurPort;//当前选中串口
    CString CurChannel;//当前选中通道
	CString CurLightType;//光源型号选择
	afx_msg void OnCbnSelchangeComboSerialList();
	//串口对象
	//CSerialPortExt serialPortExt;
	CString m_PortRecored;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_SliderValue;

	std::vector<CString> m_PortList; //串口列表	
	afx_msg void OnCbnSelchangeComboChaanelList();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaint();
	CBCGPComboBox m_LightType;
	afx_msg void OnCbnSelchangeComboLightType();
	afx_msg void OnBnClickedButton2();
};
