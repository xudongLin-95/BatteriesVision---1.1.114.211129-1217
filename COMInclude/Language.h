#pragma once
#include <vector>

enum eLanguageType//语言类型
{
	Chinese = 0,//简体中文
	English,	//英文
	TraditionalChinese,//中文繁体
};

class CLanguage
{
public:
	CLanguage();
	~CLanguage();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();
public:
	//获取语言类型
	int		GetLanguageType() { return m_eLanguageType; }
	//是否是简体中文
	BOOL	IsChinese() { return m_eLanguageType == Chinese; }
	//是否是英文
	BOOL	IsEnglish() { return m_eLanguageType == English; }
	//是否是繁体中文
	BOOL	IsTraditionalChinese() { return m_eLanguageType == TraditionalChinese; }


	//设置语言类型
	BOOL	SetLanguage(int nTypeSN);
	//加载语言配置文件
	BOOL	LoadLanguageIni(CString strFileName);
	//获取标题文字
	BOOL	GetCaptionText(const CString& strDlgID, CString& strDispText);
	//从控件ID获取显示文本
	BOOL	GetDispTextFromID(int nDlgID,int nDlgItemID,CString& strDispText);
	BOOL	GetDispTextFromID(const CString& strDlgID,int nDlgItemID,CString& strDispText);
	//设置对话框语言
	BOOL	SetDlgLanguage(HWND hWinHwnd,int nDlgID);
	BOOL	SetDlgLanguage(HWND hWinHwnd,const CString& strID);
	//修改菜单语言
	void    SetMenuLanguage(const CString& strMenuID,CMenu* pMenu);
	//设置消息对话框
	void	MultMessagebox(CString strCN, CString strEN, CString strTC);
	//设置消息对话框
	int	Messagebox(CString strCN, CString strEN, CString strTC);
	int	MessageboxOKCancel(CString strCN, CString strEN, CString strTC);
	int	MessageboxError(CString strCN, CString strEN, CString strTC);
	int	MessageboxQuestion(CString strCN, CString strEN, CString strTC);
	int	MessageboxWarn(CString strCN, CString strEN, CString strTC);
private:
	BOOL	SpiltString(const CString& strSrc,const CString& ch,std::vector<CString>& vecDst);
	CString GetCurrentAppPathA();
private:
	eLanguageType m_eLanguageType;
	CString m_strLanguageFilePath;
};