#include "Injecter.h"

const DWORD windowStyle = WS_OVERLAPPED & !WS_SIZEBOX | WS_SYSMENU;
const DWORD processNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER;
const DWORD libraryNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER;
const DWORD injectButtonStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER;
const DWORD fileAccessAttributes = FILE_READ_DATA;
const DWORD processAccessFlags = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION;
const DWORD protectFlags = PAGE_READWRITE;
const wchar_t *const caption = L"DLL-injecter";
const wchar_t *const mainClassName = L"MainClass";
const wchar_t *const editClassName = L"EDIT";
const wchar_t *const buttonClassName = L"BUTTON";
const wchar_t *const injectButtonText = L"INJECT!";
const wchar_t *const editFontName = L"CONSOLAS";
const wchar_t *const buttonFontName = L"CONSOLAS";
const wchar_t *const errorString = L"Error!";
const wchar_t *const kernel32String = L"KERNEL32.DLL";
const wchar_t *const successString = L"Successfully injected!";
const wchar_t *const loadLibraryString = L"LoadLibraryString";
const wchar_t *const incorrectInputString = L"Incorrect input!";
const wchar_t *const processNameEditText = L"Enter process name";
const wchar_t *const libraryNameEditText = L"Enter library name";
const wchar_t *const enterProcessNameString = L"Enter process name!";
const wchar_t *const enterLibraryNameString = L"Enter library name!";
const wchar_t *const wrongProcessNameString = L"Incorrect process name!";
const wchar_t *const wrongLibraryNameString = L"Incorrect library name!";
const wchar_t *const libraryLoadingFailString = L"Could not load library to the selected process!";
const wchar_t *const failAllocatingMemoryString = L"Could not allocate memory in selected process!";
const wchar_t *const failCopyingMemoryString = L"Cound not copy file name from your process to selected process!";
RECT clientRect = {0, 0, 500, 500};
HWND hMainWindow, hProcessNameEdit, hLibraryNameEdit, hInjectButton;
HFONT hEditFont, hButtonFont;
HANDLE hRemoteProcess, hLibraryFile;
HMODULE hKernel32;
void *functionAddress, *fileNameRemoteAddress;
wchar_t *fileName, *functionName, *processName;

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
	hInjectButton = CreateWindowExW(0, buttonClassName, injectButtonText, injectButtonStyle, 0, 100, clientRect.right, clientRect.bottom - 100, hMainWindow, 0, 0, 0);

	hEditFont = CreateFontW(20, 10, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, editFontName);
	hButtonFont = CreateFontW(40, 25, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, buttonFontName);

	SendMessage(hProcessNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hLibraryNameEdit, WM_SETFONT, (WPARAM)hEditFont, true);
	SendMessage(hInjectButton, WM_SETFONT, (WPARAM)hButtonFont, true);
	
	fileName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	functionName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	processName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));

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
		memset(fileName, 0, MAX_PATH * sizeof(wchar_t));
		memset(functionName, 0, MAX_PATH * sizeof(wchar_t));
		memset(processName, 0, MAX_PATH * sizeof(wchar_t));
		if (!GetWindowTextW(hProcessNameEdit, processName, MAX_PATH)) {
			MessageBoxW(0, enterProcessNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		if (!GetWindowTextW(hLibraryNameEdit, fileName, MAX_PATH)) {
			MessageBoxW(0, enterLibraryNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		hLibraryFile = CreateFileW(fileName, fileAccessAttributes, 0, (SECURITY_ATTRIBUTES *)NULL, OPEN_EXISTING, 0, (HANDLE)NULL);
		if (GetLastError()) {
			MessageBoxW(0, wrongLibraryNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		CloseHandle(hLibraryFile);
		if (!(hRemoteProcess = GetProcessByName(processName))) {
			MessageBoxW(0, wrongProcessNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		if(!(fileNameRemoteAddress = VirtualAllocEx(hRemoteProcess, (void *)NULL, wcslen(fileName) * sizeof(wchar_t), MEM_COMMIT, protectFlags))) {
			MessageBoxW(0, failAllocatingMemoryString, errorString, MB_ICONERROR);
			return 0;
		}
		if (!(WriteProcessMemory(hRemoteProcess, fileNameRemoteAddress, fileName, wcslen(fileName) * sizeof(wchar_t), (SIZE_T *)NULL))) {
			MessageBoxW(0, failCopyingMemoryString, errorString, MB_ICONERROR);
			return 0;
		}
		functionAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, (LPCSTR)loadLibraryString);
		if (!CreateRemoteThread(hRemoteProcess, (SECURITY_ATTRIBUTES *)NULL, 4096, (LPTHREAD_START_ROUTINE)functionAddress, fileName, 0, (DWORD *)NULL)) {
			MessageBoxW(0, libraryLoadingFailString, errorString, MB_ICONERROR);
			return 0;
		}
		MessageBoxW(0, successString, successString, MB_ICONWARNING);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage (0);
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);

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
