/*
   ���ߣ��
   ��Ҫ��¼һЩ���õĺ���
   12.0.0.1  ��һλ�����ǻ���Halcon12 ����
   void FastSortFileNames (HTuple hv_DirName, HTuple *hv_SortedFileNames);
   12.0.0.2: ����AdaptiveSize() ����MFC����Ӧ��Ļ�ֱ���UI�ߴ�


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
void AdaptiveSize(HWND hWnd);//����ӦUI

void disp_message (HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, 
	HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);
void set_display_font (HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, 
	HTuple hv_Slant);