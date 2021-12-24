#pragma once
#include  <vector>
#include "BaseTmpl.h"
#include "Product.in.h"
#include "stdafx.h"

class CProductTool : public CBaseTmpl
{
public:
	struct tagPDInfoFlag
	{
		BOOL                    bAllFlag;
		BOOL					bPDNameFlag;			//产品名
		BOOL					bNoteFlag;				//产品备注
		BOOL     				bExistAT11Flag;			//是否包含AT9/AT11，正极
		BOOL     				bExistBaseCoatFlag;		//是否包含底涂，负极
		BOOL     				bExistIntermittentFlag;	//是否包含间歇区域
		BOOL					bPDTypeFlag;			//产品类型
		BOOL					bCoatingTypeFlag;		//涂的类型，0单面，1双面
		BOOL					bMeasureLineNumFlag;	//测量线数量
		BOOL					bLabelParamFlag;		//贴标/报警判断参数
		BOOL				    bNameListFlag;			//显示名称
														//判定参数
		BOOL					bUseCoatWidth;			//是否使用涂布宽
		BOOL					bUseFoilWidth;			//是否使用箔材宽
		BOOL					bUseAT11Width;			//是否使用AT9/AT11宽检测
		BOOL					bUseBaseCoatWidth;		//是否使用底涂检测
		BOOL					bUseMisalignment;		//是否使用错位检测
		BOOL					bUseChecking;			//是否使用设备点检
		BOOL					bUseSurfaceDetect;		//是否使用表面缺陷检测

		void Init()
		{
			bAllFlag = FALSE;
			bPDNameFlag = FALSE;
			bNoteFlag = FALSE;
			bExistAT11Flag = FALSE;
			bExistBaseCoatFlag = FALSE;
			bExistIntermittentFlag = FALSE;
			bPDTypeFlag = FALSE;
			bCoatingTypeFlag = FALSE;
			bMeasureLineNumFlag = FALSE;
			bLabelParamFlag = FALSE;
			bNameListFlag = FALSE;
			//判定参数
			bUseCoatWidth = FALSE;			//是否使用涂布宽
			bUseFoilWidth = FALSE;			//是否使用箔材宽
			bUseAT11Width = FALSE;			//是否使用AT9/AT11宽检测
			bUseBaseCoatWidth = FALSE;		//是否使用底涂检测
			bUseMisalignment = FALSE;		//是否使用错位检测
			bUseChecking = FALSE;			//是否使用设备点检
											//bUseSurfaceDetect = FALSE;		//是否使用表面缺陷检测
		}
	};
public:
	CProductTool();
	~CProductTool();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();
	BOOL WriteProductParam(const int& nSN);

	void SetProductInfo(const int &nSN,const tagPDInfo& tInfo){
		if(nSN>=m_vecProductList.size()) return;
		for (int i=0;i<m_vecProductList.size();i++)
		{
			if (m_vecProductList[i].strPDName==tInfo.strPDName && i!=nSN)
				return;
		}
		CLockHelper lk(&m_csLock);
		m_vecProductList[nSN]=tInfo;
	}

	void SetCurrentProductInfo(const tagPDInfo& tInfo) {
		for (int i = 0; i < m_vecProductList.size(); i++)
		{
			if (m_vecProductList[i].strPDName == tInfo.strPDName)
				return;
		}

		CLockHelper lk(&m_csLock);
		m_vecProductList[m_nCurrentProductSN] = tInfo;
	}

	const ProductList & GetProductList() { /*CLockHelper lk(&m_csLock);*/ return m_vecProductList; }
	int			GetProductCount() { return m_nProductCount; }
	CString		GetCurrentProductName() { return m_strCurrentProductName; }
	const tagPDInfo& GetCurrentProductInfo() { /*CLockHelper lk(&m_csLock);*/ return m_vecProductList[m_nCurrentProductSN]; }
	int			GetCurrentProductSN() { return m_nCurrentProductSN; }

	//配方新建
	BOOL AddNew(const CString &strPDName, const CString &strNote = "");
	BOOL AddNew(const int &nSrcSN, const CString &strPDName, const CString &strNote = "");
	//配方重命名
	BOOL Rename(const int &nSN, const CString &strNewPDName, const CString &strNewNote = "");
	//配方备份
	BOOL BackUp(const CString &strPDName);
	BOOL BackUp(const int &nSN);
	//配方删除
	BOOL Delete(const CString &strPDName);
	BOOL Delete(const int &nSN);
	//配方加载
	BOOL Load(const CString &strPDName);
	BOOL Load(const int &nSN);
	//配方备注
	BOOL Note(const CString &strPDName, const CString &strNote);
	BOOL Note(const int &nSN, const CString &strNote);

	//查找产品名
	int  FindName(const CString &strPDName);
	//清空补偿值
	BOOL ClearOffset(tagPDInfo& tPdInfo);
	//计算在线补偿值
	BOOL UpdateOnlineOffset(HTuple hvOnlineOffset);
	//获取当前在线补偿值
	HTuple GetOnlineOffset();
	//判断配放参数是否更新
	BOOL ProductParamCorrected(tagPDInfo &tOldPdInfo, tagPDInfo &tNewPdInfo, BOOL bMutiCCDCheck = FALSE);
	//保存配方修改记录,多工位配方校验
	BOOL SaveProductParamModifiedRecord(CString sName, tagPDInfo &tPdInfo, BOOL bMutiCCDCheck=FALSE);
private:
	BOOL RenameFolder(LPCTSTR lpszFromPath,LPCTSTR lpszToPath);
	tagPDInfo GetPDInfoFromFile(CString strProductName);

public:
	double	m_dAutoCompenLimit;	//自动补偿限制

private:
	CString m_strCurrentProductFolderPath;
	CString m_strCurrentProductName;
	CString m_strCurrentProductParamPath;
	
	int		m_nProductCount;
	int		m_nCurrentProductSN;
	ProductList m_vecProductList;
	tagPDInfoFlag m_tPdInfoCorrectedFlag;//配方信息对比标志位
};