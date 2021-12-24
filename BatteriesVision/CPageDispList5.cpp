// CPageDispList5.cpp: 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "CPageDispList5.h"
#include "afxdialogex.h"


// CPageDispList5 对话框

IMPLEMENT_DYNAMIC(CPageDispList5, CBCGPDialog)

CPageDispList5::CPageDispList5(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_PAGE_DISP_LIST5, pParent)
	, m_EDIT_sCurrentBatch(_T(""))
	, m_Combo_nDefectType(0)
	, m_hvecWndHandle(1)
{
	m_vDefectImageFilePathList.clear();
}

CPageDispList5::~CPageDispList5()
{
}

void CPageDispList5::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_EDIT_sCurrentBatch);
	DDX_CBIndex(pDX, IDC_COMBO1, m_Combo_nDefectType);
}


BEGIN_MESSAGE_MAP(CPageDispList5, CBCGPDialog)
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_EDIT1, &CPageDispList5::OnEnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPageDispList5::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, &CPageDispList5::OnBnClickedButton1)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CPageDispList5 消息处理程序
void CPageDispList5::ResetSize() 
{
	InitUI();
}
void CPageDispList5::InitUI()
{
	m_Combo_nDefectType = 0;
	UpdateData(FALSE);
	CRect rcPic, rcDlg;
	GetClientRect(rcDlg);

	int ROW_NUM = 4;
	int COL_NUM = 5;
	int GAP = 1;
	int nHeight, nWidth;
	SetCheck("~father");
	for (int i=0;i<COL_NUM*ROW_NUM;i++)
	{
		int nRowSN = i / COL_NUM;
		int nColSN = i % COL_NUM;

		 nWidth = (rcDlg.Width() - (COL_NUM - 1) * GAP) / COL_NUM;
		 nHeight =( (rcDlg.Height() - (ROW_NUM - 1) * GAP) -50)/ ROW_NUM;

		rcPic.left = (nWidth + (COL_NUM - 1) * GAP)*nColSN;
		rcPic.right = (nWidth + (COL_NUM - 1) * GAP)*nColSN + nWidth;

		rcPic.top = (nHeight + (ROW_NUM - 3) * GAP)*nRowSN+50;
		rcPic.bottom = (nHeight + (ROW_NUM - 3) * GAP)*nRowSN + nHeight+50;

		HTuple hvTmp;
		OpenWindow(rcPic.top, rcPic.left, rcPic.Width(), rcPic.Height(),
			(Hlong)this->GetSafeHwnd(), "visible", "", &m_hvecWndHandle[i].T());
	}
	//滑块初始化
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	scrollinfo.nPage = rcDlg.bottom - rcDlg.top; //设置滑块大小
	//scrollinfo.nMin = nHeight + 1;
	scrollinfo.nMax = rcDlg.bottom - rcDlg.top; //设置滚动条的最大位置0–100
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
}

BOOL CPageDispList5::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CPageDispList5::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if (pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4 && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageDispList5::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}


BOOL CPageDispList5::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageDispList5::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	UpdateData(TRUE);
	// TODO:  在此添加控件通知处理程序代码
}


void CPageDispList5::OnCbnSelchangeCombo1()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


