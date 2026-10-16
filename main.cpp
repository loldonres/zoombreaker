#define _WIN32_WINNT 0x0600

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#undef STB_VORBIS_HEADER_ONLY

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <windows.h>
#include <windowsx.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <uxtheme.h>
#include <dwrite_3.h>
#include <wrl.h>
#include "webview2/WebView2.h"
#include <string>
#include <algorithm>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <cmath>
#include <cstring>
#include <cwctype>
#include <dwmapi.h>

#include "neon_api.h"
#include "utf8_utils.h"
#include "settings.h"
#include "bass_boost.h"
#include "zoom_parse.h"

#define IDR_BUNDLE_WEBVIEW_LOADER 5101
#define IDR_BUNDLE_UI_HTML        5102
#define IDR_BUNDLE_UI_CSS         5103
#define IDR_BUNDLE_UI_JS          5104
#define IDR_BUNDLE_UI_FONT        5105
#define IDR_BUNDLE_UI_LOGO        5106
#define IDR_BUNDLE_NEON_EXE       5107
#define IDR_BUNDLE_NEON_DLL       5108

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")

 
#define ID_BTN_JOIN        1003
#define ID_EDIT_MEETING    1004
#define ID_EDIT_PASSWORD   1005
#define ID_TIMER_STATUS    2001
#define ID_TIMER_ANIMATION 2002
#define ID_TIMER_LOADING   2003
#define ID_TIMER_WEBUI_TIMEOUT 2004
#define ID_TIMER_LOADING_REVEAL 2005
#define ID_TIMER_LOADING_FADE 2006
#define ID_COMBO_MIC       3001
#define ID_LIST_SOUNDS     3002
#define ID_BTN_BROWSE      3003
#define ID_BTN_PLAY        3004
#define ID_CHK_HEADPHONES  3005
#define ID_CHK_MICROPHONE  3006
#define ID_TAB_JOIN        3007
#define ID_TAB_SOUNDPAD    3008
#define ID_CABLE_STATUS    3009
#define ID_TAB_SPAM        3011
#define ID_EDIT_SPAM_TEXT  3012
#define ID_EDIT_SPAM_MS    3013
#define ID_EDIT_SPAM_LIMIT 3014
#define ID_BTN_SPAM_START  3015
#define ID_BTN_SPAM_STOP   3016
#define ID_STATIC_SPAM_INFO 3017

#define ID_BTN_HAND_START     3019
#define ID_BTN_HAND_STOP      3020
#define ID_STATIC_HAND_INFO   3021

#define ID_BTN_BROWSE_FOLDER  3022
#define ID_BTN_PLAY_RANDOM    3023
#define ID_STATIC_NOW_PLAYING 3024
#define ID_SLIDER_VOLUME      3025
#define ID_STATIC_VOLUME      3026
#define ID_CHK_AUTONEXT_LIST  3027
#define ID_CHK_AUTONEXT_RAND  3028
#define ID_SLIDER_BASS        3029
#define ID_STATIC_BASS        3030

#define ID_BTN_SETTINGS       3100

#define ID_TAB_VIDEO         4001
#define ID_BTN_NEON_START    4002
#define ID_BTN_NEON_STOP     4003
#define ID_LIST_NEON_CAMS    4004
#define ID_BTN_NEON_CREATE   4005
#define ID_BTN_NEON_DELETE   4006
#define ID_BTN_NEON_ADDVID   4007
#define ID_COMBO_NEON_AUDIO  4008
#define ID_BTN_NEON_PLAY     4009
#define ID_BTN_NEON_PAUSE    4010
#define ID_BTN_NEON_STOPCAM  4011
#define ID_STATIC_NEON_STATUS 4012
#define ID_TAB_USERS         4013

#define ID_TAB_SCROLL_L      4014
#define ID_TAB_SCROLL_R      4015

#define ID_LIST_USERS        5001
#define ID_BTN_USERS_REFRESH 5002
#define ID_BTN_USERS_CLEAR   5003
#define ID_STATIC_USERS_INFO 5004

#define WM_APP_NOWPLAYING    (WM_APP + 1)
#define WM_APP_PLAY_NEXT     (WM_APP + 2)

 
 
static const COLORREF C_BG        = RGB(13, 12, 19);
static const COLORREF C_PANEL     = RGB(34, 33, 46);
static const COLORREF C_PANEL_HOV = RGB(48, 47, 72);
static const COLORREF C_INPUT     = RGB(22, 21, 32);
static const COLORREF C_BORDER    = RGB(57, 52, 94);
static const COLORREF C_TEXT      = RGB(255, 255, 255);
static const COLORREF C_TEXT_DIM  = RGB(148, 163, 184);
static const COLORREF C_TEXT_SUB  = RGB(203, 213, 225);
static const COLORREF C_OK        = RGB(34, 197, 139);
static const COLORREF C_ERR       = RGB(248, 91, 111);
static const COLORREF C_ACCENT    = RGB(37, 99, 235);
static const COLORREF C_PURPLE    = RGB(99, 102, 241);

 
HWND  g_hEditMeet    = nullptr;
HWND  g_hEditPass    = nullptr;
HWND  g_hComboMic    = nullptr;
HWND  g_hListSounds  = nullptr;
HWND  g_hBtnBrowse   = nullptr;
HWND  g_hBtnPlay     = nullptr;
HWND  g_hBtnBrowseFolder = nullptr;
HWND  g_hBtnPlayRandom   = nullptr;
HWND  g_hNowPlaying  = nullptr;
HWND  g_hSliderVolume = nullptr;
HWND  g_hStaticVolume = nullptr;
HWND  g_hSliderBass  = nullptr;
HWND  g_hStaticBass  = nullptr;
HWND  g_hChkAutoNextList = nullptr;
HWND  g_hChkAutoNextRand = nullptr;
HWND  g_hChkPhones   = nullptr;
HWND  g_hChkMic      = nullptr;
HWND  g_hBtnJoin     = nullptr;
HWND  g_hTabJoin     = nullptr;
HWND  g_hTabSoundpad = nullptr;
HWND  g_hTabSpam     = nullptr;
HWND  g_hTabVideo    = nullptr;
HWND  g_hTabUsers    = nullptr;
HWND  g_hTabScrollL  = nullptr;
HWND  g_hTabScrollR  = nullptr;
HWND  g_hCableStatus = nullptr;
HWND  g_hBtnSettings = nullptr;
std::wstring g_bundledRoot;

HWND  g_hListUsers       = nullptr;
HWND  g_hBtnUsersRefresh = nullptr;
HWND  g_hBtnUsersClear   = nullptr;
HWND  g_hStaticUsersInfo = nullptr;
HWND  g_hCopyToast       = nullptr;

int   g_tabsOffset   = 0;
const int kTabsVisible = 4;

HWND  g_hEditSpamText    = nullptr;
HWND  g_hEditSpamMs      = nullptr;
HWND  g_hEditSpamLimit   = nullptr;
HWND  g_hBtnSpamStart    = nullptr;
HWND  g_hBtnSpamStop     = nullptr;
HWND  g_hStaticSpamInfo  = nullptr;

HWND  g_hBtnHandStart    = nullptr;
HWND  g_hBtnHandStop     = nullptr;
HWND  g_hStaticHandInfo  = nullptr;

HWND  g_hBtnNeonStart     = nullptr;
HWND  g_hBtnNeonStop      = nullptr;
HWND  g_hListNeonCams     = nullptr;
HWND  g_hBtnNeonCreate    = nullptr;
HWND  g_hBtnNeonDelete    = nullptr;
HWND  g_hBtnNeonAddVid    = nullptr;
HWND  g_hComboNeonAudio   = nullptr;
HWND  g_hBtnNeonPlay      = nullptr;
HWND  g_hBtnNeonPause     = nullptr;
HWND  g_hBtnNeonStopCam   = nullptr;
HWND  g_hStaticNeonStatus = nullptr;

int   g_gainMin = 0;
int   g_gainMax = 200;
int   g_gainCur = 100;

bool  g_zoomRunning   = false;
DWORD g_zoomPid       = 0;
int   g_activeTab     = 0;
int   g_cableDeviceId = -1;

bool  g_neonAlive     = false;
int   g_neonCamCounter = 0;

std::atomic<bool> g_spamRunning{false};
std::atomic<int>  g_spamSent{0};

std::atomic<bool> g_handRunning{false};
std::atomic<bool> g_altYHeld{false};

std::vector<std::wstring> g_sounds;
std::vector<std::wstring> g_soundNames;
std::vector<std::string>  g_neonCamIds;

std::vector<std::wstring> g_neonAudioLabels;
std::vector<std::string>  g_neonAudioIds;

std::wstring g_nowPlayingText;

std::atomic<int>   g_volume{100};
std::atomic<float> g_gain{1.0f};
std::atomic<int>   g_bassBoost{0};

bool g_autoNextList   = false;
bool g_autoNextRandom = false;
bool g_checkPhones = false;
bool g_checkMic = true;
bool g_checkAutoNextList = false;
bool g_checkAutoNextRandom = false;
std::mt19937 g_rng{ std::random_device{}() };

std::atomic<bool> g_playing{false};
std::atomic<bool> g_stopRequested{false};

HFONT g_hCtrlFont = nullptr;
HANDLE g_hOswaldFont = nullptr;
const wchar_t* g_uiFontFace = L"Segoe UI";
using Microsoft::WRL::ComPtr;

bool g_webUiMode = true;
bool g_webUiLoading = true;
int g_loadingAngle = 0;
BYTE g_loadingAlpha = 255;
ULONGLONG g_webUiLoadStart = 0;
HWND g_hLoadingOverlay = nullptr;
HMODULE g_webViewLoader = nullptr;
ComPtr<ICoreWebView2Controller> g_webController;
ComPtr<ICoreWebView2> g_webView;
EventRegistrationToken g_webMessageToken{};
std::wstring g_lastWebUiState;

void PostWebUiState(bool includeForms = false);
void PostWebUiToast(const std::wstring& message);
void WebViewInitializationFailed(HWND hwnd, HRESULT error);

bool IsUiChecked(HWND hwnd) {
    if (hwnd == g_hChkPhones) return g_checkPhones;
    if (hwnd == g_hChkMic) return g_checkMic;
    if (hwnd == g_hChkAutoNextList) return g_checkAutoNextList;
    if (hwnd == g_hChkAutoNextRand) return g_checkAutoNextRandom;
    return false;
}

void SetUiChecked(HWND hwnd, bool checked) {
    if (hwnd == g_hChkPhones) g_checkPhones = checked;
    else if (hwnd == g_hChkMic) g_checkMic = checked;
    else if (hwnd == g_hChkAutoNextList) g_checkAutoNextList = checked;
    else if (hwnd == g_hChkAutoNextRand) g_checkAutoNextRandom = checked;
    if (hwnd) InvalidateRect(hwnd, nullptr, FALSE);
}

bool IsUiCheckboxId(int id) {
    return id == ID_CHK_HEADPHONES || id == ID_CHK_MICROPHONE ||
           id == ID_CHK_AUTONEXT_LIST || id == ID_CHK_AUTONEXT_RAND;
}

 
std::wstring StripSpaces(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size());
    for (wchar_t c : s) {
        if (c != L' ' && c != L'\t' && c != L'\r' && c != L'\n')
            out.push_back(c);
    }
    return out;
}

std::wstring BaseNameNoExt(const std::wstring& full) {
    const wchar_t* fname = PathFindFileNameW(full.c_str());
    std::wstring s = fname ? fname : full;
    size_t dot = s.find_last_of(L'.');
    if (dot != std::wstring::npos && dot > 0) s = s.substr(0, dot);
    return s;
}

std::wstring GetBundledPath(const wchar_t* relativePath) {
    if (g_bundledRoot.empty() || !relativePath) return {};
    std::wstring path = g_bundledRoot;
    path += L"\\";
    path += relativePath;
    return path;
}

bool EnsureDirectoryTree(const std::wstring& path) {
    if (path.empty()) return false;
    const int result = SHCreateDirectoryExW(nullptr, path.c_str(), nullptr);
    if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS &&
        result != ERROR_FILE_EXISTS) return false;
    const DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool ExtractBundledResource(HINSTANCE instance, int resourceId,
                            const wchar_t* relativePath) {
    if (g_bundledRoot.empty() || !relativePath) return false;
    const std::wstring target = GetBundledPath(relativePath);
    const size_t separator = target.find_last_of(L"\\/");
    if (separator == std::wstring::npos ||
        !EnsureDirectoryTree(target.substr(0, separator))) return false;

    HRSRC resource = FindResourceW(instance, MAKEINTRESOURCEW(resourceId),
                                   RT_RCDATA);
    if (!resource) return false;
    const DWORD resourceSize = SizeofResource(instance, resource);
    HGLOBAL loadedResource = LoadResource(instance, resource);
    const BYTE* resourceData = loadedResource
        ? static_cast<const BYTE*>(LockResource(loadedResource)) : nullptr;
    if (!resourceData || resourceSize == 0) return false;

     
    HANDLE existing = CreateFileW(target.c_str(), GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (existing != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER existingSize{};
        bool identical = GetFileSizeEx(existing, &existingSize) &&
                         existingSize.QuadPart == resourceSize;
        if (identical) {
            std::vector<BYTE> current(resourceSize);
            DWORD bytesRead = 0;
            identical = ReadFile(existing, current.data(), resourceSize,
                                 &bytesRead, nullptr) &&
                        bytesRead == resourceSize &&
                        memcmp(current.data(), resourceData, resourceSize) == 0;
        }
        CloseHandle(existing);
        if (identical) return true;
    }

    HANDLE output = CreateFileW(target.c_str(), GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (output == INVALID_HANDLE_VALUE) return false;
    DWORD bytesWritten = 0;
    const BOOL wrote = WriteFile(output, resourceData, resourceSize,
                                 &bytesWritten, nullptr);
    CloseHandle(output);
    return wrote && bytesWritten == resourceSize;
}

bool PrepareBundledFiles(HINSTANCE instance) {
    wchar_t localAppData[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
                                nullptr, SHGFP_TYPE_CURRENT, localAppData)))
        return false;
    g_bundledRoot = localAppData;
    g_bundledRoot += L"\\ZoomBreaker\\Bundled";
    if (!EnsureDirectoryTree(g_bundledRoot)) return false;

    struct BundleItem { int id; const wchar_t* path; };
    static const BundleItem items[] = {
        { IDR_BUNDLE_WEBVIEW_LOADER, L"WebView2Loader.dll" },
        { IDR_BUNDLE_UI_HTML, L"ui\\app_ui.html" },
        { IDR_BUNDLE_UI_CSS, L"ui\\app_ui.css" },
        { IDR_BUNDLE_UI_JS, L"ui\\app_ui.js" },
        { IDR_BUNDLE_UI_FONT, L"ui\\Oswald-Regular.woff2" },
        { IDR_BUNDLE_UI_LOGO, L"ui\\zoomraid prokect\\zoombreaker1.png" },
        { IDR_BUNDLE_NEON_EXE, L"Neon\\binaries\\dfvcam.exe" },
        { IDR_BUNDLE_NEON_DLL, L"Neon\\binaries\\dfvcam_source.dll" },
    };
    for (const BundleItem& item : items) {
        if (!ExtractBundledResource(instance, item.id, item.path)) return false;
    }
    return true;
}

bool LoadOswaldFont() {
    const std::wstring fontPath = GetBundledPath(L"ui\\Oswald-Regular.woff2");
    if (fontPath.empty()) return false;

    HANDLE file = CreateFileW(fontPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    LARGE_INTEGER fileSize{};
    bool loaded = false;
    if (GetFileSizeEx(file, &fileSize) && fileSize.QuadPart > 0 &&
        fileSize.QuadPart <= MAXDWORD) {
        std::vector<BYTE> packed((size_t)fileSize.QuadPart);
        DWORD bytesRead = 0;
        if (ReadFile(file, packed.data(), (DWORD)packed.size(), &bytesRead,
                     nullptr) && bytesRead == packed.size()) {
            HMODULE dwriteModule = LoadLibraryW(L"dwrite.dll");
            using DWriteCreateFactoryFn = HRESULT (WINAPI *)(
                DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
            auto createFactory = dwriteModule
                ? (DWriteCreateFactoryFn)GetProcAddress(
                      dwriteModule, "DWriteCreateFactory")
                : nullptr;

            IDWriteFactory* factory = nullptr;
            IDWriteFactory5* factory5 = nullptr;
            IDWriteFontFileStream* unpacked = nullptr;

            HRESULT hr = createFactory
                ? createFactory(DWRITE_FACTORY_TYPE_SHARED,
                                __uuidof(IDWriteFactory),
                                reinterpret_cast<IUnknown**>(&factory))
                : E_FAIL;
            if (SUCCEEDED(hr) && factory) {
                hr = factory->QueryInterface(__uuidof(IDWriteFactory5),
                                             (void**)&factory5);
            }

            DWRITE_CONTAINER_TYPE container = DWRITE_CONTAINER_TYPE_UNKNOWN;
            if (SUCCEEDED(hr) && factory5) {
                container = factory5->AnalyzeContainerType(
                    packed.data(), (UINT32)packed.size());
            }
            if (SUCCEEDED(hr) && factory5 &&
                container == DWRITE_CONTAINER_TYPE_WOFF2) {
                hr = factory5->UnpackFontFile(
                    container, packed.data(), (UINT32)packed.size(), &unpacked);
            }

            if (SUCCEEDED(hr) && unpacked) {
                UINT64 unpackedSize = 0;
                if (SUCCEEDED(unpacked->GetFileSize(&unpackedSize)) &&
                    unpackedSize > 0 && unpackedSize <= MAXDWORD) {
                    const void* fontData = nullptr;
                    void* fragmentContext = nullptr;
                    hr = unpacked->ReadFileFragment(
                        &fontData, 0, unpackedSize, &fragmentContext);
                    if (SUCCEEDED(hr) && fontData) {
                        DWORD fontCount = 0;
                        g_hOswaldFont = AddFontMemResourceEx(
                            const_cast<void*>(fontData), (DWORD)unpackedSize,
                            nullptr, &fontCount);
                        loaded = g_hOswaldFont != nullptr && fontCount > 0;
                        if (fragmentContext)
                            unpacked->ReleaseFileFragment(fragmentContext);
                    }
                }
            }

            if (unpacked) unpacked->Release();
            if (factory5) factory5->Release();
            if (factory) factory->Release();
            if (dwriteModule) FreeLibrary(dwriteModule);
        }
    }
    CloseHandle(file);

    if (loaded) g_uiFontFace = L"Oswald";
    return loaded;
}

void SetTextIfChanged(HWND h, const std::wstring& newText) {
    if (!h) return;
    wchar_t cur[1024] = {0};
    GetWindowTextW(h, cur, 1023);
    if (newText == cur) return;
    SetWindowTextW(h, newText.c_str());
}

void SetNowPlaying(const std::wstring& text) {
    g_nowPlayingText = text;
    SetTextIfChanged(g_hNowPlaying, text);
    if (g_hNowPlaying)
        InvalidateRect(g_hNowPlaying, nullptr, TRUE);
}

void RefreshVolumeSliderRange() {
    if (!g_hSliderVolume) return;

    SendMessageW(g_hSliderVolume, TBM_SETRANGE, TRUE,
                 MAKELPARAM(g_gainMin, g_gainMax));

    if (g_gainCur < g_gainMin) g_gainCur = g_gainMin;
    if (g_gainCur > g_gainMax) g_gainCur = g_gainMax;

    SendMessageW(g_hSliderVolume, TBM_SETPOS, TRUE, g_gainCur);

    g_volume = g_gainCur;
    g_gain = g_gainCur / 100.0f;

    wchar_t buf[32];
    swprintf(buf, 32, L"%d%%", g_gainCur);
    SetTextIfChanged(g_hStaticVolume, buf);
    InvalidateRect(g_hStaticVolume, nullptr, TRUE);
}

bool IsAudioExtension(const std::wstring& ext) {
    if (ext.empty()) return false;
    std::wstring e = ext;
    for (auto& c : e) c = towlower(c);
    return e == L".wav" || e == L".mp3" || e == L".ogg" ||
           e == L".flac" || e == L".m4a" || e == L".aac" ||
           e == L".opus" || e == L".wma";
}

void ScanAudioFolder(const std::wstring& folder,
                     std::vector<std::wstring>& outPaths,
                     std::vector<std::wstring>& outNames) {
    WIN32_FIND_DATAW fd{};
    std::wstring mask = folder + L"\\*";
    HANDLE h = FindFirstFileW(mask.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) continue;

        std::wstring full = folder + L"\\" + fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanAudioFolder(full, outPaths, outNames);
        } else {
            std::wstring ext = PathFindExtensionW(fd.cFileName);
            if (IsAudioExtension(ext)) {
                outPaths.push_back(full);
                outNames.push_back(BaseNameNoExt(full));
            }
        }
    } while (FindNextFileW(h, &fd));

    FindClose(h);
}

 
DWORD FindProcessId(const wchar_t* exeName) {
    DWORD pid = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, exeName) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return pid;
}

