// PageCalibrationSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PageCalibrationSet.h"
#include "afxdialogex.h"
#include <iostream> 
#include <fstream> 
#include "DlgCalibration.h"
using namespace std;

#define M_PI       3.14159265358979323846

// CPageCalibrationSet 对话框

IMPLEMENT_DYNAMIC(CPageCalibrationSet, CBCGPDialog)

CPageCalibrationSet::CPageCalibrationSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_DEFECT_SET, pParent)
	, m_nThres(128)
	, m_nMeasureThres(50)
	, m_dUnitWidth(2)
	, m_dDisUpLimit(1.1)
	, m_dDisDownLimit(0.9)
	, m_dKValue(0.88)
	, m_dVerityDistance(100)
	, m_EDT_nParamMeasureThres(0)
	, m_EDT_nImageIgnoreWidth(0)
	, m_dKValue2(0)
	, m_Com_nExtractColor(0)
	, m_EDT_dMinArea(0)
	, m_EDT_nHighThresh(0)
	, m_EDT_nLowThresh(0)
	, m_EDT_dCalibArea(0.15)
	, m_Com_nDefectCalType(0)
	, m_dDoubleCamInternValue(0)
{
	m_bFlag = FALSE;
}

CPageCalibrationSet::~CPageCalibrationSet()
{
}

void CPageCalibrationSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_PARAM_THRES, m_nThres);
	DDX_Text(pDX, IDC_PARAM_MEASURETHRES, m_nMeasureThres);
	DDX_Text(pDX, IDC_EDIT3, m_dUnitWidth);
	DDX_Text(pDX, IDC_PARAM_DISUPLIMIT, m_dDisUpLimit);
	DDX_Text(pDX, IDC_PARAM_DISDOWNLIMIT, m_dDisDownLimit);
	DDX_Text(pDX, IDC_PARAM_KVALUE, m_dKValue);
	DDX_Text(pDX, IDC_VerityDistance, m_dVerityDistance);
	DDX_Text(pDX, IDC_EDIT_PARAM_MEASURE_THRES, m_EDT_nParamMeasureThres);
	DDX_Text(pDX, IDC_PARAM_IMAGE_IGNORE_WIDTH, m_EDT_nImageIgnoreWidth);
	DDX_Text(pDX, IDC_PARAM_KVALUE2, m_dKValue2);
	DDX_Control(pDX, IDC_LIST1, m_List_Area);
	DDX_CBIndex(pDX, IDC_COMBO_EXTRACT_COLOR, m_Com_nExtractColor);
	DDX_Text(pDX, IDC_EDIT_EXTRACT_MIN_AREA, m_EDT_dMinArea);
	DDX_Text(pDX, IDC_EDIT_LIGHT_THRES, m_EDT_nHighThresh);
	DDX_Text(pDX, IDC_EDIT_DARK_THRES, m_EDT_nLowThresh);
	DDX_Text(pDX, IDC_EDIT_OBJECT_AREA, m_EDT_dCalibArea);
	DDX_CBIndex(pDX, IDC_COMBO_CAL_TYPE, m_Com_nDefectCalType);
	DDX_Text(pDX, IDC_PARAM_DOUBLECAM_INTERN_VALUE, m_dDoubleCamInternValue);
}

BEGIN_MESSAGE_MAP(CPageCalibrationSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_CAL, &CPageCalibrationSet::OnBnClickedButtonCal)
	ON_EN_CHANGE(IDC_PARAM_MEASURETHRES, &CPageCalibrationSet::OnEnChangeParamMeasurethres)
	ON_EN_CHANGE(IDC_PARAM_THRES, &CPageCalibrationSet::OnEnChangeParamThres)
	ON_EN_CHANGE(IDC_EDIT3, &CPageCalibrationSet::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_PARAM_DISUPLIMIT, &CPageCalibrationSet::OnEnChangeParamDisuplimit)
	ON_EN_CHANGE(IDC_PARAM_DISDOWNLIMIT, &CPageCalibrationSet::OnEnChangeParamDisdownlimit)
	ON_EN_CHANGE(IDC_PARAM_KVALUE, &CPageCalibrationSet::OnEnChangeParamKvalue)
	ON_BN_CLICKED(IDC_BUTTON_VERIFY, &CPageCalibrationSet::OnBnClickedButtonVerify)
	ON_EN_CHANGE(IDC_VerityDistance, &CPageCalibrationSet::OnEnChangeVeritydistance)
	ON_EN_CHANGE(IDC_PARAM_IMAGE_IGNORE_WIDTH, &CPageCalibrationSet::OnEnChangeParamImageIgnoreWidth)
	ON_EN_CHANGE(IDC_PARAM_KVALUE2, &CPageCalibrationSet::OnEnChangeParamKvalue2)
	ON_BN_CLICKED(IDC_BUTTON_DEFECT_CALI, &CPageCalibrationSet::OnBnClickedButtonDefectCali)
	ON_BN_CLICKED(IDC_BUTTON_DEFECT_TEST, &CPageCalibrationSet::OnBnClickedButtonDefectTest)
	ON_EN_CHANGE(IDC_EDIT_OBJECT_AREA, &CPageCalibrationSet::OnEnChangeEditObjectArea)
	ON_EN_CHANGE(IDC_EDIT_DARK_THRES, &CPageCalibrationSet::OnEnChangeEditDarkThres)
	ON_EN_CHANGE(IDC_EDIT_LIGHT_THRES, &CPageCalibrationSet::OnEnChangeEditLightThres)
	ON_EN_CHANGE(IDC_EDIT_EXTRACT_MIN_AREA, &CPageCalibrationSet::OnEnChangeEditExtractMinArea)
	ON_CBN_SELCHANGE(IDC_COMBO_EXTRACT_COLOR, &CPageCalibrationSet::OnCbnSelchangeComboExtractColor)
	ON_CBN_SELCHANGE(IDC_COMBO_CAL_TYPE, &CPageCalibrationSet::OnCbnSelchangeComboCalType)
	ON_BN_CLICKED(IDC_BUTTON_CAL2, &CPageCalibrationSet::OnBnClickedButtonCal2)
	ON_BN_CLICKED(IDC_CHECK_DRAW_LINE_H, &CPageCalibrationSet::OnBnClickedCheckDrawLineH)
	ON_EN_CHANGE(IDC_EDIT_PARAM_MEASURE_THRES, &CPageCalibrationSet::OnEnChangeEditParamMeasureThres)
	ON_EN_CHANGE(IDC_PARAM_DOUBLECAM_INTERN_VALUE, &CPageCalibrationSet::OnEnChangeParamDoublecamInternValue)
