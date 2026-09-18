const path = require('node:path');
const { runCodeGen } = require('@react-native-windows/codegen');
const { codegenConfig } = require('../package.json');

runCodeGen({
  files: [path.resolve(__dirname, '../src/NativeDesktopWindowSize.ts')],
  libraryName: codegenConfig.name,
  outputDirectory: path.resolve(
    __dirname,
    '..',
    codegenConfig.windows.outputDirectory,
  ),
  namespace: codegenConfig.windows.namespace,
  methodOnly: false,
  modulesCxx: false,
  modulesTypeScriptTypes: false,
  modulesWindows: true,
  componentsWindows: false,
  internalComponents: false,
  cppStringType: 'std::string',
  separateDataTypes: true,
  test: false,
});
