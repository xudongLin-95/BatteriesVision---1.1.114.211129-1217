// DlgRecipes.cpp : 实现文件
#include "stdafx.h"
#include "SCIMes.h"
#include "DlgRecipes.h"
#include "afxdialogex.h"
#include "Mes.h"

// CDlgRecipes 对话框

IMPLEMENT_DYNAMIC(CDlgRecipes, CDialogEx)

CDlgRecipes::CDlgRecipes(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRecipes::IDD, pParent)
	,m_pRealMes(NULL)
	, m_nSelect(0)
	, m_cstrRecipeCode(_T(""))
	, m_cstrRecipeVison(_T(""))
	, m_cstrProductCode(_T(""))
{
	m_JsonRecipes.Clear();
	m_JsonRecipeParams.Clear();
}

CDlgRecipes::~CDlgRecipes()
{
}

void CDlgRecipes::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECIPES, m_ListRecipes);
	DDX_Control(pDX, IDC_LIST_RECIPEPARAM, m_ListRecipeParam);
	DDX_Control(pDX, IDC_STATIC_RECIPE, m_StaticRecipe);
	DDX_Control(pDX, IDC_STATIC_RECIPEPARAM, m_StaticRecipeParam);
	DDX_Text(pDX, IDC_EDIT_RECIPECODE, m_cstrRecipeCode);
	DDX_Text(pDX, IDC_EDIT_RECIPEVISON, m_cstrRecipeVison);
	DDX_Text(pDX, IDC_EDIT_PRODUCTCODE, m_cstrProductCode);
	DDX_Control(pDX, IDC_LIST_CURRENTRECIPE, m_ListCrrentRecipe);
	DDX_Control(pDX, IDC_STATIC_CURRENRECIPE, m_StaticCurrentRecipe);
}

BEGIN_MESSAGE_MAP(CDlgRecipes, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgRecipes::OnBnClickedOk)
ON_NOTIFY(NM_CLICK, IDC_LIST_RECIPES, &CDlgRecipes::OnNMClickListRecipes)
ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CDlgRecipes::OnBnClickedButtonDownload)
ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CDlgRecipes::OnBnClickedButtonUpload)
END_MESSAGE_MAP()


