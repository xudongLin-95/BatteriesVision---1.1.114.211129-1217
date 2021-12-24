#pragma once
#include "BaseTmpl.h"

class CDetectionFlow : public CBaseTmpl
{
public:
	CDetectionFlow();
	~CDetectionFlow();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

private:

protected:
};