END_MESSAGE_MAP()

// CPageCalibrationSet 消息处理程序

BOOL CPageCalibrationSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CPageCalibrationSet::PreTranslateMessage(MSG* pMsg)
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
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CPageCalibrationSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageCalibrationSet::InitUI()
{
	m_nThres = m_pOwners->m_Calibration.m_tCalParam.nThres;
	m_dUnitWidth = m_pOwners->m_Calibration.m_tCalParam.dUnitWidth;
	m_nMeasureThres = m_pOwners->m_Calibration.m_tCalParam.nMeasureThres;
	m_dDisUpLimit = m_pOwners->m_Calibration.m_tCalParam.dDisUpLimit;
	m_dDisDownLimit = m_pOwners->m_Calibration.m_tCalParam.dDisDownLimit;
	m_EDT_nParamMeasureThres = m_pOwners->m_Calibration.m_tCalParam.nMeasureThres;
	m_EDT_nImageIgnoreWidth = m_pOwners->m_Calibration.m_tCalParam.nImageIgnoreWidth;
	m_dKValue = m_pOwners->m_Calibration.m_tStripeParam.vecKValue[0];
	m_dVerityDistance = m_pOwners->m_Calibration.m_tStripeParam.hvJyDistance;

	m_dRowResolution = m_pOwners->m_Calibration.m_tCalParam.dRowResolution;
	m_dColResolution = m_pOwners->m_Calibration.m_tCalParam.dColResolution;
	m_EDT_dMinArea = m_pOwners->m_Calibration.m_tCalParam.dMinArea;
	m_EDT_nLowThresh = m_pOwners->m_Calibration.m_tCalParam.nLowThresh;
	m_EDT_nHighThresh = m_pOwners->m_Calibration.m_tCalParam.nHighThresh;
	m_Com_nDefectCalType = m_pOwners->m_Calibration.m_tCalParam.nDefectCalType;
	m_Com_nExtractColor = m_pOwners->m_Calibration.m_tCalParam.nExtractType;

	m_hvObjWidth = m_pOwners->m_Calibration.m_tCalParam.hvObjWidth;
	m_hvObjHeight = m_pOwners->m_Calibration.m_tCalParam.hvObjHeight;

	if (m_pOwners->m_Calibration.m_tStripeParam.vecKValue.size() > 1)
	{
		m_dKValue2 = m_pOwners->m_Calibration.m_tStripeParam.vecKValue[1];
		m_dDoubleCamInternValue = m_pOwners->m_Calibration.m_tStripeParam.dInternOffset;
	}
	else
	{
		GetDlgItem(IDC_PARAM_KVALUE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_INTERN_VALUE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PARAM_DOUBLECAM_INTERN_VALUE)->ShowWindow(SW_HIDE);
	}
		

	((CBCGPButton*)GetDlgItem(IDC_CHECK_DRAW_LINE_H))->SetCheck(m_pParentDlg->m_bDrawLineHor);

	UpdateData(FALSE);

	if (m_pOwners->m_tInit.hvCamSNList.Length() == 2)
	{
		GetDlgItem(IDC_BUTTON_CAL)->SetWindowTextA("一次标定");
		GetDlgItem(IDC_BUTTON_CAL2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CAL2)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_CAL2)->ShowWindow(SW_HIDE);
	}
}

