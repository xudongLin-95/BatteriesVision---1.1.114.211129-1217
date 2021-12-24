// DlgIOSetting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SCIIO.h"
#include "DlgIOSetting.h"


// CDlgIOSetting �Ի���

IMPLEMENT_DYNAMIC(CDlgIOSetting, CBCGPDialog)

CDlgIOSetting::CDlgIOSetting(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgIOSetting::IDD, pParent)
	, m_nContinuousErrorAlarmsIO(0)
	, m_nReadyIO(0)
	, m_nIOCardSN(0)
	, m_nRejectType(0)
	, m_nPulseWidth(0)
	, m_EDT_DabiaoDelay(0)

	, m_COM_nLabelType(0)
	, m_EDT_nATotalDistannce(0)
	, m_EDT_nBTotalDistannce(0)
	, m_EDT_AFrameNum(0)
	, m_EDT_BFrameNum(0)
	, m_EDT_nImageHeight(0)
{
	m_ftButton.CreatePointFont(100,"Microsoft YaHei");
	m_ftEdit.CreatePointFont(90,"Microsoft YaHei");
	m_nContinuesErrorCountMax = 0;
	m_dMarkerLengh = 0.0;
	m_nRejectDelay = 0;
	m_nRejectDelay1 = 0;
	//  m_COM_nFenPinMoShi = 0;
	m_EDT_nTiFeiPulse = 80;
	m_EDT_nBuChangHangShu = 800;
}

CDlgIOSetting::~CDlgIOSetting()
{
	m_ftButton.DeleteObject();
	m_ftEdit.DeleteObject();
}

void CDlgIOSetting::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONTINUOUSERRORALARMS, m_nContinuousErrorAlarmsIO);
	DDV_MinMaxInt(pDX, m_nContinuousErrorAlarmsIO, 0, 7);
	DDX_Text(pDX, IDC_EDIT_READY, m_nReadyIO);
	DDV_MinMaxInt(pDX, m_nReadyIO, 0, 7);
	DDX_Text(pDX, IDC_EDIT_IOCARDSN, m_nIOCardSN);
	DDV_MinMaxInt(pDX, m_nIOCardSN, 0, 4);
	DDX_Radio(pDX, IDC_RADIO_REJECT, m_nRejectType);
	DDX_Text(pDX, IDC_EDIT_PULSEWIDTH, m_nPulseWidth);
	DDX_Text(pDX, IDC_EDIT_CONTINUOUSERRORCOUNTMAX, m_nContinuesErrorCountMax);
	DDX_Text(pDX, IDC_EDIT_MARKER_LENGTH, m_dMarkerLengh);
	DDX_Text(pDX, IDC_EDIT_DELAY_REJECT, m_nRejectDelay);
	DDX_Text(pDX, IDC_EDT_DABIAODELAY, m_EDT_DabiaoDelay);
	DDX_Text(pDX, IDC_EDIT_DELAY_REJECT1, m_nRejectDelay1);
	//  DDX_Control(pDX, IDC_COM_FENPINMOSHI, m_COM_CFenPinMoShi);
	//  DDX_CBIndex(pDX, IDC_COM_FENPINMOSHI, m_COM_nFenPinMoShi);
	DDX_Text(pDX, IDC_EDT_TIFEIPULSE, m_EDT_nTiFeiPulse);
	DDX_Text(pDX, IDC_EDT_BUCHANGHANGSHU, m_EDT_nBuChangHangShu);
	DDX_Control(pDX, IDC_COM_LABELTYPE, m_COM_CLabelType);
	DDX_CBIndex(pDX, IDC_COM_LABELTYPE, m_COM_nLabelType);
	DDX_Text(pDX, IDC_EDT_ATOTALDISTANCE, m_EDT_nATotalDistannce);
	DDX_Text(pDX, IDC_EDT_BTOTALDISTANCE, m_EDT_nBTotalDistannce);
	DDX_Text(pDX, IDC_EDT_A_FRAMENUM, m_EDT_AFrameNum);
	DDX_Text(pDX, IDC_EDT_B_FRAMENUM, m_EDT_BFrameNum);
	DDX_Text(pDX, IDC_EDT_IMAGEHEIGHT, m_EDT_nImageHeight);
	DDV_MinMaxInt(pDX, m_EDT_nImageHeight, 1, 10000);
}


