#pragma once

#include "DH_HalconEx.h"
#include "SCIDetect.in.h"
#include "SCIMeasure.in.h"
#include "SCIPowerLimit.in.h"
#include "SCIIO.in.h"
#include "SCIDataFlow.in.h"
#include "SCIPLC.in.h"
#include <vector>
#include "Log.in.h"
#include "FinsTCPExt.h"
#include "ModbusTcpExt.h"
#include "AdsExt.h"
#include "Language.h"

class CPLC : DHInterface
{
public:
	enum eUsePlcType
	{
		None = 0,	//不使用
		FinsTCP,	//Fins协议
		ModbusTCP,	//Modbus协议
		AdsTCP,		//Ads协议
	};
	//纠偏类型枚举
	enum eUseCorrectType
	{
		DoubleSide=0,	//双面
		SingleSide,		//单面
	};
	//滤波类型枚举
	enum eFilterType
	{
		NoneFilter = 0,	//不滤波
		MeanFilter,		//均值滤波
		MedianFilter,	//中值滤波
	};
	//闭环逻辑类型
	enum eCorrectLogicType
	{
		StdCorrect=0,	//标准差
		MeanCorrect,	//均值
	};
	typedef struct tagTPLCSET
	{
		int nUsePlc;//是否使用与PLC通信 0：不使用，1：Fins，2：Modbus，3：Ads
		int nUseCorrect;//使用使用纠偏
		int nCorrectType;//启用单面/双面纠偏(0:双面;1:单面)
		CString strPLCIP;//PLC的IP地址
		int nPLCPort;//PLC的端口
		int nSingleCorrectAddress;//单面纠偏地址
		int nDoubleCorrectAddress;//双面纠偏地址
		int nDoCorrectAddress;//执行纠偏信号地址，嘉拓设备用
		int nPlcHeartBeatAddress;//PLC心跳地址
		int nPlcBatchNameAddress;//PLC膜卷号地址
		int nPlcBatchNameVerifyAddress;//PLC膜卷号写入验证地址
		int nPlcCCDEnableAddress;//PLC视觉启用地址
		int nPlcRunDirectionAddress;//PLC设备运行方向标志地址
		int nPlcDieHeadDirectionAddress;//PLC设备模头进退刀标志地址
		int nPlcStartCoatAddress;//PLC设备模头起涂标志地址
		int nPlcDieHeadPosAddress;//PLC设备模头位置地址
		int nPlcAlarmAddress;//PLC设备模头报警地址

		double dAlertDiff;//最大纠偏值mm
		int nFilterType;//数据滤波类型 0不滤波 1均值滤波 2中值滤波
		int nFilterSize;//数据滤波尺寸
		int nCorrectDirection;//发送值的方向，0左正右负，1左负右正
		double dCorrectPercent;	//单次纠偏比例
		double dCamDistance;//模头到相机延迟距离
		double dCorrectInterval;//短纠偏间距
		double dCorrectIntervalLong;//长纠偏间距
		double dRowResolution; //行分辨率
		int nCorrectLogicType;//闭环逻辑类型 0标准差 1均值
		BOOL bJudgeGuidFinish;//是否判断纠偏执行到位
		int nGuidFinishAddr;//判断纠偏执行到位的地址
	}TPLCSET, *PTPLCSET;
	enum eCCDType
	{
		SizeCCD = 0,
		SurfaceCCD,
	};
	enum eProductType
	{
		PT_Winding = 1,		//多极耳
		PT_Slit,			//狭缝
		PT_Interval,		//间歇涂布
	};
	enum eParamIndex
	{
		PI_FlawCCD_CountClean = 0,	//瑕疵计数清零标志位
		PI_FlawCCD_Rechange,		//瑕疵换卷标志位
		PI_MyEquipment_RunStatus,	//设备运行状态
		PI_FlawCCD_RunStatus,		//瑕疵运行信号
		PI_FlawCCD_Alarm,			//瑕疵异常报警信号
		PI_SizeCCD_CountClean,		//尺寸计数清零
		PI_SizeCCD_Rechange,		//尺寸换卷
		PI_SizeCCD_RunStatus,		//尺寸运行信号
		PI_SizeCCD_Alarm,			//尺寸异常
		PI_Rewinding_EACount,		//EA计数器
		PI_Roll_Number,				//卷号
		PI_ProductType,
		PI_RunMode,
		PI_TabNum,
		PI_CoatWidth,
		PI_TabDistace,
		PI_EALength,
		PI_AT9Width,
		PI_TabWidth,
		PI_TabHeight,
		PI_CoatWidthEAStart,
		PI_CoatWidthEAEnd,
		PI_MaterialHeight,
		PI_SlitWidth,
		PI_SlitDist,
		PI_SlitHeight,
		PI_SlitNOTabDist,
		PI_RewindingUpMARK_Number,	//上卷mark计数器
		PI_RewindingDownMARK_Number,//下卷mark计数器
		PI_RewindingMARK_Clean,		//mark清零标志位
		PI_RewindingUpCCW_Length,	//上收卷反转的长度(mm)
		PI_RewindingDownCCW_Length,	//下收卷反转的长度(mm)
		PI_Upper_EA_Number,			//EA序号
		PI_Upper_Coatingr_width,	//
		PI_Upper_CellEar_Distance,
		PI_Upper_CellEar_Width,
		PI_Upper_CellEar_Height,
		PI_Upper_EA_Length,
		PI_Upper_Picture_Saving,
		PI_Upper_Read_Finish,
		PI_Lower_EA_Number,
		PI_Lower_Coatingr_width,
		PI_Lower_CellEar_Distance,
		PI_Lower_CellEar_Width,
		PI_Lower_CellEar_Height,
		PI_Lower_EA_Length,
		PI_Lower_Picture_Saving,
		PI_Lower_Read_Finish,
		PI_Width_Picture_Saving,
		PI_Upper_Flaw_EA_Number,
		PI_Upper_Detect_Result,
		PI_Upper_Flaw_Picture_Saving,
		PI_Upper_Flaw_Read_Finish,
		PI_Lower_Flaw_EA_Length,
		PI_Lower_Detect_Result,
		PI_Lower_Flaw_Picture_Saving,
		PI_Lower_Flaw_Read_Finish,
	};
public:
	CPLC(void);
	virtual ~CPLC(void);
	virtual int DoSomething(void* message, void* wParam, void* lParam);

