#include <Windows.h>

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
DWORD __declspec(dllexport) __stdcall MessageProc(void *);
