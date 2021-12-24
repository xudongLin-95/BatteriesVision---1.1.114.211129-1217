// DlgMesSet.cpp : 实现文件
#include "stdafx.h"
#include "SCIMes.h"
#include "DlgMesSet.h"
#include "afxdialogex.h"
#include "DlgInfo.h"
// CDlgMesSet 对话框
IMPLEMENT_DYNAMIC(CDlgMesSet, CBCGPDialog)

CDlgMesSet::CDlgMesSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgMesSet::IDD, pParent)
	, m_RealMes(NULL)

	, m_Edit_strFactory(_T(""))
	, m_Edit_strEQM(_T(""))
	, m_Edit_strLineCode(_T(""))
	, m_Edit_strSectionCode(_T(""))
	, m_Edit_strEMSN(_T(""))
	, m_Edit_strEMName(_T(""))
	, m_Edit_nMesServerPort(0)
	, m_Edit_strLocalFilePath(_T(""))
	, m_Edit_strLocalFileDirectory(_T(""))
	, m_Edit_BatchNumber(_T(""))
	, m_Check_bUploadFile(FALSE)
	, m_Edit_strReportAddr(_T(""))
{
	m_Edit_cstrResourceCode = _T("");
	m_Edit_cstrFtpServerAddr = _T("");
	m_Edit_cstrFtpServerID = _T("");
	m_Edit_cstrMesServerAddr = _T("");
}

CDlgMesSet::~CDlgMesSet()
{
}

void CDlgMesSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RESOURCECODE, m_Edit_cstrResourceCode);
	DDX_Text(pDX, IDC_EDIT_FTPSERVERADDR, m_Edit_cstrFtpServerAddr);
	DDX_Text(pDX, IDC_EDIT_FTPSERVERID, m_Edit_cstrFtpServerID);
	DDX_Text(pDX, IDC_EDIT_FTPSERVERPASSWORD, m_Edit_cstrFtpServerPassword);
	DDX_Text(pDX, IDC_EDIT_MESSERVERADDR, m_Edit_cstrMesServerAddr);
	DDX_Text(pDX, IDC_EDIT_FACTORY, m_Edit_strFactory);
	DDX_Text(pDX, IDC_EDIT_EQM, m_Edit_strEQM);
	DDX_Text(pDX, IDC_EDIT_LINECODE, m_Edit_strLineCode);
	DDX_Text(pDX, IDC_EDIT_SECTIONCODE, m_Edit_strSectionCode);
	DDX_Text(pDX, IDC_EDIT_EMSN, m_Edit_strEMSN);
	DDX_Text(pDX, IDC_EDIT_EMNAME, m_Edit_strEMName);
	DDX_Text(pDX, IDC_EDIT_MESSERVERPORT, m_Edit_nMesServerPort);
	DDX_Text(pDX, IDC_EDIT_LOCALFILEPATH, m_Edit_strLocalFilePath);
	DDX_Text(pDX, IDC_EDIT_LOCALFILE, m_Edit_strLocalFileDirectory);
	DDX_Text(pDX, IDC_EDIT_BATCH_NUMBER, m_Edit_BatchNumber);
	DDX_Check(pDX, IDC_CHECK_USE_UPLOAD_AUTO, m_Check_bUploadFile);
	DDX_Text(pDX, IDC_EDIT_REPORTADDR, m_Edit_strReportAddr);
}

BEGIN_MESSAGE_MAP(CDlgMesSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDlgMesSet::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CDlgMesSet::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDCANCEL, &CDlgMesSet::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgMesSet::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CDlgMesSet::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_SELECTROOTPATH, &CDlgMesSet::OnBnClickedButtonSelectrootpath)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_FILE, &CDlgMesSet::OnBnClickedButtonUploadFile)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_UPLOAD_DIRECTORY, &CDlgMesSet::OnBnClickedButtonUploadUploadDirectory)
END_MESSAGE_MAP()


