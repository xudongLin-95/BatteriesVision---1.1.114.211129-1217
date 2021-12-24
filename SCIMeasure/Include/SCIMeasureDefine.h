
#include <afxtempl.h>
#include <vector>
#ifndef _DEFINE_GEICHECK
#define _DEFINE_GEICHECK

#define FEATURE_COUNT 18
//常用缺陷特征，显示用
const CString NAME_Feature[] = {
	_T("面积(像素)"),
	_T("直径(像素)"),
	_T("长短轴比"),
	_T("圆似度"),
	_T("矩似度"),
	_T("紧凑度"),
	_T("角度(弧度)"),
	_T("孔洞数目"),
	_T("最小灰度"),
	_T("最大灰度"),
	_T("平均灰度"),
	_T("灰度差值"),
	_T("长轴(像素)"),
	_T("短轴(像素)"),
	_T("行坐标(像素)"),
	_T("列坐标(像素)"),
	_T("宽(像素)"),
	_T("高(像素)"),
};

//常用缺陷特征参数，halcon用
const CString STR_Feature[] = {
	_T("area"),
	_T("max_diameter"),
	_T("anisometry"),
	_T("circularity"),
	_T("rectangularity"),
	_T("compactness"),
	_T("phi"),
	_T("holes_num"),
	_T("min"),
	_T("max"),
	_T("mean"),
	_T("deviation"),
	_T("ra"),
	_T("rb"),
	_T("row"),
	_T("column"),
	_T("width"),
	_T("height"),
};

enum EM_Feature
{
	area, max_diameter, anisometry, circularity, rectangularity, compactness, phi, holes_num, G_min, G_max, G_mean,
	G_deviation, ra, rb, row, column, width, height,
};


//try的宏定义
#define _TRY \
	try \
{
#define _CATCH_HALCON(str) \
} \
	catch(HException e) \
{ \
	CString strMessageBox; \
	strMessageBox.Format("%s\r\n%s",str,e.ErrorMessage().Text()); \
	AfxMessageBox(strMessageBox); \
}
#define _CATCH_HALCON_NULL \
} \
	catch(HException e) \
{ \
}
#define _CATCH_HALCON_RETURN(str) \
} \
	catch(HException e) \
{ \
	CString strMessageBox; \
	strMessageBox.Format("%s\r\n%s",str,e.ErrorMessage().Text()); \
	AfxMessageBox(strMessageBox); \
	return; \
}

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

	// 开始
	void Start()
	{
		QueryPerformanceCounter(&m_lgiCounterStart);
	}

	// 结束
	int End()	// 返回值：耗时（单位：毫秒）
	{
		QueryPerformanceCounter(&m_lgiCounterEnd);

		m_dSpendTime = (m_lgiCounterEnd.QuadPart - m_lgiCounterStart.QuadPart)*1.0 / m_lgiFrequency.QuadPart;

		return (int)m_dSpendTime;
	}

	// 获得耗时（单位：毫秒）
	int GetMillisecondInt()
	{
		return (int)(m_dSpendTime * 1000);
	}

	// 获得耗时（单位：毫秒）
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
#endif _DEFINE_GEICHECK