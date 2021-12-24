#include "StdAfx.h"
#include "Counter.h"
CCounter::CCounter()
{
	bStartNewBetchFlag = FALSE;
	m_bStopPos = FALSE;
}

CCounter::~CCounter()
{

}

BOOL CCounter::Init()
{
	m_strCounterPath.Format("%s\\Config\\Public\\Count\\", GetCurrentAppPath());
	CreateMultilevelPath(m_strCounterPath);


	ReadParam();
	//设置当前卷号
	CString strBatch = m_tCurrentBatch.strName;
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Marking, "SetCurrentBatch", &strBatch, NULL);

	return TRUE;
}

BOOL CCounter::ReadParam()
{
	CString strKey, strFile;
	m_nCounterCount = GetIniInt("Project", "MergeCount", 1);
	m_nNGTypeCount = GetIniInt("Error", "CountType", 1, CBaseTmpl::CommonFile);
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	CLockHelper lk(&m_csLock);
	//for (int i = 0; i < m_nCounterCount; i++)
	{
		strFile.Format("%s//Count%d.ini", m_strCounterPath, 0);

		COleVariant vtime;
		//获取存储时间
		CString strTime = GetPrivateProfileCString("Batch", "StartTime", "2021-1-27 9:31:37", strFile);
		try
		{
			vtime = strTime;
			vtime.ChangeType(VT_DATE);
			m_tCurrentBatch.timeStart = vtime;
		}
		catch (...)
		{
			m_tCurrentBatch.timeStart = ATL::COleDateTime::GetCurrentTime();
		}

		//获取卷号
		m_tCurrentBatch.strName = GetPrivateProfileCString("Batch", "Name", "N3220081553282", strFile);

		//获取存储时间
		strTime = GetPrivateProfileCString("Batch", "LastStartTime", "2021-1-27 5:31:37", strFile);
		try
		{
			vtime = strTime;
			vtime.ChangeType(VT_DATE);
			m_tLastBatch.timeStart = vtime;
		}
		catch (...)
		{
			m_tLastBatch.timeStart = ATL::COleDateTime(2021, 3, 20, 0, 0, 0);
		}

		strTime = GetPrivateProfileCString("Batch", "LastEndTime", "2021-1-27 9:31:37", strFile);
		try
		{
			vtime = strTime;
			vtime.ChangeType(VT_DATE);
			m_tLastBatch.timeEnd = vtime;
		}
		catch (...)
		{
			m_tLastBatch.timeEnd = ATL::COleDateTime(2021, 3, 20, 0, 0, 1);
		}

		//获取卷号
		m_tLastBatch.strName = GetPrivateProfileCString("Batch", "LastName", "N3220081553282", strFile);

		m_tCount.dTotalNGMeter = GetPrivateProfileDouble("Count", "dTotalNGMeter", 0, strFile);
		m_tCount.dTotalMeter = GetPrivateProfileDouble("Count", "dTotalMeter", 0, strFile);
		m_tCount.dTotalMeterSelect = GetPrivateProfileDouble("Count", "dTotalMeterSelect", 0, strFile);
		m_tCount.nTotalNGCount = GetPrivateProfileInt("Count", "nTotalNGCount", 0, strFile);
		m_tCount.tSizeNGCounts.dNGMeter = GetPrivateProfileDouble("Count", "tSizeNGCounts.dNGMeter", 0, strFile);
		m_tCount.tSizeNGCounts.nNGCount = GetPrivateProfileInt("Count", "tSizeNGCounts.nNGCount", 0, strFile);
		m_tCount.tSizeNGCounts.vecCoat[0].clear();
		m_tCount.tSizeNGCounts.vecFoil[0].clear();
		m_tCount.tSizeNGCounts.vecAT11BaseCoat[0].clear();
		m_tCount.tSizeNGCounts.vecCoat[1].clear();
		m_tCount.tSizeNGCounts.vecFoil[1].clear();
		m_tCount.tSizeNGCounts.vecAT11BaseCoat[1].clear();
		m_tCount.tSizeNGCounts.vecMisalignment.clear();
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			tagSingleNGCount tSingleNGCount;
			CString strName;
			strName.Format("tSizeNGCounts.Coat%dA.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("Count", strName, 0, strFile);
			m_tCount.tSizeNGCounts.vecCoat[0].push_back(tSingleNGCount);

			if (tPdInfo.nCoatingType != TwoSide) continue;

			strName.Format("tSizeNGCounts.Coat%dB.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("Count", strName, 0, strFile);
			m_tCount.tSizeNGCounts.vecCoat[1].push_back(tSingleNGCount);
		}
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			tagSingleNGCount tSingleNGCount;
			CString strName;
			strName.Format("tSizeNGCounts.Foil%dA.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("Count", strName, 0, strFile);
			m_tCount.tSizeNGCounts.vecFoil[0].push_back(tSingleNGCount);

			if (tPdInfo.nCoatingType != TwoSide) continue;

			strName.Format("tSizeNGCounts.Foil%dB.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("Count", strName, 0, strFile);
			m_tCount.tSizeNGCounts.vecFoil[1].push_back(tSingleNGCount);
		}
		if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
		{
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size() * 2; i++)
			{
				tagSingleNGCount tSingleNGCount;
				CString strName;
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.nCount", i);
				tSingleNGCount.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dMeter", i);
				tSingleNGCount.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dRate", i);
				tSingleNGCount.dRate = GetPrivateProfileDouble("Count", strName, 0, strFile);
				m_tCount.tSizeNGCounts.vecAT11BaseCoat[0].push_back(tSingleNGCount);

				if (tPdInfo.nCoatingType != TwoSide) continue;

				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.nCount", i);
				tSingleNGCount.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dMeter", i);
				tSingleNGCount.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dRate", i);
				tSingleNGCount.dRate = GetPrivateProfileDouble("Count", strName, 0, strFile);
				m_tCount.tSizeNGCounts.vecAT11BaseCoat[1].push_back(tSingleNGCount);
			}
		}
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size() * 2; i++)
		{
			tagSingleNGCount tSingleNGCount;
			CString strName;
			strName.Format("tSizeNGCounts.Misalignment%d.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Misalignment%d.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("Count", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Misalignment%d.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("Count", strName, 0, strFile);
			m_tCount.tSizeNGCounts.vecMisalignment.push_back(tSingleNGCount);
		}

		m_tCount.tDefectNGCounts.dNGMeter = GetPrivateProfileDouble("Count", "tDefectNGCounts.dNGMeter", 0, strFile);
		m_tCount.tDefectNGCounts.nNGCount = GetPrivateProfileInt("Count", "tDefectNGCounts.nNGCount", 0, strFile);
		m_tCount.tDefectNGCounts.vecCoat[0].clear();
		m_tCount.tDefectNGCounts.vecFoil[0].clear();
		m_tCount.tDefectNGCounts.vecAT11BaseCoat[0].clear();

		m_tCount.vecStopInfo.clear();
		int nListCount = GetPrivateProfileInt("Count", "nStopInfoCount", 0, strFile);
		for (int i = 0; i < nListCount; i++)
		{
			tagStopInfo tInfo;
			CString strName;
			strName.Format("StopInfo%d.strDiscribe", i);
			tInfo.strDiscribe = GetPrivateProfileCString("Count", strName, "其他", strFile);
			strName.Format("StopInfo%d.strTimeStart", i);
			tInfo.strTimeStart = GetPrivateProfileCString("Count", strName, "----", strFile);
			strName.Format("StopInfo%d.strTimeEnd", i);
			tInfo.strTimeEnd = GetPrivateProfileCString("Count", strName, "----", strFile);
			strName.Format("StopInfo%d.dMeterPosStart", i);
			tInfo.dMeterPosStart = GetPrivateProfileDouble("Count", strName, 0, strFile);
			strName.Format("StopInfo%d.dMeterPosEnd", i);
			tInfo.dMeterPosEnd = GetPrivateProfileDouble("Count", strName, 0, strFile);
			m_tCount.vecStopInfo.push_back(tInfo);
		}

		m_tLastCount.dTotalNGMeter = GetPrivateProfileDouble("LastCount", "dTotalNGMeter", 0, strFile);
		m_tLastCount.dTotalMeter = GetPrivateProfileDouble("LastCount", "dTotalMeter", 0, strFile);
		m_tLastCount.dTotalMeterSelect = GetPrivateProfileDouble("LastCount", "dTotalMeterSelect", 0, strFile);
		m_tLastCount.nTotalNGCount = GetPrivateProfileInt("LastCount", "nTotalNGCount", 0, strFile);
		m_tLastCount.tSizeNGCounts.dNGMeter = GetPrivateProfileDouble("LastCount", "tSizeNGCounts.dNGMeter", 0, strFile);
		m_tLastCount.tSizeNGCounts.nNGCount = GetPrivateProfileInt("LastCount", "tSizeNGCounts.nNGCount", 0, strFile);
		m_tLastCount.tSizeNGCounts.vecCoat[0].clear();
		m_tLastCount.tSizeNGCounts.vecFoil[0].clear();
		m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].clear();
		m_tLastCount.tSizeNGCounts.vecCoat[1].clear();
		m_tLastCount.tSizeNGCounts.vecFoil[1].clear();
		m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].clear();
		m_tLastCount.tSizeNGCounts.vecMisalignment.clear();
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			tagSingleNGCount tSingleNGCount;
			CString strName;
			strName.Format("tSizeNGCounts.Coat%dA.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			m_tLastCount.tSizeNGCounts.vecCoat[0].push_back(tSingleNGCount);

			if (tPdInfo.nCoatingType != TwoSide) continue;

			strName.Format("tSizeNGCounts.Coat%dB.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			m_tLastCount.tSizeNGCounts.vecCoat[1].push_back(tSingleNGCount);
		}
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			tagSingleNGCount tSingleNGCount;
			CString strName;
			strName.Format("tSizeNGCounts.Foil%dA.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			m_tLastCount.tSizeNGCounts.vecFoil[0].push_back(tSingleNGCount);

			if (tPdInfo.nCoatingType != TwoSide) continue;

			strName.Format("tSizeNGCounts.Foil%dB.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			m_tLastCount.tSizeNGCounts.vecFoil[1].push_back(tSingleNGCount);
		}
		if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
		{
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size() * 2; i++)
			{
				tagSingleNGCount tSingleNGCount;
				CString strName;
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.nCount", i);
				tSingleNGCount.nCount = GetPrivateProfileInt("LastCount", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dMeter", i);
				tSingleNGCount.dMeter = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dRate", i);
				tSingleNGCount.dRate = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
				m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].push_back(tSingleNGCount);

				if (tPdInfo.nCoatingType != TwoSide) continue;

				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.nCount", i);
				tSingleNGCount.nCount = GetPrivateProfileInt("LastCount", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dMeter", i);
				tSingleNGCount.dMeter = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dRate", i);
				tSingleNGCount.dRate = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
				m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].push_back(tSingleNGCount);
			}
		}
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size() * 2; i++)
		{
			tagSingleNGCount tSingleNGCount;
			CString strName;
			strName.Format("tSizeNGCounts.Misalignment%d.nCount", i);
			tSingleNGCount.nCount = GetPrivateProfileInt("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Misalignment%d.dMeter", i);
			tSingleNGCount.dMeter = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			strName.Format("tSizeNGCounts.Misalignment%d.dRate", i);
			tSingleNGCount.dRate = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			m_tLastCount.tSizeNGCounts.vecMisalignment.push_back(tSingleNGCount);
		}
		m_tLastCount.tDefectNGCounts.dNGMeter = GetPrivateProfileDouble("LastCount", "tDefectNGCounts.dNGMeter", 0, strFile);
		m_tLastCount.tDefectNGCounts.nNGCount = GetPrivateProfileInt("LastCount", "tDefectNGCounts.nNGCount", 0, strFile);
		m_tLastCount.tDefectNGCounts.vecCoat[0].clear();
		m_tLastCount.tDefectNGCounts.vecFoil[0].clear();
		m_tLastCount.tDefectNGCounts.vecAT11BaseCoat[0].clear();
		m_tLastCount.tDefectNGCounts.vecCoat[1].clear();
		m_tLastCount.tDefectNGCounts.vecFoil[1].clear();
		m_tLastCount.tDefectNGCounts.vecAT11BaseCoat[1].clear();

		nListCount = GetPrivateProfileInt("LastCount", "nStopInfoCount", 0, strFile);
		m_tLastCount.vecStopInfo.clear();
		for (int i = 0; i < nListCount; i++)
		{
			tagStopInfo tInfo;
			CString strName;
			strName.Format("StopInfo%d.strDiscribe", i);
			tInfo.strDiscribe = GetPrivateProfileCString("LastCount", strName, "其他", strFile);
			strName.Format("StopInfo%d.strTimeStart", i);
			tInfo.strTimeStart = GetPrivateProfileCString("LastCount", strName, "----", strFile);
			strName.Format("StopInfo%d.strTimeEnd", i);
			tInfo.strTimeEnd = GetPrivateProfileCString("LastCount", strName, "----", strFile);
			strName.Format("StopInfo%d.dMeterPosStart", i);
			tInfo.dMeterPosStart = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			strName.Format("StopInfo%d.dMeterPosEnd", i);
			tInfo.dMeterPosEnd = GetPrivateProfileDouble("LastCount", strName, 0, strFile);
			m_tLastCount.vecStopInfo.push_back(tInfo);
		}

		for (int j = 0; j < m_nNGTypeCount; j++)
		{
			strKey.Format("Error%dName", j);
			CString strName = GetIniString("ErrorInfo", strKey, "null", CBaseTmpl::CommonFile);
			m_vecTypeName.push_back(strName);
		}
	}

	return TRUE;
}

