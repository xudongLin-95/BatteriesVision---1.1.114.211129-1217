#include "StdAfx.h"
#include "MSA.h"


CMSA::CMSA()
{
}


CMSA::~CMSA()
{
}

void  CMSA::Init()
{
	/*m_nFNum = theGlobal.m_ProductTool.GetCurrentProductInfo().tParam.nNum;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			TupleGenConst(m_nFNum, 0, &m_hvFHeight[i][j]);
			TupleGenConst(m_nFNum, 0, &m_hvFWidth[i][j]);
			TupleGenConst(m_nFNum, 0, &m_hvLG[i][j]);
			TupleGenConst(m_nFNum, 0, &m_hvDistance[i][j]);
			TupleGenConst(1, 0, &m_hvEALength[i][j]);

			TupleGenConst(m_nFNum, 0, &m_R_hvFHeight[i][j]);
			TupleGenConst(m_nFNum, 0, &m_R_hvFWidth[i][j]);
			TupleGenConst(m_nFNum, 0, &m_R_hvLG[i][j]);
			TupleGenConst(m_nFNum, 0, &m_R_hvDistance[i][j]);
			TupleGenConst(1, 0, &m_R_hvEALength[i][j]);
		}
	}
	dScore = 0.1;*/
}

//void   CMSA::Set(BOOL bValue)
//{
//	if (TRUE == bValue)
//	{
//		for (int i = 0; i < 2; i++)
//		{
//			for (int j = 0; j < 2; j++)
//			{
//				m_R_hvFHeight[i][j]  = m_hvFHeight[i][j];
//				m_R_hvFWidth[i][j]   = m_hvFWidth[i][j];
//				m_R_hvLG[i][j]       = m_hvLG[i][j];
//				m_R_hvDistance[i][j] = m_hvDistance[i][j];
//				m_R_hvEALength[i][j] = m_hvEALength[i][j];
//			}
//		}
//	}
//}
void  CMSA::MSAFrame(C_MergeWork& tOutput)
{
	//for (int nGrabberSN =0; nGrabberSN<2; nGrabberSN++)
	//{
	//	for (int i = 0; i < tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvDistanceFoil.Length(); i++)
	//	{
	//		int nFSN = tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvDistanceFoilSN[i];//极耳序号
	//		m_hvDistance[nGrabberSN][nGrabberSN][nFSN] = tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvDistanceFoil[i];
	//		m_hvFWidth[nGrabberSN][nGrabberSN][nFSN] = tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvFoilWidth[i];
	//		m_hvFHeight[nGrabberSN][nGrabberSN][nFSN] = tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvFoilHeight[i];
	//		m_hvLG[nGrabberSN][nGrabberSN][nFSN] = tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvLG[i];
	//		if (0 == nFSN && FALSE == theGlobal.m_bMSA)//碰到第0个的时候，重新保存MSA数据
	//		{
	//			for (int i = 0; i < 2; i++)
	//			{
	//				for (int j = 0; j < 2; j++)
	//				{
	//					m_R_hvFHeight[i][j] = m_hvFHeight[i][j];
	//					m_R_hvFWidth[i][j] = m_hvFWidth[i][j];
	//					m_R_hvLG[i][j] = m_hvLG[i][j];
	//					m_R_hvDistance[i][j] = m_hvDistance[i][j];
	//					m_R_hvEALength[i][j] = m_hvEALength[i][j];
	//				}
	//			}
	//		}
	//	}
	//	m_hvEALength[nGrabberSN][nGrabberSN] = tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvEALength;
	//}
	//if (TRUE == theGlobal.m_bMSA)//MSA模式的话 进行上一次保存的随机数的生成
	//{
	//	HTuple hvRand;
	//	TupleRand(5, &hvRand);
	//	hvRand = hvRand - 0.5;
	//	hvRand = hvRand * dScore;
	//	for (int nGrabberSN =0; nGrabberSN<2; nGrabberSN++)
	//	{
	//		for (int i=0;i<tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvDistanceFoil.Length();i++)
	//		{	
	//			int nFSN = tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvDistanceFoilSN[i];//极耳序号	
	//			tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvDistanceFoil[i] = m_R_hvDistance[nGrabberSN][nGrabberSN][nFSN] + hvRand[0];
	//			tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvFoilWidth[i]    = m_R_hvFWidth[nGrabberSN][nGrabberSN][nFSN] + hvRand[1];
	//			tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvFoilHeight[i]   = m_R_hvFHeight[nGrabberSN][nGrabberSN][nFSN] + hvRand[2];
	//			tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvLG[i]           = m_R_hvLG[nGrabberSN][nGrabberSN][nFSN] + hvRand[3];
	//		}
	//		tOutput.B[nGrabberSN].tMeasureOutput.tSide[nGrabberSN].hvEALength = m_R_hvEALength[nGrabberSN][nGrabberSN] + hvRand[4];
	//	}
	//}
}
