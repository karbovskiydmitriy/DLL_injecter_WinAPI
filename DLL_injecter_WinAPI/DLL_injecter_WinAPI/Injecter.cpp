#include "Injecter.h"

const DWORD windowStyle = WS_OVERLAPPED & !WS_SIZEBOX | WS_SYSMENU;
RECT clientRect = {0, 0, 500, 500};
HWND hMainWindow, hProcessNameEdit, hLibraryNameEdit, hFunctionNameEdit, hInjectButton;
HFONT hEditFont, hButtonFont;
const DWORD processNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_UPPERCASE;
const DWORD libraryNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_UPPERCASE;
const DWORD functionNameEditStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_UPPERCASE;
const DWORD injectButtonStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | BS_CENTER;
const wchar_t *const editClassName = L"EDIT";
const wchar_t *const buttonClassName = L"BUTTON";
const wchar_t *const processNameEditText = L"Enter process name";
const wchar_t *const libraryNameEditText = L"Enter library name";
const wchar_t *const functionNameEditText = L"Enter function name";
const wchar_t *const injectButtonText = L"INJECT!";
const wchar_t *const editFontName = L"CONSOLAS";
const wchar_t *const buttonFontName = L"CONSOLAS";
const wchar_t *const caption = L"DLL-injecter";
const wchar_t *const mainClassName = L"MainClass";

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
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage (0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}