BOOL CCounter::WriteParam()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strName, strKey, strFile;
	//for (int i = 0; i < m_vecCount.size(); i++)
	{
		strFile.Format("%s//Count%d.ini", m_strCounterPath, 0);

		DeleteFileA(strFile);

		//写入存储时间
		WritePrivateProfileString("Batch", "StartTime", m_tCurrentBatch.timeStart.Format("%Y-%m-%d %H:%M:%S"), strFile);
		//写入卷号
		WritePrivateProfileString("Batch", "Name", m_tCurrentBatch.strName, strFile);

		//写入存储时间
		WritePrivateProfileString("Batch", "LastStartTime", m_tLastBatch.timeStart.Format("%Y-%m-%d %H:%M:%S"), strFile);
		//写入存储时间
		WritePrivateProfileString("Batch", "LastEndTime", m_tLastBatch.timeEnd.Format("%Y-%m-%d %H:%M:%S"), strFile);
		//写入卷号
		WritePrivateProfileString("Batch", "LastName", m_tLastBatch.strName, strFile);

		WritePrivateProfileDouble("Count", "dTotalNGMeter", m_tCount.dTotalNGMeter, strFile);
		WritePrivateProfileDouble("Count", "dTotalMeter", m_tCount.dTotalMeter, strFile);
		WritePrivateProfileDouble("Count", "dTotalMeterSelect", m_tCount.dTotalMeterSelect, strFile);
		WritePrivateProfileInt("Count", "nTotalNGCount", m_tCount.nTotalNGCount, strFile);
		WritePrivateProfileDouble("Count", "tSizeNGCounts.dNGMeter", m_tCount.tSizeNGCounts.dNGMeter, strFile);
		WritePrivateProfileInt("Count", "tSizeNGCounts.nNGCount", m_tCount.tSizeNGCounts.nNGCount, strFile);
		for (int i = 0; i < m_tCount.tSizeNGCounts.vecCoat[0].size(); i++)
		{
			tagSingleNGCount tSingleNGCount = m_tCount.tSizeNGCounts.vecCoat[0][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100;
			CString strName;
			strName.Format("tSizeNGCounts.Coat%dA.nCount", i);
			WritePrivateProfileInt("Count", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dMeter", i);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dRate", i);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);//不用下面这种方式，是因为尺寸OK时，停机写入配置文件数据时，dRate不会更新
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);

			if (tPdInfo.nCoatingType != TwoSide || i >= m_tCount.tSizeNGCounts.vecCoat[1].size()) continue;

			tSingleNGCount = m_tCount.tSizeNGCounts.vecCoat[1][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100;
			strName.Format("tSizeNGCounts.Coat%dB.nCount", i);
			WritePrivateProfileInt("Count", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dMeter", i);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dRate", i);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);
		}
		for (int i = 0; i < m_tCount.tSizeNGCounts.vecFoil[0].size(); i++)
		{
			tagSingleNGCount tSingleNGCount = m_tCount.tSizeNGCounts.vecFoil[0][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100;
			CString strName;
			strName.Format("tSizeNGCounts.Foil%dA.nCount", i);
			WritePrivateProfileInt("Count", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dMeter", i);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dRate", i);
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100, strFile);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);

			if (tPdInfo.nCoatingType != TwoSide || i >= m_tCount.tSizeNGCounts.vecFoil[1].size()) continue;

			tSingleNGCount = m_tCount.tSizeNGCounts.vecFoil[1][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100;
			strName.Format("tSizeNGCounts.Foil%dB.nCount", i);
			WritePrivateProfileInt("Count", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dMeter", i);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dRate", i);
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100, strFile);
			WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);
		}
		if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
		{
			for (int i = 0; i < m_tCount.tSizeNGCounts.vecAT11BaseCoat[0].size(); i++)
			{
				tagSingleNGCount tSingleNGCount = m_tCount.tSizeNGCounts.vecAT11BaseCoat[0][i];
				tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100;
				CString strName;
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.nCount", i);
				WritePrivateProfileInt("Count", strName, tSingleNGCount.nCount, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dMeter", i);
				WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dRate", i);
				WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);

				if (tPdInfo.nCoatingType != TwoSide || i >= m_tCount.tSizeNGCounts.vecAT11BaseCoat[1].size()) continue;

				tSingleNGCount = m_tCount.tSizeNGCounts.vecAT11BaseCoat[1][i];
				tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100;
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.nCount", i);
				WritePrivateProfileInt("Count", strName, tSingleNGCount.nCount, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dMeter", i);
				WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dRate", i);
				WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);
			}
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			for (int i = 0; i < m_tCount.tSizeNGCounts.vecMisalignment.size(); i++)
			{
				tagSingleNGCount tSingleNGCount = m_tCount.tSizeNGCounts.vecMisalignment[i];
				tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tCount.dTotalMeterSelect * 100;
				CString strName;
				strName.Format("tSizeNGCounts.Misalignment%d.nCount", i);
				WritePrivateProfileInt("Count", strName, tSingleNGCount.nCount, strFile);
				strName.Format("tSizeNGCounts.Misalignment%d.dMeter", i);
				WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter, strFile);
				strName.Format("tSizeNGCounts.Misalignment%d.dRate", i);
				WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);
				//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dRate, strFile);
			}
		}
		WritePrivateProfileInt("Count", "nStopInfoCount", m_tCount.vecStopInfo.size(), strFile);
		for (int i = 0; i < m_tCount.vecStopInfo.size(); i++)
		{
			CString strName;
			strName.Format("StopInfo%d.strDiscribe", i);
			WritePrivateProfileString("Count", strName, m_tCount.vecStopInfo[i].strDiscribe, strFile);
			strName.Format("StopInfo%d.strTimeStart", i);
			WritePrivateProfileString("Count", strName, m_tCount.vecStopInfo[i].strTimeStart, strFile);
			strName.Format("StopInfo%d.strTimeEnd", i);
			WritePrivateProfileString("Count", strName, m_tCount.vecStopInfo[i].strTimeEnd, strFile);
			strName.Format("StopInfo%d.dMeterPosStart", i);
			WritePrivateProfileDouble("Count", strName, m_tCount.vecStopInfo[i].dMeterPosStart, strFile);
			strName.Format("StopInfo%d.dMeterPosEnd", i);
			WritePrivateProfileDouble("Count", strName, m_tCount.vecStopInfo[i].dMeterPosEnd, strFile);
		}

		WritePrivateProfileDouble("LastCount", "dTotalNGMeter", m_tLastCount.dTotalNGMeter, strFile);
		WritePrivateProfileDouble("LastCount", "dTotalMeter", m_tLastCount.dTotalMeter, strFile);
		WritePrivateProfileDouble("LastCount", "dTotalMeterSelect", m_tLastCount.dTotalMeterSelect, strFile);
		WritePrivateProfileInt("LastCount", "nTotalNGCount", m_tLastCount.nTotalNGCount, strFile);
		WritePrivateProfileDouble("LastCount", "tSizeNGCounts.dNGMeter", m_tLastCount.tSizeNGCounts.dNGMeter, strFile);
		WritePrivateProfileInt("LastCount", "tSizeNGCounts.nNGCount", m_tLastCount.tSizeNGCounts.nNGCount, strFile);
		for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecCoat[0].size(); i++)
		{
			tagSingleNGCount tSingleNGCount = m_tLastCount.tSizeNGCounts.vecCoat[0][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100;
			CString strName;
			strName.Format("tSizeNGCounts.Coat%dA.nCount", i);
			WritePrivateProfileInt("LastCount", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dMeter", i);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Coat%dA.dRate", i);
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100, strFile);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dRate, strFile);

			if (tPdInfo.nCoatingType != TwoSide || i >= m_tCount.tSizeNGCounts.vecCoat[1].size()) continue;

			tSingleNGCount = m_tLastCount.tSizeNGCounts.vecCoat[1][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100;
			strName.Format("tSizeNGCounts.Coat%dB.nCount", i);
			WritePrivateProfileInt("LastCount", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dMeter", i);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Coat%dB.dRate", i);
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100, strFile);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dRate, strFile);
		}
		for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecFoil[0].size(); i++)
		{
			tagSingleNGCount tSingleNGCount = m_tLastCount.tSizeNGCounts.vecFoil[0][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100;
			CString strName;
			strName.Format("tSizeNGCounts.Foil%dA.nCount", i);
			WritePrivateProfileInt("LastCount", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dMeter", i);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Foil%dA.dRate", i);
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100, strFile);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dRate, strFile);

			if (tPdInfo.nCoatingType != TwoSide || i >= m_tCount.tSizeNGCounts.vecFoil[1].size()) continue;

			tSingleNGCount = m_tLastCount.tSizeNGCounts.vecFoil[1][i];
			tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100;
			strName.Format("tSizeNGCounts.Foil%dB.nCount", i);
			WritePrivateProfileInt("LastCount", strName, tSingleNGCount.nCount, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dMeter", i);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dMeter, strFile);
			strName.Format("tSizeNGCounts.Foil%dB.dRate", i);
			//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100, strFile);
			WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dRate, strFile);
		}
		if (tPdInfo.bExistAT11 || tPdInfo.bExistBaseCoat)
		{
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].size(); i++)
			{
				tagSingleNGCount tSingleNGCount = m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i];
				tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100;
				CString strName;
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.nCount", i);
				WritePrivateProfileInt("LastCount", strName, tSingleNGCount.nCount, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dMeter", i);
				WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dMeter, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dA.dRate", i);
				WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dRate, strFile);

				if (tPdInfo.nCoatingType != TwoSide || i >= m_tCount.tSizeNGCounts.vecAT11BaseCoat[1].size()) continue;

				tSingleNGCount = m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i];
				tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100;
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.nCount", i);
				WritePrivateProfileInt("LastCount", strName, tSingleNGCount.nCount, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dMeter", i);
				WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dMeter, strFile);
				strName.Format("tSizeNGCounts.AT11BaseCoat%dB.dRate", i);
				WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dRate, strFile);
			}
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecMisalignment.size(); i++)
			{
				tagSingleNGCount tSingleNGCount = m_tLastCount.tSizeNGCounts.vecMisalignment[i];
				tSingleNGCount.dRate = tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100;
				CString strName;
				strName.Format("tSizeNGCounts.Misalignment%d.nCount", i);
				WritePrivateProfileInt("LastCount", strName, tSingleNGCount.nCount, strFile);
				strName.Format("tSizeNGCounts.Misalignment%d.dMeter", i);
				WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dMeter, strFile);
				strName.Format("tSizeNGCounts.Misalignment%d.dRate", i);
				//WritePrivateProfileDouble("Count", strName, tSingleNGCount.dMeter / m_tLastCount.dTotalMeterSelect * 100, strFile);
				WritePrivateProfileDouble("LastCount", strName, tSingleNGCount.dRate, strFile);
			}
		}
		WritePrivateProfileInt("LastCount", "nStopInfoCount", m_tLastCount.vecStopInfo.size(), strFile);
		for (int i = 0; i < m_tLastCount.vecStopInfo.size(); i++)
		{
			CString strName;
			strName.Format("StopInfo%d.strDiscribe", i);
			WritePrivateProfileString("LastCount", strName, m_tLastCount.vecStopInfo[i].strDiscribe, strFile);
			strName.Format("StopInfo%d.strTimeStart", i);
			WritePrivateProfileString("LastCount", strName, m_tLastCount.vecStopInfo[i].strTimeStart, strFile);
			strName.Format("StopInfo%d.strTimeEnd", i);
			WritePrivateProfileString("LastCount", strName, m_tLastCount.vecStopInfo[i].strTimeEnd, strFile);
			strName.Format("StopInfo%d.dMeterPosStart", i);
			WritePrivateProfileDouble("LastCount", strName, m_tLastCount.vecStopInfo[i].dMeterPosStart, strFile);
			strName.Format("StopInfo%d.dMeterPosEnd", i);
			WritePrivateProfileDouble("LastCount", strName, m_tLastCount.vecStopInfo[i].dMeterPosEnd, strFile);
		}
	}

	return TRUE;
}

