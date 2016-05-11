// win_util.hpp

#pragma once

#include <windows.h>


namespace win {
    inline auto get_client_rect(::HWND const hWnd) {
        ::RECT rc;
        ::GetClientRect(hWnd, &rc);
        return rc;
    }

    constexpr auto width(::RECT const& rc) { return rc.right - rc.left; }
    constexpr auto height(::RECT const& rc) { return rc.bottom - rc.top; }

    namespace detail {
        template <int code, class ResultType>
        struct GDI_stock_object_type {};
    }

    namespace {
        constexpr auto white_brush = detail::GDI_stock_object_type<WHITE_BRUSH, ::HBRUSH>{};
    }

    template <int code, class ResultType>
    inline ResultType get_stock_object(detail::GDI_stock_object_type<code, ResultType>) {
        return reinterpret_cast<ResultType>(::GetStockObject(code));
    }
}
