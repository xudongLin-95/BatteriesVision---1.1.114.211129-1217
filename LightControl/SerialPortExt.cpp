#include "stdafx.h"
#include "SerialPortExt.h"

CSerialPortExt::CSerialPortExt()
{
	m_bOpen = false;
}

CSerialPortExt::CSerialPortExt(CString strPortName, int nBaudRate, int nBitCount, int nParity, int nStopBits)
{
	m_bOpen = false;
	this->Open(strPortName, nBaudRate, nBitCount, nParity, nStopBits);
}

CSerialPortExt::CSerialPortExt(int nComSN, int nBaudRate, int nBitCount, int nParity, int nStopBits)
{
	m_bOpen = false;
	this->Open(nComSN, nBaudRate, nBitCount, nParity, nStopBits);
}

CSerialPortExt::~CSerialPortExt()
{
	this->Close();
}

bool CSerialPortExt::Open(int nComSN, int nBaudRate, int nBitCount, int nParity, int nStopBits)
{
	CString strCom;
	strCom.Format("COM%d", nComSN);
	return Open(strCom, nBaudRate, nBitCount, nParity, nStopBits);
}

bool CSerialPortExt::Open(CString strPortName, int nBaudRate, int nBitCount, int nParity, int nStopBits)
{
	if (m_bOpen) this->Close();
	
	//CString strPort = "\\\\.\\" + strPortName;
	CString strPort = strPortName + ":";
	m_hComm = CreateFile(_T(strPort), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
	int error = GetLastError();

	if (m_hComm == INVALID_HANDLE_VALUE)        return false;
	if (!SetupComm(m_hComm, 1024, 512))        return false;

	COMMTIMEOUTS commtimeouts;
	commtimeouts.ReadIntervalTimeout = MAXDWORD;
	commtimeouts.ReadTotalTimeoutConstant = 0;
	commtimeouts.ReadTotalTimeoutMultiplier = 0;
	commtimeouts.WriteTotalTimeoutConstant = 0;
	commtimeouts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(m_hComm, &commtimeouts))        return false;

	memset(&m_tReadovReady, 0, sizeof(OVERLAPPED));
	memset(&m_tWriteovReady, 0, sizeof(OVERLAPPED));
	m_tReadovReady.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
	m_tWriteovReady.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	DCB dcb;
	GetCommState(m_hComm, &dcb);
	//dcb.fBinary = TRUE;
	//dcb.fParity = TRUE;
	// 波特率  数据位  标志位 根据自己的设备在此做修改 
	dcb.BaudRate = nBaudRate;        // baud rate
	dcb.ByteSize = nBitCount;
	dcb.Parity = nParity;
	dcb.StopBits = nStopBits - 1;//bytesize为8时，这里停止位为0，宏定义不同

	if (!SetCommState(m_hComm, &dcb))        return false;

	m_bOpen = true;
	return m_bOpen;
}

bool CSerialPortExt::Write(const CString &strSendData)
{
	if (!m_bOpen)    return false;

	DWORD    BytesSent;
	DWORD    resD;

	CString strSend = strSendData;
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	BytesSent = 0;
	BOOL hr = WriteFile(m_hComm,                            // Handle to COMM Port
		(char*)strSend.GetBuffer(),							// Pointer to message buffer in calling finction
		strSend.GetLength(),								// Length of message to send
		&BytesSent,											// Where to store the number of bytes sent
		&m_tWriteovReady);									// Overlapped structure
	strSend.ReleaseBuffer();
	if (!hr)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			return false;
		}
		else
		{
			resD = WaitForSingleObject(m_tWriteovReady.hEvent, READ_TIMEOUT);
		}
		switch (resD)
		{
		case WAIT_OBJECT_0:
		{
			if (!GetOverlappedResult(m_hComm, &m_tWriteovReady, &BytesSent, false))
				return false;
			else
				return true;

		}
		default:
			return false;
			break;
		}
	}
	return true;
}

