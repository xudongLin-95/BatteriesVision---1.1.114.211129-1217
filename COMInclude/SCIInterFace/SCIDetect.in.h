#pragma once
#include "Log.in.h"
#include "AlgorithmBase.in.h"
#include "Product.in.h"
#include "SCIMeasure.in.h"

/*********************************�汾��2020-02-02***********************************
    ("INITIALIZE" == strMessage)//��ʼ��
	("ACTION" == strMessage)//���
	("SHOW" == strMessage)//���öԻ���
	��Seekersϵͳ���õ�ͷ�ļ��������������ṹ�����
/***********************************************************************/

const CString g_strErrorName[]={
	_T("OK"),
	_T("Ϳ�����NG"),
	_T("AT9�ߴ�NG"),
	_T("EA�ܳ�NG"),
	_T("�ܿ�NG"),
};

typedef struct tagTDETECTINIT//SCIDetect�㷨��̬���ʼ���ṹ��
{
	CString			strProductName;		//Ʒ������
	int				nSN;				//�㷨���,ͨ��Ϊһ��һ�����
	HTuple			hvCamSNList;		//һ���㷨����Ҫ����������ͼ��
	int				nProductType;		//��Ʒ���ͣ�һ����
	int				nCoatingType;		//Ϳ�����ͣ�����˫�棬����ֻ0���⣬˫��0��1�涼���
	BOOL			bHasBaseCoat;		//�Ƿ������Ϳ/����
	BOOL			bHasAT11;			//�Ƿ����AT9/AT11
	BOOL			bHasIntermittent;	//�Ƿ������Ъ����

	LOGCALLBACK		pLog;				//��־�ص�
}DetectInit, *PDetectInit;

typedef struct tagTDETECTINPUT//SCIDetect�㷨��̬������ṹ��
{
	HObject			hoGrabImage;		//�ɼ�����ͼƬ
	int				nMode;				//���ģʽ
}DetectInput, *PDetectInput;

typedef struct tagTMEASUREOUTPUT MeasureOut;
typedef struct tagTDETECTOUTPUT//SCIDetect�㷨��̬������ṹ��
{
	MeasureOut				tMeasure;			//�������

	HTuple					hv_ErrorType;		//��������
	HTuple					hv_StripeSN;		//���ڵڼ�������ÿһ��С����һ�����
	HObject					ho_ErrorRegion;		//�������� ����

	HObject					ho_Disp;			//��ʾ����

}DetectOutput, *PDetectOutput;

