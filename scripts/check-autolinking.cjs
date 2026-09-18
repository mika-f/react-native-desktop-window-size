const assert = require('node:assert/strict');
const path = require('node:path');
const {
  getDependencyConfig,
} = require('@react-native-community/cli-config-apple');
const user = require('../react-native.config');
const root = path.resolve(__dirname, '..');

const apple = getDependencyConfig({ platformName: 'ios' })(
  root,
  user.dependency.platforms.ios,
);
assert.equal(
  apple.podspecPath,
  path.join(root, 'react-native-desktop-window-size.podspec'),
);
console.log('Apple autolinking discovers the macOS podspec.');

if (process.platform === 'win32') {
  const {
    dependencyConfigWindows,
  } = require('@react-native-windows/cli/lib-commonjs/commands/config/dependencyConfig');
  const windows = dependencyConfigWindows(
    root,
    user.dependency.platforms.windows,
  );
  assert.equal(windows.projects.length, 1);
  assert.equal(windows.projects[0].projectName, 'DesktopWindowSize');
  assert.deepEqual(windows.projects[0].cppPackageProviders, [
    'DesktopWindowSize::ReactPackageProvider',
  ]);
  assert.equal(windows.solutionFile, 'DesktopWindowSize.sln');
  console.log(
    'Windows autolinking discovers the project and package provider.',
  );
}
