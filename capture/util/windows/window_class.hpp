// window_class.hpp

#pragma once

#include <functional>
#include <memory>

#include <Windows.h>


namespace util
{
  namespace windows
  {
    using window_procedure_t = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

    class window_class
    {
    public:
      static auto register_window_class(
          ::WNDCLASSEX wc,
          window_procedure_t const& window_procedure)
        -> std::shared_ptr<window_class>;

    private:
      ATOM atom_;
      HINSTANCE instance_handle_;

      explicit window_class(ATOM atom, HINSTANCE instance_handle);

      static ::LRESULT CALLBACK window_procedure_impl(
          ::HWND window_handle,
          ::UINT message_id,
          ::WPARAM param1,
          ::LPARAM param2);
    };
  }
}
