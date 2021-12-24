#include "StdAfx.h"
#include "PController.h"


CPIDController::CPIDController(void)
	: m_dKp(0)
	, m_dOffset(0)
	, m_dIOffset(0)
	, m_nDelay(0)
	, m_dActionThresh(0)
	, m_Count(0)
	, m_bFlag(FALSE)
{
	while(!measureQueue.empty())
		measureQueue.pop();
}
CPIDController::CPIDController(double iKp,double iActionThresh,double iOffset,int iDelay)
{
	m_dKp = iKp;
	m_nDelay = iDelay;
	m_dActionThresh = iActionThresh;
	m_dOffset = iOffset;
	m_dIOffset = 0.0;
	m_Count =0;
	m_bFlag = FALSE;
	while(!measureQueue.empty())
		measureQueue.pop();
}

CPIDController::~CPIDController(void)
{
}
double CPIDController::PIDWork(double iOffset)
{
	if (m_dKp == 0 || m_nDelay ==0)
	{
		return 0;
	}
	measureQueue.push(iOffset);
	m_dIOffset += iOffset;
	if (m_Count++ >= m_nDelay || m_bFlag )
	{
		m_bFlag = TRUE;
		double iHead = measureQueue.front();
		measureQueue.pop();
		m_dIOffset -= iHead;
		double offsetValue = m_dIOffset/m_nDelay;//队列中元素的平均值，平均值>指定阈值
		if (abs(offsetValue) > m_dActionThresh)
		{
			if (m_nDelay !=0 )
			{
				if ((m_Count - 1 ) % m_nDelay == 0 )
				{
					m_dOffset = -1*offsetValue*m_dKp;	
				}	
			}
			else
			{
				m_dOffset = -1*offsetValue*m_dKp;
			}
		}
	}
	return m_dOffset;
}
