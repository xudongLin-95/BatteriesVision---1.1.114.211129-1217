#ifndef _CSTRING_TOOL_EX_
#define _CSTRING_TOOL_EX_
 
#include <cstringt.h>
 
//
// CString转CStringA
//
#ifndef CStrT2CStrA
#ifdef _UNICODE
#define CStrT2CStrA(cstr) CStrW2CStrA((cstr))
#else
#define CStrT2CStrA(cstr) (cstr)
#endif
#endif
 
//
// CString转CStringW
//
#ifndef CStrT2CStrW
#ifdef _UNICODE
#define CStrT2CStrW(cstr) (cstr)
#else
#define CStrT2CStrW(cstr) CStrA2CStrW((cstr))
#endif
#endif
 
//
// CStringA转CString
//
#ifndef CStrA2CStrT
#ifdef _UNICODE
#define CStrA2CStrT(cstr) CStrA2CStrW((cstr))
#else
#define CStrA2CStrT(cstr) (cstr)
#endif
#endif
 
//
// CStringW转CString
//
#ifndef CStrW2CStrT
#ifdef _UNICODE
#define CStrW2CStrT(cstr) (cstr)
#else
#define CStrW2CStrT(cstr) CStrW2CStrA((cstr))
#endif
#endif
 
//
// CStringA转CStringW
//
CStringW CStrA2CStrW(const CStringA &cstrSrcA);
 
//
// CStringW转CStringA
//
CStringA CStrW2CStrA(const CStringW &cstrSrcW);

std::wstring AsciiToUnicode(const std::string& str) ;
std::string UnicodeToAscii(const std::wstring& wstr) ;
std::wstring Utf8ToUnicode(const std::string& str) ;
std::string UnicodeToUtf8(const std::wstring& wstr) ;
std::string AsciiToUtf8(const std::string& str) ;
std::string Utf8ToAscii(const std::string& str) ;

 
#endif
 