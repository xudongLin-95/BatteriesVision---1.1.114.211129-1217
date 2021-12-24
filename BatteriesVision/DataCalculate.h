//////////////////////////////////////////////////////////////////////////
///DataCalculate.h 用于Cpk、sigma、均值等数据计算
//////////////////////////////////////////////////////////////////////////
#pragma once

class CDataCalculate : public CBaseTmpl
{
public:
	struct tagCalculateParam
	{
		BOOL	bUse;						//是否使用
		double	dSelectMeter;				//取值的实际长度
		double  dSemiAutoMeter;				//半自动补偿的数据长度
		BOOL	bUseStopPosDataFiltering;	//使用停机位数据过滤
	};
	//测量指标结构体
	struct tagWetAndDry
	{
		tagWetAndDry()
			:Wet(1),
			Dry(1)
		{}
		HTupleVector Wet;//湿膜
		HTupleVector Dry;//干膜
	};
	enum eTypeIndex
	{
		TI_Foil = 0,			//极耳
		TI_AT11BaseCoat,		//AT11/凹版
		TI_Coat,				//涂布
		TI_Misalignment,		//错位
	};
	//测量指标结构体
	struct tagData
	{
		tagData()
			:hvecData(2),
			hvecSideMeanValue(2),//Halcon定义二维_[测宽项目][第几条]_[第几条]仍然是一个数组,代表每一帧图像的测宽数据
			hvecSideCaValue(2),
			hvecSideCpValue(2),
			hvecSideCpkValue(2),
			hvecSideSigmaValue(2),
			hvecSideMeanTotalValue(2),
			hvecSideCaTotalValue(2),
			hvecSideCpTotalValue(2),
			hvecSideCpkTotalValue(2),
			hvecSideSigmaTotalValue(2)
		{
		}
		~tagData()
		{
		}
#pragma region	下标0:极耳;下标1:AT11/凹版;下标2:涂布
		////Halcon定义二维_[测宽项目][第几条]_[第几条]仍然是一个数组,代表每一帧图像的测宽数据
		HTupleVector hvecData;				//单面数据，
		HTupleVector hvecSideMeanValue;		//平均值，
		HTupleVector hvecSideCaValue;		//Ca值，
		HTupleVector hvecSideCpValue;		//Cp值，
		HTupleVector hvecSideCpkValue;		//Cpk值，
		HTupleVector hvecSideSigmaValue;	//Sigma值，

		HTupleVector hvecSideMeanTotalValue;	//总平均值，
		HTupleVector hvecSideCaTotalValue;		//总Ca值，
		HTupleVector hvecSideCpTotalValue;		//总Cp值，
		HTupleVector hvecSideCpkTotalValue;		//总Cpk值，
		HTupleVector hvecSideSigmaTotalValue;	//总Sigma值，
#pragma endregion

	};
public:
	CDataCalculate();
	~CDataCalculate();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//数据计算
	BOOL	Calculate(const C_MergeWork& tOutput);
	void    AddDryAndWet(const C_MergeWork& tOutput);
	//根据检测类型计算测量指标
	void CalcMeasureQuotaByType(HTuple hv_Width, ToleranceList toleranceList, int typeIndex, tagData &tagDataList, int MAX_FRAMES);

	//获取上一卷A面数据
	HTupleVector	GetLastDataA() { return m_tLastData[0].hvecData; }
	//获取上一卷B面数据
	HTupleVector	GetLastDataB() { return m_tLastData[1].hvecData; }

	//获取上一卷单面Ca数组
	HTuple	GetLastCaValue(const int& nSideS = 0);
	//获取上一卷单面Cp数组
	HTuple	GetLastCpValue(const int& nSideSN = 0);
	//获取上一卷单面Cpk数组
	HTuple	GetLastCpkValue(const int& nSideSN = 0);
	//获取上一卷单面均值数组
	HTuple	GetLastMeanValue(const int& nSideSN = 0);
	//获取上一卷单面Sigma数组
	HTuple	GetLastSigmaValue(const int& nSideSN = 0);

