// menu.cpp

#include "common.h"


namespace
{
  constexpr auto LISTMAX = 54;

  INT_PTR CALLBACK OptionProc(HWND, UINT, WPARAM, LPARAM);
  INT_PTR CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
  int     SetCombo(HWND);
}


// オプションダイアログ表示
int ShowOption(HWND hWnd)
{
  HINSTANCE hInst = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));

  DialogBoxParam(hInst, "IDD_OPTION", hWnd, OptionProc, 0);

  return 1;
}

// バージョン情報ダイアログ表示
int ShowAbout(HWND hWnd)
{
  HINSTANCE hInst = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
  DialogBox(hInst, "IDD_ABOUT", hWnd, OptionProc);

  return 1;
}


namespace
{
  // オプションダイアログプロシージャ
  INT_PTR CALLBACK OptionProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM)
  {
    switch (msg)
    {
      case WM_INITDIALOG:
        SetCombo(hDlg);
        Button_SetCheck(GetDlgItem(hDlg, IDC_CAPCTRL), bCapCtrl);
        Button_SetCheck(GetDlgItem(hDlg, IDC_CAPSHIFT), bCapShift);
        Button_SetCheck(GetDlgItem(hDlg, IDC_CAPALT), bCapAlt);
        break;

      case WM_COMMAND:
        switch (wp)
        {
          case IDOK:
            nCapKey   = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_CAPKEY));
            bCapCtrl  = Button_GetCheck(GetDlgItem(hDlg, IDC_CAPCTRL));
            bCapShift = Button_GetCheck(GetDlgItem(hDlg, IDC_CAPSHIFT));
            bCapAlt   = Button_GetCheck(GetDlgItem(hDlg, IDC_CAPALT));
            EndDialog(hDlg, IDOK);
            break;

          case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;

          default:
            return FALSE;
        }
        break;

      default:
        return FALSE;
    }

    return TRUE;
  }

  // コンボボックスのセット
  int SetCombo(HWND hDlg)
  {
    LPCTSTR szList[LISTMAX] = { "ESCAPE", "SPACE", "PAGEDOWN", "PAGEUP", "END", "HOME",
      "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F",
      "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
      "W", "X", "Y", "Z", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
      "F11", "F12" };

    HWND hCap = GetDlgItem(hDlg, IDC_CAPKEY);

    for (int i = 0; i < LISTMAX; i++)
      ComboBox_InsertString(hCap, i, szList[i]);

    ComboBox_SetCurSel(hCap, nCapKey);

    return 1;
  }

  // バージョン情報ダイアログプロシージャ
  INT_PTR CALLBACK AboutProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM)
  {
    switch (msg)
    {
      case WM_INITDIALOG:
        break;

      case WM_COMMAND:
        switch (wp)
        {
          case IDOK:
            EndDialog(hDlg, IDOK);
            break;

          default:
            return FALSE;
        }
        break;

      default:
        return FALSE;
    }

    return TRUE;
  }
}
