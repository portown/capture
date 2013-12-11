// main.cpp

// 結合
#include <windows.h>
#include "capdll.h"

// 変数の宣言
#pragma data_seg( "CAP_DATA" )
HHOOK hMouseHook = 0;
HWND hMse = 0;
UINT uType = 0;
#pragma data_seg()
HINSTANCE hInst;


// ==============================================
// 実装
// ==============================================

// エントリポイント
BOOL WINAPI DllMain( HINSTANCE hCurInst, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
			hInst = hCurInst;
			break;
	}

	return TRUE;
}

// マウスフック開始
int SetMouseHook( HWND hDst, UINT uTp )
{
	if ( uTp > SMH_MAX )
		return 0;

	hMouseHook = SetWindowsHookEx( WH_MOUSE, MouseHookProc, hInst, 0 );
	if ( !hMouseHook )
		return 0;

	hMse = hDst;
	uType = uTp;

	return 1;
}

// マウスフック検査
bool IsMouseHooking( void )
{
	return ( hMouseHook != NULL );
}

// マウスフック削除
int ResetMouseHook( void )
{
	if ( !UnhookWindowsHookEx( hMouseHook ) )
		return 0;

	hMouseHook = NULL;

	return 1;
}

// マウスフックプロシージャ
LRESULT CALLBACK MouseHookProc( int nCode, WPARAM wp, LPARAM lp )
{
	LPMOUSEHOOKSTRUCT lpmh;
	POINT pt;

	if ( nCode < 0 )
		return CallNextHookEx( hMouseHook, nCode, wp, lp );

	lpmh = ( LPMOUSEHOOKSTRUCT )lp;
	pt = lpmh->pt;
	SendMessage( hMse, WM_USER, wp, ( LPARAM )&pt );

	switch ( uType )
	{
		case SMH_SHARE:
			return CallNextHookEx( hMouseHook, nCode, wp, lp );

		case SMH_EXCLUDE:
			return TRUE;

		default:
			return CallNextHookEx( hMouseHook, nCode, wp, lp );
	}
}


// EOF