#pragma once
#include <string>

std::string WStringToUtf8(const std::wstring& wstr);
std::wstring Utf8ToWString(const std::string& str);