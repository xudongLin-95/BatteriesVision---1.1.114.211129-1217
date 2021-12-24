#pragma once
#include "BaseTmpl.h"
#include <vector>
#include "SCIDataFlow.in.h"
#include "SCIRectifcation.in.h"
#include "SCIIO.in.h"
#include "Marking.in.h"
#include "UpsControl.in.h"

class CCommunicationTool : public CBaseTmpl
{
public:
	enum eCommType
	{
		IO = 0,					//IO通信
		PLC,					//PLC通信
		Rectifcation,			//纠偏通信
		MES,					//MES通信
		LightController,		//光源控制器串口通信
		Marking,				//喷码机通信
		UPS,					//UPS通信
		Temperature,			//温控器通信
	};
public:
	CCommunicationTool();
	~CCommunicationTool();

public:
	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//消息入队
	void Enqueue(eCommType eType,HTuple& MessageHandle);
	//通用调用接口
	int	DoSomething(eCommType eType,void *message,void *wParam,void *lParam);
	//调用参数对话框
	int ShowParamDlg(eCommType eType,void* wParam=NULL);
	//纠偏闭环
	int Rectify(const C_MergeWork& tOutput);
	//纠偏库回调
	static BOOL WINAPI RecificationCallBack(PRECTIFICATIONCALL tCall);

	//使能IO卡
	int EnableIOCard(const BOOL& bEnable=TRUE);
	//获取IO卡行频
	double GetIOCardLineRate();

	//心跳信号,设置运行状态
	BOOL RefreshRunStatus();
	//PLC重连，防止断开
	BOOL PLCReconnect();
	//发送PLC视觉启用信号
	BOOL PLCCCDEnable(BOOL bEnable);
	//发送PLC报警信号
	BOOL CCDAlarm();
	//发送PLC停机信号
	BOOL CCDStop();
	//判断PLC设备运行方向,-1反向，1正向，0停止
	int  GetPlcRunDirection();
	//判断PLC设备进退刀信号,1代表模头进刀，0模头退刀
	int  GetPlcDieHeadDirection();
	//获取PLC设备开始涂和停止涂信号,1涂，0停止涂
	int  GetPlcStartCoat();
	//获取PLC设备状态
	void  GetPlcEquipmentSignal();

	//喷码接口
	BOOL Mark(C_MergeWork& tOutput);

	//上传设备状态给Mes
	BOOL UploadDeviceInfoToMes();
private:
	void InitCommLibs();

	//发送三级纠偏闭环数据
	int SendCutRectifyValue(const C_MergeWork& tOutput);
	//发送二级纠偏闭环数据
	int SendAT9RectifyValue(const C_MergeWork& tOutput);

public:
	int m_nTempertatureNums;            //温度传感器的个数
	int m_nPlcRunDirection;				//PLC设备运行方向,-1反向，1正向, 0停止
	int m_nPlcDieHeadDirection;			//PLC设备进退刀信号,1代表模头进刀，0模头退刀
	int m_nPlcRunStart;					//PLC设备启动的信号，1启动，0停止
	int m_nPlcStartCoat;				//PLC设备涂箔材的信号，1启动，0停止
	double m_dPlcDieHeadPosition;		//PLC设备模头移动位置

private:
	DHInterface* m_pIO;					//IO通信模块
	DHInterface* m_pPLC;				//PLC通信模块
	//DHInterface* m_pRectification;	//纠偏通信模块
	DHInterface* m_pMES;				//MES通信模块
	DHInterface* m_pLightControl;		//光源控制器通信模块
	DHInterface* m_pMarkingControl;		//喷码通信模块
	DHInterface* m_pUPSControl;			//UPS通信模块
	DHInterface* m_pTemperature;		//温控器通信模块

};