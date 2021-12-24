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
		int		nCount;		//数量
		double	dMeter;		//米数
		double  dRate;		//NG占比，%
	};
	typedef std::vector<tagSingleNGCount> NGCountList;

	typedef struct tagSingleTypeNGCount
	{
		double		dNGMeter;				//NG米数
		int			nNGCount;				//NG数量
		NGCountList vecCoat[2];				//正反面涂布NG，涂布数量对应元素个数
		NGCountList vecFoil[2];				//正反面极耳NG，极耳数量对应元素个数
		NGCountList vecAT11BaseCoat[2];		//正反面AT11/凹版NG，AT11/凹版数量对应元素个数
		NGCountList vecMisalignment;		//正反面错位
	};

	typedef struct tagStopInfo//停机位
	{
		CString		strDiscribe;		//停机描述
		CString		strTimeStart;		//开始时间
		CString		strTimeEnd;			//停止时间
		double		dMeterPosStart;		//停止涂布的位置
		double		dMeterPosEnd;		//开始涂布的位置

		inline void Clear()
		{
			strDiscribe = "";
			strTimeStart = "";
			strTimeEnd = "";
			dMeterPosStart = 0;
			dMeterPosEnd = 0;
		}
	};
	typedef std::vector<tagStopInfo> StopInfoList;

	typedef struct tagCount//卷统计信息
	{
		double					dTotalMeter;			//每卷总米数
		double					dTotalMeterSelect;		//每卷有效总米数(不含空箔材)
		double					dTotalNGMeter;			//综合总NG米数
		int						nTotalNGCount;			//综合总NG数量
		tagSingleTypeNGCount	tSizeNGCounts;			//尺寸NG,抛开停机前后10米
		tagSingleTypeNGCount	tDefectNGCounts;		//瑕疵NG，抛开停机前后10米
		StopInfoList			vecStopInfo;			//停机位
	};

	typedef struct tagBatch
	{
		CString			strName;
		COleDateTime	timeStart;
		COleDateTime	timeEnd;
		inline  CString  GetStartTime()
		{
			return timeStart.Format("%Y-%m-%d %H:%M:%S");
		}

		inline  CString  GetEndTime() 
		{
			return timeEnd.Format("%Y-%m-%d %H:%M:%S");
		}

		inline const CTimeSpan & GetTimeSpan() const
		{
			SYSTEMTIME tm1, tm2;
			timeStart.GetAsSystemTime(tm1);
			timeEnd.GetAsSystemTime(tm2);
			return CTimeSpan(CTime(tm2) - CTime(tm1));
		}

		inline const CString & GetTimeSpanS() const
		{
			SYSTEMTIME tm1, tm2;
			timeStart.GetAsSystemTime(tm1);
			timeEnd.GetAsSystemTime(tm2);
			CTimeSpan tSpan = CTimeSpan(CTime(tm2) - CTime(tm1));
			CString strSpan="", strTmp;
			
			if (tSpan.GetDays() != 0)
			{
				strTmp.Format("%d天 ", tSpan.GetDays());
				strSpan += strTmp;
			}
			strTmp.Format("%d小时 ", tSpan.GetHours());
			strSpan += strTmp;
			strTmp.Format("%d分钟 ", tSpan.GetMinutes());
			strSpan += strTmp;
			strTmp.Format("%d秒 ", tSpan.GetSeconds());
			strSpan += strTmp;

			return strSpan;
		}

		inline const CString & GetTimeSpanSForNow() const
		{
			SYSTEMTIME tm1;
			timeStart.GetAsSystemTime(tm1);
			CTimeSpan tSpan = CTimeSpan(CTime::GetCurrentTime() - CTime(tm1));

			CString strSpan = "", strTmp;

			if (tSpan.GetDays() != 0)
			{
				strTmp.Format("%d天 ", tSpan.GetDays());
				strSpan += strTmp;
			}
			strTmp.Format("%d小时 ", tSpan.GetHours());
			strSpan += strTmp;
			strTmp.Format("%d分钟 ", tSpan.GetMinutes());
			strSpan += strTmp;
			strTmp.Format("%d秒 ", tSpan.GetSeconds());
			strSpan += strTmp;

			return strSpan;
		}
	};

public:
	CCounter();
	~CCounter();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//计数统计
	BOOL Count(C_MergeWork& tOutput);

	//清空当前膜卷统计
	void ClearCurrent();
	//开始新膜卷
	void StartBatch(const CString strName) 
	{
		m_tCurrentBatch.timeStart = COleDateTime::GetCurrentTime();
		m_tCurrentBatch.strName = strName;
		bStartNewBetchFlag = TRUE;
	}
	//结束膜卷
	void EndBatch();
	//获取当前卷信息
	const tagBatch & GetCurrentBatch() { return m_tCurrentBatch; }
	//获取上一卷信息
	tagBatch  GetLastBatch() { return m_tLastBatch; }

	//获取当前卷统计
	const tagCount & GetCurrentCount() { return m_tCount; }

	//获取上一卷统计
	const tagCount & GetLastCount() { return m_tLastCount; }

	//获取当前卷记米
	const double & GetCurrentMeterCount() { return m_tCount.dTotalMeter; }

	//获取上一卷记米
	const double & GetLastMeterCount() { return m_tLastCount.dTotalMeter; }

	//获取NG类型名字
	const CString & GetNGTypeName(const int& nType)
	{
		if (nType > m_vecTypeName.size() - 1 || nType < 0) return "null";

		return m_vecTypeName[nType];
	}

	//判断是否统计
	BOOL GetCountEnable(const int& nNGType);
	BOOL GetCountDataFilteringEnble(C_MergeWork& tOutput);
	
	//判断是否能出货
	BOOL GetShipment();

	//换卷生成报表
	BOOL WriteLastBatchRecord();

	//设定当前停机描述
	BOOL SetCurrentStopDiscribe(CString strInfo);
	
	BOOL bStartNewBetchFlag;//是否更新了膜卷标志

	BOOL m_bStopPos;	//是否出现停机位
private:
	CString m_strCounterPath;

	int			m_nCounterCount;
	int			m_nNGTypeCount;
	//BOOL       m_CounterShipment;

	NameList	m_vecTypeName;
	tagCount	m_tCount;
	tagCount	m_tLastCount;

	tagBatch	m_tCurrentBatch;		//当前膜卷信息
	tagBatch	m_tLastBatch;			//上一膜卷信息

};