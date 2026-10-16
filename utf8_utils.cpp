#include "utf8_utils.h"
#include <windows.h>

std::string WStringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string out(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        (int)wstr.size(), &out[0], size, nullptr, nullptr);
    return out;
}

std::wstring Utf8ToWString(const std::string& str) {
    if (str.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
        (int)str.size(), nullptr, 0);
    std::wstring out(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
        (int)str.size(), &out[0], size);
    return out;
}