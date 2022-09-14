#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include <windows.h>

#include <sstream>

std::wstring ColorRefToHex(COLORREF cRef);

std::wstring ColorToRGB(COLORREF cRef);

std::wstring ColorToHSV(COLORREF cRef);

HFONT MakeFont(int size = 20, LPCSTR family = "Arial", int width = 0, int weight = FW_DONTCARE);

bool DrawIconFormResource(HWND hwnd, HBRUSH hBrush, int bkgColor, unsigned int ICON, int width, int height);

#endif  // __FUNCTIONS_H