// CDlgRecipes 消息处理程序
BOOL CDlgRecipes::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitUI();   //初始化配方设置UI
	InitValue();
	UpdateCurrentRecipeList();

	if (FALSE)
	{
		GetRecipes();
	}
	else
	{
		Soap_GetRecipeList();
	}
	
	UpDateRecipes();

	UpdatePecipeParam();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgRecipes::InitUI(void)
{
	m_LST_Font.CreatePointFont(110,"Microsoft YaHei");

	m_ListRecipes.SetHeaderHeight(1.5);          //设置头部高度
	m_ListRecipes.SetHeaderFontHW(15,0);         //设置头部字体高度,和宽度,0表示缺省，自适应 
	m_ListRecipes.SetHeaderTextColor(RGB(0,0,0)); //设置头部字体颜色 255,200,100
	m_ListRecipes.SetHeaderBKColor(255,255,255,0); //设置头部背景色 100,255,100,8
	m_ListRecipes.SetBkColor(RGB(255,255,255));        //设置背景色 50,10,10
	m_ListRecipes.SetRowHeigt(30);               //设置行高度
	m_ListRecipes.SetFontHW(14,0);               //设置字体高度，和宽度,0表示缺省宽度
	m_ListRecipes.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER /*|LVS_EX_CHECKBOXES*/);

	m_ListRecipeParam.SetHeaderHeight(1.5);          //设置头部高度
	m_ListRecipeParam.SetHeaderFontHW(15,0);         //设置头部字体高度,和宽度,0表示缺省，自适应 
	m_ListRecipeParam.SetHeaderTextColor(RGB(0,0,0)); //设置头部字体颜色
	m_ListRecipeParam.SetHeaderBKColor(255,255,255,0); //设置头部背景色
	m_ListRecipeParam.SetBkColor(RGB(255,255,255));        //设置背景色
	m_ListRecipeParam.SetRowHeigt(30);               //设置行高度
	m_ListRecipeParam.SetFontHW(14,0);               //设置字体高度，和宽度,0表示缺省宽度
	m_ListRecipeParam.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	m_ListCrrentRecipe.SetHeaderHeight(1.5);          //设置头部高度
	m_ListCrrentRecipe.SetHeaderFontHW(15,0);         //设置头部字体高度,和宽度,0表示缺省，自适应 
	m_ListCrrentRecipe.SetHeaderTextColor(RGB(0,0,0)); //设置头部字体颜色
	m_ListCrrentRecipe.SetHeaderBKColor(255,255,255,0); //设置头部背景色
	m_ListCrrentRecipe.SetBkColor(RGB(255,255,255));        //设置背景色
	m_ListCrrentRecipe.SetRowHeigt(30);               //设置行高度
	m_ListCrrentRecipe.SetFontHW(14,0);               //设置字体高度，和宽度,0表示缺省宽度
	m_ListCrrentRecipe.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);


	m_StaticRecipe.SetTextColor(RGB(0,0,0));
	m_StaticRecipeParam.SetTextColor(RGB(0,0,0));
	m_StaticCurrentRecipe.SetTextColor(RGB(0,0,0));
	m_StaticRecipe.SetFont("Microsoft YaHei", 20,TRUE);
	m_StaticRecipeParam.SetFont("Microsoft YaHei", 20,TRUE);
	m_StaticCurrentRecipe.SetFont("Microsoft YaHei", 20,TRUE);
	m_StaticRecipe.SetWindowTextA(_T("Mes系统配方"));
	m_StaticCurrentRecipe.SetWindowTextA(_T("当前配方参数"));
	m_ListRecipes.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 50);
	m_ListRecipes.InsertColumn(1, _T("RecipeCode"), LVCFMT_CENTER, 150);
	m_ListRecipes.InsertColumn(2, _T("ProductCode"), LVCFMT_CENTER, 150);
	m_ListRecipes.InsertColumn(3, _T("Version"), LVCFMT_CENTER, 150);
	m_ListRecipes.InsertColumn(4, _T("LineCode"), LVCFMT_CENTER, 150);
	m_ListRecipes.InsertColumn(5, _T("SectionCode"), LVCFMT_CENTER, 150);
	m_ListRecipes.InsertColumn(6, _T("EquipmentCode"), LVCFMT_CENTER, 150);
	m_ListRecipeParam.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 50);
	m_ListRecipeParam.InsertColumn(1, _T("ParameterCode"), LVCFMT_CENTER, 150);
	m_ListRecipeParam.InsertColumn(2, _T("ParameterValue"), LVCFMT_CENTER, 150);
	m_ListRecipeParam.InsertColumn(3, _T("ParameterMinValue"), LVCFMT_CENTER, 150);
	m_ListRecipeParam.InsertColumn(4, _T("ParameterMaxValue"), LVCFMT_CENTER, 150);
	m_ListCrrentRecipe.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 50);	
	m_ListCrrentRecipe.InsertColumn(1, _T("Code"), LVCFMT_CENTER, 100);
	m_ListCrrentRecipe.InsertColumn(2, _T("参数"), LVCFMT_CENTER, 100);
	m_ListCrrentRecipe.InsertColumn(3, _T("Value"), LVCFMT_CENTER, 100);	
	m_ListCrrentRecipe.InsertColumn(4, _T("MinValue"), LVCFMT_CENTER, 100);	
	m_ListCrrentRecipe.InsertColumn(5, _T("MaxValue"), LVCFMT_CENTER, 100);
}

int CDlgRecipes::GetRecipes(void)
{
	CString strException;
	CString cstrGetRecipes;

	std::string strRecipes,strGetRecipe;

	int iRet = -1;
	try
	{
		iRet = m_pRealMes->GetRequestRecipes(strRecipes.c_str());
	}
	catch(...)
	{
		strException.Format(_T("GetRecipes 异常catch！\t错误码：%d"),iRet);

	//	m_pRealMes->m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);
		m_pRealMes->Log(strException);
		m_pRealMes->m_record.writeHttpFile(strException);

		return iRet;
	}
	
	if (200 != iRet)
	{
		strException.Format(_T("GetRecipes失败！\t错误码：%d"),iRet);

	//	m_pRealMes->m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);

		m_pRealMes->Log(strException);	
		m_pRealMes->m_record.writeHttpFile(strException);

		return iRet;
	}

	m_JsonRecipes = neb::CJsonObject(strRecipes);
	strGetRecipe = m_JsonRecipes.ToString();
	cstrGetRecipes.Format(_T("GetRecipes: 返回码：%d\t获取信息：%s\n"),iRet,strGetRecipe.c_str());
	m_pRealMes->m_record.writeHttpFile(cstrGetRecipes);

	return iRet;
}