	//是否使用数据计算
	BOOL	IsUse() { return m_tParam.bUse; }
	//设置是否使用数据计算
	void	SetUse(const BOOL& bUse) { m_tParam.bUse = bUse; }

	//完成当前卷的数据计算
	void	FinishCalculate();

	//获取当前卷测量指标
	tagData&  GetCurrentMeasureQuota(const int& nSideSN = 0) { return m_tData[nSideSN]; }

	//获取当前卷涂布Cpk
	HTuple    GetCurrentCoatCpk(const int& nSideSN, const int& nSN) const { return m_tData[nSideSN].hvecSideCpkValue[(int)CDataCalculate::TI_Coat][nSN].T().D();}
	//获取当前卷涂布Sigma
	HTuple	  GetCurrentCoatSig(const int& nSideSN, const int& nSN) { return m_tData[nSideSN].hvecSideSigmaValue[(int)CDataCalculate::TI_Coat][nSN].T(); }
	//获取当前卷极耳Cpk
	HTuple	  GetCurrentFoilCpk(const int& nSideSN, const int& nSN) { return m_tData[nSideSN].hvecSideCpkValue[(int)CDataCalculate::TI_Foil][nSN].T(); }
	//获取当前卷极耳Sigma
	HTuple	  GetCurrentFoilSig(const int& nSideSN, const int& nSN) { return m_tData[nSideSN].hvecSideSigmaValue[(int)CDataCalculate::TI_Foil][nSN].T(); }
	//获取当前卷极耳Cpk
	HTuple	  GetCurrentAT11BasecoatCpk(const int& nSideSN, const int& nSN) { return m_tData[nSideSN].hvecSideCpkValue[(int)CDataCalculate::TI_AT11BaseCoat][nSN].T(); }
	//获取当前卷极耳Sigma
	HTuple	  GetCurrentAT11BasecoatSig(const int& nSideSN, const int& nSN) { return m_tData[nSideSN].hvecSideSigmaValue[(int)CDataCalculate::TI_AT11BaseCoat][nSN].T(); }
	//获取当前卷错位Cpk
	HTuple	  GetCurrentMisalignmentCpk(const int& nSideSN, const int& nSN) { return m_tData[nSideSN].hvecSideCpkValue[(int)CDataCalculate::TI_Misalignment][nSN].T(); }
	//获取当前卷错位Sigma
	HTuple	  GetCurrentMisalignmentSig(const int& nSideSN, const int& nSN) { return m_tData[nSideSN].hvecSideSigmaValue[(int)CDataCalculate::TI_Misalignment][nSN].T(); }

	//获取上一卷测量指标
	tagData&  GetLastMeasureQuota(const int& nSideSN = 0) { return m_tLastData[nSideSN]; }

	//填充补偿值
	BOOL CalculateDataOffset(tagPDInfo& tPdInfo);

private:
	//初始化当前数据
	void	InitData();
	void	InitHTuple(eTypeIndex eType, ToleranceList toleranceList, tagData& tagDataList);
	//计算cpk数据
	void	_CalCpk(HTuple hvDatas, const double& dUpperLimit, const double& dLowerLimit, HTuple* hvMean, HTuple* hvSigma, HTuple* Ca, HTuple* Cp, HTuple* Cpk);
public:
	tagCalculateParam m_tParam;					//数据计算参数
	tagWetAndDry m_tWetAndDry[2];                //干湿膜的数据 0---A面；1---B面
private:
	tagData	m_tData[2];							//当前计算数据 0---A面；1---B面
	tagData m_tLastData[2];						//上一卷数据
	tagData	m_tDataWhole[2];					//当前计算数据 0---A面；1---B面
	tagData m_tLastDataWhole[2];				//上一卷数据
	CString m_strDataPath;						//数据存储路径
};
