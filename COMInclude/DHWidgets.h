//////////////////////////////////////////////////////////////////////////
//DHWidgets.h  将多个第三方控件导出
//////////////////////////////////////////////////////////////////////////

#pragma once


//#define W_EXPORT
#ifdef W_EXPORT
#define W_LIB   __declspec( dllexport )
#else
#define W_LIB   __declspec( dllimport )
#endif

#include "Widgets\Blendent.h"
#include "Widgets\BMenuExt.h"
#include "Widgets\ButtonExt.h"
#include "Widgets\StatusBarExt.h"
#include "Widgets\MemDCExt.h"
#include "Widgets\StaticExt.h"
#include "Widgets\EditExt.h"
#include "Widgets\HeaderCtrlExt.h"
#include "Widgets\ListCtrlExt.h"
#include "Widgets\ReportCtrlExt.h"
#include "Widgets\IconListBoxExt.h"
#include "Widgets\MenuExt.h"
#include "Widgets\DialogExt.h"
#include "Widgets\BarChartExt.h"
#include "Widgets\TabCtrlExt.h"
#include "Widgets\PictureExt.h"