#include "frameless_win.h"

#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include <QDateTime>
#include <optional>

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#ifdef Q_OS_MSVC
#  pragma comment(lib, "user32.lib")
#  pragma comment(lib, "dwmapi.lib")
#endif

enum _DWM_SYSTEMBACKDROP_TYPE {
    _DWMSBT_AUTO, // [Default] Let DWM automatically decide the system-drawn backdrop for this
    // window.
    _DWMSBT_NONE,       // [Disable] Do not draw any system backdrop.
    _DWMSBT_MAINWINDOW, // [Mica] Draw the backdrop material effect corresponding to a
    // long-lived window.
    _DWMSBT_TRANSIENTWINDOW, // [Acrylic] Draw the backdrop material effect corresponding to a
    // transient window.
    _DWMSBT_TABBEDWINDOW, // [Mica Alt] Draw the backdrop material effect corresponding to a
    // window with a tabbed title bar.
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_HOLOGRAPHIC = 23,
    WCA_EXCLUDED_FROM_DDA = 24,
    WCA_PASSIVEUPDATEMODE = 25,
    WCA_USEDARKMODECOLORS = 26,
    WCA_CORNER_STYLE = 27,
    WCA_PART_COLOR = 28,
    WCA_DISABLE_MOVESIZE_FEEDBACK = 29,
    WCA_LAST = 30
};

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,        // Traditional DWM blur
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4, // RS4 1803
    ACCENT_ENABLE_HOST_BACKDROP = 5,     // RS5 1809
    ACCENT_INVALID_STATE = 6             // Using this value will remove the window background
};

enum ACCENT_FLAG {
    ACCENT_NONE = 0,
    ACCENT_ENABLE_ACRYLIC = 1,
    ACCENT_ENABLE_ACRYLIC_WITH_LUMINOSITY = 482
};

struct ACCENT_POLICY {
    DWORD dwAccentState;
    DWORD dwAccentFlags;
    DWORD dwGradientColor; // #AABBGGRR
    DWORD dwAnimationId;
};
using PACCENT_POLICY = ACCENT_POLICY *;
struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};
using PWINDOWCOMPOSITIONATTRIBDATA = WINDOWCOMPOSITIONATTRIBDATA *;

typedef HRESULT(WINAPI *DwmSetWindowAttributeFunc)(HWND hwnd, DWORD dwAttribute,
                                                   LPCVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT(WINAPI *DwmExtendFrameIntoClientAreaFunc)(HWND hwnd, const MARGINS *pMarInset);
typedef HRESULT(WINAPI *DwmIsCompositionEnabledFunc)(BOOL *pfEnabled);
typedef HRESULT(WINAPI *DwmEnableBlurBehindWindowFunc)(HWND hWnd,
                                                       const DWM_BLURBEHIND *pBlurBehind);
typedef BOOL(WINAPI *SetWindowCompositionAttributeFunc)(HWND hwnd,
                                                        const WINDOWCOMPOSITIONATTRIBDATA *);
typedef UINT(WINAPI *GetDpiForWindowFunc)(HWND hWnd);
typedef int(WINAPI *GetSystemMetricsForDpiFunc)(int nIndex, UINT dpi);

static DwmSetWindowAttributeFunc pDwmSetWindowAttribute = nullptr;
static DwmExtendFrameIntoClientAreaFunc pDwmExtendFrameIntoClientArea = nullptr;
static DwmIsCompositionEnabledFunc pDwmIsCompositionEnabled = nullptr;
static DwmEnableBlurBehindWindowFunc pDwmEnableBlurBehindWindow = nullptr;
static SetWindowCompositionAttributeFunc pSetWindowCompositionAttribute = nullptr;
static GetDpiForWindowFunc pGetDpiForWindow = nullptr;
static GetSystemMetricsForDpiFunc pGetSystemMetricsForDpi = nullptr;

static RTL_OSVERSIONINFOW GetRealOSVersionImpl() {
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    using RtlGetVersionPtr = NTSTATUS(WINAPI *)(PRTL_OSVERSIONINFOW);
    auto pRtlGetVersion =
        reinterpret_cast<RtlGetVersionPtr>(::GetProcAddress(hMod, "RtlGetVersion"));
    RTL_OSVERSIONINFOW rovi{};
    rovi.dwOSVersionInfoSize = sizeof(rovi);
    pRtlGetVersion(&rovi);
    return rovi;
}

RTL_OSVERSIONINFOW GetRealOSVersion() {
    static const auto result = GetRealOSVersionImpl();
    return result;
}

static inline bool isWin8OrGreater() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 6) || (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion >= 2);
}

