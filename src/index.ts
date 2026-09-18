import { findNodeHandle, Platform } from 'react-native';
import NativeDesktopWindowSize from './NativeDesktopWindowSize';

/** Content size in points (macOS) or device-independent pixels (Windows). */
export type WindowSize = Readonly<{ width: number; height: number }>;

/** A mounted native view (ref.current), or its React tag. */
export type WindowTarget = NonNullable<Parameters<typeof findNodeHandle>[0]>;

export interface DesktopWindow {
  getSize(): Promise<WindowSize>;
  setSize(size: WindowSize): Promise<void>;
  maximize(): Promise<void>;
  minimize(): Promise<void>;
  restore(): Promise<void>;
  /** Pass null to remove the library's minimum size constraint. */
  setMinSize(size: WindowSize | null): Promise<void>;
  /** Pass null to remove the library's maximum size constraint. */
  setMaxSize(size: WindowSize | null): Promise<void>;
}

function nativeModule() {
  if (Platform.OS !== 'macos' && Platform.OS !== 'windows') {
    throw new Error(
      'react-native-desktop-window-size supports only macOS and Windows.',
    );
  }
  if (!NativeDesktopWindowSize) {
    throw new Error(
      'DesktopWindowSize is not linked. Install the native dependencies and rebuild your desktop app.',
    );
  }
  return NativeDesktopWindowSize;
}

function tagFor(target: WindowTarget): number {
  if (target == null) {
    throw new TypeError('A mounted native view or React tag is required.');
  }
  const tag = typeof target === 'number' ? target : findNodeHandle(target);
  if (tag == null || !Number.isSafeInteger(tag) || tag <= 0) {
    throw new TypeError(
      'The window target must resolve to a mounted native view.',
    );
  }
  return tag;
}

function validateSize(size: WindowSize): void {
  if (
    size == null ||
    !Number.isFinite(size.width) ||
    !Number.isFinite(size.height) ||
    size.width <= 0 ||
    size.height <= 0 ||
    size.width > 1_000_000 ||
    size.height > 1_000_000
  ) {
    throw new RangeError(
      'Window width and height must be finite numbers greater than 0 and at most 1,000,000.',
    );
  }
}

export async function getSize(target: WindowTarget): Promise<WindowSize> {
  return nativeModule().getSize(tagFor(target));
}

export async function setSize(
  target: WindowTarget,
  size: WindowSize,
): Promise<void> {
  validateSize(size);
  await nativeModule().setSize(tagFor(target), size.width, size.height);
}

export async function maximize(target: WindowTarget): Promise<void> {
  await nativeModule().maximize(tagFor(target));
}

export async function minimize(target: WindowTarget): Promise<void> {
  await nativeModule().minimize(tagFor(target));
}

export async function restore(target: WindowTarget): Promise<void> {
  await nativeModule().restore(tagFor(target));
}

export async function setMinSize(
  target: WindowTarget,
  size: WindowSize | null,
): Promise<void> {
  if (size !== null) validateSize(size);
  await nativeModule().setMinSize(
    tagFor(target),
    size?.width ?? null,
    size?.height ?? null,
  );
}

export async function setMaxSize(
  target: WindowTarget,
  size: WindowSize | null,
): Promise<void> {
  if (size !== null) validateSize(size);
  await nativeModule().setMaxSize(
    tagFor(target),
    size?.width ?? null,
    size?.height ?? null,
  );
}

/** Bind operations to a mounted view; the native window is resolved on each call. */
export function forView(target: WindowTarget): DesktopWindow {
  // Retain the view, not a cached key/foreground window or HWND.
  return {
    getSize: () => getSize(target),
    setSize: (size) => setSize(target, size),
    maximize: () => maximize(target),
    minimize: () => minimize(target),
    restore: () => restore(target),
    setMinSize: (size) => setMinSize(target, size),
    setMaxSize: (size) => setMaxSize(target, size),
  };
}
