//////////////////////////////////////////////////////////////////////////
////SerialPortExt.h ����ͨ���� SW 2020.10.11
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>

class CSerialPortExt
{
public:
#define READ_TIMEOUT  100

public:
	CSerialPortExt();
	CSerialPortExt(CString strPortName, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);
	CSerialPortExt(int nComSN, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);

	~CSerialPortExt();

public:
	//�򿪴��� nBaudRate������ nBitCount����λ nParityУ��λ nStopBitsֹͣλ
	bool Open(CString strPortName, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);
	bool Open(int nComSN, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);

	//�رմ���
	bool Close();

	//��ȡ������Ϣ
	bool Read(CString &strReceiveData, DWORD dwReceiveLength = 0);

	//д�봮����Ϣ
	bool Write(const CString &strSendData);

	//��ȡ�豸�����б�
	bool GetComList(std::vector<CString> &vecComList);

	//��ȡ��Դ����(��Դ������)
	bool GetLightBrightness(const int &nChannel, int &nBrightness);

	//���ù�Դ����(��Դ������),���ȷ�Χ0~255
	bool SetLightBrightness(const int &nChannel, int nBrightness);

	//���ù�Դ����״̬(��Դ������) ��������
	bool SetLightStatus(bool bOn = true);

	bool IsOpen() { return m_bOpen; }

private:
	OVERLAPPED m_tReadovReady, m_tWriteovReady;
	HANDLE m_hComm;
	bool m_bOpen;
};