int CDlgRecipes::Soap_GetRecipeList(void)
{
	return true; //获取配方的协议尚未定义，暂时不做处理

	CString strException;
	neb::CJsonObject  nebCJsonReciev;

	int iRet = -1;
	int nCode(-11);
	try
	{
		iRet = m_pRealMes->Soap_getRecipeList(m_cstrProductCode,nebCJsonReciev);
	}
	catch(...)
	{
		strException.Format(_T("Soap_GetRecipeList 异常！\t错误码：%d"),iRet);
		m_pRealMes->Log(strException);
		iRet = -1;

		return iRet;
	}
	nebCJsonReciev.Get("code",nCode);
	if (SOAP_OK != iRet || 1 != nCode)
	{
		strException.Format(_T("Soap_GetRecipeList失败！\t错误码：%d"),iRet);
		m_pRealMes->Log(strException);
		return iRet;
	}
	nebCJsonReciev.Get("Data",m_JsonRecipes);

	return iRet;
}


int CDlgRecipes::UpDateRecipes(void)
{
	int iNum = 0;
	std::string strRecipeCode,strProductCode,strVersion,strLineCode,strSection,strEquipmentCode;
	neb::CJsonObject * pJsonRecipe = NULL;
	m_ListRecipes.DeleteAllItems();
	if (m_JsonRecipes.IsEmpty())
	{
		return 0;
	}
	iNum = m_JsonRecipes.GetArraySize();
	if (iNum <= 0)
	{
		return 0;
	}
	pJsonRecipe = new neb::CJsonObject[iNum];
	for (int i = 0 ; i < iNum ;i++)
	{
		CString strIndex;	
		strIndex.Format(_T("%d"),i+1);
		m_JsonRecipes.Get(i,pJsonRecipe[i]);
		pJsonRecipe[i].Get("RecipeCode",strRecipeCode);
		pJsonRecipe[i].Get("ProductCode",strProductCode);
		pJsonRecipe[i].Get("Version",strVersion);
		pJsonRecipe[i].Get("LineCode",strLineCode);
		pJsonRecipe[i].Get("SectionCode",strSection);
		pJsonRecipe[i].Get("EquipmentCode",strEquipmentCode);
		m_ListRecipes.InsertItem(i+1,strIndex);		
		m_ListRecipes.SetItemText(i,1,strRecipeCode.c_str());
		m_ListRecipes.SetItemText(i,2,strProductCode.c_str());
		m_ListRecipes.SetItemText(i,3,strVersion.c_str());
		m_ListRecipes.SetItemText(i,4,strLineCode.c_str());
		m_ListRecipes.SetItemText(i,5,strSection.c_str());
		m_ListRecipes.SetItemText(i,6,strEquipmentCode.c_str());
	}
	m_nSelect = 0;
	if (pJsonRecipe)
	{
		delete[] pJsonRecipe;
		pJsonRecipe = NULL;
	}
	return 1;
}

int CDlgRecipes::GetRecipeParams(CString &cstrRecipe,std::string& strRecipeParam)
{
	CString strException ,cstrGetRecipeParam;
	int iRet = -1;
	try
	{
		iRet = m_pRealMes->GetRequestRecipeParameters(cstrRecipe,strRecipeParam);
	}
	catch(...)
	{
		strException.Format(_T("GetRequestRecipeParameters异常！\t错误码：%d"),iRet);

		//	m_pRealMes->m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);

		m_pRealMes->Log(strException);
		m_pRealMes->m_record.writeHttpFile(strException);

		return iRet;
	}

	if (200 != iRet)
	{
		strException.Format(_T("GetRequestRecipeParameters失败！\t错误码：%d"),iRet);

		//	m_pRealMes->m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCSTR)strException,NULL);

		m_pRealMes->Log(strException);
		m_pRealMes->m_record.writeHttpFile(strException);

		return iRet;
	}

	cstrGetRecipeParam.Format(_T("GetRequestRecipeParameters：返回码：%d\t获取信息：%s"),iRet,strRecipeParam.c_str());
	m_pRealMes->m_record.writeHttpFile(cstrGetRecipeParam);
}