// CDlgMesSet 消息处理程序
BOOL CDlgMesSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	InitUI();
	
	return TRUE;  
}
//选择需要上传的文件
void CDlgMesSet::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_strFileName = TEXT("");
	//时间
	SYSTEMTIME	tSaveTime;
	::GetLocalTime(&tSaveTime);
	CString sTime;

	sTime.Format("%04d%02d\\%02d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay);

	TCHAR szFilter[] = _T("所有文件(*.*)|*.*||"); 
	CFileDialog FileDlg(TRUE, _T("*.*"), NULL, 0, szFilter, this); 
	if(IDOK == FileDlg.DoModal())
	{
		m_Edit_strLocalFilePath = FileDlg.GetPathName();

		//上传地址 \\ 年月 \\ 日 \\ 卷号 \\ 文件名(厂号_设备号_产品名_卷号.csv)
		m_strFilePath.Format(_T("%s\\%s\\%s"), m_Edit_cstrFtpServerAddr, sTime, m_Edit_BatchNumber);
		m_strFileName.Format(_T("%s\\%s"), m_strFilePath ,FileDlg.GetFileName());
	
		UpdateData(FALSE);//变量刷新至控件
	}
	else
	{
		return;
	}	
}

//1
void CDlgMesSet::OnBnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	//system("net use \\\\192.168.0.33\\ccd数据 Nd-myname /user:ATL-BM\NW211FS-Coating2");
	BOOL bExist = FALSE;
	try
	{
		if (!CopyFileA(m_Edit_strLocalFilePath, m_Edit_cstrFtpServerAddr, bExist))
		{
			MessageBoxA(_T("上传失败"));
			return;
		}
		MessageBoxA(_T("上传成功"));
	}
	catch (...)
	{
		MessageBoxA(_T("上传失败"));
	}
}

//1
void CDlgMesSet::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CBCGPDialog::OnCancel();
}
//1
void CDlgMesSet::OnBnClickedButtonSave()
{
	Save();
}
//上传整个文件夹
void CDlgMesSet::OnBnClickedButtonSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	TCHAR           szFolderPath[MAX_PATH] = {0};  
	m_strFolderPath = TEXT("");  

	//时间
	SYSTEMTIME	tSaveTime;
	::GetLocalTime(&tSaveTime);
	CString sTime;

	sTime.Format("%04d%02d\\%02d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay);

	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("");		//请选择处理结果存储路径
	sInfo.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_DONTGOBELOWDOMAIN;  
	sInfo.lpfn     = NULL;  
	
	//显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
		{  
			m_Edit_strLocalFileDirectory = szFolderPath;

			//上传地址(192.168.0.33/ccd数据)\\(年月) \\ 日 \\ 卷号
			m_strFolderPath.Format(_T("%s\\%s\\%s"), m_Edit_cstrFtpServerAddr,sTime, m_Edit_BatchNumber);

			UpdateData(FALSE);
		}  
	 }  
	 if(lpidlBrowse != NULL)  
	 {  
			::CoTaskMemFree(lpidlBrowse);  
	 } 
		 
}

BOOL CDlgMesSet::CopyDirectory(CString strSrcPath,CString strDestPath)
{
	try
	{
		CFileFind m_sFileFind;
		if (strSrcPath.IsEmpty())
		{
			OutputDebugString("源文件名为空，无法进行拷贝!");
			return FALSE;
		}
		if (!m_sFileFind.FindFile(strDestPath))
		{
			CreateDirectory(strDestPath, NULL);//创建目标文件夹
		}
		CFileFind finder;
		CString path;
		path.Format("%s/*.*", strSrcPath);
		//AfxMessageBox(path);
		BOOL bWorking = finder.FindFile(path);
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDirectory() && !finder.IsDots())//是文件夹 而且 名称不含 . 或 ..
			{
				CopyDirectory(finder.GetFilePath(), strDestPath + "/" + finder.GetFileName());//递归创建文件夹+"/"+finder.GetFileName()
			}
			else
			{//是文件，则直接复制
			 //AfxMessageBox("复制文件"+finder.GetFilePath());//+finder.GetFileName()
				CopyFileA(finder.GetFilePath(), strDestPath + "/" + finder.GetFileName(), FALSE);
			}
		}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}
