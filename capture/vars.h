// vars.h

#ifndef VARIABLES_HEADER
#define VARIABLES_HEADER

#ifdef MAIN_DECLARE
#define extern
#else
#define extern extern
#endif


// ==============================================
// グローバル変数の宣言
// ==============================================


extern int nCapKey;
extern BOOL bCapCtrl;
extern BOOL bCapShift;
extern BOOL bCapAlt;


#endif


// EOF