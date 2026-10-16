#include "zoom_parse.h"

#include <windows.h>
#include <uiautomation.h>
#include <oleauto.h>
#include <tlhelp32.h>
#include <set>
#include <algorithm>

#pragma comment(lib, "uiautomationcore.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "ole32.lib")

 
 

#ifdef ZOOM_PARSE_DEBUG
static void DbgMsg(const std::wstring& s) {
    MessageBoxW(nullptr, s.c_str(), L"ZoomParse", MB_OK | MB_ICONINFORMATION);
}
#else
static void DbgMsg(const std::wstring&) {}
#endif

 

static DWORD FindProcessIdLocal(const wchar_t* exeName) {
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

 

 
 
 
 
HWND FindZoomParticipantsWindow() {
    struct Ctx {
        HWND result;
    } ctx{ nullptr };

     
    EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
        Ctx* c = (Ctx*)lp;
        if (c->result) return FALSE;
        if (!IsWindowVisible(hwnd)) return TRUE;

        wchar_t cls[128] = {0};
        GetClassNameW(hwnd, cls, 127);

        if (wcscmp(cls, L"ZPlistPopupContainerWndClass") == 0) {
            c->result = hwnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&ctx);

    if (ctx.result) {
        DbgMsg(L"[Debug] Окно участников найдено (top-level)");
        return ctx.result;
    }

     
    EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
        Ctx* c = (Ctx*)lp;
        if (c->result) return FALSE;

        EnumChildWindows(hwnd, [](HWND child, LPARAM lp2) -> BOOL {
            Ctx* cc = (Ctx*)lp2;
            if (cc->result) return FALSE;

            wchar_t cls[128] = {0};
            GetClassNameW(child, cls, 127);
            if (wcscmp(cls, L"ZPlistPopupContainerWndClass") == 0) {
                cc->result = child;
                return FALSE;
            }
            return TRUE;
        }, (LPARAM)c);

        return c->result ? FALSE : TRUE;
    }, (LPARAM)&ctx);

    if (ctx.result) {
        DbgMsg(L"[Debug] Окно участников найдено (дочернее)");
        return ctx.result;
    }

    DbgMsg(L"[Debug] Окно ZPlistPopupContainerWndClass не найдено");
    return nullptr;
}

 

static IUIAutomationElement* GetRootElement(HWND hwnd, IUIAutomation** ppUIA) {
    *ppUIA = nullptr;

    IUIAutomation* pUIA = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IUIAutomation,
                                  (void**)&pUIA);
    if (FAILED(hr) || !pUIA) {
        DbgMsg(L"[Debug] CoCreateInstance CUIAutomation вернул ошибку");
        return nullptr;
    }

    IUIAutomationElement* pRoot = nullptr;
    hr = pUIA->ElementFromHandle(hwnd, &pRoot);
    if (FAILED(hr) || !pRoot) {
        DbgMsg(L"[Debug] ElementFromHandle вернул ошибку");
        pUIA->Release();
        return nullptr;
    }

    *ppUIA = pUIA;
    return pRoot;
}

 
 
 
static void WalkTree(IUIAutomationTreeWalker* pWalker,
                     IUIAutomationElement* pNode,
                     std::vector<std::wstring>& out,
                     int depth)
{
    if (!pWalker || !pNode || depth > 20) return;

    IUIAutomationElement* pChild = nullptr;
    HRESULT hr = pWalker->GetFirstChildElement(pNode, &pChild);
    if (FAILED(hr)) return;

    while (pChild) {
        BSTR name = nullptr;
        if (SUCCEEDED(pChild->get_CurrentName(&name)) && name) {
            std::wstring wname(name, SysStringLen(name));
            SysFreeString(name);

             
            while (!wname.empty() && (wname.front() == L' ' || wname.front() == L'\t'))
                wname.erase(wname.begin());
            while (!wname.empty() && (wname.back() == L' ' || wname.back() == L'\t'))
                wname.pop_back();

#ifdef ZOOM_PARSE_DEBUG
            if (!wname.empty()) {
                std::wstring dbg = L"[Raw] ";
                dbg += wname;
                DbgMsg(dbg);
            }
#endif

             
             
            if (!wname.empty() &&
                wname.find(L',') != std::wstring::npos) {
                std::wstring nick = wname.substr(0, wname.find(L','));
                while (!nick.empty() && nick.back() == L' ') nick.pop_back();
                if (!nick.empty())
                    out.push_back(nick);
            }
        }

        WalkTree(pWalker, pChild, out, depth + 1);

        IUIAutomationElement* pNext = nullptr;
        pWalker->GetNextSiblingElement(pChild, &pNext);
        pChild->Release();
        pChild = pNext;
    }
}

static std::vector<std::wstring> ReadItems(IUIAutomation* pUIA,
                                           IUIAutomationElement* pRoot) {
    std::vector<std::wstring> names;
    if (!pUIA || !pRoot) return names;

    IUIAutomationTreeWalker* pWalker = nullptr;
    HRESULT hr = pUIA->get_RawViewWalker(&pWalker);
    if (FAILED(hr) || !pWalker) {
        DbgMsg(L"[Debug] Не удалось получить RawViewWalker");
        return names;
    }

    WalkTree(pWalker, pRoot, names, 0);

    pWalker->Release();

    if (names.empty()) {
        DbgMsg(L"[Debug] RawView не нашёл ни одного имени");
    } else {
        wchar_t buf[64];
        swprintf(buf, 64, L"[Debug] RawView нашёл имён: %d", (int)names.size());
        DbgMsg(buf);
    }

    return names;
}

 

