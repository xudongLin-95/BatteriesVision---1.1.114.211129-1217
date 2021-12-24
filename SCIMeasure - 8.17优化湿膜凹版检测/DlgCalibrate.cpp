// DlgCalibrate.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SCISeparate.h"
#include "DlgCalibrate.h"
#include "afxdialogex.h"


// CDlgCalibrate �Ի���

IMPLEMENT_DYNAMIC(CDlgCalibrate, CDialogEx)

CDlgCalibrate::CDlgCalibrate(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCalibrate::IDD, pParent)
	, m_EDT_dWidth(0)
	, m_EDT_dHeight(0)
{
	m_pOwner = NULL;
}

CDlgCalibrate::~CDlgCalibrate()
{
}

void CDlgCalibrate::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_EDT_dWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_EDT_dHeight);
}


BEGIN_MESSAGE_MAP(CDlgCalibrate, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgCalibrate::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgCalibrate::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgCalibrate ��Ϣ�������


void CDlgCalibrate::OnBnClickedOk()
{
	UpdateData(TRUE);
	double A,B,M,C,D,N;
	A = (m_hvColumn[1] - m_hvColumn[0])*(m_hvColumn[1] - m_hvColumn[0]);
	B = (m_hvRow[1] - m_hvRow[0])*(m_hvRow[1] - m_hvRow[0]);
	M =  m_EDT_dHeight * m_EDT_dHeight;
	C = (m_hvColumn[3] - m_hvColumn[2])*(m_hvColumn[3] - m_hvColumn[2]);
	D = (m_hvRow[3] - m_hvRow[2])*(m_hvRow[3] - m_hvRow[2]);
	N = m_EDT_dWidth * m_EDT_dWidth;
	TupleSqrt((B*N - D*M)/(B*C - A*D),&m_hvColResolution);
	TupleSqrt((A*N - C*M)/(A*D - C*B),&m_hvRowResolution);
	CString strMessageBox;
	strMessageBox.Format("����ó�\r\n �з���ķֱ���Ϊ:%.5f\r\n�з����ϵķֱ���Ϊ%.5f\r\n�Ƿ����?",
		m_hvRowResolution.D(),m_hvColResolution.D());
	if (IDYES == AfxMessageBox(strMessageBox,MB_YESNO))
	{
		m_pOwner->m_dRowResolution = m_hvRowResolution.D();
		m_pOwner->m_dColResolution = m_hvColResolution.D();
		CDialogEx::OnOK();
	}
}


void CDlgCalibrate::OnBnClickedCancel()
{
	
	CDialogEx::OnCancel();
}
