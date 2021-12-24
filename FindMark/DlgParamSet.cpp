// DlgParamSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FindMark.h"
#include "DlgParamSet.h"
#include "afxdialogex.h"

// CDlgParamSet �Ի���

IMPLEMENT_DYNAMIC(CDlgParamSet, CBCGPDialog)

	CDlgParamSet::CDlgParamSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgParamSet::IDD, pParent)
	, m_nSearchCoefficient_Column(0)
	, m_strMarkType(_T(""))
	, m_strDrawType(_T(""))
	, m_strMarkName(_T(""))
	, m_COM_nType(1)
	, m_EDT_dMiniScore(0.8)
{
	// ��������
	m_hvDispImageWidth = 0;			// ��ʾͼ���
	m_hvDispImageHeight = 0;		// ��ʾͼ���
	m_dDispImagePartRow0 = 0;
	m_dDispImagePartCol0 = 0;
	m_dDispImagePartRow1 = 0;
	m_dDispImagePartCol1 = 0;
	m_hv_WindowID = NULL;
	m_bDrawing = FALSE;
	m_nSearchCoefficient_Column = 150;
	GenEmptyObj(&m_hoCurrentDispImage);
	m_pOwner=new CFindMark;
}

CDlgParamSet::~CDlgParamSet()
{
}

void CDlgParamSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_MarkTree);
	DDX_Text(pDX, IDC_EDIT_CREATESEARCHCOEFFICIENT_COLUMN, m_nSearchCoefficient_Column);
	DDV_MinMaxInt(pDX, m_nSearchCoefficient_Column, -1, 2046);
	DDX_CBString(pDX, IDC_COMBO_CREATETYPE, m_strMarkType);
	DDX_CBString(pDX, IDC_COMBO_DRAWTYPE, m_strDrawType);
	DDX_Text(pDX, IDC_EDIT_CREATEMARKNAME, m_strMarkName);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_COM_nType);
	DDX_Text(pDX, IDC_EDIT_MINISCORE, m_EDT_dMiniScore);
}


