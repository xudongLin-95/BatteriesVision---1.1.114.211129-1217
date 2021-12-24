//////////////////////////////////////////////////////////////////////////
///Defect.h 覴ü����
//////////////////////////////////////////////////////////////////////////
#pragma once

class CDefect
{
public:
	typedef struct tagDefectInput
	{
		HObject		hoImage;
		int			nMode;		//���ģʽ
		SingleSize	tCoat;		//Ϳ��
		SingleSize	tFoil;		//����,����
		SingleSize	tAT11;		//AT11,A11���Ϳ��ͬʱ����
		SingleSize	tBaseCoat;	//��Ϳ
	}DefectInput, *PDefectInput;

	typedef struct tagDefectOutput
	{
		HObject		hoDisp;             //�������������ͺͷ�����Ŷ�Ӧ
		HTuple		hv_Type;			//��������
		HTuple		hv_StripeSN;		//������ţ�ÿһ��С����һ�����
		Feature		tFeature;			//ȱ������������Ԫ�س������������ͺͷ�����Ŷ�Ӧ
	}DefectOutput, *PDefectOutput;

	struct PretreatParam//Ԥ����
	{
		double	dBorderWidth;		//�߽���mm
		double	dScaleFactor;		//ͼ������ϵ�� 
		double	dMidLightCof;		//������������ȡϵ��ȡֵ 0-1
		double	dMidDarkCof;		//���İ�������ȡϵ�� 0-1
		double	dBorderLightCof;	//������������ȡϵ��0-1
		double	dBorderDarkCof;		//���İ�������ȡϵ��0-1
		double  dZoomFactor;		//ͼ�����������ϵ��
	};

	struct LeakFoilParam//Ϳ��©������
	{
		BOOL	bUseDetect;		//����
		//��·1
		int		nThresh;
		double	dMinArea;
		//��·2
		int     nDynThresh;		//�Աȶ�
		double	dMinDiameter;	//��Сֱ��
		double	dMaxDiameter;	//���ֱ��
		double	dDeviation;		//ƫ���
	};

	struct AT11LeakFoilParam//AT11©��/�������
	{
		BOOL	bUseDetect;		//����
		int		nMinThresh;		//��С��ֵ
		double	dMinWidth;		//��С���
		double	dMinHeight;		//��С�߶�
	};

	struct BubbleParam//���ݲ���
	{
		BOOL   bUseDetect;		//����
		int    nDynThresh;		//�Աȶ�
		double dMinDiameter;	//��Сֱ��
		double dMinCircularity;	//��СԲ��
		int    nInOutGrayDiff;	//����ҶȲ�
	};

	struct LightLineParam//���߲���
	{
		BOOL   bUseDetect;				//����
		int    nDynThresh;				//�Աȶ�
		double dMinWidth;				//��С���
		double dMinHeight;				//��С�߶�
		double dMinLengthWidthRatio;	//��С�����
	};

	struct DarkLineParam//���߲���
	{
		BOOL   bUseDetect;				//����
		int    nDynThresh;				//�Աȶ�
		double dMinWidth;				//��С���
		double dMinHeight;				//��С�߶�
		double dMinLengthWidthRatio;	//��С�����
	};

	struct LightDotParam//�װ߲���
	{
		BOOL   bUseDetect;//����
		int    nMinThresh;//��ֵ����
		int    nMaxThresh;//��ֵ����
		double dMinWidth;//��С���
		double dMinHeight;//��С�߶�
		double dMinArea;//��С���
		double dMaxArea;//������
		int   nMinDeviation;//��С����
		int   nMinGrayRange;//��С�ҶȲ���
	};

	struct DarkDotParam//�ڰ߲���
	{
		BOOL   bUseDetect;//����
		int    nMaxThresh;//��ֵ����
		double dMinWidth;//��С���
		double dMinHeight;//��С�߶�
		double dMinArea;//��С���
		double dMaxArea;//������
	};

	struct DropParam//��̼����
	{
		BOOL   bUseDetect;//����
		int    nMaxThresh;//��ֵ����
		double dMinArea;//��С���

		int nLTMinThresh;//����������С��ֵ
		int nLTMaxThresh;//�������������ֵ
		double dLTMinArea;//����������С���

		/////AT9����ȱ��
		int nAT9MaxThresh;//��С�Ҷ�
		double dAT9DarkMinWidth;//AT9��С��ȱ�ݻҶ�
		double dAT9DarkMinArea;//��С���
	};

	struct FoilParam//����������
	{
		BOOL   bUseDetect;//����
		int    nMaxDarkThresh;//��ֵ����
		double dMinDarkWidth;//��С���
		double dMinDarkHeight;//��С�߶�
		double dMinDarkArea;//��С���
		int    nMaxDarkNum;//��С��Ŀ
		////��Ҫ�Ǽ�⼫���ܱߵĲ��ģ��ж��Ƿ����δ�жϵ�����
		int nLightThresh;//����ȡ��ֵ
		double dMinLightArea;//��С���
		double dFoilLightMinWidth;//��С���
	};

	struct TapeParam
	{
		BOOL   bUseDetect;//����
		int nDarkThresh;//����ȡ��ֵ
		double dDarkMinArea;//����С���

