// IconMenu1.h: interface for the CMenuExt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONMENU1_H__FAA8B855_EE72_4332_96D7_73F8FA07E521__INCLUDED_)
#define AFX_ICONMENU1_H__FAA8B855_EE72_4332_96D7_73F8FA07E521__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int MAX_MENUCOUNT = 80,ITEMHEIGHT = 26,ITEMWIDTH= 200; //�˵�������


/*************************************
CMenuItemInfo�ṹ���ڼ�¼�˵�����Ϣ
*************************************/
struct CMenuItemInfo
{
	CString m_ItemText;//�˵����ı�
	int m_IconIndex;//�˵�������
	int m_ItemID;//�˵���� -2����˵�,-1����ʽ�˵�,0�ָ���,������ͨ�˵�
};

class W_LIB CMenuExt : public CMenu
{
	enum
	{
		COLOR_BK_NORMAL = 0,		//����ɫ����ͨ״̬
		COLOR_BK_FOCUS,				//����ɫ���۽�
		COLOR_FG_NORMAL,			//ǰ��ɫ����ͨ״̬
		COLOR_FG_FOCUS,				//ǰ��ɫ���۽�
		COLOR_BD_FOCUS,				//�߿�ɫ���۽�
		COLOR_BK_ICON,				//����ɫ�����ͼ�걳��
		COLOR_SP_NORMAL,			//�ָ�����ɫ

		COLOR_NUM_MAX
	};
public:
	CMenuItemInfo m_ItemLists[MAX_MENUCOUNT]; //�˵�����Ϣ
	int m_index; //��ʱ����
	int m_iconindex; //ͼ������
	//CImageList m_imagelist;//�洢�˵���ͼ��

	BOOL m_isdrawtitle; //�Ƿ��ػ����
	int m_nFontPointSize;
	CString m_strFontFaceName;
	CFont m_Font; //��������

	COLORREF m_colors[COLOR_NUM_MAX];
	int m_nItemWidth;//�˵�����
	int m_nItemSpaceWidth;//�˵����϶��ȣ����������������Ŀ�ȣ�
	int m_nItemHeight;//�˵���߶�

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
	void DrawTopMenu(CDC* m_pdc,CRect m_rect,BOOL m_selected = FALSE); //���ƶ���˵�
	void DrawSeparater(CDC* m_pdc,CRect m_rect);//���Ʒָ���
	void DrawComMenu(CDC* m_pdc,CRect m_rect,COLORREF m_fromcolor,COLORREF m_tocolor, BOOL m_selected = FALSE);
	void DrawMenuIcon(CDC* m_pdc,CRect m_rect,int m_icon );
	void DrawMenuTitle(CDC* m_pdc,CRect m_rect,CString m_title);
	void SetAttributes(int nPointSize,LPCTSTR lpszFaceName);

	//override memu's viutual method
	virtual ~CMenuExt();
	virtual void MeasureItem( LPMEASUREITEMSTRUCT lpStruct );//���ò˵����С
	virtual void DrawItem( LPDRAWITEMSTRUCT lpStruct );//�ػ�˵���

};

#endif // !defined(AFX_ICONMENU1_H__FAA8B855_EE72_4332_96D7_73F8FA07E521__INCLUDED_)
