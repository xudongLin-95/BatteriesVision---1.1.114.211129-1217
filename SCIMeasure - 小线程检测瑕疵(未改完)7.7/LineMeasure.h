//////////////////////////////////////////////////////////////////////////
///LineMeasure.h �����߲���
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "SCIMeasure.in.h"

class CLineMeasure
{
public:
	enum eMeasureDirection
	{
		MD_Left2Right = 0,	//�������������
		MD_Right2Left,		//����������ҵ���
	};
	enum eMeasureTransition
	{
		MT_All = 0,			//all�����з���
		MT_Dark2Light,		//positive��������
		MT_Light2Dark,		//negative��������
	};
	enum eMeasureSelect
	{
		MS_All = 0,			//all�����е�
		MS_First,			//first����һ��
		MS_Last,			//last�����һ��
	};
	typedef struct tagLineMeasureInput//����
	{
		HObject hoImage;	//ͼ��
		int		nMode;		//���ģʽ
	}LineMeasureInput, *PLineMeasureInput;
	typedef struct tagLineMeasureOutput//���
	{
		HTuple			hv_LineNum;				//����������
		Line			tLine;					//���ز���������,��������������

		HTuple			hvErrorLineSN;			//��������������

		HObject			hoDisp;					//������ʾ�Ĳ�����
	}LineMeasureOutput, *PLineMeasureOutput;
	typedef struct tagGenLineInput//����
	{
		HObject			hoImage;				//ͼ��
		HTuple			hvBaseLineColumn;		//��׼������
	}GenLineInput, *PGenLineInput;
	typedef struct tagGenLineOutput//���
	{
		HObject			hoLine;					//��ʾ������
		Line			tLine;					//����������,��������������
		HObject			hoDisp;					//��ʾ
	}GenLineOutput, *PGenLineOutput;

	enum eLineType
	{
		LT_BgFoil = 0,			//�����ͼ�������
		LT_FoilAT11,			//������AT11����
		LT_AT11Coat,			//AT11��Ϳ������
		LT_CoatAT11,			//Ϳ����AT11����
		LT_AT11Foil,			//AT11�ͼ�������
		LT_FoilBg,				//�����ͱ�������
		LT_FoilCoat,			//������Ϳ������
		LT_CoatFoil,			//Ϳ���ͼ�������
		LT_FoilBaseCoat,		//�����͵�Ϳ����
		LT_BaseCoatFoil,		//��Ϳ�ͼ�������
		LT_BaseCoatCoat,		//��Ϳ��Ϳ������
		LT_CoatBaseCoat,		//Ϳ���͵�Ϳ����
	};
	typedef std::vector<eLineType> LineTypeList;	//����������
	typedef struct tagLineProp//������
	{
		HTuple			hv_CamSNList;			//�����������ţ�һ�������
		HTuple			hv_SNList;				//����ţ�һ�������
		LineTypeList	vecLineTypeList;		//�ߵ����ͣ��ο�eLineType
	}LineProp, *PtagLineProp;
	typedef struct tagLineParam
	{
		int		nDirection;		//�����߷���0 ������, 1 ���ҵ���
		int		nThr;			//��Ӧmeasure_pos�е�Threshold
		int		nTransition;	//��Ӧmeasure_pos�е�Transition:0-'all', 1-'positive', 2-'negative' 
		int		nSelect;		//��Ӧmeasure_pos�е�Select:0-'all', 1-'first', 2-'last'
		double	dSigma;			//��Ӧmeasure_pos�е�Sigma
		int		nMeasureNum;	//����ĸ���

		double	dWidth;			//��������Ŀ��
		double	dHeight;		//��������ĸ߶�
		
		double	dStartRow;		//��ʼ���Row����
		double	dStartCol;		//��ʼ���Col����
		double	dEndRow;		//��ֹ���Row����
		double	dEndCol;		//��ֹ���Col����
	}LineParam, *PLineParam;
	typedef std::vector<tagLineParam> LineParamList;	//�������б�
	typedef struct tagMeasureParam
	{
		LineParamList	vecLineParam;		//�����ߵ�����
		LineProp		tLineProp;			//�����ߵ�����
		double			dMaxLeanDiff;		//�����бƫ�����
	}MeasureParam, *PMeasureParam;
	
public:
	CLineMeasure(void);
    ~CLineMeasure(void);

	//��ʼ��
	void Init(MeasureInit &tIn);
	//������
	BOOL ReadParam();
	//д����
	BOOL WriteParam();

	//�����ӿ�
	BOOL Action(LineMeasureInput& tIn, LineMeasureOutput& tOut);
	//�����߲���
	BOOL FindSingleLine(int nSN, LineMeasureInput& tIn, LineMeasureOutput& tOut, BOOL bDisp = FALSE);
	//����ͼ���Զ����ɲ�������
	BOOL AutoGenMeasureLine(const GenLineInput& tIn, GenLineOutput& tOut);
	//���ݲ���������ɲ�������
	BOOL AutoGenMeasureLineWithStdVal(const GenLineInput& tIn, GenLineOutput& tOut);

	//��ȡ��������
	MeasureParam GetMeasureParam() { return m_tMeasureParam; }
	//���ò�Ʒ��Ϣ
	BOOL SetProductInfo(tagPDInfo tInfo) { m_tInit.tProductInfo = tInfo; return TRUE; }
	//���÷ֱ���
	void SetResolution(ResolutionList vecRes) { m_vecResolution = vecRes; }
private:
	//���ɲ����ߵ�Ĭ������
	void GenDefultLineProp();
	//��ʼ���������
	void InitOutputValue();
	//������в�����
	BOOL FitAllLines();
	//���������
	int  _FindLine(const HObject& Image, const int nSN, const tagLineParam& FitLinePara, HObject* Line,
		double* dStartY, double* dStartX, double* dEndY, double* dEndX, int nLineDisplay = 0);
	//��������Ϳ�����ĸ��ǵ�
	void GetLLInterSectionPoints(HTuple hvIndex,HTuple hv_CutTileImageWidth,HTuple hv_CutTileImageHeight);
	//�����ĸ��ǵ����ɼ�������Ϳ������
	void GenRegion(HObject* Region,HTuple hvIndex);
	//�ж��Ƿ�ֱ
    BOOL IsVertical (HObject Region);

	LineParam ReadLineParam(const int& nSN);
	void WriteLineParam(const int& nSN, const LineParam& tPara);

public:
	MeasureParam			m_tMeasureParam;	//��������

	HTuple					m_hvOverColumn;		//������������
private:
	MeasureInit				m_tInit;
	LineMeasureInput*		m_ptIn;
	LineMeasureOutput*		m_ptOut;
	

	HTuple m_hvErrorLineIndex;	//����ϵĴ�������

	ResolutionList	m_vecResolution;
	
	HTuple m_hvRowBegin;
	HTuple m_hvColBegin;
	HTuple m_hvRowEnd;
	HTuple m_hvColEnd;

	CString m_strMyConfig;
	CString m_strSystemVis;

	double	m_dMeasureLineMoveDiff;//�������ƶ�λ�ã�0���ҵ��ߺ����������������λ�ò��ƶ�
};

