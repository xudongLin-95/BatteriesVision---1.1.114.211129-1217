#include "StdAfx.h"
#include "MesCommon.h"


CMesCommon::CMesCommon()
{
}


CMesCommon::~CMesCommon()
{
}


int CMesCommon:: ConnectToServer(CString IP, int Port, CString strProtocol)
{
	return true;
}

//��һ���֣���MES��������������
int CMesCommon:: Send_Heatbeat(tagMesHead mesHead)
{
	return true;
}

//�ڶ����֣�״̬���ݵĲɼ����ϴ�
int CMesCommon::Send_EquipmentStateData(tagMesHead mesHead) //��ʵ��
{
	return true;
}

//�������ֹ������ݲɼ�
int CMesCommon::Send_ProcessData1(tagMesHead mesHead)//�����������
{
	return true;
}
 //���Ĳ��ֱ������ݲɼ�
int CMesCommon::Send_EquipmentAlarmMessage(tagMesHead mesHead)
{
	return true;
}

//���岿���豸�䷽��������
int CMesCommon::Get_EquipmentRecipes(tagMesHead mesHead)
{
	return true;
}

//���������豸�����䷽����У��
int CMesCommon::Check_EquipmentRecipes(tagMesHead mesHead)
{
	return true;
}