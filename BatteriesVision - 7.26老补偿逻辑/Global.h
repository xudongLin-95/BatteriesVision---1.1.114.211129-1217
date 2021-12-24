//////////////////////////////////////////////////////////////////////////
///Global.h ȫ���࣬������Ҫȫ�ִ�ŵı��������Է��ڴ˴�
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "DHInterFace.h"
#include "BaseTmpl.h"
#include "Permission.h"
#include "Project.h"
#include "Counter.h"
#include "GrabberTool.h"
#include "AlgorithmTool.h"
#include "ProductTool.h"
#include "Process.h"
#include "Judge.h"
#include "Storage.h"
#include "Language.h"
#include "CommunicationTool.h"
#include "DataFilter.h"
#include "BatteriesVisionDlg.h"
#include "SubDispScope.h"
#include "SubDispList.h"
#include "Upload.h"
#include "FoolProofing.h"
#include "ErrorInfo.h"
#include "DataCalculate.h"
#include "DlgAlarm.h"
#include "DlgTip.h"
#include "Callback.h"


extern int      WM_DISP;		//��ʾ�����Ϣ
extern int      WM_GRABBER;		//��ʾ֡����
extern int      WM_DETECT;		//��ʾ������
extern int      WM_RESTARTPROCESS;	//����
extern int		WM_REFRESH_DISP;	//ˢ����ʾ
extern int		WM_CHECK_PMSN;		//���Ȩ��
extern int      WM_CHANGEBATCH;
extern int      WM_UPDATE_PRODUCT;
extern int      WM_UPDATE_ONLINEOFFSET;
const  int      nImageZoomWidth  = 1000;
const  int      nImageZoomHeight = 240;
const  double   dImageDilation = 0.2;
class CDlgLoading;
class CDlgAlarm;
class CGlobal : public CBaseTmpl
{
public:
	enum eHistoryType
	{
		NoHistory=0,	//���в���
		ErrorHistory,	//����ֻ�������ʷ
		AllHistory,		//���б���������ʷ
	};
	enum eDeviceType//�豸����
	{
		DT_SingleLayer = 0,		//����Ϳ����
		DT_DoubleLayer,			//˫��Ϳ���������ʽͿ����
	};
	enum eStationSN//��λ
	{
		SN_Head=1,	//��λ1����ͷ
		SN_2nd,		//��λ2�����ʽ��Ϳ�������β
		SN_3rd,		//��λ3�����ʽ��β
		SN_4th,		//��λ4
		SN_5th,		//��λ5
	};
	struct tagSystemSet
	{
		eDeviceType		eDevType;				//�豸���ͣ�0���㣬1˫��
		eStationSN		eSN;					//1����λ��1Ϳ  2����2Ϳ 3����λ�ڻ�β

		CString			strWorkshopName;		//����������
		CString			strDeviceID;			//�豸���

		int				nHistoryCount;			//��������ʷͼƬ�ĸ���
		eHistoryType	eHistoryType;			//����������Ļ���ȫ������,���ǲ�����

		BOOL			bGrabAlign;				//�����ͼ��λ
		int				nCameraDistance;		//ǰ���������

		int				nActionMode;			//ϵͳ���ģʽ

		double			dEnableOffsetSpeed;		//��������ٶȿ����ջ�����

		BOOL			bUseChangeBatchAuto;	//�Ƿ�ʹ���Զ�����
		double			dChangeBatchDelayMeter;	//�����ӳ��������ӳٻ���������Ż���
		BOOL			bCheckProductInfoByChangeBatch;//����ʱ�Ƿ�ʹ�ö๤λ�䷽У��
		
		inline void Init()
		{
			eDevType = DT_SingleLayer;
			eSN = SN_Head;

			nHistoryCount = 10;
			eHistoryType = NoHistory;
			bGrabAlign = FALSE;
			nCameraDistance = 1000;

			strWorkshopName = "";
			strDeviceID = "";

			nActionMode = RunMode;

			bUseChangeBatchAuto = FALSE;
			dChangeBatchDelayMeter = 0;
			bCheckProductInfoByChangeBatch = FALSE;
		}
	};
	struct tagResolution
	{
		double dRow;
		double dColumn;
	};
	typedef std::vector<tagResolution> ResolutionList;
public:
	CGlobal();
	~CGlobal();
public:
	CProject			m_Project;			//��Ŀ�����࣬���ü���������漸����Ʒ���
	CGrabberTool		m_GrabberTool;		//�ɼ������
	CCounter			m_Counter;			//ͳ�Ƽ���
	CSpeed              m_Speed;            //�����豸�ٶ�
	CAlgorithmTool		m_AlgTool;			//�㷨����
	CProductTool		m_ProductTool;		//Ʒ�ֹ���
	CProcess			m_Process;			//��⴦��
	CStorage			m_Storage;			//�洢
	CJudge				m_Judge;			//�жϼ����
	CLog				m_Log;				//��־
	CPermission			m_Permission;		//Ȩ��
	CLanguage			m_Language;			//������
	CCommunicationTool	m_CommunicationTool;//ͨ�Ź���
	CDataFilter			m_DataFilter;		//�����˲�
	CDataCalculate		m_DataCalculate;	//���ݼ���
	CFoolProofing		m_FoolProofing;		//����ģ��
	CErrorInfo			m_ErrorInfo;		//������Ϣ
	CCallback			m_Callback;			//����ģ�����ϵͳ�ص�ִ�в���
	
