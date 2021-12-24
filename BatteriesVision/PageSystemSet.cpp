// DlgPageSystemSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageSystemSet.h"
#include "afxdialogex.h"
#include "DlgSimulateSet.h"
#include "DlgIPSet.h"

// CPageSystemSet �Ի���

IMPLEMENT_DYNAMIC(CPageSystemSet, CBCGPDialog)

CPageSystemSet::CPageSystemSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageSystemSet::IDD, pParent)
	, m_COM_nActionMode(0)
	, m_EDIT_strMachineID(_T(""))
	, m_EDIT_strWorkShopID(_T(""))
	, m_EDIT_dSelectMeter(0)
	, m_EDIT_dBatchDelayDistance(0)
	, m_EDIT_dToNextStationMeter(0)
	, m_Combo_nStationSN(0)
	, m_EDT_dScale(0)
	, m_EDIT_dCompenInterval(0)
	, m_EDIT_dStartIgnoreMeter(0)
	, m_CHECK_bUseDataFilte(FALSE)
	, m_EDIT_dCompenLimit(0)
	, m_Combo_nDeviceType(0)
{

	m_EDIT_n2CamDistance = 0;
	m_EDIT_nHistoryNum = 0;
}

CPageSystemSet::~CPageSystemSet()
{
}

void CPageSystemSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESOLUTION, m_LST_Resolution);
	DDX_Control(pDX, IDC_COMBO_HISTORYTYPE, m_COMBO_HistoryType);
	DDX_Text(pDX, IDC_EDIT_2CAM_DISTANCE, m_EDIT_n2CamDistance);
	DDV_MinMaxInt(pDX, m_EDIT_n2CamDistance, 0, 999999999);
	DDX_Text(pDX, IDC_EDIT_HISTORYNUM, m_EDIT_nHistoryNum);
	DDV_MinMaxInt(pDX, m_EDIT_nHistoryNum, 0, 100);
	DDX_CBIndex(pDX, IDC_COMBO_ACTION_MODE, m_COM_nActionMode);
	DDX_Text(pDX, IDC_EDIT_MACHINE_ID, m_EDIT_strMachineID);
	DDX_Text(pDX, IDC_EDIT_WORKSHOP_ID, m_EDIT_strWorkShopID);
	DDX_Text(pDX, IDC_EDIT_SELECT_METER, m_EDIT_dSelectMeter);
	DDX_Text(pDX, IDC_EDIT_BATCH_DELAY_DISTANCE, m_EDIT_dBatchDelayDistance);
	DDX_Text(pDX, IDC_EDIT_TO_NEXT_METER, m_EDIT_dToNextStationMeter);
	DDX_CBIndex(pDX, IDC_COMBO_STATION, m_Combo_nStationSN);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_EDT_dScale);
	DDX_Text(pDX, IDC_EDIT_COMPEN_INTERVAL, m_EDIT_dCompenInterval);
	DDX_Text(pDX, IDC_EDIT_START_IGNORE_METER, m_EDIT_dStartIgnoreMeter);
	DDX_Check(pDX, IDC_CHECK_USE_DATA_FILTER, m_CHECK_bUseDataFilte);
	DDX_Text(pDX, IDC_EDIT_COMP_LIMIT, m_EDIT_dCompenLimit);
	DDX_CBIndex(pDX, IDC_COMBO_DEVICE_TYPE, m_Combo_nDeviceType);
}


