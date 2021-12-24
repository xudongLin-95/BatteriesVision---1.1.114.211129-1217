/*
   作者：李博
   主要记录一些常用的函数
   12.0.0.1  第一位代表是基于Halcon12 增加
   void FastSortFileNames (HTuple hv_DirName, HTuple *hv_SortedFileNames);
   12.0.0.2: 增加AdaptiveSize() 用于MFC自适应屏幕分辨率UI尺寸


*/



#include "StdAfx.h"

#include "HalconCpp.h"
using namespace HalconCpp;


#pragma once
void WritePrivateProfileInt(CString lpAppName,CString lpKeyName,int Value, CString lpFileName);

void WritePrivateProfileDouble(CString lpAppName,CString lpKeyName,double Value, CString lpFileName);
double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double nDefault, LPCTSTR lpFileName);
CString GetCurrentAppPath();
BOOL IsDirectory(CString pstrPath);
BOOL CopyFolder(CString pstrSrcFolder, CString pstrDstFolder);
BOOL DeleteFolder(CString pstrFolder);
BOOL CreateMultilevelPath(CString pstrFolder);
int GetKeyValues(CStringArray& arrKey,CStringArray& arrValue, LPCTSTR lpSection,LPCTSTR lpfileName);
void TupleExceptRepeat(const HTuple& src,HTuple* dst);
void AdaptiveSize(HWND hWnd);//自适应UI

void disp_message (HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, 
	HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);
void set_display_font (HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, 
	HTuple hv_Slant);