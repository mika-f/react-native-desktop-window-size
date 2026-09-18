#include "WindowOperations.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>

using namespace desktop_window;
static bool SizeIs(HWND window, double width, double height) {
  auto size = GetSize(window);
  return std::abs(size.width - width) < 1 && std::abs(size.height - height) < 1;
}
int main() {
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  WNDCLASSW klass{};
  klass.lpfnWndProc = DefWindowProcW;
  klass.hInstance = GetModuleHandle(nullptr);
  klass.lpszClassName = L"DesktopWindowSizeTests";
  assert(RegisterClassW(&klass));
  auto create = [&]() { return CreateWindowW(klass.lpszClassName, L"Window tests", WS_OVERLAPPEDWINDOW,
      0, 0, 800, 600, nullptr, nullptr, klass.hInstance, nullptr); };
  HWND first = create();
  HWND second = create();
  assert(first && second);
  SetSize(second, {800, 600});
  SetSize(first, {900, 700});
  assert(SizeIs(first, 900, 700));
  assert(SizeIs(second, 800, 600));
  SetMinimum(first, Size{500, 400});
  SetMaximum(first, Size{850, 650});
  assert(SizeIs(first, 850, 650));
  SetSize(first, {100, 100});
  assert(SizeIs(first, 500, 400));
  bool rejected = false;
  try { SetMinimum(first, Size{900, 400}); }
  catch (Error const &e) { rejected = std::string(e.code) == "E_INVALID_CONSTRAINTS"; }
  assert(rejected);
  rejected = false;
  try { SetSize(first, {std::numeric_limits<double>::quiet_NaN(), 600}); }
  catch (Error const &e) { rejected = std::string(e.code) == "E_INVALID_SIZE"; }
  assert(rejected);
  MINMAXINFO info{};
  // USER32 normally initializes these before dispatching WM_GETMINMAXINFO.
  info.ptMaxTrackSize = {100000, 100000};
  info.ptMaxSize = {100000, 100000};
  SendMessage(first, WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&info));
  assert(info.ptMaxTrackSize.x >= info.ptMinTrackSize.x);
  assert(info.ptMaxTrackSize.y >= info.ptMinTrackSize.y);
  const auto dpi = GetDpiForWindow(first);
  RECT minimum{0, 0, MulDiv(500, dpi, 96), MulDiv(400, dpi, 96)};
  RECT maximum{0, 0, MulDiv(850, dpi, 96), MulDiv(650, dpi, 96)};
  AdjustWindowRectExForDpi(&minimum, WS_OVERLAPPEDWINDOW, FALSE, 0, dpi);
  AdjustWindowRectExForDpi(&maximum, WS_OVERLAPPEDWINDOW, FALSE, 0, dpi);
  assert(info.ptMinTrackSize.x == minimum.right - minimum.left);
  assert(info.ptMaxTrackSize.y == maximum.bottom - maximum.top);
  Maximize(first);
  assert(IsZoomed(first));
  Maximize(first);
  assert(IsZoomed(first));
  Minimize(first);
  assert(IsIconic(first));
  Restore(first);
  assert(!IsIconic(first) && !IsZoomed(first));
  SetMinimum(first, std::nullopt);
  SetMaximum(first, std::nullopt);
  SetSize(first, {950, 700});
  assert(SizeIs(first, 950, 700));
  DestroyWindow(first);
  rejected = false;
  try { GetSize(first); }
  catch (Error const &e) { rejected = std::string(e.code) == "E_NO_WINDOW"; }
  assert(rejected);
  // Destroying a constrained window must remove the subclass and its state.
  first = create();
  SetSize(first, {900, 700});
  assert(SizeIs(first, 900, 700));
  DestroyWindow(first);
  DestroyWindow(second);
  puts("Win32 window tests passed.");
}