BEGIN_MESSAGE_MAP(CPageSystemSet, CBCGPDialog)
	ON_EN_CHANGE(IDC_EDIT_HISTORYNUM, &CPageSystemSet::OnEnChangeEditHistorynum)
	ON_CBN_SELCHANGE(IDC_COMBO_HISTORYTYPE, &CPageSystemSet::OnCbnSelchangeComboHistorytype)
	ON_BN_CLICKED(IDC_CHECK_GRAB_ALIGN, &CPageSystemSet::OnBnClickedCheckGrabAlign)
	ON_EN_CHANGE(IDC_EDIT_2CAM_DISTANCE, &CPageSystemSet::OnEnChangeEdit2camDistance)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_BATCH_COUNT, &CPageSystemSet::OnBnClickedButtonClearBatchCount)
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION_MODE, &CPageSystemSet::OnCbnSelchangeComboActionMode)
	ON_EN_CHANGE(IDC_EDIT_WORKSHOP_ID, &CPageSystemSet::OnEnChangeEditWorkshopId)
	ON_EN_CHANGE(IDC_EDIT_MACHINE_ID, &CPageSystemSet::OnEnChangeEditMachineId)
	ON_BN_CLICKED(IDC_CHECK_DATA_CALCULATE, &CPageSystemSet::OnBnClickedCheckDataCalculate)
	ON_EN_CHANGE(IDC_EDIT_SELECT_METER, &CPageSystemSet::OnEnChangeEditSelectMeter)
	ON_BN_CLICKED(IDC_CHECK_CHANGE_BATCH_AUTO, &CPageSystemSet::OnBnClickedCheckChangeBatchAuto)
	ON_EN_CHANGE(IDC_EDIT_BATCH_DELAY_DISTANCE, &CPageSystemSet::OnEnChangeEditBatchDelayDistance)
	ON_BN_CLICKED(IDC_CHECK_USE_NET, &CPageSystemSet::OnBnClickedCheckUseNet)
	ON_BN_CLICKED(IDC_CHECK_DATA_COMPENSATION_ATUO, &CPageSystemSet::OnBnClickedCheckDataCompensationAtuo)
	ON_EN_CHANGE(IDC_EDIT_TO_NEXT_METER, &CPageSystemSet::OnEnChangeEditToNextMeter)
	ON_CBN_SELCHANGE(IDC_COMBO_STATION, &CPageSystemSet::OnCbnSelchangeComboStation)
	ON_BN_CLICKED(IDC_BUTTON_IP_SET, &CPageSystemSet::OnBnClickedButtonIpSet)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT_SCALE, &CPageSystemSet::OnEnChangeEditScale)
	ON_EN_CHANGE(IDC_EDIT_COMPEN_INTERVAL, &CPageSystemSet::OnEnChangeEditCompenInterval)
	ON_BN_CLICKED(IDC_CHECK_DATA_COMPENSATION_INVERSE, &CPageSystemSet::OnBnClickedCheckDataCompensationInverse)
	ON_EN_CHANGE(IDC_EDIT_START_IGNORE_METER, &CPageSystemSet::OnEnChangeEditStartIgnoreMeter)
	ON_BN_CLICKED(IDC_CHECK_USE_DATA_FILTER, &CPageSystemSet::OnBnClickedCheckUseDataFilter)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCT_CHECK, &CPageSystemSet::OnBnClickedButtonProductCheck)
	ON_BN_CLICKED(IDC_CHECK_USE_PRODUCT_CHECK, &CPageSystemSet::OnBnClickedCheckUseProductCheck)
	ON_EN_CHANGE(IDC_EDIT_COMP_LIMIT, &CPageSystemSet::OnEnChangeEditCompLimit)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_TYPE, &CPageSystemSet::OnCbnSelchangeComboDeviceType)
END_MESSAGE_MAP()


// CPageSystemSet ��Ϣ�������


BOOL CPageSystemSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CPageSystemSet::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageSystemSet::ResetSize()
{
	InitUI();
}

