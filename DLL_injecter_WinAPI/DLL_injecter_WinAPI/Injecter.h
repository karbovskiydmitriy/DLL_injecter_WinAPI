#include <Windows.h>
#include <Psapi.h>

#define MAX_PROCESSES_COUNT 256
#define MAX_MODULES_COUNT 256
#define PROCESSES_BUFFER_SIZE 4096

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
void Inject();
HANDLE GetProcessByName(wchar_t *processName);
void Error(const wchar_t *text, const wchar_t *caption);
