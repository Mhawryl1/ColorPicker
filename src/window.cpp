
#include "window.h"

std::unordered_map<std::wstring, HWND> Window::m_uMap;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hWNDTx;
    static HBRUSH hBrush = NULL;
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CREATE: {
            hWNDTx = CreateWindow(L"STATIC", L"HEXCOLOR", WS_VISIBLE | WS_CHILD, 90, 22, 140, 30, hwnd, NULL, NULL, NULL);
        } break;
        case WM_USER_SHELLICON: {
            switch (lParam) {
                case WM_LBUTTONDOWN:
                    ShowWindow(Window::GetWindowHandel(L"Color Picker"), SW_SHOW);

                    break;
                case WM_RBUTTONDOWN: {
                    POINT pt;
                    HBITMAP icon = LoadBitmap(GetModuleHandle(NULL), (LPCWSTR)IDI_CLOSEBMP);
                    GetCursorPos(&pt);
                    HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_POPUPMENU));
                    hMenu = GetSubMenu(hMenu, 0);
                    AppendMenu(hMenu, MF_BITMAP, IDM_CLOSE, LPCWSTR(IDI_PICKER));
                    TrackPopupMenu(hMenu, TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_HORNEGANIMATION, pt.x, pt.y, 0, hwnd, NULL);
                } break;

                default:
                    break;
            }
        } break;

            break;
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            if (lParam == (LPARAM)hWNDTx) {
                SetTextColor(hdcStatic, RGB(204, 204, 204));
                SetBkColor(hdcStatic, RGB(45, 45, 45));
                if (!hBrush)
                    hBrush = CreateSolidBrush(RGB(45, 45, 45));
                return (LRESULT)hBrush;
            }
        } break;

        case WM_COMMAND: {
            switch (wParam) {
                case WM_SENDCOLOR: {
                    HDC hdc = GetDC(hwnd);
                    COLORREF cRef = COLORREF(lParam);
                    std::wstring wstr = ColorRefToHex(cRef);
                    HFONT hFont = MakeFont(25);
                    SetWindowText(hWNDTx, (L'#' + wstr).c_str());
                    SetWindowText(hwnd, wstr.c_str());
                    SendMessage(hWNDTx, WM_SETFONT, WPARAM(hFont), TRUE);
                    ReleaseDC(hwnd, hdc);
                } break;
                case IDM_TEST:  // SUB menu Message handel
                    std::cout << "Test\n";
                    break;
                case IDM_CLOSE:
                    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                    break;
                default:
                    break;
            }
        } break;

        case WM_COPY: {
            auto pWin = reinterpret_cast<Window*>(lParam);
            if (!pWin->CopyToClipboard()) {
                std::cout << "Copy to clipboard\n";
            }
        } break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK PanelProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hBrush;
    switch (uMsg) {
        case WM_ERASEBKGND: {
            COLORREF cRef = COLORREF(lParam);
            HDC hdc = GetDC(hWnd);
            hBrush = CreateSolidBrush(cRef);
            RECT rc;
            GetClientRect(hWnd, &rc);
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);
            ReleaseDC(hWnd, hdc);
        } break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK BigPanelColorProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hBrush;
    switch (uMsg) {
        case WM_SETBKGCOLOR: {
            COLORREF cRef = COLORREF(lParam);
            hBrush = CreateSolidBrush(cRef);
            // set background color copied to clipboard
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
        } break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ColorPanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hWndTxRGB = NULL;
    static HWND hWndTxHEX = NULL;
    static HWND hWndTxHSV = NULL;
    static HWND hWndLine = NULL;
    static HWND hButton = NULL;
    static HBRUSH hBrush = NULL;
    static POINT lastLocation;
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CREATE: {
            hWndTxHEX = CreateWindow(L"STATIC", L"HEXCOLOR", WS_VISIBLE | WS_CHILD, 30, 80, 250, 20, hwnd, NULL, NULL, NULL);
            if (!Window::isInWndHandelMap(L"HEXCOLOR"))
                Window::m_uMap.insert({L"HEXCOLOR", hWndTxHEX});

            hWndTxRGB = CreateWindow(L"STATIC", L"RGBCOLOR", WS_VISIBLE | WS_CHILD, 30, 140, 250, 20, hwnd, NULL, NULL, NULL);
            if (!Window::isInWndHandelMap(L"RGBCOLOR"))
                Window::m_uMap.insert({L"RGBCOLOR", hWndTxRGB});

            hWndTxHSV = CreateWindow(L"STATIC", L"HSVCOLOR", WS_VISIBLE | WS_CHILD, 30, 200, 250, 20, hwnd, NULL, NULL, NULL);
            if (!Window::isInWndHandelMap(L"HSVCOLOR"))
                Window::m_uMap.insert({L"HSVCOLOR", hWndTxHSV});

            //  button with icon and custom background color BS_OWNERDRAW style define custom button and WM_DRAWITEM handel button
            hButton = CreateWindowW(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 15, 10, 30, 30, hwnd, (HMENU)ID_ICONBUTTONCLIK, NULL, NULL);
            if (!Window::isInWndHandelMap(L"hButton"))
                Window::m_uMap.insert({L"hButton", hButton});

            // change cursor when is on the button
            SetClassLongPtr(hButton, -12, (LONG_PTR)LoadCursor(NULL, IDC_HAND));
        } break;

        case WM_LBUTTONDOWN: {
            GetCursorPos(&lastLocation);
            RECT rc;
            GetWindowRect(hwnd, &rc);
            lastLocation.x = lastLocation.x - rc.left;
            lastLocation.y = lastLocation.y - rc.top;
        } break;
        case WM_MOUSEMOVE: {
            if (GetKeyState(VK_LBUTTON) & 0x1 << 15) {
                POINT currentPos;
                RECT rc;
                GetCursorPos(&currentPos);
                int32_t x = currentPos.x - lastLocation.x;
                int32_t y = currentPos.y - lastLocation.y;

                MoveWindow(hwnd, x, y, 400, 250, TRUE);
            }

        } break;
        case WM_DRAWITEM: {
            /////////////////Draw custom button with icon///////////////
            auto ds = (LPDRAWITEMSTRUCT)lParam;
            if (ds->hwndItem == hButton) {
                // Load icon from resource file
                HICON hIcon = (HICON)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_PICKER));
                if (!hIcon) {
                    std::cerr << "Couldn't load image\n";
                    return -1;
                }
                FillRect(ds->hDC, &ds->rcItem, hBrush);
                DrawIconEx(
                    ds->hDC,
                    ds->rcItem.left,
                    ds->rcItem.top,
                    hIcon,
                    30,
                    30,
                    0, NULL, DI_NORMAL);
            }
        } break;
        case WM_PAINT: {
            //////Paint separating line ///////////
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            HBRUSH bBrush = CreateSolidBrush(RGB(45, 45, 45));
            SelectObject(ps.hdc, bBrush);
            RoundRect(ps.hdc, 5, 5, 395, 245, 5, 5);
            RECT lRect = {30, 120, 370, 122};
            DrawEdge(ps.hdc, &lRect, EDGE_BUMP, BF_BOTTOM);
            lRect.top += 60;
            lRect.bottom += 62;
            DrawEdge(ps.hdc, &lRect, EDGE_BUMP, BF_BOTTOM);
            EndPaint(hwnd, &ps);
            DeleteObject(bBrush);
        } break;

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            if (lParam == (LPARAM)hWndTxRGB ||
                lParam == (LPARAM)hWndTxHEX ||
                lParam == (LPARAM)hWndTxHSV) {
                SetTextColor(hdcStatic, RGB(204, 204, 204));
                SetBkColor(hdcStatic, RGB(45, 45, 45));
                if (!hBrush)
                    hBrush = CreateSolidBrush(RGB(45, 45, 45));
                return (LRESULT)hBrush;
            }
        } break;

        case WM_COMMAND: {
            switch (wParam) {
                case ID_ICONBUTTONCLIK: {
                    HWND hParent = GetParent(hwnd);
                    ShowWindow(hwnd, SW_HIDE);
                    ShowWindow(hParent, SW_SHOW);

                } break;
                case WM_SENDCOLOR: {
                    COLORREF cRef = (COLORREF)lParam;
                    std::wstring wstr(L"HEX:     " + ColorRefToHex(cRef));
                    SetWindowText(hWndTxHEX, wstr.c_str());
                    HFONT hFont = MakeFont(22, "Arial");
                    SendMessage(hWndTxHEX, WM_SETFONT, WPARAM(hFont), TRUE);

                    std::wstring wstrgb(L"RGB:     " + ColorToRGB(cRef));
                    SetWindowText(hWndTxRGB, wstrgb.c_str());
                    SendMessage(hWndTxRGB, WM_SETFONT, WPARAM(hFont), TRUE);

                    std::wstring wsthsv(L"HSV:     " + ColorToHSV(cRef));
                    SetWindowText(hWndTxHSV, wsthsv.c_str());
                    SendMessage(hWndTxHSV, WM_SETFONT, WPARAM(hFont), TRUE);
                } break;
                default:
                    break;
            }

        } break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK BarMenuProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static bool TrackingMouse = TRUE;
    static HBRUSH hBrush = NULL;
    switch (uMsg) {
        case WM_MOUSEMOVE:
            if (TrackingMouse && hwnd == Window::GetWindowHandel(L"MiniButtons")) {
                TRACKMOUSEEVENT me{};
                me.cbSize = sizeof(TRACKMOUSEEVENT);
                me.dwHoverTime = 1;
                me.hwndTrack = hwnd;
                me.dwFlags = TME_HOVER | TME_LEAVE;
                TrackingMouse = FALSE;
                TrackMouseEvent(&me);
            }
            if (TrackingMouse && hwnd == Window::GetWindowHandel(L"CloseButton")) {
                TRACKMOUSEEVENT me{};
                me.cbSize = sizeof(TRACKMOUSEEVENT);
                me.dwHoverTime = 1;
                me.hwndTrack = hwnd;
                me.dwFlags = TME_HOVER | TME_LEAVE;
                TrackingMouse = FALSE;
                TrackMouseEvent(&me);
            }
            break;
        case WM_MOUSEHOVER: {
            if (hBrush)
                DeleteObject(hBrush);
            hBrush = CreateSolidBrush(RGB(100, 0, 0));
            SetClassLongPtr(hwnd, -10, (LONG_PTR)hBrush);
            InvalidateRect(hwnd, NULL, TRUE);
        } break;
        case WM_MOUSELEAVE: {
            if (hBrush)
                DeleteObject(hBrush);
            hBrush = CreateSolidBrush(RGB(55, 55, 55));
            SetClassLongPtr(hwnd, -10, (LONG_PTR)hBrush);  // change bkg color
            InvalidateRect(hwnd, NULL, TRUE);              // force repaint bkg
            TrackingMouse = TRUE;
        } break;
        case WM_PAINT: {
            if (hwnd == Window::GetWindowHandel(L"MiniButtons")) {
                PAINTSTRUCT ps;
                BeginPaint(hwnd, &ps);
                RECT rct;
                GetClientRect(hwnd, &rct);
                InflateRect(&rct, -5, -10);
                DrawEdge(ps.hdc, &rct, EDGE_BUMP, BF_BOTTOM);
                EndPaint(hwnd, &ps);
            }
            if (hwnd = Window::GetWindowHandel(L"CloseButton")) {
                PAINTSTRUCT ps;
                BeginPaint(hwnd, &ps);
                RECT rct;
                GetClientRect(hwnd, &rct);
                InflateRect(&rct, -8, -8);
                DrawEdge(ps.hdc, &rct, EDGE_BUMP, BF_DIAGONAL_ENDTOPLEFT);
                DrawEdge(ps.hdc, &rct, EDGE_BUMP, BF_DIAGONAL);
                EndPaint(hwnd, &ps);
            }
        } break;
        case WM_LBUTTONDOWN: {
            if (hwnd == Window::GetWindowHandel(L"CloseButton")) {
                ShowWindow(Window::GetWindowHandel(L"Color Panel"), SW_HIDE);
            }
            if (hwnd == Window::GetWindowHandel(L"MiniButtons")) {
            }
        } break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK CopyPasteProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hBrush = NULL;
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            HDC hdc = GetDC(hwnd);
            RECT rct;
            GetClientRect(hwnd, &rct);
            // Load icon from resource file
            HICON hIcon = (HICON)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_PASTE));
            if (!hIcon) {
                std::cerr << "Couldn't load image\n";
                return -1;
            }
            hBrush = CreateSolidBrush(RGB(100, 0, 0));
            FillRect(hdc, &rct, hBrush);
            DrawIconEx(
                hdc,
                rct.left,
                rct.top,
                hIcon,
                30,
                30,
                0, NULL, DI_NORMAL);
        } break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

