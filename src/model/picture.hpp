// picture.hpp

#pragma once

#include <memory>
#include <utility>

#include <Windows.h>


namespace capture
{
    namespace model
    {
        class picture
        {
        public:
            static auto capture(
                    ::HDC const& context_handle,
                    ::RECT const& capture_area)
                -> std::shared_ptr<picture>
                {
                    auto const w = capture_area.right - capture_area.left;
                    auto const h = capture_area.bottom - capture_area.top;
                    if (w <= 0 || h <= 0) return {};

                    auto const hdc = ::CreateCompatibleDC(context_handle);
                    auto const hbm = ::CreateCompatibleBitmap(context_handle, w, h);
                    ::SelectObject(hdc, hbm);

                    ::BitBlt(hdc, 0, 0, w, h, context_handle,
                            capture_area.left, capture_area.top, SRCCOPY);

                    return std::shared_ptr<picture>{new picture(hdc, hbm, w, h)};
                }

        private:
            explicit picture(
                    ::HDC const& context_handle,
                    ::HBITMAP const& bitmap_handle,
                    unsigned int const width,
                    unsigned int const height)
                : context_handle_{context_handle},
                bitmap_handle_{bitmap_handle},
                width_{width},
                height_{height}
            {
            }

        public:
            ~picture() noexcept
            {
                try
                {
                    ::DeleteObject(bitmap_handle_);
                    ::DeleteDC(context_handle_);
                }
                catch (...) {}
            }

            picture(picture const&) = delete;
            picture(picture&&) = default;

            auto context_handle() const -> ::HDC { return context_handle_; }
            auto bitmap_handle() const -> ::HBITMAP { return bitmap_handle_; }

            auto width() const -> unsigned int { return width_; }
            auto height() const -> unsigned int { return height_; }

        private:
            ::HDC context_handle_;
            ::HBITMAP bitmap_handle_;
            unsigned int width_;
            unsigned int height_;
        };
    }
}
