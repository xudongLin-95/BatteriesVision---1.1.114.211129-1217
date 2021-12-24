#pragma once

// CPageProductSet �Ի���

class CPageProductSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageProductSet)

public:
	CPageProductSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageProductSet();

// �Ի�������
	enum { IDD = IDD_PAGE_PRODUCT_SET };

	void ResetSize();
    
private:
	CBCGPGridCtrl m_Grid_ProductList;
	CBCGPGridCtrl m_Grid_ProductParam;			//��Ʒ���
	int m_nCurrentProductSel;//��ǰʹ��Ʒ�ֵ����
	int m_nProductSel;//��ǰѡ����ʾ������Ʒ�����
	CFont m_ftProp;

	int m_nGridRowSN, m_nGridOldRowSN;
	int m_nGridColSN, m_nGridOldColSN;

	struct tagPDInfoFlag
	{
		BOOL                    bAllFlag;
		BOOL					bPDNameFlag;			//��Ʒ��
		BOOL					bNoteFlag;				//��Ʒ��ע
		BOOL     				bExistAT11Flag;			//�Ƿ����AT9/AT11������
		BOOL     				bExistBaseCoatFlag;		//�Ƿ������Ϳ������
		BOOL     				bExistIntermittentFlag;	//�Ƿ������Ъ����
		BOOL					bPDTypeFlag;			//��Ʒ����
		BOOL					bCoatingTypeFlag;		//Ϳ�����ͣ�0���棬1˫��
		BOOL					bMeasureLineNumFlag;	//����������
		BOOL					bLabelParamFlag;		//����/�����жϲ���
		BOOL				    bNameListFlag;			//��ʾ����
														//�ж�����
		BOOL					bUseCoatWidth;			//�Ƿ�ʹ��Ϳ����
		BOOL					bUseFoilWidth;			//�Ƿ�ʹ�ò��Ŀ�
		BOOL					bUseAT11Width;			//�Ƿ�ʹ��AT9/AT11����
		BOOL					bUseBaseCoatWidth;		//�Ƿ�ʹ�õ�Ϳ���
		BOOL					bUseMisalignment;		//�Ƿ�ʹ�ô�λ���
		BOOL					bUseChecking;			//�Ƿ�ʹ���豸���
		BOOL					bUseSurfaceDetect;		//�Ƿ�ʹ�ñ���ȱ�ݼ��

		void Init()
		{
			bAllFlag = FALSE;
			bPDNameFlag = FALSE;
			bNoteFlag = FALSE;
			bExistAT11Flag = FALSE;
			bExistBaseCoatFlag = FALSE;
			bExistIntermittentFlag = FALSE;
			bPDTypeFlag = FALSE;
			bCoatingTypeFlag = FALSE;
			bMeasureLineNumFlag = FALSE;
			bLabelParamFlag = FALSE;
			bNameListFlag = FALSE;
			//�ж�����
			bUseCoatWidth = FALSE;			//�Ƿ�ʹ��Ϳ����
			bUseFoilWidth = FALSE;			//�Ƿ�ʹ�ò��Ŀ�
			bUseAT11Width = FALSE;			//�Ƿ�ʹ��AT9/AT11����
			bUseBaseCoatWidth = FALSE;		//�Ƿ�ʹ�õ�Ϳ���
			bUseMisalignment = FALSE;		//�Ƿ�ʹ�ô�λ���
			bUseChecking = FALSE;			//�Ƿ�ʹ���豸���
											//bUseSurfaceDetect = FALSE;		//�Ƿ�ʹ�ñ���ȱ�ݼ��
		}
	};
private:
	void InitUI();
	void RefeshProductList();
	void RenameProductList();
	void EditRefreshProductParam();
	void RefreshProductParam();
	void RefreshProductParam(tagPDInfo& tPdInfo);
	tagPDInfo GetProductParam();
	//*******************//
	BOOL ProductParamCorrected(tagPDInfo &tOldPdInfo, tagPDInfo &tNewPdInfo);//�ж���Ų����Ƿ����
	BOOL CreateRecoredFile(CString sName, tagPDInfo &tPdInfo);
	CString pDirectorPath, strFileName;
	tagPDInfoFlag tPdInfoCorrectedFlag;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonProductSet();
	afx_msg LRESULT OnEndEditGridProduct(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemChangedProduct(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBeginEditGridProduct(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemChangedGridProduct(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonProductNew();
	afx_msg void OnBnClickedButtonProductLoad();
	afx_msg void OnBnClickedButtonProductDelete();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonProductSave();
	afx_msg void OnBnClickedButtonProductClearOffset();
	afx_msg void OnBnClickedButtonProductCalOffset();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	void ShowMesageJudgeByPowerLimit();
	afx_msg void OnBnClickedButtonManual();
	afx_msg void OnBnClickedButtonSemiauto();
	afx_msg void OnBnClickedButtonK();
	afx_msg void OnBnClickedButtonAutomatic();
	afx_msg void OnBnClickedButtonAlarmSet();
};
