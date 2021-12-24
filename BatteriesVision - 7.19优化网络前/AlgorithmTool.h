#pragma once
#include "BaseTmpl.h"

class CAlgorithmTool : public CBaseTmpl
{
public:
	enum eAlgType
	{
		AlgMeasure=0,	//�����㷨
		AlgSurface,		//������
		AlgFindMark,	//Markƥ��
	};
	struct tagAlg
	{
		DHInterface* pAlg;
		eAlgType eType;
	};
	typedef std::vector<tagAlg> AlgList;
public:
	CAlgorithmTool();
	~CAlgorithmTool();

public:
	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	int	DoSomething(const int& nSideSN,eAlgType eType,void *message,void *wParam,void *lParam);
	int Detect(const int& nSideSN,eAlgType eType,void *wParam,void *lParam);
	int ShowParamDlg(const int& nSideSN,eAlgType eType,void* wParam=NULL);
	
	/*int GetAlgType(const int& nCamSN){
		if(nCamSN<0 || nCamSN>=m_vecAlg.size())
			return 0;
		return (int)m_vecAlg[nCamSN].eType;
	}*/
private:
	tagAlg InitAlg(const int& nSN,eAlgType eType);
private:
	int		m_nAlgNum;			//�㷨��������Ӧ���������
	AlgList m_vecAlgMeasure;
	AlgList m_vecAlgDetect;
	AlgList m_vecAlgFindMark;
};