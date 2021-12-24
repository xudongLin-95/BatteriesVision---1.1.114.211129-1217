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

#define MAX_QUEUE_NUM 50	//最大队列元素个数

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

	static void WINAPI AcquisitionCallBack(const s_GBSIGNALINFO* SigInfo);//采集回调

	//开始模拟采集
	void StartSimulateGrab();
	void StopSimulateGrab() { m_bSimuateGrabStop = TRUE; }

	//监听自动换卷
	void ListenChangeBatchAuto();
private:
	//检测线程
	static void* HThreadDetect(void *hcthread);
	//合并结果线程，最后显示结果
	static void* HThreadMerge(void *hcthread);
	//监听线程
	static void* HThreadListen(void *hcthread);
	//仿真采图线程
	static void* HThreadSimluateGrab(void *hcthread);

	//采集回调
	static void WINAPI T_Grab(const s_GBSIGNALINFO* SigInfo,HTupleVector hvec_QueueImages);
	//测量线程
	static void T_Detect(HTupleVector hvec_QueueImages, HTupleVector hvec_QueueResults, HTuple hv_ThreadIndex);
	//分配者线程，最后显示结果
	static void T_Merge(HTupleVector hvc_QueueResults,HTuple hv_QueueDisplay,HTuple hv_ThreadIndex);
	//监听线程，发送心跳信号，CCD设备状态监听，防呆逻辑
	static void T_Listen();
	//仿真采图线程
	static void T_SimluateGrab();

	//相机采图内存对位
	static BOOL AlignCameraImages(HObject& ho_GrabImage,HTuple&  hv_GrabberSN,HTuple& hv_Encoder,HTuple& hv_ImageSN=HTuple());
public:
	static BYTE* m_pBufferFirstImage[2];
	static BYTE* m_pBufferNextImage[2];
	static BYTE* m_pBufferTilePostion[2];

	CString m_strSimulateGrabPath;		//模拟采图路径
	int		m_nSimulateGrabTime;		//模拟采图停顿时间
	BOOL	m_bSimuateGrabStop;			//停止模拟采集
private:
	int m_nAcqThreadNum;				//采集图像队列数量，一个相机一个采集队列
	int m_nDetectThreadNum;				//检测结果队列数量，一个检测面一个检测线程
	int m_nMergeThreadNum;				//最终显示结果队列数量，一个产品一个合并结果现场

	HTupleVector m_ImageQueues;			//采集图像队列
	HTupleVector m_ResultQueues;		//测量结果队列
	HTupleVector m_DisplayQueues;		//结果显示队列,包含两侧的极耳检测和测宽综合结果显示队列
	ThreadList	 m_vecThreadHandle;		//线程句柄

};