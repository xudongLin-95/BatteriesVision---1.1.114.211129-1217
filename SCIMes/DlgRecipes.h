#pragma once

// CDlgRecipes 对话框

class CDlgRecipes : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRecipes)

public:
	CDlgRecipes(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRecipes();

// 对话框数据
	enum { IDD = IDD_DLGRECIPES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	neb::CJsonObject m_JsonRecipes;
	neb::CJsonObject m_JsonRecipeParams;
	int m_nSelect;
	CListCtrlExt m_ListRecipes;
	CListCtrlExt m_ListRecipeParam;
	CListCtrlExt m_ListCrrentRecipe;
	CFont m_LST_Font;

	CStaticExt m_StaticRecipe;
	CStaticExt m_StaticRecipeParam;
	CStaticExt m_StaticCurrentRecipe;
public:
	CMes * m_pRealMes;
	CString m_cstrRecipeCode;
	CString m_cstrRecipeVison;
	CString m_cstrProductCode;
public:
	virtual BOOL OnInitDialog();
	void InitUI(void);
	int GetRecipes(void);
	int GetRecipeParams(CString &cstrRecipe,std::string& strRecipeParam);
	int UpLoadRecipe();

	int UpDateRecipes(void);
	int UpdatePecipeParam(void);
	int Soap_GetRecipeList(void);
	void Soap_UploadRecipe();
	int UpdateCurrentRecipeList(void);
	int InitValue(void);

	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickListRecipes(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedButtonDownload();	
	afx_msg void OnBnClickedButtonUpload();
};
