const { execFileSync } = require('node:child_process');
const { version } = require('../package.json');
const file =
  process.argv[2] ||
  `.native-build/react-native-desktop-window-size-${version}.tgz`;
const files = execFileSync('tar', ['-tzf', file], { encoding: 'utf8' }).split(
  '\n',
);
for (const required of [
  'lib/index.js',
  'lib/index.d.ts',
  'src/NativeDesktopWindowSize.ts',
  'macos/RNDesktopWindowSize.mm',
  'macos/RNDWSWindowOperations.mm',
  'windows/DesktopWindowSize/DesktopWindowSize.vcxproj',
  'windows/DesktopWindowSize/codegen/NativeDesktopWindowSizeSpec.g.h',
  'windows/DesktopWindowSize/WindowOperations.cpp',
  'react-native-desktop-window-size.podspec',
  'react-native.config.js',
  'README.md',
  'LICENSE',
  'NuGet.config',
]) {
  if (!files.includes(`package/${required}`))
    throw new Error(`Missing package file: ${required}`);
}
if (
  files.some((file) =>
    /\/(node_modules|tests|\.native-build|Pods)\//.test(file),
  )
) {
  throw new Error('Development artifacts must not be published.');
}
console.log(`Verified ${files.filter(Boolean).length} package files.`);
