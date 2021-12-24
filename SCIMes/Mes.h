#pragma once
#include "SCIMes.in.h"
#include "Record.h"
#include "include/json/cJSON.h"
#include "include/json/CJsonObject.hpp"
#include "TcpMes.h"
#include "SoapMesLog.h"
#include "ProcessInfoCollect.h"
#include "curl.h" 
#include "MesGSoap.h"
#include "MesDataType.h"
class CMes;
typedef struct cMesSendInfo
{
	std::vector<CString> vFilePathList;
	int id;
	int iLenght;
	int iSum;
	CMes * pCMes;
}CMesSendInfo;

const CString g_strAPIFuncName[] = {
	_T("online"),
	_T("state"),
	_T("alarm"),
	_T("downtime"),
	_T("processingparameter"),
	_T("productparameter"),
	_T("recipeparameter"),
	_T("consumption"),
	_T("sample"),
	_T("recipeversion"),
	_T("barcoderequest"),
	_T("barcode"),
	_T("barcodebind"),
	_T("barcodeunbind"),
	_T("reciperequest"),
	_T("recipeparameterrequest"),
	_T("sendinterlock"),
	_T("getinterlock"),
	_T(" "),
};

const CString g_cstrRecipeParamCode[] = {
	_T("MAMQ1000"),
	_T("MAMQ1001"),
	_T("MAMQ1002"),
	_T("MAMQ1003"),
	_T("MAMQ1004"),
	_T("MAMQ1005"),
	_T("MAMQ1006"),
	_T("MAMQ1007"),
	_T("MAMQ1008"),
	_T("MAMQ1009"),
	_T("MAMQ1010"),
	_T("MAMQ1011"),
	_T("MAMQ1012"),
	_T("MAMQ1013"),
	_T("MAMQ1014"),
	_T("MAMQ1015"),
	_T("MAMQ1016"),
	_T("MAMQ1017"),
	_T("MAMQ1018"),
	_T("MAMQ1019"),
};

const CString g_cstrRecipeParamName[] = {
	_T("黑点"),
	_T("黑线"),
	_T("凹痕"),
	_T("白点"),
	_T("白线"),
	_T("边界漏箔"),
	_T("边线漏箔"),
	_T("漏箔"),
	_T("交界处漏箔"),
	_T("气泡"),
	_T("未辊压或未涂"),
	_T("极片长度"),
	_T("极片宽度"),
	_T("极耳长度"),
	_T("极耳高度"),
	_T("极耳料高"),
	_T("极片肩宽"),
	_T("黑点个数"),
	_T("凹坑个数"),
	_T("白点个数"),
};

enum eHttpAPI  //暂时需要，后面可能删除
{
	ReportEquipmentOnlineInfo = 0,				//获取设备在线状态(CCD要用)
	ReportEquipmentsStateInfo,					//实时状态采集
	ReportEquipmentAlarmInfo,					//设备报警信息
	ReportEquipmentDowntimeInfo,				//设备停机事件
	ReportEquipmentProcessingParametersInfo,	//过程采集(CCD要用)
	ReportEquipmentProductParametersInfo,		//设备产品参数采集
	ReportEquipmentRecipeParametersInfo,		//设备配方参数采集(CCD要用)
	ReportEquipmentConsumptionsInfo,			//设备物料消耗采集
	ReportEquipmentSamplesInfo,					//设备检测数据采集
	CheckEquipmentRecipeVersion,				//工艺配方对比
	RequestBarcode,								//产品条码下发
	ReportEquipmentBarcodesInfo,				//物料条码上传
	RequestBarcodesBinding,						//条码绑定
	RequestBarcodesUnbinding,					//解除条码绑定
	RequestRecipes,								//设备请求有效配方
	RequestRecipeParameters,					//设备请求配方参数API说明
	SendInterlock,								//MES下发互锁信号
	GetInterlock,								//设备获取互锁信号
	ReportCcdFilesUploadCompleted,				//报告CCD文件上传完毕
};

struct MemoryStruct {
	char *memory;
	size_t size;
};

typedef struct tagSelectUploadFile
{
	BOOL bUploadFile;//上传数据文件
	BOOL bUploadAllFile;//上传所有数据文件
	BOOL bUploadDefectFile;//上传瑕疵数据文件
	BOOL bUploadReportFile;//上传报表数据文件
};

enum logType
{
	SaveLogNone = 0,
	SaveThreadLog,
	SaveUpLoadLog,
	SaveAllLog,
};

class CMes : DHInterface
{
public:
	CMes();
	~CMes();

public:	
	virtual int DoSomething(void* message, void* wParam, void* lParam);
	BOOL Log2(const CString& strLog, const int& nType, const int&nExcpLevel);
public:
	int Init(PMESINIT pInit);//加载配置参数
	static void* IOThread(void *hcthread);//io线程
	void Loop();
	int PushMessage(const HTuple& hvMessageSend);//放置消息进入队列,通过Sonmthing()结构供平台传入结果数据消息
	bool ReadParams();//获取参数
	bool WriteParams();//保存参数
	BOOL DisplayMesSet(void);//显示参数设置

