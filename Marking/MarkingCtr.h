#pragma once
#include "DHInterFace.h"
#include "StdAfx.h"
//#include "DH_HalconEx.h"
#include "DH_HalconEx.h"
#include "Log.in.h"
#include "SCIMeasure.in.h"
#include "SCIDataFlow.in.h"
#include "HDevThread.h"
#include "CallBack.in.h"
#include "Marking.in.h"
#include <vector>
#include <map>
using std::map;
using std::vector;
//˵����ͼƬ���и߶Ȳ�ͬ��ÿ��ͼ����200mm�س��������Ҳ��һ�ţ�
class CMarkingCtr : DHInterface
{
public:
	typedef struct tagSOCKETSET
	{
		int     nUseCommu;       //�Ƿ�ʹ���������socketͨ��
		CString strIP;           //IP��ַ
		int     nPort;           //�˿�
		int     nImgDiff;        //ͼ����ӳ�֡��
		int     nUseCheckDefeat; //�Ƿ�ʹ�ü���ֵ
		int     nUseCheckSize;   //�Ƿ�ʹ�ü��ߴ�
		int     nMarkingWidth;   //��������Ϳ���(200�ı���)
		int     nMarkingHeight;  //��������Ϳ�߶�(200�ı���),����һ��ÿ200mm��һ��
		int     nDivImageNum;    //ÿ��ͼ����Ϊ��������Ĭ��2��
	}TSOCKETSET, *PTSOCKETSET;

	typedef struct tagTMARKINGRES//��ʼ��
	{
		int				nImgNum;			//��ǰͼ�����
		CString			sResult;			//��覴ý���ַ�����û��覴�ʱΪ��
		int				nImgSNTarget;		//��ǰ��覴�ʱҪ��ͼ��Ŷ��ٵ�ʱ���ͽ��
		vector<CString> vSaveResult;
		double			dTotalMeter;		//��ǰ����
		double			dSpeed;				//��ǰ�ٶ�
		double			dWholeWidth;		//�ܿ�
	}MARKINGRES, *PMARKINGRES;

	typedef struct tagPartitionMsg
	{		
		HTuple  hv_PartEdgeCol;   //�����Ե
		HObject ho_PartAll;		//����

		HTuple  hv_SigleStripeSN;//����
	}PARTITIONMSG;
	
	typedef struct tagRegionMsg
	{
		HObject ho_NewtRegion; //���»��ֺ��覴�����
		HTuple  hv_NewStrip;   //���»��ֺ��覴����򣬶�Ӧ����
		HTuple  hv_NewType;    //���»��ֺ��覴����򣬶�Ӧ����
		HTuple  hv_NewSide;   //���»��ֺ��覴����򣬶�Ӧ��
		
	}NEWREGIONMSG;

	struct tagDefect
	{
		//vector<int> nStripeSN; //�������(�������)
		vector<int> nType;      //覴�����
		vector<double> nRow1;   //覴��������������
		vector<double> nCol1;   //覴��������������
		vector<double> nRow2;   //覴������յ�������
		vector<double> nCol2;   //覴������յ�������
	};	

	struct tagResolution
	{
		double dRow;
		double dColumn;
	};
	vector<tagResolution> m_vResolutionList;

	struct tagCameraBaseMsg
	{
		int    nCameraCount;     //�������
		double dHResolutionSideA;//A�����ˮƽ�ֱ���
		double dHResolutionSideB;//B�����ˮƽ�ֱ���
		double dVResolutionSideA;//A�������ֱ�ֱ���
		double dVResolutionSideB;//B�������ֱ�ֱ���
		HTuple hv_ColumnOverA;   //A���ص�������Ϣ
		HTuple hv_ColumnOverB;   //B���ص�������Ϣ
	};
	tagCameraBaseMsg t_CameraBaseMsg;
public:
	CMarkingCtr(void);
	virtual ~CMarkingCtr(void);
	virtual int DoSomething(void* message, void* wParam, void* lParam);

