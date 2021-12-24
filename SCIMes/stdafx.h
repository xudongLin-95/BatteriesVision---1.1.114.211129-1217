// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once
#define  _CRT_SECURE_NO_WARNING
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN      //从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"
#define  _CRT_SECURE_NO_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
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


