// window_class.cpp

#include "window_class.hpp"

#include <unordered_map>

namespace ns = ::util::windows;


namespace
{
  std::unordered_map< ::ATOM, ns::window_procedure_t> s_procedure_map;
}


auto ns::window_class::register_window_class(
    ::WNDCLASSEX wc,
    window_procedure_t const& window_procedure)
  -> std::shared_ptr<window_class>
{
  wc.lpfnWndProc = &window_class::window_procedure_impl;
  HINSTANCE const instance_handle = wc.hInstance;
  auto const atom = ::RegisterClassEx(&wc);
  if (atom == 0) { return {}; }

  s_procedure_map.emplace(atom, window_procedure);
  auto const deleter = [](window_class const* const p)
  {
    s_procedure_map.erase(p->atom_);
    ::UnregisterClass(
        reinterpret_cast<LPCSTR>(LOWORD(p->atom_)),
        p->instance_handle_);
  };

  try
  {
    auto const ptr = new window_class{atom, instance_handle};
    return std::shared_ptr<window_class>{ptr, deleter};
  }
  catch (...)
  {
    s_procedure_map.erase(atom);
    ::UnregisterClass(
        reinterpret_cast<LPCSTR>(LOWORD(atom)),
        instance_handle);
    throw;
  }
}

ns::window_class::window_class(
    ATOM const atom,
    HINSTANCE const instance_handle)
  : atom_{atom}, instance_handle_{instance_handle}
{
}

::LRESULT CALLBACK ns::window_class::window_procedure_impl(
    ::HWND const window_handle,
    ::UINT const message_id,
    ::WPARAM const param1,
    ::LPARAM const param2)
{
  auto const atom = static_cast< ::ATOM>(::GetClassLongPtr(window_handle, GCW_ATOM));

  return s_procedure_map[atom](window_handle, message_id, param1, param2);
}
