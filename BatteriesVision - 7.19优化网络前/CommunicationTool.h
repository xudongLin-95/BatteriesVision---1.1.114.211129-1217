#pragma once
#include "BaseTmpl.h"
#include <vector>
#include "SCIDataFlow.in.h"
#include "SCIRectifcation.in.h"
#include "SCIIO.in.h"
#include "Marking.in.h"
#include "UpsControl.in.h"

class CCommunicationTool : public CBaseTmpl
{
public:
	enum eCommType
	{
		IO = 0,					//IOͨ��
		PLC,					//PLCͨ��
		Rectifcation,			//��ƫͨ��
		MES,					//MESͨ��
		LightController,		//��Դ����������ͨ��
		Marking,				//�����ͨ��
		UPS,					//UPSͨ��
		Temperature,			//�¿���ͨ��
	};
public:
	CCommunicationTool();
	~CCommunicationTool();

public:
	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//��Ϣ���
	void Enqueue(eCommType eType,HTuple& MessageHandle);
	//ͨ�õ��ýӿ�
	int	DoSomething(eCommType eType,void *message,void *wParam,void *lParam);
	//���ò����Ի���
	int ShowParamDlg(eCommType eType,void* wParam=NULL);
	//��ƫ�ջ�
	int Rectify(const C_MergeWork& tOutput);
	//��ƫ��ص�
	static BOOL WINAPI RecificationCallBack(PRECTIFICATIONCALL tCall);

	//ʹ��IO��
	int EnableIOCard(const BOOL& bEnable=TRUE);
	//��ȡIO����Ƶ
	double GetIOCardLineRate();

	//�����ź�,��������״̬
	BOOL RefreshRunStatus();
	//PLC��������ֹ�Ͽ�
	BOOL PLCReconnect();
	//����PLC�Ӿ������ź�
	BOOL PLCCCDEnable(BOOL bEnable);
	//����PLC�����ź�
	BOOL CCDAlarm();
	//����PLCͣ���ź�
	BOOL CCDStop();
	//�ж�PLC�豸���з���,-1����1����0ֹͣ
	int  GetPlcRunDirection();
	//�ж�PLC�豸���˵��ź�,1����ģͷ������0ģͷ�˵�
	int  GetPlcDieHeadDirection();
	//��ȡPLC�豸��ʼͿ��ֹͣͿ�ź�,1Ϳ��0ֹͣͿ
	int  GetPlcStartCoat();
	//��ȡPLC�豸״̬
	void  GetPlcEquipmentSignal();

	//����ӿ�
	BOOL Mark(C_MergeWork& tOutput);

	//�ϴ��豸״̬��Mes
	BOOL UploadDeviceInfoToMes();
private:
	void InitCommLibs();

	//����������ƫ�ջ�����
	int SendCutRectifyValue(const C_MergeWork& tOutput);
	//���Ͷ�����ƫ�ջ�����
	int SendAT9RectifyValue(const C_MergeWork& tOutput);

public:
	int m_nTempertatureNums;            //�¶ȴ������ĸ���
	int m_nPlcRunDirection;				//PLC�豸���з���,-1����1����, 0ֹͣ
	int m_nPlcDieHeadDirection;			//PLC�豸���˵��ź�,1����ģͷ������0ģͷ�˵�
	int m_nPlcRunStart;					//PLC�豸�������źţ�1������0ֹͣ
	int m_nPlcStartCoat;				//PLC�豸Ϳ���ĵ��źţ�1������0ֹͣ
	double m_dPlcDieHeadPosition;		//PLC�豸ģͷ�ƶ�λ��

private:
	DHInterface* m_pIO;					//IOͨ��ģ��
	DHInterface* m_pPLC;				//PLCͨ��ģ��
	//DHInterface* m_pRectification;	//��ƫͨ��ģ��
	DHInterface* m_pMES;				//MESͨ��ģ��
	DHInterface* m_pLightControl;		//��Դ������ͨ��ģ��
	DHInterface* m_pMarkingControl;		//����ͨ��ģ��
	DHInterface* m_pUPSControl;			//UPSͨ��ģ��
	DHInterface* m_pTemperature;		//�¿���ͨ��ģ��

};