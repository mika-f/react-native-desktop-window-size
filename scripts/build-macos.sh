#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
export RNDWS_ROOT="$PWD"
export RCT_NEW_ARCH_ENABLED="${RCT_NEW_ARCH_ENABLED:-1}"
mkdir -p .native-build/macos
cp tests/native/Podfile .native-build/macos/Podfile
ruby -r xcodeproj -e '
  project = Xcodeproj::Project.new(".native-build/macos/NativeBuild.xcodeproj")
  project.new_target(:static_library, "NativeBuild", :osx, "14.0")
  project.save
'
cd .native-build/macos
pod install
xcodebuild -project Pods/Pods.xcodeproj -target react-native-desktop-window-size \
  -configuration Debug -sdk macosx CODE_SIGNING_ALLOWED=NO \
  CLANG_ENABLE_MODULES=YES
