#include "pch.h"

#include "WindowOperations.h"
#include <commctrl.h>
#include <algorithm>
#include <cmath>
#include <memory>

namespace desktop_window {
namespace {
struct Limits {
  std::optional<Size> minimum;
  std::optional<Size> maximum;
};

void CheckWindow(HWND window) {
  DWORD process = 0;
  auto thread = GetWindowThreadProcessId(window, &process);
  if (!IsWindow(window) || process != GetCurrentProcessId() || thread != GetCurrentThreadId()) {
    throw Error("E_NO_WINDOW", "The target view is not attached to a window on this UI thread.");
  }
}

void Validate(Size size) {
  if (!std::isfinite(size.width) || !std::isfinite(size.height) || size.width <= 0 || size.height <= 0 ||
      size.width > 1000000 || size.height > 1000000) {
    throw Error("E_INVALID_SIZE", "Window dimensions must be positive, finite, and at most 1,000,000.");
  }
}

double Scale(HWND window) { return static_cast<double>(GetDpiForWindow(window)) / 96.0; }

SIZE FrameSize(HWND window, Size content) {
  const double scale = Scale(window);
  RECT rect{0, 0, static_cast<LONG>(std::lround(content.width * scale)),
                 static_cast<LONG>(std::lround(content.height * scale))};
  if (!AdjustWindowRectExForDpi(&rect, static_cast<DWORD>(GetWindowLongPtr(window, GWL_STYLE)),
      GetMenu(window) != nullptr, static_cast<DWORD>(GetWindowLongPtr(window, GWL_EXSTYLE)), GetDpiForWindow(window))) {
    throw Error("E_NATIVE", "Could not convert the content size to a window frame.");
  }
  return {rect.right - rect.left, rect.bottom - rect.top};
}

LRESULT CALLBACK LimitsProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam,
                            UINT_PTR id, DWORD_PTR data) noexcept {
  auto limits = reinterpret_cast<Limits *>(data);
  if (message == WM_NCDESTROY) {
    RemoveWindowSubclass(window, LimitsProc, id);
    delete limits;
    return DefSubclassProc(window, message, wparam, lparam);
  }
  auto result = DefSubclassProc(window, message, wparam, lparam);
  if (message == WM_GETMINMAXINFO) {
    // Never allow exceptions to cross the Win32 callback boundary.
    try {
      auto info = reinterpret_cast<MINMAXINFO *>(lparam);
      if (limits->minimum) {
        const auto size = FrameSize(window, *limits->minimum);
        info->ptMinTrackSize.x = std::max(info->ptMinTrackSize.x, size.cx);
        info->ptMinTrackSize.y = std::max(info->ptMinTrackSize.y, size.cy);
      }
      if (limits->maximum) {
        const auto size = FrameSize(window, *limits->maximum);
        info->ptMaxTrackSize.x = std::min(info->ptMaxTrackSize.x, size.cx);
        info->ptMaxTrackSize.y = std::min(info->ptMaxTrackSize.y, size.cy);
        info->ptMaxSize.x = std::min(info->ptMaxSize.x, size.cx);
        info->ptMaxSize.y = std::min(info->ptMaxSize.y, size.cy);
      }
      info->ptMaxTrackSize.x = std::max(info->ptMaxTrackSize.x, info->ptMinTrackSize.x);
      info->ptMaxTrackSize.y = std::max(info->ptMaxTrackSize.y, info->ptMinTrackSize.y);
    } catch (...) {
      // Retain the OS defaults if a window is being destroyed or DPI is unavailable.
    }
  }
  return result;
}

Limits *FindLimits(HWND window) {
  DWORD_PTR data = 0;
  GetWindowSubclass(window, LimitsProc, reinterpret_cast<UINT_PTR>(&LimitsProc), &data);
  return reinterpret_cast<Limits *>(data);
}

Limits &EnsureLimits(HWND window) {
  if (auto existing = FindLimits(window)) return *existing;
  auto limits = std::make_unique<Limits>();
  if (!SetWindowSubclass(window, LimitsProc, reinterpret_cast<UINT_PTR>(&LimitsProc),
                         reinterpret_cast<DWORD_PTR>(limits.get()))) {
    throw Error("E_NATIVE", "Could not install window size constraints.");
  }
  return *limits.release(); // Deleted by WM_NCDESTROY, independently of the React instance.
}

Size Clamp(HWND window, Size size) {
  if (const auto limits = FindLimits(window)) {
    if (limits->minimum) {
      size.width = std::max(size.width, limits->minimum->width);
      size.height = std::max(size.height, limits->minimum->height);
    }
    if (limits->maximum) {
      size.width = std::min(size.width, limits->maximum->width);
      size.height = std::min(size.height, limits->maximum->height);
    }
  }
  return size;
}

void SetLimit(HWND window, std::optional<Size> size, bool minimum) {
  CheckWindow(window);
  if (size) Validate(*size);
  auto &limits = EnsureLimits(window);
  auto next = limits;
  (minimum ? next.minimum : next.maximum) = size;
  if (next.minimum && next.maximum && (next.minimum->width > next.maximum->width ||
                                      next.minimum->height > next.maximum->height)) {
    throw Error("E_INVALID_CONSTRAINTS", "Minimum size cannot exceed maximum size.");
  }
  Restore(window);
  const auto previous = limits;
  limits = next;
  try { SetSize(window, GetSize(window)); }
  catch (...) { limits = previous; throw; }
}
} // namespace

Size GetSize(HWND window) {
  CheckWindow(window);
  RECT rect{};
  if (!GetClientRect(window, &rect)) throw Error("E_NATIVE", "Could not read the window size.");
  const double scale = Scale(window);
  return {(rect.right - rect.left) / scale, (rect.bottom - rect.top) / scale};
}

void Restore(HWND window) {
  CheckWindow(window);
  // A minimized maximized window first returns to maximized, then to normal.
  if (IsIconic(window)) ShowWindow(window, SW_RESTORE);
  if (IsZoomed(window)) ShowWindow(window, SW_RESTORE);
}

void SetSize(HWND window, Size size) {
  CheckWindow(window);
  Validate(size);
  Restore(window);
  const auto frame = FrameSize(window, Clamp(window, size));
  if (!SetWindowPos(window, nullptr, 0, 0, frame.cx, frame.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE)) {
    throw Error("E_NATIVE", "Could not resize the window.");
  }
}

void SetMinimum(HWND window, std::optional<Size> size) { SetLimit(window, size, true); }
void SetMaximum(HWND window, std::optional<Size> size) { SetLimit(window, size, false); }

void Maximize(HWND window) {
  CheckWindow(window);
  if (!(GetWindowLongPtr(window, GWL_STYLE) & WS_MAXIMIZEBOX)) {
    throw Error("E_UNSUPPORTED_STATE", "The window cannot be maximized.");
  }
  if (!IsZoomed(window) || IsIconic(window)) ShowWindow(window, SW_MAXIMIZE);
}

void Minimize(HWND window) {
  CheckWindow(window);
  if (!(GetWindowLongPtr(window, GWL_STYLE) & WS_MINIMIZEBOX)) {
    throw Error("E_UNSUPPORTED_STATE", "The window cannot be minimized.");
  }
  if (!IsIconic(window)) ShowWindow(window, SW_MINIMIZE);
}
} // namespace desktop_window
