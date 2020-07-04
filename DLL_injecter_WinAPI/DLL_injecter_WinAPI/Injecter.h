#ifndef __INJECTER_H__
#define __INJECTER_H__

#include <Windows.h>
#include <Psapi.h>

#pragma comment (lib, "psapi.lib")

#define true 1
#define false 0

#define null (void *)0

#define WINDOW_STYLE (WS_OVERLAPPED & !WS_SIZEBOX | WS_SYSMENU)
#define PROCESS_NAME_EDIT_STYLE (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER)
#define LIBRARY_NAME_EDIT_STYLE (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER)
#define FUNCTION_NAME_EDIT_STYLE (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER)
#define INJECT_BUTTON_STYLE (WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER)
#define FILE_ACCESS_ATTRIBUTES FILE_READ_DATA
#define PROCESS_ACCESS_FLAGS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION)
#define PROTECT_FLAGS PAGE_READWRITE
#define MAX_PROCESSES_COUNT 256
#define MAX_MODULES_COUNT 256
#define PROCESSES_BUFFER_SIZE 4096

#define LOAD_LIBRARY_STRING "LoadLibraryW"

#define MAIN_CLASS_NAME L"InjecterMainClass"
#define CAPTION L"DLL-injecter"
#define EDIT_CLASS_NAME L"EDIT"
#define BUTTON_CLASS_NAME L"BUTTON"
#define INJECT_BUTTON_TEXT L"INJECT!"
#define EDIT_FONT_NAME L"CONSOLAS"
#define BUTTON_FONT_NAME L"CONSOLAS"
#define ERROR_STRING L"Error!"
#define KERNEL32_STRING L"Kernel32.DLL"
#define SUCCESS_STRING L"Successfully injected!"
#define INCORRECT_INPUT_STRING L"Incorrect input!"
#define PROCESS_NAME_EDIT_TEXT L"Enter process name"
#define LIBRARY_NAME_EDIT_TEXT L"Enter library name"
#define FUNCTION_NAME_EDIT_TEXT L"Enter function name"
#define ENTER_PROCESS_NAME_STRING L"Enter process name!"
#define ENTER_LIBRARY_NAME_STRING L"Enter library name!"
#define ENTER_FUNCTION_NAME_STRING L"Enter funciton name!"
#define WRONG_PROCESS_NAME_STRING L"Incorrect process name!"
#define WRONG_LIBRARY_NAME_STRING L"Incorrect library name!"
#define WRONG_FUNCTION_NAME_STRING L"Incorrect function name!"
#define LIBRARY_ANALYZE_FAIL_STRING L"Failed to analyze library from this process!"
#define LIBRARY_LOADING_FAIL_STRING L"Failed to load library to the selected process!"
#define FAIL_ALLOCATIONG_MEMORY_STRING L"Failed to allocate memory in selected process!"
#define FAIL_COPYING_MEMORY_STRING L"Failed to copy file name from your process to selected process!"
#define FAIL_STARTING_REMOTE_PROCEDURE_STRING L"Failed to start remote thread on selected function in selected remote process!"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Inject(wchar_t *fileName, wchar_t *processName, char *functionName);
HANDLE GetProcessByName(wchar_t *processName);
void Error(const wchar_t *text, const wchar_t *caption);

#endif // __INJECTER_H__