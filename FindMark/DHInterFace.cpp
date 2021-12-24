#include "stdafx.h"
#include "Find.h"
#include "DHInterFace.h"

// 通过导出函数形式向调用方提供指向派生类对象的基类指针
extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CFindMark();
}