BEGIN_MESSAGE_MAP(CDlgParamSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgParamSet::OnBnClickedButtonAdd)
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_OPENIMAGE, &CDlgParamSet::OnBnClickedButtonOpenimage)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, &CDlgParamSet::OnNMDblclkTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, &CDlgParamSet::OnNMRClickTree)
	ON_COMMAND(ID_MENU_DELETE, &CDlgParamSet::OnMenuDelete)
	ON_COMMAND(ID_MENU_RENAME, &CDlgParamSet::OnMenuRename)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgParamSet::OnBnClickedButtonCancel)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_ADDLINE, &CDlgParamSet::OnBnClickedButtonAddline)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CDlgParamSet::OnBnClickedButtonTest)
	ON_WM_NCHITTEST()
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CDlgParamSet::OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BTN_SAVEPARAM, &CDlgParamSet::OnBnClickedBtnSaveparam)
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CDlgParamSet::DispMark(const SINGLEMARKINFO&  tMarkDisp)
{
	//��ʾԭͼ
	HObject ho_Image = tMarkDisp.ho_ModleInfo.SelectObj(1);
	HObject ho_Mark  = tMarkDisp.ho_ModleInfo.SelectObj(2);
	HObject ho_Search = tMarkDisp.ho_ModleInfo.SelectObj(3);
	DispImage(ho_Image);
	SetColor(m_hv_WindowID,"red");
	DispObj(ho_Mark,m_hv_WindowID);
	DispObj(tMarkDisp.ho_Line,m_hv_WindowID);
	SetColor(m_hv_WindowID,"yellow");
	DispObj(ho_Search,m_hv_WindowID);
	if ("��״ƥ��" == tMarkDisp.hv_ModelParam[1])
	{
		HObject ho_ModelContours,ho_ContoursAffinTrans;
		HTuple hv_Row,hv_Column,hvHomMat2D;
		AreaCenter(ho_Mark,NULL,&hv_Row,&hv_Column);
		GetShapeModelContours(&ho_ModelContours, tMarkDisp.hv_ModelID, 1);
		VectorAngleToRigid(0, 0, 0, hv_Row, hv_Column, 0, &hvHomMat2D);
		AffineTransContourXld(ho_ModelContours, &ho_ContoursAffinTrans, hvHomMat2D);
		DispObj(ho_ContoursAffinTrans,m_hv_WindowID);
	}

	try
	{
		m_strMarkName.Format("%s",tMarkDisp.hv_ModelParam[0].S().Text());
		m_strMarkType.Format("%s",tMarkDisp.hv_ModelParam[1].S().Text());
		m_strDrawType = "����";
		m_nSearchCoefficient_Column = tMarkDisp.hv_ModelParam[2];
		m_EDT_dMiniScore            = tMarkDisp.hv_ModelParam[3];
		m_COM_nType                 =tMarkDisp.hv_ModelParam[4];
		m_hoCurrentDispImage=ho_Image;
	}catch(HException e)
	{

	}

	UpdateData(FALSE);
}
void CDlgParamSet::DispImage(const HObject& hoImage)
{

	try
	{
		if(NULL == m_hv_WindowID)
		{	
			CWnd *pWnd;	
			CRect rect;
			pWnd = GetDlgItem(IDC_STATIC_PIC);
			pWnd->GetWindowRect(rect);	
			SetCheck("~father");
			OpenWindow(0, 0, rect.Width(), rect.Height(),(Hlong)pWnd->m_hWnd, "visible", "", &m_hv_WindowID);
			SetDraw(m_hv_WindowID,"margin");
			SetCheck("father");
		}

		HTuple tmpWidth;
		HTuple tmpHeight;
		GetImageSize(hoImage, &tmpWidth, &tmpHeight);
		if (tmpWidth != m_hvDispImageWidth || tmpHeight != m_hvDispImageHeight)
		{		
			m_dDispImagePartRow0 = 0;
			m_dDispImagePartCol0 = 0;
			m_dDispImagePartRow1 = tmpHeight - 1; 
			m_dDispImagePartCol1 = tmpWidth - 1;	

			m_hvDispImageWidth = tmpWidth;
			m_hvDispImageHeight = tmpHeight;
		}		
		SetPart(m_hv_WindowID, m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1, m_dDispImagePartCol1);
		SetSystem("flush_graphic","false");
		ClearWindow(m_hv_WindowID);
		SetSystem("flush_graphic","true");
		DispObj(hoImage, m_hv_WindowID);	
		if (m_hoDispRegion.IsInitialized())
		{
			DispObj(m_hoDispRegion,m_hv_WindowID);
		}
	}catch(HException)
	{
		ClearWindow(m_hv_WindowID);
	}


}

BOOL CDlgParamSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE,TRUE);
	try
	{
		DispImage(m_hoCurrentDispImage);
		m_hoImage=m_hoCurrentDispImage;//����
	}catch(HException)
	{
	}
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CDlgParamSet::SetOwner(CFindMark* pOwner)
{
	/*m_pOwner = pOwner;*/
	m_pRealDetect=pOwner;
}


void CDlgParamSet::InitUI()
{
	UpdateData(FALSE);
	((CComboBox*)GetDlgItem(IDC_COMBO_CREATETYPE))->SetCurSel(1);
	((CComboBox*)GetDlgItem(IDC_COMBO_DRAWTYPE))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(m_pOwner->m_nType);
	GetDlgItem(IDC_EDIT_CREATEMARKNAME)->SetWindowTextA("����");
	//m_ftButton.CreatePointFont(100,"Microsoft YaHei");
	//���ñ���ɫ ��7����ť

	InitTree();
	
}