int CDlgRecipes::UpdatePecipeParam(void)
{
	int iRet = -1;
	CString strException,cstrGetRecipeParam;
	CString cstrRecipe;
	std::string strRecipeParam,strGetRecipeParam;
	std::string strVersion,strLineCode,strSection,strEquipmentCode;
	std::string strRecipeCode,strProductCode;
	m_ListRecipeParam.DeleteAllItems();

	if (m_nSelect > m_JsonRecipes.GetArraySize() || 0 == m_JsonRecipes.GetArraySize())
	{
		return 0;
	}

	m_JsonRecipes.Get(m_nSelect,m_JsonRecipeParams);
	m_JsonRecipeParams.Get("RecipeCode",strRecipeCode);
	m_JsonRecipeParams.Get("ProductCode",strProductCode);
	m_JsonRecipeParams.Get("Version",strVersion);
	m_JsonRecipeParams.Get("LineCode",strLineCode);
	m_JsonRecipeParams.Get("SectionCode",strSection);
	m_JsonRecipeParams.Get("EquipmentCode",strEquipmentCode);
	cstrRecipe.Format(_T("LineCode=%s&SectionCode=%s&EquipmentCode=%s&RecipeCode=%s&Version=%s"),
	strLineCode.c_str(),strSection.c_str(),strEquipmentCode.c_str(),strRecipeCode.c_str(),strVersion.c_str());

	CString cstrStatic;

	cstrStatic.Format(_T("RecipCode : %s\tProductCode : %s\tVersion : %s"),strRecipeCode.c_str(),strProductCode.c_str(),strVersion.c_str());
	m_StaticRecipeParam.SetWindowText(cstrStatic);
	m_cstrRecipeCode.Format(strRecipeCode.c_str());
	m_cstrProductCode.Format(strProductCode.c_str());
	m_cstrRecipeVison.Format(strVersion.c_str());
	UpdateData(FALSE);		//更新界面显示配方
	neb::CJsonObject JsonRecipe;
	neb::CJsonObject JsonRecipeParams;
	neb::CJsonObject* pJsonParam = NULL;

	int iNum = 0;

	neb::CJsonObject nebCJsonReciev;
	if(FALSE)
	{
		if (200 != GetRecipeParams(cstrRecipe,strRecipeParam))
		{
			return 0;
		}
		JsonRecipe = neb::CJsonObject(strRecipeParam);
		JsonRecipe.Get("Parameters",JsonRecipeParams);
	}
	else
	{
		int iRet(-1),nCode(-11);
		iRet = m_pRealMes->Soap_getRecipe(m_cstrRecipeCode,nebCJsonReciev);
		nebCJsonReciev.Get("code",nCode);
		if (SOAP_OK != iRet || 1 != nCode)
		{
			return 0;
		}
		if (nebCJsonReciev["Data"].IsEmpty() )
		{
			return 0;
		}
		neb::CJsonObject cJsonData;

		nebCJsonReciev.Get("Data",cJsonData);
		if (cJsonData["ParamList"].IsEmpty() )
		{
			return 0;
		}
		cJsonData.Get("ParamList",JsonRecipeParams);
	}
	
	iNum =JsonRecipeParams.GetArraySize();

	if (iNum <= 0)
	{
		return 0;
	}

	pJsonParam = new neb::CJsonObject[iNum];

	for (int i = 0 ;i < iNum ; i++)
	{
		std::string strParameterCode,strParameterValue,strParameterMax,strParameterMin;
		CString strIndex;
		strIndex.Format(_T("%d"),i+1);
		JsonRecipeParams.Get(i,pJsonParam[i]);
		if (FALSE)
		{
			pJsonParam[i].Get("ParameterCode",strParameterCode);
			pJsonParam[i].Get("ParameterValue",strParameterValue);
			pJsonParam[i].Get("ParameterMinValue",strParameterMin);
			pJsonParam[i].Get("ParameterMaxValue",strParameterMax);
		}
		else
		{
			pJsonParam[i].Get("ParamCode",strParameterCode);
			pJsonParam[i].Get("ParamValue",strParameterValue);
			pJsonParam[i].Get("ParamLower",strParameterMin);
			pJsonParam[i].Get("ParamUpper",strParameterMax);
		}
		
		m_ListRecipeParam.InsertItem(i+1,strIndex);
		m_ListRecipeParam.SetItemText(i,1,strParameterCode.c_str());
		m_ListRecipeParam.SetItemText(i,2,strParameterValue.c_str());
		m_ListRecipeParam.SetItemText(i,3,strParameterMin.c_str());
		m_ListRecipeParam.SetItemText(i,4,strParameterMax.c_str());
	}

	if(pJsonParam)
	{
		delete[] pJsonParam;
		pJsonParam = NULL;
	}
	return iRet;
}


