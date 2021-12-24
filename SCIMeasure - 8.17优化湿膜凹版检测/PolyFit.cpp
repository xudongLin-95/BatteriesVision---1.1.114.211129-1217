#include "stdafx.h"
#include "PolyFit.h"

CPolyFit::CPolyFit():
	m_hvecEMatrix(1)
{

}

CPolyFit::~CPolyFit()
{

}

void CPolyFit::Init()
{

}

BOOL CPolyFit::BinomialFit(HTuple hvX, HTuple hvY, tagFitResult& tResult)
{
	HTuple hvA, hvB, hvC, hvR2, hvYFit;
	try
	{
		ConicComp(hvX, hvY, hvX.Length(), &hvA, &hvB, &hvC, &hvR2, &hvYFit);
		tResult.dRSquare = hvR2.D();
		tResult.hvFitY = hvYFit;
		tResult.hvCoefficients.Clear();
		tResult.hvCoefficients.Append(hvA);
		tResult.hvCoefficients.Append(hvB);
		tResult.hvCoefficients.Append(hvC);
	}
	catch (HException&)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CPolyFit::PolyFit(HTuple hvX, HTuple hvY, int nEx, tagFitResult& tResult)
{
	//计算多项式拟合系数
	try
	{
		EMatrix(hvX, hvY, nEx, &tResult.hvCoefficients);
	}
	catch (HException&)
	{
		return FALSE;
	}
	//计算R平方值，计算回归平方和占总误差平方和的比例
	//yi = getY(x[i]);
	//r_ssr += ((yi - mean_y)*(yi - mean_y));//计算回归平方和  
	//r_sse += ((yi - y[i])*(yi - y[i]));//残差平方和

	try
	{
		HTuple hvMeanY, hvMeanX, hvMeanXY, hvMeanXX, hvFitY, hvDiff, hvSSReg, hvProd, hvSSResid, hvSSTotal;
		//m = (x_mean*y_mean - xy_mean)/(x_mean^2 - xx_mean)
		//b = y_mean - m*x_mean
		//线性回归方程 f = m*x+b
		TupleMean(hvX, &hvMeanX);
		TupleMean(hvY, &hvMeanY);


		//求拟合曲线中x对应所有的Y值
		hvFitY.Clear();
		for (int i = 0; i < hvX.Length(); i++)
		{
			double dSum = 0;
			for (int i = 0; i < tResult.hvCoefficients.Length(); i++)
			{
				dSum += tResult.hvCoefficients[i].D() * pow((double)hvX[i].D(), (int)i);
			}
			hvFitY.Append(dSum);
		}
		
		//计算回归平方和
		TupleSub(hvFitY, hvMeanY.D(), &hvDiff);
		TupleMult(hvDiff, hvDiff, &hvProd);
		TupleSum(hvProd, &hvSSReg);
		//计算残差平方和
		TupleSub(hvFitY, hvY, &hvDiff);
		TupleMult(hvDiff, hvDiff, &hvProd);
		TupleSum(hvProd, &hvSSResid);
		//总平方和(sstotal)=回归平方和+残差平方和(ssresid)
		hvSSTotal = hvSSReg.D() + hvSSResid.D();

		//R平方值=回归平方和/总平方和
		tResult.dRSquare = hvSSReg.D() / hvSSTotal.D();
	}
	catch (HException&)
	{
		return FALSE;
	}

	return TRUE;
}

double CPolyFit::MultSum(HTuple hvX, HTuple hvY)
{
	HTuple hvSum = 0;
	HTuple hvProd;
	TupleMult(hvX, hvY, &hvProd);
	TupleSum(hvProd, &hvSum);

	return hvSum.D();
}

double CPolyFit::RelatePow(HTuple hvX, int nEx)
{
	HTuple hvSum = 0;
	HTuple hvPow;
	TuplePow(hvX, nEx, &hvPow);
	TupleSum(hvPow, &hvSum);
	
	return hvSum.D();
}

double CPolyFit::RelateMutiXY(HTuple hvX, HTuple hvY, int nEx)
{
	HTuple hvSum = 0;
	HTuple hvPow, hvProd;
	TuplePow(hvX, nEx, &hvPow);
	TupleMult(hvPow, hvY, &hvProd);
	TupleSum(hvProd, &hvSum);
	double dReMultiSum = 0;

	return hvSum.D();
}

void CPolyFit::EMatrix(HTuple hvX, HTuple hvY, int nEx, HTuple* hvCoefficients)
{
	m_hvecEMatrix.Clear();
	for (int i = 0; i < nEx; i++)
	{
		TupleGenConst(nEx + 1, 0, &m_hvecEMatrix[i].T());
	}
	for (int i = 0; i < nEx; i++)
	{
		for (int j = 0; j < nEx; j++)
		{
			m_hvecEMatrix[i].T()[j] = RelatePow(hvX, i + j);
		}
		m_hvecEMatrix[i].T()[nEx] = RelateMutiXY(hvX, hvY, i);
	}
	m_hvecEMatrix[0].T()[0] = hvX.Length();
	
	CalEquation(nEx, hvCoefficients);
}

void CPolyFit::CalEquation(int nExp, HTuple* hvCoefficients)
{
	//解方程，消元过程 
	for (int k = 0; k < nExp; k++)
	{
		for (int i = k; i<nExp; i++)
		{
			HTuple hvP = 0;
			if (m_hvecEMatrix[k].T()[k] != 0)
				hvP = m_hvecEMatrix[i].T()[k].D() / m_hvecEMatrix[k].T()[k].D();

			for (int j = k; j < nExp + 1; j++)
				m_hvecEMatrix[i].T()[j] = m_hvecEMatrix[i].T()[j].D() - m_hvecEMatrix[k].T()[j].D() * hvP.D();
		}
	}
	TupleGenConst(nExp, 0, &(*hvCoefficients));

	(*hvCoefficients)[nExp - 1] = m_hvecEMatrix[nExp - 1].T()[nExp].D() / m_hvecEMatrix[nExp].T()[nExp].D();
	//回代求解 
	for (int l = nExp - 2; l >= 0; l--)
	{
		(*hvCoefficients)[l] = (m_hvecEMatrix[l].T()[nExp].D() - F(*hvCoefficients, l, nExp)) / m_hvecEMatrix[l].T()[l].D();
	}
}

double CPolyFit::F(HTuple hvC, int l, int m)
{
	double sum = 0;
	for (int i = l; i < m; i++)
		sum += m_hvecEMatrix[l].T()[i].D() * hvC[i].D();
	return sum;
}

void CPolyFit::ConicComp(HTuple hv_Xdata, HTuple hv_Ydata, HTuple hv_nLen, HTuple *hv_na,
	HTuple *hv_nb, HTuple *hv_nc, HTuple *hv_R2, HTuple *hv_Ycomp)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_atemp, hv_nCoff, hv_dSwit, hv_nI, hv_nJ;
	HTuple  hv_Pow, hv_nK, hv_Pow1, hv_dk, hv_column, hv_mainelement;
	HTuple  hv_Abs, hv_dTmp, hv_bTmp, hv_dMik, hv_dSum, hv_dPow2;
	HTuple  hv_dSubPow, hv_Mean;

	TupleGenConst(5, 0, &hv_atemp);
	TupleGenConst(3, 0, &hv_nCoff);
	TupleGenConst(9, 0, &hv_dSwit);

	{
		HTuple end_val4 = hv_nLen - 1;
		HTuple step_val4 = 1;
		for (hv_nI = 0; hv_nI.Continue(end_val4, step_val4); hv_nI += step_val4)
		{

			{
				HTuple end_val6 = 3 - 1;
				HTuple step_val6 = 1;
				for (hv_nJ = 0; hv_nJ.Continue(end_val6, step_val6); hv_nJ += step_val6)
				{
					TuplePow(HTuple(hv_Xdata[hv_nI]), hv_nJ, &hv_Pow);
					hv_nCoff[hv_nJ] = HTuple(hv_nCoff[hv_nJ]) + (hv_Pow*HTuple(hv_Ydata[hv_nI]));
				}
			}
			{
				HTuple end_val10 = 5 - 1;
				HTuple step_val10 = 1;
				for (hv_nK = 0; hv_nK.Continue(end_val10, step_val10); hv_nK += step_val10)
				{
					TuplePow(HTuple(hv_Xdata[hv_nI]), hv_nK, &hv_Pow1);
					hv_atemp[hv_nK] = HTuple(hv_atemp[hv_nK]) + hv_Pow1;
				}
			}
		}
	}
	hv_atemp[0] = hv_nLen;
	{
		HTuple end_val16 = 3 - 1;
		HTuple step_val16 = 1;
		for (hv_nI = 0; hv_nI.Continue(end_val16, step_val16); hv_nI += step_val16)
		{
			////构建线性方程组系数矩阵，b[]不变
			hv_dk = hv_nI;
			{
				HTuple end_val19 = 3 - 1;
				HTuple step_val19 = 1;
				for (hv_nJ = 0; hv_nJ.Continue(end_val19, step_val19); hv_nJ += step_val19)
				{
					hv_dSwit[(hv_nI * 3) + hv_nJ] = HTuple(hv_atemp[hv_dk]);
					hv_dk += 1;
				}
			}
		}
	}
	////以下为高斯列主元消去法解线性方程组
	{
		HTuple end_val25 = 2 - 1;
		HTuple step_val25 = 1;
		for (hv_nK = 0; hv_nK.Continue(end_val25, step_val25); hv_nK += step_val25)
		{
			hv_column = hv_nK;
			hv_mainelement = HTuple(hv_dSwit[(hv_nK * 3) + hv_nK]);
			{
				HTuple end_val28 = 3 - 1;
				HTuple step_val28 = 1;
				for (hv_nI = hv_nK; hv_nI.Continue(end_val28, step_val28); hv_nI += step_val28)
				{
					TupleFabs(HTuple(hv_dSwit[(hv_nK * 3) + hv_nK]), &hv_Abs);
					if (0 != (hv_Abs>hv_mainelement))
					{
						hv_mainelement = hv_Abs;
						hv_column = hv_nI;
					}
				}
			}
			{
				HTuple end_val35 = 3 - 1;
				HTuple step_val35 = 1;
				for (hv_nJ = hv_nK; hv_nJ.Continue(end_val35, step_val35); hv_nJ += step_val35)
				{
					hv_dTmp = HTuple(hv_dSwit[(hv_nK * 3) + hv_nJ]);
					hv_dSwit[(hv_nK * 3) + hv_nJ] = HTuple(hv_dSwit[(hv_column * 3) + hv_nJ]);
					hv_dSwit[(hv_column * 3) + hv_nJ] = hv_dTmp;
				}
			}
			hv_bTmp = HTuple(hv_nCoff[hv_nK]);
			hv_nCoff[hv_nK] = HTuple(hv_nCoff[hv_column]);
			hv_nCoff[hv_column] = hv_bTmp;
			{
				HTuple end_val43 = 3 - 1;
				HTuple step_val43 = 1;
				for (hv_nI = hv_nK + 1; hv_nI.Continue(end_val43, step_val43); hv_nI += step_val43)
				{
					hv_dMik = HTuple(hv_dSwit[(hv_nI * 3) + hv_nK]) / HTuple(hv_dSwit[(hv_nK * 3) + hv_nK]);
					{
						HTuple end_val45 = 3 - 1;
						HTuple step_val45 = 1;
						for (hv_nJ = hv_nK; hv_nJ.Continue(end_val45, step_val45); hv_nJ += step_val45)
						{
							hv_dSwit[(hv_nI * 3) + hv_nJ] = HTuple(hv_dSwit[(hv_nI * 3) + hv_nJ]) - (hv_dMik*HTuple(hv_dSwit[(hv_nK * 3) + hv_nJ]));
						}
					}
					hv_nCoff[hv_nI] = HTuple(hv_nCoff[hv_nI]) - (hv_dMik*HTuple(hv_nCoff[hv_nK]));

				}
			}
		}
	}
	hv_nCoff[2] = HTuple(hv_nCoff[2]) / HTuple(hv_dSwit[(3 * 2) + 2]);
	for (hv_nI = 1; hv_nI >= 0; hv_nI += -1)
	{
		hv_dSum = 0;
		for (hv_nJ = hv_nI + 1; hv_nJ <= 2; hv_nJ += 1)
		{
			hv_dSum += HTuple(hv_dSwit[(hv_nI * 3) + hv_nJ])*HTuple(hv_nCoff[hv_nJ]);
		}
		hv_nCoff[hv_nI] = (HTuple(hv_nCoff[hv_nI]) - hv_dSum) / HTuple(hv_dSwit[(hv_nI * 3) + hv_nI]);
	}
	(*hv_nc) = ((const HTuple&)hv_nCoff)[0];
	(*hv_nb) = ((const HTuple&)hv_nCoff)[1];
	(*hv_na) = ((const HTuple&)hv_nCoff)[2];

	//***********计算R2***************
	hv_dPow2 = 0;
	hv_dSubPow = 0;
	TupleMean(hv_Ydata, &hv_Mean);
	{
		HTuple end_val68 = hv_nLen - 1;
		HTuple step_val68 = 1;
		for (hv_nI = 0; hv_nI.Continue(end_val68, step_val68); hv_nI += step_val68)
		{
			(*hv_Ycomp)[hv_nI] = ((((*hv_na)*HTuple(hv_Xdata[hv_nI]))*HTuple(hv_Xdata[hv_nI])) + ((*hv_nb)*HTuple(hv_Xdata[hv_nI]))) + (*hv_nc);
			hv_dPow2 += (HTuple(hv_Ydata[hv_nI]) - HTuple((*hv_Ycomp)[hv_nI]))*(HTuple(hv_Ydata[hv_nI]) - HTuple((*hv_Ycomp)[hv_nI]));
			hv_dSubPow += (hv_Mean - HTuple((*hv_Ycomp)[hv_nI]))*(hv_Mean - HTuple((*hv_Ycomp)[hv_nI]));
		}
	}
	(*hv_R2) = 1 - (hv_dPow2 / hv_dSubPow);

	// stop(...); only in hdevelop
}