#pragma once
#include "Detect.h"

// CDlgCalibrate 对话框

class CDlgCalibrate : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCalibrate)

public:
	CDlgCalibrate(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCalibrate();

// 对话框数据
	enum { IDD = IDD_DIALOG_CALIBRATE };
public:
	HTuple m_hvRow,m_hvColumn;	//m_hvColumn里面有4个值，分别为涂布高和极耳宽
	HTuple m_hvRowResolution,m_hvColResolution;
	CSeparate* m_pOwner;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	double m_EDT_dWidth;
	double m_EDT_dHeight;
};
