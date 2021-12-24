//////////////////////////////////////////////////////////////////////////
///Defect.h 瑕疵检测类
//////////////////////////////////////////////////////////////////////////
#pragma once

class CDefect
{
public:
	typedef struct tagDefectInput
	{
		HObject		hoImage;
		int			nMode;		//检测模式
		SingleSize	tCoat;		//涂布
		SingleSize	tFoil;		//箔材,极耳
		SingleSize	tAT11;		//AT11,A11与底涂不同时存在
		SingleSize	tBaseCoat;	//底涂
	}DefectInput, *PDefectInput;

	typedef struct tagDefectOutput
	{
		HObject		hoDisp;             //区域，与区域类型和分条序号对应
		HTuple		hv_Type;			//区域类型
		HTuple		hv_StripeSN;		//分条序号，每一个小区域一个序号
		Feature		tFeature;			//缺陷区域特征，元素长度与区域类型和分条序号对应
	}DefectOutput, *PDefectOutput;

	struct PretreatParam//预处理
	{
		double	dBorderWidth;		//边界宽度mm
		double	dScaleFactor;		//图像拉伸系数 
		double	dMidLightCof;		//中心亮区域提取系数取值 0-1
		double	dMidDarkCof;		//中心暗区域提取系数 0-1
		double	dBorderLightCof;	//中心亮区域提取系数0-1
		double	dBorderDarkCof;		//中心暗区域提取系数0-1
		double  dZoomFactor;		//图像和区域缩放系数
	};

	struct LeakFoilParam//涂布漏箔参数
	{
		BOOL	bUseDetect;		//启用
		//回路1
		int		nThresh;
		double	dMinArea;
		//回路2
		int     nDynThresh;		//对比度
		double	dMinDiameter;	//最小直径
		double	dMaxDiameter;	//最大直径
		double	dDeviation;		//偏差波动
	};

	struct AT11LeakFoilParam//AT11漏箔/亮点参数
	{
		BOOL	bUseDetect;		//启用
		int		nMinThresh;		//最小阈值
		double	dMinWidth;		//最小宽度
		double	dMinHeight;		//最小高度
	};

	struct BubbleParam//气泡参数
	{
		BOOL   bUseDetect;		//启用
		int    nDynThresh;		//对比度
		double dMinDiameter;	//最小直径
		double dMinCircularity;	//最小圆度
		int    nInOutGrayDiff;	//内外灰度差
	};

	struct LightLineParam//白线参数
	{
		BOOL   bUseDetect;				//启用
		int    nDynThresh;				//对比度
		double dMinWidth;				//最小宽度
		double dMinHeight;				//最小高度
		double dMinLengthWidthRatio;	//最小长宽比
	};

	struct DarkLineParam//黑线参数
	{
		BOOL   bUseDetect;				//启用
		int    nDynThresh;				//对比度
		double dMinWidth;				//最小宽度
		double dMinHeight;				//最小高度
		double dMinLengthWidthRatio;	//最小长宽比
	};

	struct LightDotParam//白斑参数
	{
		BOOL   bUseDetect;//启用
		int    nMinThresh;//阈值下限
		int    nMaxThresh;//阈值上限
		double dMinWidth;//最小宽度
		double dMinHeight;//最小高度
		double dMinArea;//最小面积
		double dMaxArea;//最大面积
		int   nMinDeviation;//最小波动
		int   nMinGrayRange;//最小灰度波动
	};

	struct DarkDotParam//黑斑参数
	{
		BOOL   bUseDetect;//启用
		int    nMaxThresh;//阈值上限
		double dMinWidth;//最小宽度
		double dMinHeight;//最小高度
		double dMinArea;//最小面积
		double dMaxArea;//最大面积
	};

	struct DropParam//脱碳参数
	{
		BOOL   bUseDetect;//启用
		int    nMaxThresh;//阈值上限
		double dMinArea;//最小面积

		int nLTMinThresh;//亮胶带的最小阈值
		int nLTMaxThresh;//亮胶带的最大阈值
		double dLTMinArea;//亮胶带的最小面积

		/////AT9区域暗缺陷
		int nAT9MaxThresh;//最小灰度
		double dAT9DarkMinWidth;//AT9最小暗缺陷灰度
		double dAT9DarkMinArea;//最小面积
	};