void CDlgRecipes::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (FALSE)
	{
		if (GetRecipes() <= 0)
		{
			return;
		}
	}
	else
	{
		if (Soap_GetRecipeList() < 0)
		{
			return;
		}
	}
	
	if(UpDateRecipes() <= 0)
	{
		return;
	}

	UpdatePecipeParam();

//	CDialogEx::OnOK();
}


//void CDlgRecipes::OnNMClickListRecipeparam(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//
//	POSITION pos = m_ListRecipes.GetFirstSelectedItemPosition();  
//	if (pos == NULL)  
//		TRACE0("No items were selected!\n");  
//	else  
//	{  
//		while (pos)  
//		{  
//			int nItem = m_ListRecipes.GetNextSelectedItem(pos);  
//			TRACE1("Item %d was selected!\n", nItem);  
//			// you could do your own processing on nItem here   
//		}  
//	}  
//
//
//
//	*pResult = 0;
//}


void CDlgRecipes::OnNMClickListRecipes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码


	POSITION pos = m_ListRecipes.GetFirstSelectedItemPosition(); 
	if (pos == NULL)  
		return;  
	else  
	{  
		while (pos)  
		{  
			int nItem = m_ListRecipes.GetNextSelectedItem(pos);  
			
			m_nSelect = nItem;

			UpdatePecipeParam();
		}  
	}  

	*pResult = 0;
}


int CDlgRecipes::InitValue(void)
{
	m_cstrRecipeCode = m_pRealMes->m_strRecipeCode;

	m_cstrRecipeVison = m_pRealMes->m_strRecipeVison;

	m_cstrProductCode = m_pRealMes->m_strProductCode;

	UpdateData(FALSE); //变量-->控件

	return 1;
}