		int nLightThresh;//����ֵ
		double dLightMinArea;//�����
	};

	struct  SingleFeature//�Զ���ȱ����
	{
		CString		StrSingleFeatureName;
		double		dMin;
		double		dMax;
	};

	struct  SingleErrorGroup//�Զ���ȱ����
	{
		CString		strErrorName;		//������Ŀ����
		int			nSingleGroupNum;	//��ǰ��Ŀ���ж�����ɸѡ������
		std::vector<SingleFeature>Vec_tSingleGroupFeatures;
	};

	struct  FeatureSelect//�Զ���ȱ����
	{
		std::vector<SingleErrorGroup> Vec_FSelects;//�Զ���ȱ����������
		int nGroupNum;//�Զ���ȱ����Ŀ
	};

	typedef struct  tagTPARAMETERS
	{
		PretreatParam		tPretreatParam;
		LeakFoilParam		tLeakFoilParam;
		BubbleParam			tBubbleParam;
		LightLineParam		tLightLineParam;
		DarkLineParam		tDarkLineParam;
		LightDotParam		tLightDotParam;
		DarkDotParam		tDarkDotParam;
		TapeParam			tTapeParam;
		DropParam			tDropParam;
		FoilParam			tFoilParam;
		AT11LeakFoilParam	tAT11LeakFoilParam;
		FeatureSelect		tFeatureSelect;
	}PARAMETERS, *PPARAMETERS;
	enum eErrorRegionType
	{
		RT_Coat = 0,	//Ϳ������
		RT_Foil,		//��������
		RT_AT11,		//AT11����
		RT_BaseCoat,	//��Ϳ/��������
	};
public:
	CDefect();
	~CDefect();

	void Init(const MeasureInit& tInit);
	BOOL ReadParam();
	BOOL WriteParam();

	void Action(tagDefectInput& tIn, tagDefectOutput& tOut);

	//��ʼ���������
	void InitOutputValue();
	//Ԥ����
	void Preprocess();
	//���ò�Ʒ��Ϣ
	BOOL SetProductInfo(tagPDInfo tInfo) { m_tInit.tProductInfo = tInfo; return TRUE; }
	//���÷ֱ���
	void SetResolution(ResolutionList vecRes);

private:
	//��̬��ֵ�ּ�ȱ��
	BOOL GetDynRegion(HObject& ho_Image);

	//�Զ���ȱ��
	void DIYErrorOut();

	//�õ��ּ�ȱ��������Ͱ�����
	void GetChoiceRegions(HObject& hoLChoiceRegions, HObject& hoDChoiceRegions);

	//Ϳ��©�����
	void LeakFoilDetect();
	//����ȱ�ݼ��
	void FoilDetect();
	//���ݼ��
	void BubbleDetect();
	//�����ۼ��
	void LightLineDetect();
	//�����ۼ��
	void BlackLineDetect();
	//�װ߲���
	void LightDotDetect();
	//�ڰ߼��
	void BlackDotDetect();
	//��̼���
	void DropDetect();
	//�������
	BOOL TapeDetect();

	//��ɸѡ
	void SelectLine(HObject ho_AllLineRegion, HObject *ho_SelectLineRegion, HTuple hv_StandarWidth,
		HTuple hv_StandarLength, HTuple *hv_WidthValue, HTuple *hv_LengthValue);

	//���ȱ�����ݵ�������
	BOOL AddError(HObject ho_ErrorObject, e_ErrorType eErrorType, eErrorRegionType eType=RT_Coat);//eType�� 0��Ϳ�� 1�Ǽ��� 2��AT9 3�ǰ���

	//�жϵ�ǰ��û��ȱ��
	BOOL HasDefect();
public:
	HTuple			m_hvOverColumn;			//������������
	HTuple			m_hvErrorName;			//ȱ����
	HTuple			m_hvStrFeature;			//������

	HObject			m_hoAdaptedImage;		//�Ҷ�����Ӧ��ͼ
	HObject			m_hoZoomImage;			//�Ҷ�����Ӧ����ͼ
	HObject			m_hoOverRegion;			//��������
private:
	BOOL    m_bExit;
	double  m_dMaxDetectTime;//��ⳬʱ����
	CString m_strSystemVis;

	CString m_strParamFile;//覴ò���Ŀ¼
	HObject m_hoBoderRegion;//�߽�����
	HObject m_hoSegRegions;//Ϳ���ָ�����
	HObject m_hoCoatMid;//ȥ����֮���Ϳ������
	HObject m_hoZoomCoatMid;//ȥ����֮���Ϳ����������
	HObject m_hoFoilRegion;//��������
	HObject m_hoAT11BaseCoatRegion;//AT11/��������
	HTuple  m_hvPeakGrayMid;
	HObject m_hoLEnableChoiceRegions;
	HObject m_hoDEnableChoiceRegions;

	ResolutionList		m_vecResolution;

	HTuple	m_hvImageWidth, m_hvImageHeight;

	double m_dRowResolution, m_dColResolution;
public:
	PARAMETERS			m_tParameters;
private:
	MeasureInit			m_tInit;
	tagDefectInput*		m_ptIn;
	tagDefectOutput*	m_ptOut;
};