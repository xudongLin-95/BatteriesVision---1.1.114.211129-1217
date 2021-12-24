#pragma once

// CPageProductSet 对话框

class CPageProductSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageProductSet)

public:
	CPageProductSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageProductSet();

// 对话框数据
	enum { IDD = IDD_PAGE_PRODUCT_SET };

	void ResetSize();
    
private:
	CBCGPGridCtrl m_Grid_ProductList;
	CBCGPGridCtrl m_Grid_ProductParam;			//产品规格
	int m_nCurrentProductSel;//当前使用品种的序号
	int m_nProductSel;//当前选择显示参数的品种序号
	CFont m_ftProp;

	int m_nGridRowSN, m_nGridOldRowSN;
	int m_nGridColSN, m_nGridOldColSN;

	struct tagPDInfoFlag
	{
		BOOL                    bAllFlag;
		BOOL					bPDNameFlag;			//产品名
		BOOL					bNoteFlag;				//产品备注
		BOOL     				bExistAT11Flag;			//是否包含AT9/AT11，正极
		BOOL     				bExistBaseCoatFlag;		//是否包含底涂，负极
		BOOL     				bExistIntermittentFlag;	//是否包含间歇区域
		BOOL					bPDTypeFlag;			//产品类型
		BOOL					bCoatingTypeFlag;		//涂的类型，0单面，1双面
		BOOL					bMeasureLineNumFlag;	//测量线数量
		BOOL					bLabelParamFlag;		//贴标/报警判断参数
		BOOL				    bNameListFlag;			//显示名称
														//判定参数
		BOOL					bUseCoatWidth;			//是否使用涂布宽
		BOOL					bUseFoilWidth;			//是否使用箔材宽
		BOOL					bUseAT11Width;			//是否使用AT9/AT11宽检测
		BOOL					bUseBaseCoatWidth;		//是否使用底涂检测
		BOOL					bUseMisalignment;		//是否使用错位检测
		BOOL					bUseChecking;			//是否使用设备点检
		BOOL					bUseSurfaceDetect;		//是否使用表面缺陷检测

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
			//判定参数
			bUseCoatWidth = FALSE;			//是否使用涂布宽
			bUseFoilWidth = FALSE;			//是否使用箔材宽
			bUseAT11Width = FALSE;			//是否使用AT9/AT11宽检测
			bUseBaseCoatWidth = FALSE;		//是否使用底涂检测
			bUseMisalignment = FALSE;		//是否使用错位检测
			bUseChecking = FALSE;			//是否使用设备点检
											//bUseSurfaceDetect = FALSE;		//是否使用表面缺陷检测
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
	BOOL ProductParamCorrected(tagPDInfo &tOldPdInfo, tagPDInfo &tNewPdInfo);//判断配放参数是否更新
	BOOL CreateRecoredFile(CString sName, tagPDInfo &tPdInfo);
	CString pDirectorPath, strFileName;
	tagPDInfoFlag tPdInfoCorrectedFlag;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
