#pragma once
#include "stdafx.h"
#include "Log.in.h"
#include "Product.in.h"
#include "CallBack.in.h"
typedef struct tagTPLCINIT//初始化PLC通信类
{
	tagPDInfo	tPdInfo;	//产品信息
	MAINCALLBACK Main;		//主系统回调函数
	LOGCALLBACK Log;		//写日志回调函数
}PLCINIT, *PPLCINIT;

//////////////////////////////////////////////////////////////////////////
///TwinCAT Ads 
//////////////////////////////////////////////////////////////////////////
enum eMemoryType
{
	MT_M = 0x00004020,		//M寄存器（用％M声明）
	MT_MX = 0x00004021,		//MX寄存器（用MX声明）
	MT_I = 0x0000F020,		//I寄存器（用％I声明）
	MT_IX = 0x0000F021,		//IX寄存器（用IX声明）
	MT_Q = 0x0000F030,		//Q寄存器（用％Q声明）
	MT_QX = 0x0000F031,		//QX寄存器（用QX声明）
};


typedef struct tagTAdsSigleInput//读写Ads地址的值的输入结构体
{
	unsigned long lIndexGroup;		//地址
	unsigned long lIndexOffset;		//偏移
};

typedef struct tagTAdsArrInput//读写Ads数组的输入结构体
{
	CString strKey;		//数组的标签名称
	int nNum;			//数组元素个数
};

typedef struct tagTPROCESSPARAM
{
	int					nProductType;		//产品类型 1表示多极耳 2表示狭缝 3表示间歇涂布
	int					nRunMode;			//设备运行模式
	int					nTabNum;			//极耳数目
	double				dCoatWidth;			//涂布标准宽
	std::vector<double> vecTabDistance;		//极耳间距
	double				dEALength;			//EA总长
	double				dAT9Width;			//AT9宽
	double				dTabWidth;			//极耳宽
	double				dTabHeight;			//极耳高
	double				dCoatWidthEAStart;	//EA起始涂布宽
	double				dCoatWidthEAEnd;	//EA结束涂布宽
	double				dMaterialHeight;	//定位不齐、极耳待料值
	double				dSlitWidth;			//狭缝宽
	std::vector<double> vecSlitDistance;	//狭缝间距
	double				dSlitHeight;		//狭缝高
	double				dSlitNoTabDistance;	//狭缝直线边距离
}PROCESSPARAM, *PPROCESSPARAM;

typedef struct tagTSizeCCDResult//尺寸检测结果
{
	int					nType;			//0上带路，1下带路
	int					nEASN;			//EA序号
	std::vector<double>	vecCoatWidth;	//涂布宽
	std::vector<double>	vecTabDist;		//极耳间距
	std::vector<double>	vecTabWidth;	//极耳宽
	std::vector<double>	vecTabHeight;	//极耳高
	double				dEALength;		//EA长
	std::vector<CString>strImagePath;	//图像存储路径
	BOOL				bReadFlag;		//读取标志位
	std::vector<CString>strBackCamImagePath;//后相机图像存储路径
};

typedef struct tagTSurfaceCCDResult//瑕疵CCD检测结果
{
	int		nUpEASN;		//上带路EA
	int		nUpResult;		//上带路检测结果，NG：0  OK：1,PLC读取后置为-1
	CString	strImage0Path;	//前相机图片路径
	int		nUpReadFlag;	//上带路读取标志位，CCD在该地址为0时写入EA信息，CCD写入完成置1，PLC监听该地址为1时PLC读取EA信息，读取完成，将该地址置为0
	int		nLowEASN;		//下带路EA
	int		nLowResult;		//下带路检测结果，NG：0  OK：1,PLC读取后置为-1
	CString	strImage1Path;	//后相机图片路径
	int		nLowReadFlag;	//下带路读取标志位，CCD在该地址为0时写入EA信息，CCD写入完成置1，PLC监听该地址为1时PLC读取EA信息，读取完成，将该地址置为0
};