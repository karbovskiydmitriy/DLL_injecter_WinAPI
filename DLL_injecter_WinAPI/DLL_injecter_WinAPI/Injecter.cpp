#include "Injecter.h"

const DWORD windowStyle = WS_OVERLAPPED & !WS_SIZEBOX | WS_SYSMENU;
const DWORD processNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER;
const DWORD libraryNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER;
const DWORD functionNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER;
const DWORD injectButtonStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER;
const DWORD fileAccessAttributes = FILE_READ_DATA;
const DWORD processAccessFlags = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION;
const DWORD protectFlags = PAGE_READWRITE;
const char *const loadLibraryString = "LoadLibraryW";
const wchar_t *const caption = L"DLL-injecter";
const wchar_t *const mainClassName = L"MainClass";
const wchar_t *const editClassName = L"EDIT";
const wchar_t *const buttonClassName = L"BUTTON";
const wchar_t *const injectButtonText = L"INJECT!";
const wchar_t *const editFontName = L"CONSOLAS";
const wchar_t *const buttonFontName = L"CONSOLAS";
const wchar_t *const errorString = L"Error!";
const wchar_t *const kernel32String = L"Kernel32.DLL";
const wchar_t *const successString = L"Successfully injected!";
const wchar_t *const incorrectInputString = L"Incorrect input!";
const wchar_t *const processNameEditText = L"Enter process name";
const wchar_t *const libraryNameEditText = L"Enter library name";
const wchar_t *const functionNameEditText = L"Enter function name";
const wchar_t *const enterProcessNameString = L"Enter process name!";
const wchar_t *const enterLibraryNameString = L"Enter library name!";
const wchar_t *const enterFunctionNameString = L"Enter funciton name!";
const wchar_t *const wrongProcessNameString = L"Incorrect process name!";
const wchar_t *const wrongLibraryNameString = L"Incorrect library name!";
const wchar_t *const wrongFunctionNameString = L"Incorrect function name!";
const wchar_t *const libraryAnalyzeFailString = L"Failed to analyze library from this process!";
const wchar_t *const libraryLoadingFailString = L"Failed to load library to the selected process!";
const wchar_t *const failAllocatingMemoryString = L"Failed to allocate memory in selected process!";
const wchar_t *const failCopyingMemoryString = L"Failed to copy file name from your process to selected process!";
const wchar_t *const failStartingRemoteProcedureString = L"Failed to start remote thread on selected function in selected remote process!";
RECT clientRect = {0, 0, 500, 500};
HWND hMainWindow, hProcessNameEdit, hLibraryNameEdit, hFunctionNameEdit, hInjectButton;
HFONT hEditFont, hButtonFont;
HMODULE hKernel32;
char *functionName;
wchar_t *fileName, *processName;

WNDCLASSEXW WndClassEx = {sizeof(WNDCLASSEX), 0, (WNDPROC)WindowProc, 0, 0, 0, 0, 0, 0, 0, (LPCWSTR)mainClassName, 0};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	WndClassEx.hInstance = hInstance;
	WndClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClassEx(&WndClassEx);

	hMainWindow = CreateWindowExW(0, mainClassName, caption, windowStyle, 0, 0, clientRect.right, clientRect.bottom, 0, 0, hInstance, 0);
	ShowWindow(hMainWindow, SW_SHOWNORMAL);
	UpdateWindow(hMainWindow);
	GetClientRect(hMainWindow, &clientRect);

	hProcessNameEdit = CreateWindowExW(0, editClassName, processNameEditText, processNameEditStyle, 0, 0, clientRect.right, 50, hMainWindow, 0, 0, 0);
	hLibraryNameEdit = CreateWindowExW(0, editClassName, libraryNameEditText, libraryNameEditStyle, 0, 50, clientRect.right, 50, hMainWindow, 0, 0, 0);
	hFunctionNameEdit = CreateWindowExW(0, editClassName, functionNameEditText, functionNameEditStyle, 0, 100, clientRect.right, 50, hMainWindow, 0, 0, 0);
	hInjectButton = CreateWindowExW(0, buttonClassName, injectButtonText, injectButtonStyle, 0, 150, clientRect.right, clientRect.bottom - 150, hMainWindow, 0, 0, 0);

	hEditFont = CreateFontW(20, 10, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, editFontName);
	hButtonFont = CreateFontW(40, 25, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, buttonFontName);

	SendMessage(hProcessNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hLibraryNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hFunctionNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hInjectButton, WM_SETFONT, (WPARAM)hButtonFont, true);
	
	fileName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	processName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	functionName = (char *)calloc(MAX_PATH, sizeof(wchar_t));

	hKernel32 = GetModuleHandleW(kernel32String);

	MSG msg;
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
		
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage (0);
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

