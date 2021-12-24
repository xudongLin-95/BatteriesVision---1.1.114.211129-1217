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

//第一部分：向MES服务器发送心跳
int CMesCommon:: Send_Heatbeat(tagMesHead mesHead)
{
	return true;
}

//第二部分：状态数据的采集及上传
int CMesCommon::Send_EquipmentStateData(tagMesHead mesHead) //待实现
{
	return true;
}

//第三部分过程数据采集
int CMesCommon::Send_ProcessData1(tagMesHead mesHead)//常规过程数据
{
	return true;
}
 //第四部分报警数据采集
int CMesCommon::Send_EquipmentAlarmMessage(tagMesHead mesHead)
{
	return true;
}

//第五部分设备配方数据请求
int CMesCommon::Get_EquipmentRecipes(tagMesHead mesHead)
{
	return true;
}

//第六部分设备开机配方参数校验
int CMesCommon::Check_EquipmentRecipes(tagMesHead mesHead)
{
	return true;
}