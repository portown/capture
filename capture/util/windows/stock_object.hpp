// stock_object.hpp

#pragma once

#include <Windows.h>


namespace util
{
  namespace windows
  {
    namespace stock_object
    {
      namespace detail
      {
        enum class type
        {
          brush,
          pen,
          font,
          palette,
        };

        template <class T>
        struct loader_impl
        {
          using type = T;

          constexpr T load(int const object) const
          {
            return static_cast<T>(::GetStockObject(object));
          }
        };

        template <type T> struct loader;
        template <> struct loader<type::brush>   : loader_impl< ::HBRUSH> {};
        template <> struct loader<type::pen>     : loader_impl< ::HPEN> {};
        template <> struct loader<type::font>    : loader_impl< ::HFONT> {};
        template <> struct loader<type::palette> : loader_impl< ::HPALETTE> {};

        template <type T>
        struct stock_object
        {
          constexpr stock_object(int const object_id) : id_{object_id} {}

          constexpr auto get() const -> typename loader<T>::type
          {
            return loader<T>{}.load(id_);
          }

        private:
          int id_;
        };
      }

      using stock_brush = detail::stock_object<detail::type::brush>;

      static constexpr stock_brush black_brush{BLACK_BRUSH};
      static constexpr stock_brush darkgray_brush{DKGRAY_BRUSH};
      static constexpr stock_brush dc_brush{DC_BRUSH};
      static constexpr stock_brush gray_brush{GRAY_BRUSH};
      static constexpr stock_brush hollow_brush{HOLLOW_BRUSH};
      static constexpr stock_brush lightgray_brush{LTGRAY_BRUSH};
      static constexpr stock_brush null_brush{NULL_BRUSH};
      static constexpr stock_brush white_brush{WHITE_BRUSH};
    }

    using stock_object::stock_brush;
  }
}
