// main_view.hpp

#pragma once

#include <string>
#include <vector>

#include <windows.h>

#include "../defines.h"


namespace capture
{
    namespace view
    {
        class main_view
        {
        public:
            explicit main_view() = default;

            main_view(main_view const&) = delete;

            auto on_create(
                    ::HWND const& window_handle,
                    ::CREATESTRUCT const& create_struct) -> ::LRESULT;
            auto on_event(
                    ::UINT message,
                    ::WPARAM param1,
                    ::LPARAM param2) -> ::LRESULT;

        private:
            auto on_paint() -> void;
            auto on_size(::WORD new_width) -> void;
            auto on_key_down(int key_code) -> void;
            auto on_destroy() -> void;

            auto on_tab_right_clicked() -> void;

            auto on_menu_save() -> void;
            auto on_menu_close() -> void;

        private:
            ::HWND             window_handle_;
            HINSTANCE          hInst;
            HBITMAP            hBSEnt;
            HWND               hTab;
            RECT               rcArea;
            HDC                hEntire, hSubEnt;
            std::vector<DCSET> hCap;
            std::string        szSize;
            bool               bDrop;
            int                nMax;
        };
    }
}
