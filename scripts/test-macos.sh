#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p .native-build
xcrun clang++ -std=c++20 -fobjc-arc -framework AppKit \
  -I macos macos/RNDWSWindowOperations.mm tests/native/macos.mm \
  -o .native-build/macos-tests
.native-build/macos-tests
