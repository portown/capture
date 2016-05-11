// capdll.h

#define SMH_MAX SMH_EXCLUDE

#define SMH_SHARE 0x0000
#define SMH_EXCLUDE 0x0001

#ifdef DLL_EXPORT
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C" __declspec(dllimport)
#endif

EXPORT LRESULT CALLBACK MouseHookProc( int, WPARAM, LPARAM );
EXPORT int SetMouseHook( HWND, UINT );
EXPORT bool IsMouseHooking( void );
EXPORT int ResetMouseHook( void );


// EOF
