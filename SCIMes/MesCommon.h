#pragma once
class CMesCommon
{
public:
	CMesCommon();
	~CMesCommon();

public:
	//MES����������ܷ���(���ݲ�ͬ��ͨѶЭ��ѡ��)
	//ͨѶ��������
	virtual int ConnectToServer(CString IP, int Port, CString strProtocol = "TCP");

	//��һ���֣���MES��������������
	virtual int Send_Heatbeat(tagMesHead mesHead);

	//�ڶ����֣�״̬���ݵĲɼ����ϴ�
	virtual int Send_EquipmentStateData(tagMesHead mesHead); //��ʵ��

	//�������ֹ������ݲɼ�
	virtual int Send_ProcessData1(tagMesHead mesHead);//�����������
	//int Send_ProcessData2();//��ȼ������
	//int Send_ProcessData3();//��Ƭ�ļ�

	//���Ĳ��ֱ������ݲɼ�
	virtual int Send_EquipmentAlarmMessage(tagMesHead mesHead);

	//���岿���豸�䷽��������
	virtual int Get_EquipmentRecipes(tagMesHead mesHead);

	//���������豸�����䷽����У��
	virtual int Check_EquipmentRecipes(tagMesHead mesHead);
};

