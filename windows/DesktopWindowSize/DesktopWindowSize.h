#pragma once
#include "codegen/NativeDesktopWindowSizeDataTypes.g.h"
#include "codegen/NativeDesktopWindowSizeSpec.g.h"
#include <NativeModules.h>

namespace winrt::DesktopWindowSize {
namespace React = winrt::Microsoft::ReactNative;
using WindowSize = DesktopWindowSizeCodegen::DesktopWindowSizeSpec_getSize_returnType;

REACT_MODULE(DesktopWindowSize)
struct DesktopWindowSize {
  using ModuleSpec = DesktopWindowSizeCodegen::DesktopWindowSizeSpec;
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &context) noexcept { m_context = context; }

  REACT_METHOD(getSize)
  void getSize(double tag, React::ReactPromise<WindowSize> &&promise) noexcept;
  REACT_METHOD(setSize)
  void setSize(double tag, double width, double height, React::ReactPromise<void> &&promise) noexcept;
  REACT_METHOD(maximize)
  void maximize(double tag, React::ReactPromise<void> &&promise) noexcept;
  REACT_METHOD(minimize)
  void minimize(double tag, React::ReactPromise<void> &&promise) noexcept;
  REACT_METHOD(restore)
  void restore(double tag, React::ReactPromise<void> &&promise) noexcept;
  REACT_METHOD(setMinSize)
  void setMinSize(double tag, std::optional<double> width, std::optional<double> height,
                  React::ReactPromise<void> &&promise) noexcept;
  REACT_METHOD(setMaxSize)
  void setMaxSize(double tag, std::optional<double> width, std::optional<double> height,
                  React::ReactPromise<void> &&promise) noexcept;
private:
  React::ReactContext m_context;
};
} // namespace winrt::DesktopWindowSize
