#include "StdAfx.h"
#include "ProductTool.h"

CProductTool::CProductTool()
	:m_hvcOffsetData(1)
{
	m_strCurrentProductName = GetIniString("Info","ProductName","PName");
	m_strCurrentProductFolderPath.Format("%s//Config//Products//%s",GetCurrentAppPath(),m_strCurrentProductName);
	if (!PathFileExists(m_strCurrentProductFolderPath))
	{
		CString strMessageBox;
		strMessageBox.Format("%s\r\nProduct Folder not exists,program exits\r\n配方文件夹不存在，程序将退出",m_strCurrentProductFolderPath);
		AfxMessageBox(strMessageBox);
		exit(1);
	}

	m_strSpotInspectionParamPath.Format("%s//Config//Public//Check//CheckingParam.ini");

	m_hvcOffsetData.Clear();
}

CProductTool::~CProductTool()
{

}

BOOL CProductTool::RenameFolder(LPCTSTR lpszFromPath,LPCTSTR lpszToPath)
{
	int nLengthFrm = strlen(lpszFromPath);

	char *NewPathFrm = new char[nLengthFrm + 2];

	strcpy(NewPathFrm, lpszFromPath);

	NewPathFrm[nLengthFrm] = '\0';

	NewPathFrm[nLengthFrm + 1] = '\0';

	SHFILEOPSTRUCT FileOp;

	ZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_NOCONFIRMATION;

	FileOp.hNameMappings = NULL;

	FileOp.hwnd = NULL;

	FileOp.lpszProgressTitle = NULL;

	FileOp.pFrom = NewPathFrm;

	FileOp.pTo = lpszToPath;

	FileOp.wFunc = FO_MOVE;

	return SHFileOperation(&FileOp) == 0;
}

BOOL CProductTool::Init()
{
	ReadParam();
	return TRUE;
}

BOOL CProductTool::ReadParam()
{
	m_nProductCount = GetIniInt("Product", "Count", 1);

	m_dAutoCompenLimit = GetIniDouble("Product", "AutoCompenLimit", 0.2);

	CLockHelper lk(&m_csLock);
	m_vecProductList.clear();
	for (int i = 0; i < m_nProductCount; i++)
	{
		CString strName;
		strName.Format("Product%d", i);
		CString strPDName = GetIniString("Product", strName, "PName");

		if (m_strCurrentProductName == strPDName)
			m_nCurrentProductSN = i;

		m_vecProductList.push_back(GetPDInfoFromFile(strPDName));
	}

	m_tCurrentPdInfo = m_vecProductList[m_nCurrentProductSN];



	return TRUE;
}

BOOL CProductTool::ReadSpotInspectionParam()
{
	//点检参数
	
	
	

	return TRUE;
}

BOOL CProductTool::WriteParam()
{
	DeleteIniSection("Product");
	m_nProductCount = m_vecProductList.size();
	WriteIniInt("Product", "Count", m_nProductCount);
	WriteIniString("Info", "ProductName", m_strCurrentProductName);

	WriteProductParam(m_nProductCount);

	for (int i = 0; i < m_vecProductList.size(); i++)
	{
		CString strName;
		strName.Format("Product%d", i);
		WriteIniString("Product", strName, m_vecProductList[i].strPDName);
	}

	WriteIniDouble("Product", "AutoCompenLimit", m_dAutoCompenLimit);

	return TRUE;
}

BOOL CProductTool::AddNew(const CString &strPDName,const CString &strNote)
{
	if (strPDName == m_strCurrentProductName)
		return FALSE;
	if (!PathFileExists(m_strCurrentProductFolderPath))
		return FALSE;
	for (int i=0;i<m_vecProductList.size();i++)
	{
		if(strPDName == m_vecProductList[i].strPDName)
			return FALSE;
	}
	m_nProductCount++;
	
	tagPDInfo inf = GetCurrentProductInfo();
	inf.strPDName = strPDName;
	inf.strNote = strNote;
	CLockHelper lk(&m_csLock);
	m_vecProductList.push_back(inf);

	CString strSrcFolder,strDstFolder;
	strSrcFolder.Format("%s//Config//Products//%s",GetCurrentAppPath(),m_strCurrentProductName);
	strDstFolder.Format("%s//Config//Products//%s",GetCurrentAppPath(),strPDName);
	CopyFolder(strSrcFolder,strDstFolder);
	WriteParam();

	return TRUE;
}

BOOL CProductTool::AddNew(const int &nSrcSN,const CString &strPDName,const CString &strNote)
{
	if(nSrcSN>=m_vecProductList.size())
		return FALSE;
	if (strPDName == m_strCurrentProductName)
		return FALSE;
	if (!PathFileExists(m_strCurrentProductFolderPath))
		return FALSE;
	for (int i=0;i<m_vecProductList.size();i++)
	{
		if(strPDName == m_vecProductList[i].strPDName)
			return FALSE;
	}

	m_nProductCount++;
	CLockHelper lk(&m_csLock);
	tagPDInfo inf = GetCurrentProductInfo();
	inf.strPDName = strPDName;
	inf.strNote = strNote;
	m_vecProductList.push_back(inf);

	CString strSrcFolder,strDstFolder;
	strSrcFolder.Format("%s//Config//Products//%s",GetCurrentAppPath(),m_vecProductList[nSrcSN].strPDName);
	strDstFolder.Format("%s//Config//Products//%s",GetCurrentAppPath(),strPDName);
	CopyFolder(strSrcFolder,strDstFolder);
	WriteParam();

	return TRUE;

}

BOOL CProductTool::Rename(const int &nSN,const CString &strNewPDName,const CString &strNewNote)
{
	if (nSN<0 || nSN>m_vecProductList.size())
		return FALSE;

	CString strPDName = m_vecProductList[nSN].strPDName;
	if(m_strCurrentProductName == strPDName)
	{
		m_vecProductList[nSN].strNote = strNewNote;
		WriteParam();
		return FALSE;
	}
	if("" == strNewPDName)
		return FALSE;

	for (int i=0;i<m_vecProductList.size();i++)
	{
		if (m_vecProductList[i].strPDName == strNewPDName && i!=nSN)
			return FALSE;
	}

	CLockHelper lk(&m_csLock);
	m_vecProductList[nSN].strPDName = strNewPDName;
	m_vecProductList[nSN].strNote = strNewNote;

	if (strPDName!=strNewPDName)
	{
		CString strOldFolder,strNewFolder;
		strOldFolder.Format("%s\\Config\\Products\\%s",GetCurrentAppPath(),strPDName);
		strNewFolder.Format("%s\\Config\\Products\\%s",GetCurrentAppPath(),strNewPDName);
		RenameFolder(strOldFolder,strNewFolder);
	}

	WriteParam();
	return TRUE;
}

BOOL CProductTool::Delete(const CString &strPDName)
{
	if (m_strCurrentProductName==strPDName)
		return FALSE;

	int nSN=-1;
	for (int i=0;i<m_vecProductList.size();i++)
	{
		if(strPDName == m_vecProductList[i].strPDName)
		{
			nSN = i;
			break;
		}
	}
	return Delete(nSN);
}

BOOL CProductTool::Delete(const int &nSN)
{
	if (nSN<0 || nSN>m_vecProductList.size())
		return FALSE;
	CString strPDName = m_vecProductList[nSN].strPDName;
	if(m_strCurrentProductName == strPDName)
		return FALSE;

	CLockHelper lk(&m_csLock);
	m_vecProductList.erase((m_vecProductList.begin()+nSN));

	CString strPath;
	strPath.Format("%s\\Config\\Products\\%s",GetCurrentAppPath(),strPDName);
	DeleteFolder(strPath);

	WriteParam();

	return TRUE;
}

BOOL CProductTool::Load(const CString &strPDName)
{
	if (m_strCurrentProductName==strPDName)
		return FALSE;

	int nSN=-1;
	for (int i=0;i<m_vecProductList.size();i++)
	{
		if(strPDName == m_vecProductList[i].strPDName)
		{
			nSN = i;
			break;
		}
	}

	return Load(nSN);
}

BOOL CProductTool::Load(const int &nSN)
{
	if (nSN<0 || nSN>m_vecProductList.size())
		return FALSE;
	if(m_strCurrentProductName == m_vecProductList[nSN].strPDName)
		return FALSE;

	m_strCurrentProductName = m_vecProductList[nSN].strPDName;
	WriteParam();

	return TRUE;
}

BOOL CProductTool::Note(const CString &strPDName,const CString &strNote)
{
	int nSN=-1;
	for (int i=0;i<m_vecProductList.size();i++)
	{
		if(strPDName == m_vecProductList[i].strPDName)
		{
			nSN = i;
			break;
		}
	}

	return Note(nSN,strNote);
}

BOOL CProductTool::Note(const int &nSN,const CString &strNote)
{
	if (nSN<0 || nSN>m_vecProductList.size())
		return FALSE;

	CLockHelper lk(&m_csLock);
	m_vecProductList[nSN].strNote = strNote;
	WriteParam();

	return TRUE;
}

int CProductTool::FindName(const CString &strPDName)
{
	for (int i=0;i<m_vecProductList.size();i++)
	{
		if (m_vecProductList[i].strPDName == strPDName)
			return i;
	}
	return -1;
}

BOOL CProductTool::BackUp(const CString &strPDName)
{
	if (m_strCurrentProductName==strPDName)
		return FALSE;

	int nSN=-1;
	for (int i=0;i<m_vecProductList.size();i++)
	{
		if(strPDName == m_vecProductList[i].strPDName)
		{
			nSN = i;
			break;
		}
	}

	return BackUp(nSN);
}

