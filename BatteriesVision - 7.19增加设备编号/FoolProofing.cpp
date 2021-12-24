#include "StdAfx.h"
#include "FoolProofing.h"

CFoolProofing::CFoolProofing()
{
	
}

CFoolProofing::~CFoolProofing()
{

}

BOOL CFoolProofing::Init()
{
	ReadParam();

	return TRUE;
}

BOOL CFoolProofing::ReadParam()
{
	m_tParam.bUseFoolProofing = GetIniInt("FoolProof", "bUseFoolProofing", TRUE);
	m_tParam.nMinImageGray = GetIniInt("FoolProof", "nMinImageGray", 15);
	return TRUE;
}

BOOL CFoolProofing::WriteParam()
{

	return TRUE;
}


BOOL CFoolProofing::FoolProofing(C_MergeWork& tOutput)
{
	if (!m_tParam.bUseFoolProofing) return TRUE;
	//判断图像是否质量达到检测要求
	//JudgeImageQuality(tOutput);

	return TRUE;
}

BOOL CFoolProofing::JudgeImageQuality(C_MergeWork& tOutput)
{
	static int s_nCount = 0;
	try
	{
		for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
		{
			for (int nCamSN = 0; nCamSN < tOutput.vecB[nBWorkSN].vecA.size(); nCamSN++)
			{
				HTuple hvMeanGray, hvWidth, hvHeight;
				HObject hoRect;
				GetImageSize(tOutput.vecB[nBWorkSN].vecA[nCamSN].hoGrabImage, &hvWidth, &hvHeight);
				GenRectangle1(&hoRect, HTuple(hvHeight / 4).Append(hvHeight / 4 * 3), HTuple(0).Append(0), HTuple(hvHeight / 4 + 2).Append(hvHeight / 4 * 3 + 2), HTuple(hvWidth-1).Append(hvWidth-1));
				Intensity(hoRect, tOutput.vecB[nBWorkSN].vecA[nCamSN].hoGrabImage, &hvMeanGray, &HTuple());
				if (hvMeanGray.TupleMean().D() < m_tParam.nMinImageGray)
				{
					//需要异常提醒，提醒调整相机或光源
					s_nCount++;
					if (s_nCount > 3)
					{
						CString strExcp;
						strExcp.Format("CFoolProofing: 相机%d图像平均灰度小于%d，请检查相机和光源", tOutput.vecB[nBWorkSN].vecA[nCamSN].hvGrabberSN.I(), m_tParam.nMinImageGray);
						theGlobal.m_Log.Alarm(strExcp);
						s_nCount = 0;
					}
				}
			}
		}
	}
	catch (HException& e)
	{

	}
	return TRUE;
}