#pragma once
#include <string>

struct HttpResponse {
    int status = 0;         
    std::string body;       
    bool ok = false;        
};

HttpResponse HttpGet(const std::wstring& host, int port,
                     const std::wstring& path);

HttpResponse HttpPost(const std::wstring& host, int port,
                      const std::wstring& path,
                      const std::string& body);

HttpResponse HttpPut(const std::wstring& host, int port,
                     const std::wstring& path,
                     const std::string& body);

HttpResponse HttpDelete(const std::wstring& host, int port,
                        const std::wstring& path);