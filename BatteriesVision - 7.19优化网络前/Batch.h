#pragma once
#include "BaseTmpl.h"
#include <vector>
#include "stdafx.h"
#include "SCIDataFlow.in.h"
#include "SCIGrabber.in.h"

class CCounter : public CBaseTmpl
{
public:
	typedef struct tagSingleNGCount
	{
		int		nType;
		int		nCount;
		double	dMeter;
	};
	typedef std::vector<tagSingleNGCount> NGCountList;
	typedef struct tagCount
	{
		double	dTotalMeter;
		double	dNGMeter;
		int		nNGCount;
		NGCountList vecNG;
	};
	
	typedef std::vector<tagCount> CountList;

public:
	CCounter();
	~CCounter();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//计数统计
	BOOL Count(const C_MergeWork& tOutput);

	//情况当前卷统计
	void ClearCurrent();
private:
	CString m_strCounterPath;

	int			m_nCounterCount;
	int			m_nNGTypeCount;
	CountList	m_vecCount;
};