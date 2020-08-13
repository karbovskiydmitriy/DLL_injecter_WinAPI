#include "Injecter.h"

RECT clientRect;
HWND hMainWindow, hProcessNameEdit, hLibraryNameEdit, hFunctionNameEdit, hInjectButton;
HFONT hEditFont, hButtonFont;
HMODULE hKernel32;
char *functionName;
wchar_t *fileName, *processName;

WNDCLASSEXW classEx =
{
	sizeof(WNDCLASSEX), 0, (WNDPROC)WindowProc, 0, 0, 0, 0, 0, 0, 0, (LPCWSTR)MAIN_CLASS_NAME, 0
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	classEx.hInstance = hInstance;
	classEx.hCursor = LoadCursorW(0, IDC_ARROW);
	RegisterClassExW(&classEx);

	hMainWindow = CreateWindowExW(0, MAIN_CLASS_NAME, CAPTION, WINDOW_STYLE, 0, 0, 500, 500, 0, 0, hInstance, 0);
	ShowWindow(hMainWindow, SW_SHOWNORMAL);
	UpdateWindow(hMainWindow);
	GetClientRect(hMainWindow, &clientRect);

	hProcessNameEdit = CreateWindowExW(0, EDIT_CLASS_NAME, PROCESS_NAME_EDIT_TEXT, PROCESS_NAME_EDIT_STYLE, 0, 0, clientRect.right, 50, hMainWindow, 0, 0, 0);
	hLibraryNameEdit = CreateWindowExW(0, EDIT_CLASS_NAME, LIBRARY_NAME_EDIT_TEXT, LIBRARY_NAME_EDIT_STYLE, 0, 50, clientRect.right, 50, hMainWindow, 0, 0, 0);
	hFunctionNameEdit = CreateWindowExW(0, EDIT_CLASS_NAME, FUNCTION_NAME_EDIT_TEXT, FUNCTION_NAME_EDIT_STYLE, 0, 100, clientRect.right, 50, hMainWindow, 0, 0, 0);
	hInjectButton = CreateWindowExW(0, BUTTON_CLASS_NAME, INJECT_BUTTON_TEXT, INJECT_BUTTON_STYLE, 0, 150, clientRect.right, clientRect.bottom - 150, hMainWindow, 0, 0, 0);

	hEditFont = CreateFontW(20, 10, 0, 0, FW_NORMAL, 0, 0, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, EDIT_FONT_NAME);
	hButtonFont = CreateFontW(40, 25, 0, 0, FW_NORMAL, 0, 0, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, BUTTON_FONT_NAME);

	SendMessage(hProcessNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hLibraryNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hFunctionNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hInjectButton, WM_SETFONT, (WPARAM)hButtonFont, true);
	
	fileName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	processName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	functionName = (char *)calloc(MAX_PATH, sizeof(wchar_t));

	hKernel32 = GetModuleHandleW(KERNEL32_STRING);

	while (GetMessageW(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
			
		return 0;
	case WM_COMMAND:
		if ((HWND)lParam != hInjectButton) {
			break;
		}
		memset(fileName, 0, MAX_PATH * sizeof(wchar_t));
		memset(functionName, 0, MAX_PATH * sizeof(char));
		memset(processName, 0, MAX_PATH * sizeof(wchar_t));
		if (!GetWindowTextW(hProcessNameEdit, processName, MAX_PATH)) {
			Error(ENTER_PROCESS_NAME_STRING, INCORRECT_INPUT_STRING);
			
			return 0;
		}
		if (!GetWindowTextW(hLibraryNameEdit, fileName, MAX_PATH)) {
			Error(ENTER_LIBRARY_NAME_STRING, INCORRECT_INPUT_STRING);
			
			return 0;
		}
		if (!GetWindowTextA(hFunctionNameEdit, functionName, MAX_PATH)) {
			Error(ENTER_FUNCTION_NAME_STRING, INCORRECT_INPUT_STRING);
			
			return 0;
		}
		Inject(fileName, processName, functionName);
			
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			
			return 0;
		}
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void Inject(wchar_t *fileName, wchar_t *processName, char *functionName)
{
	DWORD remoteThreadExitCode;
	HANDLE hRemoteProcess, hLibraryFile, hRemoteThread;
	HMODULE hTempLibrary;
	void *functionAddress, *functionOffset, *fileNameRemoteAddress;

	hLibraryFile = CreateFileW(fileName, FILE_ACCESS_ATTRIBUTES, 0, (SECURITY_ATTRIBUTES *)null, OPEN_EXISTING, 0, (HANDLE)null);
	if (GetLastError()) {
		Error(WRONG_LIBRARY_NAME_STRING, INCORRECT_INPUT_STRING);
		
		return;
	}
	CloseHandle(hLibraryFile);
	if (!(hTempLibrary = LoadLibraryW(fileName))) {
		Error(LIBRARY_ANALYZE_FAIL_STRING, ERROR_STRING);
		
		return;
	}
	if (!(functionOffset = (void *)GetProcAddress(hTempLibrary, functionName))) {
		Error(WRONG_FUNCTION_NAME_STRING, INCORRECT_INPUT_STRING);
		
		return;
	}
	functionOffset = (void *)((long)functionOffset - (long)hTempLibrary);
	FreeLibrary(hTempLibrary);
	if (!(hRemoteProcess = GetProcessByName(processName))) {
		Error(WRONG_PROCESS_NAME_STRING, INCORRECT_INPUT_STRING);
		
		return;
	}
	if(!(fileNameRemoteAddress = VirtualAllocEx(hRemoteProcess, (void *)null, wcslen(fileName) * sizeof(wchar_t), MEM_COMMIT, PROTECT_FLAGS))) {
		Error(FAIL_ALLOCATIONG_MEMORY_STRING, ERROR_STRING);
		
		return;
	}
	if (!(WriteProcessMemory(hRemoteProcess, fileNameRemoteAddress, fileName, wcslen(fileName) * sizeof(wchar_t), (SIZE_T *)null))) {
		Error(FAIL_COPYING_MEMORY_STRING, ERROR_STRING);
		
		return;
	}
	functionAddress = GetProcAddress(hKernel32, LOAD_LIBRARY_STRING);
	if (!(hRemoteThread = CreateRemoteThread(hRemoteProcess, (SECURITY_ATTRIBUTES *)null, 4096, (LPTHREAD_START_ROUTINE)functionAddress, fileNameRemoteAddress, 0, (DWORD *)null))) {
		Error(LIBRARY_LOADING_FAIL_STRING, ERROR_STRING);
		
		return;
	}
	WaitForSingleObject(hRemoteThread, INFINITE);
	GetExitCodeThread(hRemoteThread, &remoteThreadExitCode);
	CloseHandle(hRemoteThread);
	functionAddress = (void *)((long)remoteThreadExitCode + (long)functionOffset);
	if (!(hRemoteProcess = CreateRemoteThread(hRemoteProcess, (SECURITY_ATTRIBUTES *)null, 4096, (LPTHREAD_START_ROUTINE)functionAddress, (void *)null, 0, (DWORD *)null))) {
		Error(FAIL_STARTING_REMOTE_PROCEDURE_STRING, INCORRECT_INPUT_STRING);
		
		return;
	}
	WaitForSingleObject(hRemoteThread, INFINITE);
	CloseHandle(hRemoteThread);
	MessageBoxW(hMainWindow, SUCCESS_STRING, SUCCESS_STRING, MB_ICONINFORMATION);
}

HANDLE GetProcessByName(wchar_t *processName)
{
	int i;
	int count;
	int index;
	wchar_t *openedProcessName;
	HANDLE hOpenedProcess;
	HMODULE processMainModule;
	
	DWORD cbNeeded, *processIds = (DWORD *)calloc(PROCESSES_BUFFER_SIZE, sizeof(DWORD));
	if (!EnumProcesses(processIds, (DWORD)PROCESSES_BUFFER_SIZE, &cbNeeded)) {
		free(processIds);
		
		return 0;
	}
	count = cbNeeded / sizeof(DWORD);
	openedProcessName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	for (i = 0, index = 0; i < count; i++) {
		if (hOpenedProcess = OpenProcess(PROCESS_ACCESS_FLAGS, 0, processIds[i])) {
			if (EnumProcessModules(hOpenedProcess, &processMainModule, sizeof(HMODULE), &cbNeeded)) {
				memset(openedProcessName, 0, MAX_PATH * sizeof(wchar_t));
				if (GetModuleBaseNameW(hOpenedProcess, processMainModule, openedProcessName, MAX_PATH)) {
					if (!wcscmp(processName, openedProcessName)) {
						free(openedProcessName);
						free(processIds);
						
						return hOpenedProcess;
					}
				}
			}
		}
	}
	free(openedProcessName);
	free(processIds);

	return 0;
}

void Error(const wchar_t *text, const wchar_t *caption)
{
	MessageBox(hMainWindow, text, caption, MB_ICONERROR);
}
