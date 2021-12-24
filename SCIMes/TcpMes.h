#pragma once
#include "MesCommon.h"
class CTcpMes : public CMesCommon
{
public:
	CTcpMes();

	~CTcpMes();
public:
	struct StateParamsIn //״̬�������
	{
		CString Code;    //�豸����
		int State;       //״ֵ̬
		CString Datetime;//�����ϴ�ʱ��
		int StopReasonID;//�豸ͣ��ԭ��
	};
	struct ProcessData1In//������������-��������
	{
		CString Code;         //�豸����
		int ProductionCount;  //
		int Speed;
		int UnWindTensionShow;
		CString Datetime;     //�����ϴ�ʱ��
	};
	struct ProcessData1Out//�����������-��������
	{
		CString Code;
		int Ret;
	};

	struct AlarmIn        //�����������
	{
		CString Code;
		CString AbnormalID;
		CString Abnormal;
		CString Datetime;
	};

	struct RecipesIn     //��ȡ�䷽��������
	{
		CString Code;
		CString Workorder;
		CString Datetime;
	};
	struct RecipesOut    //��ȡ�䷽�������
	{
		CString Version;
		CString PamarnameSpeed;
		int StandardvalueSpeed;
		int UppervalueSpeed;
		int LowervalueSpeed;
		CString PamarnamePressure;
		int StandardvaluePressure;
		int UppervaluePressure;
		int LowervaluePressure;
	};

	struct CheckRecipeIn  //��������䷽�������
	{
		CString Code;
		CString Workorder;
		CString Version;
	};
	struct CheckRecipeOut //��������䷽�������
	{
		int Ret;//0
		CString Reason;//
	};

private:
	HTuple m_hvQueue;//���о��
	HTuple m_hv_SocketHandle;//�׽������Ӿ��
	BOOL m_bConnected;//����״̬

	static CTcpMes* m_instance; //��̬ʵ��
	CRITICAL_SECTION m_csSoap;//������

public:
	static CTcpMes& getInstance();//����ģʽ
	
public:
	virtual int ConnectToServer(CString IP, int Port, CString strProtocol = "TCP");

	//��һ���֣���MES��������������
	virtual int Send_Heatbeat(tagMesHead mesHead);
	bool DoHeartBeat(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//�ڶ����֣�״̬���ݵĲɼ����ϴ�
	virtual int Send_EquipmentStateData(tagMesHead mesHead); //��ʵ��
	bool DoEquipmentStateData(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//�������ֹ������ݲɼ�
	virtual int Send_ProcessData1(tagMesHead mesHead);//�����������
	bool DoProcessData1(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//int Send_ProcessData2();//��ȼ������
	//int Send_ProcessData3();//��Ƭ�ļ�

	//���Ĳ��ֱ������ݲɼ�
	virtual int Send_EquipmentAlarmMessage(tagMesHead mesHead);
	bool DoEquipmentAlarmMessage(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//���岿���豸�䷽��������
	virtual int Get_EquipmentRecipes(tagMesHead mesHead);
	bool DoEquipmentRecipes(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//���������豸�����䷽����У��
	virtual int Check_EquipmentRecipes(tagMesHead mesHead);
	bool DoCheckEquipmentRecipes(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

};

