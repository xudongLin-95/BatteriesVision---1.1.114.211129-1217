#pragma once
class CLockHelper
{
private:
	CRITICAL_SECTION *m_pcs;
public:
	CLockHelper(CRITICAL_SECTION *pcs);
	~CLockHelper(void);
};

