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
		F_Warn,		//预警
	};
	struct tagInit
	{
		int  nTitleType;		//0正面A面，反面B面，1正面B面，反面A面
		int  nCoatType;			//0单面，1双面
		BOOL bExistAT11;		//是否存在AT11
		BOOL bExistBaseCoat;	//是否存在凹版
		int  nCoatNum;			//涂布数量
	};
	struct tagDefect
	{
		double		dCenterX;		//中心坐标(相对于基材左边的位置,mm)
		double		dWidth;			//瑕疵宽
		double		dHeight;		//瑕疵高
		CString		strName;		//缺陷名
	};
	struct tagWidth
	{
		std::vector<double>		vecCoatWidth;				//正面涂布宽
		std::vector<eFlag>		vecCoatWidthFlag;			//正面涂布宽标志位 0
		std::vector<double>		vecCoatWidth2;				//反面涂布宽
		std::vector<eFlag>		vecCoatWidthFlag2;			//反面涂布宽标志位 0

		std::vector<double>		vecFoilWidth;				//正面极耳宽
		std::vector<eFlag>		vecFoilWidthFlag;			//正面极耳宽标志位
		std::vector<double>		vecFoilWidth2;				//反面极耳宽
		std::vector<eFlag>		vecFoilWidthFlag2;			//反面极耳宽标志位

		std::vector<double>		vecAT11BaseCoatWidth;		//正面AT11/凹版宽
		std::vector<eFlag>		vecAT11BaseCoatWidthFlag;	//正面AT11/凹版标志位
		std::vector<double>		vecAT11BaseCoatWidth2;		//反面AT11/凹版宽
		std::vector<eFlag>		vecAT11BaseCoatWidthFlag2;	//反面AT11/凹版标志位

		std::vector<double>		vecMisalignment;			//错位
		std::vector<eFlag>		vecMisalignmentFlag;		//错位标志位

		double					dWholeWidth;				//正面基材宽，mm
		double					dWholeWidth2;				//反面基材宽，mm

		std::vector<tagDefect>	vecDefects;					//正面瑕疵信息
		std::vector<tagDefect>	vecDefects2;				//反面瑕疵信息

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
	int		m_nCoatNum;		//涂布数量
	BOOL	m_bExitAT11;	//是否存在AT11
	BOOL	m_bExitBasecoat;//是否存在凹版

	int		m_nClientWidth;
	int		m_nClientHeight;

	CRect	m_rcClient;
	CRect   m_rcBaseFoil;

	CBitmap m_bitmapBg;
	CDC		m_dcClient;

	COLORREF	m_crBg;
	COLORREF	m_crBaseFoil;	//基材颜色
	COLORREF	m_crCoat;		//正面涂布颜色
	COLORREF	m_crBaseCoat;	//凹版颜色
	COLORREF	m_crAT11;		//AT11颜色
	COLORREF	m_crDefect;		//缺陷标记颜色

	COLORREF	m_crOK;			//OK颜色
	COLORREF	m_crNG;			//NG颜色
	COLORREF	m_crWarn;		//预警颜色

	COLORREF	m_crBigTitle;	//正反面标题颜色
	COLORREF	m_crValueName;	//测量名称颜色

	//CFont		m_ftValue;		//显示值的字体
	//CFont		m_ftTitle;		//显示标题的字体

	CPen*	m_pOldPen;
	CBrush*	m_pOldBrush;
	CFont*  m_pOldFont;

	std::vector<CRect> m_vecRtCoat;				//正面涂布位置
	std::vector<CRect> m_vecRtCoat2;			//反面涂布位置
	std::vector<CRect> m_vecRtFoil;				//正面极耳位置
	std::vector<CRect> m_vecRtFoil2;			//反面极耳位置
	std::vector<CRect> m_vecRtAT11BaseCoat;		//反面AT11位置
	std::vector<CRect> m_vecRtAT11BaseCoat2;	//反面AT11位置

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

