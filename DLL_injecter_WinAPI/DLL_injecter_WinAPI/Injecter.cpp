#include "Injecter.h"

const DWORD windowStyle = WS_OVERLAPPED & !WS_SIZEBOX | WS_SYSMENU;
const DWORD processNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_UPPERCASE;
const DWORD libraryNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_UPPERCASE;
const DWORD functionNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_UPPERCASE;
const DWORD injectButtonStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER;
const DWORD fileAccessAttributes = FILE_READ_DATA;
const wchar_t *const editClassName = L"EDIT";
const wchar_t *const buttonClassName = L"BUTTON";
const wchar_t *const processNameEditText = L"Enter process name";
const wchar_t *const libraryNameEditText = L"Enter library name";
const wchar_t *const functionNameEditText = L"Enter function name";
const wchar_t *const injectButtonText = L"INJECT!";
const wchar_t *const editFontName = L"CONSOLAS";
const wchar_t *const buttonFontName = L"CONSOLAS";
const wchar_t *const incorrectInputString = L"Incorrect input!";
const wchar_t *const enterProcessNameString = L"Enter process name!";
const wchar_t *const enterLibraryNameString = L"Enter library name!";
const wchar_t *const enterFunctionNameString = L"Enter function name!";
const wchar_t *const wrongProcessNameString = L"Incorrect process name!";
const wchar_t *const wrongLibraryNameString = L"Incorrect library name!";
const wchar_t *const wrongFunctionNameString = L"Incorrect function name!";
const wchar_t *const caption = L"DLL-injecter";
const wchar_t *const mainClassName = L"MainClass";
RECT clientRect = {0, 0, 500, 500};
HWND hMainWindow, hProcessNameEdit, hLibraryNameEdit, hFunctionNameEdit, hInjectButton;
HFONT hEditFont, hButtonFont;
HANDLE hLibraryFile;
DWORD *processIds;
HANDLE processes[MAX_PROCESSES_COUNT];
wchar_t *fileName, *functionName, *processName, *processNames[MAX_PROCESSES_COUNT];

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
	functionName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));
	processName = (wchar_t *)calloc(MAX_PATH, sizeof(wchar_t));

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
	case WM_SIZE:
		GetClientRect(hWnd, &clientRect);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		if ((HWND)lParam != hInjectButton) {
			break;
		}
		memset(fileName, 0,  MAX_PATH);
		memset(functionName, 0,  MAX_PATH);
		memset(processName, 0,  MAX_PATH);
		if (!GetWindowTextW(hProcessNameEdit, processName, MAX_PATH)) {
			MessageBox(0, enterProcessNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		if (!GetWindowTextW(hLibraryNameEdit, fileName, MAX_PATH)) {
			MessageBox(0, enterLibraryNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		if (!GetWindowTextW(hFunctionNameEdit, functionName, MAX_PATH)) {
			MessageBox(0, enterFunctionNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		hLibraryFile = CreateFileW(fileName, fileAccessAttributes, 0, (SECURITY_ATTRIBUTES *)NULL, OPEN_EXISTING, 0, (HANDLE)NULL);
		if (GetLastError()) {
			MessageBox(0, wrongLibraryNameString, incorrectInputString, MB_ICONERROR);
			return 0;
		}
		CloseHandle(hLibraryFile);

		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage (0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}
