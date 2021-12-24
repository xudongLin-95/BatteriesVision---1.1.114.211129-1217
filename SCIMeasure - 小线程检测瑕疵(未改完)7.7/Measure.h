//////////////////////////////////////////////////////////////////////////
///Measure.h ���ڲ���Ϳ�������ߵĳߴ�ͼ�����ȱ��
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "Product.in.h"
#include "LineMeasure.h"
#include "Calibration.h"
#include "Defect.h"
#include "SpotInspection.h"
#include "PolyFit.h"

class CMeasure : public DHInterface
{
public:
	enum eFoilSelect//�������㷽ʽ
	{
		FS_OnlyFoil = 0,	//������
		FS_FoilBaseCoat,	//����+����
	};
	enum eMisalignmentSelect//��λֵ���㷽ʽ
	{
		MS_WidthSum = 0,	//����ۼ�
		MS_LineDist,		//������֮��ľ���
	};
	enum eCoatSelect
	{
		CS_Nomal,			//��ͨ����
		CS_FullCoat,		//Ϳ�����ǰ���ʱ������Ϳ���ͼ����ı�
	};
	typedef struct tagPreprocParam
	{
		BOOL	bUseJudgeUncoat;	//�Ƿ��ж���ͿĤ
		int		nUncoatThresLight;	//��ͿĤ(�ղ���)��ֵ
		int		nUncoatThresDark;	//��ͿĤ(�հ���)��ֵ
		double	dUncoatAreaRate;	//��ͿĤ���ռ��
	};
	typedef struct tagMeasureAdvanceParam//�����߼�����
	{
		eFoilSelect			eFoilSelect;
		eMisalignmentSelect eMisalignmentSelect;
		eCoatSelect			eCoatSelect;
	};

	typedef struct tagTJiaoZhun//У׼
	{
		HTuple hvReal[2];//ʵ������ֵ
		HTuple hvPix[2];//����ֵ
		HTuple hvRes[2];//������
		
		int    nJysl;//У�������
		double dSingleWidth;//�������ƵĿ��
		double dSigma;
		int    nThres;
		HTuple hvJyDistance[2];//У��ľ���
	}JiaoZhun;

public:
	CMeasure(void);
	~CMeasure(void);

	virtual int DoSomething(void* message, void* wParam, void* lParam);

	CMeasure& operator= (const CMeasure& in);

	//��ʼ���㷨
	int		Init(MeasureInit &tIn);
	//���ִ�нӿ�
	int		Action(MeasureInput &tIn,PMeasureOutput ptOut);
	//�����㷨�Ի���
	int		DisplaySetDialog(MeasureInput & tIn);
	//������
	BOOL	ReadParam();
	//д����
	BOOL	WriteParam();

private:
	//��ʼ���������ֵ
	void	InitOutputValue();
	//����
	BOOL	Measure();
	//���
	void	Detect();
	//���
	void	SpotInspect();
	//�ж��Ƿ���Ϳ��
	BOOL	JudgeExistCoat();
	//�ж��Ƿ��м�Ъ����
	void	JudgeExistIntermittent();
	//��������ò���
	int		SetParam(const CString& strParamName, void* vParam);
	//���ݲ��������ɿ������
	BOOL	CombinMeasure();
	//���ò�Ʒ����
	BOOL	SetProductInfo(tagPDInfo tInfo);
	//GRRģʽ����
	BOOL	GRRTest(MeasureInput &tIn);
	//����������
	BOOL	CorrectMeasure();

	//���ɵ�һ��������
	HObject _GenSingleRegion(HTuple hvLine1Row1, HTuple hvLine1Col1, HTuple hvLine1Row2, HTuple hvLine1Col2,
		HTuple hvLine2Row1, HTuple hvLine2Col1, HTuple hvLine2Row2, HTuple hvLine2Col2);

	//�Ҳ����ϵı�ǵ㣬����GRRģʽ
	void _GetBlackMark(HObject ho_Image, HObject ho_ROI_0, HObject *ho_SelectedRegions);

	//�жϲ�������û���ҵ���
	BOOL IsLineFind(const int& nLineSN);

	//�����ߵļн�
	BOOL _GetLineAngle(int& nSN,HTuple& hvStartRow,HTuple& hvStartCol,HTuple& hvEndRow,HTuple& hvEndCol,HTuple* hvAngle);

	//�����ұߵ���������������������
	Line GetWorldCoordsFromFitLine();

	//����������֮��ľ���
	void _DistanceLL(HTuple hvStartX1,HTuple hvStartY1,HTuple hvEndX1,HTuple hvEndY1,
		HTuple hvStartX2,HTuple hvStartY2,HTuple hvEndX2,HTuple hvEndY2,HTuple *hvDist);

	//������ǰֵ�����׼ֵ����
	void _CorrectValue(HTuple hvVal, double dStd, double dTolerance, HTuple* hvValCorrect);

	//�����޸���־
	void SaveParamLog(CMeasure Old, CMeasure New);

	//������־
	void _OperateLog(const double& dOld, const double& dCurrent, const CString& strOperateName);
	void _OperateLog(int& nOld, int& nCurrent, const CString& strOperateName);
	void _OperateLog(const CString& strOld, const CString& strCurrent, const CString& strOperateName);
public:
	MeasureOutput*	m_ptOut;
	MeasureInput*	m_ptIn;
	MeasureInit		m_tInit;			//��ʼ��
	tagPreprocParam	m_tPreprocParam;	//Ԥ����
	tagMeasureAdvanceParam m_tMeasureAdvanceParam;	//�����߼�����
	CLineMeasure	m_LineMeasure;					//Ѱ�߲�����
	CCalibration	m_Calibration;					//�궨��
	CDefect         m_Defect;						//覴ü����
	CSpotInspection m_SpotInspection;				//�������
	CPolyFit		m_PolyFit;						//����ʽ�����
	JiaoZhun m_tJZ;//У׼
	void ReadJZ();
	void WriteJZ();
private:
	CRITICAL_SECTION m_csLock;
	CString			m_strSystemVis, m_strMyConfig, m_strCommondh, m_strParamFile;
	HTuple			m_hvImageWidth, m_hvImageHeight;
	HTuple			m_hvErrorLineIndex;

	std::vector<Line> m_vecLastLine;	//ǰ�����ҵ��������ı�
	int				m_nNoLineCount;		//����û�ҵ��ߵļ���
};

extern CMeasure* g_pAlgorithm;