static inline bool isWin8Point1OrGreater() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 6) || (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion >= 3);
}

static inline bool isWin10OrGreater() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) || (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion >= 0);
}

static inline bool isWin101809OrGreater() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion >= 0 && rovi.dwBuildNumber >= 17763);
}

static inline bool isWin101903OrGreater() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion >= 0 && rovi.dwBuildNumber >= 18362);
}

static inline bool isWin11OrGreater() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion >= 0 && rovi.dwBuildNumber >= 22000);
}

static inline bool isWin1122H2OrGreater() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion >= 0 && rovi.dwBuildNumber >= 22621);
}

static inline bool isWin10Only() {
    return isWin10OrGreater() && !isWin11OrGreater();
}

static inline bool isWin7Only() {
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return rovi.dwMajorVersion == 7;
}

static inline QByteArray qtNativeEventType() {
    static const auto result = "windows_generic_MSG";
    return result;
}

static inline bool initializeFunctionPointers() {
    HMODULE module = LoadLibraryW(L"dwmapi.dll");
    if (module) {
        if (!pDwmSetWindowAttribute) {
            pDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFunc>(
                GetProcAddress(module, "DwmSetWindowAttribute"));
            if (!pDwmSetWindowAttribute) {
                return false;
            }
        }
        if (!pDwmExtendFrameIntoClientArea) {
            pDwmExtendFrameIntoClientArea = reinterpret_cast<DwmExtendFrameIntoClientAreaFunc>(
                GetProcAddress(module, "DwmExtendFrameIntoClientArea"));
            if (!pDwmExtendFrameIntoClientArea) {
                return false;
            }
        }
        if (!pDwmIsCompositionEnabled) {
            pDwmIsCompositionEnabled = reinterpret_cast<DwmIsCompositionEnabledFunc>(
                ::GetProcAddress(module, "DwmIsCompositionEnabled"));
            if (!pDwmIsCompositionEnabled) {
                return false;
            }
        }
        if (!pDwmEnableBlurBehindWindow) {
            pDwmEnableBlurBehindWindow = reinterpret_cast<DwmEnableBlurBehindWindowFunc>(
                GetProcAddress(module, "DwmEnableBlurBehindWindow"));
            if (!pDwmEnableBlurBehindWindow) {
                return false;
            }
        }
    }

    HMODULE user32 = LoadLibraryW(L"user32.dll");
    if (module) {
        if (!pSetWindowCompositionAttribute) {
            pSetWindowCompositionAttribute = reinterpret_cast<SetWindowCompositionAttributeFunc>(
                GetProcAddress(user32, "SetWindowCompositionAttribute"));
            if (!pSetWindowCompositionAttribute) {
                return false;
            }
        }

        if (!pGetDpiForWindow) {
            pGetDpiForWindow =
                reinterpret_cast<GetDpiForWindowFunc>(GetProcAddress(user32, "GetDpiForWindow"));
            if (!pGetDpiForWindow) {
                return false;
            }
        }

        if (!pGetSystemMetricsForDpi) {
            pGetSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpiFunc>(
                GetProcAddress(user32, "GetSystemMetricsForDpi"));
            if (!pGetSystemMetricsForDpi) {
                return false;
            }
        }
    }


    return true;
}