bool IsProcessRunning(const wchar_t* exeName) {
    return FindProcessId(exeName) != 0;
}

 
 
 
void FillMicrophones(HWND hCombo) {
    SendMessageW(hCombo, CB_RESETCONTENT, 0, 0);

    UINT n = waveInGetNumDevs();
    if (n == 0) {
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"(микрофоны не найдены)");
        SendMessageW(hCombo, CB_SETCURSEL, 0, 0);
        return;
    }

    for (UINT i = 0; i < n; ++i) {
        WAVEINCAPSW caps{};
        if (waveInGetDevCapsW(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)caps.szPname);
        }
    }
    SendMessageW(hCombo, CB_SETCURSEL, 0, 0);
}

 
 
 
int FindCableInputDevice() {
    UINT n = waveOutGetNumDevs();
    for (UINT i = 0; i < n; ++i) {
        WAVEOUTCAPSW caps{};
        if (waveOutGetDevCapsW(i, &caps, sizeof(caps)) != MMSYSERR_NOERROR)
            continue;

        std::wstring name = caps.szPname;

        if (name.find(L"CABLE Input") != std::wstring::npos)
            return (int)i;
        if (name.find(L"VoiceMeeter Input") != std::wstring::npos)
            return (int)i;
        if (name.find(L"VB-Audio") != std::wstring::npos &&
            name.find(L"Input")   != std::wstring::npos)
            return (int)i;
    }
    return -1;
}

std::wstring GetCableFriendlyName() {
    if (g_cableDeviceId < 0) return L"";
    WAVEOUTCAPSW caps{};
    if (waveOutGetDevCapsW(g_cableDeviceId, &caps, sizeof(caps)) != MMSYSERR_NOERROR)
        return L"";
    return caps.szPname;
}

void UpdateCableStatus() {
    if (!g_hCableStatus) return;
    std::wstring txt;
    if (g_cableDeviceId < 0) {
        txt = L"Кабель: не найден — звук в микрофон недоступен";
    } else {
        txt = L"Кабель: " + GetCableFriendlyName();
        if (txt.length() > 55) txt = txt.substr(0, 52) + L"...";
    }
    SetTextIfChanged(g_hCableStatus, txt);
}

 
 
 
bool LoadAudio(const std::wstring& path, std::vector<BYTE>& outData,
               WAVEFORMATEX& outFmt) {
     
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER sz{};
    if (!GetFileSizeEx(h, &sz) || sz.QuadPart == 0) {
        CloseHandle(h);
        return false;
    }

    std::vector<BYTE> fileData((size_t)sz.QuadPart);
    DWORD read = 0;
    BOOL ok = ReadFile(h, fileData.data(), (DWORD)fileData.size(), &read, nullptr);
    CloseHandle(h);
    if (!ok || read != fileData.size()) return false;

     
    ma_decoder_config cfg = ma_decoder_config_init(ma_format_s16, 0, 0);
    ma_decoder dec;
    ma_result r = ma_decoder_init_memory(fileData.data(), fileData.size(),
                                         &cfg, &dec);
    if (r != MA_SUCCESS) {
        return false;
    }

    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&dec, &totalFrames);

    const ma_uint32 channels = dec.outputChannels;
    const ma_uint32 sampleRate = dec.outputSampleRate;

    std::vector<int16_t> pcm;

    if (totalFrames > 0) {
        pcm.resize((size_t)totalFrames * channels);
        ma_uint64 read2 = 0;
        ma_decoder_read_pcm_frames(&dec, pcm.data(), totalFrames, &read2);
        pcm.resize((size_t)read2 * channels);
    } else {
        const ma_uint64 kChunk = 4096;
        std::vector<int16_t> buf((size_t)kChunk * channels);
        while (true) {
            ma_uint64 read2 = 0;
            ma_decoder_read_pcm_frames(&dec, buf.data(), kChunk, &read2);
            if (read2 == 0) break;
            pcm.insert(pcm.end(), buf.begin(), buf.begin() + read2 * channels);
            if (read2 < kChunk) break;
        }
    }

    ma_decoder_uninit(&dec);

    if (pcm.empty()) return false;

    outData.resize(pcm.size() * sizeof(int16_t));
    memcpy(outData.data(), pcm.data(), outData.size());

    ZeroMemory(&outFmt, sizeof(outFmt));
    outFmt.wFormatTag      = WAVE_FORMAT_PCM;
    outFmt.nChannels       = (WORD)channels;
    outFmt.nSamplesPerSec  = sampleRate;
    outFmt.wBitsPerSample  = 16;
    outFmt.nBlockAlign     = (WORD)(channels * 16 / 8);
    outFmt.nAvgBytesPerSec = sampleRate * outFmt.nBlockAlign;
    outFmt.cbSize          = 0;

    return true;
}

 
 
 
struct PlayStream {
    std::vector<BYTE> source;
    size_t            cursor = 0;
    WAVEFORMATEX      fmt{};
    HWAVEOUT          hwo = nullptr;

    std::vector<BYTE> buf[3];
    WAVEHDR           hdr[3];
    bool              inUse[3] = { false, false, false };

    std::atomic<bool> finished{false};
};

static void ApplyGainToBuf(BYTE* data, size_t bytes, float gain) {
    if (gain == 1.0f) return;
    int16_t* s = (int16_t*)data;
    size_t n = bytes / sizeof(int16_t);
    for (size_t i = 0; i < n; ++i) {
        int v = (int)(s[i] * gain);
        if (v >  32767) v =  32767;
        if (v < -32768) v = -32768;
        s[i] = (int16_t)v;
    }
}

static const size_t kChunkBytes = 8192;

static bool FeedInto(PlayStream* s, int slot) {
    if (s->cursor >= s->source.size()) return false;

    size_t frameAlign = s->fmt.nBlockAlign;
    size_t avail = s->source.size() - s->cursor;
    size_t bytes = kChunkBytes;
    if (bytes > avail) bytes = avail;
    bytes = (bytes / frameAlign) * frameAlign;
    if (bytes == 0) return false;

    s->buf[slot].resize(bytes);
    memcpy(s->buf[slot].data(), s->source.data() + s->cursor, bytes);

    int bassAmt = g_bassBoost.load();
    if (bassAmt > 0) {
        static thread_local BassBoost bb;
        static thread_local int lastRate = 0;
        static thread_local int lastCh = 0;

        int ch = s->fmt.nChannels;
        int rate = s->fmt.nSamplesPerSec;
        if (lastRate != rate) {
            bb.SetSampleRate(rate);
            lastRate = rate;
            bb.Reset();
        }
        if (lastCh != ch) {
            bb.Reset();
            lastCh = ch;
        }

        bb.SetAmount(bassAmt / 100.0f);
        size_t samples = bytes / (sizeof(int16_t) * ch);
        bb.Process((int16_t*)s->buf[slot].data(), samples, ch);
    }

    ApplyGainToBuf(s->buf[slot].data(), bytes, g_gain.load());

    ZeroMemory(&s->hdr[slot], sizeof(WAVEHDR));
    s->hdr[slot].lpData = (LPSTR)s->buf[slot].data();
    s->hdr[slot].dwBufferLength = (DWORD)bytes;

    if (waveOutPrepareHeader(s->hwo, &s->hdr[slot], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        return false;
    if (waveOutWrite(s->hwo, &s->hdr[slot], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        waveOutUnprepareHeader(s->hwo, &s->hdr[slot], sizeof(WAVEHDR));
        return false;
    }

    s->inUse[slot] = true;
    s->cursor += bytes;
    return true;
}

static void CALLBACK WaveOutProc(HWAVEOUT, UINT msg, DWORD_PTR user,
                                 DWORD_PTR, DWORD_PTR) {
    if (msg != WOM_DONE) return;
    PlayStream* s = (PlayStream*)user;
    if (!s) return;
    if (g_stopRequested.load()) return;

    for (int i = 0; i < 3; ++i) {
        if (s->inUse[i] && (s->hdr[i].dwFlags & WHDR_DONE)) {
            waveOutUnprepareHeader(s->hwo, &s->hdr[i], sizeof(WAVEHDR));
            s->inUse[i] = false;
            FeedInto(s, i);
        }
    }

    if (!s->inUse[0] && !s->inUse[1] && !s->inUse[2] &&
        s->cursor >= s->source.size()) {
        s->finished = true;
    }
}

void PlayToDevice(int deviceId, std::vector<BYTE> source,
                  const WAVEFORMATEX& fmt) {
    PlayStream s;
    s.source = std::move(source);
    s.fmt = fmt;
    s.cursor = 0;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    MMRESULT r = waveOutOpen(&s.hwo, deviceId, &fmt,
                             (DWORD_PTR)WaveOutProc,
                             (DWORD_PTR)&s,
                             CALLBACK_FUNCTION);
    if (r != MMSYSERR_NOERROR) {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        return;
    }

    FeedInto(&s, 0);
    FeedInto(&s, 1);
    FeedInto(&s, 2);

    if (!s.inUse[0] && !s.inUse[1] && !s.inUse[2]) {
        waveOutClose(s.hwo);
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        return;
    }

    while (!s.finished.load()) {
        if (g_stopRequested.load()) {
            waveOutReset(s.hwo);
            break;
        }
        Sleep(10);
    }

    Sleep(30);
    waveOutReset(s.hwo);
    waveOutClose(s.hwo);

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
}

void ShowCableMissingDialog(HWND owner) {
    int res = MessageBoxW(owner,
        L"Виртуальный аудиокабель не найден.\n\n"
        L"Чтобы звук шёл в микрофон Zoom, нужно один раз "
        L"установить VB-Audio Virtual Cable.\n\n"
        L"Открыть сайт загрузки?",
        L"Soundpad — нужен виртуальный кабель",
        MB_YESNO | MB_ICONINFORMATION);

    if (res == IDYES) {
        ShellExecuteW(nullptr, L"open",
                      L"https://vb-audio.com/Cable/",
                      nullptr, nullptr, SW_SHOWNORMAL);
    }
}

std::vector<std::thread> g_playThreads;

void StopCurrentPlayback() {
    if (g_playThreads.empty()) {
        g_stopRequested = false;
        return;
    }
    g_stopRequested = true;
    for (auto& t : g_playThreads)
        if (t.joinable()) t.join();
    g_playThreads.clear();
    g_stopRequested = false;
    g_playing = false;
}

bool PlaySoundFileInternal(HWND owner, const std::wstring& path,
                           bool toPhones, bool toMic,
                           bool fromAutoNext) {
    StopCurrentPlayback();

    std::wstring shortName = PathFindFileNameW(path.c_str());

    if (toMic && g_cableDeviceId < 0) {
        SetNowPlaying(L"Ошибка: нет кабеля");
        ShowCableMissingDialog(owner);
        return false;
    }

    std::vector<BYTE> data;
    WAVEFORMATEX fmt{};
    if (!LoadAudio(path, data, fmt)) {
        SetNowPlaying(L"Ошибка чтения: " + shortName);
        return false;
    }

    g_playing = true;
    g_stopRequested = false;

    int cableId = g_cableDeviceId;

    if (toPhones) {
        std::vector<BYTE> dataCopy = data;
        g_playThreads.emplace_back([dataCopy = std::move(dataCopy), fmt]() {
            PlayToDevice(WAVE_MAPPER, dataCopy, fmt);
        });
    }
    if (toMic && cableId >= 0) {
        std::vector<BYTE> dataCopy = data;
        g_playThreads.emplace_back([dataCopy = std::move(dataCopy), fmt, cableId]() {
            PlayToDevice(cableId, dataCopy, fmt);
        });
    }

    SetNowPlaying(L"Играет: " + shortName);
    if (g_hNowPlaying) InvalidateRect(g_hNowPlaying, nullptr, TRUE);

    (void)fromAutoNext;
    return true;
}

void TriggerNextAuto(HWND hwnd, bool wasRandom) {
    if (g_sounds.empty()) return;

    bool toPhones = IsUiChecked(g_hChkPhones);
    bool toMic    = IsUiChecked(g_hChkMic);
    if (!toPhones && !toMic) return;

    int idx = -1;

    if (wasRandom && g_autoNextRandom) {
        std::uniform_int_distribution<size_t> dist(0, g_sounds.size() - 1);
        idx = (int)dist(g_rng);
    } else if (!wasRandom && g_autoNextList) {
        int cur = (int)SendMessageW(g_hListSounds, LB_GETCURSEL, 0, 0);
        idx = (cur + 1) % (int)g_sounds.size();
        SendMessageW(g_hListSounds, LB_SETCURSEL, idx, 0);
    } else {
        return;
    }

    if (idx < 0 || idx >= (int)g_sounds.size()) return;

    PlaySoundFileInternal(hwnd, g_sounds[idx], toPhones, toMic, wasRandom);
}

void PlaySoundFileUI(HWND owner, const std::wstring& path,
                     bool toPhones, bool toMic, bool random = false) {
    PlaySoundFileInternal(owner, path, toPhones, toMic, random);
}

 
 
 
bool SetClipboardTextW(const std::wstring& text) {
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();

    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hMem) {
        CloseClipboard();
        return false;
    }
    void* p = GlobalLock(hMem);
    memcpy(p, text.c_str(), bytes);
    GlobalUnlock(hMem);

    if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }
    CloseClipboard();
    return true;
}

void SendPasteAndEnter() {
    INPUT in[4] = {};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = VK_CONTROL;
    in[1].type = INPUT_KEYBOARD;
    in[1].ki.wVk = 'V';
    in[2].type = INPUT_KEYBOARD;
    in[2].ki.wVk = 'V';
    in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3].type = INPUT_KEYBOARD;
    in[3].ki.wVk = VK_CONTROL;
    in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, in, sizeof(INPUT));

    Sleep(60);

    INPUT en[2] = {};
    en[0].type = INPUT_KEYBOARD;
    en[0].ki.wVk = VK_RETURN;
    en[1].type = INPUT_KEYBOARD;
    en[1].ki.wVk = VK_RETURN;
    en[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, en, sizeof(INPUT));
}

void SpamWorker(HWND hwnd, std::wstring text, int intervalMs, int limit) {
    for (int i = 3; i > 0 && g_spamRunning; --i) {
        std::wstring s = L"Спам стартует через " + std::to_wstring(i) + L"...";
        SetTextIfChanged(g_hStaticSpamInfo, s);
        Sleep(1000);
    }

    if (!g_spamRunning) {
        SetTextIfChanged(g_hStaticSpamInfo, L"Отменено.");
        return;
    }

    int sent = 0;
    while (g_spamRunning) {
        if (!SetClipboardTextW(text)) {
            SetTextIfChanged(g_hStaticSpamInfo, L"Не удалось записать в буфер.");
            break;
        }

        SendPasteAndEnter();
        ++sent;
        g_spamSent = sent;

        wchar_t buf[128];
        if (limit > 0) {
            swprintf(buf, 128, L"Отправлено: %d / %d", sent, limit);
        } else {
            swprintf(buf, 128, L"Отправлено: %d", sent);
        }
        SetTextIfChanged(g_hStaticSpamInfo, buf);

        if (limit > 0 && sent >= limit) break;

        int slept = 0;
        while (slept < intervalMs && g_spamRunning) {
            Sleep(50);
            slept += 50;
        }
    }

    g_spamRunning = false;
    if (g_hStaticSpamInfo) {
        std::wstring done = L"Остановлено. Всего отправлено: " +
                            std::to_wstring(g_spamSent.load());
        SetTextIfChanged(g_hStaticSpamInfo, done);
    }
}

 
 
 
void PressAltY() {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = VK_MENU;
    in[1].type = INPUT_KEYBOARD;
    in[1].ki.wVk = 'Y';
    SendInput(2, in, sizeof(INPUT));
    g_altYHeld = true;
}

void ReleaseAltY() {
    if (!g_altYHeld.load()) return;

    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = 'Y';
    in[0].ki.dwFlags = KEYEVENTF_KEYUP;
    in[1].type = INPUT_KEYBOARD;
    in[1].ki.wVk = VK_MENU;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
    g_altYHeld = false;
}