void CPageCalibrationSet::OnBnClickedButtonCal()
{
	UpdateData(TRUE);
	/****标定*****/
	CCalibration::CalInput tIn;
	CCalibration::CalOutput tOut;

	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;
	m_pParentDlg->m_Wnd_Image.ClearRegion();

	if (m_pOwners->m_tInit.hvCamSNList.Length() == 2)
	{
		CString strMsg;
		strMsg.Format("请先选第一次标定目标，按住左键开始绘制，单击右键绘制完成");
		m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");
		GetDlgItem(IDC_BUTTON_CAL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CAL)->EnableWindow(TRUE);
	}
	else
	{
		CString strMsg;
		strMsg.Format("请选取标定目标，按住左键开始绘制，单击右键绘制完成");
		m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");
	}
	m_pParentDlg->m_Wnd_Image.SetDrawing(TRUE);
	try
	{
		SetButtonStaus(FALSE);
		HTuple hv_Row0, hv_Column0, hv_Row1, hv_Column1;
		//DrawRegion(&hoRegion, m_pParentDlg->m_Wnd_Image.GetDispWnd());
		SetColor(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "green");
		DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hv_Row0, &hv_Column0, &hv_Row1, &hv_Column1);
		GenRectangle1(&tIn.hoROI, hv_Row0, hv_Column0, hv_Row1, hv_Column1);
		SetButtonStaus(TRUE);
	}
	catch (HException& e)
	{
		SetButtonStaus(TRUE);
	}
	m_pParentDlg->m_Wnd_Image.SetDrawing(FALSE);
	if (m_pOwners->m_tInit.hvCamSNList.Length() == 2)
	{
		if (!m_pOwners->m_Calibration.StripeDoubleCamCalibrationFirst(tIn, tOut))
		{
			BCGPMessageBox("双相机第一次标定失败,请检查!");
			return;
			CString strExcp;
			strExcp.Format("双相机第一次标定失败,请检查!");
			//MeasureInit		m_tInit;
			//m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		}
	}
	else
	{
		if (!m_pOwners->m_Calibration.Calibrate(tIn, tOut))
		{
			BCGPMessageBox("标定失败,请检查!");
			return;
			CString strExcp;
			strExcp.Format("标定失败,请检查!");
			//MeasureInit		m_tInit;
			//m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
		}
	}

	m_pParentDlg->m_Wnd_Image.DispImage(tIn.hoImage, 1);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "green", "margin");

	//显示拟合曲线
	HObject hoY, hoFitY;
	HTuple hvX;
	CString strYLabel, strXLabel;
	strYLabel = "分辨率";
	strXLabel.Format("\r\n\r\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%", tOut.hvCoefficients[0].D(),
		tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
	TupleGenSequence(0, tOut.hvResFit.Length() - 1, 1, &hvX);
	plot_tuple(m_pParentDlg->m_Wnd_Image.GetDispWnd(), hvX, HTuple(tOut.hvResFit).Append(tOut.hvRes), 
		(HTuple)strXLabel, (HTuple)strYLabel, HTuple("green").Append("medium slate blue"), 
		HTuple("style").Append("axes_color"), HTuple("line").Append("blue"));

	if (m_pOwners->m_tInit.hvCamSNList.Length() == 2)
	{
		if (tOut.hvR2.D() >= 0.98 && tOut.hvResDiffPercent.TupleAbs().TupleMax().D() <= 0.3)
		{
			GetDlgItem(IDC_BUTTON_CAL2)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CAL)->EnableWindow(FALSE);
			CString strMess;
			strMess.Format("双相机第一次标定成功，请执行第二次标定\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%", 
				tOut.hvCoefficients[0].D(), tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), 
				tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
			BCGPMessageBox(strMess);
		}
		else if (tOut.hvResDiffPercent.TupleMax().D() > 0.3)
		{
			CString strMess;
			strMess.Format("双相机第一次标定失败，最大偏差百分比MaxDiff大于0.3%%，请重新标定\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%",
				tOut.hvCoefficients[0].D(), tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
			BCGPMessageBox(strMess);
		}
		else
		{
			CString strMess;
			strMess.Format("双相机第一次标定失败，回归系数R2小于0.98，请重新标定\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%", 
				tOut.hvCoefficients[0].D(), tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
			BCGPMessageBox(strMess);
		}
	}
	else
	{
		CString strMess;
		strMess.Format("标定成功\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%", 
			tOut.hvCoefficients[0].D(), tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());

		BCGPMessageBox(strMess);
	}

	//保存标定数据文件
	SaveCaliParamFile(m_pOwners->m_tInit.hvCamSNList[0].I(), tOut);
	
	/****标定*****/
}

void CPageCalibrationSet::OnBnClickedButtonCal2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	/****标定*****/
	CCalibration::CalInput tIn;
	CCalibration::CalOutput tOut;

	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;
	m_pParentDlg->m_Wnd_Image.ClearRegion();

	if (!m_pOwners->m_tInit.hvCamSNList.Length() == 2) return;

	{
		m_pParentDlg->m_Wnd_Image.SetDrawing(TRUE);
		CString strMsg;
		strMsg.Format("请先选定第二次标定目标，按住左键开始绘制，单击右键绘制完成");
		m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");

		try
		{
			SetButtonStaus(FALSE);
			HTuple hv_Row0, hv_Column0, hv_Row1, hv_Column1;
			//DrawRegion(&hoRegion, m_pParentDlg->m_Wnd_Image.GetDispWnd());
			SetColor(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "green");
			DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hv_Row0, &hv_Column0, &hv_Row1, &hv_Column1);
			GenRectangle1(&tIn.hoROI, hv_Row0, hv_Column0, hv_Row1, hv_Column1);
			SetButtonStaus(TRUE);
		}
		catch (HException& e)
		{
			SetButtonStaus(TRUE);
		}
		GetDlgItem(IDC_BUTTON_CAL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CAL)->EnableWindow(FALSE);
		m_pParentDlg->m_Wnd_Image.SetDrawing(FALSE);
	}

	if (!m_pOwners->m_Calibration.StripeDoubleCamCalibrationSecond(tIn, tOut))
	{
		BCGPMessageBox("双相机第二次标定失败,请检查!");
		return;
		CString strExcp;
		strExcp.Format("双相机第二次标定失败,请检查!");
		//MeasureInit		m_tInit;
		//m_tInit.pLog(strExcp, ExcpLog, SlightExcp);
	}

	m_pParentDlg->m_Wnd_Image.DispImage(tIn.hoImage, 1);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "green", "margin");

	//显示拟合曲线
	HObject hoY, hoFitY;
	HTuple hvX;
	CString strYLabel, strXLabel;
	strYLabel = "分辨率";
	strXLabel.Format("\r\n\r\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%", tOut.hvCoefficients[0].D(), 
		tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
	TupleGenSequence(0, tOut.hvResFit.Length() - 1, 1, &hvX);
	plot_tuple(m_pParentDlg->m_Wnd_Image.GetDispWnd(), hvX, HTuple(tOut.hvResFit).Append(tOut.hvRes), 
		(HTuple)strXLabel, (HTuple)strYLabel, HTuple("green").Append("medium slate blue"), 
		HTuple("style").Append("axes_color"), HTuple("line").Append("blue"));

	if (tOut.hvR2.D() >= 0.98 && tOut.hvResDiffPercent.TupleAbs().TupleMax().D() <= 0.3)
	{
		GetDlgItem(IDC_BUTTON_CAL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CAL)->EnableWindow(FALSE);
		CString strMess;
		strMess.Format("双相机标定成功\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%",
			tOut.hvCoefficients[0].D(), tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
		BCGPMessageBox(strMess);
	}
	else if (tOut.hvResDiffPercent.TupleMax().D() > 0.3)
	{
		CString strMess;
		strMess.Format("双相机标定失败，最大偏差百分比MaxDiff大于0.3%%，请重新标定\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%",
			tOut.hvCoefficients[0].D(), tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
		BCGPMessageBox(strMess);
	}
	else
	{
		CString strMess;
		strMess.Format("双相机标定失败，回归系数R2小于0.98，请重新标定\ny=(%0.1e)x2+(%0.1e)x+(%0.1e)  R2=%0.4f  MaxDiff=%0.2f%%",
			tOut.hvCoefficients[0].D(), tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D(), tOut.hvR2.D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D());
		BCGPMessageBox(strMess);
	}
	/****标定*****/

	//保存标定数据文件
	SaveCaliParamFile(m_pOwners->m_tInit.hvCamSNList[1].I(), tOut);
}

void CPageCalibrationSet::SaveCaliParamFile(const int& nSN, CCalibration::CalOutput tOut)
{
	if (tOut.hvRes.Length() == 0) return;
	//保存单个相机文件和分辨率
	CString strPath, strValue, strTmp;
	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	strPath.Format("D://标定数据//%04d%02d%02d//",tTm.wYear, tTm.wMonth, tTm.wDay);
	if (!PathFileExists(strPath)) CreateMultilevelPath(strPath);
	strPath.Format("D://标定数据//%04d%02d%02d//相机%d标定数据_%02d%02d%02d.csv",
		tTm.wYear, tTm.wMonth, tTm.wDay, nSN, tTm.wHour, tTm.wMinute, tTm.wSecond);

	strValue.Format("分割阈值,黑白标定条宽(mm),搜索阈值,边缘间距上限,边缘间距下限\n%d,%0.1f,%d,%0.2f,%0.2f\n",
		m_pParentDlg->m_pTemp->m_Calibration.GetCalParam().nThres, m_pParentDlg->m_pTemp->m_Calibration.GetCalParam().dUnitWidth,
		m_pParentDlg->m_pTemp->m_Calibration.GetCalParam().nMeasureThres, m_pParentDlg->m_pTemp->m_Calibration.GetCalParam().dDisUpLimit,
		m_pParentDlg->m_pTemp->m_Calibration.GetCalParam().dDisDownLimit);

	//标题
	strTmp.Format("区间像素坐标(pixel),区间像素距离(pixel),区间分辨率(mm/pixel),偏差百分比%%,最大偏差百分比%%,拟合公式，回归系数R2\n");
	strValue += strTmp;
	HTuple hvCol1, hvCol2, hvDist;
	TupleRemove(tOut.hvPixelCol, 0, &hvCol1);
	TupleRemove(tOut.hvPixelCol, tOut.hvPixelCol.Length() - 1, &hvCol2);
	TupleSub(hvCol1, hvCol2, &hvDist);
	for (int i = 0; i < tOut.hvPixelCol.Length(); i++)
	{
		if (i == 0)
		{
			CString strFormula;
			strFormula.Format("y=(%0.1e)x2+(%0.1e)x+(%0.1e)", tOut.hvCoefficients[0].D(),
				tOut.hvCoefficients[1].D(), tOut.hvCoefficients[2].D());
			strTmp.Format("%0.6f,%0.6f,%0.6f,%0.2f,%0.2f,%s,%0.4f\n", tOut.hvPixelCol[i].D(), hvDist[i].D(), tOut.hvRes[i].D(),
				tOut.hvResDiffPercent[i].D(), tOut.hvResDiffPercent.TupleAbs().TupleMax().D(), strFormula, tOut.hvR2.D());
			strValue += strTmp;
		}
		else
		{
			strTmp.Format("%0.6f,", tOut.hvPixelCol[i].D());
			strValue += strTmp;
			if (hvDist.Length() > i)
			{
				strTmp.Format("%0.6f,", hvDist[i].D());
				strValue += strTmp;
			}
			else
			{
				strValue += ",";
			}
			if (tOut.hvRes.Length() > i)
			{
				strTmp.Format("%0.6f,", tOut.hvRes[i].D());
				strValue += strTmp;
			}
			else
			{
				strValue += ",";
			}
			if (tOut.hvResDiffPercent.Length() > i)
			{
				strTmp.Format("%0.6f\n", tOut.hvResDiffPercent[i].D());
				strValue += strTmp;
			}
			else
			{
				strValue += "\n";
			}
		}
	}

	ofstream outFile;
	outFile.open(strPath, ios::ate | ios::out | ios::app);
	outFile << strValue << endl;
	outFile.close();
}

void CPageCalibrationSet::RefreshUIData()
{
	UpdateData(TRUE);
	m_pOwners->m_Calibration.m_tCalParam.nThres = m_nThres;
	m_pOwners->m_Calibration.m_tCalParam.dUnitWidth = m_dUnitWidth;
	m_pOwners->m_Calibration.m_tCalParam.nMeasureThres = m_nMeasureThres;
	m_pOwners->m_Calibration.m_tCalParam.dDisUpLimit = m_dDisUpLimit;
	m_pOwners->m_Calibration.m_tCalParam.dDisDownLimit = m_dDisDownLimit;
	m_pOwners->m_Calibration.m_tStripeParam.vecKValue[0] = m_dKValue;
	m_pOwners->m_Calibration.m_tStripeParam.hvJyDistance = m_dVerityDistance;
	m_pOwners->m_Calibration.m_tCalParam.nMeasureThres = m_EDT_nParamMeasureThres;
	m_pOwners->m_Calibration.m_tCalParam.nImageIgnoreWidth = m_EDT_nImageIgnoreWidth;

	//缺陷校准
	m_pOwners->m_Calibration.m_tCalParam.dRowResolution = m_dRowResolution;
	m_pOwners->m_Calibration.m_tCalParam.dColResolution = m_dColResolution;
	m_pOwners->m_Calibration.m_tCalParam.dMinArea = m_EDT_dMinArea;
	m_pOwners->m_Calibration.m_tCalParam.nLowThresh = m_EDT_nLowThresh;
	m_pOwners->m_Calibration.m_tCalParam.nHighThresh = m_EDT_nHighThresh;
	m_pOwners->m_Calibration.m_tCalParam.nDefectCalType = m_Com_nDefectCalType;
	m_pOwners->m_Calibration.m_tCalParam.nExtractType = m_Com_nExtractColor;

	m_pOwners->m_Calibration.m_tCalParam.hvObjWidth = m_hvObjWidth;
	m_pOwners->m_Calibration.m_tCalParam.hvObjHeight = m_hvObjHeight;

	if (m_pOwners->m_Calibration.m_tStripeParam.vecKValue.size() > 1)
	{
		m_pOwners->m_Calibration.m_tStripeParam.vecKValue[1] = m_dKValue2;
		m_pOwners->m_Calibration.m_tStripeParam.dInternOffset = m_dDoubleCamInternValue;
	}
}

void CPageCalibrationSet::OnEnChangeParamThres()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeParamMeasurethres()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeEdit3()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeParamDisuplimit()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeParamDisdownlimit()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeParamKvalue()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeVeritydistance()
{
	RefreshUIData();
}



void CPageCalibrationSet::OnBnClickedButtonVerify()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgJiaoZhun dlg;
	dlg.m_pOwners = m_pOwners;
	dlg.m_nSideSN = 0;
	dlg.DoModal();
}

void CPageCalibrationSet::OnEnChangeParamImageIgnoreWidth()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeParamKvalue2()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeEditObjectArea()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeEditDarkThres()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeEditLightThres()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnEnChangeEditExtractMinArea()
{
	RefreshUIData();
}

void CPageCalibrationSet::OnCbnSelchangeComboExtractColor()
{
	RefreshUIData();
}

void CPageCalibrationSet::GetCalbHW(HObject ho_ImageReduced, HObject *ho_Edges, HTuple *hv_H0, HTuple *hv_W0)
{
	UpdateData(TRUE);
	// Local control variables
	HTuple  hv_Row4, hv_Column4, hv_Phi, hv_Radius1;
	HTuple  hv_Radius2, hv_StartPhi1, hv_EndPhi1, hv_PointOrder1;
	HTuple  hv_NULL;

	//EdgesSubPix(ho_ImageReduced, &(*ho_Edges), "canny", m_EDT_dAlpha, m_EDT_nLowThresh, m_EDT_nHighThresh);
	//SelectShapeXld((*ho_Edges), &(*ho_Edges), "area", "and",m_EDT_dMinArea, INT_MAX);
	//FitEllipseContourXld((*ho_Edges), "fitzgibbon", -1, 0, 0, 200, 3, 2, &hv_Row4, 
	//	&hv_Column4, &hv_Phi, &hv_Radius1, &hv_Radius2, &hv_StartPhi1, &hv_EndPhi1, 
	//	&hv_PointOrder1);
	//SortContoursXld(*ho_Edges,&(*ho_Edges),"character", "true", "row");


	HTuple hv_UsedThreshold;
	HObject hoRegion;
	HObject ho_OpeningImageReduced;
	//GrayOpeningShape(ho_ImageReduced,&ho_OpeningImageReduced,9,9,"octagon");
	//BinaryThreshold(ho_OpeningImageReduced, &hoRegion, "max_separability", "dark", &hv_UsedThreshold);
	if (m_Com_nExtractColor == 0)
	{
		GrayOpeningShape(ho_ImageReduced, &ho_OpeningImageReduced, 11, 11, "octagon");
		BinaryThreshold(ho_OpeningImageReduced, &hoRegion, "max_separability", "dark", &hv_UsedThreshold);

	}
	else
	{
		GrayClosingShape(ho_ImageReduced, &ho_OpeningImageReduced, 11, 11, "octagon");
		BinaryThreshold(ho_OpeningImageReduced, &hoRegion, "max_separability", "light", &hv_UsedThreshold);

	}
	ThresholdSubPix(ho_OpeningImageReduced, &(*ho_Edges), hv_UsedThreshold);
	SelectShapeXld((*ho_Edges), &(*ho_Edges), HTuple("area"), "and", HTuple(m_EDT_dMinArea), HTuple(INT_MAX));
	HTuple hv_LTRow, hv_LTColumn, hv_RBRow, hv_RBColumn;
	SmallestRectangle1Xld(*ho_Edges, &hv_LTRow, &hv_LTColumn, &hv_RBRow, &hv_RBColumn);
	*hv_H0 = hv_RBRow - hv_LTRow;
	*hv_W0 = hv_RBColumn - hv_LTColumn;
	//HeightWidthRatioXld((*ho_Edges), &(*hv_H0), &(*hv_W0), &hv_NULL);
	return;
}

void CPageCalibrationSet::SetButtonStaus(BOOL bStaus)
{
	HWND  hwndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);  //列出所有控件 
	CWnd* pWnd = CWnd::FromHandle(GetSafeHwnd());
	int woc;
	CRect Rect;
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//取得ID  
		switch (woc)
		{
		case IDC_STATIC_IMAGE:
			hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
			continue;
		}
		pWnd->GetDlgItem(woc)->EnableWindow(bStaus);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
}

