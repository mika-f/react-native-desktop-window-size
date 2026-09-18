# react-native-desktop-window-size

Read and control the native window containing a React Native **macOS or Windows view**. Resize, maximize, minimize, restore, and apply minimum or maximum content sizes through a typed, asynchronous API.

The target comes from a mounted React view, so operations keep addressing that view's window even when another window gains focus.

## Compatibility

| Platform                      | React Native integration                                          | Native implementation        |
| ----------------------------- | ----------------------------------------------------------------- | ---------------------------- |
| macOS 14+                     | `react-native-macos` 0.81.x, Paper or Fabric                      | AppKit `NSWindow`            |
| Windows 10 1903+ / Windows 11 | `react-native-windows` 0.81.x, **Fabric / New Architecture only** | Win32, C++/WinRT TurboModule |

Use React Native 0.81.x and the React version required by your desktop platform package. Other React Native minor versions are outside the initial compatibility range. Windows UWP/Paper, iOS, Android, web, and Expo Go are unsupported.

This package contains native code. Rebuild your app after installation; reloading JavaScript is insufficient.

## Installation

```sh
pnpm add react-native-desktop-window-size
```

### macOS

In an existing React Native macOS app, install CocoaPods dependencies and rebuild:

```sh
cd macos
bundle exec pod install
cd ..
pnpm exec react-native run-macos
```

The package includes a macOS podspec and autolinking configuration. Use `pod install` directly if your app does not use Bundler. The pod uses your app's `RCT_NEW_ARCH_ENABLED` setting.

### Windows

In an existing React Native Windows app using the New Architecture:

```sh
pnpm exec react-native autolink-windows
pnpm exec react-native run-windows
```

Autolinking registers `DesktopWindowSize.ReactPackageProvider`. The Windows project includes generated TurboModule headers, so consumers do not need this library's development tools. Use the Visual Studio and Windows SDK versions required by your installed React Native Windows release; the development baseline in this repository uses the v145 toolset.

For pnpm apps, a hoisted `node_modules` layout is recommended for native build tools. This repository uses `node-linker=hoisted` in `.npmrc`.

## Usage

Pass a **mounted native view**, usually `ref.current`, to `forView`. Set `collapsable={false}` so React Native retains that view in the native hierarchy. Call the API after the view has mounted and received its first layout.

```tsx
import { useRef, useState } from 'react';
import { Button, View } from 'react-native';
import { forView } from 'react-native-desktop-window-size';

export function App() {
  const root = useRef<View>(null);
  const [ready, setReady] = useState(false);

  async function resize() {
    if (!root.current) return;

    try {
      const window = forView(root.current);
      await window.setMinSize({ width: 480, height: 320 });
      await window.setMaxSize({ width: 1600, height: 1000 });
      await window.setSize({ width: 1024, height: 768 });
      console.log(await window.getSize());
    } catch (error) {
      console.error('Could not resize the window', error);
    }
  }

  return (
    <View
      ref={root}
      collapsable={false}
      onLayout={() => setReady(true)}
      style={{ flex: 1 }}
    >
      <Button title="Resize this window" disabled={!ready} onPress={resize} />
    </View>
  );
}
```

Each operation resolves the native window again. Create a new controller when your view is replaced, and stop using it when the view unmounts. A missing or closed window rejects the promise; the library never falls back to a different window.

## API

```ts
type WindowSize = Readonly<{ width: number; height: number }>;
const window = forView(nativeView);
```

| Method                              | Returns               | Behavior                                                          |
| ----------------------------------- | --------------------- | ----------------------------------------------------------------- |
| `window.getSize()`                  | `Promise<WindowSize>` | Read the native content size.                                     |
| `window.setSize({ width, height })` | `Promise<void>`       | Restore to normal state, then resize within the active limits.    |
| `window.maximize()`                 | `Promise<void>`       | Zoom on macOS; maximize on Windows. Repeated calls do not toggle. |
| `window.minimize()`                 | `Promise<void>`       | Minimize the window. Repeated calls do not toggle.                |
| `window.restore()`                  | `Promise<void>`       | Return from minimized or maximized/zoomed state to normal.        |
| `window.setMinSize(size)`           | `Promise<void>`       | Set both minimum dimensions; restore and clamp the current size.  |
| `window.setMaxSize(size)`           | `Promise<void>`       | Set both maximum dimensions; restore and clamp the current size.  |
| `window.setMinSize(null)`           | `Promise<void>`       | Remove the library's minimum constraint.                          |
| `window.setMaxSize(null)`           | `Promise<void>`       | Remove the library's maximum constraint.                          |

The same operations are available as named functions, with the target as their first argument:

