// main.cpp

#include <windows.h>
#include "capdll.h"

// 変数の宣言
#pragma data_seg( "CAP_DATA" )
HHOOK hMouseHook = 0;
HWND  hMse       = 0;
UINT  uType      = 0;
#pragma data_seg()

namespace
{
    HINSTANCE hInst;
}


// エントリポイント
BOOL WINAPI DllMain(HINSTANCE hCurInst, DWORD dwReason, LPVOID)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            hInst = hCurInst;
            break;
    }

    return TRUE;
}

// マウスフック開始
EXPORT int SetMouseHook(HWND hDst, UINT uTp)
{
    if (uTp > SMH_MAX)
        return 0;

    hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, hInst, 0);
    if (!hMouseHook)
        return 0;

    hMse  = hDst;
    uType = uTp;

    return 1;
}

// マウスフック検査
EXPORT bool IsMouseHooking()
{
    return (hMouseHook != nullptr);
}

// マウスフック削除
EXPORT int ResetMouseHook()
{
    if (!UnhookWindowsHookEx(hMouseHook))
        return 0;

    hMouseHook = nullptr;

    return 1;
}

// マウスフックプロシージャ
EXPORT LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode < 0)
        return CallNextHookEx(hMouseHook, nCode, wp, lp);

    LPMOUSEHOOKSTRUCT lpmh = reinterpret_cast<LPMOUSEHOOKSTRUCT>(lp);
    POINT             pt   = lpmh->pt;
    SendMessage(hMse, WM_USER, wp, reinterpret_cast<LPARAM>(&pt));

    switch (uType)
    {
        case SMH_SHARE:
            return CallNextHookEx(hMouseHook, nCode, wp, lp);

        case SMH_EXCLUDE:
            return TRUE;

        default:
            return CallNextHookEx(hMouseHook, nCode, wp, lp);
    }
}
