
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
    Window *pChild = new Window(pWin->GetParentHWND(), L"ColorPanel", L"Color Panel", {20, 20, 400, 290});
    pChild->CreateSysTrayIcon(pWin->GetParentHWND(), IDI_PICKER, WM_USER_SHELLICON, L"Color Picker");
    pChild->CreateChildWindow(BarMenuProc, pChild->GetParentHWND(), L"BarMenuClass", L"MiniButtons", {325, 10, 30, 30}, NULL, (LPWSTR)IDC_HAND, RGB(55, 55, 55), NULL, WS_VISIBLE | WS_CHILD);
    pChild->CreateRoundedWindow(Window::GetWindowHandel(L"MiniButtons"), 5, 5);
    pChild->CreateChildWindow(pChild->GetParentHWND(), L"BarMenuClass", L"CloseButton", {358, 10, 30, 30}, NULL, WS_VISIBLE | WS_CHILD);
    pChild->CreateRoundedWindow(Window::GetWindowHandel(L"CloseButton"), 5, 5);
    HWND hCopyHEX = pChild->CreateChildWindow(CopyPasteProc, pChild->GetParentHWND(), L"CopyPaste", L"CopyPasteHEX", {335, 110, 35, 35}, NULL, (LPWSTR)IDC_HAND, RGB(55, 55, 55), NULL, WS_VISIBLE | WS_CHILD);
    pChild->CreateRoundedWindow(hCopyHEX, 5, 5);
    HWND hCopyRGB = pChild->CreateChildWindow(pChild->GetParentHWND(), L"CopyPaste", L"CopyPasteRGB", {335, 175, 35, 35}, NULL, WS_VISIBLE | WS_CHILD);
    pChild->CreateRoundedWindow(hCopyRGB, 5, 5);
    HWND hCopyHSV = pChild->CreateChildWindow(pChild->GetParentHWND(), L"CopyPaste", L"CopyPasteHSV", {335, 235, 35, 35}, NULL, WS_VISIBLE | WS_CHILD);
    pChild->CreateRoundedWindow(hCopyHSV, 5, 5);

    bool running = true;
    while (running) {
        if (!pWin->ProcessMessage()) {
            std::cout << "Close window\n";
            running = false;
        }
        // Press q to quit
        if (GetKeyState(0x51) & 0x1)
            SendMessage(pWin->GetParentHWND(), WM_CLOSE, NULL, NULL);

        if (GetCursorPos(&mPoint) && IsWindowVisible(pWin->GetParentHWND())) {
            // Prevent windows from being past the edge of screen
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
            (!IsWindowVisible(pChild->GetParentHWND()) && IsWindowVisible(pWin->GetParentHWND()))) {
            Window::m_sCref = cRef;
            ShowWindow(pWin->GetParentHWND(), SW_HIDE);
            ShowWindow(pChild->GetParentHWND(), SW_SHOW);
            SendMessage(pWin->GetParentHWND(), WM_COPY, NULL, NULL);
            SendMessage(pChild->GetParentHWND(), WM_COMMAND, WM_SENDCOLOR, (LPARAM)cRef);
            SendMessage(pChild->GetHWNDCHild(), WM_SETBKGCOLOR, NULL, (LPARAM)cRef);
            // checking if window is not out of the screen
            mPoint.x = mPoint.x + 400 > screenSize.right ? screenSize.right - 500 : mPoint.x;
            mPoint.y = mPoint.y + 200 > screenSize.bottom ? screenSize.bottom - 300 : mPoint.y;
            pChild->ChangeWinPos(mPoint, pChild->GetWinRect());
        }

        Sleep(10);
    }

    delete pWin;
    delete pChild;
    return 0;
}