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
	_T("�ڵ�"),
	_T("����"),
	_T("����"),
	_T("�׵�"),
	_T("����"),
	_T("�߽�©��"),
	_T("����©��"),
	_T("©��"),
	_T("���紦©��"),
	_T("����"),
	_T("δ��ѹ��δͿ"),
	_T("��Ƭ����"),
	_T("��Ƭ���"),
	_T("��������"),
	_T("�����߶�"),
	_T("�����ϸ�"),
	_T("��Ƭ���"),
	_T("�ڵ����"),
	_T("���Ӹ���"),
	_T("�׵����"),
};

enum eHttpAPI  //��ʱ��Ҫ���������ɾ��
{
	ReportEquipmentOnlineInfo = 0,				//��ȡ�豸����״̬(CCDҪ��)
	ReportEquipmentsStateInfo,					//ʵʱ״̬�ɼ�
	ReportEquipmentAlarmInfo,					//�豸������Ϣ
	ReportEquipmentDowntimeInfo,				//�豸ͣ���¼�
	ReportEquipmentProcessingParametersInfo,	//���̲ɼ�(CCDҪ��)
	ReportEquipmentProductParametersInfo,		//�豸��Ʒ�����ɼ�
	ReportEquipmentRecipeParametersInfo,		//�豸�䷽�����ɼ�(CCDҪ��)
	ReportEquipmentConsumptionsInfo,			//�豸�������Ĳɼ�
	ReportEquipmentSamplesInfo,					//�豸������ݲɼ�
	CheckEquipmentRecipeVersion,				//�����䷽�Ա�
	RequestBarcode,								//��Ʒ�����·�
	ReportEquipmentBarcodesInfo,				//���������ϴ�
	RequestBarcodesBinding,						//�����
	RequestBarcodesUnbinding,					//��������
	RequestRecipes,								//�豸������Ч�䷽
	RequestRecipeParameters,					//�豸�����䷽����API˵��
	SendInterlock,								//MES�·������ź�
	GetInterlock,								//�豸��ȡ�����ź�
	ReportCcdFilesUploadCompleted,				//����CCD�ļ��ϴ����
};

struct MemoryStruct {
	char *memory;
	size_t size;
};

