//////////////////////////////////////////////////////////////////////////
///GrabberTool.h ���ڲɼ������
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CGrabberTool : public CBaseTmpl
{
public:
	enum emGrabberType
	{
		Simulate=0,		//ģ��ɼ�
		DalsaLine,		//dalsa����
		AVT,			//AVT���
		Mer,			//Merˮ�����
	};
	enum emImageCardType
	{
		SimulateCard=0,	//ģ��ɼ���
		Xcelera,		//Xcelera�ɼ���
		Xtium,			//Xtium�ɼ���
		Ethernet,		//��������
		USB,			//USB��
		Card1394,		//1394�ɼ���
	};
	struct tagGrabber
	{
		DHInterface* pGrabber;
		emGrabberType eGrabberType;
		emImageCardType eCardType;
	};
	typedef std::vector<tagGrabber> GrabberList;
public:
	CGrabberTool();
	~CGrabberTool();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//��ʼ�ɼ�
	BOOL StartGrab();
	//ֹͣ�ɼ�
	BOOL StopGrab();
	//ֹͣģ��ɼ�
	BOOL StopSimGrab();//ֹͣģ��ɼ�����Ӱ��ʵʱ�ɼ�
	//��֡�ɼ�
	BOOL Snapshot();
	//�رղɼ�
	BOOL CloseGrab();

	int GetGrabberCount(){return m_nGrabberCount;}

	int	DoSomething(const int& nCamSN,void *message,void *wParam,void *lParam);

	int ShowParamDlg(const int& nCamSN,void* wParam=NULL);

	int GetGrabberType(const int& nCamSN){
		if(nCamSN<0 || nCamSN>=m_vecGrabber.size())
			return 0;
		return (int)m_vecGrabber[nCamSN].eGrabberType;
	}

private:
	int m_nGrabberCount;
	GrabberList m_vecGrabber;
	
};