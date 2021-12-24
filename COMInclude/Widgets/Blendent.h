//////////////////////////////////////////////////////////////////////////
/////blendent.h  用于设置界面和控件的颜色               /////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"

typedef struct tagTBlendentButtonExt
{
	short shOffset;//offset
	COLORREF BKColorIn;//BTNST_COLOR_BK_IN
	COLORREF BKColorOut;//BTNST_COLOR_BK_IN
	COLORREF BKColorFocus;//BTNST_COLOR_BK_FOCUS
	COLORREF FGColorIn;//BTNST_COLOR_FG_IN
	COLORREF FGColorOut;//BTNST_COLOR_FG_OUT
	COLORREF FGColorFocus;//BTNST_COLOR_FG_FOCUS
}BlendentButtonExt,*PBlendentButtonExt;

typedef struct tagTBlendentStaticExt
{
	COLORREF TextColor;//字体颜色
	COLORREF BKColor;//背景色1
	COLORREF BKHigh;//背景色2（背景色1是主色），用于渐变
}BlendentStaticExt,*PBlendentStaticExt;

typedef struct tagTBlendentReprotCtrlExt
{
	COLORREF HeaderBKColor;//表头背景颜色
	int nGradient;//渐变系数
	int nR,nG,nB;//表头背景颜色，三个分量
	COLORREF HeaderTextColor;//表头字体颜色
	COLORREF BKColor;//表身背景颜色
}BlendentReprotCtrlExt,*PBlendentReprotCtrlExt;

typedef struct tagTBlendentEditExt
{
	COLORREF BKColor;//编辑框背景色
	COLORREF BKColorFocus;//编辑框聚焦背景色
	COLORREF Normal;//默认状态边框颜色
	COLORREF Hover;//鼠标浮动状态边框颜色
	COLORREF Focus;//聚焦状态边框颜色
}BlendentEditExt,*PBlendentEditExt;

typedef struct tagTBlendentMenuExt
{
	COLORREF BKColorNormal;//菜单背景颜色
	COLORREF BKColorFocus;//菜单背景聚焦颜色
	COLORREF BDColorFocus;//菜单背景聚焦边框颜色
	COLORREF TextColorNormal;//正常状态字体颜色
	COLORREF TextColorFocus;//聚焦状态字体颜色
	COLORREF IconBKColor;//菜单左侧图标位置背景颜色
	COLORREF SPColor;//分隔符颜色
}BlendentMenuExt,*PBlendentMenuExt;

typedef struct tagTBlendentDlg
{
	COLORREF MainColor;//主配色,标题栏、按钮的颜色
	int nR,nG,nB;//主配色的三个分量;
	COLORREF BKColor;//背景配色，用于界面背景

}BlendentDlg,*PBlendentDlg;

typedef struct tagTBlendent
{
	BlendentDlg Dlg;//用于界面，含主配色
	BlendentButtonExt Button;//用于按钮
	BlendentReprotCtrlExt List;//用于列表
	BlendentStaticExt Static;//用于静态文本
	BlendentEditExt Edit;//用于编辑框
	BlendentMenuExt Menu;//用于菜单
}Blendent,*PBlendent;

class W_LIB CBlendent
{
public:
	CBlendent(void);
	~CBlendent(void);

	void Init();//用于初始化所有字体和颜色变量
	//void SettingDlg();
public:
	tagTBlendent m_Blendent;//配色
private:
	CString m_strBlendentFile;//配置文件路径

private:
	CString GetCurrentAppPath();
	void GetRGB(const CString &strRGB,int &nR,int &nG,int &nB);
};