void CCounter::ClearCurrent()
{
	tagCount tCount = m_tCount;

	tCount.dTotalMeter = 0;
	tCount.dTotalMeterSelect = 0;
	tCount.dTotalNGMeter = 0;
	tCount.nTotalNGCount = 0;
	tCount.tSizeNGCounts.dNGMeter = 0;
	tCount.tSizeNGCounts.nNGCount = 0;
	tCount.vecStopInfo.clear();
	for (int i = 0; i < tCount.tSizeNGCounts.vecCoat[0].size(); i++)
	{
		tCount.tSizeNGCounts.vecCoat[0][i].dMeter = 0;
		tCount.tSizeNGCounts.vecCoat[0][i].dRate = 0;
		tCount.tSizeNGCounts.vecCoat[0][i].nCount = 0;

		if (i + 1 > tCount.tSizeNGCounts.vecCoat[1].size()) continue;

		tCount.tSizeNGCounts.vecCoat[1][i].dMeter = 0;
		tCount.tSizeNGCounts.vecCoat[1][i].dRate = 0;
		tCount.tSizeNGCounts.vecCoat[1][i].nCount = 0;
	}
	for (int i = 0; i < tCount.tSizeNGCounts.vecFoil[0].size(); i++)
	{
		tCount.tSizeNGCounts.vecFoil[0][i].dMeter = 0;
		tCount.tSizeNGCounts.vecFoil[0][i].dRate = 0;
		tCount.tSizeNGCounts.vecFoil[0][i].nCount = 0;

		if (i + 1 > tCount.tSizeNGCounts.vecFoil[1].size()) continue;

		tCount.tSizeNGCounts.vecFoil[1][i].dMeter = 0;
		tCount.tSizeNGCounts.vecFoil[1][i].dRate = 0;
		tCount.tSizeNGCounts.vecFoil[1][i].nCount = 0;
	}
	for (int i = 0; i < tCount.tSizeNGCounts.vecAT11BaseCoat[0].size(); i++)
	{
		tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter = 0;
		tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate = 0;
		tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].nCount = 0;

		if (i + 1 > tCount.tSizeNGCounts.vecAT11BaseCoat[1].size()) continue;

		tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter = 0;
		tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate = 0;
		tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].nCount = 0;
	}
	for (int i = 0; i < tCount.tSizeNGCounts.vecMisalignment.size(); i++)
	{
		tCount.tSizeNGCounts.vecMisalignment[i].dMeter = 0;
		tCount.tSizeNGCounts.vecMisalignment[i].dRate = 0;
		tCount.tSizeNGCounts.vecMisalignment[i].nCount = 0;
	}

	tCount.tDefectNGCounts.dNGMeter = 0;
	tCount.tDefectNGCounts.nNGCount = 0;
	for (int i = 0; i < tCount.tDefectNGCounts.vecCoat[0].size(); i++)
	{
		tCount.tDefectNGCounts.vecCoat[0][i].dMeter = 0;
		tCount.tDefectNGCounts.vecCoat[0][i].dRate = 0;
		tCount.tDefectNGCounts.vecCoat[0][i].nCount = 0;

		if (i + 1 > tCount.tDefectNGCounts.vecCoat[1].size()) continue;

		tCount.tDefectNGCounts.vecCoat[1][i].dMeter = 0;
		tCount.tDefectNGCounts.vecCoat[1][i].dRate = 0;
		tCount.tDefectNGCounts.vecCoat[1][i].nCount = 0;
	}
	for (int i = 0; i < tCount.tDefectNGCounts.vecFoil[0].size(); i++)
	{
		tCount.tDefectNGCounts.vecFoil[0][i].dMeter = 0;
		tCount.tDefectNGCounts.vecFoil[0][i].dRate = 0;
		tCount.tDefectNGCounts.vecFoil[0][i].nCount = 0;

		if (i + 1 > tCount.tDefectNGCounts.vecFoil[1].size()) continue;

		tCount.tDefectNGCounts.vecFoil[1][i].dMeter = 0;
		tCount.tDefectNGCounts.vecFoil[1][i].dRate = 0;
		tCount.tDefectNGCounts.vecFoil[1][i].nCount = 0;
	}
	for (int i = 0; i < tCount.tDefectNGCounts.vecAT11BaseCoat[0].size(); i++)
	{
		tCount.tDefectNGCounts.vecAT11BaseCoat[0][i].dMeter = 0;
		tCount.tDefectNGCounts.vecAT11BaseCoat[0][i].dRate = 0;
		tCount.tDefectNGCounts.vecAT11BaseCoat[0][i].nCount = 0;

		if (i + 1 > tCount.tDefectNGCounts.vecAT11BaseCoat[1].size()) continue;

		tCount.tDefectNGCounts.vecAT11BaseCoat[1][i].dMeter = 0;
		tCount.tDefectNGCounts.vecAT11BaseCoat[1][i].dRate = 0;
		tCount.tDefectNGCounts.vecAT11BaseCoat[1][i].nCount = 0;
	}

	CLockHelper lk(&m_csLock);
	m_tCount = tCount;
}

