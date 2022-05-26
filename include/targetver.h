#pragma once

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include "winsdkver.h"

//#define WINVER 0x0501 // Windows XP
//#define _WIN32_WINNT 0x0501
#define WINVER 0x0601 // Windows 7
#define _WIN32_WINNT 0x0601
//#define WINVER 0x0602 // Windows 8
//#define _WIN32_WINNT 0x0602

// Including SDKDDKVer.h defines the highest available Windows platform.
#include "sdkddkver.h"
