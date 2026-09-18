
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

// #include "NativeDesktopWindowSizeDataTypes.g.h" before this file to use the generated type definition
#include <NativeModules.h>
#include <tuple>

namespace DesktopWindowSizeCodegen {

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DesktopWindowSizeSpec_getSize_returnType*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"width", &DesktopWindowSizeSpec_getSize_returnType::width},
        {L"height", &DesktopWindowSizeSpec_getSize_returnType::height},
    };
    return fieldMap;
}

struct DesktopWindowSizeSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double, Promise<DesktopWindowSizeSpec_getSize_returnType>) noexcept>{0, L"getSize"},
      Method<void(double, double, double, Promise<void>) noexcept>{1, L"setSize"},
      Method<void(double, Promise<void>) noexcept>{2, L"maximize"},
      Method<void(double, Promise<void>) noexcept>{3, L"minimize"},
      Method<void(double, Promise<void>) noexcept>{4, L"restore"},
      Method<void(double, std::optional<double>, std::optional<double>, Promise<void>) noexcept>{5, L"setMinSize"},
      Method<void(double, std::optional<double>, std::optional<double>, Promise<void>) noexcept>{6, L"setMaxSize"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DesktopWindowSizeSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getSize",
          "    REACT_METHOD(getSize) void getSize(double viewTag, ::React::ReactPromise<DesktopWindowSizeSpec_getSize_returnType> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getSize) static void getSize(double viewTag, ::React::ReactPromise<DesktopWindowSizeSpec_getSize_returnType> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "setSize",
          "    REACT_METHOD(setSize) void setSize(double viewTag, double width, double height, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setSize) static void setSize(double viewTag, double width, double height, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "maximize",
          "    REACT_METHOD(maximize) void maximize(double viewTag, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(maximize) static void maximize(double viewTag, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "minimize",
          "    REACT_METHOD(minimize) void minimize(double viewTag, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(minimize) static void minimize(double viewTag, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "restore",
          "    REACT_METHOD(restore) void restore(double viewTag, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(restore) static void restore(double viewTag, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "setMinSize",
          "    REACT_METHOD(setMinSize) void setMinSize(double viewTag, std::optional<double> width, std::optional<double> height, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setMinSize) static void setMinSize(double viewTag, std::optional<double> width, std::optional<double> height, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "setMaxSize",
          "    REACT_METHOD(setMaxSize) void setMaxSize(double viewTag, std::optional<double> width, std::optional<double> height, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setMaxSize) static void setMaxSize(double viewTag, std::optional<double> width, std::optional<double> height, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace DesktopWindowSizeCodegen
