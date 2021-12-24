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


////////////////////////////////////////////////////
int CSerialPortExt::char2int(char ch)
{
	int decnum(0);
	if (ch >= 'A'&&ch <= 'Z')
	{
		decnum = ch - 55;
	}
	else if (ch >= 'a'&&ch <= 'z')
	{
		decnum = ch - 87;
	}
	else if (ch >= '0'&&ch <= '9')
	{
		decnum = ch - 48;
	}
	return decnum;
}

int CSerialPortExt::hex2dec(std::string str)
{
	int decnum(0), len(0);
	const char* ch = str.c_str();
	for (int i = 0; i<strlen(ch); ++i)
	{
		len = strlen(ch) - 1 - i;
		decnum += char2int(ch[i])*pow(16.0, len);
	}
	return decnum;
}

const std::string CSerialPortExt::toHexString(const unsigned char* input, const int datasize)

{
	std::string output;
	char ch[3];

	for (int i = 0; i < datasize; ++i)
	{
		sprintf_s(ch, 3, "%02x", input[i]);
		output += ch;
	}
	return output;
}

//CRC校验的函数
unsigned short CSerialPortExt::CRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
	try
	{
		unsigned char uchCRCHi = 0xFF; /* 高CRC字节初始化 */
		unsigned char uchCRCLo = 0xFF; /* 低CRC字节初始化 */
		unsigned uIndex; /* CRC循环中的索引 */
		while (usDataLen--) /* 传输消息缓冲区 */
		{
			uIndex = uchCRCHi ^ *puchMsg++; /* 计算CRC */
			uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
			uchCRCLo = auchCRCLo[uIndex];
		}
		return (uchCRCHi << 8 | uchCRCLo);
	}
	catch (const std::exception&)
	{
		return -1;
	}
	catch (...)
	{
		return -1;
	}
}
//写数据接口
bool CSerialPortExt::Write(const unsigned char *puchMsg)
{
	try
	{
		if (!m_bOpen)    return false;

		DWORD    BytesSent;
		DWORD    resD;

		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		BytesSent = 0;
		BOOL hr = WriteFile(m_hComm,                            // Handle to COMM Port
			puchMsg,							// Pointer to message buffer in calling finction
			sizeof(puchMsg),								// Length of message to send
			&BytesSent,											// Where to store the number of bytes sent
			&m_tWriteovReady);									// Overlapped structure
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
	catch (const std::exception&)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
}
//读数据接口
bool CSerialPortExt::Read(unsigned char *puchMsg, DWORD dwReceiveLength)
{
	try
	{
		if (!m_bOpen)    return false;
		if (m_tReadovReady.hEvent == NULL)    return false;

		//char chRecv[2048];
		if (ReadFile(m_hComm, puchMsg, 2000, &dwReceiveLength, &m_tReadovReady) == FALSE)
		{
			if (GetLastError() != ERROR_IO_PENDING)    return false;
		}
		if (dwReceiveLength == 0)    return false;

		DWORD dwRead;
		DWORD dwRes = WaitForSingleObject(m_tReadovReady.hEvent, READ_TIMEOUT);
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
	catch (const std::exception&)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
}
//读取当前温度
bool CSerialPortExt::GetTemperature(unsigned char rtu,  float &temp)
{
	try
	{
		if (!m_bOpen) return false;
		//四字节： 01 03 00 00 00 02 c4 0b
		//两字节： 01 03 20 00 00 01 8F CA
		unsigned char sendCmd[8];	
		unsigned short crc16, crctemp;//进行CRC校验
		sendCmd[0] = rtu;
		sendCmd[1] = 0x03;
		sendCmd[2] = 0x20;
		sendCmd[3] = 0x00;
		sendCmd[4] = 0x00;
		sendCmd[5] = 0x01;
		crc16 = CRC16(sendCmd, 6);
		sendCmd[6] = (unsigned char)(crc16 >>8);
		sendCmd[7] = (unsigned char)crc16;

		unsigned char recvCmd[7];
		if (!Write(sendCmd)) return false;
		Sleep(300);
		if (!Read(recvCmd, sizeof(recvCmd))) return false;

		//根据回复消息，计算CRC校验
		unsigned char crcCmd[7];
		memcpy(crcCmd, recvCmd, sizeof(unsigned char) * (sizeof(recvCmd) - 2));
		crc16 = CRC16(crcCmd, sizeof(crcCmd));
		//获取校验结果
		crctemp = recvCmd[sizeof(recvCmd) - 2];
		crctemp = crctemp << 8 | recvCmd[sizeof(recvCmd) - 1];
		//校验结果与温控器自校验结果对比
		if (crctemp == crc16)
		{
			//获取数值位
			unsigned char hex[2] = { recvCmd[3],recvCmd[4] };
			//将16进制数值转为16进制字符串
			std::string str = toHexString(hex, 2);
			//求出当前温度
			temp = hex2dec(str)/10;

			return true;
		}
		else
		{
			return false;
		}
	}
	catch (const std::exception&)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
}

//读取多个温度
bool CSerialPortExt::GetTemperatureList(std::vector<float> &tempList)
{
	try
	{
		if (!m_bOpen) return false;
		std::vector<unsigned char> rtuList(tempList.size());
		//四字节： 01 03 00 00 00 02 c4 0b
		//两字节： 01 03 20 00 00 01 8F CA
		unsigned char sendCmd[8];
		unsigned short crc16 = 0, crctemp = 0;//进行CRC校验

		for (int i = 0; i < tempList.size(); i++)
		{
			//rtuList.push_back();
		}
		sendCmd[1] = 0x03;
		sendCmd[2] = 0x20;
		sendCmd[3] = 0x00;
		sendCmd[4] = 0x00;
		sendCmd[5] = 0x01;
		for (int i = 0; i < tempList.size(); i++)
		{
			sendCmd[0] = rtuList[i];
			crc16 = CRC16(sendCmd, 6);
			sendCmd[6] = (unsigned char)(crc16 >> 8);
			sendCmd[7] = (unsigned char)crc16;

			unsigned char recvCmd[7];
			if (!Write(sendCmd)) return false;
			Sleep(300);
			if (!Read(recvCmd, sizeof(recvCmd))) return false;

			//根据回复消息，计算CRC校验
			unsigned char crcCmd[7];
			memcpy(crcCmd, recvCmd, sizeof(unsigned char) * (sizeof(recvCmd) - 2));
			crc16 = CRC16(crcCmd, sizeof(crcCmd));
			//获取校验结果
			crctemp = recvCmd[sizeof(recvCmd) - 2];
			crctemp = crctemp << 8 | recvCmd[sizeof(recvCmd) - 1];
			//校验结果与温控器自校验结果对比
			if (crctemp == crc16)
			{
				//获取数值位
				unsigned char hex[2] = { recvCmd[3],recvCmd[4] };
				//将16进制数值转为16进制字符串
				std::string str = toHexString(hex, 2);
				//求出当前温度
				tempList[i] = hex2dec(str) / 10;

				return true;
			}
			else
			{
				return false;
			}
		}		
	}
	catch (const std::exception&)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
}


