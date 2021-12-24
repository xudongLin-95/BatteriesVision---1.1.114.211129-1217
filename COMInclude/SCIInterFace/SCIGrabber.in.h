#if !defined(DHGrabber_h_)
#define DHGrabber_h_
const int GBMaxFileNameLen = 255;
const int GBMaxTextLen = 255;
enum eCameraLinkConfigration
{
	CL_Base=0,	//Base模式
	CL_Medium,	//Medium模式
	CL_Full,	//Full模式
};
//回调信息结构
typedef struct _GBSignalInfoStruct
{
	PVOID Context;								//存放拥有回调函数的对象的this指针
	int iGrabberTypeSN;							// 采集卡类型编号
	int nGrabberSN;								//采集卡序号
	int nErrorCode;								//错误代码（0：正常）
	int nFrameCount;							//已采集的帧记数
	char strDescription[GBMaxTextLen];			//描述信息
	int iReserve1;								// 备用字段1
	int iReserve2;								// 备用字段2	
}s_GBSIGNALINFO;//回调信息结构
//回调函数指针声明
typedef void (WINAPI *PGBCALLBACK)(const s_GBSIGNALINFO* SignalInfo);
//采集卡初始化结构
typedef struct _GBInitStruct
{
	int iGrabberTypeSN;						// 采集卡类型编号
	int nGrabberSN;							//采集卡序号
    char strDeviceName[GBMaxTextLen];		//设备名称
	char strDeviceMark[GBMaxTextLen];		// 设备标识
	char strGrabberFile[GBMaxFileNameLen];	//用于初始化的文件
	PGBCALLBACK CallBackFunc;				//回调函数指针
	PVOID Context;							//存放拥有回调函数的对象的this指针
	int iReserve1;							// 备用字段1，用于传入CameraLink模式：Base、Medium、Full
	int iReserve2;							// 备用字段2
}s_GBINITSTRUCT;//采集卡初始化结构
//错误信息结构
typedef struct _GBErrorInfoStruct
{
   	int nErrorCode;							//错误代码（0：正常）
	char strErrorDescription[GBMaxTextLen];	//错误描述
	char strErrorRemark[GBMaxTextLen];		//附加信息
}s_GBERRORINFO;//错误信息结构
//公共参数
enum GBParamID
{
	GBImageWidth = 0,		//图像宽度 (单位:字节)
	GBImageHeight,			//图像高度 (单位:字节)
	GBImagePixelSize,		//图像象素大小 (单位:字节)
	GBImageBufferSize,		//图像缓冲区大小 (单位:字节)
	GBImageBufferAddr,		//图像缓冲区地址
	GBGrabberTypeSN			//采集卡类型编号
};
//错误代码定义
const int GBOK = 0;
#endif// !defined(DHGrabber_h_)
