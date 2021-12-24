#include "StdAfx.h"
#include "Project.h"

CProject::CProject()
	:m_hvcCamSNToSideSN(1),
	m_hvcSideSNToCamSN(1),
	m_hvcSideSNToMergeSN(1),
	m_hvcMergeSNToSideSN(1)
{
	m_hvcCamSNToSideSN.Clear();
	m_hvcSideSNToCamSN.Clear();
	m_hvcSideSNToMergeSN.Clear();
	m_hvcMergeSNToSideSN.Clear();
}

CProject::~CProject()
{
}

BOOL CProject::Init()
{
	ReadParam();

	//��ȡ��������������ŵĹ���
	for (int nCamSN = 0; nCamSN < m_tProjectConfig.nCamCount; nCamSN++)
	{
		m_hvcCamSNToSideSN[nCamSN].T().Clear();
		for (int nSideIndex = 0; nSideIndex < m_tProjectConfig.nSideCount; nSideIndex++)
		{
			for (int nSideCamIndex = 0; nSideCamIndex < m_tProjectConfig.vecSideConfig[nSideIndex].vecSideCamSN.size(); nSideCamIndex++)
			{
				if (nCamSN == m_tProjectConfig.vecSideConfig[nSideIndex].vecSideCamSN[nSideCamIndex])
				{
					m_hvcCamSNToSideSN[nCamSN].T().Append(nSideIndex);
					break;
				}
			}
		}
	}

	//��ȡ���������������š��ϲ������ŵĹ���
	for (int nSideIndex = 0; nSideIndex < m_tProjectConfig.nSideCount; nSideIndex++)
	{
		m_hvcSideSNToCamSN[nSideIndex].T().Clear();
		m_hvcSideSNToMergeSN[nSideIndex].T().Clear();
		for (int nSideCamIndex = 0; nSideCamIndex < m_tProjectConfig.vecSideConfig[nSideIndex].vecSideCamSN.size(); nSideCamIndex++)
		{
			m_hvcSideSNToCamSN[nSideIndex].T().Append(m_tProjectConfig.vecSideConfig[nSideIndex].vecSideCamSN[nSideCamIndex]);
		}

		for (int nSideMergeIndex = 0; nSideMergeIndex < m_tProjectConfig.vecSideConfig[nSideIndex].vecSideMergeSN.size(); nSideMergeIndex++)
		{
			m_hvcSideSNToMergeSN[nSideIndex].T().Append(m_tProjectConfig.vecSideConfig[nSideIndex].vecSideMergeSN[nSideMergeIndex]);
		}
	}

	//��ȡ�ϲ��������������š��������ŵĹ���
	for (int nMergeIndex = 0; nMergeIndex < m_tProjectConfig.nMergeCount; nMergeIndex++)
	{
		m_hvcMergeSNToSideSN[nMergeIndex].T().Clear();
		for (int nSideIndex = 0; nSideIndex < m_tProjectConfig.nSideCount; nSideIndex++)
		{
			for (int nSideMergeIndex = 0; nSideMergeIndex < m_tProjectConfig.vecSideConfig[nSideIndex].vecSideMergeSN.size(); nSideMergeIndex++)
			{
				HTuple hvInc = (m_hvcMergeSNToSideSN[nMergeIndex].T()).TupleFind(nSideIndex);
				if (nMergeIndex == m_tProjectConfig.vecSideConfig[nSideIndex].vecSideMergeSN[nSideMergeIndex]
					&& (hvInc.Length() == 0 || hvInc == -1))
				{
					m_hvcMergeSNToSideSN[nMergeIndex].T().Append(nSideIndex);
				}
			}
		}
	}

	return TRUE;
}

BOOL CProject::ReadParam()
{
	//��Ŀ���ò���
	m_tProjectConfig.nCamCount = GetIniInt("Project", "GrabberCount", 4);
	m_tProjectConfig.nSideCount = GetIniInt("Project", "SideCount", 2);
	m_tProjectConfig.nMergeCount = GetIniInt("Project", "MergeCount", 2);

	m_tProjectConfig.vecSideConfig.clear();
	for (int nSideIndex = 0; nSideIndex < m_tProjectConfig.nSideCount; nSideIndex++)
	{
		CString strSec;
		strSec.Format("Side%d", nSideIndex);

		tagSingleSideConfig tSide;
		tSide.nSideSN = nSideIndex;
		tSide.nSideCamCount = GetIniInt(strSec, "SideCamCount", 2);
		tSide.vecSideCamSN.clear();
		for (int nSideCamIndex = 0; nSideCamIndex < tSide.nSideCamCount; nSideCamIndex++)
		{
			CString strKey;
			strKey.Format("SideCam%dSN", nSideCamIndex);
			int nValue = GetIniInt(strSec, strKey, -1);
			if (-1 == nValue)
			{
				AfxMessageBox("������������������ô���");
				exit(0);
			}
			tSide.vecSideCamSN.push_back(nValue);
		}
		tSide.nSideMergeCount = GetIniInt(strSec, "SideMergeCount", 1);
		tSide.vecSideMergeSN.clear();
		for (int nSideMergeIndex = 0; nSideMergeIndex < tSide.nSideMergeCount; nSideMergeIndex++)
		{
			CString strKey;
			strKey.Format("SideMerge%dSN", nSideMergeIndex);
			int nValue = GetIniInt(strSec, strKey, -1);
			if (-1 == nValue)
			{
				AfxMessageBox("���������ϲ����������ô���");
				exit(0);
			}
			tSide.vecSideMergeSN.push_back(nValue);
		}
		m_tProjectConfig.vecSideConfig.push_back(tSide);
	}

	return TRUE;
}

BOOL CProject::WriteParam()
{
	return TRUE;
}

HTuple CProject::GetCamToSideSN(int nCamSN)
{
	if (nCamSN < 0) return HTuple();
	if (nCamSN > m_hvcCamSNToSideSN.Length() - 1) return HTuple();

	return m_hvcCamSNToSideSN[nCamSN].T();
}

HTuple CProject::GetSideToCamSNList(int nSideSN)
{
	if (nSideSN < 0) return HTuple();
	if (nSideSN > m_hvcCamSNToSideSN.Length() - 1) return HTuple();

	return m_hvcSideSNToCamSN[nSideSN].T();
}

HTuple CProject::GetSideToMergeSNList(int nSideSN)
{
	if (nSideSN < 0) return HTuple();
	if (nSideSN > m_hvcSideSNToMergeSN.Length() - 1) return HTuple();

	return m_hvcSideSNToMergeSN[nSideSN].T();
}

HTuple CProject::GetMergeToSideSNList(int nMergeSN)
{
	if (nMergeSN < 0) return HTuple();
	if (nMergeSN > m_hvcMergeSNToSideSN.Length() - 1) return HTuple();

	return m_hvcMergeSNToSideSN[nMergeSN].T();
}