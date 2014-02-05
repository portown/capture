// Tips.cpp

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <memory>
#include <vector>

#include <windows.h>
#include <commctrl.h>

#include <png.h>

#include "defines.h"
#include "funcs.h"


namespace
{
  bool SaveAsBmp(HDC, HBITMAP, char const*);
  bool SaveAsPng(HDC, HBITMAP, char const*);
}


// 簡易メッセージボックス
int Mes(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
  return MessageBox(nullptr, lpText, lpCaption, uType);
}

// 矩形描画（枠のみ）
int DrawBox(HDC hdc, RECT rc)
{
  int fnDrawMode;

  fnDrawMode = SetROP2(hdc, R2_NOTXORPEN);

  MoveToEx(hdc, rc.left, rc.top, nullptr);

  LineTo(hdc, rc.right, rc.top);
  LineTo(hdc, rc.right, rc.bottom);
  LineTo(hdc, rc.left, rc.bottom);
  LineTo(hdc, rc.left, rc.top);

  SetROP2(hdc, fnDrawMode);

  return 1;
}

// 排他的文字列取得
int PutStrXor(HDC hdc, int x, int y, char* lpStr)
{
  HBITMAP hBm;
  SIZE    sStr;
  HDC     hDC;

  GetTextExtentPoint32(hdc, lpStr, lstrlen(lpStr), &sStr);

  hDC = CreateCompatibleDC(hdc);
  hBm = CreateCompatibleBitmap(hdc, sStr.cx, sStr.cy);
  SelectObject(hDC, hBm);
  TextOut(hDC, 0, 0, lpStr, lstrlen(lpStr));
  PatBlt(hDC, 0, 0, sStr.cx, sStr.cy, DSTINVERT);
  BitBlt(hdc, x - sStr.cx / 2, y, sStr.cx, sStr.cy, hDC, 0, 0, SRCINVERT);
  DeleteObject(hBm);
  DeleteDC(hDC);

  return 1;
}

// RECT構造体のソート
int SortRect(LPRECT lprc)
{
  long lTmp;

  if (lprc->right < lprc->left)
  {
    lTmp        = lprc->right;
    lprc->right = lprc->left;
    lprc->left  = lTmp;
  }

  if (lprc->bottom < lprc->top)
  {
    lTmp         = lprc->bottom;
    lprc->bottom = lprc->top;
    lprc->top    = lTmp;
  }

  return 1;
}

// サーフェイスの初期化
int InitSurface(HWND hWnd, HDC& hDC, HBITMAP& hBm, int w, int h)
{
  HDC hTempDC;

  hTempDC = GetDC(hWnd);
  hDC     = CreateCompatibleDC(hTempDC);
  hBm     = CreateCompatibleBitmap(hTempDC, w, h);
  SelectObject(hDC, hBm);
  ReleaseDC(hWnd, hTempDC);

  return 1;
}

// 画像保存
BOOL SavePicture(char const* szFName, HDC hDC, HBITMAP hBm)
{
  BITMAP Bm;

  GetObject(hBm, sizeof(BITMAP), &Bm);
  if (Bm.bmWidth == 1)
    return FALSE;

  if (!lstrcmp(&szFName[lstrlen(szFName) - 4], ".bmp"))
    SaveAsBmp(hDC, hBm, szFName);

  if (!lstrcmp(&szFName[lstrlen(szFName) - 4], ".png"))
    SaveAsPng(hDC, hBm, szFName);

  return TRUE;
}

namespace
{
  constexpr auto count_bits(::BITMAP const& bitmap) -> ::DWORD
  {
    return (((bitmap.bmWidth * bitmap.bmBitsPixel + 31) & ~31) >> 3) * bitmap.bmHeight;
  }

