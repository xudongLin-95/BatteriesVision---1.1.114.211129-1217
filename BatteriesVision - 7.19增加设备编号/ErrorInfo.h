//////////////////////////////////////////////////////////////////////////
///ErrorInfo.h 定义软件错误码(面像客户)，传入错误码，获取错误信息和解决方案
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
		int nID;				//编号
		CString strDescribe;	//描述
		CString strSolution;	//解决方案
	};

public:
	CErrorInfo();
	~CErrorInfo();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//获取错误信息
	tagErrorInfo GetErrorInfo(const int& nErrroCode, const vector<tagErrorInfo>& vecAllErrorInfos);

	//创建Xml文件
	BOOL CreatXml(const char* filePath, TiXmlDocument *CreatDoc, CString RootElemnet);
	//获取所有信息
	BOOL GetErrorCodeList(const char* filePath, vector<tagErrorInfo> &vecAllErrorInfos);
	//保存所有信息到本地Xml文件
	BOOL SetErrorCode(const char* filePath, vector<tagErrorInfo> vecAllErrorInfos);

};
