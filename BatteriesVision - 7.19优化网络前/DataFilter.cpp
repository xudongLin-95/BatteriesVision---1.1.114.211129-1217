#include "StdAfx.h"
#include "DataFilter.h"

CString CDataFilter::s_strFilterName[] = {
	_T("内涂宽"),
	_T("外涂宽"),
	_T("内AT9"),
	_T("外AT9"),
};

CDataFilter::CDataFilter()
	:m_hvecData(1)
{
	for (int i=0;i<AT9Width1+1;i++)
		m_hvecData[i].T().Clear();
}

CDataFilter::~CDataFilter()
{

}

BOOL CDataFilter::Init()
{
	ReadParam();
	return TRUE;
}

BOOL CDataFilter::ReadParam()
{
	m_tFilterParam.hvFilterType.Clear();
	m_hvecData.Clear();
	m_tFilterParam.hvMarkSize.Clear();
	m_strProductParamPath.Format("%s//Config//Products//%s//ProductParam.ini", GetCurrentAppPath(), theGlobal.m_ProductTool.GetCurrentProductName());
	m_tFilterParam.bUse = GetPrivateProfileInt("DataFilter", "Use", FALSE, m_strProductParamPath);
	m_tFilterParam.nFilterNum = AT9Width1 + 1;
	for (int i = 0; i < m_tFilterParam.nFilterNum; i++)
	{
		CString strName;
		strName.Format("FilterType%d", i);
		m_tFilterParam.hvFilterType.Append((int)GetPrivateProfileInt("DataFilter", strName, None, m_strProductParamPath));
		strName.Format("MarkSize%d", i);
		m_tFilterParam.hvMarkSize.Append((int)GetPrivateProfileInt("DataFilter", strName, 5, m_strProductParamPath));
		strName.Format("Offset%d", i);
		m_tFilterParam.hvOffset.Append((int)GetPrivateProfileDouble("DataFilter", strName, 0.5, m_strProductParamPath));
	}
	return TRUE;
}

BOOL CDataFilter::WriteParam()
{
	WritePrivateProfileString("DataFilter", NULL, NULL, m_strProductParamPath);
	WritePrivateProfileInt("DataFilter", "Use", m_tFilterParam.bUse, m_strProductParamPath);
	WritePrivateProfileInt("DataFilter", "FilterNum", m_tFilterParam.nFilterNum, m_strProductParamPath);
	for (int i = 0; i < m_tFilterParam.nFilterNum; i++)
	{
		CString strName;
		strName.Format("FilterType%d", i);
		WritePrivateProfileInt("DataFilter", strName, m_tFilterParam.hvFilterType[i].I(), m_strProductParamPath);
		strName.Format("MarkSize%d", i);
		WritePrivateProfileInt("DataFilter", strName, m_tFilterParam.hvMarkSize[i].I(), m_strProductParamPath);
		strName.Format("Offset%d", i);
		WritePrivateProfileDouble("DataFilter", strName, m_tFilterParam.hvOffset[i].D(), m_strProductParamPath);
	}
	return TRUE;
}

BOOL CDataFilter::Filtering(C_MergeWork &tOutput)
{
	if (!IsUse()) return FALSE;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//for (int i=0;i<PRODUCT_COUNT;i++)
	//{
	//	SingleFiltering(tOutput.B[i].tMeasureOutput.tSize.hv_Width, i, &tOutput.B[i].tMeasureOutput.tSize.hv_Width);
	//	if (TRUE == tPdInfo.bExistAT9)
	//	{
	//		//SingleFiltering(tOutput.B[i].tMeasureOutput.tSide[i].hvAT9Width, i + 2, &tOutput.B[i].tMeasureOutput.tSide[i].hvAT9Width);
	//	}
	//}
	return TRUE;
}

