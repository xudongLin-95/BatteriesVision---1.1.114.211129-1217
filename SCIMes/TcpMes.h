#pragma once
#include "MesCommon.h"
class CTcpMes : public CMesCommon
{
public:
	CTcpMes();

	~CTcpMes();
public:
	struct StateParamsIn //状态输入参数
	{
		CString Code;    //设备编码
		int State;       //状态值
		CString Datetime;//数据上传时间
		int StopReasonID;//设备停机原因
	};
	struct ProcessData1In//过程输入数据-常规数据
	{
		CString Code;         //设备编码
		int ProductionCount;  //
		int Speed;
		int UnWindTensionShow;
		CString Datetime;     //数据上传时间
	};
	struct ProcessData1Out//过程输出数据-常规数据
	{
		CString Code;
		int Ret;
	};

	struct AlarmIn        //报警输入参数
	{
		CString Code;
		CString AbnormalID;
		CString Abnormal;
		CString Datetime;
	};

	struct RecipesIn     //获取配方输入数据
	{
		CString Code;
		CString Workorder;
		CString Datetime;
	};
	struct RecipesOut    //获取配方输出数据
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

	struct CheckRecipeIn  //开机检查配方输入参数
	{
		CString Code;
		CString Workorder;
		CString Version;
	};
	struct CheckRecipeOut //开机检查配方输出参数
	{
		int Ret;//0
		CString Reason;//
	};

private:
	HTuple m_hvQueue;//队列句柄
	HTuple m_hv_SocketHandle;//套接字连接句柄
	BOOL m_bConnected;//连接状态

	static CTcpMes* m_instance; //静态实例
	CRITICAL_SECTION m_csSoap;//保护锁

public:
	static CTcpMes& getInstance();//单例模式
	
public:
	virtual int ConnectToServer(CString IP, int Port, CString strProtocol = "TCP");

	//第一部分：向MES服务器发送心跳
	virtual int Send_Heatbeat(tagMesHead mesHead);
	bool DoHeartBeat(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//第二部分：状态数据的采集及上传
	virtual int Send_EquipmentStateData(tagMesHead mesHead); //待实现
	bool DoEquipmentStateData(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//第三部分过程数据采集
	virtual int Send_ProcessData1(tagMesHead mesHead);//常规过程数据
	bool DoProcessData1(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//int Send_ProcessData2();//厚度监测数据
	//int Send_ProcessData3();//照片文件

	//第四部分报警数据采集
	virtual int Send_EquipmentAlarmMessage(tagMesHead mesHead);
	bool DoEquipmentAlarmMessage(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//第五部分设备配方数据请求
	virtual int Get_EquipmentRecipes(tagMesHead mesHead);
	bool DoEquipmentRecipes(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	//第六部分设备开机配方参数校验
	virtual int Check_EquipmentRecipes(tagMesHead mesHead);
	bool DoCheckEquipmentRecipes(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

};