void CPageSystemSet::InitUI()
{
	BOOL bChinese = theGlobal.m_Language.IsChinese();
	CRect rcLst;
	m_LST_Resolution.GetClientRect(rcLst);
	m_LST_Resolution.InsertColumn(0,"��Ŀ",LVCFMT_LEFT,100);
	m_LST_Resolution.InsertColumn(1,"����ֱ���(mm/pixel)",LVCFMT_LEFT,(rcLst.Width()-100)/2);
	m_LST_Resolution.InsertColumn(2,"����ֱ���(mm/pixel)",LVCFMT_LEFT,(rcLst.Width()-100)/2);

	for (int i=0;i<theGlobal.GetResolutionList().size();i++)
	{
		CString strName;
		if (bChinese)
			strName.Format("���%d", i);
		else
			strName.Format("Camera%d", i);
		
		m_LST_Resolution.InsertItem(i,strName);
		strName.Format("%f",theGlobal.GetResolutionList()[i].dColumn);
		m_LST_Resolution.SetItemText(i,1,strName);
		strName.Format("%f",theGlobal.GetResolutionList()[i].dRow);
		m_LST_Resolution.SetItemText(i,2,strName);
	}

	m_COMBO_HistoryType.AddString("������");
	m_COMBO_HistoryType.AddString("ֻ�����");
	m_COMBO_HistoryType.AddString("����ȫ��");
	
	m_EDIT_nHistoryNum = theGlobal.m_tSystemSet.nHistoryCount;
	m_EDIT_n2CamDistance = theGlobal.m_tSystemSet.nCameraDistance;

	((CButton*)GetDlgItem(IDC_CHECK_GRAB_ALIGN))->SetCheck(theGlobal.m_tSystemSet.bGrabAlign);

	((CBCGPButton*)GetDlgItem(IDC_BUTTON_CLEAR_BATCH_COUNT))->SetTooltip("Clear current count and statistics\r\n��ǰ�ļ�����ͳ������");

	theGlobal.m_Language.SetDlgLanguage(GetSafeHwnd(), "IDD_PAGE_SYSTEM_SET");
	m_COMBO_HistoryType.SetCurSel((int)theGlobal.m_tSystemSet.eHistoryType);
	m_COM_nActionMode = theGlobal.m_tSystemSet.nActionMode;

	m_EDIT_strMachineID = theGlobal.m_tSystemSet.strDeviceID;
	m_EDIT_strWorkShopID = theGlobal.m_tSystemSet.strWorkshopName;
	m_Combo_nStationSN = theGlobal.m_tSystemSet.eSN - 1;
	m_Combo_nDeviceType = theGlobal.m_tSystemSet.eDevType;

	((CButton*)GetDlgItem(IDC_CHECK_DATA_CALCULATE))->SetCheck(theGlobal.m_DataCalculate.m_tParam.bUse);
	m_EDIT_dSelectMeter = theGlobal.m_DataCalculate.m_tParam.dSelectMeter;

	((CButton*)GetDlgItem(IDC_CHECK_USE_NET))->SetCheck(theGlobal.m_Upload.m_tParam.bUseDataUpload);

	((CButton*)GetDlgItem(IDC_CHECK_CHANGE_BATCH_AUTO))->SetCheck(theGlobal.m_tSystemSet.bUseChangeBatchAuto);
	m_EDIT_dBatchDelayDistance = theGlobal.m_tSystemSet.dChangeBatchDelayMeter;

	((CButton*)GetDlgItem(IDC_CHECK_DATA_COMPENSATION_ATUO))->SetCheck(theGlobal.m_Upload.m_tParam.bUseCompensationAuto);
	((CButton*)GetDlgItem(IDC_CHECK_DATA_COMPENSATION_INVERSE))->SetCheck(theGlobal.m_Upload.m_tParam.bCompensationDataInverse);
	m_EDIT_dToNextStationMeter = theGlobal.m_Upload.m_tParam.dToNextStationMeter;
	m_EDIT_dCompenInterval = theGlobal.m_Upload.m_tParam.dCompensationInvertal;
	m_EDT_dScale = theGlobal.m_Upload.m_tParam.dScale;
	m_EDIT_dStartIgnoreMeter = theGlobal.m_Upload.m_tParam.dStartIgnoreMeter;

	m_CHECK_bUseDataFilte = theGlobal.m_DataCalculate.m_tParam.bUseStopPosDataFiltering;

	((CButton*)GetDlgItem(IDC_CHECK_USE_PRODUCT_CHECK))->SetCheck(theGlobal.m_tSystemSet.bCheckProductInfoByChangeBatch);

	m_EDIT_dCompenLimit = theGlobal.m_ProductTool.m_dAutoCompenLimit;

	if (theGlobal.m_tSystemSet.eSN != CGlobal::SN_Head)
	{
		((CButton*)GetDlgItem(IDC_BUTTON_PRODUCT_CHECK))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_USE_PRODUCT_CHECK))->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}


BOOL CPageSystemSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//����ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//����ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageSystemSet::OnEnChangeEditHistorynum()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_tSystemSet.nHistoryCount, m_EDIT_nHistoryNum, "��ʷͼ�񻺴�����");
	theGlobal.m_tSystemSet.nHistoryCount = m_EDIT_nHistoryNum;
	theGlobal.WriteParam();
}


void CPageSystemSet::OnCbnSelchangeComboHistorytype()
{
	theGlobal.m_tSystemSet.eHistoryType = (CGlobal::eHistoryType)m_COMBO_HistoryType.GetCurSel();
	theGlobal.WriteParam();
}

