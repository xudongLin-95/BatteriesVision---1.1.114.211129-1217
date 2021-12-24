#pragma once


// CPageDispList2 对话框

class CPageDispList2 : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDispList2)
public:
	struct tagDefectInfo
	{
		HObject hoCropImageList;
		HTuple  hvCamSNList;
		HTuple  hvErrorNameList;
		HTuple  hvImageSNList;
		HTuple  hvSideSNList;			//面序列号
		HObject hoDefectImageList;		//存储瑕疵拼接图像队列
		HTuple  hvDefectXList;			//瑕疵X坐标，单位mm
		HTuple  hvDefectYList;			//瑕疵Y坐标，单位m
		HTuple	hvTimeList;				//瑕疵时间
	};

public:
	CPageDispList2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageDispList2();

// 对话框数据
	enum { IDD = IDD_PAGE_DISP_LIST2 };

	void ResetSize();
	void ResetList();
	void UpdateList(C_MergeWork& tOutput);
private:
	void InitUI();
private:
	HTupleVector m_hvecWndHandle;
	tagDefectInfo m_tDefects;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	BOOL LDoubleClkAlgDlg(MSG* pMsg);
};