BOOL CDlgParamSet::IsDrawing()
{
	return m_bDrawing;
}
void CDlgParamSet::MyFitImage()
{
	if (NULL != m_hv_WindowID)
	{
		CPoint   pt;
		GetCursorPos(&pt);
		GetDlgItem(IDC_STATIC_PIC)->ScreenToClient(&pt);
		CRect WndRect;
		GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&WndRect);

		// ��ԭ��ʾ�������ţ�
		if (WndRect.PtInRect(pt))
		{	
			HTuple tmpWidth;
			HTuple tmpHeight;

			GetImageSize(m_hoCurrentDispImage, &tmpWidth, &tmpHeight);

			m_dDispImagePartRow0 = 0;
			m_dDispImagePartCol0 = 0;
			m_dDispImagePartRow1 = tmpHeight - 1;
			m_dDispImagePartCol1 = tmpWidth - 1;	

			m_hvDispImageWidth = tmpWidth;
			m_hvDispImageHeight = tmpHeight;

			SetPart(m_hv_WindowID, m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1, m_dDispImagePartCol1);

			ClearWindow(m_hv_WindowID);
			DispObj(m_hoCurrentDispImage, m_hv_WindowID);	
		}
	}

}
void CDlgParamSet::MyMouseMove()
{
	CPoint   pt;
	GetCursorPos(&pt);
	GetDlgItem(IDC_STATIC_PIC)->ScreenToClient(&pt);
	CRect WndRect;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&WndRect);

	if (WndRect.PtInRect(pt))
	{
		try
		{
			HTuple hvRow,hvColumn,hvValue;
			GetMposition(m_hv_WindowID,&hvRow,&hvColumn,NULL);
			GetGrayval(m_hoCurrentDispImage,hvRow,hvColumn,&hvValue);
			DispImage(m_hoCurrentDispImage);
			SetTposition(m_hv_WindowID,hvRow,hvColumn+10);


			HTuple Row1,Column1,Row2,Column2;
			GetPart(m_hv_WindowID,&Row1,&Column1,&Row2,&Column2);

			CString strCoordinate;HTuple Message;
			strCoordinate.Format("Row:%.2f",hvRow[0].D());
			Message.Append(HTuple(strCoordinate));
			strCoordinate.Format("Column:%.2f",hvColumn[0].D());
			Message.Append(HTuple(strCoordinate));
			CString strValue;
			strValue.Format("�Ҷ�ֵ:%d",hvValue[0].I());
			Message.Append(HTuple(strValue));
			disp_message(m_hv_WindowID,Message,"image",hvRow,hvColumn,"black","true");




		}catch(HException e)
		{

		}
	}
}
void CDlgParamSet::MyMouseWheel(short zDelta)
{
	if (NULL != m_hv_WindowID)
	{
		CPoint   pt;
		GetCursorPos(&pt);
		GetDlgItem(IDC_STATIC_PIC)->ScreenToClient(&pt);
		CRect WndRect;
		GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&WndRect);
		if (WndRect.PtInRect(pt))
		{			
			double	ImagePtX,ImagePtY;
			double	Row0_1,Col0_1,Row1_1,Col1_1;
			double	Scale=0.1;

			if (zDelta>0)
			{
				ImagePtX=m_dDispImagePartCol0+(pt.x-WndRect.left)/(WndRect.Width()-1.0)*(m_dDispImagePartCol1-m_dDispImagePartCol0);
				ImagePtY=m_dDispImagePartRow0+(pt.y-WndRect.top)/(WndRect.Height()-1.0)*(m_dDispImagePartRow1-m_dDispImagePartRow0);
				Row0_1=ImagePtY-1/(1-Scale)*(ImagePtY-m_dDispImagePartRow0);
				Row1_1=ImagePtY-1/(1-Scale)*(ImagePtY-m_dDispImagePartRow1);
				Col0_1=ImagePtX-1/(1-Scale)*(ImagePtX-m_dDispImagePartCol0);
				Col1_1=ImagePtX-1/(1-Scale)*(ImagePtX-m_dDispImagePartCol1);
				m_dDispImagePartRow0=Row0_1;
				m_dDispImagePartCol0=Col0_1;
				m_dDispImagePartRow1=Row1_1;
				m_dDispImagePartCol1=Col1_1;
			}
			else
			{
				ImagePtX=m_dDispImagePartCol0+(pt.x-WndRect.left)/(WndRect.Width()-1.0)*(m_dDispImagePartCol1-m_dDispImagePartCol0);
				ImagePtY=m_dDispImagePartRow0+(pt.y-WndRect.top)/(WndRect.Height()-1.0)*(m_dDispImagePartRow1-m_dDispImagePartRow0);
				Row0_1=ImagePtY-1/(1+Scale)*(ImagePtY-m_dDispImagePartRow0);
				Row1_1=ImagePtY-1/(1+Scale)*(ImagePtY-m_dDispImagePartRow1);
				Col0_1=ImagePtX-1/(1+Scale)*(ImagePtX-m_dDispImagePartCol0);
				Col1_1=ImagePtX-1/(1+Scale)*(ImagePtX-m_dDispImagePartCol1);
				m_dDispImagePartRow0=Row0_1;
				m_dDispImagePartCol0=Col0_1;
				m_dDispImagePartRow1=Row1_1;
				m_dDispImagePartCol1=Col1_1;
			}

			DispImage(m_hoCurrentDispImage);
		}
	}
}
void CDlgParamSet::InitTree()
{
	m_MarkTree.DeleteAllItems();
	m_TreeImages.DeleteImageList();
	m_TreeImages.Create(32,32,ILC_COLOR32|ILC_MASK,0,0);
	m_TreeImages.Add(AfxGetApp()->LoadIconA(IDI_ICON_MARK));
	m_MarkTree.SetImageList(&m_TreeImages, LVSIL_NORMAL);
	int MarkCount = m_pOwner->m_tMarkList.size(); 
	CString strInsertName="";  
	for(int i=0;i<MarkCount;i++)
	{
		strInsertName.Format("%s",m_pOwner->m_tMarkList[i].hv_ModelParam[0].S().Text());
		m_MarkTree.InsertItem(strInsertName,0,0);  
	}
}
void CDlgParamSet::SetButton(CButtonExt& tButton)
{

	/*short	shBtnColor = m_tBlendent.m_Blendent.Button.shOffset;
	COLORREF BKOUT = m_tBlendent.m_Blendent.Button.BKColorOut;
	COLORREF BKIN = m_tBlendent.m_Blendent.Button.BKColorIn;
	COLORREF BKFOCUS = m_tBlendent.m_Blendent.Button.BKColorFocus;
	COLORREF FG = m_tBlendent.m_Blendent.Button.FGColorOut;


	tButton.OffsetColor(CButtonExt::BTNST_COLOR_BK_IN, shBtnColor);
	tButton.SetColor(CButtonExt::BTNST_COLOR_BK_IN, BKIN);
	tButton.SetColor(CButtonExt::BTNST_COLOR_BK_OUT, BKOUT);
	tButton.SetColor(CButtonExt::BTNST_COLOR_BK_FOCUS, BKFOCUS);
	tButton.SetColor(CButtonExt::BTNST_COLOR_FG_IN, FG);
	tButton.SetColor(CButtonExt::BTNST_COLOR_FG_OUT, FG);
	tButton.SetColor(CButtonExt::BTNST_COLOR_FG_FOCUS, FG);
	tButton.SetFont(&m_ftButton);*/

}
BOOL CDlgParamSet::Create_ShapeModel(HObject& ho_ImageReduced)
{
	HObject ho_Domain,ho_Search;
	GetDomain(ho_ImageReduced,&ho_Domain);
	DilationRectangle1(ho_Domain,&ho_Search,m_nSearchCoefficient_Column,m_nSearchCoefficient_Column);
	SINGLEMARKINFO tMarkAdd;
	tMarkAdd.hv_ModelParam.Append(HTuple(m_strMarkName));
	tMarkAdd.hv_ModelParam.Append(HTuple(m_strMarkType));
	tMarkAdd.hv_ModelParam.Append(HTuple(m_nSearchCoefficient_Column));
	tMarkAdd.hv_ModelParam.Append(HTuple(m_EDT_dMiniScore));
	tMarkAdd.hv_ModelParam.Append(HTuple(m_COM_nType));
	try
	{
		CreateShapeModel(ho_ImageReduced, "auto", HTuple(-15).TupleRad(), HTuple(15).TupleRad(), "auto", "auto", "use_polarity", 
			"auto", "auto", &tMarkAdd.hv_ModelID);
		ConcatObj(m_hoCurrentDispImage,ho_Domain,&tMarkAdd.ho_ModleInfo);
		ConcatObj(tMarkAdd.ho_ModleInfo,ho_Search,&tMarkAdd.ho_ModleInfo);


		HTuple hv_Row1,hv_Column1,hv_Row2,hv_Column2;
		SmallestRectangle1(ho_Domain,&hv_Row1,&hv_Column1,&hv_Row2,&hv_Column2);
		GenRegionLine(&tMarkAdd.ho_Line,(hv_Row2+hv_Row1)/2,hv_Column1,(hv_Row2+hv_Row1)/2,hv_Column2);
		m_pOwner->m_tMarkList.push_back(tMarkAdd);
	}catch(HException e)
	{
		BCGPMessageBox("�����״ģ��ʧ��");
		return FALSE;
	}
	return TRUE;

}

