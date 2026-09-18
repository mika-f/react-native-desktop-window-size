module.exports = {
  dependency: {
    platforms: {
      android: null,
      // macOS CocoaPods autolinking uses the shared "ios" discovery path.
      // The podspec's osx-only platform excludes this pod from iOS targets.
      windows: {
        sourceDir: 'windows',
        solutionFile: 'DesktopWindowSize.sln',
        projects: [
          {
            projectFile: 'DesktopWindowSize/DesktopWindowSize.vcxproj',
            directDependency: true,
          },
        ],
      },
    },
  },
};
