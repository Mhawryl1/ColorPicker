
#include <windows.h>

#include "window.h"

int main() {
    RECT screenSize;
    SystemParametersInfoA(SPI_GETWORKAREA, NULL, &screenSize, NULL);
    POINT mPoint;
    POINT wPoint;
    CURSORINFO pci;
    HDC hdc = GetDC(NULL);
    COLORREF cRef;
    Window *pWin = new Window({20, 20, 260, 90});
    // Window *pChild = new Window(pWin->GetParentHWND(), L"ColorPanel", L"Color Panel", {20, 20, 400, 290});
    pWin->CreateNewWindow(L"ColorPanel", L"BigColorPanel", {30, 30, 400, 290});
    pWin->CreateSysTrayIcon(pWin->GetParentHWND(), IDI_PICKER, WM_USER_SHELLICON, L"Color Picker");

    HWND hCopyHEX = pWin->CreateChildWindow(CopyPasteProc, Window::GetWindowHandel(L"BigColorPanel"), L"CopyPaste", L"CopyPasteHEX", {335, 110, 35, 35}, NULL, (LPWSTR)IDC_HAND, RGB(55, 55, 55), NULL, WS_VISIBLE | WS_CHILD);
    pWin->CreateRoundedWindow(hCopyHEX, 5, 5);
    HWND hCopyRGB = pWin->CreateChildWindow(Window::GetWindowHandel(L"BigColorPanel"), L"CopyPaste", L"CopyPasteRGB", {335, 175, 35, 35}, NULL, WS_VISIBLE | WS_CHILD);
    pWin->CreateRoundedWindow(hCopyRGB, 5, 5);
    HWND hCopyHSV = pWin->CreateChildWindow(Window::GetWindowHandel(L"BigColorPanel"), L"CopyPaste", L"CopyPasteHSV", {335, 235, 35, 35}, NULL, WS_VISIBLE | WS_CHILD);
    pWin->CreateRoundedWindow(hCopyHSV, 5, 5);

    pWin->CreateChildWindow(BarMenuProc, Window::GetWindowHandel(L"BigColorPanel"), L"BarMenuClass", L"MiniButtons", {325, 10, 30, 30}, NULL, (LPWSTR)IDC_HAND, RGB(55, 55, 55), NULL, WS_VISIBLE | WS_CHILD);
    pWin->CreateRoundedWindow(Window::GetWindowHandel(L"MiniButtons"), 5, 5);
    pWin->CreateChildWindow(Window::GetWindowHandel(L"BigColorPanel"), L"BarMenuClass", L"CloseButton", {358, 10, 30, 30}, NULL, WS_VISIBLE | WS_CHILD);
    pWin->CreateRoundedWindow(Window::GetWindowHandel(L"CloseButton"), 5, 5);
    // pWin->CreateChildWindow(Window::GetWindowHandel(L"BigColorPanel"), L"BarMenuClass", L"OpenPicker", {15, 10, 30, 30}, NULL, WS_VISIBLE | WS_CHILD);
    // pWin->CreateRoundedWindow(Window::GetWindowHandel(L"OpenPicker"), 5, 5);

    while (pWin->ProcessMessage()) {
        if (GetCursorPos(&mPoint) && IsWindowVisible(pWin->GetParentHWND()) && !IsIconic(pWin->GetParentHWND())) {
            // Prevent window from being past the edge of screen
            wPoint.x = mPoint.x + 260 > screenSize.right ? screenSize.right - 260 : mPoint.x;
            wPoint.y = mPoint.y + 90 > screenSize.bottom ? screenSize.bottom - 90 : mPoint.y;
            pWin->ChangeWinPos(wPoint, pWin->GetWinRect());

            cRef = GetPixel(hdc, mPoint.x, mPoint.y);
            SendMessage(pWin->GetParentHWND(), WM_COMMAND, WM_SENDCOLOR, (LPARAM)cRef);
            SendMessage(pWin->GetHWNDCHild(), WM_ERASEBKGND, NULL, (LPARAM)cRef);
        }

        // if ctr + c or Left mouse button  is pressed send color value to clipboard
        if (((GetKeyState(VK_CONTROL) < 0 && GetKeyState(0x43) < 0) ||
             GetKeyState(VK_LBUTTON) == 0x01) &&
            (!IsWindowVisible(Window::GetWindowHandel(L"BigColorPanel")) && IsWindowVisible(pWin->GetParentHWND()) && !IsIconic(pWin->GetParentHWND()))) {
            ShowWindow(Window::GetWindowHandel(L"BigColorPanel"), SW_SHOW);
            ShowWindow(pWin->GetParentHWND(), SW_HIDE);
            SendMessage(pWin->GetParentHWND(), WM_COPY, NULL, NULL);
            SendMessage(Window::GetWindowHandel(L"BigColorPanel"), WM_COMMAND, WM_SENDCOLOR, (LPARAM)cRef);
            SendMessage(Window::GetWindowHandel(L"PickedColorBigPanel"), WM_SETBKGCOLOR, NULL, (LPARAM)cRef);
            Window::m_sCref = cRef;
            // checking if window is not out of the screen
            // wPoint.x = mPoint.x + 400 > screenSize.right ? screenSize.right - 500 : mPoint.x;
            // wPoint.y = mPoint.y + 290 > screenSize.bottom ? screenSize.bottom - 350 : mPoint.y;
            // pWin->ChangeWinPos(Window::GetWindowHandel(L"PickedColorBigPanel"), wPoint);
        }

        Sleep(10);
    }

    delete pWin;
    return 0;
}