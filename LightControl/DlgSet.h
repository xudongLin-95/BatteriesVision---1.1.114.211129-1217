#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "SerialPortExt.h"
#include <vector>
#include "LightSerial.h"
#include "Resource.h"
#include "BCGCBProInc.h"//����ͷ�ļ�
#include "DH_HalconEx.h"
// CDlgSet �Ի���
class CDlgSet : public CBCGPDialog //CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSet)

public:
	CDlgSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSet();
	void InitUI();
    // �Ի�������
	enum { IDD = IDD_DIALOG_SET };

	CLightSerial *m_pOwner;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()
public:
	//�ؼ���������
	CBCGPComboBox m_SerialPortList;
//	CBCGPComboBox m_BoteRateList;
	CBCGPComboBox m_ChannelList;
	CBCGPButton m_Open;
	CBCGPButton m_Close;
	CBCGPSliderCtrl m_SliderV;
	//��ȡ��ǰѡ�б���
	CString CurPort;//��ǰѡ�д���
    CString CurChannel;//��ǰѡ��ͨ��
	CString CurLightType;//��Դ�ͺ�ѡ��
	afx_msg void OnCbnSelchangeComboSerialList();
	//���ڶ���
	//CSerialPortExt serialPortExt;
	CString m_PortRecored;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_SliderValue;

	std::vector<CString> m_PortList; //�����б�	
	afx_msg void OnCbnSelchangeComboChaanelList();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaint();
	CBCGPComboBox m_LightType;
	afx_msg void OnCbnSelchangeComboLightType();
	afx_msg void OnBnClickedButton2();
};
