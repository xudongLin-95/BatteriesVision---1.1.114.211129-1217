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
		BOOL					bPDNameFlag;			//��Ʒ��
		BOOL					bNoteFlag;				//��Ʒ��ע
		BOOL     				bExistAT11Flag;			//�Ƿ����AT9/AT11������
		BOOL     				bExistBaseCoatFlag;		//�Ƿ������Ϳ������
		BOOL     				bExistIntermittentFlag;	//�Ƿ������Ъ����
		BOOL					bPDTypeFlag;			//��Ʒ����
		BOOL					bCoatingTypeFlag;		//Ϳ�����ͣ�0���棬1˫��
		BOOL					bMeasureLineNumFlag;	//����������
		BOOL					bLabelParamFlag;		//����/�����жϲ���
		BOOL				    bNameListFlag;			//��ʾ����
														//�ж�����
		BOOL					bUseCoatWidth;			//�Ƿ�ʹ��Ϳ����
		BOOL					bUseFoilWidth;			//�Ƿ�ʹ�ò��Ŀ�
		BOOL					bUseAT11Width;			//�Ƿ�ʹ��AT9/AT11����
		BOOL					bUseBaseCoatWidth;		//�Ƿ�ʹ�õ�Ϳ���
		BOOL					bUseMisalignment;		//�Ƿ�ʹ�ô�λ���
		BOOL					bUseChecking;			//�Ƿ�ʹ���豸���
		BOOL					bUseSurfaceDetect;		//�Ƿ�ʹ�ñ���ȱ�ݼ��

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
			//�ж�����
			bUseCoatWidth = FALSE;			//�Ƿ�ʹ��Ϳ����
			bUseFoilWidth = FALSE;			//�Ƿ�ʹ�ò��Ŀ�
			bUseAT11Width = FALSE;			//�Ƿ�ʹ��AT9/AT11����
			bUseBaseCoatWidth = FALSE;		//�Ƿ�ʹ�õ�Ϳ���
			bUseMisalignment = FALSE;		//�Ƿ�ʹ�ô�λ���
			bUseChecking = FALSE;			//�Ƿ�ʹ���豸���
											//bUseSurfaceDetect = FALSE;		//�Ƿ�ʹ�ñ���ȱ�ݼ��
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

	//�䷽�½�
	BOOL AddNew(const CString &strPDName, const CString &strNote = "");
	BOOL AddNew(const int &nSrcSN, const CString &strPDName, const CString &strNote = "");
	//�䷽������
	BOOL Rename(const int &nSN, const CString &strNewPDName, const CString &strNewNote = "");
	//�䷽����
	BOOL BackUp(const CString &strPDName);
	BOOL BackUp(const int &nSN);
	//�䷽ɾ��
	BOOL Delete(const CString &strPDName);
	BOOL Delete(const int &nSN);
	//�䷽����
	BOOL Load(const CString &strPDName);
	BOOL Load(const int &nSN);
	//�䷽��ע
	BOOL Note(const CString &strPDName, const CString &strNote);
	BOOL Note(const int &nSN, const CString &strNote);

	//���Ҳ�Ʒ��
	int  FindName(const CString &strPDName);
	//��ղ���ֵ
	BOOL ClearOffset(tagPDInfo& tPdInfo);
	//�������߲���ֵ
	BOOL UpdateOnlineOffset(HTuple hvOnlineOffset);
	//��ȡ��ǰ���߲���ֵ
	HTuple GetOnlineOffset();
	//�ж���Ų����Ƿ����
	BOOL ProductParamCorrected(tagPDInfo &tOldPdInfo, tagPDInfo &tNewPdInfo, BOOL bMutiCCDCheck = FALSE);
	//�����䷽�޸ļ�¼,�๤λ�䷽У��
	BOOL SaveProductParamModifiedRecord(CString sName, tagPDInfo &tPdInfo, BOOL bMutiCCDCheck=FALSE);
private:
	BOOL RenameFolder(LPCTSTR lpszFromPath,LPCTSTR lpszToPath);
	tagPDInfo GetPDInfoFromFile(CString strProductName);

public:
	double	m_dAutoCompenLimit;	//�Զ���������

private:
	CString m_strCurrentProductFolderPath;
	CString m_strCurrentProductName;
	CString m_strCurrentProductParamPath;
	
	int		m_nProductCount;
	int		m_nCurrentProductSN;
	ProductList m_vecProductList;
	tagPDInfoFlag m_tPdInfoCorrectedFlag;//�䷽��Ϣ�Աȱ�־λ
};