Window::Window(const RECT& rc)
    : hInstance(GetModuleHandle(NULL)),
      mRect(rc) {
    const wchar_t* CLASS_NAME = L"SmallWndClass";
    const wchar_t* wndName = L"Color Picker";
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(hInstance, IDC_IBEAM);
    wc.hbrBackground = CreateSolidBrush(RGB(45, 45, 45));
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PICKER));
    RegisterClass(&wc);

    // Create window
    m_hWnd = CreateWindowEx(
        NULL,
        CLASS_NAME,
        wndName,
        WS_POPUP,
        // window position
        mRect.left, mRect.top, mRect.right, mRect.bottom,
        NULL,  // Parent window
        NULL,  // menu
        hInstance,
        NULL  // additional application data
    );

    if (m_hWnd == NULL) {
        std::cout << "Couldn't create window\n'";
        Window::~Window();
        return;
    }
    if (!Window::isInWndHandelMap(wndName)) {
        Window::m_uMap[wndName] = m_hWnd;
    }

    HRGN elips = CreateRoundRectRgn(0, 0, 240, 70, 10, 10);
    HBRUSH hBrush = CreateSolidBrush(RGB(96, 96, 96));
    SetWindowRgn(m_hWnd, elips, TRUE);

    // Create small window with current picked color by mouse pointer
    WNDCLASS pc = {0};
    pc.lpszClassName = L"Color child window";
    pc.hbrBackground = hBrush;
    pc.lpfnWndProc = PanelProc;
    pc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&pc);

    RECT cRect = {10, 7, 55, 55};
    m_hwNdChild = CreateWindowW(
        L"Color child window",
        NULL,
        WS_CHILD | WS_VISIBLE,
        cRect.left, cRect.top, cRect.right, cRect.bottom,
        m_hWnd,
        NULL,
        NULL,
        NULL);

    HRGN rec = CreateRoundRectRgn(0, 0, cRect.right, cRect.bottom, 5, 5);
    SetWindowRgn(m_hwNdChild, rec, TRUE);

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    // clearing
    DeleteObject(hBrush);
    DeleteObject(elips);
}

