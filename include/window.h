#ifndef _WINDOW_H
#define _WINDOW_H

#define UNICODE
#include <Windows.h>
#include <commctrl.h>
#include <shellapi.h>  //Shell_NotifyIcon() -> run api in system try area

#include <iostream>
#include <string>
#include <unordered_map>

#include "functions.h"
#include "macros.h"
#include "resource.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ColorPanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK BigPanelColorProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK BarMenuProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CopyPasteProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window {
   public:
    Window(const RECT&);
    ~Window();
    Window(Window&) = delete;
    Window operator=(Window&) = delete;
    bool ProcessMessage();
    void ChangeWinPos(HWND hwnd, POINT pt, RECT rct);
    HWND CreateNewWindow(const wchar_t* className, const wchar_t* wndTitle, const RECT&);
    HWND CreateChildWindow(WNDPROC WndProc, HWND hwnd, const wchar_t* className, const wchar_t* windowTitle, const RECT& rct, LPWSTR IDI_ICON, LPWSTR IDC_CURSOR, COLORREF color, HMENU hMenu, DWORD STYLE);
    HWND CreateChildWindow(HWND hwnd, const wchar_t* className, const wchar_t* windowTitle, const RECT& pos, HMENU hMenu, DWORD style);
    bool CreateRoundedWindow(HWND hwnd, short w, short h);
    bool CreateSysTrayIcon(HWND hwnd, int IDI_ICON, int WM_CALLBACK, const wchar_t* tip);
    HWND GetParentHWND();
    HWND GetHWNDCHild();
    RECT GetWinRect();
    static bool isInWndHandelMap(const wchar_t* wndName);
    static HWND GetWindowHandel(const wchar_t* wndName);

   public:
    static std::unordered_map<std::wstring, HWND> m_uMap;
    static COLORREF m_sCref;

   private:
    COLORREF m_cRef;
    HINSTANCE hInstance;
    HWND m_hWnd;
    HWND m_hwNdChild;
    RECT mRect;
};

#endif  // _WINDOW_H