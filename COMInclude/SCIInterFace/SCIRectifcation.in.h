#pragma once
#include "Log.in.h"

typedef struct tagTRECTIFICATIONCALL//�ص���Ϣ�ṹ
{
	int nType;
	int nErrorCode;
	BOOL bReLoad ; // �Ƿ����¼��ؾ�ƫ�⣨����IP���˿ڲ������������ã�
}RECTIFICATIONCALL,*PRECTIFICATIONCALL;

typedef BOOL (WINAPI *RECTIFICATIONCALLBACK)(PRECTIFICATIONCALL tCall);

typedef struct tagTRECTIFICATIONINIT//��ʼ��Rectficaion��ƫ��
{
	void* pDlg;				//������ָ��
	CString strProductName; //�䷽��
	LOGCALLBACK Log;		//д��־�ص�����
	int nProductType;		//��Ʒ����
	RECTIFICATIONCALLBACK RectficaionCall;//��ƫ����
	int nSN;		//��ƫ����� ��1��ʼ
}RECTIFICATIONINIT,*PRECTIFICATIONINIT;