BEGIN_MESSAGE_MAP(CDlgIOSetting, CBCGPDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_TEST_READY, &CDlgIOSetting::OnBnClickedButtonTestReady)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CONTINUOUSERRORALARMS, &CDlgIOSetting::OnBnClickedButtonTestContinuouserroralarms)
	ON_BN_CLICKED(IDC_BUTTON_CONTINUOUSERRORSET, &CDlgIOSetting::OnBnClickedButtonContinuouserrorset)
	ON_BN_CLICKED(IDC_BUTTON_CALLIOCARDPARAMSET, &CDlgIOSetting::OnBnClickedButtonCalliocardparamset)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgIOSetting::OnBnClickedButtonOk)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDCANCEL, &CDlgIOSetting::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgIOSetting::OnBnClickedButtonClose)
	ON_CBN_SELCHANGE(IDC_COM_LABELTYPE, &CDlgIOSetting::OnCbnSelchangeComLabeltype)
	ON_EN_CHANGE(IDC_EDT_ATOTALDISTANCE, &CDlgIOSetting::OnEnChangeEdtAtotaldistance)
	ON_EN_CHANGE(IDC_EDT_BTOTALDISTANCE, &CDlgIOSetting::OnEnChangeEdtBtotaldistance)
END_MESSAGE_MAP()


// CDlgIOSetting ��Ϣ�������


BOOL CDlgIOSetting::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE,TRUE);
	InitUI();
	InitValue();
	return FALSE;
	  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgIOSetting::InitUI()
{
	m_COM_CLabelType.InsertString(0,"��ģʽ");
	m_COM_CLabelType.InsertString(1,"֡����");

}

BOOL CDlgIOSetting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_MOUSEMOVE && pMsg->wParam & MK_LBUTTON)
	{
		
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CDlgIOSetting::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CBCGPDialog::OnPaint()

	CImage Image;
	CString strBmp;
	strBmp.Format("%s//res//DelayModel.bmp",GetCurrentAppPath());
	if (!PathFileExistsA(strBmp))
	{
		return;
	}
	Image.Load(strBmp);
	if (Image.IsNull())
	{
		return;
	}
	CRect rcBmp;
	GetDlgItem(IDC_STATIC_DELAYMODEL)->GetWindowRect(rcBmp); 
	ScreenToClient(rcBmp);

	::SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);  
	::SetBrushOrgEx(dc.GetSafeHdc(), 0, 0, NULL);  
	Image.Draw(dc.GetSafeHdc(),rcBmp);
}


BOOL CDlgIOSetting::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}

