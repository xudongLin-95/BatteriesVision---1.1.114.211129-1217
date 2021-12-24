#pragma once
#include "stdafx.h"


class CriticalSectionHelper
{
public:
	CriticalSectionHelper(CRITICAL_SECTION *pcs)
	{
		m_pcs = pcs;
		EnterCriticalSection(m_pcs);
	}

	~CriticalSectionHelper()
	{
		LeaveCriticalSection(m_pcs);
		m_pcs = NULL;
	}

private:
	CRITICAL_SECTION *m_pcs;
};
typedef struct tagTSINGLEMARKINFO
{
	HObject ho_ModleInfo;//0����ͼƬ 1 ����ģ������ 2 ������������
	HObject ho_Line;//ת�����ߵ�λ�ù�ϵ
	HTuple  hv_ModelID;
	HTuple  hv_ModelParam;//���֣�ģ�����ͣ����Ͳ���,��Сƥ����
}SINGLEMARKINFO,*PSINGLEMARKINFO;
typedef std::vector<SINGLEMARKINFO> MARKLIST;
#define  LINE_COUNT 1


class  CFindMark : public DHInterface
{
public:
	virtual int DoSomething(void* message,void* wParam,void* lParam);//�������

public:
	CFindMark(void);
	~CFindMark(void);
	CFindMark& operator= (const CFindMark& in);

	int Init(NS_SCIFindMark::INITPARAM& tInitParam);//��ʼ��
	int Find(const HObject& ho_Image,NS_SCIFindMark::PMARKRESULT ptResult);//ƥ��
	BOOL SaveJigsawImage(const int& nGrabberSN,const HObject& ho_ImageSave);
	int ThresholdFind(HObject ho_Image,NS_SCIFindMark::PMARKRESULT ptResult);//ƥ��
	int ShowDialog(HObject ho_Image);//���Ի���
public:
	int FindMult(const HObject& ho_Image, NS_SCIFindMark::PMARKRESULT ptResult);
	void ReadMark();
	void WriteMark();
	void ClearMark();

	CString m_strConfigPath;
	CString m_strConfigFile;

	MARKLIST   m_tMarkList;
	CString m_strLastErrorMessage;

	NS_SCIFindMark::INITPARAM  m_tInitParam;
	int m_nType;

	int FitLine (const HObject& Image,const CLineToolPara& FitLinePara, HObject* Line, 
		double* dStartY,double* dStartX,double* dEndY,double* dEndX,int nLineDisplay = 0);
	CLineToolPara m_sLineTool[LINE_COUNT];//�ֱ�Ϊ���������� �ֱ�Ϊ �������� ����
	CRITICAL_SECTION m_csFindChange;
};