void CPageSystemSet::OnBnClickedCheckGrabAlign()
{
	theGlobal.m_Log.Operate(theGlobal.m_tSystemSet.bGrabAlign, ((CButton*)GetDlgItem(IDC_CHECK_GRAB_ALIGN))->GetCheck(), "ʹ���㷨��λ");
	theGlobal.m_tSystemSet.bGrabAlign = ((CButton*)GetDlgItem(IDC_CHECK_GRAB_ALIGN))->GetCheck();
	theGlobal.WriteParam();
}

void CPageSystemSet::OnEnChangeEdit2camDistance()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_tSystemSet.nCameraDistance, m_EDIT_n2CamDistance, "�㷨��λ����(��");
	theGlobal.m_tSystemSet.nCameraDistance = m_EDIT_n2CamDistance;
	theGlobal.WriteParam();
}

void CPageSystemSet::OnBnClickedButtonClearBatchCount()
{
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("�Ƿ���յ�ǰ�����?", "Whether to clear current roll count?", ""))return;

	theGlobal.m_Counter.WriteParam();
	theGlobal.m_pMainDlg->PostMessageA(WM_REFRESH_DISP);
}


BOOL CPageSystemSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageSystemSet::OnCbnSelchangeComboActionMode()
{
	UpdateData(TRUE);
	theGlobal.m_tSystemSet.nActionMode = m_COM_nActionMode;
	theGlobal.WriteParam();
	if (SimulateMode == theGlobal.m_tSystemSet.nActionMode)
	{
		CDlgSimulateSet dlg;
		dlg.DoModal();
	}
	if (m_COM_nActionMode == GRRMode)
	{
		theGlobal.m_Storage.SetGrrNewStart();
	}
}


void CPageSystemSet::OnEnChangeEditWorkshopId()
{
	UpdateData(TRUE);
	theGlobal.m_tSystemSet.strWorkshopName = m_EDIT_strWorkShopID;
	theGlobal.WriteParam();
}


void CPageSystemSet::OnEnChangeEditMachineId()
{
	UpdateData(TRUE);
	theGlobal.m_tSystemSet.strDeviceID = m_EDIT_strMachineID;
	theGlobal.WriteParam();
}

void CPageSystemSet::OnCbnSelchangeComboDeviceType()
{
	UpdateData(TRUE);
	theGlobal.m_tSystemSet.eDevType = CGlobal::eDeviceType(m_Combo_nDeviceType);
	theGlobal.WriteParam();
}

void CPageSystemSet::OnBnClickedCheckDataCalculate()
{
	theGlobal.m_Log.Operate(theGlobal.m_DataCalculate.m_tParam.bUse, ((CButton*)GetDlgItem(IDC_CHECK_DATA_CALCULATE))->GetCheck(), "ʹ��CPK�����ݼ���");
	theGlobal.m_DataCalculate.m_tParam.bUse = ((CButton*)GetDlgItem(IDC_CHECK_DATA_CALCULATE))->GetCheck();
	theGlobal.m_DataCalculate.WriteParam();
}


void CPageSystemSet::OnEnChangeEditSelectMeter()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_DataCalculate.m_tParam.dSelectMeter, m_EDIT_dSelectMeter, "CPK���ݼ���ȡֵ��Χ(m)");
	theGlobal.m_DataCalculate.m_tParam.dSelectMeter = m_EDIT_dSelectMeter;
	theGlobal.m_DataCalculate.WriteParam();
}


void CPageSystemSet::OnBnClickedCheckChangeBatchAuto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	theGlobal.m_Log.Operate(theGlobal.m_tSystemSet.bUseChangeBatchAuto, ((CButton*)GetDlgItem(IDC_CHECK_CHANGE_BATCH_AUTO))->GetCheck(), "ʹ���Զ�����");
	theGlobal.m_tSystemSet.bUseChangeBatchAuto = ((CButton*)GetDlgItem(IDC_CHECK_CHANGE_BATCH_AUTO))->GetCheck();
	theGlobal.WriteParam();
}


void CPageSystemSet::OnEnChangeEditBatchDelayDistance()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_tSystemSet.dChangeBatchDelayMeter, m_EDIT_dBatchDelayDistance, "�����ӳ�����(m)");
	theGlobal.m_tSystemSet.dChangeBatchDelayMeter = m_EDIT_dBatchDelayDistance;
	theGlobal.WriteParam();
}


