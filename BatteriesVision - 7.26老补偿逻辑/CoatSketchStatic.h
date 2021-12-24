#ifndef __COATSKETCHSTATIC_H__ 
#define __COATSKETCHSTATIC_H__ 
#include "stdafx.h"
#include <vector>

// CCoatSketchStatic

class CCoatSketchStatic : public CWnd
{
	DECLARE_DYNAMIC(CCoatSketchStatic)
public:
	enum eFlag
	{
		F_NG=0,		//NG
		F_OK,		//OK
		F_Warn,		//Ԥ��
	};
	struct tagInit
	{
		int  nTitleType;		//0����A�棬����B�棬1����B�棬����A��
		int  nCoatType;			//0���棬1˫��
		BOOL bExistAT11;		//�Ƿ����AT11
		BOOL bExistBaseCoat;	//�Ƿ���ڰ���
		int  nCoatNum;			//Ϳ������
	};
	struct tagDefect
	{
		double		dCenterX;		//��������(����ڻ�����ߵ�λ��,mm)
		double		dWidth;			//覴ÿ�
		double		dHeight;		//覴ø�
		CString		strName;		//ȱ����
	};
	struct tagWidth
	{
		std::vector<double>		vecCoatWidth;				//����Ϳ����
		std::vector<eFlag>		vecCoatWidthFlag;			//����Ϳ�����־λ 0
		std::vector<double>		vecCoatWidth2;				//����Ϳ����
		std::vector<eFlag>		vecCoatWidthFlag2;			//����Ϳ�����־λ 0

		std::vector<double>		vecFoilWidth;				//���漫����
		std::vector<eFlag>		vecFoilWidthFlag;			//���漫�����־λ
		std::vector<double>		vecFoilWidth2;				//���漫����
		std::vector<eFlag>		vecFoilWidthFlag2;			//���漫�����־λ

		std::vector<double>		vecAT11BaseCoatWidth;		//����AT11/�����
		std::vector<eFlag>		vecAT11BaseCoatWidthFlag;	//����AT11/�����־λ
		std::vector<double>		vecAT11BaseCoatWidth2;		//����AT11/�����
		std::vector<eFlag>		vecAT11BaseCoatWidthFlag2;	//����AT11/�����־λ

		std::vector<double>		vecMisalignment;			//��λ
		std::vector<eFlag>		vecMisalignmentFlag;		//��λ��־λ

		double					dWholeWidth;				//������Ŀ�mm
		double					dWholeWidth2;				//������Ŀ�mm

		std::vector<tagDefect>	vecDefects;					//����覴���Ϣ
		std::vector<tagDefect>	vecDefects2;				//����覴���Ϣ

		inline void Clear()
		{
			vecCoatWidth.clear();
			vecCoatWidthFlag.clear();
			vecCoatWidth2.clear();
			vecCoatWidthFlag2.clear();
			vecFoilWidth.clear();
			vecFoilWidthFlag.clear();
			vecFoilWidth2.clear();
			vecFoilWidthFlag2.clear();
			vecAT11BaseCoatWidth.clear();
			vecAT11BaseCoatWidthFlag.clear();
			vecAT11BaseCoatWidth2.clear();
			vecAT11BaseCoatWidthFlag2.clear();
			vecMisalignment.clear();
			vecMisalignmentFlag.clear();

			vecDefects.clear();
			vecDefects2.clear();

			dWholeWidth = 0;
			dWholeWidth2 = 0;
		}
	};
public:
	CCoatSketchStatic();
	virtual ~CCoatSketchStatic();

	void InvalidateCtrl();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = NULL);

	void Init(tagInit tInit);
	void ShowValue(tagWidth tValue);
private:
	void DispBaseFoil(CClientDC& dc);
	void DispCoat(CClientDC& dc);
	void DispTitle(CClientDC& dc);
	void DispValue(CClientDC& dc);
	void DispDefects(CClientDC& dc);
private:
	int		m_nCoatNum;		//Ϳ������
	BOOL	m_bExitAT11;	//�Ƿ����AT11
	BOOL	m_bExitBasecoat;//�Ƿ���ڰ���

	int		m_nClientWidth;
	int		m_nClientHeight;

	CRect	m_rcClient;
	CRect   m_rcBaseFoil;

	CBitmap m_bitmapBg;
	CDC		m_dcClient;

	COLORREF	m_crBg;
	COLORREF	m_crBaseFoil;	//������ɫ
	COLORREF	m_crCoat;		//����Ϳ����ɫ
	COLORREF	m_crBaseCoat;	//������ɫ
	COLORREF	m_crAT11;		//AT11��ɫ
	COLORREF	m_crDefect;		//ȱ�ݱ����ɫ

	COLORREF	m_crOK;			//OK��ɫ
	COLORREF	m_crNG;			//NG��ɫ
	COLORREF	m_crWarn;		//Ԥ����ɫ

	COLORREF	m_crBigTitle;	//�����������ɫ
	COLORREF	m_crValueName;	//����������ɫ

	//CFont		m_ftValue;		//��ʾֵ������
	//CFont		m_ftTitle;		//��ʾ���������

	CPen*	m_pOldPen;
	CBrush*	m_pOldBrush;
	CFont*  m_pOldFont;

	std::vector<CRect> m_vecRtCoat;				//����Ϳ��λ��
	std::vector<CRect> m_vecRtCoat2;			//����Ϳ��λ��
	std::vector<CRect> m_vecRtFoil;				//���漫��λ��
	std::vector<CRect> m_vecRtFoil2;			//���漫��λ��
	std::vector<CRect> m_vecRtAT11BaseCoat;		//����AT11λ��
	std::vector<CRect> m_vecRtAT11BaseCoat2;	//����AT11λ��

	tagInit		m_tInit;
	tagWidth	m_tWidth;
protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
};

#endif

