import { beforeEach, describe, expect, it, vi } from 'vitest';
import type { WindowSize } from '../src';

const mock = vi.hoisted(() => ({
  platform: { OS: 'macos' },
  findNodeHandle: vi.fn(),
  native: {
    getSize: vi.fn(),
    setSize: vi.fn(),
    maximize: vi.fn(),
    minimize: vi.fn(),
    restore: vi.fn(),
    setMinSize: vi.fn(),
    setMaxSize: vi.fn(),
  },
  linked: true,
}));

vi.mock('react-native', () => ({
  Platform: mock.platform,
  findNodeHandle: mock.findNodeHandle,
  TurboModuleRegistry: { get: () => (mock.linked ? mock.native : null) },
}));

beforeEach(() => {
  vi.resetModules();
  vi.clearAllMocks();
  mock.linked = true;
  mock.platform.OS = 'macos';
  mock.findNodeHandle.mockReturnValue(42);
  mock.native.getSize.mockResolvedValue({ width: 800, height: 600 });
});

describe('window API', () => {
  it.each(['macos', 'windows'])(
    'resolves the caller view on %s',
    async (platform) => {
      mock.platform.OS = platform;
      const { forView } = await import('../src');
      const view = {} as Parameters<typeof forView>[0];
      const window = forView(view);
      expect(await window.getSize()).toEqual({ width: 800, height: 600 });
      expect(mock.native.getSize).toHaveBeenCalledWith(42);
      mock.findNodeHandle.mockReturnValue(88);
      await window.setSize({ width: 1024.5, height: 768 });
      expect(mock.native.setSize).toHaveBeenCalledWith(88, 1024.5, 768);
    },
  );

  it('keeps separate windows targeted even when focus changes', async () => {
    const api = await import('../src');
    const first = api.forView(10);
    const second = api.forView(20);
    await first.maximize();
    await second.minimize();
    await first.restore();
    expect(mock.native.maximize).toHaveBeenCalledWith(10);
    expect(mock.native.minimize).toHaveBeenCalledWith(20);
    expect(mock.native.restore).toHaveBeenCalledWith(10);
  });

  it('supports setting and clearing both constraints', async () => {
    const { forView } = await import('../src');
    const window = forView(1);
    await window.setMinSize({ width: 320, height: 240 });
    await window.setMaxSize({ width: 1920, height: 1080 });
    await window.setMinSize(null);
    await window.setMaxSize(null);
    expect(mock.native.setMinSize.mock.calls).toEqual([
      [1, 320, 240],
      [1, null, null],
    ]);
    expect(mock.native.setMaxSize.mock.calls).toEqual([
      [1, 1920, 1080],
      [1, null, null],
    ]);
  });

  it.each([NaN, Infinity, -Infinity, 0, -1, 1_000_001, '800', undefined])(
    'rejects invalid dimensions %s without a native call',
    async (width) => {
      const api = await import('../src');
      const size = { width, height: 600 } as WindowSize;
      await expect(api.setSize(1, size)).rejects.toBeInstanceOf(RangeError);
      await expect(api.setMinSize(1, size)).rejects.toBeInstanceOf(RangeError);
      await expect(
        api.setMaxSize(1, { width: 600, height: width } as WindowSize),
      ).rejects.toBeInstanceOf(RangeError);
      expect(mock.native.setSize).not.toHaveBeenCalled();
      expect(mock.native.setMinSize).not.toHaveBeenCalled();
      expect(mock.native.setMaxSize).not.toHaveBeenCalled();
    },
  );

  it.each([
    null,
    undefined,
    0,
    -1,
    NaN,
    Infinity,
    1.5,
    Number.MAX_SAFE_INTEGER + 1,
  ])('rejects invalid targets %s asynchronously', async (target) => {
    const { getSize } = await import('../src');
    await expect(getSize(target as number)).rejects.toBeInstanceOf(TypeError);
    expect(mock.native.getSize).not.toHaveBeenCalled();
  });

  it('rejects detached views', async () => {
    const { forView } = await import('../src');
    mock.findNodeHandle.mockReturnValue(null);
    await expect(
      forView({} as Parameters<typeof forView>[0]).getSize(),
    ).rejects.toThrow('mounted');
  });

  it('preserves native errors such as conflicting limits and closed windows', async () => {
    const { setMinSize } = await import('../src');
    const error = Object.assign(new Error('Minimum exceeds maximum'), {
      code: 'E_INVALID_CONSTRAINTS',
    });
    mock.native.setMinSize.mockRejectedValueOnce(error);
    await expect(setMinSize(1, { width: 900, height: 800 })).rejects.toBe(
      error,
    );
  });

  it('can be imported on unsupported platforms but rejects operations', async () => {
    mock.platform.OS = 'ios';
    const { getSize } = await import('../src');
    await expect(getSize(1)).rejects.toThrow('only macOS and Windows');
  });

  it('explains how to fix a missing native module', async () => {
    mock.linked = false;
    const { getSize } = await import('../src');
    await expect(getSize(1)).rejects.toThrow('rebuild your desktop app');
  });
});
