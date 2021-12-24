//////////////////////////////////////////////////////////////////////////
///DataFilter.h ���������˲��ʹ���
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CDataFilter : public CBaseTmpl
{
public:
	enum eFilterType
	{
		None=0,		//��ʹ���˲�
		Mean,		//��ֵ�˲�
		Median,		//��ֵ�˲�
		MedianMean,	//��ֵƽ���˲�
	};
	enum eFilterSN
	{
		CoatWidth0 = 0,		//Ϳ����
		CoatWidth1,
		AT9Width0,			//��AT9��
		AT9Width1,			//��AT9��
	};
	struct tagTFilterParam
	{
		BOOL bUse;					//�Ƿ�ʹ���˲�
		int	nFilterNum;				//��Ҫ�˲��������м��� �������FilterSN��Ӧ
		HTuple hvFilterType;			//�˲����ͣ����鳤�ȶ�Ӧ�˲�����
		HTuple hvOffset;				//�����������Ͳ������˲��ˣ�ֱ�����
		HTuple hvMarkSize;				//�˲�ģ���С�����鳤�ȶ�Ӧ�˲�����
	};
public:
	CDataFilter();
	~CDataFilter();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//�����˲�
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
	static CString s_strFilterName[];//�˲���

private:
	//�������ݵ��˲�
	BOOL SingleFiltering(HTuple hvSrc, const int& nSN, HTuple* hvDst);
	//������ݵ��˲�
	BOOL MultiFiltering(HTuple hvSrcs, const int& nSN, HTuple* hvDsts);
private:
	tagTFilterParam m_tFilterParam;		//�����˲�����
	HTupleVector m_hvecData;			//����
	CString m_strProductParamPath;		//��Ʒ�����ļ�
};