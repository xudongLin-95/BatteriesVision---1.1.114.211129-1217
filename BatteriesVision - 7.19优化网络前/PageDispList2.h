#pragma once


// CPageDispList2 �Ի���

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
		HTuple  hvSideSNList;			//�����к�
		HObject hoDefectImageList;		//�洢覴�ƴ��ͼ�����
		HTuple  hvDefectXList;			//覴�X���꣬��λmm
		HTuple  hvDefectYList;			//覴�Y���꣬��λm
		HTuple	hvTimeList;				//覴�ʱ��
	};

public:
	CPageDispList2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageDispList2();

// �Ի�������
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	BOOL LDoubleClkAlgDlg(MSG* pMsg);
};