void CDlgParamSet::SetButtonStaus(BOOL bStaus)
{
	HWND  hwndChild=::GetWindow(GetSafeHwnd(),GW_CHILD);  //�г����пؼ� 
	CWnd* pWnd = CWnd::FromHandle(GetSafeHwnd());
	int woc; 
	CRect Rect;
	while(hwndChild)      
	{      
		woc=::GetDlgCtrlID(hwndChild);//ȡ��ID  
		switch (woc)
		{
		case IDC_STATIC_PIC:
			hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);
			continue;
		}
		pWnd->GetDlgItem(woc)->EnableWindow(bStaus);
		hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);      
	}
}
BOOL CDlgParamSet::Create_NCCModel(HObject& ho_ImageReduced)
{
	HObject ho_Domain,ho_Search;
	GetDomain(ho_ImageReduced,&ho_Domain);
	DilationRectangle1(ho_Domain,&ho_Search,m_nSearchCoefficient_Column,m_nSearchCoefficient_Column);
	SINGLEMARKINFO tMarkAdd;
	tMarkAdd.hv_ModelParam.Append(HTuple(m_strMarkName));//ģ������
	tMarkAdd.hv_ModelParam.Append(HTuple(m_strMarkType));//�Ҷ�ƥ�仹����״ƥ��
	tMarkAdd.hv_ModelParam.Append(HTuple(m_nSearchCoefficient_Column));//�п�
	tMarkAdd.hv_ModelParam.Append(HTuple(m_EDT_dMiniScore));//��Сƥ����
	tMarkAdd.hv_ModelParam.Append(HTuple(m_COM_nType));//��λ�����Ǽ�������R��
	try
	{
		CreateNccModel(ho_ImageReduced,"auto", HTuple(-15).TupleRad(), HTuple(15).TupleRad(), "auto", "use_polarity", &tMarkAdd.hv_ModelID);
		ConcatObj(m_hoCurrentDispImage,ho_Domain,&tMarkAdd.ho_ModleInfo);
		ConcatObj(tMarkAdd.ho_ModleInfo,ho_Search,&tMarkAdd.ho_ModleInfo);
		HTuple hv_Row1,hv_Column1,hv_Row2,hv_Column2;
		SmallestRectangle1(ho_Domain,&hv_Row1,&hv_Column1,&hv_Row2,&hv_Column2);
		GenRegionLine(&tMarkAdd.ho_Line,(hv_Row2+hv_Row1)/2,hv_Column1,(hv_Row2+hv_Row1)/2,hv_Column2);
		m_pOwner->m_tMarkList.push_back(tMarkAdd);
	}catch(HException e)
	{
		BCGPMessageBox("��ӻҶ�ģ��ʧ��");
		return FALSE;
	}
	return TRUE;
}
void CDlgParamSet::Draw(CString strDrawType,HObject* Region)
{
	GenEmptyObj(Region);
	HTuple tmpButton = 1;
	//while(1 == tmpButton)
	{
		try
		{
			SetDraw(m_hv_WindowID,"margin");
			HObject tmpRegion;
			HTuple tmpRow, tmpColumn;
			DispImage(m_hoCurrentDispImage);
			SetColor(m_hv_WindowID, "green");
			SetLineWidth(m_hv_WindowID, 2);
			DispObj(*Region, m_hv_WindowID);

			SetTposition(m_hv_WindowID, m_dDispImagePartRow0, m_dDispImagePartCol0);
			SetColor(m_hv_WindowID, "blue");
			SetLineWidth(m_hv_WindowID, 2);
			WriteString(m_hv_WindowID, "�뻭���򣬵��Ҽ�ȷ�ϡ�");

			SetColor(m_hv_WindowID, "yellow");
			SetLineWidth(m_hv_WindowID, 1);
			if ("����" == strDrawType)
			{
				HTuple Row1, Column1, Row2, Column2;
				DrawRectangle1(m_hv_WindowID, &Row1, &Column1, &Row2, &Column2);
				GenRectangle1(&tmpRegion,Row1, Column1, Row2, Column2);
			}else if ("���������" == strDrawType)
			{
				HTuple Row, Column, Phi, Length1, Length2;
				DrawRectangle2(m_hv_WindowID, &Row, &Column, &Phi, &Length1, &Length2);
				GenRectangle2(&tmpRegion,Row, Column, Phi, Length1, Length2);
			}else if ("Բ��" == strDrawType)
			{
				HTuple Row, Column, Radius;
				DrawCircle(m_hv_WindowID, &Row, &Column, &Radius);
				GenCircle(&tmpRegion,Row, Column, Radius);
			}else if ("��Բ" == strDrawType)
			{
				HTuple Row, Column, Phi, Radius1, Radius2;
				DrawEllipse(m_hv_WindowID, &Row, &Column, &Phi, &Radius1, &Radius2);
				GenEllipse(&tmpRegion, Row, Column, Phi, Radius1, Radius2);
			}else if ("�����" == strDrawType)
			{
				DrawPolygon(&tmpRegion,m_hv_WindowID);
				FillUp(tmpRegion,&tmpRegion);
			}else if ("������״" == strDrawType)
			{
				DrawRegion(&tmpRegion, m_hv_WindowID);
			}


			Union2(*Region, tmpRegion, Region);

			DispImage(m_hoCurrentDispImage);
			SetColor(m_hv_WindowID, "green");
			SetLineWidth(m_hv_WindowID, 2);
			DispObj(*Region, m_hv_WindowID);

			SetTposition(m_hv_WindowID, m_dDispImagePartRow0, m_dDispImagePartCol0);
			SetColor(m_hv_WindowID, "red");
			SetLineWidth(m_hv_WindowID, 2);
		 
		//	WriteString(m_hv_WindowID, "����������... / �Ҽ�������");
	//		GetMbutton(m_hv_WindowID, &tmpRow, &tmpColumn, &tmpButton);		

		}catch(HException &e)
		{
			BCGPMessageBox(e.ErrorMessage().Text());
		}

	}	
}
void CDlgParamSet::OnBnClickedButtonAdd()
{
	SetButtonStaus(FALSE);
	UpdateData(TRUE);
	for(int i=0;i<m_pOwner->m_tMarkList.size();i++)
	{
		CString strInsertName;
		strInsertName.Format("%s",m_pOwner->m_tMarkList[i].hv_ModelParam[0].S().Text());
		if (m_pOwner->m_tMarkList.size()>2)
		{
			BCGPMessageBox("ƴͼģ��������Ŀ��3������ɾ������ģ����ٽ��л��ƣ�");
			SetButtonStaus(TRUE);
			return;
		}
		if(strInsertName ==  m_strMarkName)
		{
			BCGPMessageBox("���������������ͬ��ƴͼ��λMark");
			SetButtonStaus(TRUE);
			return;
		}
	}
	m_bDrawing = TRUE;
	BOOL bReturn;;
	/////////////////��ģ������
	HObject ho_Region;
	Draw(m_strDrawType,&ho_Region);
	HObject ho_ImageReduced;
	ReduceDomain(m_hoCurrentDispImage,ho_Region,&ho_ImageReduced);
	try
	{
		if ("��״ƥ��" == m_strMarkType)
		{
			bReturn = Create_ShapeModel(ho_ImageReduced);
		}else if ("�Ҷ�ƥ��" == m_strMarkType)
		{
			bReturn = Create_NCCModel(ho_ImageReduced);
		}
	}catch(HException)
	{
		bReturn = FALSE;
	}
	if (FALSE == bReturn)
	{
		BCGPMessageBox("ģ�����򻭵ò��ã������´�����");
		m_bDrawing = FALSE;
		SetButtonStaus(TRUE);
		return;
	}
	InitTree();
	/*m_pOwner->WriteMark();*/
	m_bDrawing = FALSE;
	DispMark(m_pOwner->m_tMarkList[m_pOwner->m_tMarkList.size()-1]);
	BCGPMessageBox("�µ�ƴͼ��λMark��������");
	SetButtonStaus(TRUE);
}


