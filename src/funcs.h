// funcs.h


#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER


// ==============================================
// 関数の宣言
// ==============================================

int Mes( LPCTSTR = NULL, LPCTSTR = NULL, UINT = MB_OK );
int DrawBox( HDC, RECT );
int PutStrXor( HDC, int, int, char * );
int SortRect( LPRECT );
int InitSurface( HWND, HDC &, HBITMAP &, int, int );
int SavePicture( char const*, HDC, HBITMAP );
HWND CreateMyTab( HWND hWnd );
int AddTab( HWND, int );
int SetTabText( HWND, int, char const* );
int GetTabText( HWND, int, char * );
DCSET CreateDCSet();

int ShowOption( HWND );
int ShowAbout( HWND );


#endif


// EOF
