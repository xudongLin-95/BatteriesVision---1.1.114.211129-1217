/*
   作者：李博
   主要记录一些常用的函数
   版本号：13.0.0.502
*/
#include "HalconCpp.h"
#include "HDevThread.h"
#include "Log.in.h"
using namespace HalconCpp;
#define GetResultColor(bOK) bOK==TRUE?RGB(255,255,255):RGB(255,0,0)
void dev_open_window_fit_image (HObject ho_Image, HTuple hv_Row, HTuple hv_Column, 
    HTuple hv_WidthLimit, HTuple hv_HeightLimit, HTuple *hv_WindowHandle);
void dev_open_window_fit_image (HObject ho_Image,const CStatic &IpWinStatic, HTuple *hv_WindowHandle);
void disp_continue_message (HTuple hv_WindowHandle, HTuple hv_Color, HTuple hv_Box);
void disp_message (HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, 
    HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);
void set_display_font (HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, 
    HTuple hv_Slant);
void TileImages (HObject ho_Images, HObject *ho_TiledImage);
void list_image_files (HTuple hv_ImageDirectory, HTuple hv_Extensions, HTuple hv_Options, 
	HTuple *hv_ImageFiles);
void FastSortFileNames (HTuple hv_DirName, HTuple *hv_SortedFileNames);
void TupleExceptRepeat(const HTuple& src,HTuple* dst);
void parse_filename (HTuple hv_FileName, HTuple *hv_BaseName, HTuple *hv_Extension, 
	HTuple *hv_Directory);
void TupleCountExceptRepeat(const HTuple& srcErrorType,const HTuple& srcBelongStrip,HTuple* dstErrorType,HTuple* dstBelongStrip);
///////////////////////////////////////程序界面调整///////////////////////////////////
void FullScreen(HWND hWnd);//调整屏幕至全屏，不显示任务栏
void FullScreenWithTaskbar(HWND hWnd);//调整屏幕至全屏，保留任务栏
void AdaptiveSize(HWND hWnd,BOOL bAdaptive);//自适应UI,需要在界面调整前调用一次，界面调整后调用一次
void FullScreenAdaptive(HWND hWnd);//调整屏幕至全屏，不显示任务栏,并且自适应UI
void FullScreenTaskbarAdaptive(HWND hWnd);//调整屏幕至全屏，保留任务栏,并且自适应UI
///////////////////////////////////////配置文件读写///////////////////////////////////
CString GetPrivateProfileCString(CString lpAppName,LPCTSTR lpKeyName,CString lpDefault,LPCTSTR lpFileName);
void WritePrivateProfileInt(CString lpAppName,CString lpKeyName,int Value, CString lpFileName);
void WritePrivateProfileDouble(CString lpAppName,CString lpKeyName,double Value, CString lpFileName);
double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double nDefault, LPCTSTR lpFileName);
CString GetCurrentAppPath();
BOOL IsDirectory(CString pstrPath);
BOOL CopyFolder(CString pstrSrcFolder, CString pstrDstFolder);
BOOL DeleteFolder(CString pstrFolder);
BOOL CreateMultilevelPath(CString pstrFolder);
int GetKeyValues(CStringArray& arrKey,CStringArray& arrValue, LPCTSTR lpSection,LPCTSTR lpfileName);
/////////////////////////////////////加载动态库/////////////////////////
DHInterface* LoadDHInterFaceDll(CString strDllName);
//////////////////////////////////////////////////////////////////////////
void OutputLogFunction(LOGCALLBACK PLog,int& nGraberSN, CString strRemindID,CString strFuntionPart,HTuple& hvStartTime,HTuple& hvEndTime);
void OutputLogFunction(LOGCALLBACK PLog,int& nGraberSN,CString strErrorID,CString strFuntionPart,CString strErrorProcName,CString strErrorContent);//输出错误停机日志
void GetEachStartAndEndPoint(HTuple hv_Row, HTuple hv_Column, HTuple hv_Phi, HTuple hv_Length1,
	HTuple hv_Length2, HTuple *hv_StartRow, HTuple *hv_StartColumn, HTuple *hv_EndRow,
	HTuple *hv_EndColumn);
void gen_arrow_contour_xld(HObject *ho_Arrow, HTuple hv_Row1, HTuple hv_Column1,
	HTuple hv_Row2, HTuple hv_Column2, HTuple hv_HeadLength, HTuple hv_HeadWidth);

//////////////////////////////////////////////////////////////////////////
#define FUNC_NAME(x) #x
#define HALCON_TRY(__FUNC__) \
try \
{ \
(__FUNC__); \
} \
catch(HException&){ \
}