BOOL CDlgParamSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
		{
			return TRUE;
		}else if (pMsg->wParam == VK_ESCAPE)//����ESC
		{
			return TRUE;
		}else if(pMsg->wParam == VK_SPACE)//
		{
			return TRUE;
		}else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//����ALT+F4
		{
			return TRUE;
		}
		else if (pMsg->message == WM_MBUTTONDOWN)
		{
			MyFitImage();
			return TRUE;
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CDlgParamSet::OnSetFocus(CWnd* pOldWnd)
{
	CBCGPDialog::OnSetFocus(pOldWnd);
	// TODO: �ڴ˴������Ϣ����������
}




void CDlgParamSet::OnBnClickedButtonOpenimage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilter[] = _T("�����ļ�(*.*)|*.*||"); 
	CFileDialog FileDlg(TRUE, _T("Image"), NULL, 0, szFilter, this); 
	if(IDOK == FileDlg.DoModal())
	{
		CString lpstrFilename = FileDlg.GetPathName(); 
		try
		{
			ReadImage(&m_hoCurrentDispImage,HTuple(lpstrFilename));
			DispImage(m_hoCurrentDispImage);
		}catch(HException )
		{
			BCGPMessageBox("���ļ�ʧ��");
		}
	}
}


void CDlgParamSet::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	//��ʾѡ�����Ƶ�Mark
	HTREEITEM SelectItem = m_MarkTree.GetSelectedItem();

	static bool bClicked=FALSE;
	if (FALSE==bClicked)//�鿴��ǰģ��
	{
		for(int i=0;i<m_pOwner->m_tMarkList.size();i++)
		{
			CString strInsertName;
			strInsertName.Format("%s",m_pOwner->m_tMarkList[i].hv_ModelParam[0].S().Text());
			if(strInsertName ==  m_MarkTree.GetItemText(SelectItem))
			{
				DispMark(m_pOwner->m_tMarkList[i]);
				break;
			}
		}
		bClicked=TRUE;
	}else
	{
		m_hoCurrentDispImage=m_hoImage;//��ʾ��ģ��ͼ��
		DispImage(m_hoCurrentDispImage);
		bClicked=FALSE;
	}
	
}


