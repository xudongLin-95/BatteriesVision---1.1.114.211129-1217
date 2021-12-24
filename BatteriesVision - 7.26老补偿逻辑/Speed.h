//////////////////////////////////////////////////////////////////////////
///Speed.h 用于计算速度
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CSpeed : public CBaseTmpl//计算速度
{
public:
	CSpeed();
	~CSpeed();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	double CalculateSpeed(const double& dLineRate)
	{
		m_dSpeed = dLineRate * m_dResultion * 0.001 * 60;
		return m_dSpeed;
	}

	double CalculateSpeed(HObject hoImage)
	{
		static BOOL s_bFirst = TRUE;
		HTuple hvHeight;
		GetImageSize(hoImage, &HTuple(), &hvHeight);
		CountSeconds(&m_hvTimeNew);
		m_dSpeed = hvHeight.I() * m_dResultion * 0.001 / (m_hvTimeNew.D() - m_hvTimeOld.D()) * 60;
		m_hvTimeOld = m_hvTimeNew;
		if (s_bFirst == TRUE)
		{
			s_bFirst = FALSE;
			return 0;
		}
		return m_dSpeed;
	}

	//速度，m/min
	double GetSpeed()
	{
		return m_dSpeed;
	}

private:
	double m_dResultion;//分辨率
	double m_dSpeed;

	HTuple m_hvTimeOld;
	HTuple m_hvTimeNew;
};
