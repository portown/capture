// main.cpp

#define MAIN_DECLARE

// 結合
#include "common.h"

// グローバル変数の宣言
int Keys[] =					// キー一覧
{ VK_ESCAPE, VK_SPACE, VK_NEXT, VK_PRIOR, VK_END, VK_HOME,
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
  'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10,
  VK_F11, VK_F12
};

// 関数の宣言
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
bool InitApp( HINSTANCE, char * );
bool InitInstance( HINSTANCE, char *, int );
int Run( void );


// ==============================================
// 実装
// ==============================================

// エントリポイント
int WINAPI WinMain( HINSTANCE hCurInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd )
{
	char szClsNm[] = "CAPTURE";

	CreateMutex( NULL, FALSE, szClsNm );
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
		return 0;

	CngCurDir();

	if ( !InitApp( hCurInst, szClsNm ) )
		return 0;

	if ( !InitInstance( hCurInst, szClsNm, nCmd ) )
		return 0;

	return Run();
}

// ウィンドウクラスの登録
bool InitApp( HINSTANCE hInst, char *lpCls )
{
	WNDCLASSEX wc;

	wc.cbClsExtra		= 0;
	wc.cbSize			= sizeof( WNDCLASSEX );
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= ( HBRUSH )GetStockObject( WHITE_BRUSH );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hIcon			= LoadIcon( hInst, MAKEINTRESOURCE( IDI_CAPTURE ) );
	wc.hIconSm			= LoadIcon( hInst, MAKEINTRESOURCE( IDI_CAPTURE ) );
	wc.hInstance		= hInst;
	wc.lpfnWndProc		= ( WNDPROC )WndProc;
	wc.lpszClassName	= lpCls;
	wc.lpszMenuName		= MAKEINTRESOURCE( IDR_MAIN );
	wc.style			= CS_HREDRAW | CS_VREDRAW;

	return ( RegisterClassEx( &wc ) != 0 );
}

// ウィンドウの作成
bool InitInstance( HINSTANCE hInst, char *lpCls, int nCmd )
{
	HWND hWnd;

	hWnd = CreateWindowEx( 0,
		lpCls,
		"Capture",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInst,
		NULL );

	if ( !hWnd )
		return false;

	ShowWindow( hWnd, nCmd );
	UpdateWindow( hWnd );

	return true;
}