void CDlgParamSet::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint  RClickPoint; 
	GetCursorPos(&RClickPoint); //��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����  
	m_MarkTree.ScreenToClient(&RClickPoint);  //m_TREE��CTreeCtrl�ؼ�������
	HTREEITEM  SelItem = m_MarkTree.HitTest(RClickPoint,NULL); //��ȡ��ǰ����Ҽ�������λ���µ�item
	m_MarkTree.SelectItem(SelItem);

	m_MarkTree.ClientToScreen(&RClickPoint);
	CMenu menu;//��������Ҫ�õ���cmenu����  
	menu.LoadMenu(IDR_MENU_RBUTTON_MARKTREE);//װ���Զ�����Ҽ��˵�  
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN,RClickPoint.x,RClickPoint.y,this);  //��ָ��λ����ʾ�����˵�
	*pResult = 0;
}


void CDlgParamSet::OnMenuDelete()
{
	HTREEITEM SelectItem = m_MarkTree.GetSelectedItem();
	std::vector<SINGLEMARKINFO>::iterator it;
	for(it=m_pOwner->m_tMarkList.begin();it!=m_pOwner->m_tMarkList.end();it++)
	{
		CString strInsertName;
		strInsertName.Format("%s",it->hv_ModelParam[0].S().Text());
		if(strInsertName ==  m_MarkTree.GetItemText(SelectItem))
		{
			m_pOwner->m_tMarkList.erase(it);
			break;
		}
	}
	InitTree();
	//m_pOwner->WriteMark();
}


