// Tips.cpp

#include <windows.h>
#include <commctrl.h>

#include <png.h>

#include "defines.h"
#include "funcs.h"


extern int nCapKey;
extern BOOL bCapCtrl;
extern BOOL bCapShift;
extern BOOL bCapAlt;


namespace
{
  bool GetSaveName(HWND, char*, char*, DWORD);
  bool SaveAsBmp(HDC, HBITMAP, char*);
  bool SaveAsPng(HDC, HBITMAP, char*);
}


// �ȈՃ��b�Z�[�W�{�b�N�X
int Mes(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
  return MessageBox(nullptr, lpText, lpCaption, uType);
}

// ��`�`��i�g�̂݁j
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

// �r���I������擾
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

// RECT�\���̂̃\�[�g
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

// �T�[�t�F�C�X�̏�����
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

// �摜�ۑ�
BOOL SavePicture(HWND hWnd, HWND hTab, int nSel, HDC hDC, HBITMAP hBm)
{
  BITMAP Bm;
  char   szFName[MAX_PATH];
  char   szTitle[MAX_PATH];

  GetObject(hBm, sizeof(BITMAP), &Bm);
  if (Bm.bmWidth == 1)
    return FALSE;

  if (!GetSaveName(hWnd, szFName, szTitle, sizeof(szFName)))
    return FALSE;

  if (!lstrcmp(&szFName[lstrlen(szFName) - 4], ".bmp"))
    SaveAsBmp(hDC, hBm, szFName);

  if (!lstrcmp(&szFName[lstrlen(szFName) - 4], ".png"))
    SaveAsPng(hDC, hBm, szFName);

  SetTabText(hTab, nSel, szTitle);

  return TRUE;
}

namespace
{
  // BMP�ۑ�
  bool SaveAsBmp(HDC hDC, HBITMAP hBm, char* szFName)
  {
    BITMAPFILEHEADER bf;
    BITMAPINFO       bi;
    HGLOBAL          hBits;
    BITMAP           Bm;
    HANDLE           hFile;
    LPBYTE           lpBits;
    DWORD            dwBmSize;
    DWORD            dwWritten;

    GetObject(hBm, sizeof(BITMAP), &Bm);

    dwBmSize = ((((Bm.bmWidth * Bm.bmBitsPixel) + 31) & ~31) >> 3) * Bm.bmHeight;

    ZeroMemory(&bf, sizeof(BITMAPFILEHEADER));
    bf.bfType    = MAKEWORD('B', 'M');
    bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bf.bfSize    = dwBmSize + bf.bfOffBits;

    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biBitCount = 24;
    bi.bmiHeader.biWidth    = Bm.bmWidth;
    bi.bmiHeader.biHeight   = Bm.bmHeight;
    bi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes   = 1;

    hBits  = GlobalAlloc(GHND, dwBmSize);
    lpBits = ( LPBYTE )GlobalLock(hBits);

    GetDIBits(hDC, hBm, 0, Bm.bmHeight, lpBits, &bi, DIB_RGB_COLORS);

    hFile = CreateFile(szFName, GENERIC_WRITE, 0, nullptr,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
      Mes("�t�@�C���̕ۑ��Ɏ��s���܂���", nullptr, MB_OK | MB_ICONERROR);
      return false;
    }

    WriteFile(hFile, &bf, sizeof(BITMAPFILEHEADER), &dwWritten, nullptr);
    WriteFile(hFile, &bi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwWritten, nullptr);
    WriteFile(hFile, lpBits, dwBmSize, &dwWritten, nullptr);

    CloseHandle(hFile);

    GlobalUnlock(hBits);
    GlobalFree(hBits);

    return true;
  }

  // PNG�ۑ�
  bool WritePng(char* szFName, LPBYTE lpScBits, int Width, int Height)
  {
    FILE*       fp;
    png_structp lpps;
    png_bytepp  lpBits;
    png_infop   lppi;
    int         i, j;
    char*       src, * dest;
    int         bmp_line_byts;

    bmp_line_byts = ((Width * 3 + 3) / 4) * 4;
    lpBits        = ( png_bytepp )malloc(Height * sizeof(LPBYTE));
    for (i = 0; i < Height; i++)
    {
      lpBits[i] = ( png_bytep )malloc(Width * 3);
      dest      = ( char* )lpBits[i];
      src       = ( char* )lpScBits + ((Height - 1 - i) * bmp_line_byts);
      for (j = 0; j < Width; j++)
      {
        *dest++ = *(src + 2);
        *dest++ = *(src + 1);
        *dest++ = *src;
        src    += 3;
      }
    }

    if (!(fp = fopen(szFName, "wb")))
    {
      fclose(fp);
      return false;
    }

    lpps = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!lpps)
    {
      fclose(fp);
      return false;
    }

    lppi = png_create_info_struct(lpps);
    if (!lppi)
    {
      png_destroy_write_struct(&lpps, ( png_infopp )nullptr);
      fclose(fp);
      return false;
    }

    if (setjmp(png_jmpbuf(lpps)))
    {
      png_destroy_write_struct(&lpps, &lppi);
      fclose(fp);
      return false;
    }

    png_init_io(lpps, fp);

