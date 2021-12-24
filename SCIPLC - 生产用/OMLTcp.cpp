#include "stdafx.h"
#include "OMLTcp.h"

OMLTcp::OMLTcp()
{
	m_bIsConnect = false;
}

OMLTcp::~OMLTcp()
{
	if (m_bIsConnect)
	{
		m_bIsConnect = false;
		m_fnClose(m_pSys);
	}
	FreeLibrary(hInst);
}

bool OMLTcp::Init()
{
	hInst = LoadLibraryA("FINSTcp.dll");
	if (NULL == hInst)
	{
		return false;
	}

	m_fnConnect = (FINSTCPCONNECT)(GetProcAddress(hInst, "finslib_tcp_connect"));
	if (NULL == m_fnConnect)
	{
		return false;
	}

	m_fnClose = (FINSTCPCLOSE)(GetProcAddress(hInst,"finslib_disconnect"));
	if (NULL == m_fnClose)
	{
		return false;
	}

	m_fnReadBit = (FINSTCP_READ_BIT)(GetProcAddress(hInst,"finslib_memory_area_read_bit"));
	if (NULL == m_fnReadBit)
	{
		return false;
	}

	m_fnReadBCD16 = (FINSTCP_READ_SBCD16)(GetProcAddress(hInst,"finslib_memory_area_read_sbcd16"));
	if (NULL == m_fnReadBCD16)
	{
		return false;
	}

	m_fnReadBCD32 = (FINSTCP_READ_SBCD32)(GetProcAddress(hInst,"finslib_memory_area_read_sbcd32"));
	if (NULL == m_fnReadBCD32)
	{
		return false;
	}

	m_fnReadInt16 = (FINSTCP_READ_INT16)(GetProcAddress(hInst,"finslib_memory_area_read_int16"));
	if (NULL == m_fnReadInt16)
	{
		return false;
	}

	m_fnReadInt32 = (FINSTCP_READ_INT32)(GetProcAddress(hInst,"finslib_memory_area_read_int32"));
	if (NULL == m_fnReadInt32)
	{
		return false;
	}

	m_fnReadUInt16 = (FINSTCP_READ_UINT16)(GetProcAddress(hInst,"finslib_memory_area_read_uint16"));
	if (NULL == m_fnReadUInt16)
	{
		return false;
	}

	m_fnReadUInt32 = (FINSTCP_READ_UINT32)(GetProcAddress(hInst,"finslib_memory_area_read_uint32"));
	if (NULL == m_fnReadUInt32)
	{
		return false;
	}

	m_fnWriteBit = (FINSTCP_WRITE_BIT)(GetProcAddress(hInst,"finslib_memory_area_write_bit"));
	if (NULL == m_fnWriteBit)
	{
		return false;
	}

	m_fnWriteBCD16 = (FINSTCP_WRITE_SBCD16)(GetProcAddress(hInst,"finslib_memory_area_write_sbcd16"));
	if (NULL == m_fnWriteBCD16)
	{
		return false;
	}

	m_fnWriteBCD32 = (FINSTCP_WRITE_SBCD32)(GetProcAddress(hInst,"finslib_memory_area_write_sbcd32"));
	if (NULL == m_fnWriteBCD32)
	{
		return false;
	}

	m_fnWriteInt16 = (FINSTCP_WRITE_INT16)(GetProcAddress(hInst,"finslib_memory_area_write_int16"));
	if (NULL == m_fnWriteInt16)
	{
		return false;
	}

	m_fnWriteInt32 = (FINSTCP_WRITE_INT32)(GetProcAddress(hInst,"finslib_memory_area_write_int32"));
	if (NULL == m_fnWriteInt32)
	{
		return false;
	}

	m_fnWriteUInt16 = (FINSTCP_WRITE_UINT16)(GetProcAddress(hInst,"finslib_memory_area_write_uint16"));
	if (NULL == m_fnWriteUInt16)
	{
		return false;
	}

	m_fnWriteUInt32 = (FINSTCP_WRITE_UINT32)(GetProcAddress(hInst,"finslib_memory_area_write_uint32"));
	if (NULL == m_fnWriteUInt32)
	{
		return false;
	}

	m_fnReadReal16 = (FINSTCP_READ_REAL)(GetProcAddress(hInst, "finslib_memory_area_read_word"));

	m_pSys = new fins_sys_tp;
	memset(m_pSys, 0, sizeof(fins_sys_tp));
	m_pSys->remote_net = 0;
	m_pSys->remote_node = 1;
	m_pSys->remote_unit = 0;
	m_pSys->local_net = 0;
	m_pSys->local_node = 3;
	m_pSys->local_unit = 0;
	m_pSys->comm_type = FINS_COMM_TYPE_TCP;
	m_pSys->plc_mode = 2;
	m_pSys->timeout = 0;
	_ReadRes1 = new WORD;
	return true;
}

