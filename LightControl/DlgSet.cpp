// DlgSet.cpp : 实现文件
//
#include "stdafx.h"
#include "LightControl.h"
#include "DlgSet.h"
#include "afxdialogex.h"

// CDlgSet 对话框
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
	// TODO: 在此添加专用代码和/或调用基类

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

// CDlgSet 消息处理程序
BOOL CDlgSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// 将“关于...”菜单项添加到系统菜单中。
	// TODO: 在此添加额外的初始化代码
	
	InitUI();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgSet::InitUI()
{
	m_Open.EnableWindow(TRUE);
	m_Close.EnableWindow(FALSE);
	m_SliderV.EnableWindow(FALSE);

	m_SliderV.SetRange(0, 255);
	m_SliderV.SetTicFreq(1);//每个单位画一刻度	

	CSerialPortExt Port;
	Port.GetComList(m_PortList);//程序初始化，自动获取可用串口
	for (int i = 0; i < m_PortList.size(); i++)
	{
		m_SerialPortList.AddString(TEXT(m_PortList[i]));
	}

	//设置下拉框默认值
	m_LightType.SetCurSel(g_This->GetLightParam().eLightType);//光源型号 
	if (m_SerialPortList.GetCount() > 0)
		m_SerialPortList.SetCurSel(g_This->GetLightParam().nComIndex);//串口下标
	OnCbnSelchangeComboLightType();
	m_ChannelList.SetCurSel(-1);//A通道	

	UpdateData(TRUE);
}
//串口下拉框选择事件
void CDlgSet::OnCbnSelchangeComboSerialList()
{
	// TODO: 在此添加控件通知处理程序代码
	int index =  m_SerialPortList.GetCurSel();
	m_SerialPortList.GetLBText(index, CurPort);
}

//打开串口
void CDlgSet::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	//打开串口前，先加载串口/波特/通道等是否为空
	int index  =  m_SerialPortList.GetCurSel();	
	if(index < 0)
	{
		BCGPMessageBox("串口选择不能为空!");
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
		//编辑框显示打开结果
		CString str;
		str.Format("成功打开串口:%s！\r\n", CurPort);
		m_PortRecored += TEXT(str); 
	}
	else
	{
		m_PortRecored += TEXT("打开串口失败！\r\n"); 
	}
	
	UpdateData(FALSE); // 将变量内容，同步到控件中；
}

//关闭光源串口
void CDlgSet::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CBCGPDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	//获取选中通道
	int index = m_ChannelList.GetCurSel();   
	if(index < 0)
	{
		BCGPMessageBox("通道选项不能为空！");
		return;
	}
	m_ChannelList.GetLBText(index, CurChannel);
	CSliderCtrl *pSlidCtrl=(CSliderCtrl*)GetDlgItem(IDC_SLIDER_SET_LIGHT);
    m_SliderValue = pSlidCtrl->GetPos();//取得当前位置值 	
	g_This->SetBrightness(index + 1, m_SliderValue);
}
//光源通道下拉框选择事件
void CDlgSet::OnCbnSelchangeComboChaanelList()
{
	// TODO: 在此添加控件通知处理程序代码
	int index =  m_ChannelList.GetCurSel();
	if (index < 0)
	{
		BCGPMessageBox("通道选项不能为空！");
		return;
	}
	m_ChannelList.GetLBText(index, CurChannel);
	//读取当前通道的默认亮度值，并初始化滑动条位置
	int light;
	CString str;
	light = g_This->GetBrightnes(index + 1);
	if (light == -1)
	{
		str.Format("获取通道%d的亮度值异常:%d \r\n", index + 1, 0);
		m_PortRecored += str;
		UpdateData(FALSE); // 将变量内容，同步到控件中；
		return;
	}
	
    str.Format("当前选中通道:%c,亮度值:%d \r\n",'A'+index, light);
	m_PortRecored += str;
	UpdateData(FALSE); // 将变量内容，同步到控件中；
	m_SliderV.SetPos(light);
		
}

void CDlgSet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	/*CBCGPDialog*/CBCGPDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码

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
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBCGPDialog::OnPaint()
}

void CDlgSet::OnCbnSelchangeComboLightType()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
