import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  getSize(viewTag: number): Promise<{ width: number; height: number }>;
  setSize(viewTag: number, width: number, height: number): Promise<void>;
  maximize(viewTag: number): Promise<void>;
  minimize(viewTag: number): Promise<void>;
  restore(viewTag: number): Promise<void>;
  setMinSize(
    viewTag: number,
    width: number | null,
    height: number | null,
  ): Promise<void>;
  setMaxSize(
    viewTag: number,
    width: number | null,
    height: number | null,
  ): Promise<void>;
}

export default TurboModuleRegistry.get<Spec>('DesktopWindowSize');
