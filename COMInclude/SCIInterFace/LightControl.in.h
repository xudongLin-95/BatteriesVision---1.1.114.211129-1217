#pragma once
#include "stdafx.h"
#include "Log.in.h"

typedef struct tagTLIGHTINIT//��ʼ����Դ������
{
	CString strProductName; //�䷽��
	LOGCALLBACK Log;		//д��־�ص�����
	int nProductType;		//��Ʒ����
	
}LIGHTINIT,*PLIGHTINIT;

enum eLightControlType	//��Դ�������ͺ�
{
	DH_HAPS_48W800_1T = 0,
	DH_HAPS_48W800_2T,
};