BOOL CCounter::Count(C_MergeWork& tOutput)
{
	try
	{
		tagCount tCount = m_tCount;

		tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
		HTuple hvHeight;
		GetImageSize(tOutput.vecB[0].vecA[0].hoGrabImage, &HTuple(), &hvHeight);
		double dMeter = hvHeight.I() * theGlobal.m_vecResolution[0].dRow / 1000;

		m_tCount.dTotalMeter += dMeter;//总米数
		//记米赋值
		tOutput.hv_MeterCount = tCount.dTotalMeter;

		//判断有没有找到边或者没涂
		BOOL bHasCoat = TRUE;
		if (tOutput.vecB[0].tMeasure.hv_HasCoat.I() != TRUE)
		{
			CString strLogHasCoat;
			strLogHasCoat.Format("tOutput.vecB[0].tMeasure.hv_HasCoat.I()=false-%d", tOutput.vecB[0].tMeasure.hv_HasCoat.I());
			theGlobal.m_Log.Debug(strLogHasCoat);
			bHasCoat = FALSE;
		}
		if (tOutput.vecB.size() > 1 && tOutput.vecB[1].tMeasure.hv_HasCoat.I() != TRUE)
		{
			CString strLogHasCoat;
			strLogHasCoat.Format("tOutput.vecB[1].tMeasure.hv_HasCoat.I()=false-%d", tOutput.vecB[0].tMeasure.hv_HasCoat.I());
			theGlobal.m_Log.Debug(strLogHasCoat);
			bHasCoat = FALSE;
		}
		HTuple hvFind0, hvFindNoData;
		TupleFind(tOutput.vecB[0].tMeasure.tSize.hv_Width, 0, &hvFind0);
		TupleFind(tOutput.vecB[0].tMeasure.tSize.hv_Width, NO_DATA, &hvFindNoData);
		if (hvFind0.Length() != 0 && hvFind0 != -1)
		{
			CString strLogHasCoat;
			strLogHasCoat.Format("(hvFind0.Length() != 0 && hvFind0 != -1)=false-%d", hvFind0.I());
			theGlobal.m_Log.Debug(strLogHasCoat);
			bHasCoat = FALSE;
		}
		if (hvFindNoData.Length() != 0 && hvFindNoData != -1)
		{
			CString strLogHasCoat;
			strLogHasCoat.Format("(hvFindNoData.Length() != 0 && hvFindNoData != -1)=false-%d", hvFindNoData.I());
			theGlobal.m_Log.Debug(strLogHasCoat);
			bHasCoat = FALSE;
		}
		if (tOutput.vecB.size() > 1&&tPdInfo.nCoatingType==1)
		{
			TupleFind(tOutput.vecB[1].tMeasure.tSize.hv_Width, 0, &hvFind0);
			TupleFind(tOutput.vecB[1].tMeasure.tSize.hv_Width, NO_DATA, &hvFindNoData);
			if (hvFind0.Length() != 0 && hvFind0 != -1)
			{
				CString strLogHasCoat;
				strLogHasCoat.Format("(tOutput.vecB.size() > 1&&hvFind0.Length() != 0 && hvFind0 != -1)=false-%d", hvFind0.I());
				theGlobal.m_Log.Debug(strLogHasCoat);
				bHasCoat = FALSE;
			}
			if (hvFindNoData.Length() != 0 && hvFindNoData != -1)
			{
				CString strLogHasCoat;
				strLogHasCoat.Format("(tOutput.vecB.size() > 1&&hvFindNoData.Length() != 0 && hvFindNoData != -1)=false-%d", hvFindNoData.I());
				theGlobal.m_Log.Debug(strLogHasCoat);
				bHasCoat = FALSE;
			}
		}

		//设备停止或者没涂都不计入统计
		BOOL bStopCoating = FALSE;
		if (theGlobal.m_DataCalculate.m_tParam.bUseStopPosDataFiltering)
		{

			if (1 != tOutput.vecB[0].vecA[0].hvRunDirection.I()
				|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[1].I()
				|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[2].I())
			{
				CString strLogPlc;
				strLogPlc.Format("Count: 收到设备停机、未涂膜信号，不进行统计%d-%d-%d", tOutput.vecB[0].vecA[0].hvRunDirection.I(), tOutput.vecB[0].vecA[0].hvRunDirection[1].I(), tOutput.vecB[0].vecA[0].hvRunDirection[1].I());
				theGlobal.m_Log.Debug(strLogPlc);
				bStopCoating = TRUE;
			}

			if (!GetCountDataFilteringEnble(tOutput))
			{
				CString strLogData;
				strLogData.Format("Count: 数据过滤，不进行统计");
				theGlobal.m_Log.Debug(strLogData);
				return TRUE;
			}
			//记录停机位
			if (bStopCoating)
			{
				if (!m_bStopPos)
				{
					m_bStopPos = TRUE;
					//记录停止涂的位置
					tCount = m_tCount;
					tagStopInfo tInfo;
					tInfo.Clear();
					tInfo.dMeterPosStart = m_tCount.dTotalMeter;
					COleDateTime time = ATL::COleDateTime::GetCurrentTime();
					tInfo.strTimeStart = time.Format("%Y-%m-%d %H:%M:%S");
					tInfo.strDiscribe = "其他";
					tCount.vecStopInfo.push_back(tInfo);

					//选择停机原因
					if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
					{
						theGlobal.SetTipInfo(TIP_StopPosReason);
					}

					CString strLog;
					strLog.Format("Count: 判断出停机位的开始位置");
					theGlobal.m_Log.Debug(strLog);

					CLockHelper lk(&m_csLock);
					m_tCount = tCount;
					return TRUE;
				}
			}
			else//设备开涂，且软件检测到有涂布
			{
				static int s_nMeterCount = m_tCount.dTotalMeter;
				if (s_nMeterCount > m_tCount.dTotalMeter) s_nMeterCount = m_tCount.dTotalMeter;
				if (m_bStopPos)
				{
					//出现停机位结束5m不记录
					if (m_tCount.dTotalMeter >= s_nMeterCount + 5)
					{
						m_bStopPos = FALSE;
						//记录开始涂的位置
						tCount = m_tCount;
						if (tCount.vecStopInfo.size() > 0)
						{
							tagStopInfo tInfo = tCount.vecStopInfo[tCount.vecStopInfo.size() - 1];
							tInfo.dMeterPosEnd = m_tCount.dTotalMeter;
							COleDateTime time = ATL::COleDateTime::GetCurrentTime();
							tInfo.strTimeEnd = time.Format("%Y-%m-%d %H:%M:%S");
							tCount.vecStopInfo[tCount.vecStopInfo.size() - 1] = tInfo;
						}

						CString strLog;
						strLog.Format("Count: 判断出停机位的结束位置");
						theGlobal.m_Log.Debug(strLog);

						CLockHelper lk(&m_csLock);
						m_tCount = tCount;
					}
					else
					{
						bHasCoat = FALSE;
					}
				}
				else
				{

				}
			}
		}


		if (!bHasCoat)
		{
			/*CLockHelper lk(&m_csLock);
			m_tCount = tCount;*/
			CString strLogbHasCoat;
			strLogbHasCoat.Format("Count: 没找到边，不进行统计");
			theGlobal.m_Log.Debug(strLogbHasCoat);
			return TRUE;
		}


		tCount = m_tCount;
		tCount.dTotalMeterSelect += dMeter;//有效总米数

		if (tOutput.tMerge.hv_FinalFlag.I() == NG && tOutput.tMerge.hv_MeasureFlag.I()==NG)
		{
			tCount.dTotalNGMeter += dMeter;//总NG米数(1109修改-只统计尺寸)
			tCount.nTotalNGCount++;
			//涂布尺寸NG统计
			HTuple hvFlagList = tOutput.tMerge.vecSideResult[0].hv_CoatWidthFlagList;
			for (int i = 0; i < hvFlagList.Length(); i++)
			{
				if (i + 1 > tCount.tSizeNGCounts.vecCoat[0].size()) continue;
				if (hvFlagList[i] == NG)
				{
					tCount.tSizeNGCounts.vecCoat[0][i].dMeter += dMeter;//涂布尺寸NG米数
					tCount.tSizeNGCounts.vecCoat[0][i].nCount += 1;
				}

				if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecCoat[0][i].dRate = 0;
				else tCount.tSizeNGCounts.vecCoat[0][i].dRate = tCount.tSizeNGCounts.vecCoat[0][i].dMeter / tCount.dTotalMeterSelect * 100;//涂布尺寸NG米数/总有效米数
			}

			if (tOutput.vecB.size() > 1 && tPdInfo.nCoatingType == TwoSide)
			{
				hvFlagList = tOutput.tMerge.vecSideResult[1].hv_CoatWidthFlagList;
				for (int i = 0; i < hvFlagList.Length(); i++)
				{
					if (i + 1 > tCount.tSizeNGCounts.vecCoat[1].size()) continue;
					if (hvFlagList[i] == NG)
					{
						tCount.tSizeNGCounts.vecCoat[1][i].dMeter += dMeter;//反面涂布尺寸NG米数
						tCount.tSizeNGCounts.vecCoat[1][i].nCount += 1;
					}

					if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecCoat[1][i].dRate = 0;
					else tCount.tSizeNGCounts.vecCoat[1][i].dRate = tCount.tSizeNGCounts.vecCoat[1][i].dMeter / tCount.dTotalMeterSelect * 100;
				}
			}

			//极耳尺寸NG统计
			hvFlagList = tOutput.tMerge.vecSideResult[0].hv_FoilWidthFlagList;
			for (int i = 0; i < hvFlagList.Length(); i++)
			{
				if (i + 1 > tCount.tSizeNGCounts.vecFoil[0].size()) continue;
				if (hvFlagList[i] == NG)
				{
					tCount.tSizeNGCounts.vecFoil[0][i].dMeter += dMeter;
					tCount.tSizeNGCounts.vecFoil[0][i].nCount += 1;
				}

				if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecFoil[0][i].dRate = 0;
				else tCount.tSizeNGCounts.vecFoil[0][i].dRate = tCount.tSizeNGCounts.vecFoil[0][i].dMeter / tCount.dTotalMeterSelect * 100;
			}
			if (tOutput.vecB.size() > 1 && tPdInfo.nCoatingType == TwoSide)
			{
				hvFlagList = tOutput.tMerge.vecSideResult[1].hv_FoilWidthFlagList;
				for (int i = 0; i < hvFlagList.Length(); i++)
				{
					if (i + 1 > tCount.tSizeNGCounts.vecFoil[1].size()) continue;
					if (hvFlagList[i] == NG)
					{
						tCount.tSizeNGCounts.vecFoil[1][i].dMeter += dMeter;
						tCount.tSizeNGCounts.vecFoil[1][i].nCount += 1;
					}

					if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecFoil[1][i].dRate = 0;
					else tCount.tSizeNGCounts.vecFoil[1][i].dRate = tCount.tSizeNGCounts.vecFoil[1][i].dMeter / tCount.dTotalMeterSelect * 100;
				}
			}
			if (tPdInfo.bExistAT11)
			{
				//AT11尺寸NG统计
				hvFlagList = tOutput.tMerge.vecSideResult[0].hv_AT11WidthFlagList;
				for (int i = 0; i < hvFlagList.Length(); i++)
				{
					if (i + 1 > tCount.tSizeNGCounts.vecAT11BaseCoat[0].size()) continue;
					if (hvFlagList[i] == NG)
					{
						tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter += dMeter;
						tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].nCount += 1;
					}

					if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate = 0;
					else tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate = tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter / tCount.dTotalMeterSelect * 100;
				}
				if (tOutput.vecB.size() > 1)
				{
					hvFlagList = tOutput.tMerge.vecSideResult[1].hv_AT11WidthFlagList;
					for (int i = 0; i < hvFlagList.Length(); i++)
					{
						if (i + 1 > tCount.tSizeNGCounts.vecAT11BaseCoat[1].size()) continue;
						if (hvFlagList[i] == NG)
						{
							tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter += dMeter;
							tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].nCount += 1;
						}

						if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate = 0;
						else tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate = tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter / tCount.dTotalMeterSelect * 100;
					}
				}
			}
			else if (tPdInfo.bExistBaseCoat)
			{
				//凹版尺寸NG统计
				hvFlagList = tOutput.tMerge.vecSideResult[0].hv_BaseCoatWidthFlagList;
				for (int i = 0; i < hvFlagList.Length(); i++)
				{
					if (i + 1 > tCount.tSizeNGCounts.vecAT11BaseCoat[0].size()) continue;
					if (hvFlagList[i] == NG)
					{
						tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter += dMeter;
						tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].nCount += 1;
					}

					if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate = 0;
					else tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate = tCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter / tCount.dTotalMeterSelect * 100;
				}
				if (tOutput.vecB.size() > 1)
				{
					hvFlagList = tOutput.tMerge.vecSideResult[1].hv_BaseCoatWidthFlagList;
					for (int i = 0; i < hvFlagList.Length(); i++)
					{
						if (i + 1 > tCount.tSizeNGCounts.vecAT11BaseCoat[1].size()) continue;
						if (hvFlagList[i] == NG)
						{
							tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter += dMeter;
							tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].nCount += 1;
						}

						if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate = 0;
						else tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate = tCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter / tCount.dTotalMeterSelect * 100;
					}
				}
			}
			//错位NG统计
			if (tOutput.vecB.size() > 1 && tPdInfo.nCoatingType == TwoSide)
			{
				hvFlagList = tOutput.tMerge.hv_MisalignmentFlagList;
				for (int i = 0; i < hvFlagList.Length(); i++)
				{
					if (i + 1 > tCount.tSizeNGCounts.vecMisalignment.size()) continue;
					if (hvFlagList[i] == NG)
					{
						tCount.tSizeNGCounts.vecMisalignment[i].dMeter += dMeter;
						tCount.tSizeNGCounts.vecMisalignment[i].nCount += 1;
					}

					if (tCount.dTotalMeterSelect == 0) tCount.tSizeNGCounts.vecMisalignment[i].dRate = 0;
					else tCount.tSizeNGCounts.vecMisalignment[i].dRate = tCount.tSizeNGCounts.vecMisalignment[i].dMeter / tCount.dTotalMeterSelect * 100;
				}
			}

			//统计瑕疵NG
			if (tOutput.tMerge.hv_DetectFlag.I() == NG)
			{
				HObject hoR;
				Union1(tOutput.tMerge.ho_NGRegion, &hoR);
				//Connection(hoR, &hoR);
				HTuple hvHei;
				RegionFeatures(hoR, "height", &hvHei);
				hvHei = hvHei * theGlobal.m_vecResolution[0].dRow / 1000;

				tCount.tDefectNGCounts.dNGMeter += hvHei.D();
				tCount.tDefectNGCounts.nNGCount += tOutput.tMerge.ho_NGRegion.CountObj();
			}
		}

		CLockHelper lk(&m_csLock);
		m_tCount = tCount;
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CCounter_Count: 异常%s(%s)", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
		return FALSE;
	}

	return TRUE;
}