static inline bool isCompositionEnabled() {
    if (initializeFunctionPointers()) {
        BOOL composition_enabled = false;
        pDwmIsCompositionEnabled(&composition_enabled);
        return composition_enabled;
    }

    return false;
}

static inline void setShadow(HWND hwnd, bool opened) {
    if (initializeFunctionPointers()) {
        MARGINS shadow;
        if (opened) {
            shadow = {1, 0, 0, 0};
        } else {
            shadow = { 0, 0, 0, 0 };
        }
        pDwmExtendFrameIntoClientArea(hwnd, &shadow);
    }

    if (isWin7Only()) {
        SetClassLong(hwnd, GCL_STYLE, GetClassLong(hwnd, GCL_STYLE) | CS_DROPSHADOW);
    }
}

static inline bool setWindowDarkMode(HWND hwnd, const BOOL enable) {
    if (!initializeFunctionPointers()) {
        return false;
    }
    return bool(pDwmSetWindowAttribute(hwnd, 20, &enable, sizeof(BOOL)));
}

static inline std::optional<MONITORINFOEXW> getMonitorForWindow(const HWND hwnd) {
    Q_ASSERT(hwnd);
    if (!hwnd) {
        return std::nullopt;
    }
    const HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return std::nullopt;
    }
    MONITORINFOEXW monitorInfo;
    ::SecureZeroMemory(&monitorInfo, sizeof(monitorInfo));
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (::GetMonitorInfoW(monitor, &monitorInfo) == FALSE) {
        return std::nullopt;
    }
    return monitorInfo;
};

static inline bool isFullScreen(const HWND hwnd) {
    RECT windowRect = {};
    if (::GetWindowRect(hwnd, &windowRect) == FALSE) {
        return false;
    }
    const std::optional<MONITORINFOEXW> mi = getMonitorForWindow(hwnd);
    if (!mi.has_value()) {
        return false;
    }
    RECT rcMonitor = mi.value().rcMonitor;
    return windowRect.top == rcMonitor.top && windowRect.left == rcMonitor.left &&
           windowRect.right == rcMonitor.right && windowRect.bottom == rcMonitor.bottom;
}

static inline bool isMaximized(const HWND hwnd) {
    WINDOWPLACEMENT wp;
    ::GetWindowPlacement(hwnd, &wp);
    return wp.showCmd == SW_MAXIMIZE;
}

static inline quint32 getDpiForWindow(const HWND hwnd, const bool horizontal) {
    if (const UINT dpi = pGetDpiForWindow(hwnd)) {
        return dpi;
    }
    if (const HDC hdc = ::GetDC(hwnd)) {
        bool valid = false;
        const int dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
        const int dpiY = ::GetDeviceCaps(hdc, LOGPIXELSY);
        if ((dpiX > 0) && (dpiY > 0)) {
            valid = true;
        }
        ::ReleaseDC(hwnd, hdc);
        if (valid) {
            return (horizontal ? dpiX : dpiY);
        }
    }
    return 96;
}

static inline int getSystemMetrics(const HWND hwnd, const int index, const bool horizontal) {
    const UINT dpi = getDpiForWindow(hwnd, horizontal);
    if (const int result = pGetSystemMetricsForDpi(index, dpi); result > 0) {
        return result;
    }
    return ::GetSystemMetrics(index);
}

static inline quint32 getResizeBorderThickness(const HWND hwnd, const bool horizontal,
                                               const qreal devicePixelRatio) {
    auto frame = horizontal ? SM_CXSIZEFRAME : SM_CYSIZEFRAME;
    auto result = getSystemMetrics(hwnd, frame, horizontal) + getSystemMetrics(hwnd, 92, horizontal);
    if (result > 0) {
        return result;
    }

    int thickness = isCompositionEnabled() ? 8 : 4;
    return qRound(thickness * devicePixelRatio);
}