	struct FoilParam//极耳检测参数
	{
		BOOL   bUseDetect;//启用
		int    nMaxDarkThresh;//阈值上限
		double dMinDarkWidth;//最小宽度
		double dMinDarkHeight;//最小高度
		double dMinDarkArea;//最小面积
		int    nMaxDarkNum;//最小数目
		////主要是检测极耳周边的箔材，判断是否存在未切断的现象
		int nLightThresh;//亮提取阈值
		double dMinLightArea;//最小面积
		double dFoilLightMinWidth;//最小宽度
	};

	struct TapeParam
	{
		BOOL   bUseDetect;//启用
		int nDarkThresh;//暗提取阈值
		double dDarkMinArea;//暗最小面积

		int nLightThresh;//亮阈值
		double dLightMinArea;//亮面积
	};

	struct  SingleFeature//自定义缺陷组
	{
		CString		StrSingleFeatureName;
		double		dMin;
		double		dMax;
	};

	struct  SingleErrorGroup//自定义缺陷组
	{
		CString		strErrorName;		//错误项目名称
		int			nSingleGroupNum;	//当前项目中有多少项筛选特征项
		std::vector<SingleFeature>Vec_tSingleGroupFeatures;
	};

	struct  FeatureSelect//自定义缺陷组
	{
		std::vector<SingleErrorGroup> Vec_FSelects;//自定义缺陷特征数组
		int nGroupNum;//自定义缺陷数目
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
		RT_Coat = 0,	//涂布区域
		RT_Foil,		//极耳区域
		RT_AT11,		//AT11区域
		RT_BaseCoat,	//底涂/凹版区域
	};
public:
	CDefect();
	~CDefect();

	void Init(const MeasureInit& tInit);
	BOOL ReadParam();
	BOOL WriteParam();

	void Action(tagDefectInput& tIn, tagDefectOutput& tOut);

	//初始化输出变量
	void InitOutputValue();
	//预处理
	void Preprocess();
	//设置产品信息
	BOOL SetProductInfo(tagPDInfo tInfo) { m_tInit.tProductInfo = tInfo; return TRUE; }
	//设置分辨率
	void SetResolution(ResolutionList vecRes);

private:
	//动态阈值粗检缺陷
	BOOL GetDynRegion(HObject& ho_Image);

	//自定义缺陷
	void DIYErrorOut();

	//得到粗检缺陷亮区域和暗区域
	void GetChoiceRegions(HObject& hoLChoiceRegions, HObject& hoDChoiceRegions);

	//涂布漏箔检测
	void LeakFoilDetect();
	//极耳缺陷检测
	void FoilDetect();
	//气泡检测
	void BubbleDetect();
	//白条痕检测
	void LightLineDetect();
	//暗条痕检测
	void BlackLineDetect();
	//白斑测试
	void LightDotDetect();
	//黑斑检测
	void BlackDotDetect();
	//脱碳检测
	void DropDetect();
	//胶带检测
	BOOL TapeDetect();

	//线筛选
	void SelectLine(HObject ho_AllLineRegion, HObject *ho_SelectLineRegion, HTuple hv_StandarWidth,
		HTuple hv_StandarLength, HTuple *hv_WidthValue, HTuple *hv_LengthValue);

	//添加缺陷数据到输出结果
	BOOL AddError(HObject ho_ErrorObject, e_ErrorType eErrorType, eErrorRegionType eType=RT_Coat);//eType： 0是涂布 1是极耳 2是AT9 3是凹版

	//判断当前有没有缺陷
	BOOL HasDefect();
public:
	HTuple			m_hvOverColumn;			//屏蔽区域坐标
	HTuple			m_hvErrorName;			//缺陷名
	HTuple			m_hvStrFeature;			//特征名

	HObject			m_hoAdaptedImage;		//灰度自适应的图
	HObject			m_hoZoomImage;			//灰度自适应缩放图
	HObject			m_hoOverRegion;			//屏蔽区域
private:
	BOOL    m_bExit;
	double  m_dMaxDetectTime;//检测超时控制
	CString m_strSystemVis;

	CString m_strParamFile;//瑕疵参数目录
	HObject m_hoBoderRegion;//边界区域
	HObject m_hoSegRegions;//涂布分割区域
	HObject m_hoCoatMid;//去掉边之后的涂布区域
	HObject m_hoZoomCoatMid;//去掉边之后的涂布缩放区域
	HObject m_hoFoilRegion;//极耳区域
	HObject m_hoAT11BaseCoatRegion;//AT11/凹版区域
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