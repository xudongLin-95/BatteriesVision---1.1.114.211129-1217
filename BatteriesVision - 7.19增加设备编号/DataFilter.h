//////////////////////////////////////////////////////////////////////////
///DataFilter.h 用于数据滤波和处理
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CDataFilter : public CBaseTmpl
{
public:
	enum eFilterType
	{
		None=0,		//不使用滤波
		Mean,		//均值滤波
		Median,		//中值滤波
		MedianMean,	//中值平均滤波
	};
	enum eFilterSN
	{
		CoatWidth0 = 0,		//涂布宽
		CoatWidth1,
		AT9Width0,			//左AT9宽
		AT9Width1,			//右AT9宽
	};
	struct tagTFilterParam
	{
		BOOL bUse;					//是否使用滤波
		int	nFilterNum;				//需要滤波的数据有几类 和上面的FilterSN对应
		HTuple hvFilterType;			//滤波类型，数组长度对应滤波数量
		HTuple hvOffset;				//超过这个公差就不进行滤波了，直接输出
		HTuple hvMarkSize;				//滤波模板大小，数组长度对应滤波数量
	};
public:
	CDataFilter();
	~CDataFilter();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//数据滤波
	BOOL Filter(HTuple &hvDataRaw,HTuple *hvDataFiltered);
	BOOL Filtering(C_MergeWork &tOutput);

	tagTFilterParam GetFilterParam() { return m_tFilterParam; }
	int				GetFilterNum() { return m_tFilterParam.nFilterNum; }
	HTuple			GetFilterType() { return m_tFilterParam.hvFilterType; }
	HTuple			GetMarkSize() { return m_tFilterParam.hvMarkSize; }
	HTupleVector	GetData() { return m_hvecData; }
	BOOL			IsUse() { return m_tFilterParam.bUse; }
	void			SetUse(const BOOL& bUse) { m_tFilterParam.bUse = bUse; }

	void SetFilterParam(CDataFilter::tagTFilterParam& tParam) {
		m_tFilterParam = tParam;
	}
	BOOL SetFilterNum(const int& nNum) { 
		return m_tFilterParam.nFilterNum = nNum;
	}
	BOOL SetFilterType(const HTuple& hvType) {
		if (hvType.Length() < m_tFilterParam.nFilterNum) return FALSE;
		return m_tFilterParam.hvFilterType = hvType;
	}
	BOOL SetMarkSize(const HTuple& hvMarkSize) {
		if (hvMarkSize.Length() < m_tFilterParam.nFilterNum) return FALSE;
		return m_tFilterParam.hvMarkSize = hvMarkSize;
	}
public:
	static CString s_strFilterName[];//滤波名

private:
	//单个数据的滤波
	BOOL SingleFiltering(HTuple hvSrc, const int& nSN, HTuple* hvDst);
	//多个数据的滤波
	BOOL MultiFiltering(HTuple hvSrcs, const int& nSN, HTuple* hvDsts);
private:
	tagTFilterParam m_tFilterParam;		//数据滤波参数
	HTupleVector m_hvecData;			//数据
	CString m_strProductParamPath;		//产品参数文件
};