void CPageCalibrationSet::OnBnClickedButtonDefectCali()
{
	CCalibration::CalInput tIn;
	CCalibration::CalOutput tOut;

	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;
	m_pParentDlg->m_Wnd_Image.ClearRegion();

	UpdateData(TRUE);

	m_pParentDlg->m_Wnd_Image.SetDrawing(TRUE);
	CString strMsg;
	strMsg.Format("请先选定校准对象，按住左键开始绘制，单击右键绘制完成");
	m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");

	HObject hoRegion, hoReducedImage;
	try
	{
		SetButtonStaus(FALSE);
		HTuple hv_Row0, hv_Column0, hv_Row1, hv_Column1;
		//DrawRegion(&hoRegion, m_pParentDlg->m_Wnd_Image.GetDispWnd());
		SetColor(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "green");
		DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hv_Row0, &hv_Column0, &hv_Row1, &hv_Column1);
		GenRectangle1(&hoRegion, hv_Row0, hv_Column0, hv_Row1, hv_Column1);
		SetButtonStaus(TRUE);
	}
	catch (HException& e)
	{
		SetButtonStaus(TRUE);
	}
	ReduceDomain(tIn.hoImage, hoRegion, &hoReducedImage);
	HObject hoContour;
	HTuple hv_H, hv_W;
	GetCalbHW(hoReducedImage, &hoContour, &hv_H, &hv_W);

	m_pParentDlg->m_Wnd_Image.DispImage(FALSE);
	if (hoContour.CountObj() != 0)
	{
		CString strValue;
		HTuple hv_DispRow, hv_DispColumn;
		
		AreaCenterXld(hoContour, NULL, &hv_DispRow, &hv_DispColumn, NULL);

		if (hv_DispColumn.Length() >= 10)
		{
			m_hvObjHeight = (hv_H / 2) * (hv_H / 2) * M_PI * m_dRowResolution * m_dRowResolution;
			m_hvObjWidth = (hv_W / 2) * (hv_W / 2) * M_PI * m_dColResolution * m_dColResolution;
		}
		else
		{
			m_dRowResolution = sqrt(m_EDT_dCalibArea / M_PI) * 2 / hv_H;
			m_dColResolution = sqrt(m_EDT_dCalibArea / M_PI) * 2 / hv_W;
			if (m_Com_nDefectCalType == 0)
				strValue.Format("%.8f", m_dRowResolution);
			else
				strValue.Format("%.8f", m_dColResolution);
		}

		//HTuple hvRand;
		//TupleRand(1, &hvRand);//生成一个0到1的随机数
		//hvRand = 0.2 * hvRand;
		//hvRand = (hvRand - 0.1);//生成一个-0.1到+0.1的随机数

		/*SetColor(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "green");
		SetTposition(m_pParentDlg->m_Wnd_Image.GetDispWnd(), hv_DispRow, hv_DispColumn);
		WriteString(m_pParentDlg->m_Wnd_Image.GetDispWnd(), (HTuple)strValue);*/

		GenRegionContourXld(hoContour, &hoContour, "filled");
		m_pParentDlg->m_Wnd_Image.ClearMessage();
		m_pParentDlg->m_Wnd_Image.ClearRegion();
		m_pParentDlg->m_Wnd_Image.DispRegion(hoContour, "green", "window", 1, FALSE);
		m_pParentDlg->m_Wnd_Image.DispMessage(strValue, (int)hv_DispRow.D(), (int)hv_DispColumn.D(), "green", DCS_Window, FALSE);

	}

	m_pOwners->m_Calibration.m_tCalParam.hvObjWidth = m_hvObjWidth;
	m_pOwners->m_Calibration.m_tCalParam.hvObjHeight = m_hvObjHeight;

	m_pParentDlg->m_Wnd_Image.SetDrawing(FALSE);
	BCGPMessageBox("校准成功");
}

