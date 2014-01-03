// application.hpp

#pragma once

#include <boost/utility/string_ref.hpp>

#include <Windows.h>

#include "window_class_spec.hpp"


namespace util
{
  namespace windows
  {
    class application
    {
    public:
      explicit application(HINSTANCE instance_handle);

      application(application const&) = delete;
      application& operator=(application const&) = delete;

      HINSTANCE instance_handle() const { return instance_handle_; }

      HICON load_icon_resource(WORD resourceId) const;

      window_class_spec create_window_class_spec(
          boost::string_ref const& name,
          window_procedure_t const& procedure) const;

    private:
      HINSTANCE instance_handle_;
    };
  }
}

inline util::windows::application::application(HINSTANCE const instance_handle)
  : instance_handle_{instance_handle}
{
}

inline HICON util::windows::application::load_icon_resource(WORD const resourceId) const
{
  return static_cast<HICON>(LoadImage(instance_handle_,
        MAKEINTRESOURCE(resourceId),
        IMAGE_ICON,
        0, 0,
        LR_DEFAULTSIZE | LR_SHARED));
}

inline auto util::windows::application::create_window_class_spec(
    boost::string_ref const& name,
    window_procedure_t const& procedure) const
  -> window_class_spec
{
  return window_class_spec{instance_handle_, name, procedure};
}