bool OMLTcp::Connect(string strip, int prot, int timeout/*=1*/)
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to WSAStartup\r\n");
		return false;
	}
	unsigned long ul = 1;
	ioctlsocket(m_pSys->sockfd, FIONBIO, &ul); 
	memcpy(m_pSys->address, strip.data(), strlen(strip.data()));
	m_pSys->port = 9600;
	int nError = 0;
	m_fnConnect(m_pSys, m_pSys->address, prot, 0, 1, 0, 0, 3, 0, &nError, 255);
	if (nError == 0)
	{
		m_bIsConnect = true;
		return true;
	}
	else
	{
		return false;
	}
}

bool OMLTcp::DisConnect()
{
	if(m_bIsConnect)
		m_fnClose(m_pSys);
	m_bIsConnect = false;
	return true;
}

bool OMLTcp::ReadWBit(char* nAddress)
{
	bool _ReadRes = false;
	if (0 != m_fnReadBit(m_pSys, nAddress, &_ReadRes, sizeof(bool))) //
	{
		DisConnect();
		return false;
	}
	return _ReadRes;
}

WORD OMLTcp::ReadMultiBit(char* nAddress)
{
	int _ReadRes[16] = { 0 };
	if (0 != m_fnReadBCD32(m_pSys, nAddress, _ReadRes, 16, 5))
	{
		DisConnect();
		return 0;
	}
	WORD _data = 0;
	for (int i = 0; i < 16; i++)
	{
		_data = (_data | _ReadRes[i]) << 1;
	}
	return _data;
}

int OMLTcp::GetInt16Data(char* nAddress)
{
	short* _ReadRes = new short;
	memset(_ReadRes, 0, sizeof(short));
	if (0 != m_fnReadInt16(m_pSys, nAddress, _ReadRes,sizeof(short)))
	{
		DisConnect();
		return 0;
	}
	return *_ReadRes;
}

int OMLTcp::GetInt32Data(char* nAddress)
{
	int* _ReadRes = new int;
	memset(_ReadRes, 0, sizeof(int));
	if (0 != m_fnReadInt32(m_pSys, nAddress, _ReadRes, sizeof(int)))
	{
		DisConnect();
		return 0;
	}
	return *_ReadRes;
}

WORD OMLTcp::GetWordData(char* nAddress)
{
	if (0 != m_fnReadUInt16(m_pSys, nAddress, _ReadRes1, sizeof(WORD)))
	{
		DisConnect();
		return 0;
	}
	return *_ReadRes1;
}

DWORD OMLTcp::GetDWordData(char* nAddress)
{
	unsigned int* _ReadRes = new unsigned int;
	memset(_ReadRes, 0, sizeof(WORD));
	if (0 != m_fnReadUInt32(m_pSys, nAddress, _ReadRes, sizeof(unsigned int)))
	{
		DisConnect();
		return 0;
	}
	return *_ReadRes;
}

float OMLTcp::GetFloatData(char* nAddress)
{
	unsigned char cRes[20] = {0};
	if (0 != m_fnReadReal16(m_pSys, nAddress, cRes, 20))
	{
		DisConnect();
		return 0;
	}
	
	char cRes2[20] = { 0 };
	memcpy(cRes2, cRes, 20);
	return atof(cRes2);
}

bool OMLTcp::WriteWBit(char* nAddress, bool bValue)
{
	if (0 == m_fnWriteBit(m_pSys, nAddress, &bValue, sizeof(bool)))
		return true;
	else
		return false;
}

bool OMLTcp::WriteMultiBit(char* nAddress, WORD wValue)
{
	short _Write[16] = {0};
	for (int i = 15; i >= 0; i--) //按二进制存放到数组中
	{
		_Write[i] = wValue % 2;
		if((wValue /= 2) == 0)
			break;
	}
	if (0 == m_fnWriteBCD16(m_pSys, nAddress, _Write, 16, 5))
		return true;
	else
		return false;
}

bool OMLTcp::SetInt16Data(char* nAddress, short nValue)
{
	if (0 == m_fnWriteInt16(m_pSys, nAddress, &nValue, sizeof(short)))
		return true;
	else
		return false;
}

bool OMLTcp::SetInt32Data(char* nAddress, int nValue)
{
	if (0 == m_fnWriteInt32(m_pSys, nAddress, &nValue, sizeof(int)))
		return true;
	else
		return false;
}

bool OMLTcp::SetWordData(char* nAddress, WORD wValue)
{
	if (0 == m_fnWriteUInt16(m_pSys, nAddress, &wValue, sizeof(WORD)))
		return true;
	else
		return false;
}

bool OMLTcp::SetDWordData(char* nAddress, DWORD dwValue)
{
	if (0 == m_fnWriteUInt32(m_pSys, nAddress, (unsigned int*)(&dwValue), sizeof(DWORD)))
		return true;
	else
		return false;
	return true;
}
