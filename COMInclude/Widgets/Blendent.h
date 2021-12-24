//////////////////////////////////////////////////////////////////////////
/////blendent.h  �������ý���Ϳؼ�����ɫ               /////////////
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
	COLORREF TextColor;//������ɫ
	COLORREF BKColor;//����ɫ1
	COLORREF BKHigh;//����ɫ2������ɫ1����ɫ�������ڽ���
}BlendentStaticExt,*PBlendentStaticExt;

typedef struct tagTBlendentReprotCtrlExt
{
	COLORREF HeaderBKColor;//��ͷ������ɫ
	int nGradient;//����ϵ��
	int nR,nG,nB;//��ͷ������ɫ����������
	COLORREF HeaderTextColor;//��ͷ������ɫ
	COLORREF BKColor;//��������ɫ
}BlendentReprotCtrlExt,*PBlendentReprotCtrlExt;

typedef struct tagTBlendentEditExt
{
	COLORREF BKColor;//�༭�򱳾�ɫ
	COLORREF BKColorFocus;//�༭��۽�����ɫ
	COLORREF Normal;//Ĭ��״̬�߿���ɫ
	COLORREF Hover;//��긡��״̬�߿���ɫ
	COLORREF Focus;//�۽�״̬�߿���ɫ
}BlendentEditExt,*PBlendentEditExt;

typedef struct tagTBlendentMenuExt
{
	COLORREF BKColorNormal;//�˵�������ɫ
	COLORREF BKColorFocus;//�˵������۽���ɫ
	COLORREF BDColorFocus;//�˵������۽��߿���ɫ
	COLORREF TextColorNormal;//����״̬������ɫ
	COLORREF TextColorFocus;//�۽�״̬������ɫ
	COLORREF IconBKColor;//�˵����ͼ��λ�ñ�����ɫ
	COLORREF SPColor;//�ָ�����ɫ
}BlendentMenuExt,*PBlendentMenuExt;

typedef struct tagTBlendentDlg
{
	COLORREF MainColor;//����ɫ,����������ť����ɫ
	int nR,nG,nB;//����ɫ����������;
	COLORREF BKColor;//������ɫ�����ڽ��汳��

}BlendentDlg,*PBlendentDlg;

typedef struct tagTBlendent
{
	BlendentDlg Dlg;//���ڽ��棬������ɫ
	BlendentButtonExt Button;//���ڰ�ť
	BlendentReprotCtrlExt List;//�����б�
	BlendentStaticExt Static;//���ھ�̬�ı�
	BlendentEditExt Edit;//���ڱ༭��
	BlendentMenuExt Menu;//���ڲ˵�
}Blendent,*PBlendent;

class W_LIB CBlendent
{
public:
	CBlendent(void);
	~CBlendent(void);

	void Init();//���ڳ�ʼ�������������ɫ����
	//void SettingDlg();
public:
	tagTBlendent m_Blendent;//��ɫ
private:
	CString m_strBlendentFile;//�����ļ�·��

private:
	CString GetCurrentAppPath();
	void GetRGB(const CString &strRGB,int &nR,int &nG,int &nB);
};