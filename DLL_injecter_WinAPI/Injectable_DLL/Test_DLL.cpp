#include "Test_DLL.h"

BOOL __declspec(dllexport) WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		MessageBox(0, L"Hello, world!", L"Message from the DLL!", 0);
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;

}
