/*********************************�汾��2021-01-18***********************************
Measure.in.h �����࣬ʵ��Ϳ������Լ�Ϳ����ؼ��
("Init" == strMessage)//��ʼ��
("Action" == strMessage)//���
("Show" == strMessage)//���öԻ���
/***********************************************************************/

#pragma once
#include "stdafx.h"
#include "Log.in.h"
#include "callback.in.h"
#include "AlgorithmBase.in.h"
#include "Product.in.h"

typedef struct tagTMEASUREINIT
{
	int					nSN;				//�㷨���,ͨ��Ϊһ��һ�����
	HTuple				hvCamSNList;		//һ���㷨����Ҫ����������ͼ��
	tagPDInfo			tProductInfo;		//��Ʒ��Ϣ

	LOGCALLBACK		pLog;					//��־�ص�
	MAINCALLBACK	pMain;					//��ϵͳ�ص�
}MeasureInit, *PMeasureInit;

typedef struct tagTMEASUREINPUT
{
	HObject		hoGrabImage;	//����ͼ������Ϊ�˵��Է���
	int			nMode;			//���ģʽ
}MeasureInput, *PMeasureInput;

typedef struct tagTBaseSize
{
	HTuple		hv_StripeSN;			//�ߴ�������������ÿһ��С����һ�����
	HTuple		hv_Width;				//���в����ߴ�
	HTuple		hv_WidthOrigin;			//����ԭʼ�����ߴ�,��������
	HTuple		hv_MisalignedDist;		//��λ����
	HTuple		hv_WholeBaseCoat;		//����ȫ�ߴ�
	HTuple		hv_WholeWidth;			//���ĳߴ�
}BaseSize, *PBaseSize;

typedef struct tagTSingleSize
{
	HTuple		hv_StripeSN;			//������ţ����п�ȷֱ��Ӧ�ڼ�������
	HTuple		hv_Width;				//���������������п��
	HObject		ho_Region;				//��������������������
}SingleSize, *PSingleTypeSize;

typedef struct tagTRect
{
	TPoint		tLeftTop;				//���Ͻ����
	TPoint		tRightBottom;			//���½�����
}TRect,*PTRect;

typedef struct tagTFeature
{
	HTuple		hvWidth;				//���
	HTuple		hvHeight;				//�߶�
	HTuple		hvDiameter;				//ֱ��
	HTuple		hvArea;					//���
	TPoint		tCenter;				//�����������꣬�����ͼ�����Ͻ�
	TRect		tRect;					//������Ӿ����������꣬�����ͼ�����Ͻ�
}Feature, *PFeature;

typedef struct tagTRegionResult
{
	HTuple		hv_Type;			//��������
	HTuple		hv_StripeSN;		//������ţ�ÿһ��С����һ�����
	HObject		ho_Region;			//�������������ͺͷ�����Ŷ�Ӧ
	Feature		tFeature;			//ȱ������������Ԫ�س������������ͺͷ�����Ŷ�Ӧ
}RegionResult, *PRegionResult;

typedef struct tagTMEASUREOUTPUT
{
	HTuple			hv_HasCoat;				//�Ƿ���Ϳ�� TRUE��FALSE
	HTuple			hv_HasIntermittent;		//�Ƿ��м�Ъ���� TRUE��FALSE

	HTuple			hv_LineNum;				//����������
	Line			tLine;					//����������,��������������

	BaseSize		tSize;					//���������ߴ�

	SingleSize		tCoat;					//����ͿĤ �ߴ������
	SingleSize		tFoil;					//���м��� �ߴ������
	SingleSize		tAT11;					//�����մ�/AT9/AT11 �ߴ������
	SingleSize		tBaseCoat;				//���е�Ϳ/����/Ϳ̼ �ߴ������
	SingleSize		tIntermittent;			//���м�Ъ �ߴ������

	SingleSize		tSpotInspection;		//�����
	HObject         ho_DispImage;           //��ʾ��ͼ��ƴ�Ӻ��ͼ���ӽ����ֱ��ʹ��
	HObject			ho_Disp;				//������������ʾ����

	RegionResult	tDefect;				//覴ü����
}MeasureOutput, *PMeasureOutput;