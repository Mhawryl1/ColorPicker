#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include <windows.h>

#include <codecvt>
#include <locale>  // for wstring_convert
#include <sstream>

std::wstring ColorRefToHex(COLORREF cRef);

std::wstring ColorToRGB(COLORREF cRef);

std::wstring ColorToHSV(COLORREF cRef);

int CopyToClipboard(std::wstring str);

int CopyToClipboard(HWND hwnd);

HFONT MakeFont(int size = 20, LPCSTR family = "Arial", int width = 0, int weight = FW_DONTCARE);

bool DrawIconFormResource(HWND hwnd, HBRUSH hBrush, int bkgColor, unsigned int ICON, int width, int height);

bool TrackWndMouseEvent(HWND hWnd);

void ChangeBackgroundColor(HWND hwnd, HBRUSH brush, COLORREF rColor);

#endif  // __FUNCTIONS_H