    png_set_IHDR(lpps, lppi, Width, Height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(lpps, lppi);
    png_write_image(lpps, lpBits);
    png_write_end(lpps, lppi);

    png_destroy_write_struct(&lpps, &lppi);

    fclose(fp);

    for (i = 0; i < Height; i++)
      delete [] lpBits[i];
    delete [] lpBits;

    return true;
  }

  // PNG�ۑ�
  bool SaveAsPng(HDC hDC, HBITMAP hBm, char* szFName)
  {
    BITMAPINFO bi;
    HGLOBAL    hBits;
    LPBYTE     lpBits;
    BITMAP     Bm;
    DWORD      dwBmSize;

    GetObject(hBm, sizeof(BITMAP), &Bm);

    dwBmSize = ((((Bm.bmWidth * Bm.bmBitsPixel) + 31) & ~31) >> 3) * Bm.bmHeight;

    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biBitCount = 24;
    bi.bmiHeader.biWidth    = Bm.bmWidth;
    bi.bmiHeader.biHeight   = Bm.bmHeight;
    bi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes   = 1;

    hBits  = GlobalAlloc(GHND, dwBmSize);
    lpBits = ( LPBYTE )GlobalLock(hBits);

    GetDIBits(hDC, hBm, 0, Bm.bmHeight, lpBits, &bi, DIB_RGB_COLORS);

    WritePng(szFName, lpBits, Bm.bmWidth, Bm.bmHeight);

    GlobalUnlock(hBits);
    GlobalFree(hBits);

    return true;
  }

  // �ۑ����擾
  bool GetSaveName(HWND hWnd, char* szFName, char* szTitle, DWORD dwSize)
  {
    OPENFILENAME ofn;

    lstrcpy(szFName, "");

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize    = sizeof(OPENFILENAME);
    ofn.hwndOwner      = hWnd;
    ofn.lpstrFilter    = "BMP�t�@�C��(*.bmp)\0*.bmp\0PNG�t�@�C��(*.png)\0*.png\0\0";
    ofn.lpstrFile      = szFName;
    ofn.lpstrFileTitle = szTitle;
    ofn.nFilterIndex   = 2;
    ofn.nMaxFile       = dwSize;
    ofn.nMaxFileTitle  = MAX_PATH;
    ofn.Flags          = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    ofn.lpstrDefExt    = "png";
    ofn.lpstrTitle     = "�摜��ۑ�";

    if (!GetSaveFileName(&ofn))
      return false;

    return true;
  }
}

// �^�u�R���g���[���̍쐬
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

// �^�u�̒ǉ�
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

// �^�u���̕ύX
int SetTabText(HWND hTab, int nSel, char* lpText)
{
  TCITEM ti;

  ZeroMemory(&ti, sizeof(TCITEM));
  ti.mask    = TCIF_TEXT;
  ti.pszText = lpText;
  TabCtrl_SetItem(hTab, nSel, &ti);

  return 1;
}

// �^�u���̎擾
int GetTabText(HWND hTab, int nSel, char* lpText)
{
  TCITEM ti;

  TabCtrl_GetItem(hTab, nSel, &ti);
  lstrcpy(lpText, ti.pszText);

  return 1;
}

// DCSET�̍쐬
DCSET CreateDCSet(int w, int h)
{
  DCSET DCSet;

  InitSurface(nullptr, DCSet.hDC, DCSet.hBm, w, h);
  DCSet.bSave = FALSE;

  return DCSet;
}

// �J�����g�f�B���N�g���̏C��
BOOL CngCurDir(void)
{
  TCHAR sztName[MAX_PATH];
  char* lpDir;

  if (!GetModuleFileName(nullptr, sztName, MAX_PATH))
    return FALSE;

  if (!(lpDir = strrchr(sztName, '\\')))
    return FALSE;

  lstrcpy(lpDir, "");

  if (!SetCurrentDirectory(sztName))
    return FALSE;

  return TRUE;
}

// �ݒ�ǂݍ���
BOOL ReadMyProfile(void)
{
  char szBuf[16];

  GetPrivateProfileString("KEY", "CAPTURE", "5", szBuf, 16, "./capture.ini");
  nCapKey = atoi(szBuf);
  GetPrivateProfileString("KEY", "CAPSHIFT", "0", szBuf, 16, "./capture.ini");
  bCapShift = ( BOOL )atoi(szBuf);
  GetPrivateProfileString("KEY", "CAPCTRL", "0", szBuf, 16, "./capture.ini");
  bCapCtrl = ( BOOL )atoi(szBuf);
  GetPrivateProfileString("KEY", "CAPALT", "0", szBuf, 16, "./capture.ini");
  bCapAlt = ( BOOL )atoi(szBuf);

  return TRUE;
}

// �ݒ菑������
BOOL WriteMyProfile(void)
{
  char szBuf[16];

  wsprintf(szBuf, "%d", nCapKey);
  WritePrivateProfileString("KEY", "CAPTURE", szBuf, "./capture.ini");
  wsprintf(szBuf, "%d", bCapCtrl);
  WritePrivateProfileString("KEY", "CAPCTRL", szBuf, "./capture.ini");
  wsprintf(szBuf, "%d", bCapShift);
  WritePrivateProfileString("KEY", "CAPSHIFT", szBuf, "./capture.ini");
  wsprintf(szBuf, "%d", bCapAlt);
  WritePrivateProfileString("KEY", "CAPALT", szBuf, "./capture.ini");

  return TRUE;
}
