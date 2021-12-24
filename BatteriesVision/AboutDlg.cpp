#include "stdafx.h"
#include "AboutDlg.h"


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

CAboutDlg::CAboutDlg() : CBCGPDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBatteriesVisionDlg �Ի���
BOOL CAboutDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE, TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CAboutDlg::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg;
	GetClientRect(rcDlg);
	rcGridBase.top = rcDlg.top + 80;
	rcGridBase.bottom = rcDlg.bottom - 50;
	rcGridBase.left = rcDlg.left + 12;
	rcGridBase.right = rcDlg.right / 2 - 4;
	m_Grid_VersionList.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_VersionList, theme);
	m_Grid_VersionList.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_VersionList.SetColorTheme(theme);
	m_Grid_VersionList.SetReadOnly(TRUE);//�ɱ༭
	m_Grid_VersionList.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_Grid_VersionList.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_Grid_VersionList.EnableRowHeader(TRUE);//ʹ���б�ͷ
	m_Grid_VersionList.EnableLineNumbers(TRUE);//ʹ�������
	m_Grid_VersionList.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_VersionList.EnableInvertSelOnCtrl(FALSE);
	m_Grid_VersionList.InsertColumn(0, "Module", 150);
	m_Grid_VersionList.InsertColumn(1, "Version", rcGridBase.Width() - 210);
	m_Grid_VersionList.LoadState(_T("BasicGrid"));
	m_Grid_VersionList.AdjustLayout();//�൱��������ʾ�ڱ����
	m_Grid_VersionList.ShowWindow(SW_NORMAL);

	RefreshModuleVersion();

	rcGridBase.top = rcDlg.top + 10;
	rcGridBase.bottom = rcDlg.bottom - 50;
	rcGridBase.left = rcDlg.right / 2 + 4;
	rcGridBase.right = rcDlg.right - 12;
	m_EDIT_UpdateInfo.Create(WS_CHILD, rcGridBase, this, 2000);
	m_EDIT_UpdateInfo.EnableAutoOutlining();
	m_EDIT_UpdateInfo.EnableOutlining();
	m_EDIT_UpdateInfo.SetReadOnly();
	m_EDIT_UpdateInfo.ShowWindow(SW_NORMAL);

	RefreshVersionInfo();
}

