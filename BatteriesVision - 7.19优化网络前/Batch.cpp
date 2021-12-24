#include "StdAfx.h"
#include "Counter.h"
CCounter::CCounter()
{
}

CCounter::~CCounter()
{

}

BOOL CCounter::Init()
{
	m_strCounterPath.Format("%s\\Config\\Public\\Count\\", GetCurrentAppPath());
	CreateMultilevelPath(m_strCounterPath);


	ReadParam();
	return TRUE;
}

BOOL CCounter::ReadParam()
{
	CString strName, strKey, strFile;
	m_nCounterCount = GetIniInt("Project", "MergeCount", 1);
	m_nNGTypeCount = GetIniInt("Error", "CountType", 1, CBaseTmpl::CommonFile);
	m_vecCount.clear();
	for (int i=0;i<m_nCounterCount;i++)
	{
		strFile.Format("%s//Count%d.ini", GetCurrentAppPath(), i);
		tagCount tCount;
		tCount.dNGMeter = GetPrivateProfileDouble("Count", "dNGMeter", 0, strFile);
		tCount.dTotalMeter = GetPrivateProfileDouble("Count", "dTotalMeter", 0, strFile);
		tCount.nNGCount = GetPrivateProfileInt("Count", "nNGCount", 0, strFile);
		tCount.vecNG.clear();
		for (int j = 0; j < m_nNGTypeCount; j++)
		{
			tagSingleNGCount tNG;
			tNG.nType = j;
			strName.Format("NG%dMeter", j);
			tNG.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
			strName.Format("NG%dCount", j);
			tNG.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
			tCount.vecNG.push_back(tNG);
		}
		m_vecCount.push_back(tCount);
	}

	return TRUE;
}

BOOL CCounter::WriteParam()
{
	CString strName, strKey, strFile;
	for (int i = 0; i < m_vecCount.size(); i++)
	{
		strFile.Format("%s//Count%d.ini", GetCurrentAppPath(), i);

		tagCount tCount = m_vecCount[i];
		WritePrivateProfileDouble("Count", "dNGMeter", tCount.dNGMeter, strFile);
		WritePrivateProfileDouble("Count", "dTotalMeter", tCount.dTotalMeter, strFile);
		WritePrivateProfileInt("Count", "nNGCount", tCount.nNGCount, strFile);
		for (int j = 0; j < tCount.vecNG.size(); j++)
		{
			tagSingleNGCount tNG = tCount.vecNG[j];
			strName.Format("NG%dMeter", j);
			WritePrivateProfileDouble("Count", strName, tNG.dMeter, strFile);
			strName.Format("NG%dCount", j);
			WritePrivateProfileInt("Count", strName, tNG.nCount, strFile);
		}
	}
	
	return TRUE;
}

void CCounter::ClearCurrent()
{
	/*for (int i = 0; i < 2; i++)
	{
		nCurrentOK[i] = 0;
		nCurrentNG[i] = 0;
		dCurrentYield[i] = 0;
		dCurrentMeters = 0;
		for (int j = 0; j < COUNT_TYPE; j++)
		{
			nCount[i][j] = 0;
		}
	}*/
}

BOOL CCounter::Count(const C_MergeWork& tOutput)
{
	//for (int i = 0; i < PRODUCT_COUNT; i++)
	//{
	//	//去除非本次循环
	//	if (TRUE == tOutput.tEA[i].hvComplete)
	//	{
	//		nCount[i][tOutput.tEA[i].hvErrorType.I()]++;
	//	}
	//}
	return TRUE;
}
