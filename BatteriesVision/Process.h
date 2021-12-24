#pragma once
#include "HalconCpp.h"
#include <vector>
#include <queue>
#include "SCIGrabber.in.h"
#include "SCIDataFlow.in.h"
#include "BaseTmpl.h"
#include "Judge.h"


void GetImage(int nGrabberSN,HObject* hoGrabberImage);
double GetEncoder(const int& nGrabberSN);
int  GetFrameSN(const int& nGrabberSN);

#define MAX_QUEUE_NUM 50	//������Ԫ�ظ���

class CProcess : public CBaseTmpl
{
public:
	typedef std::vector<HDevThread*> ThreadList;
public:
	CProcess();
	~CProcess();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	BOOL InitQueues();
	BOOL ReleaseQueues();
	BOOL InitThread();
	BOOL ReleaseThread();

	int GetAcquisitionThreadNum() { return m_nAcqThreadNum; }
	int GetMergeThreadNum() { return m_nMergeThreadNum; }

	HTuple GetImageQueue(const int& nSN) { return m_ImageQueues[nSN].T(); }
	HTupleVector GetImageQueues() { return m_ImageQueues; }
	HTuple GetResultQueue(const int& nSN) { return m_ResultQueues[nSN].T(); }
	HTuple GetDisplayQueue(const int& nSN) { return m_DisplayQueues[nSN].T(); }

	static void WINAPI AcquisitionCallBack(const s_GBSIGNALINFO* SigInfo);//�ɼ��ص�

	//��ʼģ��ɼ�
	void StartSimulateGrab();
	void StopSimulateGrab() { m_bSimuateGrabStop = TRUE; }

	//�����Զ�����
	void ListenChangeBatchAuto();
private:
	//����߳�
	static void* HThreadDetect(void *hcthread);
	//�ϲ�����̣߳������ʾ���
	static void* HThreadMerge(void *hcthread);
	//�����߳�
	static void* HThreadListen(void *hcthread);
	//�����ͼ�߳�
	static void* HThreadSimluateGrab(void *hcthread);

	//�ɼ��ص�
	static void WINAPI T_Grab(const s_GBSIGNALINFO* SigInfo,HTupleVector hvec_QueueImages);
	//�����߳�
	static void T_Detect(HTupleVector hvec_QueueImages, HTupleVector hvec_QueueResults, HTuple hv_ThreadIndex);
	//�������̣߳������ʾ���
	static void T_Merge(HTupleVector hvc_QueueResults,HTuple hv_QueueDisplay,HTuple hv_ThreadIndex);
	//�����̣߳����������źţ�CCD�豸״̬�����������߼�
	static void T_Listen();
	//�����ͼ�߳�
	static void T_SimluateGrab();

	//�����ͼ�ڴ��λ
	static BOOL AlignCameraImages(HObject& ho_GrabImage,HTuple&  hv_GrabberSN,HTuple& hv_Encoder,HTuple& hv_ImageSN=HTuple());
public:
	static BYTE* m_pBufferFirstImage[2];
	static BYTE* m_pBufferNextImage[2];
	static BYTE* m_pBufferTilePostion[2];

	CString m_strSimulateGrabPath;		//ģ���ͼ·��
	int		m_nSimulateGrabTime;		//ģ���ͼͣ��ʱ��
	BOOL	m_bSimuateGrabStop;			//ֹͣģ��ɼ�
private:
	int m_nAcqThreadNum;				//�ɼ�ͼ�����������һ�����һ���ɼ�����
	int m_nDetectThreadNum;				//���������������һ�������һ������߳�
	int m_nMergeThreadNum;				//������ʾ�������������һ����Ʒһ���ϲ�����ֳ�

	HTupleVector m_ImageQueues;			//�ɼ�ͼ�����
	HTupleVector m_ResultQueues;		//�����������
	HTupleVector m_DisplayQueues;		//�����ʾ����,��������ļ������Ͳ���ۺϽ����ʾ����
	ThreadList	 m_vecThreadHandle;		//�߳̾��

};