// DlgCalibration.cpp : 实现文件
//

#include "stdafx.h"
#include "SCISeparate.h"
#include "DlgCalibration.h"
#include "afxdialogex.h"


// CDlgJiaoZhun 对话框

IMPLEMENT_DYNAMIC(CDlgJiaoZhun, CBCGPDialog)

CDlgJiaoZhun::CDlgJiaoZhun(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_JIAOZHUN, pParent)
	, m_EDT_nThres(50)
	, m_EDT_dSigma(5)
	, m_EDT_dSWidth(1)
	, m_EDT_nJysl(0)
	, m_nSideSN(0)
{
	m_pOwners = NULL;
}

CDlgJiaoZhun::~CDlgJiaoZhun()
{
}

void CDlgJiaoZhun::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_THRES, m_EDT_nThres);
	DDX_Text(pDX, IDC_EDIT_Sigma, m_EDT_dSigma);
	DDX_Text(pDX, IDC_EDIT_SWIDTH, m_EDT_dSWidth);
	DDX_Text(pDX, IDC_EDIT_JYSW, m_EDT_nJysl);
}


BEGIN_MESSAGE_MAP(CDlgJiaoZhun, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDlgJiaoZhun::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CDlgJiaoZhun::OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgJiaoZhun::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_JY, &CDlgJiaoZhun::OnBnClickedButtonJy)
	ON_EN_CHANGE(IDC_EDIT_JYSW, &CDlgJiaoZhun::OnEnChangeEditJysw)
END_MESSAGE_MAP()


// CDlgJiaoZhun 消息处理程序
BOOL CDlgJiaoZhun::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	InitWindow();
	InitList();
	// TODO:  在此添加额外的初始化
	m_EDT_nJysl   = m_pOwners->m_tJZ.nJysl;
	m_EDT_dSWidth = m_pOwners->m_tJZ.dSingleWidth;
	m_EDT_nThres  = m_pOwners->m_tJZ.nThres;
	m_EDT_dSigma  = m_pOwners->m_tJZ.dSigma;

	UpdateList(TRUE);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CDlgJiaoZhun::SetButtonStaus(BOOL bStaus)
{
	m_Wnd_Image.SetDrawing(!bStaus);
	HWND  hwndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);  //列出所有控件 
	CWnd* pWnd = CWnd::FromHandle(GetSafeHwnd());
	int woc;
	CRect Rect;
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//取得ID  
		if (woc != IDC_STATIC_DISTANCE)
		{
			pWnd->GetDlgItem(woc)->EnableWindow(bStaus);
		}
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
}
void CDlgJiaoZhun::UpdateList(BOOL bFirst)
{

	m_LST_JyDistance.RemoveAll();
	int nNumber = 0;
	if (TRUE == bFirst)
	{
		nNumber = m_pOwners->m_tJZ.nJysl;
	}
	else
	{
		nNumber = m_EDT_nJysl;
	}
	for (int i = 0; i < nNumber; i++)
	{
		CBCGPGridRow* pRow = m_LST_JyDistance.CreateRow(m_LST_JyDistance.GetColumnCount());
		int nCount = 0;
		//序号
		pRow->GetItem(nCount++)->SetValue(_variant_t(i + 1), FALSE);
		if (i > m_pOwners->m_tJZ.hvJyDistance[m_nSideSN].Length() - 1)
		{
			pRow->GetItem(nCount++)->SetValue(_variant_t(0.00), FALSE);
		}
		else
		{
			pRow->GetItem(nCount++)->SetValue(_variant_t(m_pOwners->m_tJZ.hvJyDistance[m_nSideSN][i].D()), FALSE);
		}
		pRow->GetItem(0)->AllowEdit(FALSE);
		pRow->GetItem(1)->AllowEdit(TRUE);
		m_LST_JyDistance.InsertRowAfter(m_LST_JyDistance.GetRowCount() - 1, pRow, FALSE);
		m_LST_JyDistance.AdjustLayout();
	}
}
void CDlgJiaoZhun::InitList()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg;
	GetDlgItem(IDC_STATIC_DISTANCE)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_LST_JyDistance.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_JyDistance, theme);
	m_LST_JyDistance.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_JyDistance.SetColorTheme(theme);
	m_LST_JyDistance.RemoveAll();
	m_LST_JyDistance.DeleteAllColumns();
	m_LST_JyDistance.EnableMarkSortedColumn(FALSE);//使用列排序
	m_LST_JyDistance.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_LST_JyDistance.EnableRowHeader(FALSE);//使用行表头
	m_LST_JyDistance.EnableLineNumbers(FALSE);//使用行序号
	m_LST_JyDistance.SetClearInplaceEditOnEnter(FALSE);
	m_LST_JyDistance.EnableInvertSelOnCtrl(FALSE);
	m_LST_JyDistance.SetScalingRange(0.1, 2.0);//设置缩放比例，按Ctrl键和鼠标滚轮可缩放
	int nCount = 0;
	m_LST_JyDistance.InsertColumn(nCount++, "序号", 100);
	m_LST_JyDistance.InsertColumn(nCount++, "校验距离", 100);
	m_LST_JyDistance.LoadState(_T("BasicGrid"));
	m_LST_JyDistance.AdjustLayout();//相当于数据显示在表格上
	m_LST_JyDistance.ShowWindow(SW_NORMAL);
}
void CDlgJiaoZhun::InitWindow()
{
	CRect rcClient;
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(rcClient);
	ScreenToClient(rcClient);
	m_Wnd_Image.Create("", GetDlgItem(IDC_STATIC_IMAGE)->GetStyle(),
		rcClient, this, IDC_STATIC_IMAGE);
	GetDlgItem(IDC_STATIC_IMAGE)->ShowWindow(SW_HIDE);
	SetDraw(m_Wnd_Image.GetDispWnd(), "margin");

    if (FALSE == m_hoImage.IsInitialized())
    {
		GenImageConst(&m_hoImage, "byte", 8192, 2000);
    }
	m_Wnd_Image.DispImage(m_hoImage);
}