  // BMP保存
  bool SaveAsBmp(HDC hDC, HBITMAP hBm, char const* szFName)
  {
    ::BITMAP Bm;
    ::GetObject(hBm, sizeof(::BITMAP), &Bm);

    ::BITMAPINFO bi;
    ::ZeroMemory(&bi.bmiHeader, sizeof(::BITMAPINFOHEADER));
    bi.bmiHeader.biBitCount = 24;
    bi.bmiHeader.biWidth    = Bm.bmWidth;
    bi.bmiHeader.biHeight   = Bm.bmHeight;
    bi.bmiHeader.biSize     = sizeof(::BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes   = 1;

    std::vector<unsigned char> bits(count_bits(Bm));

    ::GetDIBits(hDC, hBm, 0, Bm.bmHeight, bits.data(), &bi, DIB_RGB_COLORS);

    std::ofstream of(szFName, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!of)
    {
      Mes("ファイルの保存に失敗しました", nullptr, MB_OK | MB_ICONERROR);
      return false;
    }

    ::BITMAPFILEHEADER bf;
    ::ZeroMemory(&bf, sizeof(::BITMAPFILEHEADER));
    bf.bfType    = MAKEWORD('B', 'M');
    bf.bfOffBits = sizeof(::BITMAPFILEHEADER) + sizeof(::BITMAPINFOHEADER);
    bf.bfSize    = bits.size() + bf.bfOffBits;

    of.write(reinterpret_cast<char const*>(&bf), sizeof(bf));
    of.write(reinterpret_cast<char const*>(&bi.bmiHeader), sizeof(bi.bmiHeader));
    of.write(reinterpret_cast<char const*>(bits.data()), bits.size());

    return of;
  }

  // PNG保存
  bool WritePng(char const* szFName, unsigned char const* lpScBits, int width, int height)
  {
    std::unique_ptr<FILE, decltype(&std::fclose)> const fp(std::fopen(szFName, "wb"), &std::fclose);
    if (!fp) return false;

    auto lpps = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!lpps) return false;

    auto lppi = ::png_create_info_struct(lpps);
    if (!lppi)
    {
      ::png_destroy_write_struct(&lpps, nullptr);
      return false;
    }

    if (setjmp(png_jmpbuf(lpps)))
    {
      ::png_destroy_write_struct(&lpps, &lppi);
      return false;
    }

    ::png_init_io(lpps, fp.get());

    ::png_set_IHDR(lpps, lppi, width, height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    ::png_write_info(lpps, lppi);

    // width * 3 を下回らない最小の 4 の倍数
    auto const bytes_per_line = ((width * 3 + 3) / 4) * 4;
    for (auto i = 0; i < height; ++i)
    {
      std::vector<png_byte> bits(width * 3);
      auto const* src = lpScBits + (height - 1 - i) * bytes_per_line;
      for (auto it = bits.begin(); it != bits.end();)
      {
        *it++ = src[2];
        *it++ = src[1];
        *it++ = src[0];
        src += 3;
      }
      ::png_write_row(lpps, bits.data());
    }

    ::png_write_end(lpps, lppi);

    ::png_destroy_write_struct(&lpps, &lppi);

    return true;
  }

  // PNG保存
  bool SaveAsPng(HDC hDC, HBITMAP hBm, char const* szFName)
  {
    ::BITMAP Bm;
    ::GetObject(hBm, sizeof(::BITMAP), &Bm);

    ::BITMAPINFO bi;
    ::ZeroMemory(&bi.bmiHeader, sizeof(::BITMAPINFOHEADER));
    bi.bmiHeader.biBitCount = 24;
    bi.bmiHeader.biWidth    = Bm.bmWidth;
    bi.bmiHeader.biHeight   = Bm.bmHeight;
    bi.bmiHeader.biSize     = sizeof(::BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes   = 1;

    std::vector<unsigned char> bits(count_bits(Bm));

    ::GetDIBits(hDC, hBm, 0, Bm.bmHeight, bits.data(), &bi, DIB_RGB_COLORS);

    WritePng(szFName, bits.data(), Bm.bmWidth, Bm.bmHeight);

    return true;
  }
}

// タブコントロールの作成
HWND CreateMyTab(HWND hWnd)
{
  INITCOMMONCONTROLSEX ic;
  HINSTANCE            hInst;
  HWND                 hTab;

  ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
  ic.dwICC  = ICC_TAB_CLASSES;
  InitCommonControlsEx(&ic);

  hInst = ( HINSTANCE )GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

  hTab = CreateWindowEx(0,
                        WC_TABCONTROL,
                        "",
                        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                        0, 0, 0, 0, hWnd,
                        ( HMENU )ID_MYTAB, hInst, nullptr);

  return hTab;
}

// タブの追加
int AddTab(HWND hTab, int nMax)
{
  TCITEM ti;
  char   szText[64];

  wsprintf(szText, "No.%d", nMax + 1);

  ZeroMemory(&ti, sizeof(TCITEM));
  ti.mask    = TCIF_TEXT;
  ti.pszText = szText;
  TabCtrl_InsertItem(hTab, nMax, &ti);

  TabCtrl_SetCurSel(hTab, nMax);

  return 1;
}

// タブ名の変更
int SetTabText(HWND hTab, int nSel, char const* lpText)
{
  TCITEM ti;

  ZeroMemory(&ti, sizeof(TCITEM));
  ti.mask    = TCIF_TEXT;
  ti.pszText = const_cast<char*>(lpText);
  TabCtrl_SetItem(hTab, nSel, &ti);

  return 1;
}

// タブ名の取得
int GetTabText(HWND hTab, int nSel, char* lpText)
{
  TCITEM ti;

  TabCtrl_GetItem(hTab, nSel, &ti);
  lstrcpy(lpText, ti.pszText);

  return 1;
}

// DCSETの作成
DCSET CreateDCSet()
{
  DCSET DCSet;

  DCSet.bSave = FALSE;

  return DCSet;
}
