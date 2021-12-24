// DlgSet.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "LightControl.h"
#include "DlgSet.h"
#include "afxdialogex.h"

// CDlgSet �Ի���
IMPLEMENT_DYNAMIC(CDlgSet, CBCGPDialog)

CDlgSet::CDlgSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSet::IDD, pParent)
	, m_PortRecored(_T(""))
	, m_SliderValue(0)
{

}

CDlgSet::~CDlgSet()
{
}

void CDlgSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_SERIAL_LIST, m_SerialPortList);
	DDX_Control(pDX, IDC_COMBO_CHAANEL_LIST, m_ChannelList);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_Open);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_Close);
	DDX_Control(pDX, IDC_SLIDER_SET_LIGHT, m_SliderV);
	DDX_Text(pDX, IDC_EDIT_SERIAL_RECORED, m_PortRecored);
	DDX_Slider(pDX, IDC_SLIDER_SET_LIGHT, m_SliderValue);
	DDV_MinMaxInt(pDX, m_SliderValue, 0, 255);
	DDX_Control(pDX, IDC_COMBO_LIGHT_TYPE, m_LightType);
}

BEGIN_MESSAGE_MAP(CDlgSet, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SERIAL_LIST, &CDlgSet::OnCbnSelchangeComboSerialList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDlgSet::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgSet::OnBnClickedButtonClose)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_CHAANEL_LIST, &CDlgSet::OnCbnSelchangeComboChaanelList)
	ON_WM_ACTIVATE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_LIGHT_TYPE, &CDlgSet::OnCbnSelchangeComboLightType)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgSet::OnBnClickedButton2)
END_MESSAGE_MAP()

BOOL CDlgSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

// CDlgSet ��Ϣ�������
BOOL CDlgSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	InitUI();
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDlgSet::InitUI()
{
	m_Open.EnableWindow(TRUE);
	m_Close.EnableWindow(FALSE);
	m_SliderV.EnableWindow(FALSE);

	m_SliderV.SetRange(0, 255);
	m_SliderV.SetTicFreq(1);//ÿ����λ��һ�̶�	

	CSerialPortExt Port;
	Port.GetComList(m_PortList);//�����ʼ�����Զ���ȡ���ô���
	for (int i = 0; i < m_PortList.size(); i++)
	{
		m_SerialPortList.AddString(TEXT(m_PortList[i]));
	}

	//����������Ĭ��ֵ
	m_LightType.SetCurSel(g_This->GetLightParam().eLightType);//��Դ�ͺ� 
	if (m_SerialPortList.GetCount() > 0)
		m_SerialPortList.SetCurSel(g_This->GetLightParam().nComIndex);//�����±�
	OnCbnSelchangeComboLightType();
	m_ChannelList.SetCurSel(-1);//Aͨ��	

	UpdateData(TRUE);
}
//����������ѡ���¼�
void CDlgSet::OnCbnSelchangeComboSerialList()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int index =  m_SerialPortList.GetCurSel();
	m_SerialPortList.GetLBText(index, CurPort);
}

//�򿪴���
void CDlgSet::OnBnClickedButtonOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�򿪴���ǰ���ȼ��ش���/����/ͨ�����Ƿ�Ϊ��
	int index  =  m_SerialPortList.GetCurSel();	
	if(index < 0)
	{
		BCGPMessageBox("����ѡ����Ϊ��!");
		return;
	}
	
	m_SerialPortList.GetLBText(index, CurPort);
	g_This->GetLightParam().nComIndex = index;
	g_This->GetLightParam().sComName = CurPort;
	if(g_This->Open())
	{
		m_Open.EnableWindow(FALSE);
		m_Close.EnableWindow(TRUE);
		m_SliderV.EnableWindow(TRUE);	
		//�༭����ʾ�򿪽��
		CString str;
		str.Format("�ɹ��򿪴���:%s��\r\n", CurPort);
		m_PortRecored += TEXT(str); 
	}
	else
	{
		m_PortRecored += TEXT("�򿪴���ʧ�ܣ�\r\n"); 
	}
	
	UpdateData(FALSE); // ���������ݣ�ͬ�����ؼ��У�
}

//�رչ�Դ����
void CDlgSet::OnBnClickedButtonClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_This->Close())
	{
		m_ChannelList.SetCurSel(-1);

		m_Open.EnableWindow(TRUE);
		m_Close.EnableWindow(FALSE);
		m_SliderV.EnableWindow(FALSE);
	}	
}


void CDlgSet::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CBCGPDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	//��ȡѡ��ͨ��
	int index = m_ChannelList.GetCurSel();   
	if(index < 0)
	{
		BCGPMessageBox("ͨ��ѡ���Ϊ�գ�");
		return;
	}
	m_ChannelList.GetLBText(index, CurChannel);
	CSliderCtrl *pSlidCtrl=(CSliderCtrl*)GetDlgItem(IDC_SLIDER_SET_LIGHT);
    m_SliderValue = pSlidCtrl->GetPos();//ȡ�õ�ǰλ��ֵ 	
	g_This->SetBrightness(index + 1, m_SliderValue);
}
//��Դͨ��������ѡ���¼�
void CDlgSet::OnCbnSelchangeComboChaanelList()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int index =  m_ChannelList.GetCurSel();
	if (index < 0)
	{
		BCGPMessageBox("ͨ��ѡ���Ϊ�գ�");
		return;
	}
	m_ChannelList.GetLBText(index, CurChannel);
	//��ȡ��ǰͨ����Ĭ������ֵ������ʼ��������λ��
	int light;
	CString str;
	light = g_This->GetBrightnes(index + 1);
	if (light == -1)
	{
		str.Format("��ȡͨ��%d������ֵ�쳣:%d \r\n", index + 1, 0);
		m_PortRecored += str;
		UpdateData(FALSE); // ���������ݣ�ͬ�����ؼ��У�
		return;
	}
	
    str.Format("��ǰѡ��ͨ��:%c,����ֵ:%d \r\n",'A'+index, light);
	m_PortRecored += str;
	UpdateData(FALSE); // ���������ݣ�ͬ�����ؼ��У�
	m_SliderV.SetPos(light);
		
}

void CDlgSet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	/*CBCGPDialog*/CBCGPDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: �ڴ˴������Ϣ����������

	if (WA_INACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
		this->SetLayeredWindowAttributes(0, (255 * 90) / 100, LWA_ALPHA);
	}
	else if (WA_CLICKACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}
	else if (WA_ACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}
}

void CDlgSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CBCGPDialog::OnPaint()
}

void CDlgSet::OnCbnSelchangeComboLightType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int index =  m_LightType.GetCurSel();
	m_LightType.GetLBText(index, CurLightType);
	int num = m_LightType.GetCount();
	for(int i = 0;i<num;i++)
	{
		m_ChannelList.DeleteString(num-i-1);
	}	
	if("DH-HAPS-48W800-1T" == CurLightType)
	{
		m_ChannelList.AddString("A");
	}
	else if("DH-HAPS-48W800-2T" == CurLightType)
	{
		m_ChannelList.AddString("A");
		m_ChannelList.AddString("B");
	}

	m_ChannelList.SetCurSel(0);
}


void CDlgSet::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_This->Close();

	int index = m_SerialPortList.GetCurSel();	
	g_This->GetLightParam().nComIndex = index;
	if (index > 0)
		m_SerialPortList.GetLBText(index, g_This->GetLightParam().sComName);
	else
		g_This->GetLightParam().sComName = "";
	g_This->GetLightParam().eLightType = (eLightControlType)m_LightType.GetCurSel();

	g_This->SaveParams();

	CBCGPDialog::OnOK();
}
