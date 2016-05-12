// funcs.h


#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include <utility>

#include "string_view.hpp"


// ==============================================
// 関数の宣言
// ==============================================

int Mes( LPCTSTR = NULL, LPCTSTR = NULL, UINT = MB_OK );
int DrawBox( HDC, RECT );
void PutStrXor(HDC, int, int, std::string_view);
void normalize(RECT& rc);
RECT normalized(RECT const& rc);
std::tuple<::HDC, ::HBITMAP> InitSurface(::HWND, ::SIZE const&);
int SavePicture( char const*, HDC, HBITMAP );
HWND CreateMyTab(HWND hWnd);
int AddTab( HWND, int );
bool SetTabText(HWND, int, std::string_view);
DCSET CreateDCSet();

int ShowOption( HWND );
int ShowAbout( HWND );


#endif


// EOF
