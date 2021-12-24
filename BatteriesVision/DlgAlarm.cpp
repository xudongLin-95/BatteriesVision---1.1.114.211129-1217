// DlgAlarm.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgAlarm.h"
#include "afxdialogex.h"


// CDlgAlarm �Ի���

IMPLEMENT_DYNAMIC(CDlgAlarm, CBCGPDialog)

CDlgAlarm::CDlgAlarm(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgAlarm::IDD, pParent)
{
	m_ftInfo.CreatePointFont(220,"����");
	m_ftDiscribe.CreatePointFont(220,"����");
	m_ftSolution.CreatePointFont(120,"����");
	m_ftObject.CreatePointFont(220,"����");

	InitializeCriticalSection(&m_csLock);

	m_vecErrorInfoList.clear();//ʵʱ�����洢
	m_vecAllErrorInfos.clear();//���ؼ����б�
}

CDlgAlarm::~CDlgAlarm()
{
	m_ftInfo.DeleteObject();
	m_ftDiscribe.DeleteObject();
	m_ftSolution.DeleteObject();
	m_ftObject.DeleteObject();
}

void CDlgAlarm::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	/*DDX_Control(pDX, IDC_STATIC_DISCRIBE, m_STC_Discribe);
	DDX_Control(pDX, IDC_STATIC_INFO, m_STC_Info);
	DDX_Control(pDX, IDC_STATIC_SOLUTION, m_STC_Solution);*/
	DDX_Control(pDX, IDC_LIST_ALARM_SHOW, m_LIST_Show);
}


BEGIN_MESSAGE_MAP(CDlgAlarm, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgAlarm::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgAlarm ��Ϣ�������


BOOL CDlgAlarm::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_LIST_Show.SetBkColor(RGB(58, 58, 63));
	m_LIST_Show.SetSelectionMark(RGB(58, 58, 63));
	m_LIST_Show.SetTextBkColor(RGB(255, 0, 0));
	m_LIST_Show.SetExtendedStyle(m_LIST_Show.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	CString  str[] = { "ID","����","�������" };
	m_LIST_Show.InsertColumn(0, str[0],LVCFMT_LEFT,30);
	m_LIST_Show.InsertColumn(1, str[1], LVCFMT_LEFT, 400);
	m_LIST_Show.InsertColumn(2, str[2], LVCFMT_LEFT, 500);


	InitUI();
	SetTimer(NULL, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CDlgAlarm::GetErrorCodeListFromXml()
{
	try
	{
		CString strFilePath;
		strFilePath.Format("%s\\Config\\Public\\ErrorCodeInfo", GetCurrentAppPath());
		if (!PathFileExists(strFilePath))
			CreateMultilevelPath(strFilePath);

		m_strErrorCodeFilePath.Format("%s\\Config\\Public\\ErrorCodeInfo\\ErrorCode.xml", GetCurrentAppPath());
		m_cErrorInfo.GetErrorCodeList(m_strErrorCodeFilePath, m_vecAllErrorInfos);
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (HException& e)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

void CDlgAlarm::ShowAlarm(CString strAlarmInfo)
{
	CErrorInfo::tagErrorInfo tInfo;
	tInfo.strDescribe = strAlarmInfo;
	tInfo.nID = 0;
	tInfo.strSolution = "";

	ShowAlarm(tInfo);
}

void CDlgAlarm::ShowAlarm(int nErrorCode)
{
	CErrorInfo::tagErrorInfo tInfo;
	tInfo = theGlobal.m_ErrorInfo.GetErrorInfo(nErrorCode, m_vecAllErrorInfos);
	if (tInfo.nID < 0) return;
	
	/*tInfo.nCode = nErrorCode;
	tInfo.strDescribe.Format("��ʱ��δ������ǰ���޾��״̬");
	tInfo.strSolution.Format("����Ĥ���");*/

	ShowAlarm(tInfo);
}

void CDlgAlarm::ShowAlarm(CErrorInfo::tagErrorInfo tInfo)
{
	if (m_tInfo.strDescribe == tInfo.strDescribe) return;

	int MAX_EXCP_COUNT = 10;
	for (int i = 0; i < m_vecErrorInfoList.size(); i++)
	{
		//if (m_vecErrorInfoList[i].nID == tInfo.nID) return;
		if (m_vecErrorInfoList[i].strDescribe == tInfo.strDescribe) return;
	}

	CLockHelper lk(&m_csLock);
	m_vecErrorInfoList.push_back(tInfo);
	if (m_vecErrorInfoList.size() > MAX_EXCP_COUNT)
	{
		CString strExcp;
		strExcp.Format("CDlgAlarm_ShowAlarm: �쳣������Ϣ����%d, �Ѻ��Բ�����Ϣ", MAX_EXCP_COUNT);
		theGlobal.m_Log.Exception(strExcp);
	}
}

BOOL CDlgAlarm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)//����ESC
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE)//
		{
			return TRUE;
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CDlgAlarm::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CDlgAlarm::InitUI()
{
	//m_STC_Info.SetBkColor(RGB(58, 58, 63));
	//m_STC_Info.SetTextColor(RGB(255, 0, 0));//248, 182, 43
	//m_STC_Info.SetFont(&m_ftInfo);

	//m_STC_Discribe.SetBkColor(RGB(255, 0, 0));
	//m_STC_Discribe.SetTextColor(RGB(255, 255, 255));//248, 182, 43
	//m_STC_Discribe.SetFont(&m_ftDiscribe);

	//m_STC_Solution.SetBkColor(RGB(58, 58, 63));
	//m_STC_Solution.SetTextColor(RGB(0, 255, 0));
	//m_STC_Solution.SetFont(&m_ftSolution);
}

void CDlgAlarm::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	{
		CLockHelper lk(&m_csLock);
		m_tInfo.strDescribe = "";
		m_tInfo.nID = 0;
		m_tInfo.strSolution = "";
	}
	ShowWindow(SW_HIDE);
}


void CDlgAlarm::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (this->IsWindowVisible() || m_vecErrorInfoList.size()==0) return;

	
	{
		CLockHelper lk(&m_csLock);
		m_tInfo = m_vecErrorInfoList.front();
		//
		std::vector<CErrorInfo::tagErrorInfo> n_vecErrorInfoList;
		n_vecErrorInfoList = m_vecErrorInfoList;
		m_vecErrorInfoList.clear();
		for (vector<CErrorInfo::tagErrorInfo>::iterator it=n_vecErrorInfoList.begin();it !=n_vecErrorInfoList.end();it++)
		{
			CString strID, strDescribe, strSolution;
			strID.Format("%d", it->nID);
			strDescribe.Format(it->strDescribe);
			strSolution.Format(it->strSolution);
			m_LIST_Show.InsertItem(0, strID);
			m_LIST_Show.SetItemText(0, 1, strDescribe);
			m_LIST_Show.SetItemText(0, 2, strSolution);
		}
	}
	
	/*m_STC_Info.Format("%d", m_tInfo.nID);
	m_STC_Discribe.Format(m_tInfo.strDescribe);
	m_STC_Solution.Format(m_tInfo.strSolution);*/

	ShowWindow(SW_NORMAL);

	{
		CLockHelper lk(&m_csLock);
		if(m_vecErrorInfoList.size() > 0)
			m_vecErrorInfoList.erase(m_vecErrorInfoList.begin());
	}
	
	CBCGPDialog::OnTimer(nIDEvent);
}
