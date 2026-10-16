#include "http_client.h"
#include <windows.h>
#include <winhttp.h>
#include <string>

#pragma comment(lib, "winhttp.lib")

 
static HINTERNET GetSession() {
    static HINTERNET hSession = nullptr;
    if (!hSession) {
        hSession = WinHttpOpen(L"ZoomBreaker/1.0",
            WINHTTP_ACCESS_TYPE_NO_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (hSession) {
            WinHttpSetTimeouts(hSession, 500, 500, 1000, 1500);
        }
    }
    return hSession;
}

static HttpResponse DoRequest(const std::wstring& method,
                              const std::wstring& host, int port,
                              const std::wstring& path,
                              const std::string& body) {
    HttpResponse res;

    HINTERNET hSession = GetSession();
    if (!hSession) return res;

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
                                        (INTERNET_PORT)port, 0);
    if (!hConnect) return res;

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, method.c_str(),
        path.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        return res;
    }

    std::wstring headers = L"Content-Type: application/json\r\n";

    BOOL sent = WinHttpSendRequest(hRequest, headers.c_str(), -1,
        body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(),
        (DWORD)body.size(), (DWORD)body.size(), 0);

    if (!sent) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        return res;
    }

    if (!WinHttpReceiveResponse(hRequest, nullptr)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        return res;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize,
        WINHTTP_NO_HEADER_INDEX);
    res.status = (int)statusCode;

    DWORD avail = 0;
    do {
        avail = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &avail)) break;
        if (avail == 0) break;
        std::string chunk(avail, 0);
        DWORD read = 0;
        WinHttpReadData(hRequest, &chunk[0], avail, &read);
        res.body.append(chunk.data(), read);
    } while (avail > 0);

    res.ok = true;

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    return res;
}

HttpResponse HttpGet(const std::wstring& host, int port,
                     const std::wstring& path) {
    return DoRequest(L"GET", host, port, path, "");
}

HttpResponse HttpPost(const std::wstring& host, int port,
                      const std::wstring& path,
                      const std::string& body) {
    return DoRequest(L"POST", host, port, path, body);
}

HttpResponse HttpPut(const std::wstring& host, int port,
                     const std::wstring& path,
                     const std::string& body) {
    return DoRequest(L"PUT", host, port, path, body);
}

HttpResponse HttpDelete(const std::wstring& host, int port,
                        const std::wstring& path) {
    return DoRequest(L"DELETE", host, port, path, "");
}