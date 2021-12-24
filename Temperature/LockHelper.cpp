#include "StdAfx.h"
#include "LockHelper.h"


CLockHelper::CLockHelper(CRITICAL_SECTION *pcs)
{
	m_pcs = pcs;
	EnterCriticalSection(m_pcs);
}


CLockHelper::~CLockHelper(void)
{
	LeaveCriticalSection(m_pcs);
	m_pcs = NULL;
}
