// win_util.hpp

#pragma once

#include <windows.h>

#include <boost/container/pmr/string.hpp>
#include <boost/container/pmr/vector.hpp>
#include <boost/expected/expected.hpp>
#include <boost/utility/string_view.hpp>


namespace win {
    inline auto get_client_rect(::HWND const hWnd) {
        ::RECT rc;
        ::GetClientRect(hWnd, &rc);
        return rc;
    }

    constexpr auto width(::RECT const& rc) { return rc.right - rc.left; }
    constexpr auto height(::RECT const& rc) { return rc.bottom - rc.top; }
    constexpr auto centerX(::RECT const& rc) { return rc.left + width(rc) / 2; }
    constexpr auto centerY(::RECT const& rc) { return rc.top + height(rc) / 2; }

    constexpr auto size(::RECT const& rc) { return ::SIZE{width(rc), height(rc)}; }

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

    inline auto get_cursor_pos() {
        ::POINT pt;
        ::GetCursorPos(&pt);
        return pt;
    }

    inline auto get_module_file_name(::HINSTANCE const hInstance) -> boost::expected<boost::container::pmr::basic_string<::TCHAR>, ::DWORD> {
        boost::container::pmr::vector<::TCHAR> buffer(MAX_PATH, boost::container::default_init);
        for (;;)
        {
            auto const ret = ::GetModuleFileName(hInstance, buffer.data(), buffer.size());
            if (ret == 0) { return boost::make_unexpected(::GetLastError()); }
            if (static_cast<std::size_t>(ret) < buffer.size()) {
                return boost::container::pmr::basic_string<::TCHAR>{buffer.data(), static_cast<std::size_t>(ret)};
            }

            buffer.resize(static_cast<std::size_t>(buffer.size() * 1.5), boost::container::default_init);
        }
    }

    inline auto get_text_extent_point(::HDC const hDC, boost::string_view const str) {
        SIZE size;
        GetTextExtentPoint32(hDC, str.data(), str.size(), &size);
        return size;
    }

    inline auto pat_blt(::HDC const hDC, ::RECT const& rect, ::DWORD const rop) {
        return ::PatBlt(hDC, rect.left, rect.top, width(rect), height(rect), rop);
    }

    inline auto bit_blt(::HDC const hDestDC, ::RECT const& destRect, HDC const hSrcDC, ::POINT const& srcPoint, ::DWORD const rop) {
        return ::BitBlt(hDestDC, destRect.left, destRect.top, width(destRect), height(destRect), hSrcDC, srcPoint.x, srcPoint.y, rop);
    }
}