void CCounter::EndBatch()
{
	CString strDataFile, strBatch;
	strDataFile.Format("%s", theGlobal.m_Storage.GetSaveDataFilePath());
	strBatch = m_tCurrentBatch.strName;

	m_tCurrentBatch.timeEnd = COleDateTime::GetCurrentTime();
	m_tLastBatch = m_tCurrentBatch;

	m_tLastCount = m_tCount;
	ClearCurrent();
	theGlobal.m_Judge.ClearJudgeSumMetes();

	//记录换卷的停机位作为结束位置
	COleDateTime time = ATL::COleDateTime::GetCurrentTime();
	if (m_tLastCount.vecStopInfo.size() > 0)
	{
		tagStopInfo tInfo = m_tLastCount.vecStopInfo[m_tLastCount.vecStopInfo.size() - 1];
		//上一卷没有空带结束位置的时候
		if (tInfo.strTimeEnd == "")
		{
			tInfo.dMeterPosEnd = m_tLastCount.dTotalMeter;
			tInfo.strTimeEnd = time.Format("%Y-%m-%d %H:%M:%S");
			m_tLastCount.vecStopInfo[m_tLastCount.vecStopInfo.size() - 1] = tInfo;

			//记录新的位置
			tagCount tCount = m_tCount;
			tagStopInfo tInfo;
			tInfo.Clear();
			tInfo.dMeterPosStart = m_tCount.dTotalMeter;
			COleDateTime time = ATL::COleDateTime::GetCurrentTime();
			tInfo.strTimeStart = time.Format("%Y-%m-%d %H:%M:%S");
			tInfo.strDiscribe = "其他";
			tCount.vecStopInfo.push_back(tInfo);

			CLockHelper lk(&m_csLock);
			m_tCount = tCount;
		}
	}


	//生成报表
	theGlobal.m_Counter.WriteLastBatchRecord();

	//上传数据文件
	if (PathFileExists(strDataFile))
	{
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::MES, "CopyCcdFileToServer", &strDataFile, &strBatch);
	}
}


BOOL CCounter::GetCountEnable(const int& nNGType)
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	BOOL bEnable = FALSE;
	switch (nNGType)
	{
	case NG_CoatWidth:
		if (tPdInfo.tParam.bUseCoatWidth) bEnable = TRUE;
		break;
	case NG_FoilWidth:
		if (tPdInfo.tParam.bUseFoilWidth) bEnable = TRUE;
		break;
	case NG_AT11Width:
		if (tPdInfo.tParam.bUseAT11Width && tPdInfo.bExistAT11) bEnable = TRUE;
		break;
	case NG_BaseCoatWidth:
		if (tPdInfo.tParam.bUseBaseCoatWidth && tPdInfo.bExistBaseCoat) bEnable = TRUE;
		break;
	case NG_ABMisalign:
		if (tPdInfo.tParam.bUseMisalignment) bEnable = TRUE;
		break;
	case NG_ExposeFoil:
		if (tPdInfo.tParam.bUseSurfaceDetect) bEnable = TRUE;
		break;
	default:
		if (tPdInfo.tParam.bUseSurfaceDetect) bEnable = TRUE;
		break;
	}

	return TRUE;
}

BOOL CCounter::SetCurrentStopDiscribe(CString strInfo)
{
	tagCount tCount = m_tCount;

	if (tCount.vecStopInfo.size() == 0) return FALSE;

	tCount.vecStopInfo[tCount.vecStopInfo.size() - 1].strDiscribe = strInfo;

	CLockHelper lk(&m_csLock);
	m_tCount = tCount;
	return TRUE;
}