```ts
import { getSize, setSize, maximize } from 'react-native-desktop-window-size';

await setSize(view, { width: 900, height: 600 });
const size = await getSize(view);
await maximize(view);
```

A positive React view tag is also accepted, for example one returned by `findNodeHandle`. Prefer passing the mounted view directly to avoid retaining an obsolete tag.

### Size and state semantics

- All dimensions describe the **native content/client area**, excluding ordinary window borders and title bars. They are macOS points or Windows device-independent pixels, matching React Native layout units. They do not measure an individual React view. Custom title bars and full-size content views follow the host window's native content geometry.
- Width and height must be finite numbers greater than zero and at most 1,000,000. Fractional sizes are accepted; Windows rounds to physical pixels using the target window's current DPI.
- The OS may impose additional limits, including a minimum title-bar width, screen bounds, and snapping rules. Read `getSize()` after resizing if you need the actual result.
- A minimum exceeding the maximum, or a maximum below the minimum, rejects without changing the existing constraint. When moving an entire range, clear the old bound first if it would conflict with the new one.
- Constraints belong to the native window and remain until cleared or the window is destroyed, including across React reloads. They apply to interactive resizing. On macOS, clearing a bound restores the AppKit value captured before the library first changed either bound. On Windows, clearing a bound leaves OS and host-window defaults in place.
- `maximize()` uses AppKit's native **zoom** behavior on macOS, which may choose a content-dependent size; it does not enter full screen. Windows uses the native maximize state. macOS full-screen size/state changes reject with `E_UNSUPPORTED_STATE`. Exit full screen first. Windows custom full-screen hosts are outside the supported window model.
- All native work runs on the UI thread. A resolved promise means the native operation has been issued; it does not wait for window animations or React layout to finish. `getSize()` while minimized follows the OS's native geometry and should not be used as a saved restore size.

### Errors

All operation errors are promise rejections, including JavaScript validation failures.

| Error                   | Meaning                                                        |
| ----------------------- | -------------------------------------------------------------- |
| `TypeError`             | Missing view, detached ref, or invalid React tag.              |
| `RangeError`            | Invalid dimensions supplied through the public JavaScript API. |
| `E_NO_WINDOW`           | The native view/window is no longer available.                 |
| `E_INVALID_SIZE`        | Native dimension validation failed.                            |
| `E_INVALID_CONSTRAINTS` | The requested minimum and maximum conflict.                    |
| `E_UNSUPPORTED_STATE`   | The window cannot perform the requested state change.          |
| `E_NATIVE`              | An underlying Windows operation failed.                        |

Unsupported platforms and missing native linkage reject with a descriptive `Error`.

## Development

Use Node.js 22.14+ (Node.js 24 in CI) and the pnpm version pinned in `package.json`.

```sh
corepack enable
pnpm install --frozen-lockfile
pnpm check
pnpm pack --pack-destination .native-build
node scripts/check-package.cjs
```

`pnpm check` verifies formatting, TypeScript, JavaScript API tests, the JavaScript build, and Windows code generation. Generated Windows headers are committed and checked for drift in CI.

On macOS with Xcode and CocoaPods installed:

```sh
pnpm test:native:macos
# Requires the xcodeproj Ruby gem (installed with CocoaPods) in the active Ruby environment.
bash scripts/build-macos.sh
```

The native harness builds this pod against React Native macOS. CI builds both Paper and Fabric, runs AppKit window tests, builds the Windows DLL against React Native Windows, and runs Win32 window tests. The isolated native tests exercise resizing, constraints, and window isolation; they do not replace an end-to-end app smoke test.

For manual verification, copy [`example/App.tsx`](example/App.tsx) into a linked desktop app and change its library import to `react-native-desktop-window-size`. Test two windows with focus moving between them, native window buttons, close/unmount, maximum/minimum drag limits, React reload, and Windows monitors with different DPI settings.

## Publishing

The **Publish** workflow runs on `v*` tags, reruns the full CI workflow, checks that the tag matches `package.json`, and publishes with provenance. Prerelease versions use the `next` npm dist-tag; stable versions use `latest`.

Before the first automated release:

1. Ensure you own the npm package name and publish the initial version with `pnpm publish --access public` if the package does not exist yet.
2. Configure [npm trusted publishing](https://docs.npmjs.com/trusted-publishers/) for this GitHub repository, workflow file `publish.yml`, and environment `npm`, with publishing enabled.
3. Create the `npm` GitHub environment and configure any desired release protection rules.

For subsequent releases, update `package.json` and `pnpm-lock.yaml`, commit the change, and push a matching tag such as `v0.1.1`. The workflow uses short-lived OIDC credentials; no npm token is stored in the repository. Do not push an automated-release tag for a version already published during bootstrap.

## License

MIT. See [LICENSE](LICENSE).
