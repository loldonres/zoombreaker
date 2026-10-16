#include "settings.h"
#include <commctrl.h>
#include <string>

extern const wchar_t* g_uiFontFace;

#pragma comment(lib, "comctl32.lib")

static const COLORREF SBG    = RGB(13, 12, 19);
static const COLORREF SPANEL = RGB(34, 33, 46);
static const COLORREF SINPUT = RGB(22, 21, 32);
static const COLORREF SBORD  = RGB(57, 52, 94);
static const COLORREF STEXT  = RGB(255, 255, 255);
static const COLORREF SDIM   = RGB(148, 163, 184);
static const COLORREF SOK    = RGB(34, 197, 139);
static const COLORREF SERR   = RGB(248, 91, 111);
static const COLORREF SACC   = RGB(37, 99, 235);
static const COLORREF SPURPLE = RGB(99, 102, 241);

#define IDC_EDIT_MIN   7001
#define IDC_EDIT_MAX   7002
#define IDC_BTN_APPLY  7003
#define IDC_BTN_CANCEL 7004

struct SettingsCtx {
    int* gainMin;
    int* gainMax;
    bool applied;
    HWND hMin;
    HWND hMax;
};

static void OnPaint(HWND hwnd, SettingsCtx* ctx) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc; GetClientRect(hwnd, &rc);

    HBRUSH brBg = CreateSolidBrush(SBG);
    FillRect(hdc, &rc, brBg);
    DeleteObject(brBg);

    SetBkMode(hdc, TRANSPARENT);
    HFONT fTitle = CreateFontW(-18, 0, 0, 0, FW_BOLD, 0, 0, 0,
                               DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HFONT fNorm = CreateFontW(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                              DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);

    HFONT oldF = (HFONT)SelectObject(hdc, fTitle);
    SetTextColor(hdc, STEXT);
    RECT t = { 20, 15, rc.right - 20, 45 };
    DrawTextW(hdc, L"Настройки усиления", -1, &t,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, fNorm);
    SetTextColor(hdc, SDIM);

    RECT d = { 20, 50, rc.right - 20, 70 };
    DrawTextW(hdc, L"Диапазон для слайдера громкости (0-1000%).",
              -1, &d, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT l1 = { 20, 90, 220, 110 };
    DrawTextW(hdc, L"Усиление от (%):", -1, &l1,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT l2 = { 20, 156, 220, 176 };
    DrawTextW(hdc, L"Усиление до (%):", -1, &l2,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, oldF);
    DeleteObject(fTitle);
    DeleteObject(fNorm);

    EndPaint(hwnd, &ps);
}

static COLORREF BlendSettingsColor(COLORREF a, COLORREF b, int amount) {
    if (amount < 0) amount = 0;
    if (amount > 255) amount = 255;
    return RGB(
        (GetRValue(a) * (255 - amount) + GetRValue(b) * amount) / 255,
        (GetGValue(a) * (255 - amount) + GetGValue(b) * amount) / 255,
        (GetBValue(a) * (255 - amount) + GetBValue(b) * amount) / 255);
}

static void DrawSettingsButton(LPDRAWITEMSTRUCT dis) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    bool apply = dis->CtlID == IDC_BTN_APPLY;
    bool pressed = (dis->itemState & ODS_SELECTED) != 0;
    bool disabled = (dis->itemState & ODS_DISABLED) != 0;

    COLORREF fill = apply ? SACC : SPANEL;
    if (pressed) fill = BlendSettingsColor(fill, RGB(15, 20, 45), 70);
    if (disabled) fill = RGB(35, 34, 43);
    HRGN region = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom,
                                     20, 20);
    if (region) {
        HBRUSH brush = CreateSolidBrush(fill);
        FillRgn(hdc, region, brush);
        DeleteObject(brush);
        DeleteObject(region);
    }

    HPEN pen = CreatePen(PS_SOLID, 1, apply ? SPURPLE : SBORD);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1, 20, 20);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    wchar_t text[128]{};
    GetWindowTextW(dis->hwndItem, text, 127);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, disabled ? SDIM : STEXT);
    HFONT font = CreateFontW(-14, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                             DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);
    HGDIOBJ oldFont = SelectObject(hdc, font);
    DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static SettingsCtx* ctx = nullptr;

    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        ctx = (SettingsCtx*)cs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)ctx);

        HFONT fCtrl = CreateFontW(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                                  DEFAULT_CHARSET, 0, 0, 0, 0, g_uiFontFace);

        wchar_t buf[32];
        swprintf(buf, 32, L"%d", *ctx->gainMin);
        ctx->hMin = CreateWindowExW(0, L"EDIT", buf,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
            20, 116, 320, 28, hwnd, (HMENU)IDC_EDIT_MIN,
            GetModuleHandleW(nullptr), nullptr);

        swprintf(buf, 32, L"%d", *ctx->gainMax);
        ctx->hMax = CreateWindowExW(0, L"EDIT", buf,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
            20, 182, 320, 28, hwnd, (HMENU)IDC_EDIT_MAX,
            GetModuleHandleW(nullptr), nullptr);

		HWND hApply = CreateWindowExW(0, L"BUTTON", L"Применить",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			20, 340, 150, 34, hwnd, (HMENU)IDC_BTN_APPLY,
			GetModuleHandleW(nullptr), nullptr);

		HWND hCancel = CreateWindowExW(0, L"BUTTON", L"Отмена",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			190, 340, 150, 34, hwnd, (HMENU)IDC_BTN_CANCEL,
			GetModuleHandleW(nullptr), nullptr);

        HRGN applyRegion = CreateRoundRectRgn(0, 0, 150, 34, 20, 20);
        if (applyRegion && !SetWindowRgn(hApply, applyRegion, TRUE))
            DeleteObject(applyRegion);
        HRGN cancelRegion = CreateRoundRectRgn(0, 0, 150, 34, 20, 20);
        if (cancelRegion && !SetWindowRgn(hCancel, cancelRegion, TRUE))
            DeleteObject(cancelRegion);

        SendMessageW(ctx->hMin, WM_SETFONT, (WPARAM)fCtrl, TRUE);
        SendMessageW(ctx->hMax, WM_SETFONT, (WPARAM)fCtrl, TRUE);
        SendMessageW(hApply, WM_SETFONT, (WPARAM)fCtrl, TRUE);
        SendMessageW(hCancel, WM_SETFONT, (WPARAM)fCtrl, TRUE);

        return 0;
    }

    case WM_CTLCOLORSTATIC:
        SetBkColor((HDC)wp, SBG);
        SetTextColor((HDC)wp, STEXT);
        {
            static HBRUSH br = CreateSolidBrush(SBG);
            return (LRESULT)br;
        }

    case WM_CTLCOLOREDIT:
        SetBkColor((HDC)wp, SINPUT);
        SetTextColor((HDC)wp, STEXT);
        {
            static HBRUSH br = CreateSolidBrush(SINPUT);
            return (LRESULT)br;
        }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
        if (dis->CtlID == IDC_BTN_APPLY || dis->CtlID == IDC_BTN_CANCEL) {
            DrawSettingsButton(dis);
            return TRUE;
        }
        break;
    }

    case WM_PAINT:
        OnPaint(hwnd, ctx);
        return 0;

    case WM_COMMAND: {
        switch (LOWORD(wp)) {
        case IDC_BTN_APPLY: {
            wchar_t b[32];
            GetWindowTextW(ctx->hMin, b, 31);
            int mn = _wtoi(b);
            if (mn < 0) mn = 0;
            if (mn > 1000) mn = 1000;

            GetWindowTextW(ctx->hMax, b, 31);
            int mx = _wtoi(b);
            if (mx < 0) mx = 0;
            if (mx > 1000) mx = 1000;

            if (mn > mx) { int t = mn; mn = mx; mx = t; }

            *ctx->gainMin = mn;
            *ctx->gainMax = mx;
            ctx->applied = true;

            DestroyWindow(hwnd);
            return 0;
        }
        case IDC_BTN_CANCEL:
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY: {
         
        HWND parent = GetWindow(hwnd, GW_OWNER);
        if (parent) PostMessageW(parent, WM_NULL, 0, 0);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

bool ShowSettingsDialog(HWND parent, int& gainMin, int& gainMax) {
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = SettingsWndProc;
        wc.hInstance     = GetModuleHandleW(nullptr);
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = L"ZoomBreakerSettings";
        RegisterClassExW(&wc);
        classRegistered = true;
    }

    SettingsCtx ctx{ &gainMin, &gainMax, false, nullptr, nullptr };

	int w = 380, h = 420;
    RECT pr; GetWindowRect(parent, &pr);
    int sx = pr.left + (pr.right - pr.left - w) / 2;
    int sy = pr.top  + (pr.bottom - pr.top - h) / 2;

    HWND dlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"ZoomBreakerSettings", L"Настройки",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_POPUP,
        sx, sy, w, h, parent, nullptr, GetModuleHandleW(nullptr), &ctx);

    if (!dlg) return false;

    ShowWindow(dlg, SW_SHOW);
    UpdateWindow(dlg);

    EnableWindow(parent, FALSE);

    MSG msg;
    while (IsWindow(dlg) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == dlg || IsChild(dlg, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else if (msg.message == WM_NULL) {
             
            continue;
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);

    return ctx.applied;
}