//FTP 上传，后面需要完善
int CDlgMesSet::FtpUploadFile(CString & strFullFilePath)
{
	CString strServerFilePath;
	m_RealMes->LocalFilePathToServerFilePath(strFullFilePath,strServerFilePath,""/*m_strRootPathSymbol*/);	
	int nRet = m_RealMes->FtpUpload(strServerFilePath,strFullFilePath/*本地路径*/);
	if ( nRet > 0)
	{
		CString strException;
		strException.Format(_T("FtpUpload错误！\t错误码：%d"),nRet);
		m_RealMes->Log(strException);
		return nRet;
	}
	return nRet;
}

//初始化MES设置界面
void CDlgMesSet::InitUI(void)
{
	m_Edit_strLineCode = m_RealMes->m_strLineCode;
	m_Edit_strSectionCode = m_RealMes->m_strSectionCode;
	m_Edit_strEMSN = m_RealMes->m_strEquipmentCode;
	m_Edit_strEMName = m_RealMes->m_strEquipmentName;
	m_Edit_strFactory = m_RealMes->m_strFactory;
	m_Edit_strEQM = m_RealMes->m_strEQM;
	m_Edit_cstrResourceCode = m_RealMes->m_cstrResourceCode;
	m_Edit_strLocalFilePath = m_RealMes->m_strLocalFilePath;

	m_Edit_nMesServerPort = m_RealMes->m_nPort;
	m_Edit_cstrMesServerAddr = m_RealMes->m_strIP;
	m_Edit_cstrFtpServerAddr = m_RealMes->m_strFtpAddr;
	m_Edit_strReportAddr = m_RealMes->m_strReportAddr;
	m_Edit_cstrFtpServerID = m_RealMes->m_strFtpUserID;
	m_Edit_cstrFtpServerPassword = m_RealMes->m_strFtpPwd;

	m_Edit_BatchNumber = "BatchNO";

	m_Check_bUploadFile = m_RealMes->m_bUploadFileAuto;

	((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_FILE))->SetCheck(m_RealMes->tSelectUploadFile.bUploadFile);
	((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_ALLFILE))->SetCheck(m_RealMes->tSelectUploadFile.bUploadAllFile);
	((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_DEFECTFILE))->SetCheck(m_RealMes->tSelectUploadFile.bUploadDefectFile);
	((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_REPORTFILE))->SetCheck(m_RealMes->tSelectUploadFile.bUploadReportFile);

	//变量到控件的刷新
	UpdateData(FALSE);
}
//选择本地文件路径
void CDlgMesSet::OnBnClickedButtonSelectrootpath()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};  
	CString         strFolderPath = TEXT("");  

	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("");		//请选择处理结果存储路径
	sInfo.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_DONTGOBELOWDOMAIN;  
	sInfo.lpfn     = NULL;  

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
		{  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	} 

	UpdateData(FALSE);
}
//保存MES设置参数
BOOL CDlgMesSet::Save(void)
{
	UpdateData(TRUE);

	CString cstrOPLog;

	m_RealMes->m_strLineCode = m_Edit_strLineCode;
	m_RealMes->m_strSectionCode = m_Edit_strSectionCode;
	m_RealMes->m_strEquipmentCode = m_Edit_strEMSN;
	m_RealMes->m_strEquipmentName = m_Edit_strEMName;
	m_RealMes->m_strFactory = m_Edit_strFactory;
	m_RealMes->m_strEQM = m_Edit_strEQM;
	m_RealMes->m_cstrResourceCode = m_Edit_cstrResourceCode;
	m_RealMes->m_strLocalFilePath = m_Edit_strLocalFilePath;
	m_RealMes->m_nPort = m_Edit_nMesServerPort;
	m_RealMes->m_strIP = m_Edit_cstrMesServerAddr;
	m_RealMes->m_strFtpAddr = m_Edit_cstrFtpServerAddr;
	m_RealMes->m_strReportAddr = m_Edit_strReportAddr;
	m_RealMes->m_strFtpUserID = m_Edit_cstrFtpServerID;
	m_RealMes->m_strFtpPwd = m_Edit_cstrFtpServerPassword;
	m_RealMes->m_bUploadFileAuto = m_Check_bUploadFile;

	m_RealMes->tSelectUploadFile.bUploadFile= ((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_FILE))->GetCheck();
	m_RealMes->tSelectUploadFile.bUploadAllFile = ((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_ALLFILE))->GetCheck();
	m_RealMes->tSelectUploadFile.bUploadDefectFile = ((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_DEFECTFILE))->GetCheck();
	m_RealMes->tSelectUploadFile.bUploadReportFile = ((CButton*)GetDlgItem(IDC_CHECK_UPLOAD_REPORTFILE))->GetCheck();

	m_RealMes->SetMesAddrAndPort();
	m_RealMes->m_record.m_nLogType = m_RealMes->m_nLogType;
	m_RealMes->WriteParams();

	MessageBox(_T("参数保存成功！"));
	return TRUE;
}