std::vector<std::wstring> GetZoomParticipants() {
    std::vector<std::wstring> names;

    HWND hwnd = FindZoomParticipantsWindow();
    if (!hwnd) return names;

    IUIAutomation* pUIA = nullptr;
    IUIAutomationElement* pRoot = GetRootElement(hwnd, &pUIA);
    if (!pRoot) return names;

    names = ReadItems(pUIA, pRoot);

    pRoot->Release();
    pUIA->Release();
    return names;
}

 

static IUIAutomationElement* FindScrollableContainer(IUIAutomation* pUIA,
                                                     IUIAutomationElement* pRoot) {
    if (!pUIA || !pRoot) return nullptr;

    VARIANT var;
    VariantInit(&var);
    var.vt = VT_BOOL;
    var.boolVal = VARIANT_TRUE;

    IUIAutomationCondition* pCondition = nullptr;
    HRESULT hr = pUIA->CreatePropertyCondition(
        UIA_IsScrollPatternAvailablePropertyId, var, &pCondition);
    if (FAILED(hr) || !pCondition) return nullptr;

    IUIAutomationElementArray* pArray = nullptr;
    hr = pRoot->FindAll(TreeScope_Descendants, pCondition, &pArray);
    IUIAutomationElement* result = nullptr;

    if (SUCCEEDED(hr) && pArray) {
        int count = 0;
        pArray->get_Length(&count);
        if (count > 0) {
            pArray->GetElement(0, &result);
        }
        pArray->Release();
    }
    pCondition->Release();
    return result;
}

void ScrollParticipantsDown(HWND participantsWnd) {
    IUIAutomation* pUIA = nullptr;
    IUIAutomationElement* pRoot = GetRootElement(participantsWnd, &pUIA);
    if (!pRoot) return;

    IUIAutomationElement* pContainer = FindScrollableContainer(pUIA, pRoot);
    if (pContainer) {
        IUIAutomationScrollPattern* pScroll = nullptr;
        if (SUCCEEDED(pContainer->GetCurrentPatternAs(
                UIA_ScrollPatternId, IID_IUIAutomationScrollPattern,
                (void**)&pScroll)) && pScroll) {
            pScroll->Scroll(ScrollAmount_LargeIncrement, ScrollAmount_NoAmount);
            pScroll->Release();
            Sleep(200);
        }
        pContainer->Release();
    }

    pRoot->Release();
    pUIA->Release();
}

void ScrollParticipantsUp(HWND participantsWnd) {
    IUIAutomation* pUIA = nullptr;
    IUIAutomationElement* pRoot = GetRootElement(participantsWnd, &pUIA);
    if (!pRoot) return;

    IUIAutomationElement* pContainer = FindScrollableContainer(pUIA, pRoot);
    if (pContainer) {
        IUIAutomationScrollPattern* pScroll = nullptr;
        if (SUCCEEDED(pContainer->GetCurrentPatternAs(
                UIA_ScrollPatternId, IID_IUIAutomationScrollPattern,
                (void**)&pScroll)) && pScroll) {
            pScroll->Scroll(ScrollAmount_LargeDecrement, ScrollAmount_NoAmount);
            pScroll->Release();
            Sleep(200);
        }
        pContainer->Release();
    }

    pRoot->Release();
    pUIA->Release();
}

std::vector<std::wstring> CollectAllZoomParticipants() {
    std::set<std::wstring> unique;

    HWND hwnd = FindZoomParticipantsWindow();
    if (!hwnd) return {};

    IUIAutomation* pUIA = nullptr;
    IUIAutomationElement* pRoot = GetRootElement(hwnd, &pUIA);
    if (!pRoot) return {};

    IUIAutomationElement* pContainer = FindScrollableContainer(pUIA, pRoot);
    if (pContainer) {
        IUIAutomationScrollPattern* pScroll = nullptr;
        if (SUCCEEDED(pContainer->GetCurrentPatternAs(
                UIA_ScrollPatternId, IID_IUIAutomationScrollPattern,
                (void**)&pScroll)) && pScroll) {
            pScroll->SetScrollPercent(ScrollAmount_NoAmount, 0.0);
            pScroll->Release();
            Sleep(200);
        }
        pContainer->Release();
    }

    const int kMaxIterations = 50;
    for (int iter = 0; iter < kMaxIterations; ++iter) {
        auto current = ReadItems(pUIA, pRoot);
        size_t before = unique.size();
        for (auto& n : current) unique.insert(n);

        if (unique.size() == before && iter > 0) break;

        IUIAutomationElement* pCont = FindScrollableContainer(pUIA, pRoot);
        if (!pCont) break;

        IUIAutomationScrollPattern* pScroll = nullptr;
        bool scrolled = false;
        if (SUCCEEDED(pCont->GetCurrentPatternAs(
                UIA_ScrollPatternId, IID_IUIAutomationScrollPattern,
                (void**)&pScroll)) && pScroll) {
            pScroll->Scroll(ScrollAmount_LargeIncrement, ScrollAmount_NoAmount);
            pScroll->Release();
            scrolled = true;
        }
        pCont->Release();

        if (!scrolled) break;
        Sleep(250);
    }

    pRoot->Release();
    pUIA->Release();

    return std::vector<std::wstring>(unique.begin(), unique.end());
}