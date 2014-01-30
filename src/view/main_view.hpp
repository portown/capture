// main_view.hpp

#pragma once

#include <vector>

#include <Windows.h>

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
      ::HWND             window_handle_;
      HINSTANCE          hInst;
      HBITMAP            hBSEnt;
      HWND               hTab;
      RECT               rcArea;
      HDC                hEntire, hSubEnt;
      std::vector<DCSET> hCap;
      char               szSize[32];
      bool               bDrop;
      int                nMax;
    };
  }
}