	void ReadParam();
	void WriteParam();

	//socket sec
	BOOL SocketConnect(const CString& strIP, const int& nPort);
	BOOL SocketDisconnect();
	BOOL SocketReconnect();
	BOOL GetIsConnected(int& res);
	
	BOOL SendResultToMarking(C_MergeWork& Result);
	BOOL ReceiveStringResult(CString& sResult);
	BOOL SendStrToMarking(CString Result, int sendSN=0,int newSN=0);

	//ˮƽ��ȡ����
	BOOL GetPartitionInHorizontalDirection(C_MergeWork& Result, vector<PARTITIONMSG>& vPartitionMsg);
	//��ȡ覴����»��ֵİ˷�����
	BOOL GetDefectInNewRegion(C_MergeWork& Result,int SideNum, vector<PARTITIONMSG>& vPartitionMsg, vector<tagDefect>& mtagDefacts);
	//��ȡ�ߴ�NG���ڰѷ�����
	BOOL GetDefectSize(C_MergeWork& Result, int SideNum, vector<PARTITIONMSG>& vPartitionMsg,vector<tagDefect>& mtagDefacts);

	BOOL SetCurrentBatch(CString strBatch);
public:
	CString m_strCurrentBatch;
	HTuple m_hv_Socket;
	TSOCKETSET m_tSocketSet;
	bool m_bIsConnected;
	CBCGPDialog* m_pDlg;//������ָ��
	std::vector<MARKINGRES> m_vMarkingRes;
	HObject m_ho_RegionSegment;//�����洢ͼ��ֱ���򻮷ֺ������
		
private:
	MARKINGINIT m_sInit;   //����������Ϊpublic,�����Ϊ˽��
	HTuple m_hvDefectToMarkigQueue;//���о��   覴ý�����У����͸������
	CString m_strConfigMarkingFile;

	HTupleVector m_hvec_Data;
	static void* HThreadMarking(void *hcthread);
	void marking_thread();
	BOOL SaveDataMarking(int nImgNum, double dTM, double dSpeed, vector<CString> saveResults);

	//static CCriticalSection m_csLock;
	CRITICAL_SECTION m_csLock;
	BOOL CreateMarkingFile(const CString& strFilePath, int nPartNum);
	BOOL GetCameraSetting(vector<tagResolution>& vResolutionList, tagCameraBaseMsg& cameraBaseMsg);
	BOOL GetCameraResolution(vector<tagResolution>& ResolutionList, tagCameraBaseMsg& tCameraBaseMsg);
private:
	int Init(MARKINGINIT &Init);
	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);
	int PushMessage(const HTuple& hvMessageSend);//������Ϣ�������
	BOOL ShowParamDlg();
	int m_nNGCount;
	CString strFileName;

	int m_nIOCount;
	
	vector<int> vACameraSnList; //A�����SN�б�
	vector<int> vBCameraSnList; //B�����SN�б�

	map<CString, CString> NgDescribe;//�������Ĵ������Ͷ�Ӧ����
	void initNgDescribe() { //��ʼ��
		NgDescribe.insert(map<CString, CString>::value_type("a", "©����"));
		NgDescribe.insert(map<CString, CString>::value_type("b", "�Ӵ�"));
		NgDescribe.insert(map<CString, CString>::value_type("c", "��̼"));
		NgDescribe.insert(map<CString, CString>::value_type("d", "����"));
		NgDescribe.insert(map<CString, CString>::value_type("e", "����"));
		NgDescribe.insert(map<CString, CString>::value_type("f", "����(����)"));
		NgDescribe.insert(map<CString, CString>::value_type("g", "����"));
		NgDescribe.insert(map<CString, CString>::value_type("h", "��Ƭ�ڵ�"));
		NgDescribe.insert(map<CString, CString>::value_type("i", "����"));
		NgDescribe.insert(map<CString, CString>::value_type("j", "��Ƭ�ߴ粻��"));
	};
};

extern CMarkingCtr* g_This;