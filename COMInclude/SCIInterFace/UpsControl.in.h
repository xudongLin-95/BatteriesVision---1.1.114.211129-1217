#pragma once
#include "stdafx.h"
#include "Product.in.h"
#include "Log.in.h"
#include "CallBack.in.h"

typedef struct tagTUPSINIT//��ʼ��
{
	tagPDInfo		tPdInfo;	//��Ʒ����
	LOGCALLBACK		Log;		//д��־�ص�����
	MAINCALLBACK	Main;		//��ϵͳ�ص�����
}UPSINIT,*PUPSINIT;