void CPageSystemSet::OnBnClickedCheckUseNet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	theGlobal.m_Log.Operate(theGlobal.m_Upload.m_tParam.bUseDataUpload, ((CButton*)GetDlgItem(IDC_CHECK_USE_NET))->GetCheck(), "ʹ�����罻��");
	theGlobal.m_Upload.m_tParam.bUseDataUpload = ((CButton*)GetDlgItem(IDC_CHECK_USE_NET))->GetCheck();
	theGlobal.m_Upload.WriteParam();
}


void CPageSystemSet::OnBnClickedCheckDataCompensationAtuo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	theGlobal.m_Log.Operate(theGlobal.m_Upload.m_tParam.bUseCompensationAuto, ((CButton*)GetDlgItem(IDC_CHECK_DATA_COMPENSATION_ATUO))->GetCheck(), "ʹ�ö�̬����");
	theGlobal.m_Upload.m_tParam.bUseCompensationAuto = ((CButton*)GetDlgItem(IDC_CHECK_DATA_COMPENSATION_ATUO))->GetCheck();
	theGlobal.m_Upload.WriteParam();
}


void CPageSystemSet::OnEnChangeEditToNextMeter()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Upload.m_tParam.dToNextStationMeter, m_EDIT_dToNextStationMeter, "����һ����λ�ľ���(m)");
	theGlobal.m_Upload.m_tParam.dToNextStationMeter = m_EDIT_dToNextStationMeter;
	theGlobal.m_Upload.WriteParam();
}


void CPageSystemSet::OnCbnSelchangeComboStation()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_tSystemSet.eSN = CGlobal::eStationSN(m_Combo_nStationSN + 1);
	theGlobal.WriteParam();
}


void CPageSystemSet::OnBnClickedButtonIpSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDlgIPSet dlg;
	dlg.DoModal();
}

void CPageSystemSet::EnableWidgetsWithPermenssion()
{
	BOOL bEnable = TRUE;
	bEnable = theGlobal.m_Permission.CheckPermission(PM_System);
	HWND  hwndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);  //�г����пؼ� 
	CWnd* pWnd = CWnd::FromHandle(GetSafeHwnd());
	int woc;
	CRect Rect;
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//ȡ��ID
		CString strClassName;
		GetClassName(hwndChild, (LPSTR)(LPCSTR)strClassName, MAX_PATH);
		if (strClassName.Find("Edit") >= 0 
			|| strClassName.Find("Button") >= 0 
			|| strClassName.Find("ListView") >= 0
			|| strClassName.Find("ComboBox") >= 0)
		{
			pWnd->GetDlgItem(woc)->EnableWindow(bEnable);
		}
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
}

void CPageSystemSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CBCGPDialog::OnPaint()
	
	ShowMesageJudgeByPowerLimit();
}

void CPageSystemSet::ShowMesageJudgeByPowerLimit()
{
	try
	{
		if (theGlobal.m_Permission.CheckPermission(PM_System))
		{
			//�ֱ���
			GetDlgItem(IDC_LIST_RESOLUTION)->EnableWindow(TRUE);
			//����״̬
			GetDlgItem(IDC_COMBO_ACTION_MODE)->EnableWindow(TRUE);
			//ϵͳ��Ϣ
			GetDlgItem(IDC_EDIT_WORKSHOP_ID)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_MACHINE_ID)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_STATION)->EnableWindow(TRUE);
			//��ʷ�ؿ�
			GetDlgItem(IDC_EDIT_HISTORYNUM)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_HISTORYTYPE)->EnableWindow(TRUE);
			//��ͼ��ƴͼ
			GetDlgItem(IDC_CHECK_GRAB_ALIGN)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_2CAM_DISTANCE)->EnableWindow(TRUE);
			//��������
			GetDlgItem(IDC_CHECK_USE_NET)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_CHANGE_BATCH_AUTO)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_DATA_COMPENSATION_ATUO)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_IP_SET)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_BATCH_DELAY_DISTANCE)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_TO_NEXT_METER)->EnableWindow(TRUE);
			//���ݼ���
			GetDlgItem(IDC_CHECK_DATA_CALCULATE)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SELECT_METER)->EnableWindow(TRUE);
			//ͳ�Ƽ���
			GetDlgItem(IDC_BUTTON_CLEAR_BATCH_COUNT)->EnableWindow(TRUE);
		}
		else
		{
			//�ֱ���
			GetDlgItem(IDC_LIST_RESOLUTION)->EnableWindow(FALSE);
			//����״̬
			GetDlgItem(IDC_COMBO_ACTION_MODE)->EnableWindow(FALSE);
			//ϵͳ��Ϣ
			GetDlgItem(IDC_EDIT_WORKSHOP_ID)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_MACHINE_ID)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_STATION)->EnableWindow(FALSE);
			//��ʷ�ؿ�
			GetDlgItem(IDC_EDIT_HISTORYNUM)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_HISTORYTYPE)->EnableWindow(FALSE);
			//��ͼ��ƴͼ
			GetDlgItem(IDC_CHECK_GRAB_ALIGN)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_2CAM_DISTANCE)->EnableWindow(FALSE);
			//��������
			GetDlgItem(IDC_CHECK_USE_NET)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_CHANGE_BATCH_AUTO)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_DATA_COMPENSATION_ATUO)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_IP_SET)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_BATCH_DELAY_DISTANCE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_TO_NEXT_METER)->EnableWindow(FALSE);
			//���ݼ���
			GetDlgItem(IDC_CHECK_DATA_CALCULATE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_SELECT_METER)->EnableWindow(FALSE);
			//ͳ�Ƽ���
			GetDlgItem(IDC_BUTTON_CLEAR_BATCH_COUNT)->EnableWindow(FALSE);
		}
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}
}

