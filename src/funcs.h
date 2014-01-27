// funcs.h


#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER


// ==============================================
// �֐��̐錾
// ==============================================

int Mes( LPCTSTR = NULL, LPCTSTR = NULL, UINT = MB_OK );
int DrawBox( HDC, RECT );
int PutStrXor( HDC, int, int, char * );
int SortRect( LPRECT );
int InitSurface( HWND, HDC &, HBITMAP &, int, int );
int SavePicture( HWND, HWND, int, HDC, HBITMAP );
HWND CreateMyTab( HWND hWnd );
int AddTab( HWND, int );
int SetTabText( HWND, int, char * );
int GetTabText( HWND, int, char * );
DCSET CreateDCSet( int, int );
BOOL CngCurDir( void );
BOOL ReadMyProfile( void );
BOOL WriteMyProfile( void );

int ShowOption( HWND );
int ShowAbout( HWND );


#endif


// EOF