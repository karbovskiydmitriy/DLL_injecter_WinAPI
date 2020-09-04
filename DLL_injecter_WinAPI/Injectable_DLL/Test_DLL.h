#pragma once

#ifndef __TEST_DLL_H__
#define __TEST_DLL_H__

#include <Windows.h>

#define POPUP_TEXT L"Hello, world!"
#define CAPTION L"Hi!"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
DWORD __declspec(dllexport) __stdcall MessageProc(void *param);

#endif // __TEST_DLL_H__