BOOL CCounter::WriteLastBatchRecord()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//定义当前系统时间变量
	SYSTEMTIME CurSysTime;
	//传引用方式获取当前系统时间
	GetLocalTime(&CurSysTime);
	//定义当前卷贴标记录全路径
	CString strSizeRecord, strSizePath;
	//定义工位名称
	CString  strGongWeiName;

	try
	{
		//定义计数
		CString strCount;
		//调用系统信息获取工位
		strGongWeiName = theGlobal.GetSystemSet().strDeviceID;
		//当前卷贴标记录路径格式化
		//区域+机台号+涂布膜卷号+品种；式例：M5_TMJI0001_C2220181124M0132_MFH3L7-BAEL
		strSizeRecord.Format("%s\\%s_%s_%s_%s_Report.csv", theGlobal.m_Storage.GetDataSaveFolderPath(), theGlobal.GetSystemSet().strWorkshopName, theGlobal.GetSystemSet().strDeviceID,
			(theGlobal.m_Counter.GetLastBatch().strName + (tPdInfo.nCoatingType == OneSide ? "A" : "")), theGlobal.m_ProductTool.GetCurrentProductName());

		if (!PathFileExists(theGlobal.m_Storage.GetDataSaveFolderPath())) CreateMultilevelPath(theGlobal.m_Storage.GetDataSaveFolderPath());

		//打开文件
		CStdioFile file;
		if (TRUE != file.Open(strSizeRecord, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("Counter_WriteBatchRecord: 创建报表文件失败 (%s)", strSizeRecord);
			theGlobal.m_Log.Exception(strException);

			return FALSE;
		}

		//写入文件
		CString strItem;
		CString strTestParamTitle;
		CString strStdValue;
		CString strSide;
		CString strTmp;
		CString strShipment;
		//格式化xls文档列标题
		//hack 出货标准
		strItem.Format(
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"尺寸&瑕疵判定结果:\n"
			", , 最终判定结果：%s\n"
			"★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★\n\n"
			"产品名：,%s\n"
			"卷开始时间：,%s\n"
			"卷结束时间：,%s\n\n"

			, strShipment=GetShipment()==TRUE?"正常出货！":"不能出货！", theGlobal.m_Counter.GetLastBatch().strName, theGlobal.m_Counter.GetLastBatch().GetStartTime(), theGlobal.m_Counter.GetLastBatch().GetEndTime());
		file.WriteString(strItem);
		strItem.Format(
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"设定参数规格：\n"
			"所有尺寸\n"
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"水平方向尺寸测量参数设置\n"
			", , 涂布条数, 单 / 双面, AT11/凹版, 涂布类型, 闭环状态, 闭环方式, Cpk分段间距\n"
		);
		file.WriteString(strItem);
		CString side = tPdInfo.nCoatingType == TwoSide ? "双面" : "单面";
		CString strAT11;
		if (tPdInfo.bExistAT11)
			strAT11 = "AT11";
		else if (tPdInfo.bExistBaseCoat)
			strAT11 = "凹版";
		else
			strAT11 = "无";
		int nCorrect = FALSE;
		theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetCorrectingEnable", NULL, &nCorrect);
		strTestParamTitle.Format(
			", , %d,%s , %s, %s, %s, %s, %.2f米\n\n"
			, tPdInfo.tParam.vecCoatWidth.size(), side, strAT11, tPdInfo.bExistIntermittent ? "间歇" : "连续",
			nCorrect == TRUE ? "开启" : "关闭", tPdInfo.nCoatingType == OneSide ? "单面闭环" : "双面错位闭环", theGlobal.m_DataCalculate.m_tParam.dSelectMeter);
		file.WriteString(strTestParamTitle);

#pragma region 正面
		strSide.Format(",正面");
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			strTmp.Format(",涂膜-%d", i);
			strSide += strTmp;
		}
		strSide += ",";
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strTmp.Format(",极耳-%d", i);
			strSide += strTmp;
		}
		if (tPdInfo.tParam.bUseAT11Width)
		{
			strSide += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				strTmp.Format(",AT11-%d", i);
				strSide += strTmp;
			}
		}
		else if (tPdInfo.tParam.bUseBaseCoatWidth)
		{
			strSide += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				strTmp.Format(",凹版-%d", i);
				strSide += strTmp;
			}
		}
		strSide += "\n";
		file.WriteString(strSide);
#pragma region 正面标准值
		strStdValue.Format(",标准值");
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[0].dStandard);
			strStdValue += strTmp;
		}
		strStdValue += ",";
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[i].dStandard);
			strStdValue += strTmp;
		}
		if (tPdInfo.tParam.bUseAT11Width)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[0].dStandard);
				strStdValue += strTmp;
			}
		}
		else if (tPdInfo.tParam.bUseBaseCoatWidth)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[0].dStandard);
				strStdValue += strTmp;
			}
		}
		strStdValue += "\n";
		file.WriteString(strStdValue);
#pragma endregion

#pragma region 正面预警公差
		strStdValue.Format(",预警公差");
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance);
			strStdValue += strTmp;
		}
		strStdValue += ",";
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance);
			strStdValue += strTmp;
		}
		if (tPdInfo.tParam.bUseAT11Width)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[0].dWarnTolerance);
				strStdValue += strTmp;
			}
		}
		else if (tPdInfo.tParam.bUseBaseCoatWidth)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance);
				strStdValue += strTmp;
			}
		}
		strStdValue += "\n";
		file.WriteString(strStdValue);
#pragma endregion

#pragma region 正面报警公差
		strStdValue.Format(",报警公差");
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[0].dTolerance);
			strStdValue += strTmp;
		}
		strStdValue += ",";
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[i].dTolerance);
			strStdValue += strTmp;
		}
		if (tPdInfo.tParam.bUseAT11Width)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[0].dTolerance);
				strStdValue += strTmp;
			}
		}
		else if (tPdInfo.tParam.bUseBaseCoatWidth)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance);
				strStdValue += strTmp;
			}
		}
		strStdValue += "\n";
		file.WriteString(strStdValue);
#pragma endregion

#pragma region 正面补偿值
		strStdValue.Format(",补偿值");
		for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[i].OffsetList[0]);
			strStdValue += strTmp;
		}
		strStdValue += ",";
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[i].OffsetList[0]);
			strStdValue += strTmp;
		}
		if (tPdInfo.tParam.bUseAT11Width)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[i].OffsetList[0]);
				strStdValue += strTmp;
			}
		}
		else if (tPdInfo.tParam.bUseBaseCoatWidth)
		{
			strStdValue += ",,";
			for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[0]);
				strStdValue += strTmp;
			}
		}
		strStdValue += "\n";
		file.WriteString(strStdValue);
#pragma endregion
#pragma endregion
		file.WriteString("\n\n");
		if (tPdInfo.nCoatingType != OneSide)
		{
#pragma region 反面
			strSide.Format(",反面");
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				strTmp.Format(",涂膜-%d", i);
				strSide += strTmp;
			}
			strSide += ",";
			for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
			{
				strTmp.Format(",极耳-%d", i);
				strSide += strTmp;
			}
			if (tPdInfo.tParam.bUseAT11Width)
			{
				strSide += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
				{
					strTmp.Format(",AT11-%d", i);
					strSide += strTmp;
				}
			}
			else if (tPdInfo.tParam.bUseBaseCoatWidth)
			{
				strSide += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
				{
					strTmp.Format(",凹版-%d", i);
					strSide += strTmp;
				}
			}
			strSide += "\n";
			file.WriteString(strSide);
#pragma region 反面标准值
			strStdValue.Format(",标准值");
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[0].dStandard);
				strStdValue += strTmp;
			}
			strStdValue += ",";
			for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[i].dStandard);
				strStdValue += strTmp;
			}
			if (tPdInfo.tParam.bUseAT11Width)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[0].dStandard);
					strStdValue += strTmp;
				}
			}
			else if (tPdInfo.tParam.bUseBaseCoatWidth)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[0].dStandard);
					strStdValue += strTmp;
				}
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion

#pragma region 反面预警公差
			strStdValue.Format(",预警公差");
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance);
				strStdValue += strTmp;
			}
			strStdValue += ",";
			for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance);
				strStdValue += strTmp;
			}
			if (tPdInfo.tParam.bUseAT11Width)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[0].dWarnTolerance);
					strStdValue += strTmp;
				}
			}
			else if (tPdInfo.tParam.bUseBaseCoatWidth)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance);
					strStdValue += strTmp;
				}
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion

#pragma region 反面报警公差
			strStdValue.Format(",报警公差");
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[0].dTolerance);
				strStdValue += strTmp;
			}
			strStdValue += ",";
			for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[0].dTolerance);
				strStdValue += strTmp;
			}
			if (tPdInfo.tParam.bUseAT11Width)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[0].dTolerance);
					strStdValue += strTmp;
				}
			}
			else if (tPdInfo.tParam.bUseBaseCoatWidth)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance);
					strStdValue += strTmp;
				}
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion

#pragma region 反面补偿值
			strStdValue.Format(",补偿值");
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecCoatWidth[i].OffsetList[1]);
				strStdValue += strTmp;
			}
			strStdValue += ",";
			for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecFoilWidth[i].OffsetList[1]);
				strStdValue += strTmp;
			}
			if (tPdInfo.tParam.bUseAT11Width)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecAT11Width.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecAT11Width[i].OffsetList[1]);
					strStdValue += strTmp;
				}
			}
			else if (tPdInfo.tParam.bUseBaseCoatWidth)
			{
				strStdValue += ",,";
				for (int i = 0; i < tPdInfo.tParam.vecBaseCoatWidth.size(); i++)
				{
					strTmp.Format(",%0.3f", tPdInfo.tParam.vecBaseCoatWidth[i].OffsetList[1]);
					strStdValue += strTmp;
				}
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion
#pragma endregion
			file.WriteString("\n\n");
#pragma region 错位
			strSide.Format(",错位");
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strTmp.Format(",涂膜错位-%d", i);
				strSide += strTmp;
			}
			strSide += "\n";
			file.WriteString(strSide);
#pragma region 错位标准值
			strStdValue.Format(",标准值");
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecMisalignment[0].dStandard);
				strStdValue += strTmp;
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion

#pragma region 错位预警公差
			strStdValue.Format(",预警公差");
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecMisalignment[0].dWarnTolerance);
				strStdValue += strTmp;
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion

#pragma region 错位报警公差
			strStdValue.Format(",报警公差");
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecMisalignment[0].dTolerance);
				strStdValue += strTmp;
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion

#pragma region 错位补偿
			strStdValue.Format(",错位补偿0");
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecMisalignment[i].OffsetList[0]);
				strStdValue += strTmp;
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
			strStdValue.Format(",错位补偿1");
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				strTmp.Format(",%0.3f", tPdInfo.tParam.vecMisalignment[i].OffsetList[1]);
				strStdValue += strTmp;
			}
			strStdValue += "\n";
			file.WriteString(strStdValue);
