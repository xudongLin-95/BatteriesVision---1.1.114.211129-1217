// PageStorageSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageStorageSet.h"
#include "afxdialogex.h"
#include "DlgViewLog.h"


// CPageStorageSet 对话框

IMPLEMENT_DYNAMIC(CPageStorageSet, CBCGPDialog)

CPageStorageSet::CPageStorageSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageStorageSet::IDD, pParent)
{
	m_EDIT_nCacheSaveDays = 0;
	m_EDIT_nMinRemainSpace = 0;
	m_EDIT_nStartupSaveTime = 0;
	m_RADIO_nDataType = 0;
	m_RADIO_nImageType = 0;
	m_RADIO_nNGImageType = 0;
	m_RADIO_nOKImageType = 0;
	m_EDIT_strSavePath = _T("");
}

CPageStorageSet::~CPageStorageSet()
{
}

void CPageStorageSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CACHE_SAVEDAYS, m_EDIT_nCacheSaveDays);
	DDV_MinMaxInt(pDX, m_EDIT_nCacheSaveDays, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_REMAIN_SPACE_MIN, m_EDIT_nMinRemainSpace);
	DDV_MinMaxInt(pDX, m_EDIT_nMinRemainSpace, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_STARTUP_SAVETIME, m_EDIT_nStartupSaveTime);
	DDV_MinMaxInt(pDX, m_EDIT_nStartupSaveTime, 0, 99999999);
	DDX_Radio(pDX, IDC_RADIO_DATA0, m_RADIO_nDataType);
	DDX_Radio(pDX, IDC_RADIO_IMAGE0, m_RADIO_nImageType);
	DDX_Radio(pDX, IDC_RADIO_NGIMAGE0, m_RADIO_nNGImageType);
	DDX_Radio(pDX, IDC_RADIO_OKIMAGE0, m_RADIO_nOKImageType);
	DDX_Control(pDX, IDC_EDIT_SAVE_PATH, m_EDIT_SavePath);
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_EDIT_strSavePath);
}


BEGIN_MESSAGE_MAP(CPageStorageSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_WATCHLOG, &CPageStorageSet::OnBnClickedButtonWatchlog)
	ON_BN_CLICKED(IDC_RADIO_IMAGE0, &CPageStorageSet::OnBnClickedRadioImage0)
	ON_BN_CLICKED(IDC_RADIO_IMAGE1, &CPageStorageSet::OnBnClickedRadioImage1)
	ON_BN_CLICKED(IDC_RADIO_IMAGE2, &CPageStorageSet::OnBnClickedRadioImage2)
	ON_BN_CLICKED(IDC_RADIO_OKIMAGE0, &CPageStorageSet::OnBnClickedRadioOkimage0)
	ON_BN_CLICKED(IDC_RADIO_OKIMAGE1, &CPageStorageSet::OnBnClickedRadioOkimage1)
	ON_BN_CLICKED(IDC_RADIO_NGIMAGE0, &CPageStorageSet::OnBnClickedRadioNgimage0)
	ON_BN_CLICKED(IDC_RADIO_NGIMAGE1, &CPageStorageSet::OnBnClickedRadioNgimage1)
	ON_BN_CLICKED(IDC_RADIO_NGIMAGE2, &CPageStorageSet::OnBnClickedRadioNgimage2)
	ON_BN_CLICKED(IDC_CHECK_SAVE_ERRORTAG, &CPageStorageSet::OnBnClickedCheckSaveErrortag)
	ON_BN_CLICKED(IDC_CHECK_STARTUP_SAVETIME, &CPageStorageSet::OnBnClickedCheckStartupSavetime)
	ON_BN_CLICKED(IDC_CHECK_DELETE_OLDCACHES, &CPageStorageSet::OnBnClickedCheckDeleteOldcaches)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_FORMAT_OK, &CPageStorageSet::OnCbnSelchangeComboImageFormatOk)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_FORMAT_NG, &CPageStorageSet::OnCbnSelchangeComboImageFormatNg)
	ON_EN_CHANGE(IDC_EDIT_REMAIN_SPACE_MIN, &CPageStorageSet::OnEnChangeEditRemainSpaceMin)
	ON_EN_CHANGE(IDC_EDIT_SAVE_PATH, &CPageStorageSet::OnEnChangeEditSavePath)
	ON_EN_CHANGE(IDC_EDIT_STARTUP_SAVETIME, &CPageStorageSet::OnEnChangeEditStartupSavetime)
	ON_EN_CHANGE(IDC_EDIT_CACHE_SAVEDAYS, &CPageStorageSet::OnEnChangeEditCacheSavedays)
	ON_CBN_SELCHANGE(IDC_COMBO_LOG_TYPE, &CPageStorageSet::OnCbnSelchangeComboLogType)
	ON_CBN_SELCHANGE(IDC_COMBO_LOG_DISPTYPE, &CPageStorageSet::OnCbnSelchangeComboLogDisptype)
	ON_BN_CLICKED(IDC_RADIO_DATA0, &CPageStorageSet::OnBnClickedRadioData0)
	ON_BN_CLICKED(IDC_RADIO_DATA1, &CPageStorageSet::OnBnClickedRadioData1)
	ON_BN_CLICKED(IDC_RADIO_DATA2, &CPageStorageSet::OnBnClickedRadioData2)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_SETLOG, &CPageStorageSet::OnBnClickedButtonSetlog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPageStorageSet 消息处理程序


