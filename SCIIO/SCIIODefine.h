#pragma once
#include "stdafx.h"
#include <vector>
#define PIO24_PORTS_MAX 32
#define PIO24_COUNT 1

enum e_AlarmStatus
{	
	ALARM_RED = 0,  //���
	ALARM_GREEN,    //�̵�
	ALARM_YELLOW,   //�Ƶ�
};

enum e_CoatType
{
	Piece_UnCut,//��Ƭδ��Ƭ
	Piece_Cut,//��Ƭ����Ƭ
	Stripe,//���� ������
	Pattern,//��� ������
};
enum e_RejectType
{
	REJECT_DEFULT,
	REJECT_NONE
};
typedef struct tagTALARMS//����������
{
	BOOL		bEffective;//�Ƿ���Ч
	int			nMaxContinuesError;//���������
	int			nCurrentContinuesError;//��ǰ�����������
	BOOL        bUseBuzzer;//ʹ�÷�����
}ALARMS,*PALARMS;
typedef struct tagTCOMMUICATEPARAM
{
	int			nPulseWidth;//����ź�������
	int			nRejectType;//�޷����� ���Ϊ1�Ǽ���޷� ���Ϊ2Ϊ�����޷� Ĭ��Ϊ0 ���ݽ���û������޷�
	int			nReadyPort;//Ready�ź�
	int			nAlarmPort;//������������������������Ժ����źŵĶ˿ں�

	int        nATotalDistance;//A����ܾ���
	int        nBTotalDistance;//B����ܾ���
	int        nAFrameNum;//A����ӳ�֡��
	int        nBFrameNum;//B����ӳ�֡��
	int        nSingleImageHeight;//���Ųɼ��߶�


	int			nMarkRowLabelOffset;//�����Markλ�õ�����λ��(��λ��)
	double		dMarkerLengthMM;//������ĳ��ȣ�mm��

	int        nAlarmGreenLight;//�̵Ʊ������IO��ţ�Ĭ��out0
	int        nAlarmRedLight;//��Ʊ������IO��ţ�Ĭ��out1
	int        nAlarmBuzzer;//�������������IO��ţ�Ĭ��out3
	int        nAlarmHeatBeat;//��������IO��ţ�Ĭ��out7
	int        nAlarmYellowLight;//�Ƶ�IO
}COMMUICATEPARAM,*PCOMMUICATEPARAM;
typedef struct tagTCOMMONPARAM
{
	e_CoatType  emCoatType;//�豸����
	double		dRowResolution[2];//����ֱ��ʣ�0 ���0��1 ���1
	double		dColumnResolution[2];//����ֱ��ʣ�0 ���0��1 ���1
}COMMONPARAM,*PCOMMONPARAM;