	ResolutionList		m_vecResolution;	//ͼ�������

	CBatteriesVisionDlg* m_pMainDlg;		//���Ի���ָ��
	CSubDispScope*		m_pSubDispScope;	//����ͼ�ӶԻ�ָ��
	CSubDispList*		m_pSubDispList;		//��ʾ�б��ӶԻ�ָ��

	HObjectVector		m_hovecHistroyImage;

	tagSystemSet		m_tSystemSet;		//ϵͳ����

	BOOL                m_bMSA;

	CUpload m_Upload;
	HTuple  m_hOffsetSocket;//����offsetֵʹ��
	BOOL    m_bExitThread;
	CString m_strVersion;

	double m_dCurSpeed;
public:
	//������������
	void CrashRecord();	
	//��֤key
	void Verify();
	//��ʾ��֤�⵽��ʱ��
	void ShowVerifyInfo();
	//��ʼ��
	BOOL Init();
	BOOL InitAll();
	//������
	BOOL ReadParam();
	BOOL ReadParamAll();
	//д����
	BOOL WriteParam();
	BOOL WriteParamAll();

	//�������ضԻ���
	void InitLoadingDlg();
	void SetLoadingDlgInfo(const CString& strInfoCN,const CString& strInfoEN);
	void SetLoadingDlgInfo(const CString& strInfoCN,const CString& strInfoEN,double dPercent);
	void CloseLoadingDlg();
	//�����Ի���
	void InitAlarmDlg();
	void SetAlarmInfo(const CString& strInfo);
	void SetAlarmInfo(const int& nErrorCode);
	void SetAlarmInfo(CErrorInfo::tagErrorInfo tInfo);
	void CloseAlarmDlg();
	//��ʾ�Ի���
	void InitTipDlg();
	void SetTipInfo(eTipType eType);
	void CloseTipDlg();

	//��ȡϵͳ���ò���
	tagSystemSet GetSystemSet() const { return m_tSystemSet; }

	//�������
	int  GetTheme(){return m_nTheme;}
	void SetTheme(int nTheme){m_nTheme=nTheme;}

	//��ŵ��ýӿ�
	CString GetCurrentBatch() {return m_Counter.GetCurrentBatch().strName;}
	void	StartCurrentBatch(const CString& strBatch){ m_Counter.StartBatch(strBatch);}
	void	EndCurrentBatch() { m_Counter.EndBatch(); }

	//���ŵ��ýӿ�
	CString GetCurrentJobNum() {return m_Permission.GetUserInfo().strJobNumber;}

	//��ʷ��¼���ýӿ�
	int	 GetHistoryCount()const {return m_tSystemSet.nHistoryCount;}
	void SetHistoryCount(const int& nCount){m_tSystemSet.nHistoryCount = nCount;}
	void SetHistoryType(eHistoryType eType){m_tSystemSet.eHistoryType = eType;}
	eHistoryType GetHistoryType()const {return m_tSystemSet.eHistoryType;}

	//��ȡ���������
	const ResolutionList & GetResolutionList()const {return m_vecResolution;}
	//��ȡ����򿪵�ʱ��
	const SYSTEMTIME & GetProgramOpenTime()const {return m_tmProgramOpen;}

	//�������淢����Ϣ
	BOOL PostMessageToMainDlg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL SendMessageToMainDlg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	//����������־λ���ýӿ�
	BOOL IsStart(){return m_bStart;}
	void SetStart(const BOOL& bStart){m_bStart=bStart;}

	//��ȡ����汾
	const CString & GetVersion() { return m_strVersion; }

	double GetCPURate();
	double GetRAMUseMB();
private:
	
private:
	CString m_strHelpFile;			//�����ļ�·��
	int		m_nTheme;				//BCG�������

	CString m_strCalibrationPath;	//�궨�����ļ���·��
	CString m_strPositionParamFile;	//λ�ò����ļ�·��
	
	SYSTEMTIME m_tmProgramOpen;		//����򿪵�ʱ��
	BOOL	m_bStart;				//�����Ƿ�ʼ���

	CDlgLoading* m_pDlgLoading;		//���ضԻ���
	CDlgAlarm* m_pDlgAlarmInfo;		//�����Ի���
	CDlgTip* m_pDlgTip;				//��ʾ�Ի���

	DHInterface* m_pCrash;
protected:
};

extern CGlobal theGlobal;