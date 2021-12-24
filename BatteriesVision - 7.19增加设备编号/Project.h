//////////////////////////////////////////////////////////////////////////
//Project.h  ��Ŀ�����࣬������Ŀ����������
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "BaseTmpl.h"
#include <vector>
#include <map>
#include "stdafx.h"
class CProject : public CBaseTmpl
{
public:
	typedef std::vector<int> IntList;
	struct tagSingleSideConfig//��ⵥ�������
	{
		int			nSideSN;			//�������
		int			nSideCamCount;		//�������õ��������
		IntList		vecSideCamSN;		//ʹ����������
		int			nSideMergeCount;	//�������õ��������
		IntList		vecSideMergeSN;		//
	};

	typedef std::vector<tagSingleSideConfig> SideConfigList;//������������б�

	struct tagProjectConfig//��Ŀ��������
	{
		int				nCamCount;		//һ�����ö��ٸ����
		int				nSideCount;		//һ�������ٸ���
		int				nMergeCount;	//һ��Ҫ�ϲ��������
		SideConfigList	vecSideConfig;	//���ÿ������ò���

		inline void Init()
		{
			nCamCount = 2;
			nSideCount = 2;
			nMergeCount = 1;
			vecSideConfig.clear();
		}
	};
	typedef std::map<int, IntList> ListMap;

public:
	CProject();
	~CProject();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();
	
	//��ȡ��Ŀ�������ò���
	const tagProjectConfig & GetProjectConfig() const { return m_tProjectConfig; }

	//��ȡ�������������ļ�������
	HTuple GetCamToSideSN(int nCamSN);

	//��ȡ����������������������
	HTuple GetSideToCamSNList(int nSideSN);
	//��ȡ���������������ĺϲ�������
	HTuple GetSideToMergeSNList(int nSideSN);

	//��ȡ�ϲ��������������ļ�������
	HTuple GetMergeToSideSNList(int nMergeSN);

private:
	tagProjectConfig m_tProjectConfig;

	HTupleVector m_hvcCamSNToSideSN;				//�����Ź����ļ�������

	HTupleVector m_hvcSideSNToCamSN;				//�������Ź�����������
	HTupleVector m_hvcSideSNToMergeSN;				//�������Ź����ĺϲ�������

	HTupleVector m_hvcMergeSNToSideSN;				//�ϲ������Ź����ļ�������

};