Window::Window(HWND hwnd, const wchar_t* className, const wchar_t* wndTitle, const RECT& rc) : mRect(rc) {
    WNDCLASS wc = {0};
    wc.lpszClassName = className;
    wc.hbrBackground = CreateSolidBrush(RGB(65, 65, 65));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc = ColorPanelProc;
    RegisterClass(&wc);
    m_hWnd = CreateWindowEx(
        NULL,
        className,
        wndTitle,
        WS_POPUP | WS_CHILD,
        mRect.left, mRect.top, mRect.right, mRect.bottom,
        hwnd,
        NULL,
        NULL,
        NULL);

    if (!Window::isInWndHandelMap(wndTitle))
        Window::m_uMap[wndTitle] = m_hWnd;

    HRGN rRect = CreateRoundRectRgn(0, 0, mRect.right, mRect.bottom, 10, 10);
    SetWindowRgn(m_hWnd, rRect, TRUE);

    /////////////////////////////////////////////////////////////
    WNDCLASS pc = {0};
    pc.lpszClassName = L"Color_window";
    pc.hbrBackground = CreateSolidBrush(RGB(100, 45, 45));
    pc.lpfnWndProc = BigPanelColorProc;
    pc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&pc);

    RECT cRect = {345, 195, 50, 50};
    m_hwNdChild = CreateWindowW(
        L"Color_window",
        NULL,
        WS_CHILD | WS_VISIBLE,
        cRect.left, cRect.top, cRect.right, cRect.bottom, m_hWnd,
        NULL,
        NULL,
        NULL);

    if (!m_hwNdChild) {
        MessageBoxW(NULL, L"Create widnow failed!", L"Error!", MB_OK | MB_ICONERROR);
        return;
    }

    HRGN rec = CreateRoundRectRgn(0, 0, cRect.right, cRect.bottom, 5, 5);
    SetWindowRgn(m_hwNdChild, rec, TRUE);

    // UpdateWindow(m_hWnd);
}

