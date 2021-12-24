#if !defined(DHGrabber_h_)
#define DHGrabber_h_
const int GBMaxFileNameLen = 255;
const int GBMaxTextLen = 255;
enum eCameraLinkConfigration
{
	CL_Base=0,	//Baseģʽ
	CL_Medium,	//Mediumģʽ
	CL_Full,	//Fullģʽ
};
//�ص���Ϣ�ṹ
typedef struct _GBSignalInfoStruct
{
	PVOID Context;								//���ӵ�лص������Ķ����thisָ��
	int iGrabberTypeSN;							// �ɼ������ͱ��
	int nGrabberSN;								//�ɼ������
	int nErrorCode;								//������루0��������
	int nFrameCount;							//�Ѳɼ���֡����
	char strDescription[GBMaxTextLen];			//������Ϣ
	int iReserve1;								// �����ֶ�1
	int iReserve2;								// �����ֶ�2	
}s_GBSIGNALINFO;//�ص���Ϣ�ṹ
//�ص�����ָ������
typedef void (WINAPI *PGBCALLBACK)(const s_GBSIGNALINFO* SignalInfo);
//�ɼ�����ʼ���ṹ
typedef struct _GBInitStruct
{
	int iGrabberTypeSN;						// �ɼ������ͱ��
	int nGrabberSN;							//�ɼ������
    char strDeviceName[GBMaxTextLen];		//�豸����
	char strDeviceMark[GBMaxTextLen];		// �豸��ʶ
	char strGrabberFile[GBMaxFileNameLen];	//���ڳ�ʼ�����ļ�
	PGBCALLBACK CallBackFunc;				//�ص�����ָ��
	PVOID Context;							//���ӵ�лص������Ķ����thisָ��
	int iReserve1;							// �����ֶ�1�����ڴ���CameraLinkģʽ��Base��Medium��Full
	int iReserve2;							// �����ֶ�2
}s_GBINITSTRUCT;//�ɼ�����ʼ���ṹ
//������Ϣ�ṹ
typedef struct _GBErrorInfoStruct
{
   	int nErrorCode;							//������루0��������
	char strErrorDescription[GBMaxTextLen];	//��������
	char strErrorRemark[GBMaxTextLen];		//������Ϣ
}s_GBERRORINFO;//������Ϣ�ṹ
//��������
enum GBParamID
{
	GBImageWidth = 0,		//ͼ���� (��λ:�ֽ�)
	GBImageHeight,			//ͼ��߶� (��λ:�ֽ�)
	GBImagePixelSize,		//ͼ�����ش�С (��λ:�ֽ�)
	GBImageBufferSize,		//ͼ�񻺳�����С (��λ:�ֽ�)
	GBImageBufferAddr,		//ͼ�񻺳�����ַ
	GBGrabberTypeSN			//�ɼ������ͱ��
};
//������붨��
const int GBOK = 0;
#endif// !defined(DHGrabber_h_)