void Inject()
{

	DWORD remoteThreadExitCode;
	HANDLE hRemoteProcess, hLibraryFile, hRemoteThread;
	HMODULE hTempLibrary;
	void *functionAddress, *functionOffset, *fileNameRemoteAddress;

	memset(fileName, 0, MAX_PATH * sizeof(wchar_t));
	memset(functionName, 0, MAX_PATH * sizeof(wchar_t));
	memset(processName, 0, MAX_PATH * sizeof(wchar_t));
	if (!GetWindowTextW(hProcessNameEdit, processName, MAX_PATH)) {
		Error(enterProcessNameString, incorrectInputString);
		return;
	}
	if (!GetWindowTextW(hLibraryNameEdit, fileName, MAX_PATH)) {
		Error(enterLibraryNameString, incorrectInputString);
		return;
	}
	if (!GetWindowTextA(hFunctionNameEdit, functionName, MAX_PATH)) {
		Error(enterFunctionNameString, incorrectInputString);
		return;
	}
	hLibraryFile = CreateFileW(fileName, fileAccessAttributes, 0, (SECURITY_ATTRIBUTES *)NULL, OPEN_EXISTING, 0, (HANDLE)NULL);
	if (GetLastError()) {
		Error(wrongLibraryNameString, incorrectInputString);
		return;
	}
	CloseHandle(hLibraryFile);
	if (!(hTempLibrary = LoadLibraryW(fileName))) {
		Error(libraryAnalyzeFailString, errorString);
		return;
	}
	if (!(functionOffset = (void *)GetProcAddress(hTempLibrary, functionName))) {
		Error(wrongFunctionNameString, incorrectInputString);
		return;
	}
	functionOffset = (void *)((long)functionOffset - (long)hTempLibrary);
	FreeLibrary(hTempLibrary);
	if (!(hRemoteProcess = GetProcessByName(processName))) {
		Error(wrongProcessNameString, incorrectInputString);
		return;
	}
	if(!(fileNameRemoteAddress = VirtualAllocEx(hRemoteProcess, (void *)NULL, wcslen(fileName) * sizeof(wchar_t), MEM_COMMIT, protectFlags))) {
		Error(failAllocatingMemoryString, errorString);
		return;
	}
	if (!(WriteProcessMemory(hRemoteProcess, fileNameRemoteAddress, fileName, wcslen(fileName) * sizeof(wchar_t), (SIZE_T *)NULL))) {
		Error(failCopyingMemoryString, errorString);
		return;
	}
	functionAddress = GetProcAddress(hKernel32, loadLibraryString);
	if (!(hRemoteThread = CreateRemoteThread(hRemoteProcess, (SECURITY_ATTRIBUTES *)NULL, 4096, (LPTHREAD_START_ROUTINE)functionAddress, fileNameRemoteAddress, 0, (DWORD *)NULL))) {
		Error(libraryLoadingFailString, errorString);
		return;
	}
	WaitForSingleObject(hRemoteThread, INFINITE);
	GetExitCodeThread(hRemoteThread, &remoteThreadExitCode);
	CloseHandle(hRemoteThread);
	functionAddress = (void *)((long)remoteThreadExitCode + (long)functionOffset);
	if (!(hRemoteProcess = CreateRemoteThread(hRemoteProcess, (SECURITY_ATTRIBUTES *)NULL, 4096, (LPTHREAD_START_ROUTINE)functionAddress, (void *)NULL, 0, (DWORD *)NULL))) {
		Error(failStartingRemoteProcedureString, incorrectInputString);
		return;
	}
	WaitForSingleObject(hRemoteThread, INFINITE);
	CloseHandle(hRemoteThread);
	MessageBoxW(hMainWindow, successString, successString, MB_ICONINFORMATION);

}

HANDLE GetProcessByName(wchar_t *processName)
{
	
	DWORD cbNeeded, *processIds = (DWORD *)calloc(PROCESSES_BUFFER_SIZE, sizeof(DWORD));
	if (!EnumProcesses(processIds, (DWORD)PROCESSES_BUFFER_SIZE, &cbNeeded)) {
		free(processIds);
		return 0;
	}
	int count = cbNeeded / sizeof(DWORD);
	wchar_t *openedProcessName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	for (int i = 0, index = 0; i < count; i++) {
		if (HANDLE hOpenedProcess = OpenProcess(processAccessFlags, 0, processIds[i])) {
			HMODULE processMainModule;
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