void CDlgParamSet::OnMenuRename()
{
	// TODO: �ڴ���������������
}


void CDlgParamSet::OnBnClickedButtonCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CBCGPDialog::OnCancel();
}


void CDlgParamSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CBCGPDialog::OnPaint()
	//CRect   rcDlg,rcTitle,rcClient;  
	//GetWindowRect(rcDlg);
	//ScreenToClient(rcDlg);

	////���߿���
	//CPen pen;
	//pen.CreatePen(PS_SOLID,1,RGB(200,200,200));//154,222,106
	//dc.SelectObject(pen);
	//dc.MoveTo(rcDlg.left,rcDlg.top);
	//dc.LineTo(rcDlg.right-1,rcDlg.top);
	//dc.LineTo(rcDlg.right-1,rcDlg.bottom-1);
	//dc.LineTo(rcDlg.left,rcDlg.bottom-1);
	//dc.LineTo(rcDlg.left,rcDlg.top);
}


HBRUSH CDlgParamSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	/*if(nCtlColor==CTLCOLOR_STATIC) 
	{ 
		pDC-> SetBkMode(TRANSPARENT); 
		return (HBRUSH)::GetStockObject(NULL_BRUSH); 
	} */

	return hbr;
}


void CDlgParamSet::OnBnClickedButtonAddline()
{
	UpdateData(TRUE);
	HTREEITEM SelectItem = m_MarkTree.GetSelectedItem();
	std::vector<SINGLEMARKINFO>::iterator it;
	for(it=m_pOwner->m_tMarkList.begin();it!=m_pOwner->m_tMarkList.end();it++)
	{
		CString strInsertName;
		strInsertName.Format("%s",it->hv_ModelParam[0].S().Text());
		if(strInsertName ==  m_MarkTree.GetItemText(SelectItem))//�޸��ߵ�λ��
		{
			m_bDrawing = TRUE;
			/////////////////��ģ������
			HObject ho_Region;
			Draw(m_strDrawType,&ho_Region);
			HTuple hv_Row1,hv_Column1,hv_Row2,hv_Column2;
			SmallestRectangle1(ho_Region,&hv_Row1,&hv_Column1,&hv_Row2,&hv_Column2);
			GenRegionLine(&it->ho_Line,(hv_Row2+hv_Row1)/2,hv_Column1,(hv_Row2+hv_Row1)/2,hv_Column2);
			m_pOwner->WriteMark();
			m_bDrawing = FALSE;

			DispMark(*it);
			BCGPMessageBox("׷�����޸����");
			break;
		}
	}


}


