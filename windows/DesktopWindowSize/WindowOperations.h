#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <optional>
#include <stdexcept>

namespace desktop_window {
struct Size { double width; double height; };
struct Error : std::runtime_error {
  const char *code;
  Error(const char *code, const char *message) : std::runtime_error(message), code(code) {}
};

// All operations must run on the window's UI thread.
Size GetSize(HWND window);
void SetSize(HWND window, Size size);
void SetMinimum(HWND window, std::optional<Size> size);
void SetMaximum(HWND window, std::optional<Size> size);
void Maximize(HWND window);
void Minimize(HWND window);
void Restore(HWND window);
} // namespace desktop_window