BOOL CDataFilter::SingleFiltering(HTuple hvSrc, const int& nSN, HTuple* hvDst)
{
	//try
	//{
	//	double dStarand;
	//	
	//	(*hvDst) = hvSrc;
	//	if (m_tFilterParam.hvFilterType[nSN] == None) return TRUE;
	//	if (hvSrc.Length() != 1) return FALSE;
	//	if (nSN > m_tFilterParam.hvFilterType.Length() - 1) return FALSE;
	//	if (nSN > m_tFilterParam.hvOffset.Length() - 1) return FALSE;
	//	if (nSN > m_tFilterParam.hvMarkSize.Length() - 1) return FALSE;

	//	m_hvecData[nSN].T().Append(hvSrc);
	//	if (m_hvecData[nSN].T().Length()+1 < m_tFilterParam.hvMarkSize[nSN].I()//MarkSize 代表使用多少个数据进行滤波
	//		|| (m_tFilterParam.hvFilterType[nSN] == MedianMean && m_hvecData[nSN].T().Length() < 3))
	//		return TRUE;

	//	if (abs(hvSrc.D() - dStarand) > m_tFilterParam.hvOffset[nSN])//超出上下限的时候不滤波
	//	{
	//		m_hvecData[nSN].Remove(m_hvecData[nSN].Length() - 1);
	//		return TRUE;
	//	}
	//		
	//	HTuple hvSingleFiltered = hvSrc;
	//	if (m_tFilterParam.hvFilterType[nSN] == Mean)
	//	{
	//		//求均值
	//		TupleMean(m_hvecData[nSN].T(), &hvSingleFiltered);
	//	}
	//	else if (m_tFilterParam.hvFilterType[nSN] == Median)
	//	{
	//		//求中值
	//		TupleMedian(m_hvecData[nSN].T(), &hvSingleFiltered);
	//	}
	//	else if (m_tFilterParam.hvFilterType[nSN] == MedianMean)
	//	{
	//		//去掉最大值，去掉最小值，剩下的求均值
	//		HTuple hvSorted, hvSelected;
	//		TupleSort(m_hvecData[nSN].T(), &hvSorted);
	//		TupleSelectRange(hvSorted, 1, hvSorted.Length() - 2, &hvSelected);
	//		TupleMean(hvSelected, &hvSingleFiltered);
	//	}

	//	if (m_hvecData[nSN].T().Length() >= m_tFilterParam.hvMarkSize[nSN].I())
	//		TupleRemove(m_hvecData[nSN].T(), 0, &(m_hvecData[nSN].T()));

	//	(*hvDst) = hvSingleFiltered;
	//}
	//catch (HException& e)
	//{
	//	return FALSE;
	//}

	return TRUE;
}

BOOL CDataFilter::MultiFiltering(HTuple hvSrcs, const int& nSN, HTuple* hvDsts)
{
	try
	{
		double dStarand;
		
		(*hvDsts) = hvSrcs;
		if (m_tFilterParam.hvFilterType[nSN] == None) return TRUE;
		if (hvSrcs.Length() <= 0) return FALSE;
		if (nSN > m_tFilterParam.hvFilterType.Length() - 1) return FALSE;
		if (nSN > m_tFilterParam.hvOffset.Length() - 1) return FALSE;
		if (nSN > m_tFilterParam.hvMarkSize.Length() - 1) return FALSE;

		for (int i = 0; i < hvSrcs.Length(); i++)
		{
			m_hvecData[nSN].T().Append(hvSrcs[i]);
			if (m_hvecData[nSN].T().Length() < m_tFilterParam.hvMarkSize[nSN].I()
				|| (m_tFilterParam.hvFilterType[nSN] == MedianMean && m_hvecData[nSN].T().Length() < 3))
				return TRUE;

			if (abs(hvSrcs[i].D()- dStarand) > m_tFilterParam.hvOffset[nSN])
				return TRUE;

			HTuple hvSingleFiltered = (*hvDsts)[i];

			if (m_tFilterParam.hvFilterType[nSN] == Mean)
			{
				//求均值
				TupleMean(m_hvecData[nSN].T(), &hvSingleFiltered);
			}
			else if (m_tFilterParam.hvFilterType[nSN] == Median)
			{
				//求中值
				TupleMedian(m_hvecData[nSN].T(), &hvSingleFiltered);
			}
			else if (m_tFilterParam.hvFilterType[nSN] == MedianMean)
			{
				//去掉最大值，去掉最小值，剩下的求均值
				HTuple hvSorted, hvSelected;
				TupleSort(m_hvecData[nSN].T(), &hvSorted);
				TupleSelectRange(hvSorted, 1, hvSorted.Length() - 2, &hvSelected);
				TupleMean(hvSelected, &hvSingleFiltered);
			}

			if (m_hvecData[nSN].T().Length() >= m_tFilterParam.hvMarkSize[nSN].I())
				TupleRemove(m_hvecData[nSN].T(), 0, &(m_hvecData[nSN].T()));
			(*hvDsts)[i] = hvSingleFiltered;
		}
	}
	catch (HException& e)
	{
		return FALSE;
	}

	return TRUE;
}