void CPageSystemSet::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBCGPDialog::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
	//EnableWidgetsWithPermenssion();
}


void CPageSystemSet::OnEnChangeEditScale()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Upload.m_tParam.dScale, max(0.1, m_EDT_dScale), "��̬����ϵ��");
	theGlobal.m_Upload.m_tParam.dScale = max(0.1,m_EDT_dScale);
	theGlobal.m_Upload.WriteParam();
}


void CPageSystemSet::OnEnChangeEditCompenInterval()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Upload.m_tParam.dCompensationInvertal, m_EDIT_dCompenInterval, "��̬�������(m)");
	theGlobal.m_Upload.m_tParam.dCompensationInvertal = m_EDIT_dCompenInterval;
}


void CPageSystemSet::OnBnClickedCheckDataCompensationInverse()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Upload.m_tParam.bCompensationDataInverse, ((CButton*)GetDlgItem(IDC_CHECK_DATA_COMPENSATION_INVERSE))->GetCheck(), "ʹ�����ݷ�ת");
	theGlobal.m_Upload.m_tParam.bCompensationDataInverse = ((CButton*)GetDlgItem(IDC_CHECK_DATA_COMPENSATION_INVERSE))->GetCheck();
	theGlobal.m_Upload.WriteParam();
}


void CPageSystemSet::OnEnChangeEditStartIgnoreMeter()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Upload.m_tParam.dStartIgnoreMeter, m_EDIT_dStartIgnoreMeter, "��̬����������������(m)");
	theGlobal.m_Upload.m_tParam.dStartIgnoreMeter = m_EDIT_dStartIgnoreMeter;
	theGlobal.m_Upload.WriteParam();
}


void CPageSystemSet::OnBnClickedCheckUseDataFilter()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_DataCalculate.m_tParam.bUseStopPosDataFiltering, m_CHECK_bUseDataFilte, "ͣ��λ���ݹ��˿���");
	theGlobal.m_DataCalculate.m_tParam.bUseStopPosDataFiltering = m_CHECK_bUseDataFilte;
	theGlobal.m_DataCalculate.WriteParam();
}


void CPageSystemSet::OnBnClickedButtonProductCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	theGlobal.SetTipInfo(TIP_CheckProduct);
}


void CPageSystemSet::OnBnClickedCheckUseProductCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_tSystemSet.bCheckProductInfoByChangeBatch, ((CButton*)GetDlgItem(IDC_CHECK_USE_PRODUCT_CHECK))->GetCheck(), "����ͬ������λ�䷽����");
	theGlobal.m_tSystemSet.bCheckProductInfoByChangeBatch = ((CButton*)GetDlgItem(IDC_CHECK_USE_PRODUCT_CHECK))->GetCheck();
	theGlobal.WriteParam();
}


void CPageSystemSet::OnEnChangeEditCompLimit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_ProductTool.m_dAutoCompenLimit, m_EDIT_dCompenLimit, "��ʪĤ����������(mm)");
	theGlobal.m_ProductTool.m_dAutoCompenLimit = m_EDIT_dCompenLimit;
	theGlobal.m_ProductTool.WriteParam();
}
