#include "Test_DLL.h"

const wchar_t *const text = L"Hello, world!";
const wchar_t *const caption = L"Hi!";

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	
	return TRUE;

}

DWORD __declspec(dllexport) __stdcall MessageProc(void *)
{

	MessageBox(0, text, caption, MB_ICONINFORMATION);
	return 0;

}