BOOL CProductTool::BackUp(const int &nSN)
{
	if (nSN<0 || nSN>m_vecProductList.size())
		return FALSE;

	CString strPDName = m_vecProductList[nSN].strPDName;

	return AddNew(nSN,strPDName+"-Backup","");
}

tagPDInfo CProductTool::GetPDInfoFromFile(CString strProductName)
{
	CString strProductParamPath;
	strProductParamPath.Format("%s//Config//Products//%s//ProductParam.ini", GetCurrentAppPath(), strProductName);
	int nCount = 0;
	int nSideCount = theGlobal.m_Project.GetProjectConfig().nSideCount;
	int nSideCamCount = theGlobal.m_Project.GetProjectConfig().vecSideConfig[0].nSideCamCount;
	int nCamCount = theGlobal.m_Project.GetProjectConfig().nCamCount;
	CString strName;

	tagPDInfo inf;
	inf.strPDName = strProductName;
	inf.strNote = GetPrivateProfileCString("Info", "Note", "", strProductParamPath);
	inf.nPDType = (e_ProductType)GetPrivateProfileInt("Info", "nPDType", SixCavity, strProductParamPath);
	inf.bExistAT11 = GetPrivateProfileInt("Info", "bExistAT11", TRUE, strProductParamPath);
	inf.bExistBaseCoat = GetPrivateProfileInt("Info", "bExistBaseCoat", 0, strProductParamPath);
	inf.bExistIntermittent = GetPrivateProfileInt("Info", "bExistIntermittent", 0, strProductParamPath);
	inf.nCoatingType = (e_ProductType)GetPrivateProfileInt("Info", "nCoatingType", TwoSide, strProductParamPath);
	if (theGlobal.m_Project.GetProjectConfig().nCamCount == 1 ||
		nSideCount == 1)
		inf.nCoatingType = OneSide;
	//间歇识别
	inf.bExistIntermittent = GetPrivateProfileInt("Param", "bExistIntermittent", FALSE, strProductParamPath);
	
	//涂布宽度规格
	inf.tParam.bUseCoatWidth = GetPrivateProfileInt("Param", "bUseCoatWidth", TRUE, strProductParamPath);
	nCount = (inf.nPDType == ThreeCavity ? 2 : inf.nPDType);
	inf.tParam.vecCoatWidth.clear();
	for (int i = 0; i < nCount; i++)
	{
		tagTolerance t;
		strName.Format("CoatWidth%dStandard", i);
		t.dStandard = GetPrivateProfileDouble("Param", strName, 255, strProductParamPath);
		strName.Format("CoatWidth%dTolerance", i);
		t.dTolerance = max(0.01,GetPrivateProfileDouble("Param", strName, 3, strProductParamPath));
		strName.Format("CoatWidth%dWarnTolerance", i);
		t.dWarnTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
		strName.Format("CoatWidth%dFAITolerance", i);
		t.dFAITolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
		t.OffsetList.clear();
		for (int k = 0; k < nSideCount; k++)
		{
			strName.Format("CoatWidth%dOffset%d", i, k);
			t.OffsetList.push_back(GetPrivateProfileDouble("Param", strName, 0, strProductParamPath));
		}
		strName.Format("CoatWidth%dOffsetOL%d", i, 0);
		t.dOffsetOL = GetPrivateProfileDouble("Param", strName, 0, strProductParamPath);
		inf.tParam.vecCoatWidth.push_back(t);
	}

	//箔材宽度规格
	inf.tParam.bUseFoilWidth = GetPrivateProfileInt("Param", "bUseFoilWidth", TRUE, strProductParamPath);
	nCount = (inf.nPDType == ThreeCavity ? 2 : inf.nPDType + 1);
	inf.tParam.vecFoilWidth.clear();
	for (int i = 0; i < nCount; i++)
	{
		tagTolerance t;
		strName.Format("FoilWidth%dStandard", i);
		t.dStandard = GetPrivateProfileDouble("Param", strName, 30, strProductParamPath);
		strName.Format("FoilWidth%dTolerance", i);
		t.dTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.1, strProductParamPath));
		strName.Format("FoilWidth%dWarnTolerance", i);
		t.dWarnTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
		strName.Format("FoilWidth%dFAITolerance", i);
		t.dFAITolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
		t.OffsetList.clear();
		for (int k = 0; k < nSideCount; k++)
		{
			strName.Format("FoilWidth%dOffset%d", i, k);
			t.OffsetList.push_back(GetPrivateProfileDouble("Param", strName, 0, strProductParamPath));
		}
		strName.Format("FoilWidth%dOffsetOL%d", i, 0);
		t.dOffsetOL = GetPrivateProfileDouble("Param", strName, 0, strProductParamPath);
		inf.tParam.vecFoilWidth.push_back(t);
	}

	//AT11宽度规格
	if (inf.bExistAT11)
	{
		inf.tParam.bUseAT11Width = GetPrivateProfileInt("Param", "bUseAT11Width", TRUE, strProductParamPath);
		nCount = (inf.nPDType == ThreeCavity ? 2 : inf.nPDType * 2);
		inf.tParam.vecAT11Width.clear();
		for (int i = 0; i < nCount; i++)
		{
			tagTolerance t;
			strName.Format("AT11Width%dStandard", i);
			t.dStandard = GetPrivateProfileDouble("Param", strName, 5, strProductParamPath);
			strName.Format("AT11Width%dTolerance", i);
			t.dTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.1, strProductParamPath));
			strName.Format("AT11Width%dWarnTolerance", i);
			t.dWarnTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
			strName.Format("AT11Width%dFAITolerance", i);
			t.dFAITolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
			t.OffsetList.clear();
			for (int k = 0; k < nSideCount; k++)
			{
				strName.Format("AT11Width%dOffset%d", i, k);
				t.OffsetList.push_back(GetPrivateProfileDouble("Param", strName, 0, strProductParamPath));
			}
			strName.Format("AT11Width%dOffsetOL%d", i, 0);
			t.dOffsetOL = GetPrivateProfileDouble("Param", strName, 0, strProductParamPath);
			inf.tParam.vecAT11Width.push_back(t);
		}
	}
	
	//凹版宽度规格
	if (inf.bExistBaseCoat)
	{
		inf.tParam.bUseBaseCoatWidth = GetPrivateProfileInt("Param", "bUseBaseCoatWidth", TRUE, strProductParamPath);
		nCount = (inf.nPDType == ThreeCavity ? 2 : inf.nPDType * 2);
		inf.tParam.vecBaseCoatWidth.clear();
		for (int i = 0; i < nCount; i++)
		{
			tagTolerance t;
			strName.Format("BaseCoatWidth%dStandard", i);
			t.dStandard = GetPrivateProfileDouble("Param", strName, 5, strProductParamPath);
			strName.Format("BaseCoatWidth%dTolerance", i);
			t.dTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.1, strProductParamPath));
			strName.Format("BaseCoatWidth%dWarnTolerance", i);
			t.dWarnTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
			strName.Format("BaseCoatWidth%dFAITolerance", i);
			t.dFAITolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
			t.OffsetList.clear();
			for (int k = 0; k < nSideCount; k++)
			{
				strName.Format("BaseCoatWidth%dOffset%d", i, k);
				t.OffsetList.push_back(GetPrivateProfileDouble("Param", strName, 0, strProductParamPath));
			}
			strName.Format("BaseCoatWidth%dOffsetOL%d", i, 0);
			t.dOffsetOL = GetPrivateProfileDouble("Param", strName, 0, strProductParamPath);
			inf.tParam.vecBaseCoatWidth.push_back(t);
		}
	}

	//A/B面错位值检测，只求涂膜宽度
	//if (inf.nCoatingType == TwoSide)
	{
		inf.tParam.bUseMisalignment = GetPrivateProfileInt("Param", "bUseMisalignment", TRUE, strProductParamPath);
		
		nCount = (inf.nPDType == ThreeCavity ? 3 : inf.nPDType * 2);
		inf.tParam.vecMisalignment.clear();
		for (int i = 0; i < nCount; i++)
		{
			tagTolerance t;
			strName.Format("Misalignment%dStandard", i);
			t.dStandard = GetPrivateProfileDouble("Param", strName, 0, strProductParamPath);
			strName.Format("Misalignment%dTolerance", i);
			t.dTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.1, strProductParamPath));
			strName.Format("Misalignment%dWarnTolerance", i);
			t.dWarnTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
			strName.Format("Misalignment%dFAITolerance", i);
			t.dFAITolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, strProductParamPath));
			t.OffsetList.clear();
			for (int k = 0; k < nSideCount; k++)
			{
				strName.Format("Misalignment%dOffset%d", i, k);
				t.OffsetList.push_back(GetPrivateProfileDouble("Param", strName, 0, strProductParamPath));
			}
			strName.Format("Misalignment%dOffsetOL%d", i, 0);
			t.dOffsetOL = GetPrivateProfileDouble("Param", strName, 0, strProductParamPath);
			inf.tParam.vecMisalignment.push_back(t);
		}
	}

	//点检参数
	CString stCheckingParamPath;
	stCheckingParamPath.Format("%s//Config//Public//Check//CheckingParam.ini", GetCurrentAppPath());
	inf.tParam.bUseChecking = GetPrivateProfileInt("Param", "bUseChecking", FALSE, stCheckingParamPath);
	inf.tParam.vecChecking.clear();
	for (int i = 0; i < 2 * 3 + 1; i++)//每个相机7个参数，4个点检目标的宽，3个点检间距
	{
		tagTolerance t;
		strName.Format("Checking%dStandard", i);
		t.dStandard = GetPrivateProfileDouble("Param", strName, 0, stCheckingParamPath);
		strName.Format("Checking%dStandard2", i);
		t.dStandard2 = GetPrivateProfileDouble("Param", strName, 0, stCheckingParamPath);
		strName.Format("Checking%dTolerance", i);
		t.dTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.1, stCheckingParamPath));
		strName.Format("Checking%dWarnTolerance", i);
		t.dWarnTolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, stCheckingParamPath));
		strName.Format("Checking%dFAITolerance", i);
		t.dFAITolerance = max(0.01, GetPrivateProfileDouble("Param", strName, 0.3, stCheckingParamPath));
		t.OffsetList.clear();
		for (int k = 0; k < 2/* * 2 + 1*/; k++)//每面的相机数量对应补偿数量
		{
			strName.Format("Checking%dOffset%d", i, k);
			t.OffsetList.push_back(GetPrivateProfileDouble("Param", strName, 0, stCheckingParamPath));
		}
		inf.tParam.vecChecking.push_back(t);
	}

	//瑕疵检测
	inf.tParam.bUseSurfaceDetect = GetPrivateProfileInt("Param", "bUseSurfaceDetect", FALSE, strProductParamPath);

	//打标/报警参数
	nCount = GetIniInt("Error", "CountType", 1, CBaseTmpl::CommonFile);
	for (int i = 0; i < nCount; i++)
	{
		tagSingleLabelParam tParam;
		strName.Format("NGNum%d", i);
		tParam.nNGNum = GetPrivateProfileInt("Param", strName, 5, strProductParamPath);
		inf.tLabelParam.ParamList.push_back(tParam);
		/*strName.Format("NGSize%d", i);
		tParam.dNGSize = GetPrivateProfileInt("Param", strName, 5, strProductParamPath);
		strName.Format("NGArea%d", i);
		tParam.dNGArea = GetPrivateProfileInt("Param", strName, 5, strProductParamPath);*/
	}

	//测量线数量
	if (inf.bExistAT11 || inf.bExistBaseCoat)
		inf.nMeasureLineNum = (inf.nPDType == ThreeCavity ? 7 : inf.nPDType * 4 + 1) + 1;
	else
		inf.nMeasureLineNum = (inf.nPDType == ThreeCavity ? 7 : inf.nPDType * 2 + 1) + 1;

	//生成测量名称
	int nCnt[3] = { 1,1,1 };
	strName.Format("极耳-%d", nCnt[0]++);
	inf.vecNameList.push_back(strName);
	for (int i = 0; i < inf.tParam.vecCoatWidth.size(); i++)
	{
		if (inf.bExistAT11)
		{
			strName.Format("AT11-%d", nCnt[1]++);
			inf.vecNameList.push_back(strName);
		}
		else if (inf.bExistBaseCoat)
		{
			strName.Format("凹版-%d", nCnt[1]++);
			inf.vecNameList.push_back(strName);
		}
		strName.Format("涂布-%d", nCnt[2]++);
		inf.vecNameList.push_back(strName);
		if (inf.bExistAT11)
		{
			strName.Format("AT11-%d", nCnt[1]++);
			inf.vecNameList.push_back(strName);
		}
		else if (inf.bExistBaseCoat)
		{
			strName.Format("凹版-%d", nCnt[1]++);
			inf.vecNameList.push_back(strName);
		}
		strName.Format("极耳-%d", nCnt[0]++);
		inf.vecNameList.push_back(strName);
	}


	return inf;
}