void CDlgIOSetting::InitValue()
{
	m_nContinuousErrorAlarmsIO = m_pCommuicate->m_Commuicate.nAlarmPort;
	m_nReadyIO = m_pCommuicate->m_Commuicate.nReadyPort;
	m_nRejectType =  m_pCommuicate->m_Commuicate.nRejectType;
	m_nPulseWidth =  m_pCommuicate->m_Commuicate.nPulseWidth;
	m_nContinuesErrorCountMax = m_pCommuicate->m_Alarms.nMaxContinuesError;
	m_dMarkerLengh = m_pCommuicate->m_Commuicate.dMarkerLengthMM;

	m_EDT_nATotalDistannce=m_pCommuicate->m_Commuicate.nATotalDistance;
	m_EDT_AFrameNum=m_pCommuicate->m_Commuicate.nAFrameNum;
	m_EDT_nBTotalDistannce=m_pCommuicate->m_Commuicate.nBTotalDistance;
	m_EDT_BFrameNum=m_pCommuicate->m_Commuicate.nBFrameNum;

	m_EDT_nImageHeight=m_pCommuicate->m_Commuicate.nSingleImageHeight;






	if (m_COM_nLabelType==LineStype)
	{
		GetDlgItem(IDC_EDT_ATOTALDISTANCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_A_FRAMENUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_BTOTALDISTANCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_B_FRAMENUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_IMAGEHEIGHT)->EnableWindow(FALSE);
	}else
	{
		GetDlgItem(IDC_EDT_ATOTALDISTANCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_A_FRAMENUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_BTOTALDISTANCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_B_FRAMENUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_IMAGEHEIGHT)->EnableWindow(TRUE);
	}

	((CButton*)GetDlgItem(IDC_BUTTON_CONTINUOUSERRORSET))->SetCheck(m_pCommuicate->m_Alarms.bEffective);


	if (m_pCommuicate->m_IOCard[0].hasCard)
	{
		//m_nFrontDelay = m_pCommuicate->m_IOCard[0].readParam(32);
		//m_nBehindDelay = m_pCommuicate->m_IOCard[0].readParam(57);

		m_nRejectDelay = m_pCommuicate->m_IOCard[0].readParam(49);
        m_nRejectDelay1=m_pCommuicate->m_IOCard[0].readParam(76);
		m_EDT_DabiaoDelay = m_pCommuicate->m_IOCard[0].readParam(141);

		m_EDT_nBuChangHangShu=m_pCommuicate->m_IOCard[0].readParam(142);//�ܵĲ�������
		m_EDT_nTiFeiPulse=m_pCommuicate->m_IOCard[0].readParam(92);//�߷϶����������
		

		
	}
	GetDlgItem(IDC_EDIT_CONTINUOUSERRORALARMS)->EnableWindow(m_pCommuicate->m_Alarms.bEffective);
	GetDlgItem(IDC_EDIT_CONTINUOUSERRORCOUNTMAX)->EnableWindow(m_pCommuicate->m_Alarms.bEffective);
	UpdateData(FALSE);
}

