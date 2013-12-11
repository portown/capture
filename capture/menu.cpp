// menu.cpp


// 結合
#include "common.h"

// 定数の定義
#define LISTMAX 54

// 関数の宣言
INT_PTR OptionProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR AboutProc( HWND, UINT, WPARAM, LPARAM );
int SetCombo( HWND );


// ==============================================
// 実装
// ==============================================

// オプションダイアログ表示
int ShowOption( HWND hWnd )
{
	HINSTANCE hInst;
	INT_PTR iRet;

	hInst = ( HINSTANCE )GetWindowLongPtr( hWnd, GWLP_HINSTANCE );
	iRet = DialogBoxParam( hInst, "IDD_OPTION", hWnd, ( DLGPROC )OptionProc, NULL );

	return 1;
}

// オプションダイアログプロシージャ
INT_PTR OptionProc( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			SetCombo( hDlg );
			Button_SetCheck( GetDlgItem( hDlg, IDC_CAPCTRL ), bCapCtrl );
			Button_SetCheck( GetDlgItem( hDlg, IDC_CAPSHIFT ), bCapShift );
			Button_SetCheck( GetDlgItem( hDlg, IDC_CAPALT ), bCapAlt );
			break;

		case WM_COMMAND:
			switch ( wp )
			{
				case IDOK:
					nCapKey = ComboBox_GetCurSel( GetDlgItem( hDlg, IDC_CAPKEY ) );
					bCapCtrl = Button_GetCheck( GetDlgItem( hDlg, IDC_CAPCTRL ) );
					bCapShift = Button_GetCheck( GetDlgItem( hDlg, IDC_CAPSHIFT ) );
					bCapAlt = Button_GetCheck( GetDlgItem( hDlg, IDC_CAPALT ) );
					EndDialog( hDlg, IDOK );
					break;

				case IDCANCEL:
					EndDialog( hDlg, IDCANCEL );
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
int SetCombo( HWND hDlg )
{
	LPCTSTR szList[LISTMAX] = { "ESCAPE", "SPACE", "PAGEDOWN", "PAGEUP", "END", "HOME",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F",
		"G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
		"W", "X", "Y", "Z", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
		"F11", "F12" };
	HWND hCap;
	int i;

	hCap = GetDlgItem( hDlg, IDC_CAPKEY );

	for ( i = 0; i < LISTMAX; i++ )
		ComboBox_InsertString( hCap, i, szList[i] );

	ComboBox_SetCurSel( hCap, nCapKey );

	return 1;
}

// バージョン情報ダイアログ表示
int ShowAbout( HWND hWnd )
{
	HINSTANCE hInst;

	hInst = ( HINSTANCE )GetWindowLongPtr( hWnd, GWLP_HINSTANCE );
	DialogBox( hInst, "IDD_ABOUT", hWnd, ( DLGPROC )OptionProc );

	return 1;
}

// バージョン情報ダイアログプロシージャ
INT_PTR AboutProc( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			break;

		case WM_COMMAND:
			switch ( wp )
			{
				case IDOK:
					EndDialog( hDlg, IDOK );
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


// EOF