int CDlgRecipes::UpLoadRecipe()
{
	CString cstrMessage;
	std::string strSendDate,strSendDateUT8,strRespone;
	int iRet = -1;
	cstrMessage.Format(_T("请确认是否要上传当前配方！\n配方号：%s\t配方版本：%s"),m_cstrRecipeCode,m_cstrRecipeVison);

	if (IDYES != MessageBox(cstrMessage,_T("请确认是否上传当前配方"),MB_ICONEXCLAMATION|MB_YESNO ))
	{
		return 0;
	}

	try
	{
		CString cstrTimestamp;
		SYSTEMTIME iLocalTime;
		GetLocalTime(&iLocalTime);
		cstrTimestamp.Format(_T("%4d-%2d-%2dT%2d:%2d:%2d.%3d+08:00"),
			iLocalTime.wYear,iLocalTime.wMonth,iLocalTime.wDay,iLocalTime.wHour,iLocalTime.wMinute,iLocalTime.wSecond,iLocalTime.wMilliseconds);
		m_pRealMes->m_cJsonRecipes.Replace("Timestamp",cstrTimestamp.GetString());
		strSendDate = "["+ m_pRealMes->m_cJsonRecipes.ToString()+"]";
		strSendDateUT8 = AsciiToUtf8(strSendDate);
		iRet = m_pRealMes->MESReportEquipmentRecipeParametersInfo((CString)strSendDateUT8.c_str(),strRespone);

		if (200 != iRet)
		{
			cstrMessage.Format(_T("配方上传失败！错误码：%d\n 错误信息：%s\n上传信息：%s\n"),iRet,strRespone.c_str(),strSendDate.c_str());

			//	m_pRealMes->m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCTSTR)cstrMessage,NULL);

			m_pRealMes->Log(cstrMessage);
			m_pRealMes->m_record.writeHttpFile(cstrMessage);
			MessageBox(cstrMessage);
			return 0;
		}
	}
	catch (...)
	{
		cstrMessage.Format(_T("配方上传失败！错误码：%d\n 错误信息：%s\n上传信息：%s\n"),iRet,strRespone.c_str(),strSendDate.c_str());

		//	m_pRealMes->m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCTSTR)cstrMessage,NULL);

		m_pRealMes->Log(cstrMessage);
		m_pRealMes->m_record.writeHttpFile(cstrMessage);
		MessageBox(cstrMessage);

		return 0;
	}

	cstrMessage.Format(_T("MESReportEquipmentRecipeParametersInfo:返回码：%d\n上传信息：\t%s\n"),iRet,strSendDate.c_str());
	m_pRealMes->m_record.writeHttpFile(cstrMessage);
	cstrMessage.Format(_T("配方上传成功！\n上传配方号：%s\t配方版本：%s"),m_cstrRecipeCode,m_cstrRecipeVison);
	MessageBox(cstrMessage);

	return 0;
}
void CDlgRecipes::Soap_UploadRecipe()
{
	CString cstrMessage;
	int iRet(-1),nCode(-11);
	std::string strMsg;
	
	cstrMessage.Format(_T("请确认是否要上传当前配方！\n配方号：%s\t配方版本：%s"),m_cstrRecipeCode,m_cstrRecipeVison);

	if (IDYES != MessageBox(cstrMessage,_T("请确认是否上传当前配方"),MB_ICONEXCLAMATION|MB_YESNO ))
	{
		return;
	}

	try
	{
		std::string strProductCode,strRecipeCode,strVersion;
		ParamRecipe paramRecipe;
		neb::CJsonObject nebCJsonReciev;
		neb::CJsonObject cJsonParamArray ;
		time_t sendTime;
		time(&sendTime);
		m_pRealMes->m_cJsonRecipes.Get("ProductCode",strProductCode);
		m_pRealMes->m_cJsonRecipes.Get("RecipeCode",strRecipeCode);
		m_pRealMes->m_cJsonRecipes.Get("Version",strVersion);

		paramRecipe.cstrProductCode.Format("%s", strProductCode.c_str());
		paramRecipe.cstrRecipeCode.Format("%s",strRecipeCode.c_str());
		paramRecipe.cstrVersion.Format("%s",strVersion.c_str());

		int iNum = 0;
		m_pRealMes->m_cJsonRecipes.Get("Parameters",cJsonParamArray);
		iNum = cJsonParamArray.GetArraySize();
		for (int i = 0 ;i < iNum; i++)
		{
			std::string strParameterCode,strParameterMinValue,strParameterMaxValue,strParameterValue;
			neb::CJsonObject pJsonParams;
			ParamRecipeBase recipeBase;
			cJsonParamArray.Get(i,pJsonParams);
			pJsonParams.Get("ParameterCode",strParameterCode);
			pJsonParams.Get("ParameterMinValue",strParameterMinValue);
			pJsonParams.Get("ParameterMaxValue",strParameterMaxValue);
			pJsonParams.Get("ParameterValue",strParameterValue);

			recipeBase.cstrParamCode.Format("%s",strParameterCode.c_str());
			recipeBase.cstrParamLower.Format("%s",strParameterMinValue.c_str());
			recipeBase.cstrParamUpper.Format("%s",strParameterMaxValue.c_str());
			recipeBase.cstrParamValue.Format("%s",strParameterValue.c_str());
			recipeBase.timeStamp = sendTime;
			paramRecipe.vecParamList.push_back(recipeBase);
		}

		iRet = m_pRealMes->Soap_recipe(paramRecipe,nebCJsonReciev);
		nebCJsonReciev.Get("code",iRet);
		nebCJsonReciev.Get("msg",strMsg);

		if (SOAP_OK != iRet)
		{
			cstrMessage.Format(_T("配方上传失败！错误码：%d\n 错误信息：%s\n"),iRet,strMsg.c_str());
			MessageBox(cstrMessage);
			return ;
		}
	}
	catch (...)
	{
		cstrMessage.Format(_T("配方上传失败！错误码：%d\n 错误信息：%s\n"),iRet,strMsg.c_str());
		MessageBox(cstrMessage);
		return ;
	}

	cstrMessage.Format(_T("配方上传成功！\n上传配方号：%s\t配方版本：%s"),m_cstrRecipeCode,m_cstrRecipeVison);
	MessageBox(cstrMessage);

	return ;
}
void CDlgRecipes::OnBnClickedButtonDownload()
{
	if (FALSE)
	{
		UpLoadRecipe();
	}
	else
	{
		Soap_UploadRecipe();
	}
	return;
}