typedef struct tagSelectUploadFile
{
	BOOL bUploadFile;//�ϴ������ļ�
	BOOL bUploadAllFile;//�ϴ����������ļ�
	BOOL bUploadDefectFile;//�ϴ�覴������ļ�
	BOOL bUploadReportFile;//�ϴ����������ļ�
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
	int Init(PMESINIT pInit);//�������ò���
	static void* IOThread(void *hcthread);//io�߳�
	void Loop();
	int PushMessage(const HTuple& hvMessageSend);//������Ϣ�������,ͨ��Sonmthing()�ṹ��ƽ̨������������Ϣ
	bool ReadParams();//��ȡ����
	bool WriteParams();//�������
	BOOL DisplayMesSet(void);//��ʾ��������

	CString GetCurrentLocalTime();
	CString GetCurrentLocalTime(const time_t & tTime);
	CString GetSystemLocalTime();
	
	//�����豸״̬
	BOOL UploadDeviceInfo(tagMesDeviceInfo& tInfo);
public:
	BOOL UploadCcdFileOnly(CString strFullFilePath);//��ʱ�ӿڣ���������

	//���ܽӿ�
	//������
	BOOL OpenSocket(CString strProtocol = "TCP");//������MES������������
	//��������
	BOOL Send_Heatbeat(CString strProtocol = "TCP");
	//״̬���ݵĲɼ����ϴ�
	BOOL Send_EquipmentStateData(CString strProtocol = "TCP");
	//��Ʒ���������ϴ�-��������
	BOOL Send_equipmentProcessData(CString strProtocol = "TCP");
	//�������ݵĲɼ����ϴ�(��Ҫ�����豸�ı�����Ϣ���Ա����ʽ����)
	BOOL Send_EquipmentAlarmData(CString strProtocol = "TCP");
	//�豸�䷽���ݵ�����(������MES��ȡ���䷽����->ת�����豸��������)
	//�豸ͨ����ť���д������Ի�ȥ��ǰ��ѡ�񹤵����䷽������Ҳ֧����λ���ڿ���ǰ����ȥ��ȡ�䷽
	BOOL GetRequestRecipes(CString strProtocol = "TCP");
	//�豸�����䷽����У��
	BOOL Send_CheckRecipeVersion(CString strProtocol = "TCP");

	//�ϴ������ļ���������
	BOOL UpdateFileToServer(CString strFilePath, CString strNum);
public:
	//�����ϴ��߳�
	BOOL StartUploadFileThread(void);	
public:
	CString m_strConfigFile;	
	tagMesHead mesHead;        //MESͷ����
	CString m_strIP;
	int m_nPort;

	BOOL	m_bUploadFileAuto;//�Ƿ��Զ��ϴ��ļ�

	tagSelectUploadFile tSelectUploadFile;//ѡ���ϴ��ļ�

	CString m_strFtpAddr;//FTP��ַ
	CString m_strReportAddr;//�����ϴ���ַ
	CString m_strFtpUserID;
	CString m_strFtpPwd;

	int m_nDataSourceIndex;//����Դ�±�

public:
	MESINIT m_sInit;         
	HTuple m_hvQueue;          //���о��
	bool m_useMes;             //�Ƿ�ʹ��MES
	SYSTEMTIME m_currentTime;  //ϵͳʱ��
	CWinThread * m_ThreadFtpUpload;
	int m_intervalMinute;      //�ļ��ϴ����(����)
	BOOL m_bExit;
	std::vector<CString> m_vFilePathList;
	CString m_strRenameSymbol; //��������־
	CString m_strCurrentBatch;
	HTuple m_hvUploaded;       //�Ѿ��ϴ�
	HTuple m_hvCurrentDisk;	   //����ɨ����
	CString m_strLocalRootPath;//�ļ���·��
	CString m_strRootPathSymbol;
	BOOL m_bSendOK;     //(�Ƿ���OKͼƬ)
	BOOL m_bSendNG;     //(�Ƿ���NGͼƬ)
	BOOL m_bSendNGCrop; //(�Ƿ���NG��ͼͼƬ)
	BOOL m_bSendCover;  //()
	int m_iThreadNumber;//�̸߳���
	CSoapMesLog* m_pSoapMesLog;//��־�����
	CRITICAL_SECTION m_csWriteTuple;//�̻߳���	
	CString m_strEquipmentCode;//�豸���(�����ļ��ϴ�)
	ProcessInfoCollect picProcessInfoCollect; //��Դʹ��ͳ�� ����
	CString m_cstrUploadTuple;
	CRecord m_record;
	CString m_strLineCode;//������(���ƺ������������ȡ��)
	CString m_strSectionCode;//(���ƺ������������ȡ��)
	int m_iConnectStauts;//�ļ�����������״̬
	int m_nHeartBeatTime;  //����ʱ��(��Ϊ��λ)
	BOOL m_isSendHttpMSG;//(���������ܷ�ȡ��)
	BOOL m_bRename;//(�Ƿ��������ļ�)
public:
	//��ȡ�ϴ��ļ�·��
	void GetFilePath(std::vector<CString>& vFilePathList, CString &strDir, BOOL & bSendOK, BOOL &bSendNG, BOOL & bSendNGC, BOOL & bSendCover);		//ɨ������ļ�
	void GetFilePath(std::vector<CString>& vFilePathList, CString &strDir, BOOL & bSendOK, BOOL &bSendNG, BOOL & bSendNGC);
	//����ļ��Ƿ��ϴ���
	BOOL HasBeenSent(CString const& strLocalFilePath);	
	BOOL IsUpload(const CString & cstrFile);//�Ѿ��ϴ� ����FALS��û���ϴ� ����TRUE
	BOOL UnRenameFile(CString const & strLocalFilePath, CString const & strSymbol, CString& strOldFilePath);
    //FTP�ϴ��ļ�
	int FtpUploadFile(CString & strFullFilePath);
	int FtpUpload(CString& strRemoteFilePath, CString& strLocalFilePath);
	//��¼���ϴ��ļ�
	BOOL AppendUploadTuple(CString & cstrFile);		       
	//�����ļ� �������ļ�  ·��ת��
	BOOL LocalFilePathToServerFilePath(CString const & strLocalFilePath, CString & strServerFilePath, CString const & strSymbol);
	//����дTuple�ļ�
	BOOL WriteUploadTuple(void);
	////��־�ӿ�
	void Log(const CString &cstrMessage, const CString &cstrType = _T("EXCEPTION"));
	//�ļ��ϴ���ɱ���Http??
	int MesReportFileUploadCompleted(CString & strFilePath);     
	//�����ļ��ϴ�����json��Ϣ(����Ҫȷ���£�����ļ�ֻ��ͼƬ�Ļ����Ƿ���Ҫת��ΪJSON��ʽ�������ļ������Ƿ����FATЭ�飡����)
	BOOL GetSendFileJson(CString const& strFilePath, std::string & strSendJson, SYSTEMTIME const& iLocalTime);
	//��ȡ�ϴ��ļ� CCD���
	int GetParamCCDUploadFiles(const CString& cstrURI, std::vector<ParamCCDUploadFile>& vecParam); 
	//�ļ��ϴ����
	int Soap_CCDFileUploadComplete(const std::vector<ParamCCDUploadFile> & vecParamCCDUpload, neb::CJsonObject & nebCJsonReciev);
	//�������ļ�
	BOOL RenameFile(CString const& strLocalFilePath, CString const & strSymbol);
	//�ϴ��ļ� 
	int ftp_upload(const char *remote_file_path, const char *local_file_path, const char *username, const char *password, long timeout = 200, long tries = 3);
	//�ϴ����
	int MESReportCcdFilesUploadCompleted(CString& strData, std::string &strRespons);

	int http_request(const char* url, std::string& strResponse, const std::string postdata = "", const char* headers = "Content-Type:application/json;charset=UTF-8", bool bReserveHeaders = false, int timeout = 20);
	
	static size_t write_data_callback(void *ptr, size_t size, size_t nmemb, void *stream);

public:
	//MES���ý��������������ʱ������һ�𣬺���������������ɾ
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
	neb::CJsonObject m_cJsonRecipes;	//�䷽Json��
	CString m_strRecipeVison; //�䷽�汾
	CString m_strProductCode; //��Ʒ���
	CString m_strRecipeCode;  //�䷽���
public: //MES�����õ��ӿڣ�����������������ɾ
	void SetMesAddrAndPort();
	BOOL UNRename(CString & strFilePath);//ɾ���ļ������
	void GetFilePath(std::vector<CString>& vFilePathList, CString &strDir);
	int CHeckPrecipeVersion(std::string & strResponse = std::string(""));//�䷽У��
	int MESRequest(CString& strData, std::string& strRespons, eHttpAPI eApi);
	int Soap_CheckRecipeVersion(std::string &strResponse = std::string(""));
	int Soap_recipeVersionExamine(const ParamRecipeVersionExamine& paramRecipeVersion, neb::CJsonObject & nebCJsonReciev); //�䷽�汾У��
	int Soap_getRecipeList(const CString& cstrPruoductCode, neb::CJsonObject & nebCJsonReciev);//��ȡ�䷽�б�
	int GetRequestRecipeParameters(CString const &strRecipe, std::string &strRespons);//��ȡ�䷽����
	int http_getquest(const char* url, std::string& strResponse, const char* headers = "Accept:application/json;Content-Type:application/json;charset=UTF-8", bool bReserveHeaders = false, int timeout = 20);
	static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *stream);//д�ڴ�ص�����
	int Soap_getRecipe(const CString& cstrRecipeCode, neb::CJsonObject & nebCJsonReciev);//��ȡ�䷽��ϸ
	int MESReportEquipmentRecipeParametersInfo(CString& strData, std::string &strRespons);//�ϴ��䷽������Ϣ
	int Soap_recipe(const ParamRecipe& pRecipe, neb::CJsonObject & nebCJsonReciev);	//�䷽�����ϴ�

	//������ļ����ļ��еĿ���
	void GetFileNamePath(std::vector<CString>&vFilePathList, const CString&sPath, CString&sName);//��ȡ�����ϴ��ļ��б�
	BOOL CopyFileToServer(CString strLocalFullFilePath,CString strNumber);//�ļ�·�� + ��� 
	BOOL CopyFilePathToServer(CString strLocalPath, CString strNumber);
	BOOL CopyDirectory(CString strSrcPath, CString strDestPath);
	void StartCopyFileToServerThread(void);//�߳���������

	bool startCopyFileFlag;
};
extern CMes* g_This;

void  ThreadFtpUpload(PVOID * pParam);//�ļ��ϴ�
UINT ThreadOnInfo(LPVOID lpVoid);     //�ļ��ϴ��������
UINT ThreadProcDynamic_1(LPVOID lpVoid);

UINT ThreadCopyFileToServer(LPVOID lpVoid);     //�ļ������߳�
