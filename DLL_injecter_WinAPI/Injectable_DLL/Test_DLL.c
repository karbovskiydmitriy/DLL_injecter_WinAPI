#include "Test_DLL.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}

DWORD __declspec(dllexport) __stdcall MessageProc(void *param)
{
	MessageBox(0, POPUP_TEXT, CAPTION, MB_ICONINFORMATION);

	return 0;
}