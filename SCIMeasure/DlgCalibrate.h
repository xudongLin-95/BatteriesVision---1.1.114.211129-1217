#pragma once
#include "Detect.h"

// CDlgCalibrate �Ի���

class CDlgCalibrate : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCalibrate)

public:
	CDlgCalibrate(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCalibrate();

// �Ի�������
	enum { IDD = IDD_DIALOG_CALIBRATE };
public:
	HTuple m_hvRow,m_hvColumn;	//m_hvColumn������4��ֵ���ֱ�ΪͿ���ߺͼ�����
	HTuple m_hvRowResolution,m_hvColResolution;
	CSeparate* m_pOwner;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	double m_EDT_dWidth;
	double m_EDT_dHeight;
};
