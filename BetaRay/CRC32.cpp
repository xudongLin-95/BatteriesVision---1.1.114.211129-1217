#include "StdAfx.h"
#include "CRC32.h"

CCRC32::CCRC32() {

}

CCRC32::~CCRC32() {
}

UINT32 CCRC32::GetCRC32(unsigned char* buf, int nLength)
{
	if (nLength < 1)
		return 0xffffffff;

	#define INIT  0xffffffff
	#define XOROT 0xffffffff

	UINT32 uCRCValue;
	unsigned char* pData;

	/* init the start value */
	uCRCValue = INIT;
	pData = buf;

	/* calculate CRC */
	while (nLength--)
	{
		uCRCValue = crc32_tab[(uCRCValue ^ *pData++) & 0xFF] ^ (uCRCValue >> 8);
	}
	/* XOR the output value */
	return uCRCValue ^ XOROT;

	/*UINT32 crc = 0;
	unsigned char* data = buf;
	for (int i = nLength; i--;) {
		crc = (UINT32)(crc32_tab[*data ^ ((crc >> 24) & 0xFF)] ^ (crc << 8));
		data++;
	}
	return crc;*/
}