void CDlgIOSetting::Save()
{
	UpdateData(TRUE);
	m_pCommuicate->m_Commuicate.nAlarmPort = m_nContinuousErrorAlarmsIO;
	m_pCommuicate->m_Commuicate.nReadyPort = m_nReadyIO;
	m_pCommuicate->m_Commuicate.nRejectType = m_nRejectType;
	m_pCommuicate->m_Commuicate.nPulseWidth = m_nPulseWidth;
	m_pCommuicate->m_Alarms.nMaxContinuesError = m_nContinuesErrorCountMax;
	m_pCommuicate->m_Commuicate.dMarkerLengthMM = m_dMarkerLengh;

	m_pCommuicate->m_Alarms.bEffective = ((CButton*)GetDlgItem(IDC_BUTTON_CONTINUOUSERRORSET))->GetCheck();

	m_pCommuicate->m_Commuicate.nATotalDistance=m_EDT_nATotalDistannce;
	m_pCommuicate->m_Commuicate.nAFrameNum=m_EDT_AFrameNum;
	m_pCommuicate->m_Commuicate.nBTotalDistance=m_EDT_nBTotalDistannce;
	m_pCommuicate->m_Commuicate.nBFrameNum=m_EDT_BFrameNum;





	if (m_pCommuicate->m_IOCard[0].hasCard)
	{

		m_pCommuicate->m_IOCard[0].writeParam(49,m_nRejectDelay);//������x
		m_pCommuicate->m_IOCard[0].writeParam(76,m_nRejectDelay1);//�ڶ�·�����ʱ����
		m_pCommuicate->m_IOCard[0].writeParam(141, m_EDT_DabiaoDelay);//�����ʱ
		m_pCommuicate->m_IOCard[0].writeParam(142, m_EDT_nBuChangHangShu);//�ܵĲ�������
		m_pCommuicate->m_IOCard[0].writeParam(92, m_EDT_nTiFeiPulse);//�߷϶����������

		CString strFile = GetCurrentAppPath() + "\\PIO24B_reg_init.txt";
		CString strApp = "PIO24B";
		CString strValue;

		strValue.Format("0x%x", m_nRejectDelay);
		WritePrivateProfileString(strApp, "49", strValue, strFile);
		strValue.Format("0x%x", m_nRejectDelay1);
		WritePrivateProfileString(strApp, "76", strValue, strFile);
		strValue.Format("0x%x", m_EDT_DabiaoDelay);
		WritePrivateProfileString(strApp, "141", strValue, strFile);

		strValue.Format("0x%x", m_EDT_nTiFeiPulse);
		WritePrivateProfileString(strApp, "92", strValue, strFile);
		strValue.Format("0x%x", m_EDT_nBuChangHangShu);
		WritePrivateProfileString(strApp, "142", strValue, strFile);

	}

	/////д���ļ�/////
	WritePrivateProfileInt("IO","ContinuesErrorAlarms",m_nContinuousErrorAlarmsIO,m_pCommuicate->m_strConfigCommunicateFile);//�������󱨾��ź�
	WritePrivateProfileInt("IO","Ready",m_nReadyIO,m_pCommuicate->m_strConfigCommunicateFile);//Ready�ź�
	WritePrivateProfileInt("IO","RejectType",m_nRejectType,m_pCommuicate->m_strConfigCommunicateFile);//IO����ź�
	WritePrivateProfileInt("IO","PulseWidth",m_nPulseWidth,m_pCommuicate->m_strConfigCommunicateFile);//IO������
	WritePrivateProfileInt("ContinuesErrorAlarms","Effective",m_pCommuicate->m_Alarms.bEffective,m_pCommuicate->m_strConfigCommunicateFile);//�������󱨾��Ƿ���Ч
	WritePrivateProfileInt("ContinuesErrorAlarms","MaxContinuesError",m_nContinuesErrorCountMax,m_pCommuicate->m_strConfigCommunicateFile);//��������������
	WritePrivateProfileDouble("Marker","Length",m_dMarkerLengh,m_pCommuicate->m_strConfigCommunicateFile);//���곤��

	WritePrivateProfileInt("IO","nATotalDistance",m_pCommuicate->m_Commuicate.nATotalDistance,m_pCommuicate->m_strConfigCommunicateFile);
	WritePrivateProfileInt("IO","nAFrameNum",m_pCommuicate->m_Commuicate.nAFrameNum,m_pCommuicate->m_strConfigCommunicateFile);
	WritePrivateProfileInt("IO","nBTotalDistance",m_pCommuicate->m_Commuicate.nBTotalDistance,m_pCommuicate->m_strConfigCommunicateFile);
	WritePrivateProfileInt("IO","nBFrameNum",m_pCommuicate->m_Commuicate.nBFrameNum,m_pCommuicate->m_strConfigCommunicateFile);
    WritePrivateProfileInt("IO","nSingleImageHeight",m_pCommuicate->m_Commuicate.nSingleImageHeight,m_pCommuicate->m_strConfigCommunicateFile);





}

void CDlgIOSetting::OnBnClickedButtonTestReady()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (UpdateData(TRUE))
	{
		static BOOL bTest=TRUE;
		if(bTest)
		{
			GetDlgItem(IDC_BUTTON_TEST_READY)->SetWindowTextA("Stop testing");
		}else
		{
			GetDlgItem(IDC_BUTTON_TEST_READY)->SetWindowTextA("Test");
		}

		if (m_pCommuicate->m_IOCard[0].hasCard)//IO��
		{
			m_pCommuicate->m_IOCard[0].setCardOutput(m_nReadyIO,bTest);
		}
		bTest = !bTest;
		GetDlgItem(IDC_EDIT_READY)->EnableWindow(bTest);
	}
}


void CDlgIOSetting::OnBnClickedButtonTestContinuouserroralarms()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (UpdateData(TRUE))
	{
		static BOOL bTest=TRUE;
		if(bTest)
		{
			GetDlgItem(IDC_BUTTON_TEST_CONTINUOUSERRORALARMS)->SetWindowTextA("Stop testing");
		}else
		{
			GetDlgItem(IDC_BUTTON_TEST_CONTINUOUSERRORALARMS)->SetWindowTextA("Test");
		}

		if (m_pCommuicate->m_IOCard[0].hasCard)//IO��
		{
			m_pCommuicate->m_IOCard[0].setCardOutput(m_nContinuousErrorAlarmsIO,bTest);
		}
		bTest = !bTest;
		GetDlgItem(IDC_EDIT_CONTINUOUSERRORALARMS)->EnableWindow(bTest);
		m_pCommuicate->m_Alarms.nCurrentContinuesError=0;
	}
}