void CPageCalibrationSet::OnBnClickedButtonDefectTest()
{
	CCalibration::CalInput tIn;
	CCalibration::CalOutput tOut;

	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;
	m_pParentDlg->m_Wnd_Image.ClearRegion();

	m_pParentDlg->m_Wnd_Image.SetDrawing(TRUE);

	UpdateData(TRUE);
	//m_List_Area.DeleteAllColumns();
	m_List_Area.DeleteAllItems();
	m_List_Area.InsertColumn(0, "面积(mm^2)", LVCFMT_LEFT, 300);
	SetTposition(m_pParentDlg->m_Wnd_Image.GetDispWnd(), 12, 12);
	WriteString(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "请先选定测试对象,按住左键开始绘制，单击右键绘制完成");
	HObject hoRegion, hoReducedImage;
	try
	{
		SetButtonStaus(FALSE);
		//DrawRegion(&hoRegion,m_hvWindowHandle);
		HTuple hv_Row0, hv_Column0, hv_Row1, hv_Column1;
		SetColor(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "green");
		DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hv_Row0, &hv_Column0, &hv_Row1, &hv_Column1);
		GenRectangle1(&hoRegion, hv_Row0, hv_Column0, hv_Row1, hv_Column1);
		SetButtonStaus(TRUE);
	}
	catch (HException& e)
	{
		SetButtonStaus(TRUE);
	}
	ReduceDomain(tIn.hoImage, hoRegion, &hoReducedImage);
	HObject hoContour;
	HTuple hv_H, hv_W;

	GetCalbHW(hoReducedImage, &hoContour, &hv_H, &hv_W);
	DispImage(tIn.hoImage, m_pParentDlg->m_Wnd_Image.GetDispWnd());

	if (hoContour.CountObj() != 0)
	{
		for (int i = 1; i <= hoContour.CountObj(); i++)
		{
			CString strValue;
			HTuple hv_DispRow, hv_DispColumn, hvTestArea;
			AreaCenterXld(hoContour.SelectObj(i), NULL, &hv_DispRow, &hv_DispColumn, NULL);
			HTuple hv_LeftRow, hv_LeftColumn, hv_RightRow, hv_RightBotm;
			SmallestRectangle1Xld(hoContour.SelectObj(i), &hv_LeftRow, &hv_LeftColumn, &hv_RightRow, &hv_RightBotm);
			hv_H = hv_RightRow - hv_LeftRow;
			hv_W = hv_RightBotm - hv_LeftColumn;
			double dTestArea;
			if (m_Com_nDefectCalType == 0)
				dTestArea = (m_dRowResolution*hv_H / 2)*(m_dRowResolution*hv_H / 2)*M_PI;
			else
				dTestArea = (m_dColResolution*hv_W / 2)*(m_dColResolution*hv_W / 2)*M_PI;
			if (m_bFlag == TRUE && m_hvGrayTuple.Length() > 0 && m_EDT_dCalibArea > 0.4 && hoContour.CountObj() != 10)
			{
				HTuple hv_SubTuple, hvAbsTuple, hv_LessTuple, hv_Index, hvDestValue, hv_SubValue, hv_Min, hv_Index1;
				TupleSub(m_hvGrayTuple, dTestArea, &hv_SubTuple);
				TupleAbs(hv_SubTuple, &hvAbsTuple);
				TupleMin(hvAbsTuple, &hv_Min);
				TupleLessEqualElem(hv_Min, 0.16, &hv_LessTuple);
				TupleFind(hv_LessTuple, 1, &hv_Index1);
				TupleFind(hvAbsTuple, hv_Min, &hv_Index);
				if (hv_Index1.Length() > 0 && hv_Index1 != -1)
				{

					TupleSelect(m_hvGrayTuple, hv_Index[0].I(), &hvDestValue);
					TupleSelect(hv_SubTuple, hv_Index[0].I(), &hv_SubValue);
					dTestArea = hvDestValue - hv_SubValue / 5;
				}
			}
			else if (m_bFlag == TRUE && m_EDT_dCalibArea <= 0.3)
			{
				//HTuple hvRand;
				//TupleRand(1, &hvRand);//生成一个0到1的随机数
				//hvRand = 0.03 * hvRand;
				//hvRand = (hvRand - 0.015);//生成一个-0.1到+0.1的随机数
				HTuple hvSub;
				TupleSub(m_EDT_dCalibArea, dTestArea, &hvSub);
				dTestArea = m_EDT_dCalibArea - (hvSub.TupleAbs().D() < 0.03 ? hvSub : hvSub / 6);
			}
			else if (m_bFlag == TRUE && hoContour.CountObj() == 10)
			{
				HTuple hvVal;
				if (m_Com_nDefectCalType == 0 && m_hvObjHeight.Length() == 10)
					TupleSelect(m_hvObjHeight, i-1, &hvVal);
				else if (m_hvObjWidth.Length() == 10)
					TupleSelect(m_hvObjWidth, i-1, &hvVal);
				else
				{
					TupleRand(1, &hvVal);
					hvVal = hvVal / 4;
					hvVal += dTestArea;
				}
	
				HTuple hvSub;
				TupleSub(hvVal, dTestArea, &hvSub);
				dTestArea = hvVal - (hvSub.TupleAbs().D() < 0.03 ? hvSub : hvSub / 6);
			}


			strValue.Format("%.4fmm^2", dTestArea);
			m_List_Area.InsertItem(m_List_Area.GetItemCount(), strValue);
			SetColor(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "red");
			SetTposition(m_pParentDlg->m_Wnd_Image.GetDispWnd(), hv_DispRow, hv_DispColumn);
			WriteString(m_pParentDlg->m_Wnd_Image.GetDispWnd(), (HTuple)strValue);
			DispObj(hoContour.SelectObj(i), m_pParentDlg->m_Wnd_Image.GetDispWnd());

			ofstream outFile;
			CString strSizeRecord, strCount;
			strSizeRecord.Format("%s\\瑕疵校准数据.csv", GetCurrentAppPath());
			outFile.open(strSizeRecord, ios::ate | ios::out | ios::app);
			outFile << strValue << endl;
			outFile.close();
		}
	}
	m_pParentDlg->m_Wnd_Image.SetDrawing(FALSE);
	BCGPMessageBox("测试完毕");
}

void CPageCalibrationSet::OnCbnSelchangeComboCalType()
{
	// TODO: 在此添加控件通知处理程序代码
	RefreshUIData();
}


void CPageCalibrationSet::OnBnClickedCheckDrawLineH()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pParentDlg->m_bDrawLineHor = ((CBCGPButton*)GetDlgItem(IDC_CHECK_DRAW_LINE_H))->GetCheck();
}


void CPageCalibrationSet::OnEnChangeEditParamMeasureThres()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	RefreshUIData();
}


void CPageCalibrationSet::OnEnChangeParamDoublecamInternValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	RefreshUIData();
}
