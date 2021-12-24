#pragma once
#include <vector>

class CPolyFit
{
public:
	struct tagFitResult
	{
		HTuple hvCoefficients;					//���ϵ��,a b c d e....
		double dRSquare;						//Rƽ��ֵ���ع�ϵ��
		HTuple hvFitY;							//���Y
		double dMaxDiffPercent;					//���ƫ��ٷֱ�
	};

public:
	CPolyFit();
	~CPolyFit();

	void Init();

	//����ʽ���,nEx����ʽ��
	BOOL PolyFit(HTuple hvX, HTuple hvY, int nEx, tagFitResult& tResult);
	//����ʽ���
	BOOL BinomialFit(HTuple hvX, HTuple hvY, tagFitResult& tResult);

private:
	//�˻���
	double MultSum(HTuple hvX, HTuple hvY);
	//ex�η���
	double RelatePow(HTuple hvX, int nEx);
	//x��ex�η���y�ĳ˻����ۼ�
	double RelateMutiXY(HTuple hvX, HTuple hvY, int nEx);
	//���㷽������������
	void EMatrix(HTuple hvX, HTuple hvY, int nEx, HTuple* hvCoefficients);
	//��ⷽ��
	void CalEquation(int nEx, HTuple* hvCoefficients);
	//�ⷽ�̵��ú���
	double F(HTuple hvC, int l, int m);

	//�������
	void ConicComp(HTuple hv_Xdata, HTuple hv_Ydata, HTuple hv_nLen, HTuple *hv_na,
		HTuple *hv_nb, HTuple *hv_nc, HTuple *hv_R2, HTuple *hv_Ycomp);
private:
	//����������
	HTupleVector m_hvecEMatrix;
};