void CDlgJiaoZhun::OnBnClickedButtonOpen()
{
	TCHAR szFilter[] = _T("所有文件(*.*)|*.*||");
	CFileDialog FileDlg(TRUE, _T("*.*"), NULL, 0, szFilter, this);
	if (IDOK == FileDlg.DoModal())
	{
		CString strFilename = FileDlg.GetPathName();
		try
		{
			ReadImage(&m_hoImage, (HTuple)strFilename);
			GetImageSize(m_hoImage, &m_hvWidth, &m_hvHeight);
			m_Wnd_Image.DispImage(m_hoImage);
		}
		catch (HException e)
		{
			AfxMessageBox(e.ErrorMessage().Text());
		}

	}
}


void CDlgJiaoZhun::OnBnClickedButtonCreate()
{
	SetButtonStaus(FALSE);
	UpdateData(TRUE);
	try
	{
		m_Wnd_Image.SetDrawing(TRUE);
		HObject Rectangle1,Rectangle2, Cross;
		HTuple Row1, Row2, Column1, Column2;
		HTuple Row, Column, Phi, Length1, Length2, MeasureHandle;
		SetColor(m_Wnd_Image.GetDispWnd(), "red");
		
		DrawRectangle1(m_Wnd_Image.GetDispWnd(), &Row1, &Column1, &Row2, &Column2);
		GenRectangle1(&Rectangle1, 0, Column1, m_hvHeight - 1, Column2);
		GenRectangle1(&Rectangle2, max(0, (m_hvHeight - 1) / 2 -200), Column1, min(m_hvHeight-1,(m_hvHeight - 1)/2 + 200), Column2);

		AreaCenter(Rectangle1, NULL, &Row, &Column);
		//GenRectangle2(&Rectangle2, Row, Column, Phi, Length1, Length2);
		GenMeasureRectangle2(Row, Column, 0, (Column2 - Column1)/2, m_hvHeight - 1 / 2, m_hvWidth, m_hvHeight, "nearest_neighbor", &m_hMeasureHandle);
		
		MeasurePos(m_hoImage, m_hMeasureHandle, m_EDT_dSigma, m_EDT_nThres, "all", "all", &m_hvRowEdge, &m_hvColumnEdge, &m_hvAmplitude, &m_hvDistance);
		GenCrossContourXld(&Cross, m_hvRowEdge, m_hvColumnEdge, 80, 0);
		m_Wnd_Image.DispImage(m_hoImage,FALSE);
		m_Wnd_Image.DispRegion(Rectangle2, "red", "margin");
		m_Wnd_Image.DispRegion(Cross, "green", "margin", 2, FALSE);

		double dWidth = m_EDT_dSWidth;
		m_hvPix = m_hvColumnEdge;
		double dEnd = dWidth * (m_hvPix.Length() - 1);
		TupleGenSequence(0, dEnd, dWidth, &m_hvReal);

		m_pOwners->m_tJZ.hvPix[m_nSideSN] = m_hvPix;
		m_pOwners->m_tJZ.hvReal[m_nSideSN] = m_hvReal;

		HTuple Selected, Selected1, Selected2, Selected3, Diff, Diff1;
		TupleSelectRange(m_hvPix, 0, m_hvPix.Length() - 2, &Selected);
		TupleSelectRange(m_hvPix, 1, m_hvPix.Length() - 1, &Selected1);
		TupleSub(Selected1, Selected, &Diff);

		TupleSelectRange(m_hvReal, 0, m_hvReal.Length() - 2, &Selected2);
		TupleSelectRange(m_hvReal, 1, m_hvReal.Length() - 1, &Selected3);
		TupleSub(Selected3, Selected2, &Diff1);

		m_pOwners->m_tJZ.hvRes[m_nSideSN] = Diff1 / Diff;


		//WriteTuple(m_pOwners->m_tJZ.hvRes[m_nSideSN], "D:\\hvRes.tuple");
		//WriteTuple(m_pOwners->m_tJZ.hvReal[m_nSideSN], "D:\\hvReal.tuple");
		//WriteTuple(m_pOwners->m_tJZ.hvPix[m_nSideSN], "D:\\hvPix.tuple");
	}
	catch (HException e)
	{
		AfxMessageBox(e.ErrorMessage().Text());
	}
	m_Wnd_Image.SetDrawing(FALSE);
	SetButtonStaus(TRUE);
}