BOOL CPageStorageSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageStorageSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageStorageSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageStorageSet::ResetSize()
{
	InitUI();
}

void CPageStorageSet::InitUI()
{
	m_RADIO_nImageType = theGlobal.m_Storage.GetImageSaveType();
	m_RADIO_nDataType = theGlobal.m_Storage.GetRecordSaveType();
	m_RADIO_nNGImageType = theGlobal.m_Storage.GetNGImageSaveType();
	m_RADIO_nOKImageType = theGlobal.m_Storage.GetOKImageSaveType();
	((CButton*)GetDlgItem(IDC_CHECK_SAVE_ERRORTAG))->SetCheck(theGlobal.m_Storage.GetSaveErrorTag());
	((CButton*)GetDlgItem(IDC_CHECK_STARTUP_SAVETIME))->SetCheck(theGlobal.m_Storage.GetUseProgramStartSaveTime());
	((CButton*)GetDlgItem(IDC_CHECK_DELETE_OLDCACHES))->SetCheck(theGlobal.m_Storage.GetDeleteOldCaches());

	m_EDIT_nCacheSaveDays = theGlobal.m_Storage.GetCacheSaveDays();
	m_EDIT_nMinRemainSpace = theGlobal.m_Storage.GetMinRemainStorageGB();
	m_EDIT_nStartupSaveTime = theGlobal.m_Storage.GetProgramStartSaveSeconds();

	m_EDIT_SavePath.EnableFolderBrowseButton("Data save path/数据保存路径");
	m_EDIT_strSavePath = theGlobal.m_Storage.GetMapFolderPath();
	
	UpdateData(FALSE);
	EnableRadio();
	EnableControl();

	((CBCGPButton*)GetDlgItem(IDC_BUTTON_WATCHLOG))->SetTooltip("View saved logs\r\n查看已经保存的日志");

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_STORAGE_SET");

	((CComboBox*)GetDlgItem(IDC_COMBO_IMAGE_FORMAT_OK))->SetCurSel(theGlobal.m_Storage.GetOKImageSaveFormat());
	((CComboBox*)GetDlgItem(IDC_COMBO_IMAGE_FORMAT_NG))->SetCurSel(theGlobal.m_Storage.GetNGImageSaveFormat());

	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->AddString("无");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->AddString("运行");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->AddString("运行,用户报警");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->AddString("运行,用户报警,操作");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->AddString("运行,用户报警,操作,通信");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->AddString("运行,用户报警,操作,通信,异常");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->AddString("运行,用户报警,操作,通信,异常,调试");

	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->AddString("无");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->AddString("运行");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->AddString("运行,用户报警");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->AddString("运行,用户报警,操作");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->AddString("运行,用户报警,操作,通信");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->AddString("运行,用户报警,操作,通信,异常");
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->AddString("运行,用户报警,操作,通信,异常,调试");

	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->SetCurSel(theGlobal.m_Log.GetLogLevel());
	((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->SetCurSel(theGlobal.m_Log.GetListDispLevel());
}

void CPageStorageSet::OnBnClickedButtonWatchlog()
{
	/*CDlgViewLog dlg;
	dlg.DoModal();*/

	theGlobal.m_Log.ReviewLog();
}


void CPageStorageSet::OnBnClickedRadioImage0()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetImageSaveType(m_RADIO_nImageType);
	theGlobal.m_Storage.WriteParam();
	EnableRadio();
}


void CPageStorageSet::OnBnClickedRadioImage1()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetImageSaveType(m_RADIO_nImageType);
	theGlobal.m_Storage.WriteParam();
	EnableRadio();
}


void CPageStorageSet::OnBnClickedRadioImage2()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetImageSaveType(m_RADIO_nImageType);
	theGlobal.m_Storage.WriteParam();
	EnableRadio();
}