	void ReadParam();
	void WriteParam();

	PLCINIT GetPlcInit() { return m_sInit; }

	TPLCSET m_tPLCSet;
	CFinsTcpExt* m_pFinsTcp;
	CModbusTcpExt*m_pMdbsTcp;
	CAdsExt*m_pAdsTcp;
	CBCGPDialog* m_pDlg;//主界面指针
	CLanguage m_Lang;
private:
	PLCINIT m_sInit;
	HTuple m_hvQueue;		//队列句柄
	CString m_strConfigPLCFile;
	CString m_strConfigCalibrationFile;

	HTupleVector m_hvec_Data;
	double m_dCorrectValue;//闭环值
	double m_dDiffValue;//差值

private:
	//PLC线程
	static void* HThreadPlc(void *hcthread);
	//PLC线程
	void plc_thread();

	int Init(PLCINIT &Init);

	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);

	int PushMessage(const HTuple& hvMessageSend);//放置消息进入队列

	INT32 TransInt(double dIn, int nDecimal);//double转换成int,小数点后几位取整

	//调用参数对话框
	BOOL ShowParamDlg();

	BOOL ReadInt32(const int& nDMAddr, int& nReadVal);

	BOOL WriteInt32(const int& nDMAddr, const int& nWriteVal);

	BOOL ReadString(const int& nDMAddrStart, CString& strReadVal);

	BOOL ReadInt32Batch(const int& nDMAddrStart, const int& nReadNum, std::vector<int>& vecReadVal);

	BOOL ReadFloat32(const int& nDMAddr, float& fReadVal);

	BOOL WriteFloat32(const int& nDMAddr, const float& fWriteVal);

	//纠偏逻辑
	BOOL Correcting(C_MergeWork &Output);

	//计算闭环纠偏,标准差
	BOOL CalCorrect(HTuple hvOriginal, double& dDstVal);

	//数据滤波
	double FilterData(const int& nIndex, const double& dData);
	//向PLC发送心跳信号
	int SendHeartBeat(const int& nStatus);
	//向PLC发送视觉启用信号
	int SendCCDEnable(const int& nStatus);
	//向PLC发送报警信号
	int SendAlarm(const int& nAlarmCode);
	//获取PLC设备运行方向
	int GetPLCRunDirection(int& nDirection);
	//获取PLC设备模头进退刀方向
	int GetPLCDieHeadDirection(int& nDirection);
	//获取PLC起涂信号
	int GetPLCStartCoat(int& nStartCoat);
	//获取PLC起涂信号
	int GetPLCDieHeadPosition(double& dPos);

	//获取闭环值
	BOOL GetCorrectVal(double& bVal) 
	{ 
		bVal = m_dCorrectValue; 
		return TRUE;
		if (m_tPLCSet.nUsePlc!=0 && m_pFinsTcp->IsConnected() && m_tPLCSet.nUseCorrect==TRUE)
			return TRUE;
		else
			return FALSE; 
	}
	//获取计算的差值
	BOOL GetCorrectDiffVal(double& bVal)
	{
		bVal = m_dDiffValue;
		return TRUE;
		if (m_tPLCSet.nUsePlc != 0 && m_pFinsTcp->IsConnected() && m_tPLCSet.nUseCorrect == TRUE)
			return TRUE;
		else
			return FALSE;
	}

	//获取膜卷号
	BOOL GetBatchName(CString& strBatch);

	//写入膜卷号，用于设备三方校验膜卷
	BOOL WriteBatchNameVerify(CString& strBatch);

	//发送单面闭环值
	BOOL _SendCorrectValue1(double& dValue);

	//发送AB面闭环值
	BOOL _SendCorrectValue2(double& dValue);

	//判断模头是否移动到位
	BOOL _JudgeGuideFinish();

	//计算纠偏值，包含可选择单双面，均值和标准差
	BOOL _CalCorrectDiff(C_MergeWork &Output, HTuple& hvDiff);

	//判断数据是否波动大
	BOOL _IsDataDeviationBig(const int& nIndex, const double& dStdevLimit);
};

extern CPLC* g_This;