#pragma endregion

#pragma endregion
		}

		file.WriteString("\n\n");

		strItem.Format(
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"补偿记录(见配方修改日志)\n\n"
			/*"第1次, 手动, 2020 - 10 - 01(10:46)\n"
			", 正面, A1(左极耳), A2(膜1), A3(空箔), A4(膜2), A5(右极耳), , Gra_1, Gra_2, Gra_3, Gra_4\n"
			", 补偿值, 0.00, 0.00, 0.00, 0.00, 0.00, , 0.00, 0.00, 0.00, 0.00\n"
			", 补偿(离线值), 0.00, 0.00, 0.00, 0.00, 0.00, , 0.00, 0.00, 0.00, 0.00\n"
			", 补偿(在线值), 0.00, 0.00, 0.00, 0.00, 0.00, , 0.00, 0.00, 0.00, 0.00\n"
			", 反面, A1(左极耳), A2(膜1), A3(空箔), A4(膜2), A5(右极耳), , Gra_1, Gra_2, Gra_3, Gra_4\n"*/

		);
		file.WriteString(strItem);

		//strItem.Format(
		//	/*", 补偿值, 0.00, 0.00, 0.00, 0.00, 0.00, , 0.00, 0.00, 0.00, 0.00\n"
		//	", 补偿(离线值), 0.00, 0.00, 0.00, 0.00, 0.00, , 0.00, 0.00, 0.00, 0.00\n"
		//	", 补偿(在线值), 0.00, 0.00, 0.00, 0.00, 0.00, , 0.00, 0.00, 0.00, 0.00\n"*/
		//	"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
		//	"瑕疵检测设置\n"
		//	", , 涂布瑕疵(正), 涂布瑕疵(反), Gra瑕疵(正), Gra瑕疵(反), 极耳瑕疵(正), 极耳瑕疵(反)\n"
		//	", 方法, 方法一, 方法一, 方法一, 方法一, 方法一, 方法一\n"
		//	", 亮度, 80, 0, 40, 0, 255, 0\n"
		//	", 暗度, 20, 0, 0, 0, 225, 0\n"
		//	", 最小面积, 0.10, 0.00, 0.10, 0.00, 0.10, 0.00\n"
		//	", 最大面积, 9999.99, 0.00, 9999.99, 0.00, 9999.99, 0.00\n"
		//);
		//file.WriteString(strItem);
#pragma region 单值判定
		strItem.Format("== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"单值判定\n"
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"料卷总长：, %0.3fm\n涂膜区总长：, %0.3fm\n涂膜区占比：, %0.2f%%\n", m_tLastCount.dTotalMeter, m_tLastCount.dTotalMeterSelect,
			m_tLastCount.dTotalMeter == 0 ? 0.0 : m_tLastCount.dTotalMeterSelect / m_tLastCount.dTotalMeter * 100);
		for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecCoat[0].size(); i++)
		{
			m_tLastCount.tSizeNGCounts.vecCoat[0][i].dRate = m_tLastCount.tSizeNGCounts.vecCoat[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
			strTmp.Format("正面第%d条膜宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
				m_tLastCount.tSizeNGCounts.vecCoat[0][i].dMeter, m_tLastCount.tSizeNGCounts.vecCoat[0][i].dRate);
			strItem += strTmp;
		}
		for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecFoil[0].size(); i++)
		{
			m_tLastCount.tSizeNGCounts.vecFoil[0][i].dRate = m_tLastCount.tSizeNGCounts.vecFoil[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
			strTmp.Format("正面第%d条极耳宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
				m_tLastCount.tSizeNGCounts.vecFoil[0][i].dMeter, m_tLastCount.tSizeNGCounts.vecFoil[0][i].dRate);
			strItem += strTmp;
		}
		if (tPdInfo.bExistAT11)
		{
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].size(); i++)
			{
				m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate = m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
				strTmp.Format("正面第%d条AT11宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
					m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter, m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate);
				strItem += strTmp;
			}
		}
		else if (tPdInfo.bExistBaseCoat)
		{
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].size(); i++)
			{
				m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate = m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
				strTmp.Format("正面第%d条凹版宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
					m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter, m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dRate);
				strItem += strTmp;
			}
		}
		file.WriteString(strItem);
		if (tPdInfo.nCoatingType == TwoSide)
		{
			strItem = "";
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecCoat[1].size(); i++)
			{
				m_tLastCount.tSizeNGCounts.vecCoat[1][i].dRate = m_tLastCount.tSizeNGCounts.vecCoat[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
				strTmp.Format("反面第%d条膜宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
					m_tLastCount.tSizeNGCounts.vecCoat[1][i].dMeter, m_tLastCount.tSizeNGCounts.vecCoat[1][i].dRate);
				strItem += strTmp;
			}
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecFoil[1].size(); i++)
			{
				m_tLastCount.tSizeNGCounts.vecFoil[1][i].dRate = m_tLastCount.tSizeNGCounts.vecFoil[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
				strTmp.Format("反面第%d条极耳宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
					m_tLastCount.tSizeNGCounts.vecFoil[1][i].dMeter, m_tLastCount.tSizeNGCounts.vecFoil[1][i].dRate);
				strItem += strTmp;
			}
			if (tPdInfo.bExistAT11)
			{
				for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size(); i++)
				{
					m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate = m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
					strTmp.Format("反面第%d条AT11宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
						m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter, m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate);
					strItem += strTmp;
				}
			}
			else if (tPdInfo.bExistBaseCoat)
			{
				for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size(); i++)
				{
					m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate = m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
					strTmp.Format("反面第%d条凹版宽异常米数： %0.3fm  所占比例： %0.2f%%\n", i + 1,
						m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter, m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dRate);
					strItem += strTmp;
				}
			}
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecMisalignment.size(); i++)
			{
				m_tLastCount.tSizeNGCounts.vecMisalignment[i].dRate = m_tLastCount.tSizeNGCounts.vecMisalignment[i].dMeter / m_tLastCount.dTotalMeterSelect * 100;
				strTmp.Format("第%d条涂膜%s错位异常米数： %0.3fm  所占比例： %0.2f%%\n", i/2 + 1,i%2==0?"左":"右",
					m_tLastCount.tSizeNGCounts.vecMisalignment[i].dMeter, m_tLastCount.tSizeNGCounts.vecMisalignment[i].dRate);
				strItem += strTmp;
			}
			file.WriteString(strItem);
		}
		//总统计
		strItem.Format("异常尺寸总占比：%0.2f%%\n\n", m_tLastCount.dTotalNGMeter / m_tLastCount.dTotalMeterSelect * 100);
		/*strItem = (
			", , , 膜1, 膜2,\n"
			"(正)膜宽\tOK, OK,\n"
			"(正)极耳宽度\tOK, OK, OK,\n"
			"(正)左侧AT11宽度\tOK, OK,\n"
			"(正)右侧AT11宽度\t OK, OK,\n"
			"异常尺寸判断结果：OK，能出货。\n\n"
			"异常尺寸总占比：32.31%\n\n"
			);*/
		file.WriteString(strItem);
#pragma endregion

		strItem.Format(
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"均值判定\r\n"
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			", , 膜宽, Sig, Ca, Cp, Cpk\n");
		file.WriteString(strItem);
#pragma region A面测量指标
		for (int i = 0; i < theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecData[(int)CDataCalculate::TI_Coat].Length(); i++)
		{
			strItem.Format(
				"正面第%d条涂膜,,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n"
				, i + 1, theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideMeanTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideSigmaTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideCaTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideCpTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideCpkTotalValue[(int)CDataCalculate::TI_Coat][i].T().D()
			);
			file.WriteString(strItem);
		}

		if (tPdInfo.bExistBaseCoat || tPdInfo.bExistAT11)
		{
			strItem.Format("\n, , 均值, Sig\n");
			file.WriteString(strItem);
			CString strATBaseCoat = "AT11";
			if (tPdInfo.bExistBaseCoat)
			{
				strATBaseCoat = "凹版";
			}
			for (int i = 0; i < theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecData[(int)CDataCalculate::TI_AT11BaseCoat].Length(); i++)
			{
				strItem.Format(
					"正面第%d条%s,,%0.3f,%0.3f\n"
					, i + 1, strATBaseCoat, theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideMeanTotalValue[(int)CDataCalculate::TI_AT11BaseCoat][i].T().D(),
					theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideSigmaTotalValue[(int)CDataCalculate::TI_AT11BaseCoat][i].T().D()
				);
				file.WriteString(strItem);
			}
		}

		/*strItem.Format(
			"正面膜宽均值正常，在【LSL ~USL】范围内\n");
		file.WriteString(strItem);*/
#pragma endregion


