// window_class_spec.hpp

#pragma once

#include <cstring>
#include <memory>

#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>

#include <Windows.h>

#include "stock_object.hpp"
#include "window_class.hpp"


namespace util
{
  namespace windows
  {
    class window_class_spec
    {
    public:
      explicit window_class_spec(
          HINSTANCE instance_handle,
          boost::string_ref class_name,
          WNDPROC procedure);

      void set_background(stock_brush const& brush) { wc_.hbrBackground = brush.get(); }
      void set_cursor(HCURSOR const cursor) { wc_.hCursor = cursor; }
      void set_icon(HICON const icon) { wc_.hIcon = icon; }
      void set_small_icon(HICON const icon) { wc_.hIconSm = icon; }
      void set_menu(WORD const menu_id) { wc_.lpszMenuName = MAKEINTRESOURCE(menu_id); }
      void set_style(UINT const style) { wc_.style = style; }

      boost::optional<window_class> register_class() const;

    private:
      ::WNDCLASSEX wc_;
    };
  }
}


inline util::windows::window_class_spec::window_class_spec(
    HINSTANCE const instance_handle,
    boost::string_ref const class_name,
    WNDPROC const procedure)
{
  std::memset(&wc_, 0, sizeof(wc_));
  wc_.cbSize = sizeof(wc_);
  wc_.hInstance = instance_handle;
  wc_.lpszClassName = class_name.data();
  wc_.lpfnWndProc = procedure;
}

inline auto util::windows::window_class_spec::register_class() const
  -> boost::optional<window_class>
{
  HINSTANCE const instance_handle = wc_.hInstance;
  auto const atom = ::RegisterClassEx(&wc_);
  if (atom == 0) { return {}; }

  std::shared_ptr<void> const releaser{nullptr, [atom, instance_handle](void*) {
    ::UnregisterClass(reinterpret_cast<LPCSTR>(LOWORD(atom)), instance_handle);
  }};

  return boost::make_optional(window_class{atom, instance_handle, releaser});
}
