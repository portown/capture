// main.cpp

#include <memory>

#include <Windows.h>

#include "view/main_view.hpp"

#include "resource.h"


namespace
{
    namespace view = capture::view;

    // 関数の宣言
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    bool             InitApp(HINSTANCE, char const*);
    bool             InitInstance(HINSTANCE, char const*, int, view::main_view&);
    int Run();
}


// エントリポイント
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE, LPSTR, int nCmd)
{
    CreateMutex(nullptr, FALSE, "jp.portown.capture");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        return 0;

    constexpr auto main_class_name = "main";
    if (!InitApp(hCurInst, main_class_name))
        return 0;

    auto const main_view = std::make_shared<view::main_view>();
    if (!InitInstance(hCurInst, main_class_name, nCmd, *main_view))
        return 0;

    return Run();
}

namespace
{
    // ウィンドウクラスの登録
    bool InitApp(HINSTANCE hInst, char const* lpCls)
    {
        WNDCLASSEX wc;

        wc.cbClsExtra    = 0;
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.cbWndExtra    = sizeof(void*);
        wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wc.hCursor       = static_cast<HCURSOR>(LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
        wc.hIcon         = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_CAPTURE), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
        wc.hIconSm       = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_CAPTURE), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
        wc.hInstance     = hInst;
        wc.lpfnWndProc   = WndProc;
        wc.lpszClassName = lpCls;
        wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAIN);
        wc.style         = CS_HREDRAW | CS_VREDRAW;

        return (RegisterClassEx(&wc) != 0);
    }

    // ウィンドウの作成
    bool InitInstance(HINSTANCE hInst, char const* lpCls, int nCmd, view::main_view& view)
    {
        HWND hWnd = CreateWindowEx(0,
                lpCls,
                "Capture",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                nullptr,
                nullptr,
                hInst,
                &view);

        if (!hWnd)
            return false;

        ShowWindow(hWnd, nCmd);
        UpdateWindow(hWnd);

        return true;
    }

    // メッセージ・ループ
    int Run()
    {
        MSG msg;

        while (GetMessage(&msg, nullptr, 0, 0L) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

    // ウィンドウプロシージャ
    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        if (msg == WM_CREATE)
        {
            auto const args = *reinterpret_cast<LPCREATESTRUCT>(lp);
            auto& view = *reinterpret_cast<view::main_view*>(args.lpCreateParams);
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast< ::LONG_PTR>(&view));
            return view.on_create(hWnd, args);
        }

        auto const view = reinterpret_cast<view::main_view*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (!view) return ::DefWindowProc(hWnd, msg, wp, lp);

        return view->on_event(msg, wp, lp);
    }
}
