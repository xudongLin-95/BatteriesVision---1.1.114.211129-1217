
// BatteriesVisionDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "BCGCBProInc.h"
#include "SubDispScope.h"
#include "CxStatic.h"
#include "TitleStatic.h"
#include "DlgLoading.h"
#include "ProgressCtrlExt.h"
#include "afxcmn.h"
#include "SubMainDisp.h"


#define UM_TIMER_QUEUEINFO WM_USER+1
#define UM_TIMER_STARTGRAB WM_USER+2
#define UM_TIMER_TIME WM_USER+3
typedef enum eCountType
{
	JigsawCount = 0,
	MeasureCount,
	DetectCount,
};

// CBatteriesVisionDlg 对话框
class CBatteriesVisionDlg : public CBCGPDialog
{
public:
	enum eStatusBar
	{
		STATUS_BAR_USER = 0,
		STATUS_BAR_GRAB,
		STATUS_BAR_DETECT,
		STATUS_BAR_QUEUE,
		STATUS_BAR_PLC,
		STATUS_BAR_IO,
		STATUS_BAR_RECTIFY,
		STATUS_BAR_NET,
		STATUS_BAR_OFFSET,
		STATUS_BAR_REMAIN_DISK,
		STATUS_BAR_TIME,
	};
	// 构造
public:
	void ExitDo();
	CBatteriesVisionDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CBatteriesVisionDlg();

	// 对话框数据
	enum { IDD = IDD_BATTERIESVISION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	CBCGPStatusBar m_wndStatusBar;
public:
	CSubDispScope* m_pSubDispScope;
	CSubMainDisp* m_pSubMainDisp;
private:
	void InitUI();
	void InitStatusBar();
	void InitLogo();
	void InitButton();
	void InitSubDlg();
	void InitWindowShow();
	void OpenTimer();
	void CloseTimer();
	CString GetCurTime();

	void SwitchDispSubDlg();
	void LoginOrLogout();
	void RefreshStatusBar();
	void DispOutput(const HTuple& hv_DispQueue);
	void DispStatic(C_MergeWork& tOutput);//显示主界面static
	void DispHistory(C_MergeWork& tOutput);//历史记录
	void DispSubDlg(C_MergeWork& tOutput);//子窗口
	void DispCount(C_MergeWork& tOutput);//计数
	BOOL RDoubleClkShowFindMarkDlg(MSG* pMsg);//右键双击历史图像显示FindMark参数对话框
	BOOL LDoubleClkAlgDlg(MSG* pMsg);//左键双击历史图像显示算法参数对话框

	time_t FileTimeToTime_t(const FILETIME &ft);
	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CFont m_ftCountGrid;
	CStatic m_STC_Logo;
	//HTupleVector m_hvecMainImageHandle;
	HTupleVector m_hvecHistoryImageHandle;
	HTupleVector m_hvecTmpHandle;
	HObjectVector m_hovecHistroy;
	CMenu m_muLanguage;
	CFont m_fontStatic;
	CFont m_ftButton;
	
	int m_nSubDlgDispSN;
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnDisp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGrabber(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDetect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeBatch(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateProduct(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateOnlineOffset(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRestartProcess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshDisp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCheckPermission(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonLanguage();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSwitchView();
	afx_msg void OnBnClickedButtonBatch();
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnMenuLog();
	afx_msg void OnMenuRestart();
	afx_msg void OnMenuAbout();
	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnBnClickedButtonList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLanguageCn();
	afx_msg void OnLanguageEn();
	afx_msg void OnBnClickedButtonProductChange();
};