static inline bool setWindowEffect(HWND hwnd, const QString &key, const bool &enable) {
    static constexpr const MARGINS extendedMargins = {-1, -1, -1, -1};
    if (key == QStringLiteral("mica")) {
        if (!isWin11OrGreater() || !initializeFunctionPointers()) {
            return false;
        }
        if (enable) {
            pDwmExtendFrameIntoClientArea(hwnd, &extendedMargins);
            if (isWin1122H2OrGreater()) {
                const DWORD backdropType = _DWMSBT_MAINWINDOW;
                pDwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
            } else {
                const BOOL enable = TRUE;
                pDwmSetWindowAttribute(hwnd, 1029, &enable, sizeof(enable));
            }
        } else {
            if (isWin1122H2OrGreater()) {
                const DWORD backdropType = _DWMSBT_AUTO;
                pDwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
            } else {
                const BOOL enable = FALSE;
                pDwmSetWindowAttribute(hwnd, 1029, &enable, sizeof(enable));
            }
        }

        return true;
    }

    if (key == QStringLiteral("mica-alt")) {
        if (!isWin1122H2OrGreater() || !initializeFunctionPointers()) {
            return false;
        }
        if (enable) {
            pDwmExtendFrameIntoClientArea(hwnd, &extendedMargins);
            const DWORD backdropType = _DWMSBT_TABBEDWINDOW;
            pDwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        } else {
            const DWORD backdropType = _DWMSBT_AUTO;
            pDwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        }

        return true;
    }

    if (key == QStringLiteral("acrylic")) {
        if (!isWin11OrGreater() || !initializeFunctionPointers()) {
            return false;
        }
        if (enable) {
            pDwmExtendFrameIntoClientArea(hwnd, &extendedMargins);
            DWORD system_backdrop_type = _DWMSBT_TRANSIENTWINDOW;
            pDwmSetWindowAttribute(hwnd, 38, &system_backdrop_type, sizeof(DWORD));
        } else {
            const DWORD backdropType = _DWMSBT_AUTO;
            pDwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        }

        return true;
    }

    if (key == QStringLiteral("dwm-blur")) {
        if ((isWin7Only() && !isCompositionEnabled()) || !initializeFunctionPointers()) {
            return false;
        }
        if (enable) {
            if (isWin8OrGreater()) {
                ACCENT_POLICY policy{};
                policy.dwAccentState = ACCENT_ENABLE_BLURBEHIND;
                policy.dwAccentFlags = ACCENT_NONE;
                WINDOWCOMPOSITIONATTRIBDATA wcad{};
                wcad.Attrib = WCA_ACCENT_POLICY;
                wcad.pvData = &policy;
                wcad.cbData = sizeof(policy);
                pSetWindowCompositionAttribute(hwnd, &wcad);
            } else {
                DWM_BLURBEHIND bb{};
                bb.fEnable = TRUE;
                bb.dwFlags = DWM_BB_ENABLE;
                pDwmEnableBlurBehindWindow(hwnd, &bb);
            }
        } else {
            if (isWin8OrGreater()) {
                ACCENT_POLICY policy{};
                policy.dwAccentState = ACCENT_DISABLED;
                policy.dwAccentFlags = ACCENT_NONE;
                WINDOWCOMPOSITIONATTRIBDATA wcad{};
                wcad.Attrib = WCA_ACCENT_POLICY;
                wcad.pvData = &policy;
                wcad.cbData = sizeof(policy);
                pSetWindowCompositionAttribute(hwnd, &wcad);
            } else {
                DWM_BLURBEHIND bb{};
                bb.fEnable = FALSE;
                bb.dwFlags = DWM_BB_ENABLE;
                pDwmEnableBlurBehindWindow(hwnd, &bb);
            }
        }

        return true;
    }

    return false;
}

bool containsCursorToItem(QQuickItem *item)
{
    if (!item || !item->isVisible() || !item->isEnabled()) {
        return false;
    }

    auto point = item->window()->mapFromGlobal(QCursor::pos());
    auto rect = QRectF(item->mapToItem(item->window()->contentItem(), QPointF(0, 0)), item->size());
    if (rect.contains(point)) {
        return true;
    }
    return false;
}

