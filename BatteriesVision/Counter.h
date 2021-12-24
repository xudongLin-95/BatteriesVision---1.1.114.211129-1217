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
		int		nCount;		//����
		double	dMeter;		//����
		double  dRate;		//NGռ�ȣ�%
	};
	typedef std::vector<tagSingleNGCount> NGCountList;

	typedef struct tagSingleTypeNGCount
	{
		double		dNGMeter;				//NG����
		int			nNGCount;				//NG����
		NGCountList vecCoat[2];				//������Ϳ��NG��Ϳ��������ӦԪ�ظ���
		NGCountList vecFoil[2];				//�����漫��NG������������ӦԪ�ظ���
		NGCountList vecAT11BaseCoat[2];		//������AT11/����NG��AT11/����������ӦԪ�ظ���
		NGCountList vecMisalignment;		//�������λ
	};

	typedef struct tagStopInfo//ͣ��λ
	{
		CString		strDiscribe;		//ͣ������
		CString		strTimeStart;		//��ʼʱ��
		CString		strTimeEnd;			//ֹͣʱ��
		double		dMeterPosStart;		//ֹͣͿ����λ��
		double		dMeterPosEnd;		//��ʼͿ����λ��

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

	typedef struct tagCount//��ͳ����Ϣ
	{
		double					dTotalMeter;			//ÿ��������
		double					dTotalMeterSelect;		//ÿ����Ч������(�����ղ���)
		double					dTotalNGMeter;			//�ۺ���NG����
		int						nTotalNGCount;			//�ۺ���NG����
		tagSingleTypeNGCount	tSizeNGCounts;			//�ߴ�NG,�׿�ͣ��ǰ��10��
		tagSingleTypeNGCount	tDefectNGCounts;		//覴�NG���׿�ͣ��ǰ��10��
		StopInfoList			vecStopInfo;			//ͣ��λ
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
				strTmp.Format("%d�� ", tSpan.GetDays());
				strSpan += strTmp;
			}
			strTmp.Format("%dСʱ ", tSpan.GetHours());
			strSpan += strTmp;
			strTmp.Format("%d���� ", tSpan.GetMinutes());
			strSpan += strTmp;
			strTmp.Format("%d�� ", tSpan.GetSeconds());
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
				strTmp.Format("%d�� ", tSpan.GetDays());
				strSpan += strTmp;
			}
			strTmp.Format("%dСʱ ", tSpan.GetHours());
			strSpan += strTmp;
			strTmp.Format("%d���� ", tSpan.GetMinutes());
			strSpan += strTmp;
			strTmp.Format("%d�� ", tSpan.GetSeconds());
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

	//����ͳ��
	BOOL Count(C_MergeWork& tOutput);

	//��յ�ǰĤ��ͳ��
	void ClearCurrent();
	//��ʼ��Ĥ��
	void StartBatch(const CString strName) 
	{
		m_tCurrentBatch.timeStart = COleDateTime::GetCurrentTime();
		m_tCurrentBatch.strName = strName;
		bStartNewBetchFlag = TRUE;
	}
	//����Ĥ��
	void EndBatch();
	//��ȡ��ǰ����Ϣ
	const tagBatch & GetCurrentBatch() { return m_tCurrentBatch; }
	//��ȡ��һ����Ϣ
	tagBatch  GetLastBatch() { return m_tLastBatch; }

	//��ȡ��ǰ��ͳ��
	const tagCount & GetCurrentCount() { return m_tCount; }

	//��ȡ��һ��ͳ��
	const tagCount & GetLastCount() { return m_tLastCount; }

	//��ȡ��ǰ�����
	const double & GetCurrentMeterCount() { return m_tCount.dTotalMeter; }

	//��ȡ��һ�����
	const double & GetLastMeterCount() { return m_tLastCount.dTotalMeter; }

	//��ȡNG��������
	const CString & GetNGTypeName(const int& nType)
	{
		if (nType > m_vecTypeName.size() - 1 || nType < 0) return "null";

		return m_vecTypeName[nType];
	}

	//�ж��Ƿ�ͳ��
	BOOL GetCountEnable(const int& nNGType);
	BOOL GetCountDataFilteringEnble(C_MergeWork& tOutput);
	
	//�ж��Ƿ��ܳ���
	BOOL GetShipment();

	//�������ɱ���
	BOOL WriteLastBatchRecord();

	//�趨��ǰͣ������
	BOOL SetCurrentStopDiscribe(CString strInfo);
	
	BOOL bStartNewBetchFlag;//�Ƿ������Ĥ���־

	BOOL m_bStopPos;	//�Ƿ����ͣ��λ
private:
	CString m_strCounterPath;

	int			m_nCounterCount;
	int			m_nNGTypeCount;
	//BOOL       m_CounterShipment;

	NameList	m_vecTypeName;
	tagCount	m_tCount;
	tagCount	m_tLastCount;

	tagBatch	m_tCurrentBatch;		//��ǰĤ����Ϣ
	tagBatch	m_tLastBatch;			//��һĤ����Ϣ

};