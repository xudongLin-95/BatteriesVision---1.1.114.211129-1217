#pragma once
#include "stdafx.h"
#include "Log.in.h"

typedef struct tagTIOINIT//��ʼ��ͨ����
{
	CString strProductName;	//�䷽��
	LOGCALLBACK Log;		//д��־�ص�����
	int nProductType;		//��Ʒ����
}IOINIT,*PIOINIT;



