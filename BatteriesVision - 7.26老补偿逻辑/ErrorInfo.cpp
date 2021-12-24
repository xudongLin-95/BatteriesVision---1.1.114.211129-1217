/*     TinyXml内存管理相关
TinyXml是利用DOM(文档对象模型)来呈现XML文档的, 在它眼中XML文档就是一棵树,
TiXmlDocument对象就是这棵树的根结点, 在一个完整的文档中, 除了它, 其余结点
必须都是它的后代, 所以TinyXml用了一个很巧妙的方法来析构每一个结点所对应的
对象----每个结点的析构任务都委托给了它的父亲, 这样只要保证父亲被正确析构,
或者调用了父亲的Clear函数, 它的所有后代都会被正确的析构, 所以对整个文档来说
只要TiXmlDocument对象被正确析构, 那就万无一失了, 这棵树会先从叶子销毁, 一直
到树根.像这种数据结构, 为了保证树的完整性而使用堆内存, 和由它自己管理内存也是
理所当然的
*/

#include "StdAfx.h"

CErrorInfo::CErrorInfo()
{

}

CErrorInfo::~CErrorInfo()
{

}

BOOL CErrorInfo::Init()
{
	ReadParam();
	return TRUE;
}

BOOL CErrorInfo::ReadParam()
{

	return TRUE;
}

BOOL CErrorInfo::WriteParam()
{

	return TRUE;
}
//根据ID号查询错误代码信息
CErrorInfo::tagErrorInfo CErrorInfo::GetErrorInfo(const int& nErrroCode,const vector<tagErrorInfo>& vecAllErrorInfos)
{
	if (nErrroCode < 0 || vecAllErrorInfos.size()==0) return tagErrorInfo();
	//读取错误码，找到对应信息
	CErrorInfo::tagErrorInfo tInfo;

	for (int i = 0; i < vecAllErrorInfos.size(); i++)
	{
		if (nErrroCode == vecAllErrorInfos[i].nID)
		{
			tInfo = vecAllErrorInfos[i];
		}
	}
	return tInfo;
}

//创建Xml文件
BOOL CErrorInfo::CreatXml(const char* filePath, TiXmlDocument *CreatDoc, CString RootElemnet)
{
	try
	{
		TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "gb2312", "");
		CreatDoc->LinkEndChild(decl); //写入文档  

		TiXmlElement *RootElement = new TiXmlElement(RootElemnet);//根元素
		CreatDoc->LinkEndChild(RootElement);  //写入根元素

		CreatDoc->SaveFile(filePath);//保存xml文档
	}
	catch (const std::exception&)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}
//获取所有信息
BOOL CErrorInfo::GetErrorCodeList(const char* filePath, vector<CErrorInfo::tagErrorInfo> &vecAllErrorInfos)
{
	//注意:TinyXml 中 TiXmlDocument 就像一个树的根节点，释放根节点，其他会自然释放
	TiXmlDocument * pDoc = new TiXmlDocument(filePath);
	try
	{
		if (!pDoc->LoadFile())
		{
			AfxMessageBox("无法加载ErrCode.xml文件");
			delete pDoc;
			return FALSE;
		}
		if (!vecAllErrorInfos.empty())
		{
			vecAllErrorInfos.clear();//清空目标容器
		}
		//下面代码是将XML文档中的属性的值赋值给vector<ErrorCode>
		const TiXmlElement * root = pDoc->RootElement();
		for (const TiXmlNode *child = root->FirstChild(); child; child = child->NextSibling())
		{
			if ((child->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(child->Value(), "ErrCode")))
			{
				CErrorInfo::tagErrorInfo Node;
				const TiXmlElement *element = (const TiXmlElement*)child;//将child节点的类型转化为元素的类型
				Node.nID = atoi(element->Attribute("ID"));
				Node.strDescribe = element->Attribute("Describe");
				Node.strSolution = element->Attribute("Solution");
				vecAllErrorInfos.push_back(Node);
			}
		}
	}
	catch (const std::exception&)
	{
		delete pDoc;
		return FALSE;
	}	
	catch (...)
	{
		delete pDoc;
		return FALSE;
	}
	delete pDoc;
	return TRUE;
}
//保存所有信息到本地Xml文件
BOOL CErrorInfo::SetErrorCode(const char* filePath, vector<CErrorInfo::tagErrorInfo> vecAllErrorInfos)
{
	//注意:TinyXml 中 TiXmlDocument 就像一个树的根节点，释放根节点，其他会自然释放
	if (vecAllErrorInfos.empty())
	{
		return FALSE;//原容器为空，返回false
	}
	//每次保存重新保存一份xml文件
	TiXmlDocument *myDocument = new TiXmlDocument(filePath);

	try
	{
		if (false == CreatXml(filePath, myDocument, "ErrCode") || FALSE == myDocument->LoadFile())
		{
			delete myDocument;
			return FALSE;
		}
		//获得根元素
		TiXmlElement *RootElement = myDocument->RootElement();
		CErrorInfo::tagErrorInfo Node;
		if (RootElement != NULL)
		{
			for (int i = 0; i < vecAllErrorInfos.size(); i++)
			{
				TiXmlElement *NewNode = new TiXmlElement("ErrCode");
				CString strId;
				strId.Format("%d", vecAllErrorInfos[i].nID);
				NewNode->SetAttribute("ID", (char*)strId.GetBuffer());
				strId.ReleaseBuffer();
				NewNode->SetAttribute("Describe", (char*)vecAllErrorInfos[i].strDescribe.GetBuffer());
				vecAllErrorInfos[i].strDescribe.ReleaseBuffer();
				NewNode->SetAttribute("Solution", (char*)vecAllErrorInfos[i].strSolution.GetBuffer());
				vecAllErrorInfos[i].strSolution.ReleaseBuffer();

				RootElement->LinkEndChild(NewNode);  //将相应的节点添加到DOM文档树中
			}
		}
		myDocument->SaveFile(filePath);	  //将文档树对象保存到Student文件中
	}
	catch (const std::exception&)
	{
		delete myDocument;
		return FALSE;
	}
	catch (...)
	{
		delete myDocument;
		return FALSE;
	}
	
	delete myDocument;
	return TRUE;
}