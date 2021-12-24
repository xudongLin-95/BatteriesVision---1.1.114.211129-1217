#pragma once
#include "resource.h"
#include "afxcmn.h"

// CPageCalibrationSet 对话框

class CDlgParam;
class CPageCalibrationSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageCalibrationSet)

public:
	CPageCalibrationSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageCalibrationSet();

	// 对话框数据
	//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALIBRATION_SET };
	//#endif

	CMeasure* m_pOwners;
	CDlgParam* m_pParentDlg;

	//保存标定文件数据
	void SaveCaliParamFile(const int& nSN, CCalibration::CalOutput tOut);
private:
	void InitUI();
	void RefreshUIData();
	void GetCalbHW(HObject ho_ImageReduced, HObject *ho_Edges, HTuple *hv_H0, HTuple *hv_W0);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonCal();
	afx_msg void OnEnChangeParamMeasurethres();
	afx_msg void OnEnChangeParamThres();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeParamDisuplimit();
	afx_msg void OnEnChangeParamDisdownlimit();
	afx_msg void OnEnChangeParamKvalue();
	int m_nThres;
	int m_nMeasureThres;
	double m_dUnitWidth;
	double m_dDisUpLimit;
	double m_dDisDownLimit;
	double m_dKValue;
	double m_dVerityDistance;
	afx_msg void OnBnClickedButtonVerify();
	afx_msg void OnEnChangeVeritydistance();
	int m_EDT_nParamMeasureThres;
	int m_EDT_nImageIgnoreWidth;
	afx_msg void OnEnChangeParamImageIgnoreWidth();
	afx_msg void OnEnChangeParamKvalue2();
	double m_dKValue2;
	void SetButtonStaus(BOOL bStaus);
	afx_msg void OnBnClickedButtonDefectCali();
	afx_msg void OnBnClickedButtonDefectTest();
	CListCtrl m_List_Area;
	int m_Com_nExtractColor;
	double m_EDT_dMinArea;
	int m_EDT_nHighThresh;
	int m_EDT_nLowThresh;
	double m_EDT_dCalibArea;
	double m_dRowResolution;
	double m_dColResolution;
	BOOL m_bFlag;
	HTuple m_hvGrayTuple;
	HTuple m_hvObjWidth, m_hvObjHeight;
	afx_msg void OnEnChangeEditObjectArea();
	afx_msg void OnEnChangeEditDarkThres();
	afx_msg void OnEnChangeEditLightThres();
	afx_msg void OnEnChangeEditExtractMinArea();
	afx_msg void OnCbnSelchangeComboExtractColor();
	int m_Com_nDefectCalType;
	afx_msg void OnCbnSelchangeComboCalType();
	afx_msg void OnBnClickedButtonCal2();
	afx_msg void OnBnClickedCheckDrawLineH();
	afx_msg void OnEnChangeEditParamMeasureThres();
	double m_dDoubleCamInternValue;
	afx_msg void OnEnChangeParamDoublecamInternValue();
};