void CDlgParamSet::OnBnClickedButtonTest()
{
	HTuple hv_Start,hv_End;
	CountSeconds(&hv_Start);
	NS_SCIFindMark::MARKRESULT tMarkResult;
	m_pOwner->DoSomething("FIND",&m_hoCurrentDispImage,&tMarkResult);
	DispImage(m_hoCurrentDispImage);
	SetColor(m_hv_WindowID,"red");
	DispObj(tMarkResult.ho_DispObj,m_hv_WindowID);
	CountSeconds(&hv_End);

	HTuple hv_Message;
	for (int i=0;i<tMarkResult.hv_AllScore.Length();i++)
	{
		CString strScore;
		strScore.Format("%d:ƥ���� %.6f",i+1,tMarkResult.hv_AllScore[i].D());
		hv_Message.Append(HTuple(strScore));
	}
	CString strSpeedTime;
	strSpeedTime.Format("ƥ���ʱ:%05.03f ms",(hv_End.D() - hv_Start.D())*1000);
	hv_Message.Append(HTuple(strSpeedTime));
	disp_message(m_hv_WindowID,hv_Message,"image",m_dDispImagePartRow0, m_dDispImagePartCol0,"red","false");
}


LRESULT CDlgParamSet::OnNcHitTest(CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return CBCGPDialog::OnNcHitTest(point);
}


void CDlgParamSet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CBCGPDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: �ڴ˴������Ϣ����������
	/*if (WA_INACTIVE == nState)  
	{  
		::SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE
			,::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
		this->SetLayeredWindowAttributes(0,(255*85)/100,LWA_ALPHA);
	}else if (WA_CLICKACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE
			,::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}else if (WA_ACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE
			,::GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}*/
}


BOOL CDlgParamSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return CBCGPDialog::OnEraseBkgnd(pDC);
}

void CDlgParamSet::OnCbnSelchangeComboType()
{
	UpdateData(TRUE);
	m_pOwner->m_nType = m_COM_nType;
	WritePrivateProfileInt("Mark","Type",m_pOwner->m_nType,m_pOwner->m_strConfigFile);
}

void CDlgParamSet::OnBnClickedBtnSaveparam()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pRealDetect->DoSomething("COPYOBJECTS",m_pOwner,NULL);
	m_pRealDetect->WriteMark();	
	BCGPMessageBox("����ɹ���");

}


BOOL CDlgParamSet::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MyMouseWheel(zDelta);
	return CBCGPDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CDlgParamSet::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MyFitImage();
	CBCGPDialog::OnMButtonDown(nFlags, point);
}


void CDlgParamSet::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MyMouseMove();
	CBCGPDialog::OnMouseMove(nFlags, point);
}