WinFrameless::WinFrameless(QWindow *window, QObject *parent)
    : QObject{parent}
    , _window(window)
    , _effect("normal")
{

}

WinFrameless::~WinFrameless()
{

}

void WinFrameless::setEnabled(bool enabled)
{
    if (_enabled == enabled)
        return;

    _enabled = enabled;
    doFrameless(_enabled);
}

bool WinFrameless::enabled() const
{
    return _enabled;
}

void WinFrameless::setCanWindowResize(bool canResize)
{
    if (_canWindowResize == canResize)
        return;

    _canWindowResize = canResize;
}

bool WinFrameless::canWindowResize() const
{
    return _canWindowResize;
}

void WinFrameless::setEffect(const QString &effect)
{
    if (_effect == effect)
        return;

    if (effect != "normal" && !enabled())
        return;

    HWND hwnd = reinterpret_cast<HWND>(window()->winId());
    if (_effect != "normal") {
        setWindowEffect(hwnd, _effect, false);
    }

    if (effect == "normal") {
        _effect = effect;
        return;
    }

    if (setWindowEffect(hwnd, effect, true))
        _effect = effect;
}

QVector<QQuickItem *> *WinFrameless::moveUnderPointer()
{
    return &_moveUnderContainer;
}

QVector<QQuickItem *> *WinFrameless::moveExcludePointer()
{
    return &_moveExcludeContainer;
}

QStringList WinFrameless::availableEffects()
{
    if (isWin11OrGreater()) {
        return {"mica", "mica-alt", "acrylic", "dwm-blur", "normal"};
    }

    return {"dwm-blur", "normal"};
}

QWindow *WinFrameless::window() const
{
    return _window;
}

bool WinFrameless::isMoveContains()
{
    auto underIncludeContainer = std::find_if(_moveUnderContainer.begin(), _moveUnderContainer.end(),
                                              [](QQuickItem *item) {
                                                  return containsCursorToItem(item);
    });

    auto underExcludeContainer = std::find_if(_moveExcludeContainer.begin(), _moveExcludeContainer.end(),
                                              [](QQuickItem *item) {
                                                  return containsCursorToItem(item);
    });

    return (underIncludeContainer != _moveUnderContainer.end()) &&
           ((underExcludeContainer == _moveExcludeContainer.end()) ||
                                ((*underIncludeContainer)->isAncestorOf((*underExcludeContainer)) ? false
                                    : ((*underIncludeContainer)->z() > (*underExcludeContainer)->z())));
}

