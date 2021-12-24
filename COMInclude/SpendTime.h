#if !defined(SpendTime_h_)
#define SpendTime_h_

class CSpendTime
{
public:
	CSpendTime()
	{
		m_dSpendTime = 0.0;

		QueryPerformanceFrequency(&m_lgiFrequency);
	}

	virtual ~CSpendTime()
	{
	}

	// 寮€濮?
	void Start()
	{
		QueryPerformanceCounter(&m_lgiCounterStart);
	}

	// 缁撴潫
	int End()	// 杩斿洖鍊硷細鑰楁椂锛堝崟浣嶏細姣锛?
	{
		QueryPerformanceCounter(&m_lgiCounterEnd);

		m_dSpendTime = (m_lgiCounterEnd.QuadPart - m_lgiCounterStart.QuadPart)*1.0 / m_lgiFrequency.QuadPart;

		return (int)m_dSpendTime;
	}

	// 鑾峰緱鑰楁椂锛堝崟浣嶏細姣锛?
	int GetMillisecondInt()
	{
		return (int)(m_dSpendTime * 1000);
	}

	// 鑾峰緱鑰楁椂锛堝崟浣嶏細姣锛?
	double GetMillisecondDouble()
	{
		return (m_dSpendTime * 1000);
	}

protected:

	LARGE_INTEGER m_lgiCounterStart;
	LARGE_INTEGER m_lgiCounterEnd;
	LARGE_INTEGER m_lgiFrequency;
	double m_dSpendTime;
};

#endif// !defined(SpendTime)