	CString GetCurrentLocalTime();
	CString GetCurrentLocalTime(const time_t & tTime);
	CString GetSystemLocalTime();
	
	//设置设备状态
	BOOL UploadDeviceInfo(tagMesDeviceInfo& tInfo);
public:
	BOOL UploadCcdFileOnly(CString strFullFilePath);//临时接口，后面完善

	//功能接口
	//打开连接
	BOOL OpenSocket(CString strProtocol = "TCP");//建立与MES服务器的连接
	//发送心跳
	BOOL Send_Heatbeat(CString strProtocol = "TCP");
	//状态数据的采集及上传
	BOOL Send_EquipmentStateData(CString strProtocol = "TCP");
	//产品过程数据上传-常规数据
	BOOL Send_equipmentProcessData(CString strProtocol = "TCP");
	//报警数据的采集及上传(需要汇总设备的报警信息，以表格形式呈现)
	BOOL Send_EquipmentAlarmData(CString strProtocol = "TCP");
	//设备配方数据的请求(传出从MES获取的配方数据->转发至设备进行设置)
	//设备通过按钮自行触发，以回去当前已选择工单的配方参数，也支持上位机在开机前自行去获取配方
	BOOL GetRequestRecipes(CString strProtocol = "TCP");
	//设备开机配方参数校验
	BOOL Send_CheckRecipeVersion(CString strProtocol = "TCP");

	//上传数据文件至服务器
	BOOL UpdateFileToServer(CString strFilePath, CString strNum);
public:
	//启动上传线程
	BOOL StartUploadFileThread(void);	
public:
	CString m_strConfigFile;	
	tagMesHead mesHead;        //MES头部分
	CString m_strIP;
	int m_nPort;

	BOOL	m_bUploadFileAuto;//是否自动上传文件

	tagSelectUploadFile tSelectUploadFile;//选择上传文件

	CString m_strFtpAddr;//FTP地址
	CString m_strReportAddr;//报表上传地址
	CString m_strFtpUserID;
	CString m_strFtpPwd;

	int m_nDataSourceIndex;//数据源下标

public:
	MESINIT m_sInit;         
	HTuple m_hvQueue;          //队列句柄
	bool m_useMes;             //是否使用MES
	SYSTEMTIME m_currentTime;  //系统时间
	CWinThread * m_ThreadFtpUpload;
	int m_intervalMinute;      //文件上传间隔(分钟)
	BOOL m_bExit;
	std::vector<CString> m_vFilePathList;
	CString m_strRenameSymbol; //重命名标志
	CString m_strCurrentBatch;
	HTuple m_hvUploaded;       //已经上传
	HTuple m_hvCurrentDisk;	   //磁盘扫描结果
	CString m_strLocalRootPath;//文件根路径
	CString m_strRootPathSymbol;
	BOOL m_bSendOK;     //(是否发送OK图片)
	BOOL m_bSendNG;     //(是否发送NG图片)
	BOOL m_bSendNGCrop; //(是否发送NG抠图图片)
	BOOL m_bSendCover;  //()
	int m_iThreadNumber;//线程个数
	CSoapMesLog* m_pSoapMesLog;//日志类对象
	CRITICAL_SECTION m_csWriteTuple;//线程互锁	
	CString m_strEquipmentCode;//设备编号(用于文件上传)
	ProcessInfoCollect picProcessInfoCollect; //资源使用统计 对象
	CString m_cstrUploadTuple;
	CRecord m_record;
	CString m_strLineCode;//线体编号(完善后根据需求保留或取消)
	CString m_strSectionCode;//(完善后根据需求保留或取消)
	int m_iConnectStauts;//文件服务器连接状态
	int m_nHeartBeatTime;  //心跳时间(秒为单位)
	BOOL m_isSendHttpMSG;//(变量看下能否取消)
	BOOL m_bRename;//(是否重命名文件)
public:
	//获取上传文件路径
	void GetFilePath(std::vector<CString>& vFilePathList, CString &strDir, BOOL & bSendOK, BOOL &bSendNG, BOOL & bSendNGC, BOOL & bSendCover);		//扫描磁盘文件
	void GetFilePath(std::vector<CString>& vFilePathList, CString &strDir, BOOL & bSendOK, BOOL &bSendNG, BOOL & bSendNGC);
	//检测文件是否上传过
	BOOL HasBeenSent(CString const& strLocalFilePath);	
	BOOL IsUpload(const CString & cstrFile);//已经上传 返回FALS，没有上传 返回TRUE
	BOOL UnRenameFile(CString const & strLocalFilePath, CString const & strSymbol, CString& strOldFilePath);
    //FTP上传文件
	int FtpUploadFile(CString & strFullFilePath);
	int FtpUpload(CString& strRemoteFilePath, CString& strLocalFilePath);
	//记录已上传文件
	BOOL AppendUploadTuple(CString & cstrFile);		       
	//本地文件 服务器文件  路径转换
	BOOL LocalFilePathToServerFilePath(CString const & strLocalFilePath, CString & strServerFilePath, CString const & strSymbol);
	//本地写Tuple文件
	BOOL WriteUploadTuple(void);
	////日志接口
	void Log(const CString &cstrMessage, const CString &cstrType = _T("EXCEPTION"));
	//文件上传完成报告Http??
	int MesReportFileUploadCompleted(CString & strFilePath);     
	//生成文件上传报告json信息(这里要确认下：如果文件只有图片的话，是否需要转换为JSON格式，另外文件传输是否采用FAT协议！！！)
	BOOL GetSendFileJson(CString const& strFilePath, std::string & strSendJson, SYSTEMTIME const& iLocalTime);
	//获取上传文件 CCD相关
	int GetParamCCDUploadFiles(const CString& cstrURI, std::vector<ParamCCDUploadFile>& vecParam); 
	//文件上传完成
	int Soap_CCDFileUploadComplete(const std::vector<ParamCCDUploadFile> & vecParamCCDUpload, neb::CJsonObject & nebCJsonReciev);
	//重命名文件
	BOOL RenameFile(CString const& strLocalFilePath, CString const & strSymbol);
	//上传文件 
	int ftp_upload(const char *remote_file_path, const char *local_file_path, const char *username, const char *password, long timeout = 200, long tries = 3);
	//上传完成
	int MESReportCcdFilesUploadCompleted(CString& strData, std::string &strRespons);