void CPageStorageSet::EnableRadio()
{
	if (0==m_RADIO_nImageType)
	{
		GetDlgItem(IDC_RADIO_OKIMAGE0)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OKIMAGE1)->EnableWindow(FALSE);


		GetDlgItem(IDC_RADIO_NGIMAGE0)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_NGIMAGE1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_NGIMAGE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SAVE_ERRORTAG)->EnableWindow(FALSE);

		GetDlgItem(IDC_COMBO_IMAGE_FORMAT_OK)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_IMAGE_FORMAT_NG)->EnableWindow(FALSE);
	}
	else if (1==m_RADIO_nImageType)
	{
		GetDlgItem(IDC_RADIO_OKIMAGE0)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OKIMAGE1)->EnableWindow(FALSE);


		GetDlgItem(IDC_RADIO_NGIMAGE0)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_NGIMAGE1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_NGIMAGE2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SAVE_ERRORTAG)->EnableWindow(TRUE);

		GetDlgItem(IDC_COMBO_IMAGE_FORMAT_OK)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_IMAGE_FORMAT_NG)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_RADIO_OKIMAGE0)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OKIMAGE1)->EnableWindow(TRUE);


		GetDlgItem(IDC_RADIO_NGIMAGE0)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_NGIMAGE1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_NGIMAGE2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SAVE_ERRORTAG)->EnableWindow(TRUE);

		GetDlgItem(IDC_COMBO_IMAGE_FORMAT_OK)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_IMAGE_FORMAT_NG)->EnableWindow(TRUE);
	}
}

void CPageStorageSet::EnableControl()
{
	GetDlgItem(IDC_EDIT_STARTUP_SAVETIME)->EnableWindow(((CButton*)GetDlgItem(IDC_CHECK_STARTUP_SAVETIME))->GetCheck());
	GetDlgItem(IDC_EDIT_CACHE_SAVEDAYS)->EnableWindow(((CButton*)GetDlgItem(IDC_CHECK_DELETE_OLDCACHES))->GetCheck());
}

void CPageStorageSet::OnBnClickedRadioOkimage0()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.WriteParam();
	theGlobal.m_Storage.SetOKImageSaveType(m_RADIO_nOKImageType);
}


void CPageStorageSet::OnBnClickedRadioOkimage1()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.WriteParam();
	theGlobal.m_Storage.SetOKImageSaveType(m_RADIO_nOKImageType);
}


void CPageStorageSet::OnBnClickedRadioNgimage0()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.WriteParam();
	theGlobal.m_Storage.SetNGImageSaveType(m_RADIO_nNGImageType);
}


void CPageStorageSet::OnBnClickedRadioNgimage1()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.WriteParam();
	theGlobal.m_Storage.SetNGImageSaveType(m_RADIO_nNGImageType);
}


void CPageStorageSet::OnBnClickedRadioNgimage2()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.WriteParam();
	theGlobal.m_Storage.SetNGImageSaveType(m_RADIO_nNGImageType);
}


void CPageStorageSet::OnBnClickedCheckSaveErrortag()
{
	theGlobal.m_Storage.SetSaveErrorTag(((CButton*)GetDlgItem(IDC_CHECK_SAVE_ERRORTAG))->GetCheck());
	theGlobal.m_Storage.WriteParam();
}


void CPageStorageSet::OnBnClickedCheckStartupSavetime()
{
	theGlobal.m_Storage.SetUseProgramStartSaveTime(((CButton*)GetDlgItem(IDC_CHECK_STARTUP_SAVETIME))->GetCheck());
	theGlobal.m_Storage.WriteParam();
	EnableControl();
}


void CPageStorageSet::OnBnClickedCheckDeleteOldcaches()
{
	theGlobal.m_Storage.SetDeleteOldCaches(((CButton*)GetDlgItem(IDC_CHECK_DELETE_OLDCACHES))->GetCheck());
	theGlobal.m_Storage.WriteParam();
	EnableControl();
}


void CPageStorageSet::OnCbnSelchangeComboImageFormatOk()
{
	theGlobal.m_Storage.SetOKImageSaveFormat(((CComboBox*)GetDlgItem(IDC_COMBO_IMAGE_FORMAT_OK))->GetCurSel());
	theGlobal.m_Storage.WriteParam();
}


void CPageStorageSet::OnCbnSelchangeComboImageFormatNg()
{
	theGlobal.m_Storage.SetNGImageSaveFormat(((CComboBox*)GetDlgItem(IDC_COMBO_IMAGE_FORMAT_NG))->GetCurSel());
	theGlobal.m_Storage.WriteParam();
}


