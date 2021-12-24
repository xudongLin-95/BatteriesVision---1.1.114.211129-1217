// IconMenu1.h: interface for the CMenuExt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONMENU1_H__FAA8B855_EE72_4332_96D7_73F8FA07E521__INCLUDED_)
#define AFX_ICONMENU1_H__FAA8B855_EE72_4332_96D7_73F8FA07E521__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int MAX_MENUCOUNT = 80,ITEMHEIGHT = 26,ITEMWIDTH= 200; //菜单项数量


/*************************************
CMenuItemInfo结构用于记录菜单项信息
*************************************/
struct CMenuItemInfo
{
	CString m_ItemText;//菜单项文本
	int m_IconIndex;//菜单项索引
	int m_ItemID;//菜单标记 -2顶层菜单,-1弹出式菜单,0分隔条,其他普通菜单
};

class W_LIB CMenuExt : public CMenu
{
	enum
	{
		COLOR_BK_NORMAL = 0,		//背景色，普通状态
		COLOR_BK_FOCUS,				//背景色，聚焦
		COLOR_FG_NORMAL,			//前景色，普通状态
		COLOR_FG_FOCUS,				//前景色，聚焦
		COLOR_BD_FOCUS,				//边框色，聚焦
		COLOR_BK_ICON,				//背景色，左侧图标背景
		COLOR_SP_NORMAL,			//分隔符颜色

		COLOR_NUM_MAX
	};
public:
	CMenuItemInfo m_ItemLists[MAX_MENUCOUNT]; //菜单项信息
	int m_index; //临时索引
	int m_iconindex; //图像索引
	//CImageList m_imagelist;//存储菜单项图标

	BOOL m_isdrawtitle; //是否重绘标题
	int m_nFontPointSize;
	CString m_strFontFaceName;
	CFont m_Font; //标题字体

	COLORREF m_colors[COLOR_NUM_MAX];
	int m_nItemWidth;//菜单项宽度
	int m_nItemSpaceWidth;//菜单项空隙宽度（除字体宽度外其他的宽度）
	int m_nItemHeight;//菜单项高度

	CMenuExt();
	int m_save;

	BOOL SetAttributes(int nFontPointSize,
		LPCTSTR lpszFontFaceName,
		int nItemSpaceWidth,
		int nItemHeight,
		COLORREF colorBKNormal,
		COLORREF colorBKFocus,
		COLORREF colorBDFocus,
		COLORREF colorFGNormal,
		COLORREF colorFGFocus,
		COLORREF colorBKIcon,
		COLORREF colorSPNormal);
	BOOL ChangeMenuItem(CMenu* m_menu,BOOL m_Toped = FALSE);
	BOOL  AttatchMenu(HMENU m_hmenu);
	void DrawItemText(CDC* m_pdc,LPSTR str,CRect m_rect,BOOL m_selected = FALSE);
	void DrawTopMenu(CDC* m_pdc,CRect m_rect,BOOL m_selected = FALSE); //绘制顶层菜单
	void DrawSeparater(CDC* m_pdc,CRect m_rect);//绘制分隔条
	void DrawComMenu(CDC* m_pdc,CRect m_rect,COLORREF m_fromcolor,COLORREF m_tocolor, BOOL m_selected = FALSE);
	void DrawMenuIcon(CDC* m_pdc,CRect m_rect,int m_icon );
	void DrawMenuTitle(CDC* m_pdc,CRect m_rect,CString m_title);
	void SetAttributes(int nPointSize,LPCTSTR lpszFaceName);

	//override memu's viutual method
	virtual ~CMenuExt();
	virtual void MeasureItem( LPMEASUREITEMSTRUCT lpStruct );//设置菜单项大小
	virtual void DrawItem( LPDRAWITEMSTRUCT lpStruct );//重绘菜单项

};

#endif // !defined(AFX_ICONMENU1_H__FAA8B855_EE72_4332_96D7_73F8FA07E521__INCLUDED_)
