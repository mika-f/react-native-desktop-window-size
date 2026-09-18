#include "pch.h"
#include "DesktopWindowSize.h"
#include "WindowOperations.h"
#include <ComponentView.Experimental.interop.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <cmath>
#include <type_traits>

namespace winrt::DesktopWindowSize {
namespace {
HWND WindowForTag(React::ReactContext const &context, double tag) {
  if (!std::isfinite(tag) || tag <= 0 || tag > 9007199254740991.0 || std::floor(tag) != tag) {
    throw desktop_window::Error("E_NO_WINDOW", "Invalid React view tag.");
  }
  const auto view = React::Composition::CompositionUIService::ComponentFromReactTag(context.Handle(), static_cast<int64_t>(tag));
  if (!view) throw desktop_window::Error("E_NO_WINDOW", "The target React view is not mounted.");
  const auto interop = view.try_as<::Microsoft::ReactNative::Composition::Experimental::IComponentViewInterop>();
  const auto parent = interop ? interop->GetHwndForParenting() : nullptr;
  const auto window = parent ? GetAncestor(parent, GA_ROOT) : nullptr;
  if (!window) throw desktop_window::Error("E_NO_WINDOW", "The target React view is not attached to a desktop window.");
  return window;
}

template <typename T, typename Action>
void OnUI(React::ReactContext context, double tag, React::ReactPromise<T> promise, Action action) noexcept {
  const auto dispatcher = context.UIDispatcher();
  if (!dispatcher) {
    promise.Reject(React::ReactError{"E_NO_WINDOW", "The React UI dispatcher is unavailable.", {}});
    return;
  }
  dispatcher.Post([context, tag, promise, action]() noexcept {
    try {
      auto window = WindowForTag(context, tag);
      if constexpr (std::is_void_v<T>) {
        action(window);
        promise.Resolve();
      } else {
        promise.Resolve(action(window));
      }
    } catch (desktop_window::Error const &error) {
      promise.Reject(React::ReactError{error.code, error.what(), {}});
    } catch (winrt::hresult_error const &error) {
      promise.Reject(React::ReactError{"E_NATIVE", winrt::to_string(error.message()), {}});
    } catch (std::exception const &error) {
      promise.Reject(React::ReactError{"E_NATIVE", error.what(), {}});
    } catch (...) {
      promise.Reject(React::ReactError{"E_NATIVE", "Unexpected native window error.", {}});
    }
  });
}

std::optional<desktop_window::Size> OptionalSize(std::optional<double> width, std::optional<double> height) {
  if (width.has_value() != height.has_value()) {
    throw desktop_window::Error("E_INVALID_SIZE", "Both dimensions must be specified or both must be null.");
  }
  if (!width) return std::nullopt;
  return desktop_window::Size{*width, *height};
}
} // namespace

void DesktopWindowSize::getSize(double tag, React::ReactPromise<WindowSize> &&promise) noexcept {
  OnUI(m_context, tag, std::move(promise), [](HWND window) {
    auto size = desktop_window::GetSize(window);
    return WindowSize{size.width, size.height};
  });
}
void DesktopWindowSize::setSize(double tag, double width, double height, React::ReactPromise<void> &&promise) noexcept {
  OnUI(m_context, tag, std::move(promise), [width, height](HWND window) { desktop_window::SetSize(window, {width, height}); });
}
void DesktopWindowSize::maximize(double tag, React::ReactPromise<void> &&promise) noexcept {
  OnUI(m_context, tag, std::move(promise), desktop_window::Maximize);
}
void DesktopWindowSize::minimize(double tag, React::ReactPromise<void> &&promise) noexcept {
  OnUI(m_context, tag, std::move(promise), desktop_window::Minimize);
}
void DesktopWindowSize::restore(double tag, React::ReactPromise<void> &&promise) noexcept {
  OnUI(m_context, tag, std::move(promise), desktop_window::Restore);
}
void DesktopWindowSize::setMinSize(double tag, std::optional<double> width, std::optional<double> height,
                                  React::ReactPromise<void> &&promise) noexcept {
  OnUI(m_context, tag, std::move(promise), [width, height](HWND window) { desktop_window::SetMinimum(window, OptionalSize(width, height)); });
}
void DesktopWindowSize::setMaxSize(double tag, std::optional<double> width, std::optional<double> height,
                                  React::ReactPromise<void> &&promise) noexcept {
  OnUI(m_context, tag, std::move(promise), [width, height](HWND window) { desktop_window::SetMaximum(window, OptionalSize(width, height)); });
}
} // namespace winrt::DesktopWindowSize