void CDlgIOSetting::OnBnClickedButtonContinuouserrorset()
{

	GetDlgItem(IDC_EDIT_CONTINUOUSERRORALARMS)->EnableWindow(((CButton*)GetDlgItem(IDC_BUTTON_CONTINUOUSERRORSET))->GetCheck());
	GetDlgItem(IDC_EDIT_CONTINUOUSERRORCOUNTMAX)->EnableWindow(((CButton*)GetDlgItem(IDC_BUTTON_CONTINUOUSERRORSET))->GetCheck());
}


void CDlgIOSetting::OnBnClickedButtonCalliocardparamset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (AfxMessageBox("���ڲ���ʮ����Ҫ!\r\n����ĸ��Ļ���������쳣����!\r\n���Ѿ��ͳ�����ϵ��Ҫ���Ĵ˲�������?\r\n",MB_YESNO) == IDYES)
	{
		if (UpdateData(TRUE))
		{
			if (m_pCommuicate->m_IOCard[m_nIOCardSN].hasCard)
			{
				m_pCommuicate->m_IOCard[m_nIOCardSN].Show_PIO24B_DebugDialog(NULL);
			}else
			{
				AfxMessageBox("�޷��򿪣�û�иýӿڿ�������");
			}
		}
	}
}

void CDlgIOSetting::OnBnClickedButtonOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Save();
	CBCGPDialog::OnOK();
}

HBRUSH CDlgIOSetting::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor==CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	return hbr;
}


void CDlgIOSetting::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CBCGPDialog::OnCancel();
}


void CDlgIOSetting::OnBnClickedButtonClose()
{
	CBCGPDialog::OnCancel();
}







void CDlgIOSetting::Calculate()
{

	if (m_EDT_nATotalDistannce>20000)
	{
		m_nRejectDelay =(m_EDT_nATotalDistannce-20000)%m_EDT_nImageHeight+20000;
		m_EDT_AFrameNum=(m_EDT_nATotalDistannce-20000)/m_EDT_nImageHeight;

	}else
	{
		m_nRejectDelay =m_EDT_nATotalDistannce;
		m_EDT_AFrameNum=0;
	}

	if (m_EDT_nATotalDistannce>20000)
	{
		m_nRejectDelay1 =(m_EDT_nBTotalDistannce-20000)%m_EDT_nImageHeight+20000;
		m_EDT_BFrameNum=(m_EDT_nBTotalDistannce-20000)/m_EDT_nImageHeight;

	}else
	{
		m_nRejectDelay1 =m_EDT_nBTotalDistannce;
		m_EDT_BFrameNum=0;
	}
}

void CDlgIOSetting::OnCbnSelchangeComLabeltype()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_COM_nLabelType==LineStype)
	{
		GetDlgItem(IDC_EDT_ATOTALDISTANCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_A_FRAMENUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_BTOTALDISTANCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_B_FRAMENUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_IMAGEHEIGHT)->EnableWindow(FALSE);

	}else
	{
		GetDlgItem(IDC_EDT_ATOTALDISTANCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_A_FRAMENUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_BTOTALDISTANCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_B_FRAMENUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_IMAGEHEIGHT)->EnableWindow(TRUE);
		Calculate();
		UpdateData(FALSE);
	}




}


void CDlgIOSetting::OnEnChangeEdtAtotaldistance()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	UpdateData(TRUE);
	Calculate();
	UpdateData(FALSE);

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CDlgIOSetting::OnEnChangeEdtBtotaldistance()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	UpdateData(TRUE);
	Calculate();
	UpdateData(FALSE);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
