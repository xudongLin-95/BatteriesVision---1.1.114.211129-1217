#include "stdafx.h"
#include "DHInterFace.h"
#include "DHSocket.h"

// ͨ������������ʽ����÷��ṩָ�����������Ļ���ָ��
extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CDHSocket();
}