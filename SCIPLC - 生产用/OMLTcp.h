#pragma once
#include<string>
#include <iostream>
#include <Winsock2.h>
#include <stdint.h>
#pragma comment(lib,"ws2_32.lib")


using namespace std;

typedef struct tag_fins_sys_tp 
{
	char		address[128];
	uint16_t	port;
	SOCKET		sockfd;
	time_t		timeout;
	int		error_count;
	int		error_max;
	int		last_error;
	bool		error_changed;
	uint8_t		local_net;
	uint8_t		local_node;
	uint8_t		local_unit;
	uint8_t		remote_net;
	uint8_t		remote_node;
	uint8_t		remote_unit;
	uint8_t		sid;
	uint8_t		comm_type;
	char		model[21];
	char		version[21];
	int		plc_mode;
}fins_sys_tp;

#define FINS_COMM_TYPE_TCP			0x01			/* The communication protocol is FINS/TCP		*/

typedef int(*FINSTCPCONNECT)(fins_sys_tp *sys, const char *address, uint16_t port, uint8_t local_net, uint8_t local_node, uint8_t local_unit, uint8_t remote_net, uint8_t remote_node, uint8_t remote_unit, int *error_val, int error_max);

typedef int(*FINSTCPCLOSE)(fins_sys_tp* sys);
//read
//bool
typedef int(*FINSTCP_READ_BIT)(fins_sys_tp *sys, const char *start, bool *data, size_t num_bits);
typedef int(*FINSTCP_READ_SBCD16)(fins_sys_tp *sys, const char *start, int16_t *data, size_t num_sbcd16, int type);
typedef int(*FINSTCP_READ_SBCD32)(fins_sys_tp *sys, const char *start, int32_t *data, size_t num_sbcd32, int type);
//int
typedef int(*FINSTCP_READ_INT16)(fins_sys_tp *sys, const char *start, int16_t *data, size_t num_int16);
typedef int(*FINSTCP_READ_INT32)(fins_sys_tp *sys, const char *start, int32_t *data, size_t num_int32);
typedef int(*FINSTCP_READ_UINT16)(fins_sys_tp *sys, const char *start, uint16_t *data, size_t num_uint16);
typedef int(*FINSTCP_READ_UINT32)(fins_sys_tp *sys, const char *start, uint32_t *data, size_t num_uint32);
//float
typedef int(*FINSTCP_READ_REAL)(fins_sys_tp *sys, const char *start, unsigned char *data, size_t num_word);

//write
typedef int(*FINSTCP_WRITE_BIT)(fins_sys_tp *sys, const char *start, const bool *data, size_t num_bit);
typedef int(*FINSTCP_WRITE_SBCD16)(fins_sys_tp *sys, const char *start, const int16_t *data, size_t num_sbcd16, int type);
typedef int(*FINSTCP_WRITE_SBCD32)(fins_sys_tp *sys, const char *start, const int32_t *data, size_t num_sbcd32, int type);
//int
typedef int(*FINSTCP_WRITE_INT16)(fins_sys_tp *sys, const char *start, const int16_t *data, size_t num_int16);
typedef int(*FINSTCP_WRITE_INT32)(fins_sys_tp *sys, const char *start, const int32_t *data, size_t num_int32);
typedef int(*FINSTCP_WRITE_UINT16)(fins_sys_tp *sys, const char *start, const uint16_t *data, size_t num_uint16);
typedef int(*FINSTCP_WRITE_UINT32)(fins_sys_tp *sys, const char *start, const uint32_t *data, size_t num_uint32);


class OMLTcp
{
public:
	OMLTcp();
	~OMLTcp();
	
private:
	FINSTCPCONNECT		m_fnConnect;
	FINSTCPCLOSE		m_fnClose;
	FINSTCP_READ_BIT	m_fnReadBit;
	FINSTCP_READ_SBCD16 m_fnReadBCD16;
	FINSTCP_READ_SBCD32 m_fnReadBCD32;
	FINSTCP_READ_INT16	m_fnReadInt16;
	FINSTCP_READ_INT32	m_fnReadInt32;
	FINSTCP_READ_UINT16	m_fnReadUInt16;
	FINSTCP_READ_UINT32	m_fnReadUInt32;
	FINSTCP_READ_REAL m_fnReadReal16;
	
	FINSTCP_WRITE_BIT	m_fnWriteBit;
	FINSTCP_WRITE_SBCD16 m_fnWriteBCD16;
	FINSTCP_WRITE_SBCD32 m_fnWriteBCD32;
	FINSTCP_WRITE_INT16  m_fnWriteInt16;
	FINSTCP_WRITE_INT32  m_fnWriteInt32;
	FINSTCP_WRITE_UINT16 m_fnWriteUInt16;
	FINSTCP_WRITE_UINT32 m_fnWriteUInt32;

	HINSTANCE hInst;
	fins_sys_tp* m_pSys;

public:
	bool m_bIsConnect;
	//WORD m_test;
	WORD* _ReadRes1;

public:
	bool	Init();
	bool	Connect(string strip,int prot,int timeout=1);
	bool	DisConnect();

	bool	ReadWBit(char* nAddress);
	WORD	ReadMultiBit(char* nAddress);
	int		GetInt16Data(char* nAddress);
	int		GetInt32Data(char* nAddress);
	WORD	GetWordData(char* nAddress);
	DWORD	GetDWordData(char* nAddress);
	float	GetFloatData(char* nAddress);

	bool	WriteWBit(char* nAddress,bool bValue);
	bool	WriteMultiBit(char* nAddress,WORD wValue);
	bool	SetInt16Data(char* nAddress,short nValue);
	bool	SetInt32Data(char* nAddress,int nValue);
	bool	SetWordData(char* nAddress,WORD wValue);
	bool	SetDWordData(char* nAddress,DWORD dwValue);
};