bool CSerialPortExt::Close()
{
	if (!m_bOpen) return false;	

	CloseHandle(m_tReadovReady.hEvent);
	CloseHandle(m_tWriteovReady.hEvent);
	m_tReadovReady.hEvent = NULL;
	m_tWriteovReady.hEvent = NULL;

	if (CloseHandle(m_hComm))
	{
		m_hComm = NULL;
		m_bOpen = false;
		return true;
	}
	else
	{
		m_hComm = NULL;
		return false;
	}
}

bool CSerialPortExt::Read(CString &strReceiveData, DWORD dwReceiveLength)
{
	if (!m_bOpen)    return false;
	if (m_tReadovReady.hEvent == NULL)    return false;

	char chRecv[2048];
	if (ReadFile(m_hComm, chRecv, 2000, &dwReceiveLength, &m_tReadovReady) == FALSE)
	{
		if (GetLastError() != ERROR_IO_PENDING)    return false;
	}
	if (dwReceiveLength == 0)    return false;
	chRecv[dwReceiveLength] = 0;

	DWORD dwRead;
	DWORD dwRes = WaitForSingleObject(m_tReadovReady.hEvent, READ_TIMEOUT);
	strReceiveData.Format("%s", chRecv);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		if (!GetOverlappedResult(m_hComm, &m_tReadovReady, &dwRead, FALSE))    return false;
		break;

	case WAIT_TIMEOUT:
		break;

	default:
		break;
	}
	return true;
}

bool CSerialPortExt::GetComList(std::vector<CString> &vecList)
{
	HKEY hKey;
	vecList.clear();

	LONG nRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"Hardware\\DeviceMap\\SerialComm", NULL,
		KEY_READ, &hKey);
	if (nRetVal == ERROR_SUCCESS)
	{
		int i = 0;
		char portName[256], commName[256];// commName：串口名字
		DWORD dwLong, dwSize;
		while (1)
		{
			dwLong = dwSize = sizeof(portName);
			nRetVal = RegEnumValue(hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize);
			if (nRetVal == ERROR_NO_MORE_ITEMS) // 枚举串口
				break;

			CString strCommName;
			strCommName.Format("%s", commName);
			vecList.push_back(strCommName);
			i++;
		}
		RegCloseKey(hKey);
	}

	if (vecList.size() == 0) return false;
	
	return true;
}

bool CSerialPortExt::GetLightBrightness(const int& nChannel, int &nBrightness)
{
	if (!m_bOpen) return false;
	if (nChannel < 1) return false;

	CString strCmd,strRes;
	strCmd.Format("S%c#", 'A' + (nChannel - 1));

	if (!Write(strCmd)) return false;
	Sleep(50);//需要休眠
	if (!Read(strRes)) return false;

	strRes.Delete(0,1);//b0001, 所以删掉b，再进行转换为整数
	nBrightness = atoi(strRes);
	return true;
}

bool CSerialPortExt::SetLightBrightness(const int &nChannel, int nBrightness)
{
	if (!m_bOpen) return false;
	if (nChannel < 1) return false;
	if (nBrightness < 0) nBrightness = 0;
	if (nBrightness > 255) nBrightness = 255;

	CString strCmd, strRes;
	strCmd.Format("S%c%0.4d#", 'A' + (nChannel - 1), nBrightness);

	if (!Write(strCmd)) return false;
	Sleep(50);
	if (!Read(strRes)) return false;

	//判断有没有设置成功
	if(strRes != _T('A' + (nChannel - 1))) return false;

	return true;
}

bool CSerialPortExt::SetLightStatus(bool bOn)
{
	if (!m_bOpen) return false;

	CString strCmd, strRes;
	if(bOn) strCmd.Format("SH#");//常亮
	else strCmd.Format("SL#");//常灭

	if (!Write(strCmd)) return false;
	Sleep(50);
	if (!Read(strRes)) return false;

	//判断有没有设置成功
	if (-1 == strCmd.Find(strRes)) return false;

	return true;
}