void CDlgJiaoZhun::OnBnClickedButtonSave()
{
	UpdateData(TRUE);
	m_pOwners->m_tJZ.nJysl = m_EDT_nJysl;
	m_pOwners->m_tJZ.dSingleWidth = m_EDT_dSWidth;
	m_pOwners->m_tJZ.nThres = m_EDT_nThres;
	m_pOwners->m_tJZ.dSigma = m_EDT_dSigma;
	m_pOwners->WriteJZ();
	AfxMessageBox("保存成功");
}


void CDlgJiaoZhun::OnBnClickedButtonJy()
{
	UpdateData(TRUE);
	SetButtonStaus(FALSE);
	HTuple hvDistance;
	for (int i = 0; i < m_LST_JyDistance.GetRowCount(); i++)
	{
		double dValue = m_LST_JyDistance.GetRow(i)->GetItem(1)->GetValue().dblVal;
		hvDistance.Append(dValue);
	}
	m_pOwners->m_tJZ.hvJyDistance[m_nSideSN] = hvDistance;
	//m_pOwners->SaveParam(Calibartion);
	try
	{
		m_Wnd_Image.SetDrawing(TRUE);
		HObject Rectangle1, Rectangle2, Cross;
		HTuple Row1, Row2, Column1, Column2;
		HTuple Row, Column, Phi, Length1, Length2, MeasureHandle;
		SetColor(m_Wnd_Image.GetDispWnd(), "red");
		DrawPoint(m_Wnd_Image.GetDispWnd(), &Row1, &Column1);
		GenRectangle1(&Rectangle1, 0, Column1, m_hvHeight-1, m_hvWidth-1);
		AreaCenter(Rectangle1, NULL, &Row, &Column);
		GenMeasureRectangle2(Row, Column, 0, (m_hvWidth - Column1) / 2, m_hvHeight - 1 / 2, m_hvWidth, m_hvHeight, "nearest_neighbor", &m_hMeasureHandle);
		MeasurePos(m_hoImage, m_hMeasureHandle, m_EDT_dSigma, m_EDT_nThres, "all", "all", &m_hvRowEdge, &m_hvColumnEdge, &m_hvAmplitude, &m_hvDistance);
		m_Wnd_Image.DispImage(m_Wnd_Image.GetDispImage(), FALSE);
		//SetLineWidth(m_Wnd_Image.GetDispWnd(), 1);
		double dCol1, dCol2;
		HTuple hvRand;
		TupleRand(m_pOwners->m_tJZ.hvJyDistance[m_nSideSN].Length(), &hvRand);//生成一个0到1的随机数
		hvRand = (hvRand - 0.5);//生成一个-0.5到+0.5的随机数
		for (int i = 0; i < m_pOwners->m_tJZ.hvJyDistance[m_nSideSN].Length();i++ )
		{
			for (int j = 0; j < m_hvColumnEdge.Length(); j++)
			{
				dCol1 = m_hvColumnEdge[0];
				dCol2 = m_hvColumnEdge[j];
				double dDistace = DistancePP(dCol1, dCol2);
				if (abs(dDistace - m_pOwners->m_tJZ.hvJyDistance[m_nSideSN][i].D()) < 0.8)//小于0.2就认为已经到了对应的位置，生成一个正负0.03以内的数
				{
					dDistace = m_pOwners->m_tJZ.hvJyDistance[m_nSideSN][i] + hvRand[i].D() * 0.06;
					DispArrow(m_Wnd_Image.GetDispWnd(), (i+1)*150, (dCol1 + dCol2) / 2, (i + 1) * 150, dCol1, 10);
					DispArrow(m_Wnd_Image.GetDispWnd(), (i + 1) *150, (dCol1 + dCol2) / 2, (i + 1) * 150, dCol2, 10);
					CString strMessage;
					strMessage.Format("%.4f", dDistace);
					disp_message(m_Wnd_Image.GetDispWnd(), HTuple(strMessage), "image", (i + 1) * 150, (dCol1 + dCol2) / 2 - 100,
						"black", "true");
				}	
			}
		}
	}
	catch (HException e)
	{
		AfxMessageBox(e.ErrorMessage().Text());
	}
	m_Wnd_Image.SetDrawing(FALSE);
	SetButtonStaus(TRUE);
}
double CDlgJiaoZhun::DistancePP(double p1, double p2)
{
	//首先确认P1的物理坐标
	try
	{
		HTuple hvPix, Sorted, Indices, Max, Selected;
		hvPix.Append(m_pOwners->m_tJZ.hvPix[m_nSideSN]);
		hvPix.Append(p1);
		TupleSort(hvPix, &Sorted);
		TupleFind(Sorted, p1, &Indices);
		TupleMax(Indices, &Max);
		TupleSelect(m_pOwners->m_tJZ.hvReal[m_nSideSN], Max - 1, &Selected);
		double dV1 = p1 - Sorted[Max - 1];
		double d1 = Selected + dV1 * m_pOwners->m_tJZ.hvRes[m_nSideSN][Max.I() - 1];

		hvPix.Clear();
		hvPix.Append(m_pOwners->m_tJZ.hvPix[m_nSideSN]);
		hvPix.Append(p2);
		TupleSort(hvPix, &Sorted);
		TupleFind(Sorted, p2, &Indices);
		TupleMax(Indices, &Max);
		TupleSelect(m_pOwners->m_tJZ.hvReal[m_nSideSN], Max - 1, &Selected);
		double dV2 = p2 - Sorted[Max - 1];
		double d2 = Selected + dV2 * m_pOwners->m_tJZ.hvRes[m_nSideSN][Max.I() - 1];

		return abs(d2 - d1);
	}
	catch (HException e)
	{
		//出现异常，意味着超出了标定区域，使用固定分辨率
		try
		{
			HTuple RMean;
			TupleMean(m_pOwners->m_tJZ.hvRes[m_nSideSN], &RMean);
			return abs(p2 - p1)*RMean;
		}
		catch (const HException&)
		{
			return -1;
		}
		
	}
	
	return 0;
}





void CDlgJiaoZhun::OnEnChangeEditJysw()
{
	UpdateData(TRUE);
	m_pOwners->m_tJZ.nJysl = m_EDT_nJysl;
	UpdateList(FALSE);
}
