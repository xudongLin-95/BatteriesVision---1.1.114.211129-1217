/*     TinyXml�ڴ�������
TinyXml������DOM(�ĵ�����ģ��)������XML�ĵ���, ��������XML�ĵ�����һ����,
TiXmlDocument�������������ĸ����, ��һ���������ĵ���, ������, ������
���붼�����ĺ��, ����TinyXml����һ��������ķ���������ÿһ���������Ӧ��
����----ÿ��������������ί�и������ĸ���, ����ֻҪ��֤���ױ���ȷ����,
���ߵ����˸��׵�Clear����, �������к�����ᱻ��ȷ������, ���Զ������ĵ���˵
ֻҪTiXmlDocument������ȷ����, �Ǿ�����һʧ��, ��������ȴ�Ҷ������, һֱ
������.���������ݽṹ, Ϊ�˱�֤���������Զ�ʹ�ö��ڴ�, �������Լ������ڴ�Ҳ��
������Ȼ��
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
//����ID�Ų�ѯ���������Ϣ
CErrorInfo::tagErrorInfo CErrorInfo::GetErrorInfo(const int& nErrroCode,const vector<tagErrorInfo>& vecAllErrorInfos)
{
	if (nErrroCode < 0 || vecAllErrorInfos.size()==0) return tagErrorInfo();
	//��ȡ�����룬�ҵ���Ӧ��Ϣ
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

//����Xml�ļ�
BOOL CErrorInfo::CreatXml(const char* filePath, TiXmlDocument *CreatDoc, CString RootElemnet)
{
	try
	{
		TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "gb2312", "");
		CreatDoc->LinkEndChild(decl); //д���ĵ�  

		TiXmlElement *RootElement = new TiXmlElement(RootElemnet);//��Ԫ��
		CreatDoc->LinkEndChild(RootElement);  //д���Ԫ��

		CreatDoc->SaveFile(filePath);//����xml�ĵ�
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
//��ȡ������Ϣ
BOOL CErrorInfo::GetErrorCodeList(const char* filePath, vector<CErrorInfo::tagErrorInfo> &vecAllErrorInfos)
{
	//ע��:TinyXml �� TiXmlDocument ����һ�����ĸ��ڵ㣬�ͷŸ��ڵ㣬��������Ȼ�ͷ�
	TiXmlDocument * pDoc = new TiXmlDocument(filePath);
	try
	{
		if (!pDoc->LoadFile())
		{
			AfxMessageBox("�޷�����ErrCode.xml�ļ�");
			delete pDoc;
			return FALSE;
		}
		if (!vecAllErrorInfos.empty())
		{
			vecAllErrorInfos.clear();//���Ŀ������
		}
		//��������ǽ�XML�ĵ��е����Ե�ֵ��ֵ��vector<ErrorCode>
		const TiXmlElement * root = pDoc->RootElement();
		for (const TiXmlNode *child = root->FirstChild(); child; child = child->NextSibling())
		{
			if ((child->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(child->Value(), "ErrCode")))
			{
				CErrorInfo::tagErrorInfo Node;
				const TiXmlElement *element = (const TiXmlElement*)child;//��child�ڵ������ת��ΪԪ�ص�����
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
//����������Ϣ������Xml�ļ�
BOOL CErrorInfo::SetErrorCode(const char* filePath, vector<CErrorInfo::tagErrorInfo> vecAllErrorInfos)
{
	//ע��:TinyXml �� TiXmlDocument ����һ�����ĸ��ڵ㣬�ͷŸ��ڵ㣬��������Ȼ�ͷ�
	if (vecAllErrorInfos.empty())
	{
		return FALSE;//ԭ����Ϊ�գ�����false
	}
	//ÿ�α������±���һ��xml�ļ�
	TiXmlDocument *myDocument = new TiXmlDocument(filePath);

	try
	{
		if (false == CreatXml(filePath, myDocument, "ErrCode") || FALSE == myDocument->LoadFile())
		{
			delete myDocument;
			return FALSE;
		}
		//��ø�Ԫ��
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

				RootElement->LinkEndChild(NewNode);  //����Ӧ�Ľڵ���ӵ�DOM�ĵ�����
			}
		}
		myDocument->SaveFile(filePath);	  //���ĵ������󱣴浽Student�ļ���
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