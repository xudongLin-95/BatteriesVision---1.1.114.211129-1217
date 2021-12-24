#pragma once
#include "resource.h"
#include "dlginfo.h"
#include "afxwin.h"
#include "DlgRecipes.h"
#include "BCGCBProInc.h"//����ͷ�ļ�

#include "Mes.h"
#define THREADCOUNT 5

class CDlgMesSet;
// CDlgMesSet �Ի���
#define WM_UPDATE_MESSAGE (WM_USER+300)
class CDlgMesSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgMesSet)

public:
	CDlgMesSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMesSet();

// �Ի�������
	enum { IDD = IDD_DLGMESSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	//�豸��Ϣ
	CString m_Edit_strFactory;    //�����༭���������
	CString m_Edit_strEQM;        //�豸���ұ༭���������
	CString m_Edit_strLineCode;   //�����ű༭���������
	CString m_Edit_strSectionCode;//���α�ű༭���������
	CString m_Edit_strEMSN;       //�豸��ű༭���������
	CString m_Edit_strEMName;     //�豸���Ʊ༭���������
	CString m_Edit_cstrResourceCode; //��Դ��ű༭���������
	//MES������
	CString m_Edit_cstrMesServerAddr; //MES������IP�༭���������
	int m_Edit_nMesServerPort;    //MES�������˿ںű༭���������
	//FTP������
	CString m_Edit_cstrFtpServerAddr;    //FTP��������ַ�༭���������
	CString m_Edit_cstrFtpServerID;      //FTP�˺ű༭���������
	CString m_Edit_cstrFtpServerPassword;//FTP����༭���������
	//�ֶ������ļ�·��
	CString m_Edit_strLocalFilePath;     //�����ļ�·��
	CString m_Edit_strLocalFileDirectory;//�����ļ���·��

	CString m_Edit_BatchNumber; //��Ҫ�����Ĥ���

	CString m_strFileName;//��Ҫ�ϴ��ļ���
	CString m_strFilePath;//�ļ�·��
	CString m_strFolderPath;//�ļ�����
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

