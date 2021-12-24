#pragma once
#include "Log.in.h"

typedef struct tagCrashRecordInit
{
	LOGCALLBACK		pLog;				//日志回调
}CrashRecordInit, *PCrashRecordInit;