#pragma once
#include "stdafx.h"
#include <vector>
#define PIO24_PORTS_MAX 32
#define PIO24_COUNT 1

enum e_AlarmStatus
{	
	ALARM_RED = 0,  //红灯
	ALARM_GREEN,    //绿灯
	ALARM_YELLOW,   //黄灯
};

enum e_CoatType
{
	Piece_UnCut,//单片未切片
	Piece_Cut,//单片已切片
	Stripe,//条纹 分条机
	Pattern,//间隔 分条机
};
enum e_RejectType
{
	REJECT_DEFULT,
	REJECT_NONE
};
typedef struct tagTALARMS//报警灯响起
{
	BOOL		bEffective;//是否生效
	int			nMaxContinuesError;//最大错误个数
	int			nCurrentContinuesError;//当前连续错误个数
	BOOL        bUseBuzzer;//使用蜂鸣器
}ALARMS,*PALARMS;
typedef struct tagTCOMMUICATEPARAM
{
	int			nPulseWidth;//输出信号脉冲宽度
	int			nRejectType;//剔废类型 如果为1是间隔剔废 如果为2为连续剔废 默认为0 根据结果好坏进行剔废
	int			nReadyPort;//Ready信号
	int			nAlarmPort;//当发现连续错误大于最大个数以后发送信号的端口号

	int        nATotalDistance;//A标机总距离
	int        nBTotalDistance;//B标机总距离
	int        nAFrameNum;//A标机延迟帧数
	int        nBFrameNum;//B标机延迟帧数
	int        nSingleImageHeight;//单张采集高度


	int			nMarkRowLabelOffset;//相对于Mark位置的贴标位置(单位行)
	double		dMarkerLengthMM;//打标机标的长度（mm）

	int        nAlarmGreenLight;//绿灯报警输出IO编号，默认out0
	int        nAlarmRedLight;//红灯报警输出IO编号，默认out1
	int        nAlarmBuzzer;//蜂鸣器报警输出IO编号，默认out3
	int        nAlarmHeatBeat;//报警心跳IO编号，默认out7
	int        nAlarmYellowLight;//黄灯IO
}COMMUICATEPARAM,*PCOMMUICATEPARAM;
typedef struct tagTCOMMONPARAM
{
	e_CoatType  emCoatType;//设备类型
	double		dRowResolution[2];//纵向分辨率，0 相机0，1 相机1
	double		dColumnResolution[2];//横向分辨率，0 相机0，1 相机1
}COMMONPARAM,*PCOMMONPARAM;


