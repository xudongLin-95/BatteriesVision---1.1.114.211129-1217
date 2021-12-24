#include "StdAfx.h"
#include "BaseFunction.h"

BOOL BaseFunction::CheckIllageChar(CString strString)
{
	int nBadNum = 0;
	if (strString.GetLength() <= 0) return FALSE;
	
	for (int i = 0; i < strString.GetLength(); i++)
	{
		if (strString.GetAt(i) == '\\' ||
			strString.GetAt(i) == '/' ||
			strString.GetAt(i) == ':' ||
			strString.GetAt(i) == '*' ||
			strString.GetAt(i) == '?' ||
			strString.GetAt(i) == '"' ||
			strString.GetAt(i) == '>' ||
			strString.GetAt(i) == '<' ||
			strString.GetAt(i) == '|' )
			nBadNum++;

	}

	if (nBadNum > 0) return TRUE;
	else return FALSE;
}