BOOL CProductTool::WriteProductParam(const int& nSN)
{
	if (nSN > m_nProductCount - 1) return FALSE;

	tagPDInfo inf = m_vecProductList[nSN];
	CString strName;
	strName.Format("Product%d", nSN);
	WriteIniString("Product", strName, m_vecProductList[nSN].strPDName);

	CString strProductParamPath, strProductFolder, strOffsetPath, strCheckingParamPath, strCkProductParamPath;
	strProductFolder.Format("%s//Config//Products//%s//", GetCurrentAppPath(), m_vecProductList[nSN].strPDName);
	if (!PathFileExists(strProductFolder))
		CreateMultilevelPath(strProductFolder);
	strProductParamPath.Format("%s//ProductParam.ini", strProductFolder);

	WritePrivateProfileString("Info", "Note", inf.strPDName, strProductParamPath);
	WritePrivateProfileString("Info", "Note", inf.strNote, strProductParamPath);
	WritePrivateProfileInt("Info", "bExistAT11", inf.bExistAT11, strProductParamPath);
	WritePrivateProfileInt("Info", "bExistBaseCoat", inf.bExistBaseCoat, strProductParamPath);
	WritePrivateProfileInt("Info", "bExistIntermittent", inf.bExistIntermittent, strProductParamPath);
	WritePrivateProfileInt("Info", "nPDType", inf.nPDType, strProductParamPath);
	WritePrivateProfileInt("Info", "nCoatingType", inf.nCoatingType, strProductParamPath);
	//间歇识别
	WritePrivateProfileInt("Param", "bExistIntermittent", inf.bExistIntermittent, strProductParamPath);

	//涂布宽度规格
	WritePrivateProfileInt("Param", "bUseCoatWidth", inf.tParam.bUseCoatWidth, strProductParamPath);
	for (int i = 0; i < inf.tParam.vecCoatWidth.size(); i++)
	{
		tagTolerance t = inf.tParam.vecCoatWidth[i];
		strName.Format("CoatWidth%dStandard", i);
		WritePrivateProfileDouble("Param", strName, t.dStandard, strProductParamPath);
		strName.Format("CoatWidth%dTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dTolerance, strProductParamPath);
		strName.Format("CoatWidth%dWarnTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dWarnTolerance, strProductParamPath);
		strName.Format("CoatWidth%dFAITolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dFAITolerance, strProductParamPath);
		for (int k = 0; k < t.OffsetList.size(); k++)
		{
			strName.Format("CoatWidth%dOffset%d", i, k);
			WritePrivateProfileDouble("Param", strName, t.OffsetList[k], strProductParamPath);
		}
		strName.Format("CoatWidth%dOffsetOL%d", i, 0);
		WritePrivateProfileDouble("Param", strName, t.dOffsetOL, strProductParamPath);
	}

	//箔材宽度规格
	inf.tParam.bUseFoilWidth = GetPrivateProfileInt("Param", "bUseFoilWidth", TRUE, strProductParamPath);
	for (int i = 0; i < inf.tParam.vecFoilWidth.size(); i++)
	{
		tagTolerance t = inf.tParam.vecFoilWidth[i];
		strName.Format("FoilWidth%dStandard", i);
		WritePrivateProfileDouble("Param", strName, t.dStandard, strProductParamPath);
		strName.Format("FoilWidth%dTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dTolerance, strProductParamPath);
		strName.Format("FoilWidth%dWarnTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dWarnTolerance, strProductParamPath);
		strName.Format("FoilWidth%dFAITolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dFAITolerance, strProductParamPath);
		for (int k = 0; k < t.OffsetList.size(); k++)
		{
			strName.Format("FoilWidth%dOffset%d", i, k);
			WritePrivateProfileDouble("Param", strName, t.OffsetList[k], strProductParamPath);
		}
		strName.Format("FoilWidth%dOffsetOL%d", i, 0);
		WritePrivateProfileDouble("Param", strName, t.dOffsetOL, strProductParamPath);
	}

	//AT11宽度规格
	if (inf.bExistAT11)
	{
		WritePrivateProfileInt("Param", "bUseAT11Width", inf.tParam.bUseAT11Width, strProductParamPath);
		for (int i = 0; i < inf.tParam.vecAT11Width.size(); i++)
		{
			tagTolerance t = inf.tParam.vecAT11Width[i];
			strName.Format("AT11Width%dStandard", i);
			WritePrivateProfileDouble("Param", strName, t.dStandard, strProductParamPath);
			strName.Format("AT11Width%dTolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dTolerance, strProductParamPath);
			strName.Format("AT11Width%dWarnTolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dWarnTolerance, strProductParamPath);
			strName.Format("AT11Width%dFAITolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dFAITolerance, strProductParamPath);
			for (int k = 0; k < t.OffsetList.size(); k++)
			{
				strName.Format("AT11Width%dOffset%d", i, k);
				WritePrivateProfileDouble("Param", strName, t.OffsetList[k], strProductParamPath);
			}
			strName.Format("AT11Width%dOffsetOL%d", i, 0);
			WritePrivateProfileDouble("Param", strName, t.dOffsetOL, strProductParamPath);
		}
	}

	//凹版宽度规格
	if (inf.bExistBaseCoat)
	{
		WritePrivateProfileInt("Param", "bUseBaseCoatWidth", inf.tParam.bUseBaseCoatWidth, strProductParamPath);
		for (int i = 0; i < inf.tParam.vecBaseCoatWidth.size(); i++)
		{
			tagTolerance t = inf.tParam.vecBaseCoatWidth[i];
			strName.Format("BaseCoatWidth%dStandard", i);
			WritePrivateProfileDouble("Param", strName, t.dStandard, strProductParamPath);
			strName.Format("BaseCoatWidth%dTolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dTolerance, strProductParamPath);
			strName.Format("BaseCoatWidth%dWarnTolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dWarnTolerance, strProductParamPath);
			strName.Format("BaseCoatWidth%dFAITolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dFAITolerance, strProductParamPath);
			for (int k = 0; k < t.OffsetList.size(); k++)
			{
				strName.Format("BaseCoatWidth%dOffset%d", i, k);
				WritePrivateProfileDouble("Param", strName, t.OffsetList[k], strProductParamPath);
			}
			strName.Format("BaseCoatWidth%dOffsetOL%d", i, 0);
			WritePrivateProfileDouble("Param", strName, t.dOffsetOL, strProductParamPath);
		}
	}

	//A/B面错位值检测
	//if (inf.nCoatingType == TwoSide)
	{
		WritePrivateProfileInt("Param", "bUseMisalignment", inf.tParam.bUseMisalignment, strProductParamPath);

		for (int i = 0; i < inf.tParam.vecMisalignment.size(); i++)
		{
			tagTolerance t = inf.tParam.vecMisalignment[i];
			strName.Format("Misalignment%dStandard", i);
			WritePrivateProfileDouble("Param", strName, t.dStandard, strProductParamPath);
			strName.Format("Misalignment%dTolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dTolerance, strProductParamPath);
			strName.Format("Misalignment%dWarnTolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dWarnTolerance, strProductParamPath);
			strName.Format("Misalignment%dFAITolerance", i);
			WritePrivateProfileDouble("Param", strName, t.dFAITolerance, strProductParamPath);
			for (int k = 0; k < t.OffsetList.size(); k++)
			{
				strName.Format("Misalignment%dOffset%d", i, k);
				WritePrivateProfileDouble("Param", strName, t.OffsetList[k], strProductParamPath);
			}
			strName.Format("Misalignment%dOffsetOL%d", i, 0);
			WritePrivateProfileDouble("Param", strName, t.dOffsetOL, strProductParamPath);
		}
	}

	//点检参数
	strCheckingParamPath.Format("%s//Config//Public//Check//", GetCurrentAppPath());
	if (!PathFileExists(strCheckingParamPath))
		CreateMultilevelPath(strCheckingParamPath);
	strCkProductParamPath.Format("%s//CheckingParam.ini", strCheckingParamPath);

	WritePrivateProfileInt("Param", "bUseChecking", inf.tParam.bUseChecking, strCkProductParamPath);
	for (int i = 0; i < inf.tParam.vecChecking.size(); i++)
	{
		tagTolerance t = inf.tParam.vecChecking[i];
		strName.Format("Checking%dStandard", i);
		WritePrivateProfileDouble("Param", strName, t.dStandard, strCkProductParamPath);
		strName.Format("Checking%dStandard2", i);
		WritePrivateProfileDouble("Param", strName, t.dStandard2, strCkProductParamPath);
		strName.Format("Checking%dTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dTolerance, strCkProductParamPath);
		strName.Format("Checking%dWarnTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dWarnTolerance, strCkProductParamPath);
		strName.Format("Checking%dFAITolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dFAITolerance, strCkProductParamPath);
		for (int k = 0; k < t.OffsetList.size(); k++)
		{
			strName.Format("Checking%dOffset%d", i, k);
			WritePrivateProfileDouble("Param", strName, t.OffsetList[k], strCkProductParamPath);
		}
	}

	//瑕疵
	WritePrivateProfileInt("Param", "bUseSurfaceDetect", inf.tParam.bUseSurfaceDetect, strProductParamPath);

	//打标/报警参数
	for (int i = 0; i < inf.tLabelParam.ParamList.size(); i++)
	{
		tagSingleLabelParam tParam = inf.tLabelParam.ParamList[i];
		strName.Format("NGNum%d", i);
		WritePrivateProfileInt("Param", strName, tParam.nNGNum, strProductParamPath);

		/*strName.Format("NGSize%d", i);
		WritePrivateProfileInt("Param", strName, tParam.dNGSize, strProductParamPath);
		strName.Format("NGArea%d", i);
		WritePrivateProfileInt("Param", strName, tParam.dNGArea, strProductParamPath);*/
	}

	return TRUE;
}

BOOL CProductTool::WriteSpotInspectionParam()
{
	tagPDInfo inf = m_tCurrentPdInfo;
	//点检参数
	CString strCheckingParamPath, strCkProductParamPath, strName;
	strCheckingParamPath.Format("%s//Config//Public//Check//", GetCurrentAppPath());
	if (!PathFileExists(strCheckingParamPath))
		CreateMultilevelPath(strCheckingParamPath);
	strCkProductParamPath.Format("%s//CheckingParam.ini", strCheckingParamPath);

	WritePrivateProfileInt("Param", "bUseChecking", inf.tParam.bUseChecking, strCkProductParamPath);
	for (int i = 0; i < inf.tParam.vecChecking.size(); i++)
	{
		tagTolerance t = inf.tParam.vecChecking[i];
		strName.Format("Checking%dStandard", i);
		WritePrivateProfileDouble("Param", strName, t.dStandard, strCkProductParamPath);
		strName.Format("Checking%dStandard2", i);
		WritePrivateProfileDouble("Param", strName, t.dStandard2, strCkProductParamPath);
		strName.Format("Checking%dTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dTolerance, strCkProductParamPath);
		strName.Format("Checking%dWarnTolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dWarnTolerance, strCkProductParamPath);
		strName.Format("Checking%dFAITolerance", i);
		WritePrivateProfileDouble("Param", strName, t.dFAITolerance, strCkProductParamPath);
		for (int k = 0; k < t.OffsetList.size(); k++)
		{
			strName.Format("Checking%dOffset%d", i, k);
			WritePrivateProfileDouble("Param", strName, t.OffsetList[k], strCkProductParamPath);
		}
	}

	return TRUE;
}

BOOL CProductTool::ClearOffset(tagPDInfo& tPdInfo)
{
	int nNum = 0;
	for (int i=0;i<tPdInfo.tParam.vecCoatWidth.size();i++)
	{
		nNum = tPdInfo.tParam.vecCoatWidth[i].OffsetList.size();
		tPdInfo.tParam.vecCoatWidth[i].OffsetList.clear();
		tPdInfo.tParam.vecCoatWidth[i].OffsetList.resize(nNum, 0.0);
		tPdInfo.tParam.vecCoatWidth[i].dOffsetOL = 0.0;
	}
	for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
	{
		nNum = tPdInfo.tParam.vecFoilWidth[i].OffsetList.size();
		tPdInfo.tParam.vecFoilWidth[i].OffsetList.clear();
		tPdInfo.tParam.vecFoilWidth[i].OffsetList.resize(nNum, 0.0);
		tPdInfo.tParam.vecFoilWidth[i].dOffsetOL = 0.0;
	}
	for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
	{
		nNum = tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList.size();
		tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList.clear();
		tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList.resize(nNum, 0.0);
		tPdInfo.tParam.vecBaseCoatWidth[i].dOffsetOL = 0.0;
	}
	for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
	{
		nNum = tPdInfo.tParam.vecAT11Width[i].OffsetList.size();
		tPdInfo.tParam.vecAT11Width[i].OffsetList.clear();
		tPdInfo.tParam.vecAT11Width[i].OffsetList.resize(nNum, 0.0);
		tPdInfo.tParam.vecAT11Width[i].dOffsetOL = 0.0;
	}
	for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
	{
		nNum = tPdInfo.tParam.vecMisalignment[i].OffsetList.size();
		tPdInfo.tParam.vecMisalignment[i].OffsetList.clear();
		tPdInfo.tParam.vecMisalignment[i].OffsetList.resize(nNum, 0.0);
		tPdInfo.tParam.vecMisalignment[i].dOffsetOL = 0.0;
	}
	for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
	{
		nNum = tPdInfo.tParam.vecChecking[i].OffsetList.size();
		tPdInfo.tParam.vecChecking[i].OffsetList.clear();
		tPdInfo.tParam.vecChecking[i].OffsetList.resize(nNum, 0.0);	
	}

	return TRUE;
}

BOOL CProductTool::UpdateOnlineOffset(HTuple hvOnlineOffset)
{
	HTuple hvAT11, hvCoat, hvFoil, hvBaseCoat;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	double MAX_DIFF = abs(m_dAutoCompenLimit);

	try
	{
		if (hvOnlineOffset.Length() != tPdInfo.vecNameList.size())
		{
			CString strLog;
			strLog.Format("UpdateOnlineOffset:干湿膜接收在线补偿值与配方补偿值数量不符，请检查");
			theGlobal.m_Log.Exception(strLog);
			return FALSE;
		}
		//补偿值限制
		int nModifyCount = 0;
		for (int i = 0; i < hvOnlineOffset.Length(); i++)
		{
			if (hvOnlineOffset[i].D() > MAX_DIFF)
			{
				nModifyCount++;
				hvOnlineOffset[i] = MAX_DIFF;
			}
			else if (hvOnlineOffset[i].D() < -MAX_DIFF)
			{
				nModifyCount++;
				hvOnlineOffset[i] = -MAX_DIFF;
			}
		}
		if (nModifyCount>0)
		{
			CString strLog;
			strLog.Format("UpdateOnlineOffset: 干湿膜补偿偏差过大的已限制补偿值(±%0.2f)", MAX_DIFF);
			theGlobal.m_Log.Std(strLog);
		}

		int nCount = 0;
		hvFoil.Append(hvOnlineOffset[nCount++].D());
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			if (tPdInfo.bExistAT11)
			{
				hvAT11.Append(hvOnlineOffset[nCount++].D());
			}
			else if (tPdInfo.bExistBaseCoat)
			{
				hvBaseCoat.Append(hvOnlineOffset[nCount++].D());
			}
			hvCoat.Append(hvOnlineOffset[nCount++].D());
			if (tPdInfo.bExistAT11)
			{
				hvAT11.Append(hvOnlineOffset[nCount++].D());
			}
			else if (tPdInfo.bExistBaseCoat)
			{
				hvBaseCoat.Append(hvOnlineOffset[nCount++].D());
			}
			hvFoil.Append(hvOnlineOffset[nCount++].D());
		}

		int nEnd = 1;
		int nNum = tPdInfo.tParam.vecCoatWidth.size();
		if (nNum*nEnd == hvCoat.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				if (abs(hvCoat[j].D()) > MAX_DIFF)
				{
					tPdInfo.tParam.vecCoatWidth[j].dOffsetOL = hvCoat[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
					CString strLog;
					strLog.Format("涂布%d在线补偿已限制补偿值(%0.2f),请注意干湿膜自动补偿参数是否正确", j + 1, tPdInfo.tParam.vecCoatWidth[j].dOffsetOL);
					theGlobal.m_Log.Exception(strLog);

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecCoatWidth[j].OffsetList[0] = hvCoat[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
				}
				else
				{
					tPdInfo.tParam.vecCoatWidth[j].dOffsetOL = hvCoat[j].D();

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecCoatWidth[j].OffsetList[0] = hvCoat[j].D();
				}

			}
		}
		nNum = tPdInfo.tParam.vecFoilWidth.size();
		if (nNum*nEnd == hvFoil.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				if (abs(hvFoil[j].D()) > MAX_DIFF)
				{
					tPdInfo.tParam.vecFoilWidth[j].dOffsetOL = hvFoil[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
					CString strLog;
					strLog.Format("极耳%d在线补偿已限制补偿值%0.2fmm,请注意干湿膜自动补偿参数是否正确", j + 1, tPdInfo.tParam.vecFoilWidth[j].dOffsetOL);
					theGlobal.m_Log.Exception(strLog);

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecFoilWidth[j].OffsetList[0] = hvFoil[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
				}
				else
				{
					tPdInfo.tParam.vecFoilWidth[j].dOffsetOL = hvFoil[j].D();

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecFoilWidth[j].OffsetList[0] = hvFoil[j].D();
				}

			}
		}
		nNum = tPdInfo.tParam.vecAT11Width.size();
		if (nNum*nEnd == hvAT11.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				if (abs(hvAT11[j].D()) > MAX_DIFF)
				{
					tPdInfo.tParam.vecAT11Width[j].dOffsetOL = hvAT11[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
					CString strLog;
					strLog.Format("AT11 %d在线补已限制补偿值0.2fmm,请注意干湿膜自动补偿参数是否正确", j + 1, tPdInfo.tParam.vecAT11Width[j].dOffsetOL);
					theGlobal.m_Log.Exception(strLog);

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecAT11Width[j].OffsetList[0] = hvAT11[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
				}
				else
				{
					tPdInfo.tParam.vecAT11Width[j].dOffsetOL = hvAT11[j].D();

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecAT11Width[j].OffsetList[0] = hvAT11[j].D();
				}

			}
		}
		nNum = tPdInfo.tParam.vecBaseCoatWidth.size();
		if (nNum*nEnd == hvBaseCoat.Length())
		{
			for (int j = 0; j < nNum; j++)
			{
				if (abs(hvBaseCoat[j].D()) > MAX_DIFF)
				{
					tPdInfo.tParam.vecBaseCoatWidth[j].dOffsetOL = hvBaseCoat[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
					CString strLog;
					strLog.Format("凹版 %d在线补偿已限制补偿值0.2fmm,请注意干湿膜自动补偿参数是否正确", j + 1, tPdInfo.tParam.vecBaseCoatWidth[j].dOffsetOL);
					theGlobal.m_Log.Exception(strLog);

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecBaseCoatWidth[j].OffsetList[0] = hvBaseCoat[j].D() >= 0 ? MAX_DIFF : -MAX_DIFF;
				}
				else
				{
					tPdInfo.tParam.vecBaseCoatWidth[j].dOffsetOL = hvBaseCoat[j].D();

					if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
						tPdInfo.tParam.vecBaseCoatWidth[j].OffsetList[0] = hvBaseCoat[j].D();
				}

			}
		}

		//更新参数
		SetProductInfo(m_nCurrentProductSN, tPdInfo);

		//给算法更新产品信息
		for (int nSideSN = 0; nSideSN < theGlobal.m_Project.GetProjectConfig().nSideCount; nSideSN++)
		{
			theGlobal.m_AlgTool.DoSomething(nSideSN, CAlgorithmTool::AlgMeasure, "SetProductInfo", &tPdInfo, NULL);
		}

		if (theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen)
		{
			//首件计算补偿值完成
			theGlobal.m_Judge.m_tFirstArticleInspCount.bStartCalculateCompen = FALSE;

			SetCurrentProductInfoTmp(tPdInfo);

			WriteProductParam(m_nCurrentProductSN);

			theGlobal.SetTipInfo(TIP_FirstArticleCompenFinsh);
			CString strLog;
			strLog.Format("首件自动计算补偿值完成");
			theGlobal.m_Log.Std(strLog);
		}
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("CProductTool_UpdateOffsetOnline: 请注意干湿膜自动补偿参数是否正确");
		theGlobal.m_Log.Exception(strLog);
	}
	
	return TRUE;
}

HTuple CProductTool::GetOnlineOffset()
{
	HTuple hvReturn;
	const tagPDInfo tPdInfo = GetCurrentProductInfo();
	int nFoil(0), nCoat(0), nAT11(0), nBaseCoat(0);

	hvReturn.Append(tPdInfo.tParam.vecFoilWidth[nFoil++].dOffsetOL);
	for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
	{
		if (tPdInfo.bExistAT11)
		{
			hvReturn.Append(tPdInfo.tParam.vecAT11Width[nAT11++].dOffsetOL);
		}
		else if (tPdInfo.bExistBaseCoat)
		{
			hvReturn.Append(tPdInfo.tParam.vecBaseCoatWidth[nBaseCoat++].dOffsetOL);
		}
		hvReturn.Append(tPdInfo.tParam.vecCoatWidth[nCoat++].dOffsetOL);
		if (tPdInfo.bExistAT11)
		{
			hvReturn.Append(tPdInfo.tParam.vecAT11Width[nAT11++].dOffsetOL);
		}
		else if (tPdInfo.bExistBaseCoat)
		{
			hvReturn.Append(tPdInfo.tParam.vecBaseCoatWidth[nBaseCoat++].dOffsetOL);
		}
		hvReturn.Append(tPdInfo.tParam.vecFoilWidth[nFoil++].dOffsetOL);
	}

	return hvReturn;
}

BOOL CProductTool::ProductParamCorrected(tagPDInfo &tOldPdInfo, tagPDInfo &tNewPdInfo, BOOL bMutiCCDCheck)
{
	try
	{
		//判断参数是否修改
		m_tPdInfoCorrectedFlag.Init();
		if (tOldPdInfo.strPDName != tNewPdInfo.strPDName)
		{
			m_tPdInfoCorrectedFlag.bPDNameFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.strNote != tNewPdInfo.strNote)
		{
			m_tPdInfoCorrectedFlag.bNoteFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.bExistAT11 != tNewPdInfo.bExistAT11)
		{
			m_tPdInfoCorrectedFlag.bExistAT11Flag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.bExistBaseCoat != tNewPdInfo.bExistBaseCoat)
		{
			m_tPdInfoCorrectedFlag.bExistBaseCoatFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.bExistIntermittent != tNewPdInfo.bExistIntermittent)
		{
			m_tPdInfoCorrectedFlag.bExistIntermittentFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.nPDType != tNewPdInfo.nPDType)
		{
			m_tPdInfoCorrectedFlag.bPDTypeFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.nCoatingType != tNewPdInfo.nCoatingType)
		{
			m_tPdInfoCorrectedFlag.bCoatingTypeFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.nMeasureLineNum != tNewPdInfo.nMeasureLineNum)
		{
			m_tPdInfoCorrectedFlag.bMeasureLineNumFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}

		//*贴标/报警判断参数
		if (tOldPdInfo.tLabelParam.ParamList.size() != tNewPdInfo.tLabelParam.ParamList.size())
		{
			m_tPdInfoCorrectedFlag.bLabelParamFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			for (int i = 0; i < tOldPdInfo.tLabelParam.ParamList.size(); i++)
			{
				if (tOldPdInfo.tLabelParam.ParamList[i].dNGArea != tOldPdInfo.tLabelParam.ParamList[i].dNGArea ||
					tOldPdInfo.tLabelParam.ParamList[i].dNGSize != tOldPdInfo.tLabelParam.ParamList[i].dNGSize ||
					tOldPdInfo.tLabelParam.ParamList[i].nNGNum != tOldPdInfo.tLabelParam.ParamList[i].nNGNum)
				{
					m_tPdInfoCorrectedFlag.bLabelParamFlag = TRUE;
					m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
				}
			}
		}
		//*判定参数
		//涂布宽
		if (tOldPdInfo.tParam.bUseCoatWidth != tNewPdInfo.tParam.bUseCoatWidth)
		{
			m_tPdInfoCorrectedFlag.bUseCoatWidth = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.tParam.vecCoatWidth.size() != tNewPdInfo.tParam.vecCoatWidth.size())
		{
			m_tPdInfoCorrectedFlag.bUseCoatWidth = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			if (tOldPdInfo.tParam.bUseCoatWidth == true && tNewPdInfo.tParam.bUseCoatWidth == true)
			{
				for (int i = 0; i < tOldPdInfo.tParam.vecCoatWidth.size(); i++)
				{
					if (tOldPdInfo.tParam.vecCoatWidth[i].dOffsetOL != tNewPdInfo.tParam.vecCoatWidth[i].dOffsetOL ||
						tOldPdInfo.tParam.vecCoatWidth[i].dStandard != tNewPdInfo.tParam.vecCoatWidth[i].dStandard ||
						tOldPdInfo.tParam.vecCoatWidth[i].dTolerance != tNewPdInfo.tParam.vecCoatWidth[i].dTolerance ||
						tOldPdInfo.tParam.vecCoatWidth[i].dWarnTolerance != tNewPdInfo.tParam.vecCoatWidth[i].dWarnTolerance)
					{
						m_tPdInfoCorrectedFlag.bUseCoatWidth = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}

					if (tOldPdInfo.tParam.vecCoatWidth[i].OffsetList.size() != tNewPdInfo.tParam.vecCoatWidth[i].OffsetList.size())
					{
						m_tPdInfoCorrectedFlag.bUseCoatWidth = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}
					else
					{
						for (int j = 0; j < tOldPdInfo.tParam.vecCoatWidth[i].OffsetList.size(); j++)
						{
							if (tOldPdInfo.tParam.vecCoatWidth[i].OffsetList[j] != tNewPdInfo.tParam.vecCoatWidth[i].OffsetList[j])
							{
								m_tPdInfoCorrectedFlag.bUseCoatWidth = TRUE;
								m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
							}
						}
					}
				}
			}
		}
		//箔材宽
		if (tOldPdInfo.tParam.bUseFoilWidth != tNewPdInfo.tParam.bUseFoilWidth)
		{
			m_tPdInfoCorrectedFlag.bUseFoilWidth = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.tParam.vecFoilWidth.size() != tNewPdInfo.tParam.vecFoilWidth.size())
		{
			m_tPdInfoCorrectedFlag.bUseFoilWidth = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			if (tOldPdInfo.tParam.bUseFoilWidth == true && tNewPdInfo.tParam.bUseFoilWidth == true)
			{
				for (int i = 0; i < tOldPdInfo.tParam.vecFoilWidth.size(); i++)
				{
					if (tOldPdInfo.tParam.vecFoilWidth[i].dOffsetOL != tNewPdInfo.tParam.vecFoilWidth[i].dOffsetOL ||
						tOldPdInfo.tParam.vecFoilWidth[i].dStandard != tNewPdInfo.tParam.vecFoilWidth[i].dStandard ||
						tOldPdInfo.tParam.vecFoilWidth[i].dTolerance != tNewPdInfo.tParam.vecFoilWidth[i].dTolerance ||
						tOldPdInfo.tParam.vecFoilWidth[i].dWarnTolerance != tNewPdInfo.tParam.vecFoilWidth[i].dWarnTolerance)
					{
						m_tPdInfoCorrectedFlag.bUseFoilWidth = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}

					if (tOldPdInfo.tParam.vecFoilWidth[i].OffsetList.size() != tNewPdInfo.tParam.vecFoilWidth[i].OffsetList.size())
					{
						m_tPdInfoCorrectedFlag.bUseFoilWidth = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}
					else
					{
						for (int j = 0; j < tOldPdInfo.tParam.vecFoilWidth[i].OffsetList.size(); j++)
						{
							if (tOldPdInfo.tParam.vecFoilWidth[i].OffsetList[j] != tNewPdInfo.tParam.vecFoilWidth[i].OffsetList[j])
							{
								m_tPdInfoCorrectedFlag.bUseFoilWidth = TRUE;
								m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
							}
						}
					}
				}
			}
		}
		//AT9/AT11宽检测
		if (tOldPdInfo.tParam.bUseAT11Width != tNewPdInfo.tParam.bUseAT11Width)
		{
			m_tPdInfoCorrectedFlag.bUseAT11Width = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.tParam.vecAT11Width.size() != tNewPdInfo.tParam.vecAT11Width.size())
		{
			m_tPdInfoCorrectedFlag.bUseAT11Width = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			if (tOldPdInfo.tParam.bUseAT11Width == true && tNewPdInfo.tParam.bUseAT11Width == true)
			{
				for (int i = 0; i < tOldPdInfo.tParam.vecAT11Width.size(); i++)
				{
					if (tOldPdInfo.tParam.vecAT11Width[i].dOffsetOL != tNewPdInfo.tParam.vecAT11Width[i].dOffsetOL ||
						tOldPdInfo.tParam.vecAT11Width[i].dStandard != tNewPdInfo.tParam.vecAT11Width[i].dStandard ||
						tOldPdInfo.tParam.vecAT11Width[i].dTolerance != tNewPdInfo.tParam.vecAT11Width[i].dTolerance ||
						tOldPdInfo.tParam.vecAT11Width[i].dWarnTolerance != tNewPdInfo.tParam.vecAT11Width[i].dWarnTolerance)
					{
						m_tPdInfoCorrectedFlag.bUseAT11Width = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}

					if (tOldPdInfo.tParam.vecAT11Width[i].OffsetList.size() != tNewPdInfo.tParam.vecAT11Width[i].OffsetList.size())
					{
						m_tPdInfoCorrectedFlag.bUseAT11Width = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}
					else
					{
						for (int j = 0; j < tOldPdInfo.tParam.vecAT11Width[i].OffsetList.size(); j++)
						{
							if (tOldPdInfo.tParam.vecAT11Width[i].OffsetList[j] != tNewPdInfo.tParam.vecAT11Width[i].OffsetList[j])
							{
								m_tPdInfoCorrectedFlag.bUseAT11Width = TRUE;
								m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
							}
						}
					}

				}
			}
		}
		//底涂检测
		if (tOldPdInfo.tParam.bUseBaseCoatWidth != tNewPdInfo.tParam.bUseBaseCoatWidth)
		{
			m_tPdInfoCorrectedFlag.bUseBaseCoatWidth = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.tParam.vecBaseCoatWidth.size() != tNewPdInfo.tParam.vecBaseCoatWidth.size())
		{
			m_tPdInfoCorrectedFlag.bUseBaseCoatWidth = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			if (tOldPdInfo.tParam.bUseBaseCoatWidth == true && tNewPdInfo.tParam.bUseBaseCoatWidth == true)
			{
				for (int i = 0; i < tOldPdInfo.tParam.vecBaseCoatWidth.size(); i++)
				{
					if (tOldPdInfo.tParam.vecBaseCoatWidth[i].dOffsetOL != tNewPdInfo.tParam.vecBaseCoatWidth[i].dOffsetOL ||
						tOldPdInfo.tParam.vecBaseCoatWidth[i].dStandard != tNewPdInfo.tParam.vecBaseCoatWidth[i].dStandard ||
						tOldPdInfo.tParam.vecBaseCoatWidth[i].dTolerance != tNewPdInfo.tParam.vecBaseCoatWidth[i].dTolerance ||
						tOldPdInfo.tParam.vecBaseCoatWidth[i].dWarnTolerance != tNewPdInfo.tParam.vecBaseCoatWidth[i].dWarnTolerance)
					{
						m_tPdInfoCorrectedFlag.bUseBaseCoatWidth = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}

					if (tOldPdInfo.tParam.vecBaseCoatWidth[i].OffsetList.size() != tNewPdInfo.tParam.vecBaseCoatWidth[i].OffsetList.size())
					{
						m_tPdInfoCorrectedFlag.bUseBaseCoatWidth = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}
					else
					{
						for (int j = 0; j < tOldPdInfo.tParam.vecBaseCoatWidth[i].OffsetList.size(); j++)
						{
							if (tOldPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[j] != tNewPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[j])
							{
								m_tPdInfoCorrectedFlag.bUseBaseCoatWidth = TRUE;
								m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
							}
						}
					}
				}
			}
		}
		//错位检测
		if (tOldPdInfo.tParam.bUseMisalignment != tNewPdInfo.tParam.bUseMisalignment)
		{
			m_tPdInfoCorrectedFlag.bUseMisalignment = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.tParam.vecMisalignment.size() != tNewPdInfo.tParam.vecMisalignment.size())
		{
			m_tPdInfoCorrectedFlag.bUseMisalignment = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			if (tOldPdInfo.tParam.bUseMisalignment == true && tNewPdInfo.tParam.bUseMisalignment == true)
			{
				for (int i = 0; i < tOldPdInfo.tParam.vecCoatWidth.size(); i++)
				{
					if (tOldPdInfo.tParam.vecMisalignment[i].dOffsetOL != tNewPdInfo.tParam.vecMisalignment[i].dOffsetOL ||
						tOldPdInfo.tParam.vecMisalignment[i].dStandard != tNewPdInfo.tParam.vecMisalignment[i].dStandard ||
						tOldPdInfo.tParam.vecMisalignment[i].dTolerance != tNewPdInfo.tParam.vecMisalignment[i].dTolerance ||
						tOldPdInfo.tParam.vecMisalignment[i].dWarnTolerance != tNewPdInfo.tParam.vecMisalignment[i].dWarnTolerance)
					{
						m_tPdInfoCorrectedFlag.bUseMisalignment = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}

					if (tOldPdInfo.tParam.vecMisalignment[i].OffsetList.size() != tNewPdInfo.tParam.vecMisalignment[i].OffsetList.size())
					{
						m_tPdInfoCorrectedFlag.bUseMisalignment = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}
					else
					{
						for (int j = 0; j < tOldPdInfo.tParam.vecMisalignment[i].OffsetList.size(); j++)
						{
							if (tOldPdInfo.tParam.vecMisalignment[i].OffsetList[j] != tNewPdInfo.tParam.vecMisalignment[i].OffsetList[j])
							{
								m_tPdInfoCorrectedFlag.bUseMisalignment = TRUE;
								m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
							}
						}
					}

				}
			}
		}
		//设备点检
		if (tOldPdInfo.tParam.bUseChecking != tNewPdInfo.tParam.bUseChecking)
		{
			m_tPdInfoCorrectedFlag.bUseChecking = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		if (tOldPdInfo.tParam.vecChecking.size() != tNewPdInfo.tParam.vecChecking.size())
		{
			m_tPdInfoCorrectedFlag.bUseChecking = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			if (tOldPdInfo.tParam.bUseChecking == true && tNewPdInfo.tParam.bUseChecking == true)
			{
				for (int i = 0; i < tOldPdInfo.tParam.vecChecking.size(); i++)
				{
					if (tOldPdInfo.tParam.vecChecking[i].dOffsetOL != tNewPdInfo.tParam.vecChecking[i].dOffsetOL ||
						tOldPdInfo.tParam.vecChecking[i].dStandard != tNewPdInfo.tParam.vecChecking[i].dStandard ||
						tOldPdInfo.tParam.vecChecking[i].dTolerance != tNewPdInfo.tParam.vecChecking[i].dTolerance ||
						tOldPdInfo.tParam.vecChecking[i].dWarnTolerance != tNewPdInfo.tParam.vecChecking[i].dWarnTolerance)
					{
						m_tPdInfoCorrectedFlag.bUseChecking = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}

					if (tOldPdInfo.tParam.vecChecking[i].OffsetList.size() != tNewPdInfo.tParam.vecChecking[i].OffsetList.size())
					{
						m_tPdInfoCorrectedFlag.bUseChecking = TRUE;
						m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
					}
					else
					{
						for (int j = 0; j < tOldPdInfo.tParam.vecChecking[i].OffsetList.size(); j++)
						{
							if (tOldPdInfo.tParam.vecChecking[i].OffsetList[j] != tNewPdInfo.tParam.vecChecking[i].OffsetList[j])
							{
								m_tPdInfoCorrectedFlag.bUseChecking = TRUE;
								m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
							}
						}
					}

				}
			}
		}
		////表面缺陷检测-配置文件并没有保存这项，会导致return true
		//if (tOldPdInfo.tParam.bUseSurfaceDetect != tNewPdInfo.tParam.bUseSurfaceDetect)
		//return TRUE;

		//*显示名称
		if (tOldPdInfo.vecNameList.size() != tNewPdInfo.vecNameList.size())
		{
			m_tPdInfoCorrectedFlag.bNameListFlag = TRUE;
			m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
		}
		else
		{
			for (int i = 0; i < tOldPdInfo.vecNameList.size(); i++)
			{
				if (tOldPdInfo.vecNameList[i] != tNewPdInfo.vecNameList[i])
				{
					m_tPdInfoCorrectedFlag.bNameListFlag = TRUE;
					m_tPdInfoCorrectedFlag.bAllFlag = TRUE;
				}
			}
		}
		//记录配方修改信息
		if (m_tPdInfoCorrectedFlag.bAllFlag)
		{
			SaveProductParamModifiedRecord("更新前", tOldPdInfo, bMutiCCDCheck);
			SaveProductParamModifiedRecord("更新后", tNewPdInfo, bMutiCCDCheck);
		}
	}
	catch (const std::exception&)
	{
		CString strLog;
		strLog.Format("CPageProductSet_ProductParamCorrected异常");
		theGlobal.m_Log.Log(strLog, ExcpLog);
		return FALSE;
	}
	catch (...)
	{
		CString strLog;
		strLog.Format("CPageProductSet_ProductParamCorrected异常");
		theGlobal.m_Log.Log(strLog, ExcpLog);
		return FALSE;
	}
	return m_tPdInfoCorrectedFlag.bAllFlag;
}

BOOL CProductTool::SaveProductParamModifiedRecord(CString sName, tagPDInfo &tPdInfo, BOOL bMutiCCDCheck)
{
	try
	{
		//时间
		SYSTEMTIME	tSaveTime;
		::GetLocalTime(&tSaveTime);
		CString sTime;

		//获取当前用户
		tagUSERINFO user = theGlobal.m_Permission.GetUserInfo();

		CString strPath, strFileName;
		if(bMutiCCDCheck)
			strPath.Format("%s\\Log\\Operator\\%04d\\%02d\\配方校验修改记录\\%02d\\%s_%s_%02d%02d%02d", GetCurrentAppPath(), tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay, tPdInfo.strPDName, user.strJobNumber, tSaveTime.wHour, tSaveTime.wMinute, tSaveTime.wSecond);
		else
			strPath.Format("%s\\Log\\Operator\\%04d\\%02d\\配方修改记录\\%02d\\%s_%s_%02d%02d%02d", GetCurrentAppPath(), tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay, tPdInfo.strPDName, user.strJobNumber, tSaveTime.wHour, tSaveTime.wMinute, tSaveTime.wSecond);
		strFileName.Format("%s\\%s.csv", strPath, sName);
		CStdioFile file;

		if (!PathFileExists(strPath))CreateMultilevelPath(strPath);
		if (TRUE != file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strLog;
			strLog.Format("CProductTool_SaveProductParamModifiedRecord：创建文件异常, %s", strFileName);
			theGlobal.m_Log.Log(strLog, ExcpLog);
		}

		CString strItem, strTemp;
		int nCount = 0;
		if (tPdInfo.bExistAT11 == TRUE)
			nCount = max(tPdInfo.tParam.vecAT11Width.size(), tPdInfo.tParam.vecMisalignment.size());
		else if (tPdInfo.bExistBaseCoat == TRUE)
			nCount = max(tPdInfo.tParam.vecBaseCoatWidth.size(), tPdInfo.tParam.vecMisalignment.size());
		else
			nCount = max(tPdInfo.tParam.vecFoilWidth.size(), tPdInfo.tParam.vecMisalignment.size());
		nCount = max(nCount, tPdInfo.tParam.vecChecking.size());


		for (int i = 0; i < nCount; i++)
		{
			strItem.Format(",规格%d", i + 1);
			strTemp += strItem;
		}
		strItem.Format("产品信息,设置%s\n", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		//产品名称
		strItem.Format("%s产品名称,%s \n", m_tPdInfoCorrectedFlag.bPDNameFlag ? "*** " : "", tPdInfo.strPDName);
		file.WriteString(strItem);
		//产品备注
		strItem.Format("%s产品备注,%s \n", m_tPdInfoCorrectedFlag.bNoteFlag ? "*** " : "", tPdInfo.strNote);
		file.WriteString(strItem);
		//产品类型
		if (tPdInfo.nPDType == ThreeCavity)
			strTemp = "一出三";
		else if (tPdInfo.nPDType == TwoCavity)
			strTemp = "一出二";
		else if (tPdInfo.nPDType == FourCavity)
			strTemp = "一出四";
		else if (tPdInfo.nPDType == SixCavity)
			strTemp = "一出六";
		else if (tPdInfo.nPDType == EightCavity)
			strTemp = "一出八";
		else
			strTemp = "一出十";

		strItem.Format("%s产品类型,%s \n", m_tPdInfoCorrectedFlag.bPDTypeFlag ? "*** " : "", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		//有无AT11/凹版
		if (tPdInfo.bExistAT11)
			strTemp = "有AT11";
		else if (tPdInfo.bExistBaseCoat)
			strTemp = "有凹版";
		else
			strTemp = "无";

		strItem.Format("%sAT11/凹版,%s \n", (m_tPdInfoCorrectedFlag.bExistAT11Flag || m_tPdInfoCorrectedFlag.bExistBaseCoatFlag) ? "*** " : "", strTemp);
		file.WriteString(strItem);

		//有无间歇
		if (tPdInfo.bExistIntermittent)
			strTemp = "有间歇";
		else
			strTemp = "无";

		strItem.Format("%s有无间歇,%s \n", m_tPdInfoCorrectedFlag.bExistIntermittentFlag ? "*** " : "", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		//涂布类型
		if (tPdInfo.nCoatingType == OneSide || theGlobal.m_Project.GetProjectConfig().nSideCount == 1)
			strTemp = "单面";
		else
			strTemp = "双面";

		strItem.Format("%s涂布类型,%s \n", m_tPdInfoCorrectedFlag.bCoatingTypeFlag ? "*** " : "", strTemp);
		file.WriteString(strItem);

		//涂布宽
		file.WriteString("\n");
		if (tPdInfo.tParam.bUseCoatWidth)
			strTemp = "启用";
		else
			strTemp = "不启用";

		strItem.Format("%s涂膜标准值(mm),%s,%.2f\n", m_tPdInfoCorrectedFlag.bUseCoatWidth ? "*** " : "", strTemp, tPdInfo.tParam.vecCoatWidth[0].dStandard);
		file.WriteString(strItem);
		strItem.Format("涂膜公差(mm),,%.2f\n", tPdInfo.tParam.vecCoatWidth[0].dTolerance);
		file.WriteString(strItem);
		strItem.Format("涂膜预警公差(mm),,%.2f\n", tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance);
		file.WriteString(strItem);
		strTemp = "";

		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			strItem.Format(",%.2f", tPdInfo.tParam.vecCoatWidth[i].OffsetList[0]);
			strTemp += strItem;
		}

		strItem.Format("涂膜正面补偿(mm),%s\n", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		if (tPdInfo.nCoatingType == TwoSide)
		{
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				strItem.Format(",%.2f", tPdInfo.tParam.vecCoatWidth[i].OffsetList[1]);
				strTemp += strItem;
			}
			strItem.Format("涂膜反面补偿(mm),%s\n", strTemp);
			file.WriteString(strItem);
			strTemp = "";
		}

		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			strItem.Format(",%.2f", tPdInfo.tParam.vecCoatWidth[i].dOffsetOL);
			strTemp += strItem;
		}

		strItem.Format("涂膜正面在线补偿(mm),%s\n", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		//极耳宽
		file.WriteString("\n");
		if (tPdInfo.tParam.bUseFoilWidth)
			strItem = "启用";
		else
			strItem = "不启用";

		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strItem.Format(",%.2f", tPdInfo.tParam.vecFoilWidth[i].dStandard);
			strTemp += strItem;
		}

		strItem.Format("%s极耳标准值(mm),%s\n", m_tPdInfoCorrectedFlag.bUseFoilWidth ? "*** " : "", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strItem.Format(",%.2f", tPdInfo.tParam.vecFoilWidth[i].dTolerance);
			strTemp += strItem;
		}
		strItem.Format("极耳公差(mm),%s\n", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strItem.Format(",%.2f", tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance);
			strTemp += strItem;
		}
		strItem.Format("极耳预警公差(mm),%s\n", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strItem.Format(",%.2f", tPdInfo.tParam.vecFoilWidth[i].OffsetList[0]);
			strTemp += strItem;
		}
		strItem.Format("极耳正面补偿(mm),%s\n", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		if (tPdInfo.nCoatingType == TwoSide)
		{
			for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
			{
				strItem.Format(",%.2f", tPdInfo.tParam.vecFoilWidth[i].OffsetList[1]);
				strTemp += strItem;
			}
			strItem.Format("极耳反面补偿(mm),%s\n", strTemp);
			file.WriteString(strItem);
			strTemp = "";
		}

		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strItem.Format(",%.2f", tPdInfo.tParam.vecFoilWidth[i].dOffsetOL);
			strTemp += strItem;
		}
		strItem.Format("极耳正面在线补偿(mm),%s\n", strTemp);
		file.WriteString(strItem);
		strTemp = "";

		//AT11/凹版宽		
		if (tPdInfo.bExistAT11)
		{
			file.WriteString("\n");

			if (tPdInfo.tParam.bUseAT11Width)
				strTemp = "启用";
			else
				strTemp = "不启用";

			strItem.Format("%sAT11标准值(mm),%s,%0.2f \n", m_tPdInfoCorrectedFlag.bUseAT11Width ? "*** " : "", strTemp, tPdInfo.tParam.vecAT11Width[0].dStandard);
			file.WriteString(strItem);
			strTemp = "";
			strItem.Format("AT11公差(mm),,%0.2f \n", tPdInfo.tParam.vecAT11Width[0].dTolerance);
			file.WriteString(strItem);
			strTemp = "";
			strItem.Format("AT11预警公差(mm),,%0.2f \n", tPdInfo.tParam.vecAT11Width[0].dWarnTolerance);
			file.WriteString(strItem);
			strTemp = "";

			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecAT11Width[i].OffsetList[0]);
				strTemp += strItem;
			}
			strItem.Format("AT11正面补偿(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";

			if (tPdInfo.nCoatingType == TwoSide)
			{
				for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
				{
					strItem.Format(",%0.2f", tPdInfo.tParam.vecAT11Width[i].OffsetList[1]);
					strTemp += strItem;
				}
				strItem.Format("AT11反面补偿(mm),%s \n", strTemp);
				file.WriteString(strItem);
				strTemp = "";
			}

			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecAT11Width[i].dOffsetOL);
				strTemp += strItem;
			}

			strItem.Format("AT11正面在线补偿(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";
		}
		else if (tPdInfo.bExistBaseCoat)
		{
			file.WriteString("\n");
			if (tPdInfo.tParam.bUseBaseCoatWidth)
				strTemp = "启用";
			else
				strTemp = "不启用";

			strItem.Format("%s凹版标准值(mm),%s,%0.2f \n", m_tPdInfoCorrectedFlag.bUseBaseCoatWidth ? "*** " : "", strTemp, tPdInfo.tParam.vecBaseCoatWidth[0].dStandard);
			file.WriteString(strItem);
			strTemp = "";
			strItem.Format("凹版公差(mm),,%0.2f \n", tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance);
			file.WriteString(strItem);
			strTemp = "";
			strItem.Format("凹版预警公差(mm),,%0.2f \n", tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance);
			file.WriteString(strItem);
			strTemp = "";

			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[0]);
				strTemp += strItem;
			}
			strItem.Format("凹版正面补偿(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";

			if (tPdInfo.nCoatingType == TwoSide)
			{
				for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
				{
					strItem.Format(",%0.2f", tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[1]);
					strTemp += strItem;
				}
				strItem.Format("凹版反面补偿(mm),%s \n", strTemp);
				file.WriteString(strItem);
				strTemp = "";
			}

			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecBaseCoatWidth[i].dOffsetOL);
				strTemp += strItem;
			}

			strItem.Format("凹版正面在线补偿(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";
		}

		//错位值
		file.WriteString("\n");
		if (tPdInfo.nCoatingType == TwoSide && theGlobal.m_Project.GetProjectConfig().nSideCount > 1)
		{
			if (tPdInfo.tParam.bUseMisalignment)
				strTemp = "启用";
			else
				strTemp = "不启用";

			strItem.Format("%s错位标准值(mm),%s,%0.2f \n", m_tPdInfoCorrectedFlag.bUseMisalignment ? "*** " : "", strTemp, tPdInfo.tParam.vecMisalignment[0].dStandard);
			file.WriteString(strItem);
			strTemp = "";
			strItem.Format("错位公差(mm),,%0.2f \n", tPdInfo.tParam.vecMisalignment[0].dTolerance);
			file.WriteString(strItem);
			strTemp = "";
			strItem.Format("错位预警公差(mm),,%0.2f \n", tPdInfo.tParam.vecMisalignment[0].dWarnTolerance);
			file.WriteString(strItem);
			strTemp = "";

			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecMisalignment[i].OffsetList[0]);
				strTemp += strItem;
			}
			strItem.Format("错位正面补偿(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";

			if (tPdInfo.nCoatingType == TwoSide)
			{
				for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
				{
					strItem.Format(",%0.2f", tPdInfo.tParam.vecMisalignment[i].OffsetList[1]);
					strTemp += strItem;
				}
				strItem.Format("错位反面补偿(mm),%s \n", strTemp);
				file.WriteString(strItem);
				strTemp = "";
			}

			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecMisalignment[i].dOffsetOL);
				strTemp += strItem;
			}

			strItem.Format("错位正面在线补偿(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";
		}
		//点检
		if (!bMutiCCDCheck)
		{
			file.WriteString("\n");
			if (tPdInfo.tParam.bUseChecking)
				strTemp = "启用";
			else
				strTemp = "不启用";

			for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecChecking[i].dStandard);
				strTemp += strItem;
			}
			strItem.Format("%s点检正面宽/高/间距标准值(mm),%s, \n", m_tPdInfoCorrectedFlag.bUseChecking ? "*** " : "", strTemp);
			file.WriteString(strItem);
			strTemp = "";

			if (tPdInfo.nCoatingType == TwoSide)
			{
				for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
				{
					strItem.Format(",%0.2f", tPdInfo.tParam.vecChecking[i].dStandard);
					strTemp += strItem;
				}
				strItem.Format("%s点检反面宽/高/间距标准值(mm),%s, \n", m_tPdInfoCorrectedFlag.bUseChecking ? "*** " : "", strTemp);
				file.WriteString(strItem);
				strTemp = "";
			}

			for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecChecking[i].dTolerance);
				strTemp += strItem;
			}
			strItem.Format("点检宽/高公差(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";
	
			for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
			{
				strItem.Format(",%0.2f", tPdInfo.tParam.vecChecking[i].OffsetList[0]);
				strTemp += strItem;
			}
			strItem.Format("点检正面补偿(mm),%s \n", strTemp);
			file.WriteString(strItem);
			strTemp = "";

			if (tPdInfo.nCoatingType == TwoSide)
			{
				for (int i = 0; i < tPdInfo.tParam.vecChecking.size(); i++)
				{
					strItem.Format(",%0.2f", tPdInfo.tParam.vecChecking[i].OffsetList[1]);
					strTemp += strItem;
				}
				strItem.Format("点检反面补偿(mm),%s \n", strTemp);
				file.WriteString(strItem);
				strTemp = "";
			}
		}
		
		//表面瑕疵
		file.WriteString("\n");
		if (tPdInfo.tParam.bUseSurfaceDetect)
			strTemp = "启用";
		else
			strTemp = "不启用";

		strItem.Format("表面瑕疵,%s, \n", strTemp);
		file.WriteString(strItem);
		strTemp = "";
		//////////////////////////////////////////////////////////
		file.Close();

		return true;
	}
	catch (HException& e)
	{
		CString strLog;
		strLog.Format("CProductTool_SaveProductParamModifiedRecord异常");
		theGlobal.m_Log.Log(strLog, ExcpLog);
		return false;
	}
	catch (const std::exception&)
	{
		CString strLog;
		strLog.Format("CProductTool_SaveProductParamModifiedRecord异常");
		theGlobal.m_Log.Log(strLog, ExcpLog);
		return FALSE;
	}
	catch (...)
	{
		CString strLog;
		strLog.Format("CProductTool_SaveProductParamModifiedRecord异常");
		theGlobal.m_Log.Log(strLog, ExcpLog);
		return FALSE;
	}
}