
#include <afxtempl.h>
#include <vector>
#ifndef _DEFINE_GEICHECK
#define _DEFINE_GEICHECK

#define FEATURE_COUNT 18
//����ȱ����������ʾ��
const CString NAME_Feature[] = {
	_T("���(����)"),
	_T("ֱ��(����)"),
	_T("�������"),
	_T("Բ�ƶ�"),
	_T("���ƶ�"),
	_T("���ն�"),
	_T("�Ƕ�(����)"),
	_T("�׶���Ŀ"),
	_T("��С�Ҷ�"),
	_T("���Ҷ�"),
	_T("ƽ���Ҷ�"),
	_T("�ҶȲ�ֵ"),
	_T("����(����)"),
	_T("����(����)"),
	_T("������(����)"),
	_T("������(����)"),
	_T("��(����)"),
	_T("��(����)"),
};

//����ȱ������������halcon��
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


//try�ĺ궨��
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

	// ��ʼ
	void Start()
	{
		QueryPerformanceCounter(&m_lgiCounterStart);
	}

	// ����
	int End()	// ����ֵ����ʱ����λ�����룩
	{
		QueryPerformanceCounter(&m_lgiCounterEnd);

		m_dSpendTime = (m_lgiCounterEnd.QuadPart - m_lgiCounterStart.QuadPart)*1.0 / m_lgiFrequency.QuadPart;

		return (int)m_dSpendTime;
	}

	// ��ú�ʱ����λ�����룩
	int GetMillisecondInt()
	{
		return (int)(m_dSpendTime * 1000);
	}

	// ��ú�ʱ����λ�����룩
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