// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once
#define  _CRT_SECURE_NO_WARNING
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN      //�� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"
#define  _CRT_SECURE_NO_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _CRT_SECURE_NO_WARNINGS
#include <afxdialogex.h>
//log4cplus
//#include "log4cplus/logger.h"  
//#include "log4cplus/configurator.h"
//#include "log4cplus/fileappender.h"
//#include "log4cplus/loggingmacros.h"


#include "HalconCpp.h"
#include "HDevThread.h"
using namespace HalconCpp;

#include "DHInterFace.h"
#include "DH_HalconEx.h"
#include "SCIMes.in.h"
#include "SCIDetect.in.h"
#include "Mes.h"
#include <afxcontrolbars.h>
#include <stdio.h>  
#include <iostream>
#include <sstream>
#include <vector>
#include "DHWidgets.h"
#include "LockHelper.h "
#include "include/gSoap/soapEquToMesAlarmSoapProxy.h"
#include "include/gSoap/soapH.h"
#include "include/gSoap/soapStub.h"
#include "CStringToolEx.h"
#include "LockHelper.h"
#include "MesDataType.h"
#include "DlgMesSet.h"


