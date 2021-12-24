//////////////////////////////////////////////////////////////////////////
///Language.h ���������л�
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CLanguage : public CBaseTmpl
{
public:
	enum eLanguageType
	{
		Chinese=0,			//��������
		English,			//Ӣ��
		TraditionalChinese,	//��������
	};
public:
	CLanguage();
	~CLanguage();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();
public:
	//�Ƿ�������
	BOOL	IsChinese() { return m_eLanguageType == Chinese; }
	//�Ƿ���Ӣ��
	BOOL	IsEnglish() { return m_eLanguageType == English; }
	//������������
	BOOL	SetLanguage(int nTypeSN);
	//�������������ļ�
	BOOL	LoadLanguageIni(CString strFileName);
	//��ȡ��������
	BOOL	GetCaptionText(const CString& strDlgID,CString& strDispText);
	//�ӿؼ�ID��ȡ��ʾ�ı�
	BOOL	GetDispTextFromID(int nDlgID,int nDlgItemID,CString& strDispText);
	BOOL	GetDispTextFromID(const CString& strDlgID,int nDlgItemID,CString& strDispText);
	//���öԻ�������
	BOOL	SetDlgLanguage(HWND hWinHwnd,int nDlgID);
	BOOL	SetDlgLanguage(HWND hWinHwnd,const CString& strID);
	//�޸Ĳ˵�����
	void    SetMenuLanguage(const CString& strMenuID, CMenu* pMenu);
	//��ȡ��������
	int		GetLanguageType();
	//������Ϣ�Ի���
	int	Messagebox(CString strCN, CString strEN, CString strTC);
	int	MessageboxOKCancel(CString strCN, CString strEN, CString strTC);
	int	MessageboxError(CString strCN, CString strEN, CString strTC);
	int	MessageboxQuestion(CString strCN, CString strEN, CString strTC);
	int	MessageboxWarn(CString strCN, CString strEN, CString strTC);

private:
	BOOL	SpiltString(const CString& strSrc,const CString& ch,std::vector<CString>& vecDst);
private:
	eLanguageType m_eLanguageType;
	CString m_strLanguageFilePath;
};