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

    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInst, 0);
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

    PostMessage(hMse, WM_USER, wp, 0);

    if (wp == WM_LBUTTONDOWN
            || wp == WM_NCLBUTTONDOWN
            || wp == WM_LBUTTONUP
            || wp == WM_NCLBUTTONUP
            || wp == WM_RBUTTONDOWN
            || wp == WM_NCRBUTTONDOWN
            || wp == WM_RBUTTONUP
            || wp == WM_NCRBUTTONUP) {
        return TRUE;
    }

    return CallNextHookEx(hMouseHook, nCode, wp, lp);
}
