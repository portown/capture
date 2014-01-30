// main_view.cpp

#include "main_view.hpp"

#include <stdexcept>
#include <string>

#include <boost/exception/all.hpp>

#include <commctrl.h>

#include <capdll.h>

#include "../resource.h"
#include "../funcs.h"


int nCapKey;
BOOL bCapCtrl;
BOOL bCapShift;
BOOL bCapAlt;


namespace
{
  namespace ns = capture::view;

  constexpr int Keys[] =          // キー一覧
  { VK_ESCAPE, VK_SPACE, VK_NEXT, VK_PRIOR, VK_END, VK_HOME,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10,
    VK_F11, VK_F12
  };

  std::string load_string_from_resource(HINSTANCE instance_handle,
      UINT resource_id);
}


auto ns::main_view::on_create(
    ::HWND const& window_handle,
    ::CREATESTRUCT const& create_struct) -> ::LRESULT
{
  window_handle_ = window_handle;
  hInst = create_struct.hInstance;
  ReadMyProfile();
  hEntire = GetDC(nullptr);
  hTab    = CreateMyTab(window_handle);
  EnableMenuItem(GetMenu(window_handle), IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
  nMax = 0;

  return 0;
}

auto ns::main_view::on_event(
    ::UINT message,
    ::WPARAM param1,
    ::LPARAM param2) -> ::LRESULT
{
  switch (message)
  {
    case WM_USER:
      switch (param1)
      {
        case WM_LBUTTONDOWN:
        case WM_NCLBUTTONDOWN:
          if (!bDrop)
          {
            bDrop = true;
            POINT pt;
            GetCursorPos(&pt);
            SetRect(&rcArea, pt.x, pt.y, pt.x, pt.y);
            RECT rc;
            GetClientRect(GetDesktopWindow(), &rc);
            InitSurface(nullptr, hSubEnt, hBSEnt, rc.right, rc.bottom);
            BitBlt(hSubEnt, 0, 0, rc.right, rc.bottom, hEntire, 0, 0, SRCCOPY);
            lstrcpy(szSize, "");
          }
          break;

        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
          if (bDrop)
          {
            if (lstrlen(szSize))
            {
              RECT rc = rcArea;
              SortRect(&rc);
              PutStrXor(hEntire,
                  (rc.right - rc.left) / 2 + rc.left,
                  rc.top - 20, szSize);
              DrawBox(hEntire, rcArea);
            }

            POINT pt;
            GetCursorPos(&pt);
            rcArea.right  = pt.x;
            rcArea.bottom = pt.y;
            wsprintf(szSize, "%d * %d", rcArea.right - rcArea.left,
                rcArea.bottom - rcArea.top);
            DrawBox(hEntire, rcArea);
            RECT rc = rcArea;
            SortRect(&rc);
            PutStrXor(hEntire,
                (rc.right - rc.left) / 2 + rc.left,
                rc.top - 20, szSize);
          }
          break;

        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP:
          if (bDrop)
          {
            bDrop = false;

            POINT pt;
            GetCursorPos(&pt);
            rcArea.right  = pt.x;
            rcArea.bottom = pt.y;
            ReleaseDC(window_handle_, hSubEnt);

            SortRect(&rcArea);
            RECT rc;
            GetClientRect(GetDesktopWindow(), &rc);
            BitBlt(hEntire, 0, 0, rc.right, rc.bottom, hSubEnt, 0, 0, SRCCOPY);
            hCap.push_back(CreateDCSet(rcArea.right - rcArea.left, rcArea.bottom - rcArea.top));
            BitBlt(hCap[nMax].hDC, 0, 0, rcArea.right - rcArea.left,
                rcArea.bottom - rcArea.top, hSubEnt,
                rcArea.left, rcArea.top, SRCCOPY);
            AddTab(hTab, nMax);
            ++nMax;
            if (IsIconic(window_handle_))
              OpenIcon(window_handle_);
            if (GetForegroundWindow() != window_handle_)
              SetForegroundWindow(window_handle_);
            InvalidateRect(window_handle_, nullptr, FALSE);
            ResetMouseHook();
            if (nMax == 1)
              EnableMenuItem(GetMenu(window_handle_), IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
          }
          break;

        case WM_RBUTTONDOWN:
        case WM_NCRBUTTONDOWN:
          if (bDrop)
          {
            bDrop = false;

            RECT rc;
            GetClientRect(GetDesktopWindow(), &rc);
            BitBlt(hEntire, 0, 0, rc.right, rc.bottom, hSubEnt, 0, 0, SRCCOPY);
          }
          break;
      }
      break;

    case WM_NOTIFY:
      switch (reinterpret_cast<NMHDR*>(param2)->code)
      {
        case TCN_SELCHANGE:
          InvalidateRect(window_handle_, nullptr, FALSE);
          break;

        case NM_RCLICK:
          {
            auto const hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_TAB));
            auto const hSub  = GetSubMenu(hMenu, 0);
            POINT pt;
            GetCursorPos(&pt);
            TrackPopupMenu(hSub, TPM_LEFTALIGN, pt.x, pt.y, 0, window_handle_, nullptr);
            DestroyMenu(hMenu);
            break;
          }
      }
      break;

    case WM_PAINT:
      on_paint();
      break;

    case WM_SIZE:
      on_size(LOWORD(param2));
      break;

    case WM_KEYDOWN:
      on_key_down(param1);
      break;

    case WM_COMMAND:
      switch (LOWORD(param1))
      {
        case IDM_SAVE:
          {
            auto const nSel = TabCtrl_GetCurSel(hTab);
            if (SavePicture(window_handle_, hTab, nSel, hCap[nSel].hDC, hCap[nSel].hBm))
              hCap[nSel].bSave = TRUE;
            break;
          }
        case IDM_EXIT:
          SendMessage(window_handle_, WM_CLOSE, 0, 0);
          break;

        case IDM_OPTION:
          ShowOption(window_handle_);
          break;

        case IDM_ABOUT:
          ShowAbout(window_handle_);
          break;

        case IDM_CLOSE:
          {
            auto const nSel = TabCtrl_GetCurSel(hTab);
            if (!hCap[nSel].bSave)
            {
              auto const ret = Mes(
                  load_string_from_resource(hInst, IDS_MSG_NOT_SAVED).data(),
                  load_string_from_resource(hInst, IDS_MSG_NOT_SAVED_TITLE).data(),
                  MB_YESNOCANCEL | MB_ICONQUESTION);
              if (ret == IDCANCEL) break;
              if (ret == IDYES)
              {
                SavePicture(window_handle_, hTab, nSel, hCap[nSel].hDC, hCap[nSel].hBm);
              }
            }
            TabCtrl_DeleteItem(hTab, nSel);
            hCap.erase(std::next(hCap.begin(), nSel));
            if (TabCtrl_GetItemCount(hTab) > 1)
            {
              for (auto i = nSel + 1; TabCtrl_SetCurSel(hTab, i) == -1; --i);
            }
            else if (TabCtrl_GetItemCount(hTab) == 1)
            {
              TabCtrl_SetCurSel(hTab, 0);
            }
            --nMax;
            RECT rc;
            GetClientRect(window_handle_, &rc);
            SendMessage(window_handle_, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
            InvalidateRect(window_handle_, nullptr, FALSE);
            if (nMax == 0)
              EnableMenuItem(GetMenu(window_handle_), IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
            break;
          }
        default:
          return DefWindowProc(window_handle_, message, param1, param2);
      }
      break;

    case WM_DESTROY:
      if (IsMouseHooking())
        ResetMouseHook();
      DestroyWindow(hTab);
      WriteMyProfile();
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(window_handle_, message, param1, param2);
  }

  return 0;
}

auto ns::main_view::on_paint() -> void
{
  ::PAINTSTRUCT ps;
  ::HDC const hdc = ::BeginPaint(window_handle_, &ps);
  ::RECT rc;
  ::GetClientRect(window_handle_, &rc);
  ::PatBlt(hdc, 0, 0, rc.right, rc.bottom, WHITENESS);
  auto const nSel = TabCtrl_GetCurSel(hTab);
  if (nSel >= 0)
  {
    TabCtrl_AdjustRect(hTab, FALSE, &rc);
    ::BITMAP bm;
    ::GetObject(hCap[nSel].hBm, sizeof(::BITMAP), &bm);
    ::BitBlt(hdc, 0, rc.top, bm.bmWidth, bm.bmHeight, hCap[nSel].hDC, 0, 0, SRCCOPY);
  }
  ::EndPaint(window_handle_, &ps);
}

auto ns::main_view::on_size(::WORD const new_width) -> void
{
  ::RECT rc;
  ::GetClientRect(window_handle_, &rc);
  TabCtrl_AdjustRect(hTab, FALSE, &rc);
  ::MoveWindow(hTab, 0, 0, new_width, rc.top, TRUE);
}

auto ns::main_view::on_key_down(int const key_code) -> void
{
  if (key_code != Keys[nCapKey]) return;

  if (bCapCtrl && ::GetKeyState(VK_CONTROL) >= 0) return;
  if (bCapShift && ::GetKeyState(VK_SHIFT) >= 0) return;
  if (bCapAlt && ::GetKeyState(VK_MENU) >= 0) return;

  if (!::IsMouseHooking())
  {
    ::SetMouseHook(window_handle_, SMH_EXCLUDE);
    ::CloseWindow(window_handle_);
  }
}


namespace
{
  auto load_string_from_resource(
      HINSTANCE const instance_handle,
      UINT const resource_id)
    -> std::string
  {
    std::vector<char> buffer(1024);
    for (;;)
    {
      auto const ret = ::LoadString(instance_handle, resource_id, buffer.data(), buffer.size());
      if (ret == 0) { BOOST_THROW_EXCEPTION(std::logic_error{"resource not found"}); }
      if (static_cast<std::size_t>(ret) < buffer.size()) { break; }

      buffer.resize(static_cast<std::size_t>(buffer.size() * 1.5));
    }

    return std::string{buffer.data()};
  }
}
