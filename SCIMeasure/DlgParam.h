#pragma once
#include "SCIMeasure.in.h"
#include "Measure.h"
// CDlgParam �Ի���
#include "afxwin.h"
#include "PageMeasureSet.h"
#include "PageDefectSet.h"
#include "PageCalibrationSet.h"
#include "PageSpotInspectSet.h"

extern int WM_TEST;			//����
enum eTestType
{
	Test_Measure = 0,
	Test_Defect,
	Test_Calibration,
	Test_SpotInspection,
};

class CDlgParam : public CBCGPDialog
{
public:
	enum eTabItem
	{
		MeasureSet = 0,
		DefectSet,
		CalibrationSet,
		SpotInsptectSet,
	};

	DECLARE_DYNAMIC(CDlgParam)
public:
	CDlgParam(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParam();

	// �Ի�������
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PARAM };
//#endif

	CMeasure*		m_pTemp;		//ģ�������
	CMeasure*		m_pReal;		//ʵ�ʼ����
	MeasureInput	m_tInput;		//�������
	MeasureOutput	m_tOutput;		//������
	CXmvImageWnd	m_Wnd_Image;
	CLineMeasure*   m_LineMeasure;

	void GenSearchRegion();

	void SetMarkWithMouse(BOOL bUse = FALSE) { m_bMarkWithMouse = bUse; }
private:
	void SwitchDispPage(const int& nSN);
	void InitUI();
	void InitWindow();
	void InitTab();

	void RefreshTestList();
	BOOL GetLButtonLineSN(MSG* pMsg);
	BOOL DispLineHor(MSG* pMsg);

	void AddDefectList(int i);
	void AddErrorLineIndex();
private:
	HObject					m_hoSearchRegion;
	HTuple					m_hvDispY, m_hvDispX;
	std::vector<CString>	m_vecDispString;
	std::vector<CBCGPDialog*> m_vecPages;
	CPageMeasureSet*		m_pPageMeasureSet;
	CPageDefectSet*			m_pPageDefectSet;
	CPageCalibrationSet*	m_pPageCalibrationSet;
	CPageSpotInspectSet*	m_pPageSpotInspectSet;

	CFont m_ftButton;
	CTabCtrlExt m_TAB_List;
	CBCGPGridCtrl m_listInfo;

	CSpendTime m_SpendTime;

	int						m_nMouseX;
	int						m_nMouseY;

	BOOL					m_bMarkWithMouse;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_COM_bAT9;
	int m_COM_nProType;

	int m_nSearchLineSN;
	BOOL m_bDrawLineHor;//�Ƿ���ʾ��ע����

	BOOL m_bConfirmMeasure;//�Ƿ���Ҫ�ͻ�ȷ�ϲ������ұ�
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonExit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonTestall();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonSaveimage();
	afx_msg void OnBnClickedButtonOpenimage();
	afx_msg void OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnTest(WPARAM wParam, LPARAM lParam);
	void DispSearchLine(BOOL bDisp = FALSE);
	void MoveSearchReion();
	void MoveSearchRegionStep(BOOL bLeft);
	BOOL m_CHECK_bShowAdaptedImage;
	afx_msg void OnBnClickedCheckShowAdaptedImage();
	afx_msg void OnBnClickedButtonDispNewimage();
};