void CPageStorageSet::OnEnChangeEditRemainSpaceMin()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetMinRemainStorageGB(m_EDIT_nMinRemainSpace);
	theGlobal.m_Storage.WriteParam();
}

void CPageStorageSet::OnEnChangeEditSavePath()
{
	UpdateData(TRUE);
	/*if(GetFileAttributes(m_EDIT_strSavePath) == INVALID_FILE_ATTRIBUTES)
	{
		BCGPMessageBox("Invalid save folder path, please reenter.\r\n保存路径无效，请重新输入");
		m_EDIT_strSavePath = theGlobal.m_Storage.GetMapFolderPath();
		UpdateData(FALSE);
	}*/
	theGlobal.m_Storage.SetMapFolderPath(m_EDIT_strSavePath);
	theGlobal.m_Storage.WriteParam();
}


void CPageStorageSet::OnEnChangeEditStartupSavetime()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetProgramStartSaveSeconds(m_EDIT_nStartupSaveTime);
	theGlobal.m_Storage.WriteParam();
}

void CPageStorageSet::OnEnChangeEditCacheSavedays()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetCacheSaveDays(m_EDIT_nCacheSaveDays);
	theGlobal.m_Storage.WriteParam();
}

void CPageStorageSet::OnBnClickedRadioData0()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetRecordSaveType(m_RADIO_nDataType);
	theGlobal.m_Storage.WriteParam();
}


void CPageStorageSet::OnBnClickedRadioData1()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetRecordSaveType(m_RADIO_nDataType);
	theGlobal.m_Storage.WriteParam();
}


void CPageStorageSet::OnBnClickedRadioData2()
{
	UpdateData(TRUE);
	theGlobal.m_Storage.SetRecordSaveType(m_RADIO_nDataType);
	theGlobal.m_Storage.WriteParam();
}

void CPageStorageSet::OnCbnSelchangeComboLogType()
{
	theGlobal.m_Log.SetLogLevel(((CComboBox*)GetDlgItem(IDC_COMBO_LOG_TYPE))->GetCurSel());
	theGlobal.m_Log.WriteParam();
}


void CPageStorageSet::OnCbnSelchangeComboLogDisptype()
{
	theGlobal.m_Log.SetListDispLevel(((CComboBox*)GetDlgItem(IDC_COMBO_LOG_DISPTYPE))->GetCurSel());
	theGlobal.m_Log.WriteParam();
}

BOOL CPageStorageSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageStorageSet::OnBnClickedButtonSetlog()
{
	// TODO: 在此添加控件通知处理程序代码
	theGlobal.m_Log.DispSetLogDialog();
}


void CPageStorageSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBCGPDialog::OnPaint()

	ShowMesageJudgeByPowerLimit();
}

void CPageStorageSet::ShowMesageJudgeByPowerLimit()
{
	try
	{
		if (theGlobal.m_Permission.CheckPermission(PM_Storage))
		{
			//存储设置
			GetDlgItem(IDC_RADIO_IMAGE0)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_IMAGE1)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_IMAGE2)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_OKIMAGE0)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_OKIMAGE1)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_IMAGE_FORMAT_OK)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_NGIMAGE0)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_NGIMAGE1)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_NGIMAGE2)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_IMAGE_FORMAT_NG)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_SAVE_ERRORTAG)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_DATA0)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_DATA1)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_DATA2)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_REMAIN_SPACE_MIN)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SAVE_PATH)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_STARTUP_SAVETIME)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_DELETE_OLDCACHES)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_STARTUP_SAVETIME)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_CACHE_SAVEDAYS)->EnableWindow(TRUE);

			//日志设置
			GetDlgItem(IDC_COMBO_LOG_TYPE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SETLOG)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_LOG_DISPTYPE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_WATCHLOG)->EnableWindow(TRUE);
		}
		else
		{
			//存储设置
			GetDlgItem(IDC_RADIO_IMAGE0)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_IMAGE1)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_IMAGE2)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_OKIMAGE0)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_OKIMAGE1)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_IMAGE_FORMAT_OK)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_NGIMAGE0)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_NGIMAGE1)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_NGIMAGE2)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_IMAGE_FORMAT_NG)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_SAVE_ERRORTAG)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_DATA0)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_DATA1)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_DATA2)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_REMAIN_SPACE_MIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_SAVE_PATH)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_STARTUP_SAVETIME)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_DELETE_OLDCACHES)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_STARTUP_SAVETIME)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CACHE_SAVEDAYS)->EnableWindow(FALSE);

			//日志设置
			GetDlgItem(IDC_COMBO_LOG_TYPE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SETLOG)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_LOG_DISPTYPE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_WATCHLOG)->EnableWindow(FALSE);
		}
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}
}