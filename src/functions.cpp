#include "functions.h"

#include <algorithm>

struct sRGB {
  sRGB(COLORREF cRef) : color(cRef){};
  union {
    COLORREF color;
    struct {
      COLORREF r : 8, g : 8, b : 8;
    };
  };

  std::wstringstream GetHex() {
    std::wstringstream ss;
    if (r < 16)
      ss << std::hex << 0 << r;
    else
      ss << std::hex << r;
    if (g < 16)
      ss << std::hex << 0 << g;
    else
      ss << std::hex << g;
    if (b < 16)
      ss << std::hex << 0 << b;
    else
      ss << std::hex << b;
    return ss;
  }
};

std::wstring ColorRefToHex(COLORREF cRef) {
  sRGB rgb(cRef);
  std::wstring wstr = rgb.GetHex().str();
  for (auto& c : wstr) c = towupper(c);
  return wstr;
}

std::wstring ColorToRGB(COLORREF cRef) {
  sRGB rgb(cRef);

  return std::wstring{std::to_wstring(rgb.r) + L", " + std::to_wstring(rgb.g) +
                      L", " + std::to_wstring(rgb.b)};
}

std::wstring ColorToHSV(COLORREF cRef) {
  sRGB rgb(cRef);

  float r = rgb.r / 255.0f;
  float g = rgb.g / 255.0f;
  float b = rgb.b / 255.0f;

  float Cmax = max(r, max(g, b));
  float Cmin = min(r, min(g, b));
  float diff = Cmax - Cmin;
  float h = -1, s = -1;

  if (Cmax == Cmin)
    h = 0;
  else if (Cmax == r)
    h = fmod(60 * ((g - b) / diff) + 360, 360);
  else if (Cmax == g)
    h = fmod(60 * ((b - r) / diff) + 120, 360);
  else if (Cmax == b)
    h = fmod(60 * ((r - g) / diff) + 240, 360);

  if (Cmax == 0)
    s = 0;
  else
    s = (diff / Cmax) * 100;

  float v = Cmax * 100.0f;

  return std::wstring{std::to_wstring((short)h) + L"\u00BA   " +
                      std::to_wstring((short)s) + L"%   " +
                      std::to_wstring((short)v) + L"%"};
}

HFONT MakeFont(int size, LPCSTR family, int width, int weight) {
  return CreateFontA(size,
                     width,
                     0,
                     0,
                     weight,
                     FALSE,
                     FALSE,
                     FALSE,
                     ANSI_CHARSET,
                     OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS,
                     DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_SWISS,
                     family);
}
bool DrawIconFormResource(HWND hwnd,
                          HBRUSH hBrush,
                          int bkgColor,
                          unsigned int ICON,
                          int width,
                          int height) {
  hBrush = CreateSolidBrush(bkgColor);
  HICON hIcon = (HICON)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(ICON));
  if (!hIcon) {
    MessageBoxW(NULL,
                L"Load icon from resource failed!",
                L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }
  HDC hdc = GetDC(hwnd);
  RECT rct;
  GetClientRect(hwnd, &rct);
  FillRect(hdc, &rct, hBrush);
  DrawIconEx(
      hdc, rct.left + 5, rct.top + 5, hIcon, width, height, 0, NULL, DI_NORMAL);
  DeleteObject(hBrush);
  ReleaseDC(hwnd, hdc);
  return true;
}

bool TrackWndMouseEvent(HWND hWnd) {
  TRACKMOUSEEVENT me{};
  me.cbSize = sizeof(TRACKMOUSEEVENT);
  me.dwHoverTime = 1;
  me.hwndTrack = hWnd;
  me.dwFlags = TME_HOVER | TME_LEAVE;
  return TrackMouseEvent(&me);
}

int CopyToClipboard(std::wstring wstr) {
  std::string str = {wstr.begin(), wstr.end()};
  OpenClipboard(GetDesktopWindow());
  EmptyClipboard();
  HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, str.size() + 1);
  if (!hg) {
    CloseClipboard();
    return -1;
  }
  memcpy(GlobalLock(hg), str.c_str(), str.size() + 1);
  GlobalUnlock(hg);
  SetClipboardData(CF_TEXT, hg);
  CloseClipboard();
  GlobalFree(hg);
  return 0;
}

int CopyToClipboard(HWND hwnd) {
  {
    char data[256] = {0};
    GetWindowTextA(hwnd, data, 8);
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

void ChangeBackgroundColor(HWND hwnd, HBRUSH hBrush, COLORREF rColor) {
  hBrush = CreateSolidBrush(rColor);
  SetClassLongPtr(hwnd, -10, (LONG_PTR)hBrush);  // change bkg color
  InvalidateRect(hwnd, NULL, TRUE);              // force repaint bkg
}