	int http_request(const char* url, std::string& strResponse, const std::string postdata = "", const char* headers = "Content-Type:application/json;charset=UTF-8", bool bReserveHeaders = false, int timeout = 20);
	
	static size_t write_data_callback(void *ptr, size_t size, size_t nmemb, void *stream);

public:
	//MES设置界面所需变量，暂时定义在一起，后面根据需求进行增删
	CString m_strEquipmentName;
	CString m_strFactory;
	CString m_strEQM;
	CString m_cstrResourceCode;
	//CString m_cstrOperatorID;
	//CString m_cstrOperatorPassword;
	//CString m_cstrUserName;
	//CString m_cstrUserPassword;
	CString m_strLocalFilePath;
	CString m_strServerFilePath;
	int m_nLogSaveDays;
	bool m_nType;
	int m_nLogType;
	neb::CJsonObject m_cJsonRecipes;	//配方Json包
	CString m_strRecipeVison; //配方版本
	CString m_strProductCode; //产品编号
	CString m_strRecipeCode;  //配方编号
public: //MES界面用到接口，后面根据需求进行增删
	void SetMesAddrAndPort();
	BOOL UNRename(CString & strFilePath);//删除文件名标记
	void GetFilePath(std::vector<CString>& vFilePathList, CString &strDir);
	int CHeckPrecipeVersion(std::string & strResponse = std::string(""));//配方校验
	int MESRequest(CString& strData, std::string& strRespons, eHttpAPI eApi);
	int Soap_CheckRecipeVersion(std::string &strResponse = std::string(""));
	int Soap_recipeVersionExamine(const ParamRecipeVersionExamine& paramRecipeVersion, neb::CJsonObject & nebCJsonReciev); //配方版本校验
	int Soap_getRecipeList(const CString& cstrPruoductCode, neb::CJsonObject & nebCJsonReciev);//获取配方列表
	int GetRequestRecipeParameters(CString const &strRecipe, std::string &strRespons);//获取配方参数
	int http_getquest(const char* url, std::string& strResponse, const char* headers = "Accept:application/json;Content-Type:application/json;charset=UTF-8", bool bReserveHeaders = false, int timeout = 20);
	static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *stream);//写内存回调函数
	int Soap_getRecipe(const CString& cstrRecipeCode, neb::CJsonObject & nebCJsonReciev);//获取配方明细
	int MESReportEquipmentRecipeParametersInfo(CString& strData, std::string &strRespons);//上传配方参数信息
	int Soap_recipe(const ParamRecipe& pRecipe, neb::CJsonObject & nebCJsonReciev);	//配方参数上传

	//新添加文件及文件夹的拷贝
	void GetFileNamePath(std::vector<CString>&vFilePathList, const CString&sPath, CString&sName);//获取所需上传文件列表
	BOOL CopyFileToServer(CString strLocalFullFilePath,CString strNumber);//文件路径 + 卷号 
	BOOL CopyFilePathToServer(CString strLocalPath, CString strNumber);
	BOOL CopyDirectory(CString strSrcPath, CString strDestPath);
	void StartCopyFileToServerThread(void);//线程启动函数

	bool startCopyFileFlag;
};
extern CMes* g_This;

void  ThreadFtpUpload(PVOID * pParam);//文件上传
UINT ThreadOnInfo(LPVOID lpVoid);     //文件上传心跳监测
UINT ThreadProcDynamic_1(LPVOID lpVoid);

UINT ThreadCopyFileToServer(LPVOID lpVoid);     //文件拷贝线程