void HandSpamWorker(HWND hwnd) {
    for (int i = 3; i > 0 && g_handRunning; --i) {
        std::wstring s = L"Зажим Alt+Y через " + std::to_wstring(i) + L"...";
        SetTextIfChanged(g_hStaticHandInfo, s);
        Sleep(1000);
    }

    if (!g_handRunning) {
        SetTextIfChanged(g_hStaticHandInfo, L"Отменено.");
        return;
    }

    PressAltY();
    SetTextIfChanged(g_hStaticHandInfo, L"Alt+Y зажат (STOP для отпускания)");

    while (g_handRunning) {
        Sleep(100);
    }

    ReleaseAltY();
    SetTextIfChanged(g_hStaticHandInfo, L"Остановлено. Клавиши отпущены.");
}

 
 
 
void BrowseSounds(HWND hwnd) {
    wchar_t fileBuf[4096] = {0};

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hwnd;
    ofn.lpstrFilter =
        L"Аудио (*.wav;*.mp3;*.ogg;*.flac;*.m4a)\0"
        L"*.wav;*.mp3;*.ogg;*.flac;*.m4a\0"
        L"Все файлы (*.*)\0*.*\0";
    ofn.lpstrFile   = fileBuf;
    ofn.nMaxFile    = 4096;
    ofn.lpstrTitle  = L"Обзор звуков";
    ofn.Flags       = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                      OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (!GetOpenFileNameW(&ofn)) return;

    if (ofn.nFileOffset > 0 && fileBuf[ofn.nFileOffset - 1] == L'\0') {
        std::wstring dir = fileBuf;
        const wchar_t* p = fileBuf + dir.length() + 1;
        while (*p) {
            std::wstring full = dir + L"\\" + p;
            g_sounds.push_back(full);
            std::wstring name = BaseNameNoExt(full);
            g_soundNames.push_back(name);
            SendMessageW(g_hListSounds, LB_ADDSTRING, 0, (LPARAM)name.c_str());
            p += wcslen(p) + 1;
        }
    } else {
        std::wstring full = fileBuf;
        g_sounds.push_back(full);
        std::wstring name = BaseNameNoExt(full);
        g_soundNames.push_back(name);
        SendMessageW(g_hListSounds, LB_ADDSTRING, 0, (LPARAM)name.c_str());
    }
}

void BrowseSoundFolder(HWND hwnd) {
    BROWSEINFOW bi{};
    bi.hwndOwner = hwnd;
    bi.lpszTitle = L"Выбери папку со звуками";
    bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (!pidl) return;

    wchar_t path[MAX_PATH] = {0};
    SHGetPathFromIDListW(pidl, path);
    CoTaskMemFree(pidl);

    if (path[0] == 0) return;

    std::vector<std::wstring> paths, names;
    ScanAudioFolder(path, paths, names);

    if (paths.empty()) {
        MessageBoxW(hwnd, L"В выбранной папке нет аудиофайлов.",
                    L"Soundpad", MB_OK | MB_ICONINFORMATION);
        return;
    }

    for (size_t i = 0; i < paths.size(); ++i) {
        g_sounds.push_back(paths[i]);
        g_soundNames.push_back(names[i]);
        SendMessageW(g_hListSounds, LB_ADDSTRING, 0, (LPARAM)names[i].c_str());
    }

    wchar_t msg[128];
    swprintf(msg, 128, L"Добавлено файлов: %d", (int)paths.size());
    MessageBoxW(hwnd, msg, L"Soundpad", MB_OK | MB_ICONINFORMATION);
}

 
 
 
struct BtnState {
    bool hover = false;
    bool isPrimary = false;
    bool isActiveTab = false;
    bool isTab = false;
    float activeProgress = 0.0f;
    float animationFrom = 0.0f;
    float animationTarget = 0.0f;
    ULONGLONG animationStart = 0;
    bool animating = false;
};

COLORREF MixColor(COLORREF a, COLORREF b, int amount) {
    if (amount < 0) amount = 0;
    if (amount > 255) amount = 255;
    return RGB(
        (GetRValue(a) * (255 - amount) + GetRValue(b) * amount) / 255,
        (GetGValue(a) * (255 - amount) + GetGValue(b) * amount) / 255,
        (GetBValue(a) * (255 - amount) + GetBValue(b) * amount) / 255);
}

void DrawRoundedPanel(HDC hdc, const RECT& rc, COLORREF fill,
                      COLORREF border, int radius) {
    HRGN region = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom,
                                     radius, radius);
    if (region) {
        HBRUSH brush = CreateSolidBrush(fill);
        FillRgn(hdc, region, brush);
        DeleteObject(brush);
        DeleteObject(region);
    }

    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1,
              radius, radius);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void DrawRadialGlow(HDC hdc, int cx, int cy, int maxRadius,
                    COLORREF tint, int strength) {
     
    const int layers = 44;
    for (int i = layers; i >= 1; --i) {
        int radius = maxRadius * i / layers;
        int amount = strength * (layers - i + 1) / layers;
        HBRUSH brush = CreateSolidBrush(MixColor(C_BG, tint, amount));
        HGDIOBJ oldBrush = SelectObject(hdc, brush);
        HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
        Ellipse(hdc, cx - radius, cy - radius, cx + radius, cy + radius);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(brush);
    }
}

void DrawButtonFill(HDC hdc, const RECT& rc, bool gradient,
                    bool hover, COLORREF solidColor, int radius) {
    if (!gradient) {
        HRGN region = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom,
                                         radius, radius);
        if (region) {
            HBRUSH brush = CreateSolidBrush(solidColor);
            FillRgn(hdc, region, brush);
            DeleteObject(brush);
            DeleteObject(region);
        }
        return;
    }

    int saved = SaveDC(hdc);
    HRGN region = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom,
                                     radius, radius);
    if (region) SelectClipRgn(hdc, region);
    int height = rc.bottom - rc.top;
    COLORREF topColor = hover ? RGB(59, 130, 246) : C_ACCENT;
    COLORREF bottomColor = hover ? C_ACCENT : C_PURPLE;
    for (int y = 0; y < height; ++y) {
        int amount = height > 1 ? (y * 255) / (height - 1) : 0;
        COLORREF color = MixColor(topColor, bottomColor, amount);
        HPEN pen = CreatePen(PS_SOLID, 1, color);
        HGDIOBJ oldPen = SelectObject(hdc, pen);
        MoveToEx(hdc, rc.left, rc.top + y, nullptr);
        LineTo(hdc, rc.right, rc.top + y);
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
    if (saved) RestoreDC(hdc, saved);
    if (region) DeleteObject(region);
}

void DrawOwnerButton(LPDRAWITEMSTRUCT dis, BtnState* st) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;

    bool hover   = st ? st->hover : false;
    bool pressed = (dis->itemState & ODS_SELECTED) != 0;
    bool primary = st ? st->isPrimary : false;
    bool active  = st ? st->isActiveTab : false;
    bool disabled = (dis->itemState & ODS_DISABLED) != 0;
    bool tabButton = st && st->isTab;

    if (disabled) {
        hover = false;
        pressed = false;
    }

    COLORREF borderCol;
    if (tabButton) {
        float progress = st->activeProgress;
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;
        COLORREF tabBase = disabled ? RGB(26, 25, 35)
                           : ((hover || pressed) ? C_PANEL_HOV : C_PANEL);
        DrawButtonFill(hdc, rc, false, hover, tabBase, 24);

        if (!disabled && progress > 0.01f) {
            float scale = 0.84f + 0.16f * progress;
            int insetX = (int)((rc.right - rc.left) * (1.0f - scale) / 2.0f);
            int insetY = (int)((rc.bottom - rc.top) * (1.0f - scale) / 2.0f);
            RECT activeRc = { rc.left + insetX, rc.top + insetY,
                              rc.right - insetX, rc.bottom - insetY };
            bool useGradient = progress >= 0.98f && !pressed;
            COLORREF activeFill = MixColor(C_PANEL, C_ACCENT,
                                           (int)(progress * 255));
            if (pressed) activeFill = MixColor(activeFill, RGB(26, 43, 83), 90);
            DrawButtonFill(hdc, activeRc, useGradient, hover, activeFill, 24);
        }
        borderCol = MixColor(C_BORDER, C_PURPLE, (int)(progress * 220));
        if (hover) borderCol = C_PURPLE;
    } else {
        bool useGradient = !disabled && !pressed && (active || primary);
        COLORREF bg = disabled ? RGB(26, 25, 35)
                      : (pressed ? RGB(31, 48, 92)
                      : (hover ? C_PANEL_HOV : C_PANEL));
        DrawButtonFill(hdc, rc, useGradient, hover, bg, 24);
        borderCol = disabled ? RGB(44, 42, 57)
                    : (active || primary || hover ? C_PURPLE : C_BORDER);
    }

    HPEN pen = CreatePen(PS_SOLID, 1, borderCol);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1, 24, 24);
    SelectObject(hdc, oldBr);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    wchar_t txt[128] = {0};
    GetWindowTextW(dis->hwndItem, txt, 127);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, disabled ? RGB(90, 90, 100)
                               : (tabButton && st->activeProgress < 0.45f
                                      ? C_TEXT_SUB : C_TEXT));

    HFONT f = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
                          DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HFONT old = (HFONT)SelectObject(hdc, f);
    DrawTextW(hdc, txt, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old);
    DeleteObject(f);
}

