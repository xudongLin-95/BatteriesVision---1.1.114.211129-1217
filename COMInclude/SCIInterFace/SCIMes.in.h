#pragma once
#include "stdafx.h"
#include "Log.in.h"
#include "Product.in.h"
#include "CallBack.in.h"

typedef struct tagTMESINIT//��ʼ��ͨ����
{
	LOGCALLBACK		Log;		//д��־�ص�����
	MAINCALLBACK	Main;		//��ϵͳ�ص�����
	tagPDInfo		tPdInfo;	//ϵͳ����
}MESINIT,*PMESINIT;

struct tagMesDeviceInfo
{
	//�豸��Ϣ
	CString strDeviceCode;				//�豸��� Coater_CCD,
	CString strDeviceStatus;            //�豸״̬ Run    Alarm    Idle,
	CString	strReserve1;				//��ע1
	CString	strReserve2;				//��ע2
};

struct tagMesProductInfo
{
	CString strBatch;					//ĸ������
	CString strUserID;					//����Ա
	CString strShift;					//���
	CString	strPasteCode;				//���Ϻ�
	CString	strProductType;				//Ʒ��
	double	dCoatStd;					//Ϳ�����
	double	dFoilStd;					//�������
};

struct tagMesHead
{
	//MES����ͷ����
	CString m_ProtocolNO;   //:"1110",
	int m_MsgNO;            // : 8999,
	int m_NeedReply;        // : 1,
	CString m_CmdID;        //: "1701B",
	CString m_EID;          // "010920161010000000001"
};

struct tagParametricData
{
	CString name;       //����������
	CString dateType;   //��������
	CString value;      //��ֵ
	CString describe;   //����
};

struct tagNcCode
{
	CString ncCode;     //NG���
	bool    isNc;       //�Ƿ�NG
	CString value;      //��ֵ(�����豸)
	CString describe;   //NG����
};


