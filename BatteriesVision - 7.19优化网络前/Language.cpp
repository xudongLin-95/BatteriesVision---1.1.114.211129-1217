#include "stdafx.h"
#include "Language.h"

CLanguage::CLanguage()
{
	m_eLanguageType = Chinese;
}

CLanguage::~CLanguage()
{

}

BOOL CLanguage::Init()
{
	ReadParam();

	return TRUE;
}

BOOL CLanguage::ReadParam()
{
	SetLanguage(GetIniInt("Language","Type",0));

	return TRUE;
}

BOOL CLanguage::WriteParam()
{
	WriteIniInt("Language","Type",(int)m_eLanguageType);

	return TRUE;
}

BOOL CLanguage::SetLanguage(int nTypeSN)
{
	m_eLanguageType = (eLanguageType)nTypeSN;
	CString strPath;
	strPath.Format("%s//Config//Public//Language//",GetCurrentAppPath());
	if (Chinese==nTypeSN)
	{
		strPath += "CN.la";
	}
	else
	{
		strPath += "EN.la";
	}
	
	return LoadLanguageIni(strPath);
}

BOOL CLanguage::LoadLanguageIni(CString strFileName)
{
	if (PathFileExists(strFileName))
	{
		m_strLanguageFilePath = strFileName;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CLanguage::GetDispTextFromID(int nDlgID,int nDlgItemID,CString& strDispText)
{
	CString strSection,strKeyName;

	strSection.Format("%d",nDlgID);

	strKeyName.Format("%d",nDlgItemID);

	CString strValue;

	GetPrivateProfileStringA(strSection,strKeyName,"null",strValue.GetBuffer(MAX_PATH),MAX_PATH,m_strLanguageFilePath);
	
	strValue.ReleaseBuffer();

	if (strValue == "null")
	{
		return FALSE;
	}
	else
	{
		strDispText = strValue;
		return TRUE;
	}
}

BOOL CLanguage::GetCaptionText(const CString& strDlgID,CString& strDispText)
{
	CString strSection,strKeyName;

	strSection.Format("%s",strDlgID);

	strKeyName.Format("%s","Caption");

	CString strValue;

	GetPrivateProfileStringA(strSection,strKeyName,"null",strValue.GetBuffer(MAX_PATH),MAX_PATH,m_strLanguageFilePath);

	strValue.ReleaseBuffer();

	if (strValue == "null")
	{
		return FALSE;
	}
	else
	{
		strDispText = strValue;
		return TRUE;
	}
}

BOOL CLanguage::GetDispTextFromID(const CString& strDlgID,int nDlgItemID,CString& strDispText)
{
	CString strSection,strKeyName;

	strSection.Format("%s",strDlgID);

	strKeyName.Format("%d",nDlgItemID);

	CString strValue;

	GetPrivateProfileStringA(strSection,strKeyName,"null",strValue.GetBuffer(MAX_PATH),MAX_PATH,m_strLanguageFilePath);

	strValue.ReleaseBuffer();

	if (strValue == "null")
	{
		return FALSE;
	}
	else
	{
		strDispText = strValue;
		return TRUE;
	}
}

BOOL CLanguage::SpiltString(const CString& strSrc,const CString& ch,std::vector<CString>& vecDst)
{
	//将一串字符用指定分割成数组
	vecDst.clear();
	if (strSrc=="")
	{
		return FALSE;
	}
	int nPos = 0;
	CString strTmp = strSrc.Tokenize(ch,nPos);

	while(strTmp.Trim() != _T(""))
	{
		vecDst.push_back(strTmp);
		strTmp = strSrc.Tokenize(ch,nPos);
	}

	if (vecDst.size()>0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CLanguage::SetDlgLanguage(HWND hWinHwnd,int nDlgID)
{
	CString strDlgID;
	strDlgID.Format("%d",nDlgID);
	return SetDlgLanguage(hWinHwnd,strDlgID);
}

BOOL CLanguage::SetDlgLanguage(HWND hWinHwnd,const CString& strID)
{
	//设置对话框标题
	CString strCaptionText="";
	if (GetCaptionText(strID,strCaptionText))
		CWnd::FromHandle(hWinHwnd)->SetWindowTextA(strCaptionText);

	//设置控件显示字符
	HWND  hwndChild=::GetWindow(hWinHwnd,GW_CHILD);  //列出所有控件
	CWnd* pWnd = CWnd::FromHandle(hWinHwnd);
	while(hwndChild)
	{      
		int nItemID =::GetDlgCtrlID(hwndChild);//取得ID

		CString strDispText="";
		if (!GetDispTextFromID(strID,nItemID,strDispText))//获取要显示的字符
		{
			hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);
			continue;
		}

		CString strClass;
		::GetClassNameA(pWnd->GetDlgItem(nItemID)->m_hWnd,strClass.GetBufferSetLength(255),255);
		strClass.ReleaseBuffer();

		if ("Button"==strClass
			|| "Static"==strClass)
		{
			pWnd->GetDlgItem(nItemID)->SetWindowTextA(strDispText);
		}
		else if ("SysListView32"==strClass)
		{
			std::vector<CString> vecDispText;
			if (SpiltString(strDispText,";",vecDispText))
			{
				CHeaderCtrl* pHead = ((CListCtrl*)pWnd->GetDlgItem(nItemID))->GetHeaderCtrl();
				int nCount = pHead->GetItemCount();
				for (int i=0;i<nCount;i++)
				{
					if (i>vecDispText.size()-1)
					{
						continue;
					}
					HDITEM   hdi;
					TCHAR     lpBuffer[256];
					hdi.mask   =   HDI_TEXT;  
					hdi.pszText   =   lpBuffer;
					hdi.cchTextMax   =   256; 
					pHead->GetItem( i, &hdi );
					hdi.pszText = (LPSTR)(LPCTSTR)vecDispText[i];
					pHead->SetItem( i, &hdi );
				}
			}
		}
		else if ("ComboBox"==strClass)
		{
			std::vector<CString> vecDispText;
			if (SpiltString(strDispText,";",vecDispText))
			{
				CComboBox* pComb = (CComboBox*)pWnd->GetDlgItem(nItemID);
				int nCount = pComb->GetCount();
				int nSel = pComb->GetCurSel();
				for (int i=nCount-1;i>=0;i--)
				{
					pComb->DeleteString(i);
				}
				for (int i=0;i<vecDispText.size();i++)
				{
					pComb->AddString(vecDispText[i]);
				}
				pComb->SetCurSel(nSel);
			}
		}
		else if ("ListBox"==strClass)
		{
			std::vector<CString> vecDispText;
			if (SpiltString(strDispText,";",vecDispText))
			{
				CListBox* pLB = (CListBox*)pWnd->GetDlgItem(nItemID);
				int nCount = pLB->GetCount();
				for (int i=nCount-1;i>=0;i--)
				{
					pLB->DeleteString(i);
				}
				for (int i=0;i<vecDispText.size();i++)
				{
					pLB->AddString(vecDispText[i]);
				}
			}
		}
		else if (-1!=strClass.Find("BCGPGridCtrl"))
		{
			std::vector<CString> vecDispText;
			if (SpiltString(strDispText,";",vecDispText))
			{
				CBCGPGridCtrl* pGC = (CBCGPGridCtrl*)pWnd->GetDlgItem(nItemID);
				int nCount = pGC->GetColumnCount();
				for (int i=0;i<nCount;i++)
				{
					if (i>vecDispText.size()-1)
					{
						continue;
					}
					pGC->SetColumnName(i,vecDispText[i]);
				}
			}
		}
		else if ("SysTabControl32"==strClass)
		{
			std::vector<CString> vecDispText;
			if (SpiltString(strDispText, ";", vecDispText))
			{
				CTabCtrl* pTB = (CTabCtrl*)pWnd->GetDlgItem(nItemID);
				int nCount = pTB->GetItemCount();
				for (int i = 0; i < vecDispText.size(); i++)
				{
					TCITEMA tc;
					pTB->GetItem(i, &tc);
					tc.pszText = (LPSTR)(LPCTSTR)vecDispText[i];
					pTB->SetItem(i, &tc);
				}
			}
		}

		
		hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);
	}

	return TRUE;
}

void CLanguage::SetMenuLanguage(const CString& strMenuID, CMenu* pMenu)
{
	int cntMain = pMenu->GetMenuItemCount();
	for (int i = 0; i < cntMain; i++)
	{
		CString menuName;
		CMenu *cMenuSub = pMenu->GetSubMenu(i);

		int cntSub = cMenuSub->GetMenuItemCount();
		for (int j = 0; j < cntSub; j++)
		{

			int IndexFlag;
			IndexFlag = GetMenuItemID(cMenuSub->GetSafeHmenu(), j);//IndexFlag=-1是有子菜单项目  IndexFlag=0是分隔符
			if (IndexFlag != 0)
			{

				if (IndexFlag != 0)
				{
					{
						cMenuSub->GetMenuString(j, menuName, MF_BYPOSITION);
						CString strSection, strKeyName;
						CString strValue;
						strSection.Format("%s", strMenuID);
						strKeyName.Format("%s", menuName);
						GetPrivateProfileStringA(strSection, strKeyName, "null", strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strLanguageFilePath);
						strValue.ReleaseBuffer();
						cMenuSub->ModifyMenu(j, MF_BYPOSITION | MF_STRING, IndexFlag, strValue);
					}
					if (IndexFlag == -1)
					{
						CMenu *cMenuSSub = cMenuSub->GetSubMenu(j);//获取子菜单
						int nSSubCount = cMenuSSub->GetMenuItemCount(); //获取子菜单的数目 包含分隔符
						for (int j = 0; j < nSSubCount; j++)
						{
							int IDIndex = ::GetMenuItemID(cMenuSSub->GetSafeHmenu(), j);//得到当前的ID
							cMenuSSub->GetMenuString(j, menuName, MF_BYPOSITION);
							CString strSection, strKeyName;
							CString strValue;
							strSection.Format("%s", strMenuID);
							strKeyName.Format("%s", menuName);
							GetPrivateProfileStringA(strSection, strKeyName, "null", strValue.GetBuffer(MAX_PATH), MAX_PATH, m_strLanguageFilePath);
							strValue.ReleaseBuffer();
							cMenuSSub->ModifyMenu(j, MF_BYPOSITION | MF_STRING, IDIndex, strValue);//修改当前ID的内容
						}
					}
				}
			}
			else
			{
				continue;
			}
		}
	}
}

int	CLanguage::GetLanguageType()
{
	return (int)m_eLanguageType;
}

int CLanguage::Messagebox(CString strCN, CString strEN, CString strTC)
{
	if (Chinese == m_eLanguageType)
		return BCGPMessageBox(NULL, strCN, "提示",MB_OK);
	else if(English == m_eLanguageType)
		return BCGPMessageBoxEx(NULL, strEN, "Tips", MB_OK, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	else if (TraditionalChinese == m_eLanguageType)
		return BCGPMessageBox(strTC);
}

int CLanguage::MessageboxOKCancel(CString strCN, CString strEN, CString strTC)
{
	if (Chinese == m_eLanguageType)
		return BCGPMessageBox(NULL, strCN, "提示", MB_OKCANCEL);
	else if (English == m_eLanguageType)
		return BCGPMessageBoxEx(NULL, strEN, "Tips", MB_OKCANCEL, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	else if (TraditionalChinese == m_eLanguageType)
		return BCGPMessageBox(strTC, MB_OKCANCEL);
}

int CLanguage::MessageboxError(CString strCN, CString strEN, CString strTC)
{
	if (Chinese == m_eLanguageType)
		return BCGPMessageBox(NULL, strCN, "错误", MB_ICONERROR);
	else if (English == m_eLanguageType)
		return BCGPMessageBoxEx(NULL, strEN, "Error", MB_ICONERROR, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	else if (TraditionalChinese == m_eLanguageType)
		return BCGPMessageBox(strTC, MB_ICONERROR);
}

int CLanguage::MessageboxQuestion(CString strCN, CString strEN, CString strTC)
{
	if (Chinese == m_eLanguageType)
		return BCGPMessageBox(NULL, strCN, "问题", MB_ICONQUESTION | MB_YESNO);
	else if (English == m_eLanguageType)
		return BCGPMessageBoxEx(NULL, strEN, "Question", MB_ICONQUESTION | MB_YESNO, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	else if (TraditionalChinese == m_eLanguageType)
		return BCGPMessageBox(strTC, MB_ICONQUESTION | MB_YESNO);
}


int CLanguage::MessageboxWarn(CString strCN, CString strEN, CString strTC)
{
	if (Chinese == m_eLanguageType)
		return BCGPMessageBox(NULL, strCN, "警告", MB_ICONWARNING | MB_YESNO);
	else if (English == m_eLanguageType)
		return BCGPMessageBoxEx(NULL, strEN, "Warn", MB_ICONWARNING | MB_YESNO, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	else if (TraditionalChinese == m_eLanguageType)
		return BCGPMessageBox(strTC, MB_ICONWARNING | MB_YESNO);
}