void CPageDispList5::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_vDefectImageFilePathList.clear();
	CString  sCurrentBatch,sDefectType, strFileName, STR;
	int nCount = 0;
	try
	{
		sCurrentBatch.Format("%s%s_Defect", theGlobal.m_Storage.GetImageInquireFolderPath(), m_EDIT_sCurrentBatch);//瑕疵图像路径
		if (m_Combo_nDefectType == LeakFoil)
		{
			sDefectType.Format("漏金属");
			STR = "漏金属";
		}
		if (m_Combo_nDefectType == Drop)
		{
			sDefectType.Format("脱碳");
			STR = "脱碳";
		}
		//遍历文件夹
		CFileFind finder;
		BOOL isNotEmpty = finder.FindFile(sCurrentBatch );//总文件夹，开始遍历
		if (!isNotEmpty)
		{
			AfxMessageBox("目录不存在!");
			return;
		}

		sCurrentBatch += "\\*.*";
		isNotEmpty = (BOOL)finder.FindFile(sCurrentBatch);

		while (isNotEmpty)
		{
			isNotEmpty = (BOOL)finder.FindNextFile();
			if (!finder.IsDots())
			{
				nCount++;
				//每次得到一个文件名
				strFileName = finder.GetFileName();
				CString strDefectName,strDefectImagePath;
				strDefectName = STR.SpanIncluding(strFileName);
				bool bDefectFile=strDefectName.IsEmpty();
				if (strDefectName==STR && !bDefectFile)
				{
					strDefectImagePath = finder.GetFilePath();
					m_vDefectImageFilePathList.push_back(strDefectImagePath);
				}
			}
		}
		finder.Close();
		if (m_vDefectImageFilePathList.size() > 20)
		{
			SCROLLINFO scrollinfo;
			GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);  //检索 SCROLLINFO 结构维护的有关滚动条的信息。
			int nRow = (m_vDefectImageFilePathList.size() - 20) / 5;
			int nCol = (m_vDefectImageFilePathList.size() - 20) % 5;
			scrollinfo.nMax += 218 * (nRow + nCol - 4);
			scrollinfo.nPage = scrollinfo.nPage / (nRow + nCol) * 4;
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);//设置有关滚动条的信息。
		}
		else
		{
			CRect rcPic, rcDlg;
			GetClientRect(rcDlg);
			SCROLLINFO scrollinfo;
			GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
			scrollinfo.nPage = rcDlg.bottom - rcDlg.top; //设置滑块大小
			//scrollinfo.nMin = nHeight + 1;
			scrollinfo.nMax = rcDlg.bottom - rcDlg.top; //设置滚动条的最大位置0–100
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		}
		UpdateList();
	}
	catch (const std::exception&)
	{

	}
}

//显示图像
void CPageDispList5::UpdateList()
{
	HObject hoBG;
	HTuple hvW, hvH, hvNum;
	int nNumber =  m_vDefectImageFilePathList.size();
	for (int i = 0; i < MIN(m_hvecWndHandle.Length(), m_vDefectImageFilePathList.size()); i++)
	{
		ReadImage(&hoBG, (HTuple)m_vDefectImageFilePathList[i]);
		GetImageSize(hoBG, &hvW, &hvH);
		SetPart(m_hvecWndHandle[i].T(), 0, 0, hvH - 1, hvW - 1);
		DispObj(hoBG, m_hvecWndHandle[i].T());
	}
}

void CPageDispList5::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);  //检索 SCROLLINFO 结构维护的有关滚动条的信息。
	int unit = 3;
	switch (nSBCode)
	{
	case SB_LINEUP:          //向上滚动一行
		scrollinfo.nPos -= 218;
		if (scrollinfo.nPos < scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);//设置有关滚动条的信息。
		ScrollWindow(0, unit);//设置滚动信息参数
		break;
	case SB_LINEDOWN:           //向下滚动一行
		scrollinfo.nPos += 218;
		if (scrollinfo.nPos + scrollinfo.nPage > scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;   //最大滚动
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -unit);
		break;
	case SB_PAGEUP:            //向上滚动一页
		scrollinfo.nPos -= 5;
		if (scrollinfo.nPos <= scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, unit * 5);
		break;
	case SB_PAGEDOWN:        //向下滚动一页 
		scrollinfo.nPos += 5;
		if (scrollinfo.nPos + scrollinfo.nPage >= scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -unit * 5);
		break;
	case SB_ENDSCROLL:      //用户结束滚动。
		break;
	case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos用户已拖动滚动框 (滚动) 并释放鼠标按钮。
		break;
	case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos用户正在拖动滚动框
		ScrollWindow(0, (scrollinfo.nPos - nPos)*unit);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		break;
	}

	CBCGPDialog::OnVScroll(nSBCode, nPos, pScrollBar);

}