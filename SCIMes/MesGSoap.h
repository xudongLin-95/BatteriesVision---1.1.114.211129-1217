#pragma once
#include "soapStub.h"
#include "MesDataType.h"
enum  EMESOperate
{
	OperatorLogin,
	Heartbeat,
	State,
	DownReason,
	Alarm,
	GetRecipeList,
	GetRecipe,
	RecipeVersionExamine,
	Recipe,
	ToolBind,
	ToolLife,
	EquipmentProcessParam,
	Feeding,
	Inbound,
	Outbound,
	EquipmentProductProcessParam,
	BindSFC,
	InboundInContainer,
	BindContainer,
	OutboundInSFCOutputQty,
	FeedingDJInLaminating,
	ExamineExists,
	OutboundInLaminating,
	GenerateCellSFC,
	FillingData,
	InboundInTwoInjection,
	InboundInSFCContainer,
	InboundMore,
	OutboundMore,
	CCDFileUploadComplete
};

class CMesGSoap
{
private:
	CStringW m_EquUserID;
	CStringW m_EquPassword;
	CStringW m_OperatorUserID;
	CStringW m_OperatorPassword;
	CStringW m_EquipmentCode;
	CStringW m_ResourceCode;
	BOOL	m_bConnected;
	CString m_cstrMesIP;
	int m_nMesPort;
	std::map <EMESOperate,CString> m_urlMap;
	CRITICAL_SECTION m_csSoap;

private:
	CMesGSoap(void);
	static CMesGSoap* m_instance;
	inline const char* getEndpoint(const EMESOperate& op);
	inline void loadConfig();
	inline void setUrlArray();
	template <typename T> // 也不需要用泛型；避免传入的是不一样的Soap
	void setupSoap(T& soap);
	void setupSoaps();
	ns1__LoginSoapHeader getLoginHeader();
	ns1__LoginSoapHeader getHeader();
public:
	~CMesGSoap(void); 
	BOOL	isConnected();
	CString GetCurrentLocalTime();
	CString GetCurrentLocalTime(const time_t & tTime);
	CString GetSystemLocalTime();
	static CMesGSoap& getInstance(); 
	void setUserInfo(const CString& vEquUserID, const CString& vEquPassword);
	void setOperatorInfo(const CString& vOperatorUserID, const CString& vOperatorPassword);
	void setEquipmentInfo(const CString& vEquipmentCode, const CString& vResourceCode);
	void setInfo(const CString& vEquUserID, const CString& vEquPassword, const CString& vOperatorUserID,
				 const CString& vOperatorPassword,const CString& vEquipmentCode,const CString& vResourceCode);
	void setMesAddrAndPort(const CString& strIP = "10.36.23.54", const int& nPort = 50899);
	int Heartbeat(bool isOnline = true);

	int Heartbeat(bool isOnline, neb::CJsonObject & send, neb::CJsonObject& rcv);

	int CCDFileUploadComplete(const std::vector<ParamCCDUploadFile>& vecParam );
	
	int CCDFileUploadComplete(const std::vector<ParamCCDUploadFile>& vecParam ,neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);


	int GetRecipeList(const CString& strProductCode,neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	int GetRecipe(const CString& strRecipeCode,neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	void GetRcvCJson(neb::CJsonObject& cJsonRcv, const CString & cstrAPI);
	void GetRcvCJson(neb::CJsonObject& cJsonRcv, const EMESOperate& op);

	int RecipeVersionExamine(const ParamRecipeVersionExamine& paramRecipeVersion, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);

	int Recipe(const ParamRecipe& paramRecipe, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv);
};

