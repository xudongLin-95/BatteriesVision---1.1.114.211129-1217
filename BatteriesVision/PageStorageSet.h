#pragma once


// CPageStorageSet 对话框

class CPageStorageSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageStorageSet)

public:
	CPageStorageSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageStorageSet();

// 对话框数据
	enum { IDD = IDD_PAGE_STORAGE_SET };

	void ResetSize();
private:
	void InitUI();

	void EnableRadio();
	void EnableControl();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	int m_EDIT_nCacheSaveDays;
	int m_EDIT_nMinRemainSpace;
	int m_EDIT_nStartupSaveTime;
	int m_RADIO_nDataType;
	int m_RADIO_nImageType;
	int m_RADIO_nNGImageType;
	int m_RADIO_nOKImageType;
	CBCGPEdit m_EDIT_SavePath;
	CString m_EDIT_strSavePath;
	afx_msg void OnBnClickedButtonWatchlog();
	afx_msg void OnBnClickedRadioImage0();
	afx_msg void OnBnClickedRadioImage1();
	afx_msg void OnBnClickedRadioImage2();
	afx_msg void OnBnClickedRadioOkimage0();
	afx_msg void OnBnClickedRadioOkimage1();
	afx_msg void OnBnClickedRadioNgimage0();
	afx_msg void OnBnClickedRadioNgimage1();
	afx_msg void OnBnClickedRadioNgimage2();
	afx_msg void OnBnClickedCheckSaveErrortag();
	afx_msg void OnBnClickedCheckStartupSavetime();
	afx_msg void OnBnClickedCheckDeleteOldcaches();
	afx_msg void OnCbnSelchangeComboImageFormatOk();
	afx_msg void OnCbnSelchangeComboImageFormatNg();
	afx_msg void OnEnChangeEditRemainSpaceMin();
	afx_msg void OnEnChangeEditSavePath();
	afx_msg void OnEnChangeEditStartupSavetime();
	afx_msg void OnEnChangeEditCacheSavedays();
	afx_msg void OnCbnSelchangeComboLogType();
	afx_msg void OnCbnSelchangeComboLogDisptype();
	afx_msg void OnBnClickedRadioData0();
	afx_msg void OnBnClickedRadioData1();
	afx_msg void OnBnClickedRadioData2();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonSetlog();
	afx_msg void OnPaint();
	void ShowMesageJudgeByPowerLimit();
};
