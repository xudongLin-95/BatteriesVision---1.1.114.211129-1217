//////////////////////////////////////////////////////////////////////////
//Project.h  项目方案类，管理项目方案的配置
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
	struct tagSingleSideConfig//检测单面的配置
	{
		int			nSideSN;			//单面序号
		int			nSideCamCount;		//单面配置的相机数量
		IntList		vecSideCamSN;		//使用相机的序号
		int			nSideMergeCount;	//单面配置的相机数量
		IntList		vecSideMergeSN;		//
	};

	typedef std::vector<tagSingleSideConfig> SideConfigList;//检测多面的配置列表

	struct tagProjectConfig//项目方案配置
	{
		int				nCamCount;		//一共配置多少个相机
		int				nSideCount;		//一共检测多少个面
		int				nMergeCount;	//一共要合并几个结果
		SideConfigList	vecSideConfig;	//检测每面的配置参数

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
	
	//获取项目方案配置参数
	const tagProjectConfig & GetProjectConfig() const { return m_tProjectConfig; }

	//获取相机序号所关联的检测面序号
	HTuple GetCamToSideSN(int nCamSN);

	//获取检测面序号所关联的相机序号
	HTuple GetSideToCamSNList(int nSideSN);
	//获取检测面序号所关联的合并结果序号
	HTuple GetSideToMergeSNList(int nSideSN);

	//获取合并结果序号所关联的检测面序号
	HTuple GetMergeToSideSNList(int nMergeSN);

private:
	tagProjectConfig m_tProjectConfig;

	HTupleVector m_hvcCamSNToSideSN;				//相机序号关联的检测面序号

	HTupleVector m_hvcSideSNToCamSN;				//检测面序号关联的相机序号
	HTupleVector m_hvcSideSNToMergeSN;				//检测面序号关联的合并结果序号

	HTupleVector m_hvcMergeSNToSideSN;				//合并结果序号关联的检测面序号

};