int CDlgRecipes::UpdateCurrentRecipeList(void)
{
	neb::CJsonObject cJsonParamArray ;
	neb::CJsonObject* pJsonParams = NULL;
	int iNum = 0;
	m_pRealMes->m_cJsonRecipes.Get("Parameters",cJsonParamArray);
	iNum = cJsonParamArray.GetArraySize();
	pJsonParams = new neb::CJsonObject[iNum];

	for(int i = 0 ; i < iNum ; i++)
	{
		CString strIndex;

		strIndex.Format(_T("%d"),i+1);

		m_ListCrrentRecipe.InsertItem(i+1,strIndex);
	}

	for (int i = 0; i< iNum ; i++)
	{
		std::string strParameterCode,strParameterMinValue,strParameterMaxValue,strParameterValue;
		CString cstrParameterCode;
		cJsonParamArray.Get(i,pJsonParams[i]);
		pJsonParams[i].Get("ParameterCode",strParameterCode);
		pJsonParams[i].Get("ParameterMinValue",strParameterMinValue);
		pJsonParams[i].Get("ParameterMaxValue",strParameterMaxValue);
		pJsonParams[i].Get("ParameterValue",strParameterValue);

		int nIndex = -1;

		int iLenght = sizeof(g_cstrRecipeParamCode)/sizeof(g_cstrRecipeParamCode[0]);

		cstrParameterCode.Format("%s",strParameterCode.c_str());

		for (int i = 0; i < iLenght ;i++)
		{
			if (cstrParameterCode == g_cstrRecipeParamCode[i])
			{
				nIndex = i;
			}
		}

		if (nIndex < 0 || nIndex >= iLenght	)
		{
			continue;
		}

		int iIndex = 0;

		if (nIndex < iNum)
		{
			iIndex = nIndex;
		}
		else
		{
			//if (m_pRealMes->m_sInit.nProductType == 1)
			{
				iIndex = iNum + nIndex - iLenght;
			}
			//if (m_pRealMes->m_sInit.nProductType == 2)
			{
				iIndex = i;
			}
			
		}

		m_ListCrrentRecipe.SetItemText(iIndex ,1,strParameterCode.c_str());
		m_ListCrrentRecipe.SetItemText(iIndex ,2,g_cstrRecipeParamName[nIndex]);
		m_ListCrrentRecipe.SetItemText(iIndex ,3,strParameterValue.c_str());
		m_ListCrrentRecipe.SetItemText(iIndex ,4,strParameterMinValue.c_str());
		m_ListCrrentRecipe.SetItemText(iIndex ,5,strParameterMaxValue.c_str());
	}

	if (pJsonParams)
	{
		delete[] pJsonParams;	
		pJsonParams = NULL;
	}
	return 1;
}


void CDlgRecipes::OnBnClickedButtonUpload()
{
	 UpdateData(TRUE);

	 m_pRealMes->m_strRecipeCode = m_cstrRecipeCode;
	 m_pRealMes->m_strRecipeVison = m_cstrRecipeVison;
	 m_pRealMes->m_strProductCode = m_cstrProductCode;

	 try
	 {
		 WritePrivateProfileString("RecipeInfo","RecipeCode",m_cstrRecipeCode, m_pRealMes->m_strConfigFile);
		 WritePrivateProfileString("RecipeInfo","ProdctCode",m_cstrProductCode, m_pRealMes->m_strConfigFile);
		 WritePrivateProfileString("RecipeInfo","RecipeVison",m_cstrRecipeVison, m_pRealMes->m_strConfigFile);
	 }
	 catch(...)
	 {
		 MessageBox("配方版本下载失败！","配方管理",MB_OK );
	 }

	 MessageBox("配方版本下载成功！","配方管理",MB_OK );
}