void CAboutDlg::RefreshModuleVersion()
{
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextA("Version: " + theGlobal.GetVersion());


	HWND hwndList = m_Grid_VersionList.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);//SetWindowRedraw
	m_Grid_VersionList.RemoveAll();
	int nCount = 0;
	CBCGPGridRow* pRow;
	CString strTmp;

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("System Main Program"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("BatteriesVision.exe"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("Vision Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("halconxl.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("Measure Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIMeasure.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("FindMark Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIFindMark.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("GrabberDS Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIGrabberDS.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("IO Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIIO.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("PLC Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIPLC.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("Rectification Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("SCIRectification.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("PowerLimit Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("PowerLimit.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("LightControl Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("LightControl.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	pRow = m_Grid_VersionList.CreateRow(m_Grid_VersionList.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t("CrashRecord Libary"));
	pRow->GetItem(1)->SetValue(_variant_t(GetFileVersion(GetModulePath("CrashRecord.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);

	/*pRow = m_Grid_VersionList.CreateRow (m_Grid_VersionList.GetColumnCount());
	pRow->GetItem (0)->SetValue (_variant_t ("MES Libary"));
	pRow->GetItem (1)->SetValue (_variant_t (GetFileVersion(GetModulePath("SCIMES.dll"))));
	m_Grid_VersionList.AddRow(pRow, FALSE);*/


	m_Grid_VersionList.AdjustLayout();
	SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
}

void CAboutDlg::RefreshVersionInfo()
{
	CString strInfo = "\n�汾��������:\n\n";

	strInfo += "---------------Version 1.1.114.211129---------------\n"
		"\t1.�޸ĵ����ʾ�б��޸ĵ���ж��б�\n"
		"\t2.�����׼��Զ����㲹�����ܣ�\n"
		"\t3.�Ż��洢��PLC���������Ϣ����߼���������ʱ�Զ���ն��У�\n\n";

	strInfo += "---------------Version 1.1.113.211125---------------\n"
		"\t1.�޸ĵ���㷨��һ�����4��Բֱ����3��Բ�ľࣻ\n"
		"\t2.������ò�����������һҳ��\n"
		"\t3.�������ļ���ʽ���ж�NG����ʾ�߼������޸ģ�\n\n";

	strInfo += "---------------Version 1.1.111.210902---------------\n"
		"\t1.�Ż�����ջ��߼���\n"
		"\t2.�����׼��������ã�\n\n";

	strInfo += "---------------Version 1.1.109.210831---------------\n"
		"\t1.�����޸Ĳ����ͨ��Ϊһ��ģ�飻\n"
		"\t2.�Ż��㷨Ѱ���߼�,�Ż�����ͿĤ���ұߣ�\n\n";

	strInfo += "---------------Version 1.1.107.210819---------------\n"
		"\t1.���Ӳ����ͨ�ţ���ȡ�������ݣ�\n"
		"\t2.�Ż��㷨Ѱ���߼�,�Ż�����ͿĤ���ұߣ�\n"
		"\t3.�Ż������Ͱ����ұߣ����˼�����˿���ţ�\n\n";

	strInfo += "---------------Version 1.1.105.210809---------------\n"
		"\t1.�Ż�ʪĤ�Ͱ���߲�����\n"
		"\t2.���ӵ���ջ��߼���\n\n";

	strInfo += "---------------Version 1.1.103.210729---------------\n"
		"\t1.�Ż��㷨�������ƶ�,����������ȣ�\n"
		"\t2.�Ż�����߼���\n"
		"\t3.�޸�һ���׼��߼���\n\n";

	strInfo += "---------------Version 1.1.101.210723---------------\n"
		"\t1.�Ż��㷨�ṹ����Ϊ��ⵥ��ͼ��\n"
		"\t2.���տͻ�Ҫ���޸ĵ���߼����޸ĵ���㷨Ϊ���˫Բ�ģ�\n";

	strInfo += "---------------Version 1.1.96.210717---------------\n"
		"\t1.�Ż��ջ��߼���\n"
		"\t2.�޸���־����ʾ�����⣻\n\n";

	strInfo += "---------------Version 1.1.94.210716---------------\n"
		"\t1.�޸�ͳ������ֵ�vector�쳣��\n"
		"\t2.�Ż������׼��߼���\n\n";

	strInfo += "---------------Version 1.1.92.210713---------------\n"
		"\t1.����һ���׼����ܣ�\n"
		"\t2.�޸�ͣ��λͳ�����ã�\n"
		"\t3.�޸ĵ���߼���\n\n";

	strInfo += "---------------Version 1.1.90.210709---------------\n"
		"\t1.�����㷨����Ѱ��ʧ�ܵı���������\n"
		"\t2.�޸���λ֮�����������Ż���\n"
		"\t3.���ӱջ����������쳣�����ϴ���ƫ�ı�����\n"
		"\t4.�޸�ͣ��λԭ��ѡ��Ƶ���������⣻\n\n";

	strInfo += "---------------Version 1.1.87.210701---------------\n"
		"\t1.����ʪĤ�����滻Ϊ����ȫ�����ݣ�\n"
		"\t2.��λ���ͷ����������\n"
		"\t3.������λ֮��ͨ�ţ�\n"
		"\t4.����λ�䷽У�飻\n\n";

	strInfo += "---------------Version 1.1.84.210627---------------\n"
		"\t1.�Ż������߼���\n"
		"\t2.�޸�ʾ��ͼ��ʾȱ�ݣ�\n"
		"\t3.�Ż�˫ǿ������������NG������ͣ���ж��߼���\n"
		"\t4.�Ż���̬��������������������������̬������\n\n";

	strInfo += "---------------Version 1.1.81.210617---------------\n"
		"\t1.�Ż�����ʱ���޸Ĵ洢覴�ͼƬ��\n"
		"\t2.�޸�PLC���������Ӷ�ȡͿ�������˵��źţ���Ϳ�źţ�ģͷλ�ã�ͣ��λ���ݹ��˵ȣ�\n"
		"\t3.���ӳߴ硢覴ñ�����ͣ���ж��߼���\n\n";

	strInfo += "---------------Version 1.1.71.210519---------------\n"
		"\t1.�޸���ʪĤ�������⣬���������淢�Ͳ���ֵ���ã�\n"
		"\t2.�޸�PLC��ȡ���д���ţ�\n\n";

	strInfo += "---------------Version 1.0.69.210419---------------\n"
		"\t1.�޸�CCD���ݴ洢��λ����/�ļ��洢���⣻\n"
		"\t2.����覴�˫�������������棻\n"
		"\t3.��Ʒ�������棺�¼�K����/�ֶ�����/���Զ��������ܣ�\n\n";

	strInfo += "---------------Version 1.0.69.211217---------------\n"
		"\t1.�䷽�з�����ģ�飻\n"
		"\t2.�������ݱ��ߴ��ٶȣ�\n"
		"\t3.����Ĥ��š�Ʒ�ֺš��汾��д���ϴ���\n\n";

	m_EDIT_UpdateInfo.SetWindowTextA(strInfo);
}

CString CAboutDlg::GetFileVersion(CString strFileName)
{
#pragma comment(lib, "Version.lib ")
	//��ȡ��ǰ�ļ��İ汾��Ϣ
	DWORD dwLen = GetFileVersionInfoSize(strFileName, NULL);
	char *pszAppVersion = new char[dwLen + 1];
	CString strVersion;
	if (pszAppVersion)
	{
		memset(pszAppVersion, 0, sizeof(char)*(dwLen + 1));
		GetFileVersionInfo(strFileName, NULL, dwLen, pszAppVersion);

		UINT nLen(0);
		VS_FIXEDFILEINFO *pFileInfo(NULL);
		VerQueryValue(pszAppVersion, "\\", (LPVOID*)&pFileInfo, &nLen);
		if (pFileInfo)
		{
			//��ȡ�汾��
			strVersion.Format("%d.%d.%d.%d", HIWORD(pFileInfo->dwFileVersionMS),
				LOWORD(pFileInfo->dwFileVersionMS),
				HIWORD(pFileInfo->dwFileVersionLS),
				LOWORD(pFileInfo->dwFileVersionLS));
		}
	}
	delete pszAppVersion;
	return strVersion;
}

CString CAboutDlg::GetModulePath(CString strModuleName)
{
	CString strPath;
	strPath.Format("%s//%s", GetCurrentAppPath(), strModuleName);

	return strPath;
}


void CAboutDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���

	CBCGPDialog::PostNcDestroy();
}


void CAboutDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CBCGPDialog::OnTimer(nIDEvent);
}


BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	return CBCGPDialog::PreTranslateMessage(pMsg);
}
