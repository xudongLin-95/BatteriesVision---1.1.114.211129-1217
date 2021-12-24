//////////////////////////////////////////////////////////////////////////
///ErrorInfo.h �������������(����ͻ�)����������룬��ȡ������Ϣ�ͽ������
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "ErrorCode.in.h"
#include "BaseTmpl.h"
#include "tinyxml.h"
#include <vector>

using std::vector;

class CErrorInfo : public CBaseTmpl
{
public:
	struct tagErrorInfo
	{
		int nID;				//���
		CString strDescribe;	//����
		CString strSolution;	//�������
	};

public:
	CErrorInfo();
	~CErrorInfo();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//��ȡ������Ϣ
	tagErrorInfo GetErrorInfo(const int& nErrroCode, const vector<tagErrorInfo>& vecAllErrorInfos);

	//����Xml�ļ�
	BOOL CreatXml(const char* filePath, TiXmlDocument *CreatDoc, CString RootElemnet);
	//��ȡ������Ϣ
	BOOL GetErrorCodeList(const char* filePath, vector<tagErrorInfo> &vecAllErrorInfos);
	//����������Ϣ������Xml�ļ�
	BOOL SetErrorCode(const char* filePath, vector<tagErrorInfo> vecAllErrorInfos);

};
