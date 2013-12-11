// main.cpp

// ����
#include <windows.h>
#include "capdll.h"

// �ϐ��̐錾
#pragma data_seg( "CAP_DATA" )
HHOOK hMouseHook = 0;
HWND hMse = 0;
UINT uType = 0;
#pragma data_seg()
HINSTANCE hInst;


// ==============================================
// ����
// ==============================================

// �G���g���|�C���g
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

// �}�E�X�t�b�N�J�n
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

// �}�E�X�t�b�N����
bool IsMouseHooking( void )
{
	return ( hMouseHook != NULL );
}

// �}�E�X�t�b�N�폜
int ResetMouseHook( void )
{
	if ( !UnhookWindowsHookEx( hMouseHook ) )
		return 0;

	hMouseHook = NULL;

	return 1;
}

// �}�E�X�t�b�N�v���V�[�W��
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