bool Window::isInWndHandelMap(const wchar_t* wndName) {
    if (m_uMap.contains(wndName)) {
        MessageBoxW(NULL, L"Window name must be unique", L"Warning.", MB_ICONWARNING | MB_OK);
        return true;
    }
    return false;
}

HWND Window::GetWindowHandel(const wchar_t* wndName) {
    if (!m_uMap.contains(wndName)) {
        MessageBoxW(NULL, L"Invalid handel name!", L"Warning", MB_ICONWARNING | MB_OK);
        return 0;
    }
    return m_uMap[wndName];
}

HWND Window::CreateChildWindow(WNDPROC WndProc, HWND hwnd, const wchar_t* className, const wchar_t* windowTitle, const RECT& rct, LPWSTR ICON, LPWSTR CURSOR, COLORREF color, HMENU hMenu, DWORD STYLE) {
    if (WndProc == NULL) {
        WndProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    }
    if (ICON == NULL)
        ICON = IDI_APPLICATION;
    if (hwnd == NULL)
        hwnd = Window::GetParentHWND();
    if (CURSOR == NULL)
        CURSOR = IDC_ARROW;
    if (STYLE == NULL)
        STYLE = WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE;
    if (className == NULL)
        className = L"Default Name";
    if (windowTitle == NULL || isInWndHandelMap(windowTitle)) {
        return 0;
    }

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hIcon = LoadIcon(NULL, (LPCWSTR)ICON);
    wc.hCursor = LoadCursor(NULL, (LPCWSTR)CURSOR);
    wc.hbrBackground = CreateSolidBrush(color);
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Class Registration Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hWND = CreateWindowEx(NULL,
                               className,
                               windowTitle,
                               STYLE,
                               rct.left, rct.top, rct.right, rct.bottom,
                               hwnd,
                               hMenu,
                               hInstance,
                               NULL);

    if (hWND == NULL) {
        DWORD error = GetLastError();
        MessageBox(NULL,
                   (std::wstring(L"Creating window failed! Error code: ") + std::to_wstring(error)).c_str(),
                   L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    Window::m_uMap[windowTitle] = hWND;
    return hWND;
}

HWND Window::CreateChildWindow(HWND hwnd, const wchar_t* className, const wchar_t* windowTitle, const RECT& pos, HMENU hMenu, DWORD STYLE) {
    wchar_t buffer[25] = {0};
    if (STYLE == NULL)
        STYLE = WS_CHILD | WS_VISIBLE;
    if (windowTitle == NULL || isInWndHandelMap(windowTitle)) {
        return 0;
    }
    if (!wcslen(className)) {
        GetClassNameW(hwnd, buffer, 25);
    } else {
        wcscpy(buffer, className);
    }
    if (!wcslen(buffer)) {
        MessageBox(NULL, L"Class name is required!", L"Error", MB_ICONERROR | MB_OK);
    }
    HWND hWND = CreateWindowEx(NULL,
                               buffer,
                               windowTitle,
                               STYLE,
                               pos.left, pos.top, pos.right, pos.bottom,
                               hwnd,
                               hMenu,
                               hInstance,
                               NULL);

    if (hWND == NULL) {
        MessageBox(NULL, L"Creating window failed!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }
    Window::m_uMap[windowTitle] = hWND;
    return hWND;
}

bool Window::CreateRoundedWindow(HWND hwnd, short w, short h) {
    RECT rgn = {0};
    GetWindowRect(hwnd, &rgn);

    HRGN hRgn = CreateRoundRectRgn(0, 0, rgn.right - rgn.left, rgn.bottom - rgn.top, w, h);
    if (hRgn) {
        SetWindowRgn(hwnd, hRgn, TRUE);
        return true;
    }
    DeleteObject(hRgn);
    return false;
}

bool Window::CreateSysTrayIcon(HWND hwnd, int IDI_ICON, int WM_CALLBACK, const wchar_t* tip) {
    NOTIFYICONDATA nd{0};
    nd.cbSize = sizeof(NOTIFYICONDATA);
    nd.hWnd = hwnd;
    nd.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
    nd.uID = IDI_ICON;
    nd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_STATE | NIF_GUID | NIF_REALTIME | NIF_SHOWTIP;
    nd.uCallbackMessage = WM_CALLBACK;
    wcscpy_s(nd.szTip, sizeof(nd.szTip), tip);
    nd.dwState = NIS_SHAREDICON;

    return Shell_NotifyIcon(NIM_ADD, &nd);
}
Window::~Window() {
    const wchar_t* CLASS_NAME = L"Color picker";
    UnregisterClass(CLASS_NAME, hInstance);
    UnregisterClass(L"ColorPanel", hInstance);
}

bool Window::ProcessMessage() {
    MSG msg = {};
    while (PeekMessageA(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

void Window::ChangeWinPos(POINT pnt, RECT rc) {
    SetWindowPos(
        m_hWnd,
        HWND_TOPMOST,
        pnt.x + 20,
        pnt.y + 20,
        rc.right,
        rc.bottom,
        SWP_SHOWWINDOW);
}

HWND Window::GetParentHWND() {
    return m_hWnd;
}

HWND Window::GetHWNDCHild() {
    return m_hwNdChild;
}

int Window::CopyToClipboard() {
    {
        char data[256];
        GetWindowTextA(this->GetParentHWND(), data, 8);
        std::cout << "Title: " << data << std::endl;
        OpenClipboard(GetDesktopWindow());
        EmptyClipboard();
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, ARRAYSIZE(data) + 1);
        if (!hg) {
            CloseClipboard();
            return -1;
        }
        memcpy(GlobalLock(hg), data, ARRAYSIZE(data) + 1);
        GlobalUnlock(hg);
        SetClipboardData(CF_TEXT, hg);
        CloseClipboard();
        GlobalFree(hg);
    }
    return 0;
}

RECT Window::GetWinRect() {
    return mRect;
}