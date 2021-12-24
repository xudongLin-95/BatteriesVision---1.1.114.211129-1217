#pragma once
#include "resource.h"
#include "dlginfo.h"
#include "afxwin.h"
#include "DlgRecipes.h"
#include "BCGCBProInc.h"//背景头文件

#include "Mes.h"
#define THREADCOUNT 5

class CDlgMesSet;
// CDlgMesSet 对话框
#define WM_UPDATE_MESSAGE (WM_USER+300)
class CDlgMesSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgMesSet)

public:
	CDlgMesSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMesSet();

// 对话框数据
	enum { IDD = IDD_DLGMESSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	//设备信息
	CString m_Edit_strFactory;    //工厂编辑框关联变量
	CString m_Edit_strEQM;        //设备厂家编辑框关联变量
	CString m_Edit_strLineCode;   //线体编号编辑框关联变量
	CString m_Edit_strSectionCode;//工段编号编辑框关联变量
	CString m_Edit_strEMSN;       //设备编号编辑框关联变量
	CString m_Edit_strEMName;     //设备名称编辑框关联变量
	CString m_Edit_cstrResourceCode; //资源编号编辑框关联变量
	//MES服务器
	CString m_Edit_cstrMesServerAddr; //MES服务器IP编辑框关联变量
	int m_Edit_nMesServerPort;    //MES服务器端口号编辑框关联变量
	//FTP服务器
	CString m_Edit_cstrFtpServerAddr;    //FTP服务器地址编辑框关联变量
	CString m_Edit_cstrFtpServerID;      //FTP账号编辑框关联变量
	CString m_Edit_cstrFtpServerPassword;//FTP密码编辑框关联变量
	//手动操作文件路径
	CString m_Edit_strLocalFilePath;     //本地文件路径
	CString m_Edit_strLocalFileDirectory;//本地文件夹路径

	CString m_Edit_BatchNumber; //需要保存的膜卷号

	CString m_strFileName;//需要上传文件名
	CString m_strFilePath;//文件路径
	CString m_strFolderPath;//文件夹名
	CMes *m_RealMes;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonSelectrootpath();

public:
	int FtpUploadFile(CString & strFullFilePath);
	void InitUI(void);
	BOOL Save(void);				
	BOOL CopyDirectory(CString strSrcPath,CString strDestPath);

	afx_msg void OnBnClickedButtonUploadFile();
	afx_msg void OnBnClickedButtonUploadUploadDirectory();	
	BOOL m_Check_bUploadFile;
	CString m_Edit_strReportAddr;
};

