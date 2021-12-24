#pragma once
#include <vector>

enum e_ProductType //��Ʒ����
{
	ThreeCavity = 0,		//1��3��ÿ��һ����Ϳ��
	TwoCavity,				//1��2��ÿ��һ��Ϳ��
	FourCavity,				//1��4��ÿ������Ϳ��
	SixCavity,				//1��6��ÿ������Ϳ��
	EightCavity,			//1��8��ÿ���ĸ�Ϳ��
	TenCavity,				//1��10��ÿ�����Ϳ��
};

enum e_CoatingType
{
	OneSide = 0,			//����Ϳ��
	TwoSide,				//˫��Ϳ��
};

typedef std::vector<double> DoubleList;
struct tagTolerance
{
	double		dStandard;			//��׼ֵ
	double		dStandard2;			//��׼ֵ2,����
	double		dTolerance;			//���ھ�����/�����Ĺ���ֵ
	double		dWarnTolerance;		//Ԥ���Ĺ���ֵ
	double		dFAITolerance;		//�׼��Ĺ���ֵ
	DoubleList	OffsetList;			//double�������飬0A�油����1B�油��
	double		dOffsetOL;			//double���߲���ֵ��A���ʪĤ����

	tagTolerance& operator = (const tagTolerance &t)
	{
		if (this != &t)
		{
			dStandard = t.dStandard;
			dStandard2 = t.dStandard2;
			dTolerance = t.dTolerance;
			dWarnTolerance = t.dWarnTolerance;
			dFAITolerance = t.dFAITolerance;
			OffsetList = t.OffsetList;
			dOffsetOL = t.dOffsetOL;
		}
		return *this;
	}
};
typedef std::vector<tagTolerance> ToleranceList;	//�����б�

typedef std::vector<int> IntList;
struct tagToleranceN
{
	int		nStandard;			//��׼ֵ
	int		nTolerance;			//���ھ�����/�����Ĺ���ֵ
	int		nWarnTolerance;		//Ԥ���Ĺ���ֵ
	IntList OffsetList;			//int�������飬0A�油����1B�油��

	tagToleranceN& operator = (const tagToleranceN &t)
	{
		if (this != &t)
		{
			nStandard = t.nStandard;
			nTolerance = t.nTolerance;
			nWarnTolerance = t.nWarnTolerance;
			OffsetList = t.OffsetList;
		}
		return *this;
	}
};

struct tagPDJudgeParam//��Ʒ�ж�����
{
	bool			bUseCoatWidth;			//�Ƿ�ʹ��Ϳ����
	ToleranceList	vecCoatWidth;
	bool			bUseFoilWidth;			//�Ƿ�ʹ�ò��Ŀ�
	ToleranceList	vecFoilWidth;
	bool			bUseAT11Width;			//�Ƿ�ʹ��AT9/AT11����
	ToleranceList	vecAT11Width;
	bool			bUseBaseCoatWidth;		//�Ƿ�ʹ�õ�Ϳ���
	ToleranceList	vecBaseCoatWidth;
	bool			bUseMisalignment;		//�Ƿ�ʹ�ô�λ���
	ToleranceList	vecMisalignment;
	bool			bUseChecking;			//�Ƿ�ʹ���豸���
	ToleranceList	vecChecking;
	bool			bUseSurfaceDetect;		//�Ƿ�ʹ�ñ���ȱ�ݼ��
};

struct tagSingleLabelParam
{
	int				nNGNum;					//NG����
	double			dNGSize;				//NG����ߴ�
	double			dNGArea;				//NG�������
};
typedef std::vector<tagSingleLabelParam> LabelParamList;

struct tagPDLabelJudgeParam
{
	LabelParamList ParamList;				//���/�����жϲ������飬
};
typedef std::vector<CString> NameList;
struct tagPDInfo
{
	CString					strPDName;			//��Ʒ��
	CString					strNote;			//��Ʒ��ע
	BOOL     				bExistAT11;			//�Ƿ����AT9/AT11������
	BOOL     				bExistBaseCoat;		//�Ƿ������Ϳ������
	BOOL     				bExistIntermittent;	//�Ƿ������Ъ����
	int						nPDType;			//��Ʒ����
	int						nCoatingType;		//Ϳ�����ͣ�0���棬1˫��
	int						nMeasureLineNum;	//����������
	tagPDJudgeParam			tParam;				//�ж�����
	tagPDLabelJudgeParam	tLabelParam;		//����/�����жϲ���
	NameList				vecNameList;		//��ʾ����
};
typedef std::vector<tagPDInfo> ProductList;