// メッセージ・ループ
int Run( void )
{
	MSG msg;

	while ( GetMessage( &msg, NULL, 0, 0L ) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return ( int )msg.wParam;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
	static HINSTANCE hInst;
	static HBITMAP hBSEnt;
	static HWND hTab;
	static RECT rcArea;
	static HDC hEntire, hSubEnt;
	static vector<DCSET> hCap;
	static char szSize[32];
	static bool bDrop;
	static int nMax;
	PAINTSTRUCT ps;
	BITMAP Bm;
	POINT pt;
	HMENU hMenu, hSub;
	RECT rc;
	HDC hdc;
	vector<DCSET>::iterator p;
	int nSel;

	switch ( msg )
	{
		case WM_CREATE:
			hInst = ( ( LPCREATESTRUCT )lp )->hInstance;
			ReadMyProfile();
			hEntire = GetDC( NULL );
			hTab = CreateMyTab( hWnd );
			EnableMenuItem( GetMenu( hWnd ), IDM_SAVE, MF_BYCOMMAND | MF_GRAYED );
			nMax = 0;
			break;

		case WM_USER:
			switch ( wp )
			{
				case WM_LBUTTONDOWN:
				case WM_NCLBUTTONDOWN:
					if ( !bDrop )
					{
						bDrop = true;
						GetCursorPos( &pt );
						SetRect( &rcArea, pt.x, pt.y, pt.x, pt.y );
						GetClientRect( GetDesktopWindow(), &rc );
						InitSurface( NULL, hSubEnt, hBSEnt, rc.right, rc.bottom );
						BitBlt( hSubEnt, 0, 0, rc.right, rc.bottom, hEntire, 0, 0, SRCCOPY );
						lstrcpy( szSize, "" );
					}
					break;

				case WM_MOUSEMOVE:
				case WM_NCMOUSEMOVE:
					if ( bDrop )
					{
						if ( lstrlen( szSize ) )
						{
							rc = rcArea;
							SortRect( &rc );
							PutStrXor( hEntire,
								( rc.right - rc.left ) / 2 + rc.left,
								rc.top - 20, szSize );
							DrawBox( hEntire, rcArea );
						}

						GetCursorPos( &pt );
						rcArea.right = pt.x;
						rcArea.bottom = pt.y;
						wsprintf( szSize, "%d * %d", rcArea.right - rcArea.left,
							rcArea.bottom - rcArea.top );
						DrawBox( hEntire, rcArea );
						rc = rcArea;
						SortRect( &rc );
						PutStrXor( hEntire,
							( rc.right - rc.left ) / 2 + rc.left,
							rc.top - 20, szSize );
					}
					break;

				case WM_LBUTTONUP:
				case WM_NCLBUTTONUP:
					if ( bDrop )
					{
						bDrop = false;

						GetCursorPos( &pt );
						rcArea.right = pt.x;
						rcArea.bottom = pt.y;
						ReleaseDC( hWnd, hSubEnt );

						SortRect( &rcArea );
						GetClientRect( GetDesktopWindow(), &rc );
						BitBlt( hEntire, 0, 0, rc.right, rc.bottom, hSubEnt, 0, 0, SRCCOPY );
						hCap.push_back( CreateDCSet( rcArea.right - rcArea.left, rcArea.bottom - rcArea.top ) );
						BitBlt( hCap[nMax].hDC, 0, 0, rcArea.right - rcArea.left,
							rcArea.bottom - rcArea.top, hSubEnt,
							rcArea.left, rcArea.top, SRCCOPY );
						AddTab( hTab, nMax );
						++nMax;
						if ( IsIconic( hWnd ) )
							OpenIcon( hWnd );
						if ( GetForegroundWindow() != hWnd )
							SetForegroundWindow( hWnd );
						InvalidateRect( hWnd, NULL, FALSE );
						ResetMouseHook();
						if ( nMax == 1 )
							EnableMenuItem( GetMenu( hWnd ), IDM_SAVE, MF_BYCOMMAND | MF_ENABLED );
					}
					break;

				case WM_RBUTTONDOWN:
				case WM_NCRBUTTONDOWN:
					if ( bDrop )
					{
						bDrop = false;

						GetClientRect( GetDesktopWindow(), &rc );
						BitBlt( hEntire, 0, 0, rc.right, rc.bottom, hSubEnt, 0, 0, SRCCOPY );
					}
					break;
			}
			break;

		case WM_NOTIFY:
			switch ( ( ( NMHDR * )lp )->code )
			{
				case TCN_SELCHANGE:
					InvalidateRect( hWnd, NULL, FALSE );
					break;

				case NM_RCLICK:
					hMenu = LoadMenu( hInst, MAKEINTRESOURCE( IDR_TAB ) );
					hSub = GetSubMenu( hMenu, 0 );
					GetCursorPos( &pt );
					TrackPopupMenu( hSub, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL );
					DestroyMenu( hMenu );
					break;
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps );
			GetClientRect( hWnd, &rc );
			PatBlt( hdc, 0, 0, rc.right, rc.bottom, WHITENESS );
			nSel = TabCtrl_GetCurSel( hTab );
			if ( nSel >= 0 )
			{
				TabCtrl_AdjustRect( hTab, FALSE, &rc );
				GetObject( hCap[nSel].hBm, sizeof( BITMAP ), &Bm );
				BitBlt( hdc, 0, rc.top, Bm.bmWidth, Bm.bmHeight, hCap[nSel].hDC, 0, 0, SRCCOPY );
			}
			EndPaint( hWnd, &ps );
			break;

		case WM_SIZE:
			GetClientRect( hWnd, &rc );
			TabCtrl_AdjustRect( hTab, FALSE, &rc );
			MoveWindow( hTab, 0, 0, LOWORD( lp ), rc.top, TRUE );
			break;

		case WM_KEYDOWN:
			if ( wp == Keys[nCapKey] )
			{
				if ( bCapCtrl && GetKeyState( VK_CONTROL ) >= 0 ) break;
				if ( bCapShift && GetKeyState( VK_SHIFT ) >= 0 ) break;
				if ( bCapAlt && GetKeyState( VK_MENU ) >= 0 ) break;

				if ( !IsMouseHooking() )
				{
					SetMouseHook( hWnd, SMH_EXCLUDE );
					CloseWindow( hWnd );
				}
				break;
			}
			break;

		case WM_COMMAND:
			switch ( LOWORD( wp ) )
			{
				case IDM_SAVE:
					nSel = TabCtrl_GetCurSel( hTab );
					if ( SavePicture( hWnd, hTab, nSel, hCap[nSel].hDC, hCap[nSel].hBm ) )
						hCap[nSel].bSave = TRUE;
					break;

				case IDM_EXIT:
					SendMessage( hWnd, WM_CLOSE, 0, 0 );
					break;

				case IDM_OPTION:
					ShowOption( hWnd );
					break;

				case IDM_ABOUT:
					ShowAbout( hWnd );
					break;

				case IDM_CLOSE:
					nSel = TabCtrl_GetCurSel( hTab );
					if ( !hCap[nSel].bSave )
					{
						nSel = Mes( "画像が保存されていません。\n"
							"保存しますか？", "確認", MB_YESNOCANCEL | MB_ICONQUESTION );
						if ( nSel == IDCANCEL ) break;
						if ( nSel == IDYES )
						{
							nSel = TabCtrl_GetCurSel( hTab );
							SavePicture( hWnd, hTab, nSel, hCap[nSel].hDC, hCap[nSel].hBm );
						}
					}
					nSel = TabCtrl_GetCurSel( hTab );
					TabCtrl_DeleteItem( hTab, nSel );
					p = hCap.begin();
					p += nSel;
					hCap.erase( p );
					if ( TabCtrl_GetItemCount( hTab ) > 1 )
					{
						++nSel;
						while ( TabCtrl_SetCurSel( hTab, nSel ) == -1 )
							--nSel;
					}
					else if ( TabCtrl_GetItemCount( hTab ) == 1 )
					{
						TabCtrl_SetCurSel( hTab, 0 );
					}
					--nMax;
					GetClientRect( hWnd, &rc );
					SendMessage( hWnd, WM_SIZE, 0, MAKELPARAM( rc.right, rc.bottom ) );
					InvalidateRect( hWnd, NULL, FALSE );
					if ( nMax == 0 )
						EnableMenuItem( GetMenu( hWnd ), IDM_SAVE, MF_BYCOMMAND | MF_GRAYED );
					break;

				default:
					return DefWindowProc( hWnd, msg, wp, lp );
			}
			break;

		case WM_DESTROY:
			if ( IsMouseHooking() )
				ResetMouseHook();
			DestroyWindow( hTab );
			WriteMyProfile();
			PostQuitMessage( 0 );
			break;

		default:
			return DefWindowProc( hWnd, msg, wp, lp );
	}

	return 0;
}


// EOF