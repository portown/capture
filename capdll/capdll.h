// capdll.h

#define SMH_MAX SMH_EXCLUDE

#define SMH_SHARE 0x0000
#define SMH_EXCLUDE 0x0001

LRESULT CALLBACK MouseHookProc( int, WPARAM, LPARAM );
int SetMouseHook( HWND, UINT );
bool IsMouseHooking( void );
int ResetMouseHook( void );


// EOF