LRESULT CALLBACK BtnSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                             UINT_PTR, DWORD_PTR) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEMOVE: {
        BtnState* st = (BtnState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (st && !st->hover) {
            st->hover = true;
            TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE: {
        BtnState* st = (BtnState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (st) {
            st->hover = false;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_NCDESTROY: {
        BtnState* st = (BtnState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (st) {
            delete st;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        }
        RemoveWindowSubclass(hwnd, BtnSubclass, 1);
        break;
    }
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

void UpdateTabAnimations(HWND hwnd) {
    HWND tabs[5] = {
        g_hTabJoin, g_hTabSoundpad, g_hTabSpam, g_hTabVideo, g_hTabUsers
    };
    ULONGLONG now = GetTickCount64();
    bool stillAnimating = false;

    for (HWND tab : tabs) {
        if (!tab) continue;
        BtnState* state = (BtnState*)GetWindowLongPtrW(tab, GWLP_USERDATA);
        if (!state || !state->animating) continue;

        float t = (float)(now - state->animationStart) / 220.0f;
        if (t >= 1.0f) {
            state->activeProgress = state->animationTarget;
            state->animating = false;
        } else {
            float eased = t * t * (3.0f - 2.0f * t);
            state->activeProgress = state->animationFrom +
                (state->animationTarget - state->animationFrom) * eased;
            stillAnimating = true;
        }
        InvalidateRect(tab, nullptr, FALSE);
    }

    if (!stillAnimating) KillTimer(hwnd, ID_TIMER_ANIMATION);
}

HWND CreateStyledButton(HWND parent, const wchar_t* text, int id,
                        int x, int y, int w, int h, bool primary,
                        bool activeTab = false) {
    HWND btn = CreateWindowExW(0, L"BUTTON", text,
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        x, y, w, h, parent, (HMENU)(INT_PTR)id,
        GetModuleHandleW(nullptr), nullptr);

    BtnState* st = new BtnState();
    st->isPrimary = primary;
    st->isActiveTab = activeTab;
    st->isTab = id == ID_TAB_JOIN || id == ID_TAB_SOUNDPAD ||
                id == ID_TAB_SPAM || id == ID_TAB_VIDEO ||
                id == ID_TAB_USERS;
    st->activeProgress = activeTab ? 1.0f : 0.0f;
    SetWindowLongPtrW(btn, GWLP_USERDATA, (LONG_PTR)st);
    SetWindowSubclass(btn, BtnSubclass, 1, 0);
    HRGN buttonRegion = CreateRoundRectRgn(0, 0, w, h, 24, 24);
    if (buttonRegion && !SetWindowRgn(btn, buttonRegion, TRUE))
        DeleteObject(buttonRegion);
    return btn;
}

void SetTabActive(HWND hwnd, bool active) {
    BtnState* st = (BtnState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (st) {
        if (st->isActiveTab != active) {
            st->animationFrom = st->activeProgress;
            st->animationTarget = active ? 1.0f : 0.0f;
            st->animationStart = GetTickCount64();
            st->animating = true;
            SetTimer(GetParent(hwnd), ID_TIMER_ANIMATION, 16, nullptr);
        }
        st->isActiveTab = active;
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

struct CheckBoxState {
    bool hover = false;
};

LRESULT CALLBACK CheckBoxSubclassProc(HWND hwnd, UINT msg, WPARAM wp,
                                      LPARAM lp, UINT_PTR subclassId,
                                      DWORD_PTR refData) {
    CheckBoxState* state = (CheckBoxState*)refData;
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEMOVE:
        if (state && !state->hover) {
            state->hover = true;
            TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    case WM_MOUSELEAVE:
        if (state) {
            state->hover = false;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    case WM_NCDESTROY:
        delete state;
        RemoveWindowSubclass(hwnd, CheckBoxSubclassProc, subclassId);
        break;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

void DrawOwnerCheckbox(LPDRAWITEMSTRUCT dis) {
    if (!dis || !dis->hwndItem) return;
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    bool checked = IsUiChecked(dis->hwndItem);
    bool enabled = (dis->itemState & ODS_DISABLED) == 0;
    bool pressed = (dis->itemState & ODS_SELECTED) != 0;
    POINT cursor{};
    GetCursorPos(&cursor);
    ScreenToClient(dis->hwndItem, &cursor);
    bool hover = PtInRect(&rc, cursor) != FALSE;

    COLORREF rowFill = hover || pressed ? C_PANEL_HOV : C_PANEL;
    DrawRoundedPanel(hdc, rc, rowFill, rowFill, 12);

    const int boxSize = 18;
    int boxY = rc.top + ((rc.bottom - rc.top) - boxSize) / 2;
    RECT box = { rc.left + 4, boxY, rc.left + 4 + boxSize, boxY + boxSize };
    COLORREF edge = checked ? C_ACCENT : (hover ? C_PURPLE : C_BORDER);
    DrawRoundedPanel(hdc, box,
                     checked ? C_ACCENT : (enabled ? C_INPUT : C_PANEL),
                     edge, 8);

    if (checked) {
        HPEN checkPen = CreatePen(PS_SOLID, 2, C_TEXT);
        HGDIOBJ oldPen = SelectObject(hdc, checkPen);
        POINT points[3] = {
            { box.left + 4, box.top + 9 },
            { box.left + 7, box.top + 12 },
            { box.left + 14, box.top + 5 }
        };
        Polyline(hdc, points, 3);
        SelectObject(hdc, oldPen);
        DeleteObject(checkPen);
    }

    wchar_t label[256]{};
    GetWindowTextW(dis->hwndItem, label, 255);
    const wchar_t* display = label;
    while (*display == L' ' || *display == L'\t') ++display;
    HFONT font = (HFONT)SendMessageW(dis->hwndItem, WM_GETFONT, 0, 0);
    if (!font) font = g_hCtrlFont;
    HGDIOBJ oldFont = font ? SelectObject(hdc, font) : nullptr;
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, enabled ? C_TEXT_SUB : C_TEXT_DIM);
    RECT textRc = { box.right + 8, rc.top, rc.right - 4, rc.bottom };
    DrawTextW(hdc, display, -1, &textRc,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    if (oldFont) SelectObject(hdc, oldFont);
    if (dis->itemState & ODS_FOCUS) {
        RECT focusRc = { rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1 };
        DrawFocusRect(hdc, &focusRc);
    }
}

LRESULT CALLBACK TrackbarSubclassProc(HWND hwnd, UINT msg, WPARAM wp,
                                      LPARAM lp, UINT_PTR subclassId,
                                      DWORD_PTR) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc{};
        GetClientRect(hwnd, &rc);
        HBRUSH bg = CreateSolidBrush(C_PANEL);
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);

        int minValue = (int)SendMessageW(hwnd, TBM_GETRANGEMIN, 0, 0);
        int maxValue = (int)SendMessageW(hwnd, TBM_GETRANGEMAX, 0, 0);
        int value = (int)SendMessageW(hwnd, TBM_GETPOS, 0, 0);
        if (maxValue <= minValue) maxValue = minValue + 1;
        if (value < minValue) value = minValue;
        if (value > maxValue) value = maxValue;

        int thumbInset = 10;
        int x0 = thumbInset;
        int x1 = rc.right - thumbInset;
        int centerY = (rc.top + rc.bottom) / 2;
        int thumbX = x0 + (x1 - x0) * (value - minValue) /
                             (maxValue - minValue);

        RECT channel = { x0, centerY - 2, x1 + 1, centerY + 2 };
        DrawRoundedPanel(hdc, channel, C_INPUT, C_INPUT, 6);
        if (thumbX > x0) {
            RECT fill = { x0, centerY - 2, thumbX + 1, centerY + 2 };
            DrawButtonFill(hdc, fill, true, false, C_ACCENT, 8);
        }

        HBRUSH thumbFill = CreateSolidBrush(C_PANEL_HOV);
        HPEN thumbPen = CreatePen(PS_SOLID, 1, C_PURPLE);
        HGDIOBJ oldPen = SelectObject(hdc, thumbPen);
        HGDIOBJ oldBrush = SelectObject(hdc, thumbFill);
        Ellipse(hdc, thumbX - 8, centerY - 8, thumbX + 9, centerY + 9);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(thumbFill);
        DeleteObject(thumbPen);

        HBRUSH innerFill = CreateSolidBrush(C_TEXT_SUB);
        HPEN innerPen = CreatePen(PS_SOLID, 1, C_TEXT_SUB);
        oldPen = SelectObject(hdc, innerPen);
        oldBrush = SelectObject(hdc, innerFill);
        Ellipse(hdc, thumbX - 3, centerY - 3, thumbX + 4, centerY + 4);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(innerFill);
        DeleteObject(innerPen);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, TrackbarSubclassProc, subclassId);
        break;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

struct ComboState {
    bool hover = false;
};

LRESULT CALLBACK ComboSubclassProc(HWND hwnd, UINT msg, WPARAM wp,
                                   LPARAM lp, UINT_PTR subclassId,
                                   DWORD_PTR refData) {
    ComboState* state = (ComboState*)refData;
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEMOVE:
        if (state && !state->hover) {
            state->hover = true;
            TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    case WM_MOUSELEAVE:
        if (state) {
            state->hover = false;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT client{};
        GetClientRect(hwnd, &client);
        int boxHeight = (int)SendMessageW(hwnd, CB_GETITEMHEIGHT, (WPARAM)-1, 0);
        if (boxHeight <= 0) boxHeight = 26;
        int clientHeight = (int)client.bottom;
        int visibleHeight = boxHeight < clientHeight ? boxHeight : clientHeight;
        RECT rc = { 0, 0, client.right, visibleHeight };
        bool enabled = IsWindowEnabled(hwnd) != FALSE;
        bool focused = GetFocus() == hwnd;
        COLORREF border = (state && state->hover) || focused ? C_PURPLE : C_BORDER;
        DrawRoundedPanel(hdc, rc, C_INPUT, border, 20);

        wchar_t label[512]{};
        LRESULT selected = SendMessageW(hwnd, CB_GETCURSEL, 0, 0);
        if (selected != CB_ERR)
            SendMessageW(hwnd, CB_GETLBTEXT, (WPARAM)selected, (LPARAM)label);

        HFONT font = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
        if (!font) font = g_hCtrlFont;
        HGDIOBJ oldFont = font ? SelectObject(hdc, font) : nullptr;
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, enabled ? C_TEXT : C_TEXT_DIM);
        RECT textRc = { 10, rc.top + 1, rc.right - 30, rc.bottom - 1 };
        DrawTextW(hdc, label, -1, &textRc,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        if (oldFont) SelectObject(hdc, oldFont);

        int arrowX = rc.right - 16;
        int arrowY = (rc.top + rc.bottom) / 2;
        HPEN arrowPen = CreatePen(PS_SOLID, 2, enabled ? C_TEXT_SUB : C_TEXT_DIM);
        HGDIOBJ oldPen = SelectObject(hdc, arrowPen);
        MoveToEx(hdc, arrowX - 4, arrowY - 2, nullptr);
        LineTo(hdc, arrowX, arrowY + 2);
        LineTo(hdc, arrowX + 4, arrowY - 2);
        SelectObject(hdc, oldPen);
        DeleteObject(arrowPen);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_NCDESTROY:
        delete state;
        RemoveWindowSubclass(hwnd, ComboSubclassProc, subclassId);
        break;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

 
 
 
void DrawComboItem(LPDRAWITEMSTRUCT dis) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;

    bool selected = (dis->itemState & ODS_SELECTED) != 0;
    bool editField = (dis->itemState & ODS_COMBOBOXEDIT) != 0;
    bool disabled = (dis->itemState & ODS_DISABLED) != 0;

    COLORREF bg = selected && !editField ? C_ACCENT : C_INPUT;
    HBRUSH br = CreateSolidBrush(bg);
    FillRect(hdc, &rc, br);
    DeleteObject(br);

    wchar_t label[512]{};
    LRESULT item = (dis->itemID == (UINT)-1)
        ? SendMessageW(dis->hwndItem, CB_GETCURSEL, 0, 0)
        : (LRESULT)dis->itemID;
    if (item != CB_ERR)
        SendMessageW(dis->hwndItem, CB_GETLBTEXT, (WPARAM)item,
                     (LPARAM)label);
    else
        GetWindowTextW(dis->hwndItem, label, 511);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, disabled ? C_TEXT_DIM : C_TEXT);

    HFONT f = CreateFontW(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                          DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HFONT old = (HFONT)SelectObject(hdc, f);

    RECT trc = rc;
    trc.left += 9;
    trc.right -= 6;
    DrawTextW(hdc, label, -1, &trc,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    SelectObject(hdc, old);
    DeleteObject(f);
}

 
 
 
#define ID_COPY_TOAST_TIMER 9001

LRESULT CALLBACK CopyToastProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);

        DrawButtonFill(hdc, rc, true, false, C_ACCENT, 16);
        HPEN borderPen = CreatePen(PS_SOLID, 1, C_PURPLE);
        HGDIOBJ oldPen = SelectObject(hdc, borderPen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        RoundRect(hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1, 16, 16);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(borderPen);

        wchar_t buf[256] = {0};
        GetWindowTextW(hwnd, buf, 255);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        HFONT f = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
                              DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
        HFONT old = (HFONT)SelectObject(hdc, f);
        DrawTextW(hdc, buf, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, old);
        DeleteObject(f);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (wp == ID_COPY_TOAST_TIMER) {
            KillTimer(hwnd, ID_COPY_TOAST_TIMER);
            ShowWindow(hwnd, SW_HIDE);
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void ShowCopyToast(HWND parent, const std::wstring& text) {
    if (g_webUiMode && g_webView) {
        PostWebUiToast(text);
        return;
    }

    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = CopyToastProc;
        wc.hInstance     = GetModuleHandleW(nullptr);
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = L"ZoomBreakerCopyToast";
        RegisterClassExW(&wc);
        registered = true;
    }

    if (!g_hCopyToast) {
        g_hCopyToast = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
            L"ZoomBreakerCopyToast", L"",
            WS_POPUP,
            0, 0, 260, 40,
            parent, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (g_hCopyToast) {
            HRGN region = CreateRoundRectRgn(0, 0, 260, 40, 16, 16);
            if (!SetWindowRgn(g_hCopyToast, region, TRUE))
                DeleteObject(region);
        }
    }

    if (!g_hCopyToast) return;

    SetWindowTextW(g_hCopyToast, text.c_str());

    RECT pr; GetWindowRect(parent, &pr);
    int toastW = 260, toastH = 40;
    int x = pr.left + (pr.right - pr.left - toastW) / 2;
    int y = pr.top + 60;

    SetWindowPos(g_hCopyToast, HWND_TOPMOST,
                 x, y, toastW, toastH,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);

    InvalidateRect(g_hCopyToast, nullptr, TRUE);
    UpdateWindow(g_hCopyToast);

    SetTimer(g_hCopyToast, ID_COPY_TOAST_TIMER, 1500, nullptr);
}

 
 
 
void SetDirectChildrenVisibility(HWND parent, int showCommand) {
    for (HWND child = GetWindow(parent, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        ShowWindow(child, showCommand);
    }
}

void UpdateTabsVisibility() {
    if (g_webUiMode) return;

    HWND tabs[5] = {
        g_hTabJoin, g_hTabSoundpad, g_hTabSpam, g_hTabVideo, g_hTabUsers
    };

    const int kBaseX = 46;
    const int kStep  = 98;
    const int kTabW  = 94;
    const int kVisibleL = 46;
    const int kVisibleR = 438;

    int shift = -g_tabsOffset * kStep;

    for (int i = 0; i < 5; ++i) {
        if (!tabs[i]) continue;

        int x = kBaseX + i * kStep + shift;
        bool visible = (x >= kVisibleL && (x + kTabW) <= kVisibleR);

        SetWindowPos(tabs[i], nullptr, x, 112, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

        if (visible) {
            ShowWindow(tabs[i], SW_SHOW);
            InvalidateRect(tabs[i], nullptr, TRUE);
            UpdateWindow(tabs[i]);
        } else {
            ShowWindow(tabs[i], SW_HIDE);
        }
    }

    int maxOffset = 5 - kTabsVisible;
    if (maxOffset < 0) maxOffset = 0;

    EnableWindow(g_hTabScrollL, g_tabsOffset > 0);
    EnableWindow(g_hTabScrollR, g_tabsOffset < maxOffset);

    InvalidateRect(g_hTabScrollL, nullptr, TRUE);
    InvalidateRect(g_hTabScrollR, nullptr, TRUE);
    UpdateWindow(g_hTabScrollL);
    UpdateWindow(g_hTabScrollR);
}

void ScrollTabs(int delta) {
    int maxOffset = 5 - kTabsVisible;
    if (maxOffset < 0) maxOffset = 0;

    int newOffset = g_tabsOffset + delta;
    if (newOffset < 0) newOffset = 0;
    if (newOffset > maxOffset) newOffset = maxOffset;
    if (newOffset == g_tabsOffset) return;

    g_tabsOffset = newOffset;
    UpdateTabsVisibility();
}

 
 
 
void ShowTab(int idx) {
    if (idx < 0 || idx > 4) return;
    g_activeTab = idx;

    if (g_webUiMode) {
        PostWebUiState();
        return;
    }

    ShowWindow(g_hEditMeet, idx == 0 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hEditPass, idx == 0 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnJoin,  idx == 0 ? SW_SHOW : SW_HIDE);

    ShowWindow(g_hComboMic,        idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hListSounds,      idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnBrowse,       idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnBrowseFolder, idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnPlay,         idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnPlayRandom,   idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hNowPlaying,      idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hSliderVolume,    idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hStaticVolume,    idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hSliderBass,      idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hStaticBass,      idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hChkAutoNextList, idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hChkAutoNextRand, idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hChkPhones,       idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hChkMic,          idx == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hCableStatus,     idx == 1 ? SW_SHOW : SW_HIDE);

    ShowWindow(g_hEditSpamText,   idx == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hEditSpamMs,     idx == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hEditSpamLimit,  idx == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnSpamStart,   idx == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnSpamStop,    idx == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hStaticSpamInfo, idx == 2 ? SW_SHOW : SW_HIDE);

    ShowWindow(g_hBtnHandStart,   idx == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnHandStop,    idx == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hStaticHandInfo, idx == 2 ? SW_SHOW : SW_HIDE);

    ShowWindow(g_hBtnNeonStart,    idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnNeonStop,     idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hListNeonCams,    idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnNeonCreate,   idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnNeonDelete,   idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnNeonAddVid,   idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hComboNeonAudio,  idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnNeonPlay,     idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnNeonPause,    idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnNeonStopCam,  idx == 3 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hStaticNeonStatus,idx == 3 ? SW_SHOW : SW_HIDE);

    ShowWindow(g_hListUsers,       idx == 4 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnUsersRefresh, idx == 4 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hBtnUsersClear,   idx == 4 ? SW_SHOW : SW_HIDE);
    ShowWindow(g_hStaticUsersInfo, idx == 4 ? SW_SHOW : SW_HIDE);

    SetTabActive(g_hTabJoin,     idx == 0);
    SetTabActive(g_hTabSoundpad, idx == 1);
    SetTabActive(g_hTabSpam,     idx == 2);
    SetTabActive(g_hTabVideo,    idx == 3);
    SetTabActive(g_hTabUsers,    idx == 4);
}

 
 
 
void PaintStatusPanel(HDC hdc) {
    RECT panel = { 20, 55, 460, 105 };
    DrawRoundedPanel(hdc, panel, C_PANEL, C_BORDER, 16);

    bool online = g_zoomRunning;
    COLORREF col = online ? C_OK : C_ERR;

    int dotR = 8;
    int dotX = panel.left + 24;
    int dotY = (panel.top + panel.bottom) / 2;
    HPEN haloPen = CreatePen(PS_SOLID, 1, MixColor(C_PANEL, col, 90));
    HGDIOBJ oldHaloPen = SelectObject(hdc, haloPen);
    HGDIOBJ oldHaloBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Ellipse(hdc, dotX - 12, dotY - 12, dotX + 12, dotY + 12);
    SelectObject(hdc, oldHaloBrush);
    SelectObject(hdc, oldHaloPen);
    DeleteObject(haloPen);

    HBRUSH dotBr = CreateSolidBrush(col);
    HPEN dotPen = CreatePen(PS_SOLID, 1, col);
    HPEN op = (HPEN)SelectObject(hdc, dotPen);
    HBRUSH ob = (HBRUSH)SelectObject(hdc, dotBr);
    Ellipse(hdc, dotX - dotR, dotY - dotR, dotX + dotR, dotY + dotR);
    SelectObject(hdc, ob);
    SelectObject(hdc, op);
    DeleteObject(dotBr);
    DeleteObject(dotPen);

    SetBkMode(hdc, TRANSPARENT);

    HFONT fBig = CreateFontW(-18, 0, 0, 0, FW_BOLD, 0, 0, 0,
                             DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HFONT fSmall = CreateFontW(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                               DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);

    HFONT old = (HFONT)SelectObject(hdc, fBig);
    SetTextColor(hdc, col);
    const wchar_t* mainTxt = online ? L"ONLINE" : L"OFFLINE";
    RECT t1 = { panel.left + 50, panel.top + 6, panel.right - 10, panel.top + 30 };
    DrawTextW(hdc, mainTxt, -1, &t1, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, fSmall);
    SetTextColor(hdc, C_TEXT_DIM);

    std::wstring subTxt;
    if (g_zoomRunning) {
        subTxt = L"Zoom.exe запущен";
        if (g_zoomPid) {
            wchar_t pidBuf[32];
            swprintf(pidBuf, 32, L" (PID %lu)", g_zoomPid);
            subTxt += pidBuf;
        }
    } else {
        subTxt = L"Zoom.exe не найден";
    }

    RECT t2 = { panel.left + 50, panel.top + 28, panel.right - 10, panel.bottom - 4 };
    DrawTextW(hdc, subTxt.c_str(), -1, &t2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, old);
    DeleteObject(fBig);
    DeleteObject(fSmall);
}

void DrawLoadingScreen(HDC target, const RECT& bounds) {
    const int width = bounds.right - bounds.left;
    const int height = bounds.bottom - bounds.top;
    HDC backBuffer = CreateCompatibleDC(target);
    HBITMAP bitmap = (width > 0 && height > 0)
        ? CreateCompatibleBitmap(target, width, height) : nullptr;
    HGDIOBJ oldBitmap = bitmap ? SelectObject(backBuffer, bitmap) : nullptr;
    bool buffered = bitmap && oldBitmap && oldBitmap != HGDI_ERROR;
    HDC hdc = buffered ? backBuffer : target;
    if (buffered) {
        IntersectClipRect(hdc, bounds.left, bounds.top,
                          bounds.right, bounds.bottom);
    }

    HBRUSH background = CreateSolidBrush(C_BG);
    FillRect(hdc, &bounds, background);
    DeleteObject(background);

    const int centerX = width / 2;
    const int centerY = height / 2;
    DrawRadialGlow(hdc, centerX - 210, centerY - 110, 270, C_ACCENT, 27);
    DrawRadialGlow(hdc, centerX + 220, centerY + 130, 300, C_PURPLE, 25);

    int cardWidth = (std::min)(460, width - 32);
    if (cardWidth < 280) cardWidth = 280;
    RECT card = { centerX - cardWidth / 2, centerY - 162,
                  centerX + cardWidth / 2, centerY + 162 };
    DrawRoundedPanel(hdc, card, C_PANEL, C_BORDER, 26);

    SetBkMode(hdc, TRANSPARENT);
    RECT badge = { centerX - 27, card.top + 27, centerX + 27, card.top + 81 };
    DrawButtonFill(hdc, badge, true, false, C_ACCENT, 18);
    HFONT badgeFont = CreateFontW(-21, 0, 0, 0, FW_BOLD, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HGDIOBJ oldFont = SelectObject(hdc, badgeFont);
    SetTextColor(hdc, C_TEXT);
    DrawTextW(hdc, L"ZR", -1, &badge, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    HFONT brandFont = CreateFontW(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    SelectObject(hdc, brandFont);
    SetTextColor(hdc, RGB(165, 180, 252));
    RECT brand = { card.left + 20, card.top + 88, card.right - 20, card.top + 110 };
    DrawTextW(hdc, L"ZOOM RAIDERS", -1, &brand,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    HFONT titleFont = CreateFontW(-25, 0, 0, 0, FW_BOLD, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    SelectObject(hdc, titleFont);
    SetTextColor(hdc, C_TEXT);
    RECT title = { card.left + 20, card.top + 116, card.right - 20, card.top + 151 };
    DrawTextW(hdc, L"Zoom Breaker", -1, &title,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, brandFont);
    SetTextColor(hdc, C_TEXT_DIM);
    RECT subtitle = { card.left + 18, card.top + 158, card.right - 18, card.top + 181 };
    DrawTextW(hdc, L"Подготавливаем интерфейс и устройства", -1, &subtitle,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    const int spinnerY = card.top + 220;
    HPEN trackPen = CreatePen(PS_SOLID, 3, C_BORDER);
    HGDIOBJ oldPen = SelectObject(hdc, trackPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Ellipse(hdc, centerX - 14, spinnerY - 14, centerX + 14, spinnerY + 14);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(trackPen);

    const double radians = (double)g_loadingAngle * 3.14159265358979323846 / 180.0;
    int dotX = centerX + (int)(12.0 * cos(radians));
    int dotY = spinnerY + (int)(12.0 * sin(radians));
    HBRUSH dotBrush = CreateSolidBrush(RGB(129, 140, 248));
    oldBrush = SelectObject(hdc, dotBrush);
    oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
    Ellipse(hdc, dotX - 4, dotY - 4, dotX + 4, dotY + 4);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(dotBrush);

    HFONT hintFont = CreateFontW(-10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
    SelectObject(hdc, hintFont);
    SetTextColor(hdc, RGB(112, 117, 139));
    RECT hint = { card.left + 20, card.top + 264, card.right - 20, card.top + 282 };
    DrawTextW(hdc, L"Это займёт всего несколько секунд", -1, &hint,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, oldFont);
    DeleteObject(hintFont);
    DeleteObject(titleFont);
    DeleteObject(brandFont);
    DeleteObject(badgeFont);

    if (buffered) {
        SelectObject(backBuffer, oldBitmap);
        BitBlt(target, 0, 0, width, height, backBuffer, 0, 0, SRCCOPY);
    }
    if (bitmap) DeleteObject(bitmap);
    if (backBuffer) DeleteDC(backBuffer);
}

void InvalidateLoadingSpinner(HWND hwnd) {
    if (g_hLoadingOverlay && IsWindowVisible(g_hLoadingOverlay)) {
        RECT overlayBounds{};
        GetClientRect(g_hLoadingOverlay, &overlayBounds);
        const int overlayCenterX = (overlayBounds.left + overlayBounds.right) / 2;
        const int overlayCenterY = (overlayBounds.top + overlayBounds.bottom) / 2;
        RECT spinner = { overlayCenterX - 22, overlayCenterY + 36,
                         overlayCenterX + 22, overlayCenterY + 80 };
        InvalidateRect(g_hLoadingOverlay, &spinner, FALSE);
        return;
    }
    RECT bounds{};
    GetClientRect(hwnd, &bounds);
    const int centerX = (bounds.left + bounds.right) / 2;
    const int centerY = (bounds.top + bounds.bottom) / 2;
    RECT spinner = { centerX - 22, centerY + 36, centerX + 22, centerY + 80 };
    InvalidateRect(hwnd, &spinner, FALSE);
}

LRESULT CALLBACK LoadingOverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT bounds{};
        GetClientRect(hwnd, &bounds);
        DrawLoadingScreen(hdc, bounds);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void PositionLoadingOverlay(HWND parent) {
    if (!g_hLoadingOverlay || !IsWindow(g_hLoadingOverlay) ||
        !g_webUiMode ||
        (!g_webUiLoading && !IsWindowVisible(g_hLoadingOverlay))) return;
    if (IsIconic(parent)) {
        ShowWindow(g_hLoadingOverlay, SW_HIDE);
        return;
    }
    RECT bounds{};
    GetClientRect(parent, &bounds);
    POINT origin{ bounds.left, bounds.top };
    ClientToScreen(parent, &origin);
    SetWindowPos(g_hLoadingOverlay, HWND_TOPMOST, origin.x, origin.y,
        bounds.right - bounds.left, bounds.bottom - bounds.top,
        SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateWindow(g_hLoadingOverlay);
}

void HideLoadingOverlay() {
    if (!g_hLoadingOverlay || !IsWindow(g_hLoadingOverlay)) return;
    ShowWindow(g_hLoadingOverlay, SW_HIDE);
    SetWindowPos(g_hLoadingOverlay, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void PaintMain(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc; GetClientRect(hwnd, &rc);

    if (g_webUiMode && g_webUiLoading) {
        DrawLoadingScreen(hdc, rc);
        EndPaint(hwnd, &ps);
        return;
    }

    HBRUSH brBg = CreateSolidBrush(C_BG);
    FillRect(hdc, &rc, brBg);
    DeleteObject(brBg);

    DrawRadialGlow(hdc, 38, 96, 315, RGB(37, 99, 235), 40);
    DrawRadialGlow(hdc, 452, 615, 330, C_PURPLE, 40);

    SetBkMode(hdc, TRANSPARENT);
    HFONT fBig = CreateFontW(-18, 0, 0, 0, FW_BOLD, 0, 0, 0,
                             DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HFONT old = (HFONT)SelectObject(hdc, fBig);
    SetTextColor(hdc, C_TEXT);
    RECT titleMark = { 20, 18, 25, 41 };
    DrawRoundedPanel(hdc, titleMark, C_ACCENT, C_ACCENT, 5);
    RECT title = { 34, 15, 390, 45 };
    DrawTextW(hdc, L"ZOOM BREAKER", -1, &title, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old);
    DeleteObject(fBig);

    PaintStatusPanel(hdc);

    RECT content = { 12, 158, 468, 708 };
    DrawRoundedPanel(hdc, content, C_PANEL, C_BORDER, 18);

    HFONT fSmall = CreateFontW(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                               DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HFONT oldF = (HFONT)SelectObject(hdc, fSmall);
    SetTextColor(hdc, C_TEXT_SUB);

    if (g_activeTab == 0) {
        RECT l1 = { 20, 170, 220, 188 };
        DrawTextW(hdc, L"Meeting ID / Code", -1, &l1,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l2 = { 20, 234, 220, 252 };
        DrawTextW(hdc, L"Password", -1, &l2,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    } else if (g_activeTab == 1) {
        RECT l3 = { 20, 170, 220, 188 };
        DrawTextW(hdc, L"Микрофон", -1, &l3,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l4 = { 20, 234, 220, 252 };
        DrawTextW(hdc, L"Звуки", -1, &l4,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l5 = { 20, 510, 220, 528 };
        DrawTextW(hdc, L"Громкость", -1, &l5,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l5b = { 20, 552, 220, 570 };
        DrawTextW(hdc, L"Bass Boost", -1, &l5b,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    } else if (g_activeTab == 2) {
        RECT l6 = { 20, 170, 220, 188 };
        DrawTextW(hdc, L"Текст сообщения", -1, &l6,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l7 = { 20, 280, 210, 298 };
        DrawTextW(hdc, L"Интервал, мс", -1, &l7,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l8 = { 250, 280, 440, 298 };
        DrawTextW(hdc, L"Лимит (0 = ∞)", -1, &l8,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l9 = { 20, 406, 440, 424 };
        DrawTextW(hdc, L"Alt+Y — непрерывный зажим", -1, &l9,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT l10 = { 20, 430, 440, 448 };
        DrawTextW(hdc, L"Зажимается через 3 сек, отпускается на STOP",
                  -1, &l10, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    } else if (g_activeTab == 4) {
        RECT lu = { 20, 170, 440, 188 };
        DrawTextW(hdc, L"Участники Zoom (видимые в окне Participants)",
                  -1, &lu, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(hdc, oldF);
    DeleteObject(fSmall);

    EndPaint(hwnd, &ps);
}

void RefreshStatus(HWND hwnd) {
    bool wasRunning = g_zoomRunning;
    DWORD wasPid    = g_zoomPid;

    DWORD pid = FindProcessId(L"Zoom.exe");
    g_zoomRunning = (pid != 0);
    g_zoomPid = pid;

    if (!g_zoomRunning && g_spamRunning.load()) {
        g_spamRunning = false;
        SetTextIfChanged(g_hStaticSpamInfo,
                         L"Zoom.exe пропал — спам остановлен.");
    }

    if (!g_zoomRunning && g_handRunning.load()) {
        g_handRunning = false;
        SetTextIfChanged(g_hStaticHandInfo,
                         L"Zoom.exe пропал — спам рукой остановлен.");
    }

    if (wasRunning != g_zoomRunning || wasPid != g_zoomPid) {
        InvalidateRect(hwnd, nullptr, TRUE);
    }

    int cableNow = FindCableInputDevice();
    if (cableNow != g_cableDeviceId) {
        g_cableDeviceId = cableNow;
        UpdateCableStatus();
    }

    if (g_hNowPlaying) {
        static bool wasPlaying = false;
        bool nowPlaying = g_playing.load();
        if (wasPlaying != nowPlaying) {
            wasPlaying = nowPlaying;
            InvalidateRect(g_hNowPlaying, nullptr, TRUE);
        }
    }

    if (g_activeTab == 3) {
        bool alive = NeonApi::IsAlive();
        if (alive != g_neonAlive) {
            g_neonAlive = alive;
            SetTextIfChanged(g_hStaticNeonStatus,
                alive ? L"Neon: сервис активен"
                      : L"Neon: сервис не найден");
            InvalidateRect(g_hStaticNeonStatus, nullptr, TRUE);
        }
    }
}

 
 
 
void RefreshNeonCameras() {
    SendMessageW(g_hListNeonCams, LB_RESETCONTENT, 0, 0);
    g_neonCamIds.clear();

    if (!g_neonAlive) return;

    auto cams = NeonApi::ListCameras();
    for (const auto& c : cams) {
        std::wstring name  = Utf8ToWString(c.name);
        std::wstring state = Utf8ToWString(c.playState);
        std::wstring line  = name + L"  [" + state + L"]  " +
                             (c.vcam_active ? L"active" : L"inactive");
        SendMessageW(g_hListNeonCams, LB_ADDSTRING, 0, (LPARAM)line.c_str());
        g_neonCamIds.push_back(c.id);
    }
}

std::string GetSelectedNeonCamId() {
    int sel = (int)SendMessageW(g_hListNeonCams, LB_GETCURSEL, 0, 0);
    if (sel < 0 || sel >= (int)g_neonCamIds.size()) return {};
    return g_neonCamIds[sel];
}

void FillNeonAudioDevices(HWND hCombo) {
    SendMessageW(hCombo, CB_RESETCONTENT, 0, 0);
    g_neonAudioLabels.clear();
    g_neonAudioIds.clear();

    auto devices = NeonApi::ListAudioDevices();
    for (const auto& d : devices) {
        std::wstring label = Utf8ToWString(d.name);
        if (label.empty()) label = Utf8ToWString(d.id);
        if (d.isDefault) label += L" (default)";

        g_neonAudioLabels.push_back(label);
        g_neonAudioIds.push_back(d.id);

        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"");
    }

    if (!devices.empty())
        SendMessageW(hCombo, CB_SETCURSEL, 0, 0);
}

std::wstring JsonEscape(const std::wstring& value) {
    std::wstring out;
    out.reserve(value.size() + 8);
    for (wchar_t ch : value) {
        switch (ch) {
        case L'"': out += L"\\\""; break;
        case L'\\': out += L"\\\\"; break;
        case L'\b': out += L"\\b"; break;
        case L'\f': out += L"\\f"; break;
        case L'\n': out += L"\\n"; break;
        case L'\r': out += L"\\r"; break;
        case L'\t': out += L"\\t"; break;
        default:
            if (ch < 0x20) {
                wchar_t escaped[7];
                swprintf(escaped, 7, L"\\u%04x", (unsigned int)ch);
                out += escaped;
            } else {
                out.push_back(ch);
            }
        }
    }
    return out;
}

std::wstring JsonQuoted(const std::wstring& value) {
    return L"\"" + JsonEscape(value) + L"\"";
}

std::wstring JsonBoolean(bool value) {
    return value ? L"true" : L"false";
}

std::wstring JsonArray(const std::vector<std::wstring>& values) {
    std::wstring out = L"[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i) out += L",";
        out += JsonQuoted(values[i]);
    }
    out += L"]";
    return out;
}

std::wstring GetControlText(HWND hwnd) {
    if (!hwnd) return {};
    int length = GetWindowTextLengthW(hwnd);
    if (length < 0) length = 0;
    std::wstring text((size_t)length + 1, L'\0');
    GetWindowTextW(hwnd, &text[0], length + 1);
    text.resize(wcslen(text.c_str()));
    return text;
}

std::vector<std::wstring> GetControlItems(HWND hwnd, bool comboBox) {
    std::vector<std::wstring> items;
    if (!hwnd) return items;
    LRESULT count = SendMessageW(hwnd, comboBox ? CB_GETCOUNT : LB_GETCOUNT, 0, 0);
    if (count < 0 || count > 20000) return items;
    items.reserve((size_t)count);
    for (LRESULT i = 0; i < count; ++i) {
        LRESULT length = SendMessageW(hwnd,
            comboBox ? CB_GETLBTEXTLEN : LB_GETTEXTLEN, (WPARAM)i, 0);
        if (length < 0 || length > 16384) continue;
        std::wstring text((size_t)length + 1, L'\0');
        LRESULT result = SendMessageW(hwnd,
            comboBox ? CB_GETLBTEXT : LB_GETTEXT, (WPARAM)i,
            (LPARAM)&text[0]);
        if (result == CB_ERR || result == LB_ERR) continue;
        text.resize(wcslen(text.c_str()));
        items.push_back(std::move(text));
    }
    return items;
}

void PostWebUiState(bool includeForms) {
    if (!g_webView) return;

    std::vector<std::wstring> sounds = g_soundNames;
    if (sounds.size() != g_sounds.size()) {
        sounds.clear();
        sounds.reserve(g_sounds.size());
        for (const auto& path : g_sounds) sounds.push_back(BaseNameNoExt(path));
    }

    std::wstring state = L"{\"type\":\"state\",\"initial\":" +
        JsonBoolean(includeForms) +
        L",\"zoomOnline\":" + JsonBoolean(g_zoomRunning) +
        L",\"zoomPid\":" + JsonQuoted(g_zoomPid ? std::to_wstring(g_zoomPid) : L"") +
        L",\"microphones\":" + JsonArray(GetControlItems(g_hComboMic, true)) +
        L",\"selectedMic\":" + std::to_wstring((int)SendMessageW(g_hComboMic, CB_GETCURSEL, 0, 0)) +
        L",\"sounds\":" + JsonArray(sounds) +
        L",\"selectedSound\":" + std::to_wstring((int)SendMessageW(g_hListSounds, LB_GETCURSEL, 0, 0)) +
        L",\"volumeMin\":" + std::to_wstring(g_gainMin) +
        L",\"volumeMax\":" + std::to_wstring(g_gainMax) +
        L",\"volume\":" + std::to_wstring(g_volume.load()) +
        L",\"bass\":" + std::to_wstring(g_bassBoost.load()) +
        L",\"routePhones\":" + JsonBoolean(g_checkPhones) +
        L",\"routeMic\":" + JsonBoolean(g_checkMic) +
        L",\"autoNextList\":" + JsonBoolean(g_autoNextList) +
        L",\"autoNextRandom\":" + JsonBoolean(g_autoNextRandom) +
        L",\"cableStatus\":" + JsonQuoted(GetControlText(g_hCableStatus)) +
        L",\"nowPlaying\":" + JsonQuoted(g_nowPlayingText) +
        L",\"playing\":" + JsonBoolean(g_playing.load()) +
        L",\"spamStatus\":" + JsonQuoted(GetControlText(g_hStaticSpamInfo)) +
        L",\"spamRunning\":" + JsonBoolean(g_spamRunning.load()) +
        L",\"handStatus\":" + JsonQuoted(GetControlText(g_hStaticHandInfo)) +
        L",\"handRunning\":" + JsonBoolean(g_handRunning.load()) +
        L",\"neonStatus\":" + JsonQuoted(GetControlText(g_hStaticNeonStatus)) +
        L",\"neonAlive\":" + JsonBoolean(g_neonAlive) +
        L",\"neonAudioDevices\":" + JsonArray(g_neonAudioLabels) +
        L",\"selectedAudio\":" + std::to_wstring((int)SendMessageW(g_hComboNeonAudio, CB_GETCURSEL, 0, 0)) +
        L",\"cameras\":" + JsonArray(GetControlItems(g_hListNeonCams, false)) +
        L",\"selectedCamera\":" + std::to_wstring((int)SendMessageW(g_hListNeonCams, LB_GETCURSEL, 0, 0)) +
        L",\"usersStatus\":" + JsonQuoted(GetControlText(g_hStaticUsersInfo)) +
        L",\"users\":" + JsonArray(GetControlItems(g_hListUsers, false)) +
        L",\"settings\":{\"min\":" + std::to_wstring(g_gainMin) +
        L",\"max\":" + std::to_wstring(g_gainMax) + L"}";

    if (includeForms) {
        state += L",\"spamText\":" + JsonQuoted(GetControlText(g_hEditSpamText)) +
            L",\"spamInterval\":" + JsonQuoted(GetControlText(g_hEditSpamMs)) +
            L",\"spamLimit\":" + JsonQuoted(GetControlText(g_hEditSpamLimit));
    }
    state += L"}";

    if (!includeForms && state == g_lastWebUiState) return;
    g_lastWebUiState = state;
    g_webView->PostWebMessageAsJson(state.c_str());
}

void PostWebUiToast(const std::wstring& message) {
    if (!g_webView) return;
    std::wstring json = L"{\"type\":\"toast\",\"text\":" + JsonQuoted(message) + L"}";
    g_webView->PostWebMessageAsJson(json.c_str());
}

std::wstring GetJsonField(const std::wstring& json, const wchar_t* name) {
    std::wstring key = L"\"" + std::wstring(name) + L"\"";
    size_t pos = json.find(key);
    if (pos == std::wstring::npos) return {};
    pos = json.find(L':', pos + key.size());
    if (pos == std::wstring::npos) return {};
    ++pos;
    while (pos < json.size() && iswspace(json[pos])) ++pos;
    if (pos >= json.size()) return {};

    if (json[pos] != L'"') {
        size_t end = json.find_first_of(L",}", pos);
        if (end == std::wstring::npos) end = json.size();
        while (end > pos && iswspace(json[end - 1])) --end;
        return json.substr(pos, end - pos);
    }

    std::wstring out;
    for (size_t i = pos + 1; i < json.size(); ++i) {
        wchar_t ch = json[i];
        if (ch == L'"') return out;
        if (ch != L'\\') {
            out.push_back(ch);
            continue;
        }
        if (++i >= json.size()) break;
        switch (json[i]) {
        case L'"': out.push_back(L'"'); break;
        case L'\\': out.push_back(L'\\'); break;
        case L'/': out.push_back(L'/'); break;
        case L'b': out.push_back(L'\b'); break;
        case L'f': out.push_back(L'\f'); break;
        case L'n': out.push_back(L'\n'); break;
        case L'r': out.push_back(L'\r'); break;
        case L't': out.push_back(L'\t'); break;
        case L'u': {
            if (i + 4 >= json.size()) return out;
            wchar_t hex[5] = { json[i + 1], json[i + 2], json[i + 3], json[i + 4], 0 };
            wchar_t* end = nullptr;
            unsigned long code = wcstoul(hex, &end, 16);
            if (!end || *end) return out;
            out.push_back((wchar_t)code);
            i += 4;
            break;
        }
        default: out.push_back(json[i]); break;
        }
    }
    return out;
}

int JsonInteger(const std::wstring& value, int fallback = 0) {
    if (value.empty()) return fallback;
    wchar_t* end = nullptr;
    long parsed = wcstol(value.c_str(), &end, 10);
    if (!end || end == value.c_str()) return fallback;
    if (parsed < INT_MIN) return INT_MIN;
    if (parsed > INT_MAX) return INT_MAX;
    return (int)parsed;
}

bool JsonIsTrue(const std::wstring& value) {
    return value == L"true" || value == L"1";
}

void SendWebCommand(HWND hwnd, int commandId) {
    SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(commandId, 0), 0);
}

void SetWebTrackbar(HWND hwnd, HWND trackbar, int value) {
    if (!trackbar) return;
    int minValue = (int)SendMessageW(trackbar, TBM_GETRANGEMIN, 0, 0);
    int maxValue = (int)SendMessageW(trackbar, TBM_GETRANGEMAX, 0, 0);
    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;
    SendMessageW(trackbar, TBM_SETPOS, TRUE, value);
    SendMessageW(hwnd, WM_HSCROLL, MAKEWPARAM(TB_THUMBTRACK, value), (LPARAM)trackbar);
}

void ProcessWebUiMessage(HWND hwnd, const std::wstring& json) {
    if (!g_webUiMode || !IsWindow(hwnd)) return;
    std::wstring action = GetJsonField(json, L"action");
    if (action.empty()) return;

    if (action == L"ready") {
        PostWebUiState(true);
        return;
    }
    if (action == L"ui-loaded") {
        if (g_webUiMode && g_webUiLoading) {
            g_webUiLoading = false;
            KillTimer(hwnd, ID_TIMER_LOADING);
            KillTimer(hwnd, ID_TIMER_WEBUI_TIMEOUT);
            HRESULT showResult = g_webController
                ? g_webController->put_IsVisible(TRUE) : E_FAIL;
            if (FAILED(showResult)) {
                WebViewInitializationFailed(hwnd, showResult);
                return;
            }
            if (!SetTimer(hwnd, ID_TIMER_LOADING_REVEAL, 400, nullptr))
                HideLoadingOverlay();
            InvalidateRect(hwnd, nullptr, TRUE);
        }
        return;
    }
    if (action == L"tab") {
        int tab = JsonInteger(GetJsonField(json, L"value"), 0);
        ShowTab(tab);
        if (tab == 3 && g_neonAlive) {
            FillNeonAudioDevices(g_hComboNeonAudio);
            RefreshNeonCameras();
        }
        PostWebUiState();
        return;
    }
    if (action == L"join") {
        SetWindowTextW(g_hEditMeet, GetJsonField(json, L"meeting").c_str());
        SetWindowTextW(g_hEditPass, GetJsonField(json, L"password").c_str());
        SendWebCommand(hwnd, ID_BTN_JOIN);
    } else if (action == L"microphone") {
        int index = JsonInteger(GetJsonField(json, L"index"), -1);
        SendMessageW(g_hComboMic, CB_SETCURSEL, (WPARAM)index, 0);
    } else if (action == L"sound-select") {
        int index = JsonInteger(GetJsonField(json, L"index"), -1);
        SendMessageW(g_hListSounds, LB_SETCURSEL, (WPARAM)index, 0);
    } else if (action == L"volume") {
        SetWebTrackbar(hwnd, g_hSliderVolume,
                       JsonInteger(GetJsonField(json, L"value"), g_volume.load()));
    } else if (action == L"bass") {
        SetWebTrackbar(hwnd, g_hSliderBass,
                       JsonInteger(GetJsonField(json, L"value"), g_bassBoost.load()));
    } else if (action == L"check") {
        std::wstring name = GetJsonField(json, L"name");
        bool checked = JsonIsTrue(GetJsonField(json, L"checked"));
        if (name == L"route-phones") SetUiChecked(g_hChkPhones, checked);
        else if (name == L"route-mic") SetUiChecked(g_hChkMic, checked);
        else if (name == L"auto-next-list") {
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ID_CHK_AUTONEXT_LIST, 0),
                         (LPARAM)g_hChkAutoNextList);
        } else if (name == L"auto-next-random") {
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ID_CHK_AUTONEXT_RAND, 0),
                         (LPARAM)g_hChkAutoNextRand);
        }
    } else if (action == L"browse-sounds") {
        SendWebCommand(hwnd, ID_BTN_BROWSE);
    } else if (action == L"browse-folder") {
        SendWebCommand(hwnd, ID_BTN_BROWSE_FOLDER);
    } else if (action == L"play") {
        SendWebCommand(hwnd, ID_BTN_PLAY);
    } else if (action == L"play-random") {
        SendWebCommand(hwnd, ID_BTN_PLAY_RANDOM);
    } else if (action == L"spam-start") {
        SetWindowTextW(g_hEditSpamText, GetJsonField(json, L"text").c_str());
        SetWindowTextW(g_hEditSpamMs, GetJsonField(json, L"interval").c_str());
        SetWindowTextW(g_hEditSpamLimit, GetJsonField(json, L"limit").c_str());
        SendWebCommand(hwnd, ID_BTN_SPAM_START);
    } else if (action == L"spam-stop") {
        SendWebCommand(hwnd, ID_BTN_SPAM_STOP);
    } else if (action == L"hand-start") {
        SendWebCommand(hwnd, ID_BTN_HAND_START);
    } else if (action == L"hand-stop") {
        SendWebCommand(hwnd, ID_BTN_HAND_STOP);
    } else if (action == L"neon-start") {
        SendWebCommand(hwnd, ID_BTN_NEON_START);
    } else if (action == L"neon-stop") {
        SendWebCommand(hwnd, ID_BTN_NEON_STOP);
    } else if (action == L"neon-create") {
        SendWebCommand(hwnd, ID_BTN_NEON_CREATE);
    } else if (action == L"neon-delete") {
        SendWebCommand(hwnd, ID_BTN_NEON_DELETE);
    } else if (action == L"neon-add-video") {
        SendWebCommand(hwnd, ID_BTN_NEON_ADDVID);
    } else if (action == L"neon-play") {
        SendWebCommand(hwnd, ID_BTN_NEON_PLAY);
    } else if (action == L"neon-pause") {
        SendWebCommand(hwnd, ID_BTN_NEON_PAUSE);
    } else if (action == L"neon-stop-camera") {
        SendWebCommand(hwnd, ID_BTN_NEON_STOPCAM);
    } else if (action == L"neon-audio") {
        int index = JsonInteger(GetJsonField(json, L"index"), -1);
        SendMessageW(g_hComboNeonAudio, CB_SETCURSEL, (WPARAM)index, 0);
        SendMessageW(hwnd, WM_COMMAND,
                     MAKEWPARAM(ID_COMBO_NEON_AUDIO, CBN_SELCHANGE),
                     (LPARAM)g_hComboNeonAudio);
    } else if (action == L"camera-select") {
        int index = JsonInteger(GetJsonField(json, L"index"), -1);
        SendMessageW(g_hListNeonCams, LB_SETCURSEL, (WPARAM)index, 0);
    } else if (action == L"users-refresh") {
        SendWebCommand(hwnd, ID_BTN_USERS_REFRESH);
    } else if (action == L"users-clear") {
        SendWebCommand(hwnd, ID_BTN_USERS_CLEAR);
    } else if (action == L"user-select") {
        int index = JsonInteger(GetJsonField(json, L"index"), -1);
        SendMessageW(g_hListUsers, LB_SETCURSEL, (WPARAM)index, 0);
        SendMessageW(hwnd, WM_COMMAND,
                     MAKEWPARAM(ID_LIST_USERS, LBN_SELCHANGE),
                     (LPARAM)g_hListUsers);
    } else if (action == L"settings-save") {
        int minValue = JsonInteger(GetJsonField(json, L"min"), g_gainMin);
        int maxValue = JsonInteger(GetJsonField(json, L"max"), g_gainMax);
        minValue = (std::max)(0, (std::min)(1000, minValue));
        maxValue = (std::max)(0, (std::min)(1000, maxValue));
        if (minValue > maxValue) std::swap(minValue, maxValue);
        g_gainMin = minValue;
        g_gainMax = maxValue;
        RefreshVolumeSliderRange();
    }

    PostWebUiState();
}

void UpdateWebViewBounds(HWND hwnd) {
    PositionLoadingOverlay(hwnd);
    if (!g_webController) return;
    RECT bounds{};
    GetClientRect(hwnd, &bounds);
    g_webController->put_Bounds(bounds);
}

void WebViewInitializationFailed(HWND hwnd, HRESULT error) {
    if (!g_webUiMode) return;
    g_webUiMode = false;
    g_webUiLoading = false;
    KillTimer(hwnd, ID_TIMER_LOADING);
    KillTimer(hwnd, ID_TIMER_WEBUI_TIMEOUT);
    KillTimer(hwnd, ID_TIMER_LOADING_REVEAL);
    KillTimer(hwnd, ID_TIMER_LOADING_FADE);
    HideLoadingOverlay();
    g_webView.Reset();
    if (g_webController) {
        g_webController->Close();
        g_webController.Reset();
    }
    SetDirectChildrenVisibility(hwnd, SW_SHOW);
    ShowTab(g_activeTab);
    UpdateTabsVisibility();
    wchar_t message[256]{};
    swprintf(message, 256,
        L"Не удалось запустить встроенный web-интерфейс (0x%08lX).\n"
        L"Показан предыдущий интерфейс. Проверь наличие WebView2 Runtime.",
        (unsigned long)error);
    MessageBoxW(hwnd, message, L"Zoom Breaker", MB_OK | MB_ICONWARNING);
}

template <typename Interface>
class WebViewHandlerBase : public Interface {
public:
    virtual ~WebViewHandlerBase() = default;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object,
                                             const IID& interfaceId) {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, interfaceId)) {
            *object = static_cast<Interface*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() { return (ULONG)InterlockedIncrement(&refs_); }
    ULONG STDMETHODCALLTYPE Release() {
        ULONG value = (ULONG)InterlockedDecrement(&refs_);
        if (!value) delete this;
        return value;
    }
protected:
    volatile LONG refs_ = 1;
};

class WebMessageReceivedHandler final : public WebViewHandlerBase<ICoreWebView2WebMessageReceivedEventHandler> {
public:
    explicit WebMessageReceivedHandler(HWND hwnd) : hwnd_(hwnd) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        return WebViewHandlerBase<ICoreWebView2WebMessageReceivedEventHandler>::QueryInterface(
            riid, object, IID_ICoreWebView2WebMessageReceivedEventHandler);
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return WebViewHandlerBase<ICoreWebView2WebMessageReceivedEventHandler>::AddRef(); }
    ULONG STDMETHODCALLTYPE Release() override { return WebViewHandlerBase<ICoreWebView2WebMessageReceivedEventHandler>::Release(); }
    HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2*,
        ICoreWebView2WebMessageReceivedEventArgs* args) override {
        if (!args) return E_POINTER;
        if (!g_webUiMode || !IsWindow(hwnd_)) return S_OK;
        LPWSTR message = nullptr;
        HRESULT hr = args->get_WebMessageAsJson(&message);
        if (SUCCEEDED(hr) && message) {
            ProcessWebUiMessage(hwnd_, message);
            CoTaskMemFree(message);
        }
        return S_OK;
    }
private:
    HWND hwnd_;
};

class WebViewControllerHandler final : public WebViewHandlerBase<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler> {
public:
    explicit WebViewControllerHandler(HWND hwnd) : hwnd_(hwnd) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        return WebViewHandlerBase<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>::QueryInterface(
            riid, object, IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler);
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return WebViewHandlerBase<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>::AddRef(); }
    ULONG STDMETHODCALLTYPE Release() override { return WebViewHandlerBase<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>::Release(); }
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT error, ICoreWebView2Controller* controller) override {
        if (!g_webUiMode || !IsWindow(hwnd_)) {
            if (controller) controller->Close();
            return S_OK;
        }
        if (FAILED(error) || !controller) {
            WebViewInitializationFailed(hwnd_, FAILED(error) ? error : E_FAIL);
            return S_OK;
        }

        g_webController = controller;
        ComPtr<ICoreWebView2Controller2> controller2;
        if (SUCCEEDED(g_webController->QueryInterface(
                IID_ICoreWebView2Controller2,
                reinterpret_cast<void**>(controller2.GetAddressOf()))) && controller2) {
            COREWEBVIEW2_COLOR background{};
            background.A = 255;
            background.R = 13;
            background.G = 12;
            background.B = 19;
            controller2->put_DefaultBackgroundColor(background);
        }
        HRESULT hr = g_webController->get_CoreWebView2(g_webView.GetAddressOf());
        if (FAILED(hr) || !g_webView) {
            WebViewInitializationFailed(hwnd_, FAILED(hr) ? hr : E_FAIL);
            return S_OK;
        }

        UpdateWebViewBounds(hwnd_);
        HRESULT visibilityResult = g_webController->put_IsVisible(TRUE);
        if (FAILED(visibilityResult)) {
            WebViewInitializationFailed(hwnd_, visibilityResult);
            return S_OK;
        }
        ComPtr<ICoreWebView2Settings> settings;
        if (SUCCEEDED(g_webView->get_Settings(settings.GetAddressOf())) && settings) {
            settings->put_AreDefaultContextMenusEnabled(FALSE);
            settings->put_IsStatusBarEnabled(FALSE);
            settings->put_AreDevToolsEnabled(FALSE);
        }

        auto* messageHandler = new WebMessageReceivedHandler(hwnd_);
        hr = g_webView->add_WebMessageReceived(messageHandler, &g_webMessageToken);
        messageHandler->Release();
        if (FAILED(hr)) {
            WebViewInitializationFailed(hwnd_, hr);
            return S_OK;
        }

        const std::wstring pagePath = GetBundledPath(L"ui\\app_ui.html");
        if (pagePath.empty() || pagePath.size() >= MAX_PATH) {
            WebViewInitializationFailed(hwnd_, E_FAIL);
            return S_OK;
        }
        wchar_t pageUrl[MAX_PATH * 4]{};
        DWORD urlLength = ARRAYSIZE(pageUrl);
        hr = UrlCreateFromPathW(pagePath.data(), pageUrl, &urlLength, 0);
        if (FAILED(hr)) {
            WebViewInitializationFailed(hwnd_, hr);
            return S_OK;
        }
        hr = g_webView->Navigate(pageUrl);
        if (FAILED(hr)) WebViewInitializationFailed(hwnd_, hr);
        return S_OK;
    }
private:
    HWND hwnd_;
};

class WebViewEnvironmentHandler final : public WebViewHandlerBase<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler> {
public:
    explicit WebViewEnvironmentHandler(HWND hwnd) : hwnd_(hwnd) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        return WebViewHandlerBase<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>::QueryInterface(
            riid, object, IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler);
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return WebViewHandlerBase<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>::AddRef(); }
    ULONG STDMETHODCALLTYPE Release() override { return WebViewHandlerBase<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>::Release(); }
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT error, ICoreWebView2Environment* environment) override {
        if (!g_webUiMode || !IsWindow(hwnd_)) return S_OK;
        if (FAILED(error) || !environment) {
            WebViewInitializationFailed(hwnd_, FAILED(error) ? error : E_FAIL);
            return S_OK;
        }
        auto* controllerHandler = new WebViewControllerHandler(hwnd_);
        HRESULT hr = environment->CreateCoreWebView2Controller(hwnd_, controllerHandler);
        controllerHandler->Release();
        if (FAILED(hr)) WebViewInitializationFailed(hwnd_, hr);
        return S_OK;
    }
private:
    HWND hwnd_;
};

void CreateWebView(HWND hwnd) {
    if (!g_webUiMode) return;

    const std::wstring loaderPath = GetBundledPath(L"WebView2Loader.dll");
    if (loaderPath.empty()) {
        WebViewInitializationFailed(hwnd, E_FAIL);
        return;
    }
    g_webViewLoader = LoadLibraryW(loaderPath.c_str());
    if (!g_webViewLoader) {
        WebViewInitializationFailed(hwnd, HRESULT_FROM_WIN32(GetLastError()));
        return;
    }

    using CreateEnvironmentFn = HRESULT (STDAPICALLTYPE *)(
        PCWSTR, PCWSTR, ICoreWebView2EnvironmentOptions*,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*);
    auto createEnvironment = (CreateEnvironmentFn)GetProcAddress(
        g_webViewLoader, "CreateCoreWebView2EnvironmentWithOptions");
    if (!createEnvironment) {
        WebViewInitializationFailed(hwnd, HRESULT_FROM_WIN32(GetLastError()));
        return;
    }

    wchar_t dataPath[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
                                nullptr, SHGFP_TYPE_CURRENT, dataPath))) {
        WebViewInitializationFailed(hwnd, E_FAIL);
        return;
    }
    PathAppendW(dataPath, L"ZoomBreaker");
    CreateDirectoryW(dataPath, nullptr);
    PathAppendW(dataPath, L"WebView2");
    CreateDirectoryW(dataPath, nullptr);

    auto* handler = new WebViewEnvironmentHandler(hwnd);
    HRESULT hr = createEnvironment(nullptr, dataPath, nullptr, handler);
    handler->Release();
    if (FAILED(hr)) WebViewInitializationFailed(hwnd, hr);
}

 
 
 
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_CREATE: {
        g_hCtrlFont = CreateFontW(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                                  DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);

        g_hTabScrollL = CreateStyledButton(hwnd, L"◀", ID_TAB_SCROLL_L,
                                           18, 112, 24, 40, false);
        g_hTabScrollR = CreateStyledButton(hwnd, L"▶", ID_TAB_SCROLL_R,
                                           438, 112, 24, 40, false);

        g_hTabJoin     = CreateStyledButton(hwnd, L"JOIN",     ID_TAB_JOIN,
                                            46, 112, 94, 40, false, true);
        g_hTabSoundpad = CreateStyledButton(hwnd, L"SOUNDPAD", ID_TAB_SOUNDPAD,
                                            144, 112, 94, 40, false, false);
        g_hTabSpam     = CreateStyledButton(hwnd, L"SPAM",     ID_TAB_SPAM,
                                            242, 112, 94, 40, false, false);
        g_hTabVideo    = CreateStyledButton(hwnd, L"VIDEO",    ID_TAB_VIDEO,
                                            340, 112, 94, 40, false, false);
        g_hTabUsers    = CreateStyledButton(hwnd, L"USERS",    ID_TAB_USERS,
                                            438, 112, 94, 40, false, false);

        g_hBtnSettings = CreateStyledButton(hwnd, L"⚙", ID_BTN_SETTINGS,
                                             404, 15, 30, 30, false);

         
        g_hEditMeet = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 192, 440, 30, hwnd, (HMENU)ID_EDIT_MEETING,
            GetModuleHandleW(nullptr), nullptr);

        g_hEditPass = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD,
            20, 256, 440, 30, hwnd, (HMENU)ID_EDIT_PASSWORD,
            GetModuleHandleW(nullptr), nullptr);

        g_hBtnJoin = CreateStyledButton(hwnd, L"JOIN MEETING", ID_BTN_JOIN,
                                        20, 302, 440, 46, true);

         
        g_hComboMic = CreateWindowExW(0, L"COMBOBOX", L"",
            WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS |
            CBS_OWNERDRAWFIXED | WS_VSCROLL,
            20, 192, 440, 200, hwnd, (HMENU)ID_COMBO_MIC,
            GetModuleHandleW(nullptr), nullptr);
        SendMessageW(g_hComboMic, CB_SETITEMHEIGHT, (WPARAM)-1, 26);
        SendMessageW(g_hComboMic, CB_SETITEMHEIGHT, 0, 24);
        SetWindowSubclass(g_hComboMic, ComboSubclassProc, 1,
                          (DWORD_PTR)new ComboState());

        g_hListSounds = CreateWindowExW(0, L"LISTBOX", L"",
            WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_HASSTRINGS,
            20, 256, 440, 160, hwnd, (HMENU)ID_LIST_SOUNDS,
            GetModuleHandleW(nullptr), nullptr);

        g_hBtnBrowse = CreateStyledButton(hwnd, L"Файлы", ID_BTN_BROWSE,
                                          20, 426, 213, 34, false);
        g_hBtnBrowseFolder = CreateStyledButton(hwnd, L"Папка", ID_BTN_BROWSE_FOLDER,
                                                247, 426, 213, 34, false);

        g_hBtnPlay = CreateStyledButton(hwnd, L"Играть", ID_BTN_PLAY,
                                        20, 466, 213, 40, true);
        g_hBtnPlayRandom = CreateStyledButton(hwnd, L"Рандом", ID_BTN_PLAY_RANDOM,
                                              247, 466, 213, 40, false);

        g_hSliderVolume = CreateWindowExW(0, TRACKBAR_CLASSW, L"",
            WS_CHILD | TBS_HORZ | TBS_NOTICKS,
            20, 528, 380, 24, hwnd, (HMENU)ID_SLIDER_VOLUME,
            GetModuleHandleW(nullptr), nullptr);
        SetWindowSubclass(g_hSliderVolume, TrackbarSubclassProc, 1, 0);

        g_hStaticVolume = CreateWindowExW(0, L"STATIC", L"100%",
            WS_CHILD | SS_CENTER,
            405, 530, 55, 22, hwnd, (HMENU)ID_STATIC_VOLUME,
            GetModuleHandleW(nullptr), nullptr);

        g_hSliderBass = CreateWindowExW(0, TRACKBAR_CLASSW, L"",
            WS_CHILD | TBS_HORZ | TBS_NOTICKS,
            20, 570, 380, 24, hwnd, (HMENU)ID_SLIDER_BASS,
            GetModuleHandleW(nullptr), nullptr);
        SetWindowSubclass(g_hSliderBass, TrackbarSubclassProc, 1, 0);
        SendMessageW(g_hSliderBass, TBM_SETRANGE, TRUE, MAKELPARAM(0, 300));
        SendMessageW(g_hSliderBass, TBM_SETPOS, TRUE, 0);

        g_hStaticBass = CreateWindowExW(0, L"STATIC", L"0%",
            WS_CHILD | SS_CENTER,
            405, 572, 55, 22, hwnd, (HMENU)ID_STATIC_BASS,
            GetModuleHandleW(nullptr), nullptr);

        g_hChkAutoNextList = CreateWindowExW(0, L"BUTTON", L"  Авто-следующий",
            WS_CHILD | BS_OWNERDRAW,
            20, 602, 213, 22, hwnd, (HMENU)ID_CHK_AUTONEXT_LIST,
            GetModuleHandleW(nullptr), nullptr);

        g_hChkAutoNextRand = CreateWindowExW(0, L"BUTTON", L"  Авто-рандом",
            WS_CHILD | BS_OWNERDRAW,
            247, 602, 213, 22, hwnd, (HMENU)ID_CHK_AUTONEXT_RAND,
            GetModuleHandleW(nullptr), nullptr);

        g_hNowPlaying = CreateWindowExW(0, L"STATIC", L"—",
            WS_CHILD | SS_LEFT | SS_ENDELLIPSIS,
            20, 630, 440, 20, hwnd, (HMENU)ID_STATIC_NOW_PLAYING,
            GetModuleHandleW(nullptr), nullptr);

        g_hChkPhones = CreateWindowExW(0, L"BUTTON", L"  В наушники",
            WS_CHILD | BS_OWNERDRAW,
            20, 656, 210, 24, hwnd, (HMENU)ID_CHK_HEADPHONES,
            GetModuleHandleW(nullptr), nullptr);

        g_hChkMic = CreateWindowExW(0, L"BUTTON", L"  В микрофон",
            WS_CHILD | BS_OWNERDRAW,
            250, 656, 210, 24, hwnd, (HMENU)ID_CHK_MICROPHONE,
            GetModuleHandleW(nullptr), nullptr);

        g_hCableStatus = CreateWindowExW(0, L"STATIC", L"",
            WS_CHILD | SS_LEFT,
            20, 682, 440, 20, hwnd, (HMENU)ID_CABLE_STATUS,
            GetModuleHandleW(nullptr), nullptr);

         
        g_hEditSpamText = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL |
            WS_VSCROLL | ES_WANTRETURN,
            20, 192, 440, 80, hwnd, (HMENU)ID_EDIT_SPAM_TEXT,
            GetModuleHandleW(nullptr), nullptr);

        g_hEditSpamMs = CreateWindowExW(0, L"EDIT", L"2000",
            WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
            20, 280, 210, 26, hwnd, (HMENU)ID_EDIT_SPAM_MS,
            GetModuleHandleW(nullptr), nullptr);

        g_hEditSpamLimit = CreateWindowExW(0, L"EDIT", L"0",
            WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
            250, 280, 210, 26, hwnd, (HMENU)ID_EDIT_SPAM_LIMIT,
            GetModuleHandleW(nullptr), nullptr);

        g_hBtnSpamStart = CreateStyledButton(hwnd, L"START SPAM", ID_BTN_SPAM_START,
                                             20, 316, 210, 38, true);
        g_hBtnSpamStop  = CreateStyledButton(hwnd, L"STOP", ID_BTN_SPAM_STOP,
                                             250, 316, 210, 38, false);

        g_hStaticSpamInfo = CreateWindowExW(0, L"STATIC", L"Готов.",
            WS_CHILD | SS_LEFT,
            20, 360, 440, 22, hwnd, (HMENU)ID_STATIC_SPAM_INFO,
            GetModuleHandleW(nullptr), nullptr);

         
        g_hBtnHandStart = CreateStyledButton(hwnd, L"START HAND", ID_BTN_HAND_START,
                                             20, 448, 213, 34, true);
        g_hBtnHandStop  = CreateStyledButton(hwnd, L"STOP HAND", ID_BTN_HAND_STOP,
                                             247, 448, 213, 34, false);

        g_hStaticHandInfo = CreateWindowExW(0, L"STATIC", L"Готов.",
            WS_CHILD | SS_LEFT,
            20, 486, 440, 22, hwnd, (HMENU)ID_STATIC_HAND_INFO,
            GetModuleHandleW(nullptr), nullptr);

         
        g_hBtnNeonStart = CreateStyledButton(hwnd, L"START SERVICE", ID_BTN_NEON_START,
                                             20, 192, 210, 36, true);
        g_hBtnNeonStop  = CreateStyledButton(hwnd, L"STOP SERVICE", ID_BTN_NEON_STOP,
                                             250, 192, 210, 36, false);

        g_hListNeonCams = CreateWindowExW(0, L"LISTBOX", L"",
            WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_HASSTRINGS,
            20, 236, 440, 140, hwnd, (HMENU)ID_LIST_NEON_CAMS,
            GetModuleHandleW(nullptr), nullptr);

        g_hBtnNeonCreate = CreateStyledButton(hwnd, L"+ Камера", ID_BTN_NEON_CREATE,
                                              20, 384, 140, 36, false);
        g_hBtnNeonDelete = CreateStyledButton(hwnd, L"− Камера", ID_BTN_NEON_DELETE,
                                              170, 384, 140, 36, false);
        g_hBtnNeonAddVid = CreateStyledButton(hwnd, L"+ Видео", ID_BTN_NEON_ADDVID,
                                              320, 384, 140, 36, false);

        g_hComboNeonAudio = CreateWindowExW(0, L"COMBOBOX", L"",
            WS_CHILD | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |
            CBS_HASSTRINGS | WS_VSCROLL,
            20, 430, 440, 220, hwnd, (HMENU)ID_COMBO_NEON_AUDIO,
            GetModuleHandleW(nullptr), nullptr);
        SetWindowSubclass(g_hComboNeonAudio, ComboSubclassProc, 1,
                          (DWORD_PTR)new ComboState());

        SendMessageW(g_hComboNeonAudio, CB_SETITEMHEIGHT, (WPARAM)-1, 22);
        SendMessageW(g_hComboNeonAudio, CB_SETITEMHEIGHT, 0, 22);

        g_hBtnNeonPlay    = CreateStyledButton(hwnd, L"PLAY",  ID_BTN_NEON_PLAY,
                                               20, 472, 140, 36, false);
        g_hBtnNeonPause   = CreateStyledButton(hwnd, L"PAUSE", ID_BTN_NEON_PAUSE,
                                               170, 472, 140, 36, false);
        g_hBtnNeonStopCam = CreateStyledButton(hwnd, L"STOP",  ID_BTN_NEON_STOPCAM,
                                               320, 472, 140, 36, false);

        g_hStaticNeonStatus = CreateWindowExW(0, L"STATIC", L"Neon: не запущен",
            WS_CHILD | SS_LEFT,
            20, 518, 440, 20, hwnd, (HMENU)ID_STATIC_NEON_STATUS,
            GetModuleHandleW(nullptr), nullptr);

         
        g_hListUsers = CreateWindowExW(0, L"LISTBOX", L"",
            WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_HASSTRINGS,
            20, 192, 440, 400, hwnd, (HMENU)ID_LIST_USERS,
            GetModuleHandleW(nullptr), nullptr);

        g_hBtnUsersRefresh = CreateStyledButton(hwnd, L"Обновить", ID_BTN_USERS_REFRESH,
                                                20, 604, 213, 40, true);
        g_hBtnUsersClear   = CreateStyledButton(hwnd, L"Очистить", ID_BTN_USERS_CLEAR,
                                                247, 604, 213, 40, false);

        g_hStaticUsersInfo = CreateWindowExW(0, L"STATIC", L"Готов.",
            WS_CHILD | SS_LEFT,
            20, 652, 440, 22, hwnd, (HMENU)ID_STATIC_USERS_INFO,
            GetModuleHandleW(nullptr), nullptr);

        SendMessageW(g_hEditMeet,      EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));
        SendMessageW(g_hEditPass,      EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));
        SendMessageW(g_hEditSpamText,  EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));
        SendMessageW(g_hEditSpamMs,    EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(9, 9));
        SendMessageW(g_hEditSpamLimit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(9, 9));

         
        SendMessageW(g_hEditMeet,       WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hEditPass,       WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hChkPhones,      WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hChkMic,         WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hChkAutoNextList,WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hChkAutoNextRand,WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hComboMic,       WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hListSounds,     WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hCableStatus,    WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hEditSpamText,   WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hEditSpamMs,     WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hEditSpamLimit,  WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hStaticSpamInfo, WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hStaticHandInfo, WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hListNeonCams,   WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hComboNeonAudio, WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hStaticNeonStatus,WM_SETFONT,(WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hNowPlaying,     WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hStaticVolume,   WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hStaticBass,     WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hListUsers,      WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);
        SendMessageW(g_hStaticUsersInfo,WM_SETFONT, (WPARAM)g_hCtrlFont, TRUE);

        SetWindowSubclass(g_hChkPhones, CheckBoxSubclassProc, 1,
                          (DWORD_PTR)new CheckBoxState());
        SetWindowSubclass(g_hChkMic, CheckBoxSubclassProc, 1,
                          (DWORD_PTR)new CheckBoxState());
        SetWindowSubclass(g_hChkAutoNextList, CheckBoxSubclassProc, 1,
                          (DWORD_PTR)new CheckBoxState());
        SetWindowSubclass(g_hChkAutoNextRand, CheckBoxSubclassProc, 1,
                          (DWORD_PTR)new CheckBoxState());

        SetUiChecked(g_hChkPhones, false);
        SetUiChecked(g_hChkMic, true);

        FillMicrophones(g_hComboMic);

        g_cableDeviceId = FindCableInputDevice();
        UpdateCableStatus();

        RefreshVolumeSliderRange();

        RefreshStatus(hwnd);
        ShowTab(0);
        SetDirectChildrenVisibility(hwnd, SW_HIDE);
        UpdateTabsVisibility();
        InvalidateRect(hwnd, nullptr, TRUE);

        g_hLoadingOverlay = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_LAYERED,
            L"ZoomBreakerLoadingOverlay", L"",
            WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
            GetModuleHandleW(nullptr), nullptr);
        if (!g_hLoadingOverlay ||
            !SetLayeredWindowAttributes(g_hLoadingOverlay, 0, 255, LWA_ALPHA)) {
            DWORD error = GetLastError();
            if (g_hLoadingOverlay) {
                DestroyWindow(g_hLoadingOverlay);
                g_hLoadingOverlay = nullptr;
            }
            WebViewInitializationFailed(hwnd,
                error ? HRESULT_FROM_WIN32(error) : E_FAIL);
            return 0;
        }
        PositionLoadingOverlay(hwnd);
        ShowWindow(g_hLoadingOverlay, SW_SHOWNOACTIVATE);

        SetTimer(hwnd, ID_TIMER_STATUS, 1000, nullptr);
        g_webUiLoadStart = GetTickCount64();
        SetTimer(hwnd, ID_TIMER_LOADING, 50, nullptr);
        SetTimer(hwnd, ID_TIMER_WEBUI_TIMEOUT, 500, nullptr);
        CreateWebView(hwnd);
        return 0;
    }

    case WM_SIZE:
        UpdateWebViewBounds(hwnd);
        return 0;

    case WM_SHOWWINDOW:
        if (wp && g_webUiMode && g_webUiLoading) {
            PositionLoadingOverlay(hwnd);
        } else if (!wp) {
            if (!g_webUiLoading) {
                KillTimer(hwnd, ID_TIMER_LOADING_REVEAL);
                KillTimer(hwnd, ID_TIMER_LOADING_FADE);
            }
            HideLoadingOverlay();
        }
        break;

    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wp;
        SetBkColor(hdc, C_INPUT);
        SetTextColor(hdc, C_TEXT);
        static HBRUSH br = CreateSolidBrush(C_INPUT);
        return (LRESULT)br;
    }

    case WM_CTLCOLORLISTBOX: {
        HDC hdc = (HDC)wp;
        SetBkColor(hdc, C_INPUT);
        SetTextColor(hdc, C_TEXT);
        static HBRUSH br = CreateSolidBrush(C_INPUT);
        return (LRESULT)br;
    }

    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wp;
        HWND h = (HWND)lp;
        if (h == g_hChkPhones || h == g_hChkMic ||
            h == g_hChkAutoNextList || h == g_hChkAutoNextRand) {
            SetBkColor(hdc, C_PANEL);
            SetTextColor(hdc, C_TEXT_SUB);
            static HBRUSH br = CreateSolidBrush(C_PANEL);
            return (LRESULT)br;
        }
        break;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wp;
        HWND h = (HWND)lp;
        SetBkColor(hdc, C_PANEL);
        if (h == g_hCableStatus) {
            SetTextColor(hdc, g_cableDeviceId < 0 ? C_ERR : C_OK);
        } else if (h == g_hStaticNeonStatus) {
            SetTextColor(hdc, g_neonAlive ? C_OK : C_ERR);
        } else if (h == g_hStaticHandInfo) {
            SetTextColor(hdc, g_handRunning.load() ? C_OK : C_TEXT_DIM);
        } else if (h == g_hNowPlaying) {
            if (g_nowPlayingText.find(L"Ошибка") != std::wstring::npos)
                SetTextColor(hdc, C_ERR);
            else if (g_playing.load())
                SetTextColor(hdc, C_OK);
            else
                SetTextColor(hdc, C_TEXT_DIM);
        } else if (h == g_hStaticVolume) {
            int v = g_volume.load();
            if (v > 100)      SetTextColor(hdc, C_ERR);
            else if (v >= 80) SetTextColor(hdc, C_OK);
            else              SetTextColor(hdc, C_TEXT_DIM);
        } else if (h == g_hStaticBass) {
            int v = g_bassBoost.load();
            if (v > 150)      SetTextColor(hdc, C_ERR);
            else if (v > 0)   SetTextColor(hdc, C_OK);
            else              SetTextColor(hdc, C_TEXT_DIM);
        } else if (h == g_hStaticUsersInfo) {
            SetTextColor(hdc, C_TEXT_DIM);
        } else {
            SetTextColor(hdc, C_TEXT);
        }
        static HBRUSH br = CreateSolidBrush(C_PANEL);
        return (LRESULT)br;
    }

    case WM_HSCROLL: {
        if ((HWND)lp == g_hSliderVolume) {
            InvalidateRect(g_hSliderVolume, nullptr, FALSE);
            int pos = (int)SendMessageW(g_hSliderVolume, TBM_GETPOS, 0, 0);
            g_gainCur = pos;
            g_volume = pos;
            g_gain = pos / 100.0f;
            wchar_t buf[16];
            swprintf(buf, 16, L"%d%%", pos);
            SetTextIfChanged(g_hStaticVolume, buf);
            InvalidateRect(g_hStaticVolume, nullptr, TRUE);
        } else if ((HWND)lp == g_hSliderBass) {
            InvalidateRect(g_hSliderBass, nullptr, FALSE);
            int pos = (int)SendMessageW(g_hSliderBass, TBM_GETPOS, 0, 0);
            g_bassBoost = pos;
            wchar_t buf[16];
            swprintf(buf, 16, L"%d%%", pos);
            SetTextIfChanged(g_hStaticBass, buf);
            InvalidateRect(g_hStaticBass, nullptr, TRUE);
        }
        PostWebUiState();
        return 0;
    }

    case WM_MEASUREITEM: {
        LPMEASUREITEMSTRUCT mis = (LPMEASUREITEMSTRUCT)lp;
        if (mis->CtlType == ODT_COMBOBOX) {
            mis->itemHeight = mis->CtlID == ID_COMBO_MIC ? 24 : 22;
            return TRUE;
        }
        break;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;

        if (dis->CtlType == ODT_BUTTON) {
            if (IsUiCheckboxId(dis->CtlID)) {
                DrawOwnerCheckbox(dis);
                return TRUE;
            }
            BtnState* st = (BtnState*)GetWindowLongPtrW(dis->hwndItem, GWLP_USERDATA);
            DrawOwnerButton(dis, st);
            return TRUE;
        }

        if (dis->CtlType == ODT_COMBOBOX) {
            DrawComboItem(dis);
            return TRUE;
        }

        break;
    }

    case WM_APP_NOWPLAYING:
        if (lp == 0) {
            if (g_hNowPlaying) InvalidateRect(g_hNowPlaying, nullptr, TRUE);
        }
        PostWebUiState();
        return 0;

    case WM_APP_PLAY_NEXT: {
        bool wasRandom = (lp == 1);
        TriggerNextAuto(hwnd, wasRandom);
        if (g_hNowPlaying) InvalidateRect(g_hNowPlaying, nullptr, TRUE);
        PostWebUiState();
        return 0;
    }

    case WM_PAINT:
        PaintMain(hwnd);
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (wp == ID_TIMER_STATUS) {
            RefreshStatus(hwnd);
            PostWebUiState();
        }
        else if (wp == ID_TIMER_LOADING && g_webUiLoading) {
            g_loadingAngle = (g_loadingAngle + 14) % 360;
            InvalidateLoadingSpinner(hwnd);
        }
        else if (wp == ID_TIMER_WEBUI_TIMEOUT && g_webUiLoading &&
                 GetTickCount64() - g_webUiLoadStart >= 30000) {
            WebViewInitializationFailed(hwnd, HRESULT_FROM_WIN32(ERROR_TIMEOUT));
        }
        else if (wp == ID_TIMER_LOADING_REVEAL) {
            KillTimer(hwnd, ID_TIMER_LOADING_REVEAL);
            g_loadingAlpha = 255;
            if (!g_hLoadingOverlay || !IsWindow(g_hLoadingOverlay) ||
                !SetLayeredWindowAttributes(g_hLoadingOverlay, 0,
                                            g_loadingAlpha, LWA_ALPHA) ||
                !SetTimer(hwnd, ID_TIMER_LOADING_FADE, 16, nullptr)) {
                HideLoadingOverlay();
            }
        }
        else if (wp == ID_TIMER_LOADING_FADE) {
            g_loadingAlpha = (g_loadingAlpha > 17)
                ? (BYTE)(g_loadingAlpha - 17) : 0;
            if (!g_hLoadingOverlay || !IsWindow(g_hLoadingOverlay) ||
                !SetLayeredWindowAttributes(g_hLoadingOverlay, 0,
                                            g_loadingAlpha, LWA_ALPHA) ||
                g_loadingAlpha == 0) {
                KillTimer(hwnd, ID_TIMER_LOADING_FADE);
                HideLoadingOverlay();
            }
        }
        else if (wp == ID_TIMER_ANIMATION) UpdateTabAnimations(hwnd);
        return 0;

    case WM_COMMAND: {
        if (HIWORD(wp) == BN_CLICKED && IsUiCheckboxId(LOWORD(wp))) {
            HWND checkbox = (HWND)lp;
            SetUiChecked(checkbox, !IsUiChecked(checkbox));
        }

        switch (LOWORD(wp)) {

        case ID_BTN_SETTINGS: {
            int mn = g_gainMin;
            int mx = g_gainMax;
            if (ShowSettingsDialog(hwnd, mn, mx)) {
                g_gainMin = mn;
                g_gainMax = mx;
                RefreshVolumeSliderRange();
            }
            break;
        }

        case ID_TAB_SCROLL_L:
            ScrollTabs(-1);
            break;

        case ID_TAB_SCROLL_R:
            ScrollTabs(1);
            break;

        case ID_TAB_JOIN:
            ShowTab(0);
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_TAB_SOUNDPAD:
            ShowTab(1);
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_TAB_SPAM:
            ShowTab(2);
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_TAB_VIDEO:
            ShowTab(3);
            if (g_neonAlive) {
                FillNeonAudioDevices(g_hComboNeonAudio);
                RefreshNeonCameras();
            }
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_TAB_USERS:
            ShowTab(4);
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_BTN_USERS_REFRESH: {
            SendMessageW(g_hListUsers, LB_RESETCONTENT, 0, 0);

            auto names = GetZoomParticipants();

            if (names.empty()) {
                SetTextIfChanged(g_hStaticUsersInfo,
                                 L"Участники не найдены. Открой окно Zoom «Участники».");
            } else {
                for (const auto& n : names) {
                    SendMessageW(g_hListUsers, LB_ADDSTRING, 0, (LPARAM)n.c_str());
                }
                wchar_t buf[64];
                swprintf(buf, 64, L"Найдено участников: %d", (int)names.size());
                SetTextIfChanged(g_hStaticUsersInfo, buf);
            }
            InvalidateRect(g_hStaticUsersInfo, nullptr, TRUE);
            break;
        }

        case ID_BTN_USERS_CLEAR: {
            SendMessageW(g_hListUsers, LB_RESETCONTENT, 0, 0);
            SetTextIfChanged(g_hStaticUsersInfo, L"Список очищен.");
            InvalidateRect(g_hStaticUsersInfo, nullptr, TRUE);
            break;
        }

        case ID_LIST_USERS: {
            if (HIWORD(wp) == LBN_SELCHANGE) {
                int sel = (int)SendMessageW(g_hListUsers, LB_GETCURSEL, 0, 0);
                if (sel < 0) break;

                int len = (int)SendMessageW(g_hListUsers, LB_GETTEXTLEN, sel, 0);
                if (len <= 0) break;

                std::wstring name(len + 1, L'\0');
                SendMessageW(g_hListUsers, LB_GETTEXT, sel, (LPARAM)&name[0]);
                name.resize(len);

                if (OpenClipboard(hwnd)) {
                    EmptyClipboard();
                    size_t bytes = (name.size() + 1) * sizeof(wchar_t);
                    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
                    if (hMem) {
                        void* p = GlobalLock(hMem);
                        memcpy(p, name.c_str(), bytes);
                        GlobalUnlock(hMem);
                        SetClipboardData(CF_UNICODETEXT, hMem);
                    }
                    CloseClipboard();
                }

                ShowCopyToast(hwnd, L"Скопировано: " + name);
            }
            break;
        }

        case ID_BTN_JOIN: {
            wchar_t buf[256] = {0};
            GetWindowTextW(g_hEditMeet, buf, 255);
            std::wstring meet = buf;
            GetWindowTextW(g_hEditPass, buf, 255);
            std::wstring pass = buf;

            if (!g_zoomRunning) {
                MessageBoxW(hwnd, L"Zoom.exe не запущен.",
                            L"Ошибка", MB_OK | MB_ICONWARNING);
                break;
            }

            meet = StripSpaces(meet);
            pass = StripSpaces(pass);

            if (meet.empty()) {
                MessageBoxW(hwnd, L"Введи Meeting ID.",
                            L"Ошибка", MB_OK | MB_ICONWARNING);
                break;
            }

            std::wstring url = L"zoommtg://zoom.us/join?action=join&confno=" + meet;
            if (!pass.empty()) url += L"&pwd=" + pass;

            ShellExecuteW(nullptr, L"open", url.c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            break;
        }

        case ID_BTN_BROWSE:
            BrowseSounds(hwnd);
            break;

        case ID_BTN_BROWSE_FOLDER:
            BrowseSoundFolder(hwnd);
            break;

        case ID_BTN_PLAY: {
            int sel = (int)SendMessageW(g_hListSounds, LB_GETCURSEL, 0, 0);
            if (sel < 0 || sel >= (int)g_sounds.size()) {
                MessageBoxW(hwnd, L"Выбери звук в списке.",
                            L"Soundpad", MB_OK | MB_ICONINFORMATION);
                break;
            }
            bool toPhones = IsUiChecked(g_hChkPhones);
            bool toMic    = IsUiChecked(g_hChkMic);

            if (!toPhones && !toMic) {
                MessageBoxW(hwnd, L"Включи хотя бы один выход.",
                            L"Soundpad", MB_OK | MB_ICONWARNING);
                break;
            }

            g_autoNextRandom = false;
            SetUiChecked(g_hChkAutoNextRand, false);

            PlaySoundFileUI(hwnd, g_sounds[sel], toPhones, toMic, false);
            break;
        }

        case ID_BTN_PLAY_RANDOM: {
            if (g_sounds.empty()) {
                MessageBoxW(hwnd, L"Список звуков пуст.",
                            L"Soundpad", MB_OK | MB_ICONINFORMATION);
                break;
            }
            bool toPhones = IsUiChecked(g_hChkPhones);
            bool toMic    = IsUiChecked(g_hChkMic);

            if (!toPhones && !toMic) {
                MessageBoxW(hwnd, L"Включи хотя бы один выход.",
                            L"Soundpad", MB_OK | MB_ICONWARNING);
                break;
            }

            std::uniform_int_distribution<size_t> dist(0, g_sounds.size() - 1);
            size_t idx = dist(g_rng);

            PlaySoundFileUI(hwnd, g_sounds[idx], toPhones, toMic, true);
            break;
        }

        case ID_CHK_AUTONEXT_LIST:
            g_autoNextList = IsUiChecked(g_hChkAutoNextList);
            if (g_autoNextList) {
                g_autoNextRandom = false;
                SetUiChecked(g_hChkAutoNextRand, false);
            }
            break;

        case ID_CHK_AUTONEXT_RAND:
            g_autoNextRandom = IsUiChecked(g_hChkAutoNextRand);
            if (g_autoNextRandom) {
                g_autoNextList = false;
                SetUiChecked(g_hChkAutoNextList, false);
            }
            break;

        case ID_BTN_SPAM_START: {
            if (g_spamRunning.load()) {
                MessageBoxW(hwnd, L"Спам уже идёт.", L"Spam", MB_OK);
                break;
            }

            int len = GetWindowTextLengthW(g_hEditSpamText);
            if (len <= 0) {
                MessageBoxW(hwnd, L"Введи текст сообщения.",
                            L"Spam", MB_OK | MB_ICONWARNING);
                break;
            }

            std::wstring text(len + 1, L'\0');
            GetWindowTextW(g_hEditSpamText, &text[0], len + 1);
            text.resize(len);

            wchar_t buf[32] = {0};
            GetWindowTextW(g_hEditSpamMs, buf, 31);
            int ms = _wtoi(buf);
            if (ms < 100) ms = 100;

            GetWindowTextW(g_hEditSpamLimit, buf, 31);
            int limit = _wtoi(buf);
            if (limit < 0) limit = 0;

            g_spamSent = 0;
            g_spamRunning = true;

            std::thread([hwnd, text, ms, limit]() {
                SpamWorker(hwnd, text, ms, limit);
            }).detach();

            break;
        }

        case ID_BTN_SPAM_STOP: {
            g_spamRunning = false;
            break;
        }

        case ID_BTN_HAND_START: {
            if (g_handRunning.load()) {
                MessageBoxW(hwnd, L"Спам рукой уже идёт.", L"Hand Spam", MB_OK);
                break;
            }

            if (!g_zoomRunning) {
                MessageBoxW(hwnd, L"Zoom.exe не запущен.",
                            L"Hand Spam", MB_OK | MB_ICONWARNING);
                break;
            }

            g_handRunning = true;

            std::thread([hwnd]() {
                HandSpamWorker(hwnd);
            }).detach();

            break;
        }

        case ID_BTN_HAND_STOP: {
            g_handRunning = false;
            break;
        }

        case ID_LIST_SOUNDS: {
            if (HIWORD(wp) == LBN_DBLCLK) {
                SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ID_BTN_PLAY, 0), 0);
            }
            break;
        }

         
        case ID_BTN_NEON_START: {
            if (!NeonApi::StartService()) {
                MessageBoxW(hwnd,
                    L"Не удалось запустить dfvcam.exe.\n"
                    L"Проверь Neon\\binaries\\dfvcam.exe",
                    L"Neon", MB_OK | MB_ICONWARNING);
                break;
            }
            Sleep(800);
            g_neonAlive = NeonApi::IsAlive();
            SetTextIfChanged(g_hStaticNeonStatus,
                g_neonAlive ? L"Neon: сервис запущен"
                            : L"Neon: сервис не отвечает");
            if (g_neonAlive) {
                FillNeonAudioDevices(g_hComboNeonAudio);
                RefreshNeonCameras();
            }
            break;
        }

        case ID_BTN_NEON_STOP: {
            NeonApi::StopService();
            Sleep(800);
            g_neonAlive = false;
            SetTextIfChanged(g_hStaticNeonStatus, L"Neon: сервис остановлен");
            SendMessageW(g_hListNeonCams, LB_RESETCONTENT, 0, 0);
            g_neonCamIds.clear();
            break;
        }

        case ID_BTN_NEON_CREATE: {
            if (!g_neonAlive) {
                MessageBoxW(hwnd, L"Сначала запусти сервис.",
                            L"Neon", MB_OK | MB_ICONWARNING);
                break;
            }
            g_neonCamCounter++;
            wchar_t nameBuf[64];
            swprintf(nameBuf, 64, L"Neon Cam %d", g_neonCamCounter);
            std::string id = NeonApi::CreateCamera(nameBuf, 1920, 1080, 30);
            if (id.empty()) {
                MessageBoxW(hwnd, L"Не удалось создать камеру.",
                            L"Neon", MB_OK | MB_ICONWARNING);
                break;
            }

            if (!g_neonAudioIds.empty()) {
                int sel = (int)SendMessageW(g_hComboNeonAudio, CB_GETCURSEL, 0, 0);
                std::string chosenId;
                if (sel >= 0 && sel < (int)g_neonAudioIds.size()) {
                    chosenId = g_neonAudioIds[sel];
                } else {
                    for (size_t i = 0; i < g_neonAudioIds.size(); ++i) {
                        if (g_neonAudioLabels[i].find(L"CABLE Input") != std::wstring::npos ||
                            g_neonAudioLabels[i].find(L"VB-Audio")    != std::wstring::npos) {
                            chosenId = g_neonAudioIds[i];
                            break;
                        }
                    }
                    if (chosenId.empty()) chosenId = g_neonAudioIds[0];
                }
                NeonApi::SetAudioDevice(id, chosenId);
            }

            RefreshNeonCameras();
            break;
        }

        case ID_BTN_NEON_DELETE: {
            std::string id = GetSelectedNeonCamId();
            if (id.empty()) {
                MessageBoxW(hwnd, L"Выбери камеру в списке.",
                            L"Neon", MB_OK | MB_ICONINFORMATION);
                break;
            }
            NeonApi::DeleteCamera(id);
            RefreshNeonCameras();
            break;
        }

        case ID_BTN_NEON_ADDVID: {
            std::string id = GetSelectedNeonCamId();
            if (id.empty()) {
                MessageBoxW(hwnd, L"Выбери камеру в списке.",
                            L"Neon", MB_OK | MB_ICONINFORMATION);
                break;
            }

            wchar_t fileBuf[4096] = {0};
            OPENFILENAMEW ofn{};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner   = hwnd;
            ofn.lpstrFilter =
                L"Видео (*.mp4;*.mkv;*.avi;*.mov;*.webm)\0"
                L"*.mp4;*.mkv;*.avi;*.mov;*.webm\0"
                L"Все файлы (*.*)\0*.*\0";
            ofn.lpstrFile   = fileBuf;
            ofn.nMaxFile    = 4096;
            ofn.lpstrTitle  = L"Выбери видео";
            ofn.Flags       = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                              OFN_EXPLORER | OFN_ALLOWMULTISELECT;

            if (!GetOpenFileNameW(&ofn)) break;

            std::vector<std::wstring> files;
            if (ofn.nFileOffset > 0 && fileBuf[ofn.nFileOffset - 1] == L'\0') {
                std::wstring dir = fileBuf;
                const wchar_t* p = fileBuf + dir.length() + 1;
                while (*p) {
                    files.push_back(dir + L"\\" + p);
                    p += wcslen(p) + 1;
                }
            } else {
                files.push_back(fileBuf);
            }

            for (const auto& f : files)
                NeonApi::AddToPlaylist(id, f);
            break;
        }

        case ID_BTN_NEON_PLAY:
        case ID_BTN_NEON_PAUSE:
        case ID_BTN_NEON_STOPCAM: {
            std::string id = GetSelectedNeonCamId();
            if (id.empty()) break;

            switch (LOWORD(wp)) {
            case ID_BTN_NEON_PLAY:    NeonApi::Play(id);  break;
            case ID_BTN_NEON_PAUSE:   NeonApi::Pause(id); break;
            case ID_BTN_NEON_STOPCAM: NeonApi::Stop(id);  break;
            }
            break;
        }

        case ID_COMBO_NEON_AUDIO: {
            if (HIWORD(wp) != CBN_SELCHANGE) break;
            std::string camId = GetSelectedNeonCamId();
            if (camId.empty()) break;

            int devIdx = (int)SendMessageW(g_hComboNeonAudio, CB_GETCURSEL, 0, 0);
            if (devIdx >= 0 && devIdx < (int)g_neonAudioIds.size()) {
                NeonApi::SetAudioDevice(camId, g_neonAudioIds[devIdx]);
            }
            break;
        }

        case ID_LIST_NEON_CAMS: {
            if (HIWORD(wp) == LBN_SELCHANGE) {
                 
            }
            break;
        }

        }
        PostWebUiState();
        return 0;
    }

    case WM_DESTROY:
        if (g_webView) {
            g_webView->remove_WebMessageReceived(g_webMessageToken);
            g_webView.Reset();
        }
        if (g_webController) {
            g_webController->Close();
            g_webController.Reset();
        }
        ReleaseAltY();
        g_spamRunning = false;
        g_handRunning = false;
        g_stopRequested = true;
        for (auto& t : g_playThreads)
            if (t.joinable()) t.join();
        if (g_hCopyToast) {
            DestroyWindow(g_hCopyToast);
            g_hCopyToast = nullptr;
        }
        KillTimer(hwnd, ID_TIMER_STATUS);
        KillTimer(hwnd, ID_TIMER_ANIMATION);
        KillTimer(hwnd, ID_TIMER_LOADING);
        KillTimer(hwnd, ID_TIMER_WEBUI_TIMEOUT);
        KillTimer(hwnd, ID_TIMER_LOADING_REVEAL);
        KillTimer(hwnd, ID_TIMER_LOADING_FADE);
        if (g_hLoadingOverlay) {
            DestroyWindow(g_hLoadingOverlay);
            g_hLoadingOverlay = nullptr;
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

 
 
 
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nShow) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (!PrepareBundledFiles(hInst)) {
        MessageBoxW(nullptr,
            L"Не удалось подготовить встроенные файлы ZoomBreaker в папке профиля пользователя.",
            L"ZoomBreaker", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return 1;
    }
    LoadOswaldFont();

    typedef HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    auto pSetAttr = dwm ? (pDwmSetWindowAttribute)GetProcAddress(dwm, "DwmSetWindowAttribute") : nullptr;

    INITCOMMONCONTROLSEX icc{ sizeof(icc),
        ICC_STANDARD_CLASSES | ICC_BAR_CLASSES | ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    WNDCLASSEXW loadingClass{};
    loadingClass.cbSize = sizeof(loadingClass);
    loadingClass.lpfnWndProc = LoadingOverlayProc;
    loadingClass.hInstance = hInst;
    loadingClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    HBRUSH loadingBackground = CreateSolidBrush(C_BG);
    loadingClass.hbrBackground = loadingBackground;
    loadingClass.lpszClassName = L"ZoomBreakerLoadingOverlay";
    if (!loadingBackground || !RegisterClassExW(&loadingClass)) {
        if (loadingBackground) DeleteObject(loadingBackground);
        loadingBackground = nullptr;
    }

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInst, MAKEINTRESOURCEW(1));
    wc.hIconSm       = wc.hIcon;
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"ZoomBreakerWnd";
    RegisterClassExW(&wc);

    int w = 1180, h = 860;
    int sx = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int sy = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    HWND hwnd = CreateWindowExW(
        0, L"ZoomBreakerWnd", L"ZoomBreaker",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        sx, sy, w, h, nullptr, nullptr, hInst, nullptr);

    if (pSetAttr) {
        BOOL dark = TRUE;
        pSetAttr(hwnd, 20, &dark, sizeof(dark));
    }

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hCtrlFont) {
        DeleteObject(g_hCtrlFont);
        g_hCtrlFont = nullptr;
    }
    if (g_hOswaldFont) {
        RemoveFontMemResourceEx(g_hOswaldFont);
        g_hOswaldFont = nullptr;
    }

    UnregisterClassW(L"ZoomBreakerLoadingOverlay", hInst);
    if (loadingBackground) DeleteObject(loadingBackground);

    CoUninitialize();
    return (int)msg.wParam;
}