bool WinFrameless::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if ((eventType != qtNativeEventType()) || !message) {
        return false;
    }

    const auto msg = static_cast<const MSG *>(message);
    auto hwnd = msg->hwnd;
    if (!hwnd) {
        return false;
    }

    const quint64 wid = reinterpret_cast<qint64>(hwnd);
    if (wid != _current) {
        return false;
    }

    const auto uMsg = msg->message;
    const auto wParam = msg->wParam;
    const auto lParam = msg->lParam;
    if (uMsg == WM_WINDOWPOSCHANGING) {
        auto *wp = reinterpret_cast<WINDOWPOS *>(lParam);
        if (wp != nullptr && (wp->flags & SWP_NOSIZE) == 0) {
            wp->flags |= SWP_NOCOPYBITS;
            *result = static_cast<qintptr>(::DefWindowProcW(hwnd, uMsg, wParam, lParam));
            return true;
        }

        return false;
    }

    if (uMsg == WM_NCCALCSIZE && wParam == TRUE) {
        const auto clientRect = ((wParam == FALSE) ? reinterpret_cast<LPRECT>(lParam)
                                                   : &(reinterpret_cast<LPNCCALCSIZE_PARAMS>(lParam))->rgrc[0]);

        bool isMax = ::isMaximized(hwnd);
        bool isFull = ::isFullScreen(hwnd);

        if (isMax && !isFull) {
            auto ty = getResizeBorderThickness(hwnd, false, window()->devicePixelRatio());
            clientRect->top += ty;
            clientRect->bottom -= ty;
            auto tx = getResizeBorderThickness(hwnd, true, window()->devicePixelRatio());
            clientRect->left += tx;
            clientRect->right -= tx;
        }

        if (isMax || isFull) {
            APPBARDATA abd;
            SecureZeroMemory(&abd, sizeof(abd));
            abd.cbSize = sizeof(abd);
            const UINT taskbarState = ::SHAppBarMessage(ABM_GETSTATE, &abd);

            if (!(taskbarState & ABS_AUTOHIDE)) {
                clientRect->bottom += 1;
                *result = 0;
                return true;
            }

            bool top = false, bottom = false, left = false, right = false;
            int edge = -1;
            APPBARDATA abd2;
            SecureZeroMemory(&abd2, sizeof(abd2));
            abd2.cbSize = sizeof(abd2);
            abd2.hWnd = ::FindWindowW(L"Shell_TrayWnd", nullptr);

            if (abd2.hWnd) {
                const HMONITOR windowMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                const HMONITOR taskbarMonitor = ::MonitorFromWindow(abd2.hWnd, MONITOR_DEFAULTTOPRIMARY);
                if (windowMonitor && taskbarMonitor && taskbarMonitor == windowMonitor) {
                    ::SHAppBarMessage(ABM_GETTASKBARPOS, &abd2);
                    edge = abd2.uEdge;
                }
            }

            top = (edge == ABE_TOP);
            bottom = (edge == ABE_BOTTOM);
            left = (edge == ABE_LEFT);
            right = (edge == ABE_RIGHT);

            if (top) {
                clientRect->top += 1;
            } else if (bottom) {
                clientRect->bottom -= 1;
            } else if (left) {
                clientRect->left += 1;
            } else if (right) {
                clientRect->right -= 1;
            } else {
                clientRect->bottom -= 1;
            }
        }

        *result = 0;
        return true;
    }

    if (uMsg == WM_NCHITTEST) {
        *result = 0;

        if (_canWindowResize) {
            POINT nativeGlobalPos{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            POINT nativeLocalPos = nativeGlobalPos;
            ::ScreenToClient(hwnd, &nativeLocalPos);
            RECT clientRect{0, 0, 0, 0};
            ::GetClientRect(hwnd, &clientRect);
            auto clientWidth = clientRect.right - clientRect.left;
            auto clientHeight = clientRect.bottom - clientRect.top;
            bool left = nativeLocalPos.x < _margins;
            bool right = nativeLocalPos.x > clientWidth - _margins;
            bool top = nativeLocalPos.y < _margins;
            bool bottom = nativeLocalPos.y > clientHeight - _margins;

            if (!isFullScreen() && !isMaximized()) {
                if (left && top) {
                    *result = HTTOPLEFT;
                } else if (left && bottom) {
                    *result = HTBOTTOMLEFT;
                } else if (right && top) {
                    *result = HTTOPRIGHT;
                } else if (right && bottom) {
                    *result = HTBOTTOMRIGHT;
                } else if (left) {
                    *result = HTLEFT;
                } else if (right) {
                    *result = HTRIGHT;
                } else if (top) {
                    *result = HTTOP;
                } else if (bottom) {
                    *result = HTBOTTOM;
                }
            }
        }

        if (0 != *result) {
            return true;
        }

        if (isMoveContains()) {
            *result = HTCAPTION;
            return true;
        }

        *result = HTCLIENT;
        return true;
    }

    if (uMsg == WM_NCPAINT) {
        if (isCompositionEnabled() && !this->isFullScreen()) {
            return false;
        }
        *result = FALSE;
        return true;
    }

    if (uMsg == WM_NCACTIVATE) {
        if (isCompositionEnabled()) {
            return false;
        }

        *result = true;
        return true;
    }

    if (uMsg == WM_NCRBUTTONDOWN) {
        if (wParam == HTCAPTION) {
            auto pos = window()->position();
            auto offset = window()->mapFromGlobal(QCursor::pos());
            showSystemMenu(QPoint(pos.x() + offset.x(), pos.y() + offset.y()));
        }

        return false;
    }

    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
        const bool altPressed = ((wParam == VK_MENU) || (::GetKeyState(VK_MENU) < 0));
        const bool spacePressed = ((wParam == VK_SPACE) || (::GetKeyState(VK_SPACE) < 0));
        if (altPressed && spacePressed) {
            showSystemMenu(window()->position());
        }

        return false;
    }

    if (uMsg == WM_SYSCOMMAND) {
        if (wParam == SC_MINIMIZE) {
            if (window()->transientParent()) {
                auto _hwnd = reinterpret_cast<HWND>(window()->transientParent()->winId());
                ::ShowWindow(_hwnd, 2);
            } else {
                auto _hwnd = reinterpret_cast<HWND>(window()->winId());
                ::ShowWindow(_hwnd, 2);
            }
            return true;
        }

        return false;
    }

    return false;
}

