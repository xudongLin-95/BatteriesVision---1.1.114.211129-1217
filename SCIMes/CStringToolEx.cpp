
#include "stdafx.h"
#include "CStringToolEx.h"
#include <cstringt.h>
 
//
// CStringA转CStringW
//
CStringW CStrA2CStrW(const CStringA &cstrSrcA)
{
    int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
    wchar_t *wstr = new wchar_t[len];
    memset(wstr, 0, len*sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
    CStringW cstrDestW = wstr;
    delete[] wstr;
 
    return cstrDestW;
}
 
//
// CStringW转CStringA
//
CStringA CStrW2CStrA(const CStringW &cstrSrcW)
{
    int len = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, NULL, 0, NULL, NULL);
    char *str = new char[len];
    memset(str, 0, len);
    WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, str, len, NULL, NULL);
    CStringA cstrDestA = str;
    delete[] str;
 
    return cstrDestA;
}

std::wstring AsciiToUnicode(const std::string& str) {  
	// 预算-缓冲区中宽字节的长度    
	int unicodeLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);  
	// 给指向缓冲区的指针变量分配内存    
	wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen+1);
	memset(pUnicode,0,unicodeLen+1);
	// 开始向缓冲区转换字节    
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pUnicode, unicodeLen);  
	std::wstring ret_str = pUnicode;  
	free(pUnicode);  
	return ret_str;  
}  
std::string UnicodeToAscii(const std::wstring& wstr) {  
	// 预算-缓冲区中多字节的长度    
	int ansiiLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);  
	// 给指向缓冲区的指针变量分配内存    
	char *pAssii = (char*)malloc(sizeof(char)*ansiiLen+1);
	memset(pAssii,0,ansiiLen+1);
	// 开始向缓冲区转换字节    
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);  
	std::string ret_str = pAssii;  
	free(pAssii);  
	return ret_str;  
}  
std::wstring Utf8ToUnicode(const std::string& str) {  
	// 预算-缓冲区中宽字节的长度    
	int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);  
	// 给指向缓冲区的指针变量分配内存    
	wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen+1);  
	memset(pUnicode,0,unicodeLen +1);
	// 开始向缓冲区转换字节    
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, pUnicode, unicodeLen);  
	std::wstring ret_str = pUnicode;  
	free(pUnicode);  
	return ret_str;  
}  
std::string UnicodeToUtf8(const std::wstring& wstr) {  
	// 预算-缓冲区中多字节的长度    
	int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);  
	// 给指向缓冲区的指针变量分配内存    
	char *pAssii = (char*)malloc(sizeof(char)*ansiiLen + 1);  

	memset(pAssii,0,ansiiLen+1);
	// 开始向缓冲区转换字节    
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);  
	std::string ret_str = pAssii;  
	free(pAssii);  
	return ret_str;  
}  
std::string AsciiToUtf8(const std::string& str) {  
	return UnicodeToUtf8(AsciiToUnicode(str));  
}  
std::string Utf8ToAscii(const std::string& str) {  
	return UnicodeToAscii(Utf8ToUnicode(str));  
}  