//手动上传
void CDlgMesSet::OnBnClickedButtonUploadFile()
{
	// TODO: 在此添加控件通知处理程序代码

	BOOL bExist = FALSE;
	try
	{
		//判断是否能连接共享文件夹
		//TCHAR szPasswd[] = TEXT("");          //共享资源授权用户的密码
		//TCHAR szUserName[] = TEXT("Administrator");        //共享资源授权的用户
		NETRESOURCE net_Resource;
		net_Resource.dwDisplayType = RESOURCEDISPLAYTYPE_DIRECTORY;
		net_Resource.dwScope = RESOURCE_CONNECTED;
		net_Resource.dwType = RESOURCETYPE_ANY;
		net_Resource.dwUsage = 0;
		net_Resource.lpComment = TEXT("");
		net_Resource.lpLocalName = TEXT("");  //映射成本地驱动器:
		net_Resource.lpProvider = NULL;
		net_Resource.lpRemoteName = (LPSTR)(LPCTSTR)g_This->m_strFtpAddr; // \\servername\共享资源名

		DWORD dwFlags = CONNECT_TEMPORARY;
		/*DWORD dw = WNetAddConnection2A(&net_Resource, g_This->m_strFtpPwd, g_This->m_strFtpUserID, dwFlags);
		if (dw != ERROR_SUCCESS)
		{
			CString strMess;
			strMess.Format("上传文件失败(错误码%d),无法连接服务器%s", dw, g_This->m_strFtpAddr);
			g_This->m_sInit.Log(strMess, AlarmLog, SlightExcp);
			return;
		}*/

		if (!PathFileExists(m_strFilePath)) CreateMultilevelPath(m_strFilePath);

		if (!CopyFileA(m_Edit_strLocalFilePath, m_strFileName, bExist))
		{
			BCGPMessageBox(_T("上传失败"));
			return;
		}
		BCGPMessageBox(_T("上传成功"));
	}
	catch (...)
	{
		BCGPMessageBox(_T("上传失败"));
	}
}

void CDlgMesSet::OnBnClickedButtonUploadUploadDirectory()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!PathFileExists(m_strFolderPath)) CreateMultilevelPath(m_strFolderPath);
	if (!CopyDirectory(m_Edit_strLocalFileDirectory, m_strFolderPath))
	{
		MessageBoxA(_T("上传文件夹失败"));
	}
}