bool WinFrameless::isMaximized()
{
    return window()->visibility() == QWindow::Maximized;
}

bool WinFrameless::isFullScreen()
{
    return window()->visibility() == QWindow::FullScreen;
}

void WinFrameless::showSystemMenu(QPoint point)
{
    QScreen *screen = window()->screen();
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    if (!screen) {
        return;
    }
    const QPoint origin = screen->geometry().topLeft();
    auto nativePos =
        QPointF(QPointF(point - origin) * window()->devicePixelRatio()).toPoint() + origin;
    HWND hwnd = reinterpret_cast<HWND>(window()->winId());
    auto hMenu = ::GetSystemMenu(hwnd, FALSE);
    if (isMaximized() || isFullScreen()) {
        ::EnableMenuItem(hMenu, SC_MOVE, MFS_DISABLED);
        ::EnableMenuItem(hMenu, SC_RESTORE, MFS_ENABLED);
    } else {
        ::EnableMenuItem(hMenu, SC_MOVE, MFS_ENABLED);
        ::EnableMenuItem(hMenu, SC_RESTORE, MFS_DISABLED);
    }

    if (!isMaximized() && !isFullScreen()) {
        ::EnableMenuItem(hMenu, SC_SIZE, MFS_ENABLED);
        ::EnableMenuItem(hMenu, SC_MAXIMIZE, MFS_ENABLED);
    } else {
        ::EnableMenuItem(hMenu, SC_SIZE, MFS_DISABLED);
        ::EnableMenuItem(hMenu, SC_MAXIMIZE, MFS_DISABLED);
    }

    const int result = ::TrackPopupMenu(hMenu, (TPM_RETURNCMD | (QGuiApplication::isRightToLeft() ? TPM_RIGHTALIGN : TPM_LEFTALIGN)),
                                        nativePos.x(), nativePos.y(), 0, hwnd, nullptr);

    if (result) {
        ::PostMessageW(hwnd, WM_SYSCOMMAND, result, 0);
    }
}

void WinFrameless::doFrameless(bool enabled)
{
    HWND hwnd = reinterpret_cast<HWND>(window()->winId());
    _current = window()->winId();

    if (enabled) {
        QGuiApplication::instance()->installNativeEventFilter(this);
    } else {
        QGuiApplication::instance()->removeNativeEventFilter(this);
    }

#  if (QT_VERSION == QT_VERSION_CHECK(6, 5, 3))
    qWarning()
        << "Qt's own frameless bug, currently only exist in 6.5.3, please use other versions";
#  endif

    DWORD style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
#  if (QT_VERSION == QT_VERSION_CHECK(6, 7, 2))
    style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
#  endif

    ::SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    connect(window(), &QQuickWindow::screenChanged, this, [hwnd] {
        ::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED |
                           SWP_NOOWNERZORDER);
        ::RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
    });

    setShadow(hwnd, enabled);
}
