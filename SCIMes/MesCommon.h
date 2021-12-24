#pragma once
class CMesCommon
{
public:
	CMesCommon();
	~CMesCommon();

public:
	//MES交互需求汇总分析(根据不同的通讯协议选择)
	//通讯建立连接
	virtual int ConnectToServer(CString IP, int Port, CString strProtocol = "TCP");

	//第一部分：向MES服务器发送心跳
	virtual int Send_Heatbeat(tagMesHead mesHead);

	//第二部分：状态数据的采集及上传
	virtual int Send_EquipmentStateData(tagMesHead mesHead); //待实现

	//第三部分过程数据采集
	virtual int Send_ProcessData1(tagMesHead mesHead);//常规过程数据
	//int Send_ProcessData2();//厚度监测数据
	//int Send_ProcessData3();//照片文件

	//第四部分报警数据采集
	virtual int Send_EquipmentAlarmMessage(tagMesHead mesHead);

	//第五部分设备配方数据请求
	virtual int Get_EquipmentRecipes(tagMesHead mesHead);

	//第六部分设备开机配方参数校验
	virtual int Check_EquipmentRecipes(tagMesHead mesHead);
};

