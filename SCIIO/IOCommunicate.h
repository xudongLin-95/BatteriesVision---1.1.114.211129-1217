#pragma once
#include "PIO24BLib.h"
#include "SCIIO.in.h"
#include "Log.in.h"
#include <vector>
#include <setupapi.h>
#include "rpcdce.h"
#include "BCGCBProInc.h"
#include "Language.h"
const int MAX_LANE = 2;
const int DEF_LASTLABLEPOS = -99999;//Ĭ������µı�ǩλ��


typedef struct tagDeviceInfo
{
	//�豸�Ѻ�����,���Ѻá���
	CString szDeviceName;
	//�豸��
	CString szDeviceClass;
	//�豸��ʾ��
	CString szDeviceDesc;
	//�豸����
	CString szDriverName;
	//�豸ʵ��
	DWORD dwDevIns;
	//�豸���־
	GUID Guid;
	//����������
	bool operator < (const tagDeviceInfo &tmp) const
	{
		if (tmp.szDeviceClass != szDeviceClass)
		{
			return tmp.szDeviceClass.CompareNoCase (szDeviceClass) > 0;
		}
		else
		{
			return tmp.szDeviceDesc.CompareNoCase (szDeviceDesc) > 0;
		}
	}
}DeviceInfo;
#define TB_CHANNEL 2
class CIOCommunicate : DHInterface
{
public:
	CIOCommunicate(void);
	virtual ~CIOCommunicate(void);
	virtual int DoSomething(void* message,void* wParam,void* lParam);
public:
	
	int m_nMarkerLength;//������ĳ��ȣ��У�,0 ���0 ,1 ���1

	CString		m_strProductName;
	CString strProductName;

	CString				m_strConfigCommonFile;
	CString				m_strConfigSystemFile;
	CString				m_strConfigCommunicateFile;
	CString				m_strConfigCalibrationFile;
	COMMUICATEPARAM		m_Commuicate;//�߷���ز���

	ALARMS				m_Alarms;//������ز���
	COMMONPARAM			m_Common;//��������
	static CPIO24BLib   m_IOCard[PIO24_COUNT];//���ͼ��IO��
	int					m_nProductType;
	
    HWND				m_OutputhWnd;
	int					m_nCurrentStatus;//��ǰ״̬
	CLanguage			m_Lang;

	int					m_nCamRelation;//�����ϵ��0 ���0��Ӧ�ϴ�·(����)��1 ���1��Ӧ�ϴ�·(����)
	int                 m_nIn[2];
	BOOL                m_bIn[2];
	BOOL                m_bLXTB[TB_CHANNEL];//4����·�Ƿ���������������
	BOOL m_bExit;//�Ƿ��˳�
private:
	void InitThread();
	void ReadParam();
	void WriteParam();
	BOOL SendMeasureIO(C_MergeWork& tOutput);
	BOOL SendLabelIO(const BOOL& bFlag, const int& nPictureSN, const int& nChannel, int& nBias);
	BOOL GetDeviceList(LPGUID lpGuid,BOOL bsFlag);
	BOOL SetDeviceStatus(DeviceInfo &theDevice,BOOL bStatusFlag);

	BOOL SetAlarmStatus(const e_AlarmStatus& nParam);//���ñ���״̬
	BOOL IO_heartBeat (BOOL useHeartBeat = true);
private:
	
	IOINIT m_tInit;

	HTuple m_hvQueue;//���о��

	std::vector<HDevThread*> m_vecThreadHandle;

	HTuple m_hvLabelPointRow[2];//��������������
	HTuple m_hvDecreaseFrames[2];//�ӳ�֡���ݼ���������
	HTuple m_hvResultFlag[2];//�����־λ���飬�������ӳ�
	HTuple m_hvOriImageSN[2];//ԭʼͼ��ţ��������ӳ�
private:
	static void* HThreadIO(void *hcthread);//io�߳�
	void IO_thread(HTuple hvQueue);
	int Init(PIOINIT pInit);//��ʼ��
	int PushMessage(const HTuple& hvMessageSend);//������Ϣ�������
	BOOL ReadyOutput(CString & strParam);//Ready�ź�
	BOOL Log(const CString& strLog,const int& nType,const int&nExcpLevel=NoExcp);
};

extern CIOCommunicate* g_This;