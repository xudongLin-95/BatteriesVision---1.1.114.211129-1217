#pragma once
class CPIDController
{
public:
	CPIDController(void);
	CPIDController(double dKp,double dActionThresh,double dOffset,int nDelay);
	~CPIDController(void);
	std::queue<double> measureQueue;
	double m_dKp;
	double m_dOffset;
	double m_dIOffset;
	int m_nDelay;
	double m_dActionThresh;
	double PIDWork(double dOffset);
	LONGLONG m_Count;

	BOOL m_bFlag;
	BOOL m_bEnable;
};

