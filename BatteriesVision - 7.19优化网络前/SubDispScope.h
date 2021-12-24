#pragma once
#include "OScopeCtrl.h"
#include "TitleStatic.h"
#include "CoatSketchStatic.h"

// CSubDispScope �Ի���
#define MAX_SCOPE 20

class CSubDispScope : public CBCGPDialog
{
	DECLARE_DYNAMIC(CSubDispScope)

public:
	CSubDispScope(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubDispScope();

// �Ի�������
	enum { IDD = IDD_SUB_DISP_SCOPE };

	void InitUI();
	void ResetSize();
	void ResetScope();
	void UpdateData(C_MergeWork& tOutput);

	void RefreshBatchDisp();
	
private:
	void InitStatic();
	void RefreshCountStatic();

	void OpenTimer();
	void CloseTimer();
	void InitWindowShow();
	void DispScope(C_MergeWork& tOutput);
	void DispCoatSketch(C_MergeWork& tOutput);
	void DispOutput(C_MergeWork& tOutput);
	void SetScopeColor(COScopeCtrl& Scop);
private:
	CTitleStatic m_STC_CurrentProduct;
	CTitleStatic m_STC_CurrentBatch;
	CTitleStatic m_STC_CurrentSpeed;
	CTitleStatic m_STC_CurrentMeterCount;
	CTitleStatic m_STC_YieldTotal;
	CTitleStatic m_STC_CurrentStatus;
	CFont m_fontStatic;

	CCoatSketchStatic m_CoatSketch;

	CStringArray strArrCode;
	int SCOPE_COL_NUM[4];
	COScopeCtrl m_Oscope1[MAX_SCOPE];//��һ������ͼ
	COScopeCtrl m_Oscope2[MAX_SCOPE];//�ڶ�������ͼ
	COScopeCtrl m_Oscope3[MAX_SCOPE];//����������ͼ
	COScopeCtrl m_Oscope4[MAX_SCOPE];//����������ͼ

	HTupleVector m_hvecMainImageHandle;
	HTupleVector m_hvecTmpHandle;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnPaint();
	afx_msg LRESULT OnRefreshDisp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
