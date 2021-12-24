#pragma once
#include <vector>

class CPolyFit
{
public:
	struct tagFitResult
	{
		HTuple hvCoefficients;					//拟合系数,a b c d e....
		double dRSquare;						//R平方值，回归系数
		HTuple hvFitY;							//拟合Y
		double dMaxDiffPercent;					//最大偏差百分比
	};

public:
	CPolyFit();
	~CPolyFit();

	void Init();

	//多项式拟合,nEx多项式数
	BOOL PolyFit(HTuple hvX, HTuple hvY, int nEx, tagFitResult& tResult);
	//二项式拟合
	BOOL BinomialFit(HTuple hvX, HTuple hvY, tagFitResult& tResult);

private:
	//乘积和
	double MultSum(HTuple hvX, HTuple hvY);
	//ex次方和
	double RelatePow(HTuple hvX, int nEx);
	//x的ex次方与y的乘积的累加
	double RelateMutiXY(HTuple hvX, HTuple hvY, int nEx);
	//计算方程组的增广矩阵
	void EMatrix(HTuple hvX, HTuple hvY, int nEx, HTuple* hvCoefficients);
	//求解方程
	void CalEquation(int nEx, HTuple* hvCoefficients);
	//解方程调用函数
	double F(HTuple hvC, int l, int m);

	//二次拟合
	void ConicComp(HTuple hv_Xdata, HTuple hv_Ydata, HTuple hv_nLen, HTuple *hv_na,
		HTuple *hv_nb, HTuple *hv_nc, HTuple *hv_R2, HTuple *hv_Ycomp);
private:
	//增广矩阵参数
	HTupleVector m_hvecEMatrix;
};
