// DlgInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SCIMes.h"
#include "DlgInfo.h"
#include "afxdialogex.h"


// CDlgInfo �Ի���

IMPLEMENT_DYNAMIC(CDlgInfo, CDialogEx)

CDlgInfo::CDlgInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgInfo::IDD, pParent)
	, m_strUploadInfo(_T(""))
	, m_iCount(0)
	, m_iComplete(0)
{

}

CDlgInfo::~CDlgInfo()
{
}

void CDlgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_UPDATA, m_strUploadInfo);
	DDX_Control(pDX, IDC_STATIC_UPDATA, m_staticInfo);
}


BEGIN_MESSAGE_MAP(CDlgInfo, CDialogEx)
	ON_MESSAGE(WM_UPDATE_MESSAGE, OnUpdateMessage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()






BOOL CDlgInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetLocalTime(&m_beginTime);

	m_strUploadInfo.Format(_T("һ��%d���ļ����Ѿ��ϴ�%d���ļ�"),m_iCount,m_iComplete);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
LRESULT CDlgInfo::OnUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	m_iComplete++;
	m_strUploadInfo.Format(_T("һ��%d���ļ����Ѿ��ϴ�%d���ļ�"),m_iCount,m_iComplete);
	UpdateData(FALSE);
	return 0;
}


void CDlgInfo::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	GetLocalTime(&m_EndTime);

	__int64 iSpeedTime = TimeDiff(m_EndTime,m_beginTime);

	CString strSpendTime,strMessage;

	MillisecondToHours(iSpeedTime,strSpendTime);

	strMessage.Format(_T("һ������ϴ�%d���ļ���һ����ʱ%s"),m_iComplete,strSpendTime);

	MessageBox(strMessage);

	m_iComplete = 0;

	CDialogEx::OnClose();
}

__int64 TimeDiff(SYSTEMTIME left, SYSTEMTIME right)  
{
	CTime tmLeft(left.wYear, left.wMonth, left.wDay, 0, 0, 0);
	CTime tmRight(right.wYear, right.wMonth, right.wDay, 0, 0, 0);

	CTimeSpan sp = tmLeft - tmRight;
	long MillisecondsL = (left.wHour*3600 + left.wMinute*60 + left.wSecond)*1000 + left.wMilliseconds;  
	long MillisecondsR = (right.wHour*3600 + right.wMinute*60 + right.wSecond)*1000 + right.wMilliseconds;  

	return  (__int64)sp.GetDays()*86400000 + (MillisecondsL - MillisecondsR);//�˴����غ���
}

BOOL MillisecondToHours(__int64 const& Millisecond,CString& strHours)
{
	__int64 inMillisecond = Millisecond;

	__int64 iMillisecond;
	__int64 iSecond;
	__int64 iMinute;
	__int64	iHour;

	iMillisecond = inMillisecond % 1000;

	inMillisecond = inMillisecond /1000;

	iSecond = inMillisecond % 60;

	inMillisecond = inMillisecond /60;

	iMinute = inMillisecond % 60;

	iHour = inMillisecond /60;

	strHours.Format(_T("%dСʱ%d����%d��%d����"),iHour,iMinute,iSecond,iMillisecond);

	return TRUE;
}