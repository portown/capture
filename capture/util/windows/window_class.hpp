// window_class.hpp

#pragma once

#include <memory>

#include <Windows.h>


namespace util
{
  namespace windows
  {
    class window_class
    {
    public:
      explicit window_class(
          ATOM atom,
          HINSTANCE instance_handle,
          std::shared_ptr<void> releaser);

    private:
      ATOM atom_;
      HINSTANCE instance_handle_;
      std::shared_ptr<void> releaser_;
    };
  }
}

inline util::windows::window_class::window_class(
    ATOM const atom,
    HINSTANCE const instance_handle,
    std::shared_ptr<void> const releaser)
  : atom_{atom}, instance_handle_{instance_handle}, releaser_{releaser}
{
}
