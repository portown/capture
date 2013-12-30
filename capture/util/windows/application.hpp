// application.hpp

#pragma once

#include <string>

#include <boost/utility/string_ref.hpp>

#include <Windows.h>


namespace util
{
  namespace windows
  {
    class application
    {
    public:
      explicit application(
          HINSTANCE instance_handle,
          boost::string_ref class_name);

      application(application const&) = delete;
      application& operator=(application const&) = delete;

      HINSTANCE instance_handle() const { return instance_handle_; }
      boost::string_ref class_name() const { return class_name_; }

      HICON load_icon_resource(WORD resourceId) const;

    private:
      HINSTANCE instance_handle_;
      std::string class_name_;
    };
  }
}

inline util::windows::application::application(
    HINSTANCE const instance_handle,
    boost::string_ref const class_name)
  : instance_handle_{instance_handle},
    class_name_{class_name}
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
