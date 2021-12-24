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
	HObject ho_ModleInfo;//0代表图片 1 代表模板区域 2 代表搜索区域
	HObject ho_Line;//转换的线的位置关系
	HTuple  hv_ModelID;
	HTuple  hv_ModelParam;//名字，模板类型，膨胀参数,最小匹配率
}SINGLEMARKINFO,*PSINGLEMARKINFO;
typedef std::vector<SINGLEMARKINFO> MARKLIST;
#define  LINE_COUNT 1


class  CFindMark : public DHInterface
{
public:
	virtual int DoSomething(void* message,void* wParam,void* lParam);//函数入口

public:
	CFindMark(void);
	~CFindMark(void);
	CFindMark& operator= (const CFindMark& in);

	int Init(NS_SCIFindMark::INITPARAM& tInitParam);//初始化
	int Find(const HObject& ho_Image,NS_SCIFindMark::PMARKRESULT ptResult);//匹配
	BOOL SaveJigsawImage(const int& nGrabberSN,const HObject& ho_ImageSave);
	int ThresholdFind(HObject ho_Image,NS_SCIFindMark::PMARKRESULT ptResult);//匹配
	int ShowDialog(HObject ho_Image);//调对话框
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
	CLineToolPara m_sLineTool[LINE_COUNT];//分别为检测的四条边 分别为 上下左右 极耳
	CRITICAL_SECTION m_csFindChange;
};