#pragma region B面测量指标
		for (int i = 0; i < theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecData[(int)CDataCalculate::TI_Coat].Length(); i++)
		{
			strItem.Format(
				"反面第%d条涂膜,,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n"
				, i, theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecSideMeanTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecSideSigmaTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecSideCaTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecSideCpTotalValue[(int)CDataCalculate::TI_Coat][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecSideCpkTotalValue[(int)CDataCalculate::TI_Coat][i].T().D()
			);
			file.WriteString(strItem);
		}
		if (tPdInfo.bExistBaseCoat || tPdInfo.bExistAT11)
		{
			strItem.Format("\n, , 均值, Sig\n");
			file.WriteString(strItem);
			CString strATBaseCoat = "AT11";
			if (tPdInfo.bExistBaseCoat)
			{
				strATBaseCoat = "凹版";
			}
			for (int i = 0; i < theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecData[(int)CDataCalculate::TI_AT11BaseCoat].Length(); i++)
			{
				strItem.Format(
					"反面第%d条%s,,%0.3f,%0.3f\n"
					, i + 1, strATBaseCoat, theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecSideMeanTotalValue[(int)CDataCalculate::TI_AT11BaseCoat][i].T().D(),
					theGlobal.m_DataCalculate.GetLastMeasureQuota(1).hvecSideSigmaTotalValue[(int)CDataCalculate::TI_AT11BaseCoat][i].T().D()
				);
				file.WriteString(strItem);
			}
		}
		/*strItem.Format(
			"反面膜宽均值正常，在【LSL ~USL】范围内\n");
		file.WriteString(strItem);*/

		//错位指标
		strItem.Format("\n, , 均值, Sig, Ca, Cp, Cpk\n");
		file.WriteString(strItem);
		for (int i = 0; i < theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecData[(int)CDataCalculate::TI_Misalignment].Length(); i++)
		{
			strItem.Format(
				"反面第%d条错位,,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n"
				, i + 1, theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideMeanTotalValue[(int)CDataCalculate::TI_Misalignment][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideSigmaTotalValue[(int)CDataCalculate::TI_Misalignment][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideCaTotalValue[(int)CDataCalculate::TI_Misalignment][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideCpTotalValue[(int)CDataCalculate::TI_Misalignment][i].T().D(),
				theGlobal.m_DataCalculate.GetLastMeasureQuota(0).hvecSideCpkTotalValue[(int)CDataCalculate::TI_Misalignment][i].T().D()
			);
			file.WriteString(strItem);
		}

#pragma endregion
		strItem = (
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"瑕疵判定, , 详情见瑕疵报表\n"
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			);
		file.WriteString(strItem);

		double dDefectRate = 0;
		if (m_tLastCount.dTotalMeterSelect != 0) dDefectRate = m_tLastCount.tDefectNGCounts.dNGMeter / m_tLastCount.dTotalMeterSelect * 100;
		strItem.Format("本卷膜片一共有%d处异常\n"
			", 数量, 占比\n"
			", %0.3fm, %0.2f%%\n\n"
			"== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			, m_tLastCount.tDefectNGCounts.nNGCount, m_tLastCount.tDefectNGCounts.dNGMeter, dDefectRate
		);
		file.WriteString(strItem);

		strItem.Format("以下涂布长度处出现停机位：\n\n,,停机开始时间,停机结束时间,停机开始位置,停机结束位置,停机原因,\n");
		double dStopMeterCount = 0;
		for (int i = 0; i < m_tLastCount.vecStopInfo.size(); i++)
		{
			strTmp.Format(",,%s,%s,%0.3fm,%0.3fm,%s\n", m_tLastCount.vecStopInfo[i].strTimeStart, m_tLastCount.vecStopInfo[i].strTimeEnd,
				m_tLastCount.vecStopInfo[i].dMeterPosStart, m_tLastCount.vecStopInfo[i].dMeterPosEnd, m_tLastCount.vecStopInfo[i].strDiscribe);
			strItem += strTmp;
			dStopMeterCount += (m_tLastCount.vecStopInfo[i].dMeterPosEnd - m_tLastCount.vecStopInfo[i].dMeterPosStart);
		}
		strItem += "\n,,停机米数,空带率,\n";
		strTmp.Format(",,%0.3f,%0.2f%%,\n", dStopMeterCount, dStopMeterCount == 0 ? 0 : dStopMeterCount / m_tLastCount.dTotalMeter * 100);
		strItem += strTmp;
		file.WriteString(strItem);

		strItem.Format("== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==\n"
			"★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★\n");
		file.WriteString(strItem);
		//关闭文件流
		file.Close();
	}
	catch (HException)
	{

	}
	WriteParam();
	return TRUE;
}
//数据滤波
BOOL CCounter::GetCountDataFilteringEnble(C_MergeWork& tOutput)
{
	try
	{
		tagPDInfo tInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
		for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
		{
			if (nSideSN == 1 && tInfo.nCoatingType == OneSide) continue;;
			if (tInfo.tParam.bUseCoatWidth)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tCoat.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecCoatWidth.size()) continue;
					if (tOutput.vecB[nSideSN].tMeasure.tCoat.hv_Width[j] >(tInfo.tParam.vecCoatWidth[0].dStandard + 3)
						|| tOutput.vecB[nSideSN].tMeasure.tCoat.hv_Width[j] < (tInfo.tParam.vecCoatWidth[0].dStandard - 3))
					{
						return FALSE;
					}
				}
			}
			if (tInfo.tParam.bUseFoilWidth)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecFoilWidth.size()) continue;
					tagTolerance tFoilWidth = tInfo.tParam.vecFoilWidth[j];
					if (tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width[j] >(tInfo.tParam.vecFoilWidth[0].dStandard + 3)
						|| tOutput.vecB[nSideSN].tMeasure.tFoil.hv_Width[j] < (tInfo.tParam.vecFoilWidth[0].dStandard - 3))
					{
						return FALSE;
					}
				}
			}
			if (tInfo.bExistAT11 && tInfo.tParam.bUseAT11Width)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tAT11.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecAT11Width.size()) continue;
					if (tOutput.vecB[nSideSN].tMeasure.tAT11.hv_Width[j] >(tInfo.tParam.vecAT11Width[0].dStandard + 3)
						|| tOutput.vecB[nSideSN].tMeasure.tAT11.hv_Width[j] < (tInfo.tParam.vecAT11Width[0].dStandard - 3))
					{
						return FALSE;
					}
				}
			}
			if (tInfo.bExistBaseCoat && tInfo.tParam.bUseBaseCoatWidth)
			{
				for (int j = 0; j < tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_Width.Length(); j++)
				{
					if (j >= tInfo.tParam.vecBaseCoatWidth.size()) continue;
					if (tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_Width[j] >(tInfo.tParam.vecBaseCoatWidth[0].dStandard + 3)
						|| tOutput.vecB[nSideSN].tMeasure.tBaseCoat.hv_Width[j] <(tInfo.tParam.vecBaseCoatWidth[0].dStandard - 3))
					{
						return FALSE;
					}
				}
			}
			//A/B面错位
			if (tInfo.nCoatingType == TwoSide && tOutput.vecB.size() == 2 && tInfo.tParam.bUseMisalignment &&
				tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE &&  tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE)
			{
				for (int j = 0; j < tOutput.tMerge.hv_Misalignment.Length(); j++)
				{
					if (tOutput.tMerge.hv_Misalignment[j] >(tInfo.tParam.vecMisalignment[0].dStandard + 3)
						|| tOutput.tMerge.hv_Misalignment[j] <(tInfo.tParam.vecMisalignment[0].dStandard - 3))
					{
						return FALSE;
					}
				}
			}
		}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("CCounter_GetCountDataFilteringEnble: 数据滤波执行异常 %s(%s)", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
		return FALSE;
	}
	return TRUE;
}

//出货判定
BOOL CCounter::GetShipment()
{
	try
	{
		tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
			//涂布尺寸NG出货判定
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecCoat[0].size(); i++)
			{
				if (i + 1 > m_tLastCount.tSizeNGCounts.vecCoat[0].size()) continue;
				if (m_tLastCount.tSizeNGCounts.vecCoat[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >1.25)//OK的前提下，NG率不刷新，因此需要再次计算一次
				{
					return FALSE;
				}
			}

			if (tPdInfo.nCoatingType == TwoSide && m_tLastCount.tSizeNGCounts.vecCoat[1].size()>0)
			{
				for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecCoat[1].size(); i++)
				{
					if (i + 1 > m_tLastCount.tSizeNGCounts.vecCoat[1].size()) continue;
					if (m_tLastCount.tSizeNGCounts.vecCoat[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >1.25)
					{
						return FALSE;
					}
				}
			}

			//极耳尺寸NG出货判定
			for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecFoil[0].size(); i++)
			{
				if (i + 1 > m_tLastCount.tSizeNGCounts.vecFoil[0].size()) continue;
				if (m_tLastCount.tSizeNGCounts.vecFoil[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >1.25)
				{
					return FALSE;
				}
			}
			if (tPdInfo.nCoatingType == TwoSide && m_tLastCount.tSizeNGCounts.vecFoil[1].size()>0)
			{
				for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecFoil[1].size(); i++)
				{
					if (i + 1 > m_tLastCount.tSizeNGCounts.vecFoil[1].size()) continue;
					if (m_tLastCount.tSizeNGCounts.vecFoil[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >1.25)
					{
						return FALSE;
					}
				}
			}
			if (tPdInfo.bExistAT11)
			{
				//AT11尺寸NG出货判定
				for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].size(); i++)
				{
					if (i + 1 > m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].size()) continue;
					if (m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >3)
					{
						return FALSE;
					}
				}
				if (tPdInfo.nCoatingType == TwoSide && m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size()>0)
				{
					for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size(); i++)
					{
						if (i + 1 > m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size()) continue;
						if (m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >3)
						{
							return FALSE;
						}
					}
				}
			}
			else if (tPdInfo.bExistBaseCoat)
			{
				//凹版尺寸NG出货判定
				for (int i = 0; i <m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].size(); i++)
				{
					if (i + 1 > m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0].size()) continue;
					if (m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[0][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >3)
					{
						return FALSE;
					}
				}
				if (tPdInfo.nCoatingType == TwoSide && m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size()> 0)
				{
					for (int i = 0; i <m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size(); i++)
					{
						if (i + 1 > m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1].size()) continue;
						if (m_tLastCount.tSizeNGCounts.vecAT11BaseCoat[1][i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >3)
						{
							return FALSE;
						}
					}
				}
			}
			//错位NG出货判定
			if (tPdInfo.nCoatingType == TwoSide && m_tLastCount.tSizeNGCounts.vecMisalignment.size()>0)
			{
				for (int i = 0; i < m_tLastCount.tSizeNGCounts.vecMisalignment.size(); i++)
				{
					if (i + 1 > m_tLastCount.tSizeNGCounts.vecMisalignment.size()) continue;
					if (m_tLastCount.tSizeNGCounts.vecMisalignment[i].dMeter / m_tLastCount.dTotalMeterSelect * 100 >1.25)
					{
						return FALSE;
					}
				}
			}
	}
	catch (HException& e)
	{
		CString strExcp;
		strExcp.Format("CCounter_GetShipment: 异常%s(%s)", e.ProcName().Text(), e.ErrorMessage().Text());
		theGlobal.m_Log.Exception(strExcp);
		return FALSE;
	}
	return TRUE;
}