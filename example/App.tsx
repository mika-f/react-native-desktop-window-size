import { useRef, useState } from 'react';
import { Button, ScrollView, StyleSheet, Text, View } from 'react-native';
import { forView, type DesktopWindow } from '../src';

/** Copy into a linked macOS or Windows app to exercise the native APIs. */
export default function App() {
  const view = useRef<View>(null);
  const [status, setStatus] = useState('Ready');

  async function run(action: (window: DesktopWindow) => Promise<unknown>) {
    if (!view.current) return;
    try {
      const window = forView(view.current);
      await action(window);
      const size = await window.getSize();
      setStatus(`${size.width.toFixed(1)} × ${size.height.toFixed(1)}`);
    } catch (error) {
      setStatus(error instanceof Error ? error.message : String(error));
    }
  }

  return (
    <View ref={view} collapsable={false} style={styles.root}>
      <ScrollView contentContainerStyle={styles.content}>
        <Text accessibilityRole="header" style={styles.title}>
          Desktop window size
        </Text>
        <Text selectable accessibilityLiveRegion="polite">
          {status}
        </Text>
        <Button
          title="Read size"
          onPress={() => void run((window) => window.getSize())}
        />
        <Button
          title="Resize to 1024 × 768"
          onPress={() =>
            void run((window) => window.setSize({ width: 1024, height: 768 }))
          }
        />
        <Button
          title="Minimum 480 × 320"
          onPress={() =>
            void run((window) => window.setMinSize({ width: 480, height: 320 }))
          }
        />
        <Button
          title="Maximum 1280 × 900"
          onPress={() =>
            void run((window) =>
              window.setMaxSize({ width: 1280, height: 900 }),
            )
          }
        />
        <Button
          title="Clear limits"
          onPress={() =>
            void run(async (window) => {
              await window.setMinSize(null);
              await window.setMaxSize(null);
            })
          }
        />
        <Button
          title="Maximize"
          onPress={() => void run((window) => window.maximize())}
        />
        <Button
          title="Minimize"
          onPress={() => void run((window) => window.minimize())}
        />
        <Button
          title="Restore"
          onPress={() => void run((window) => window.restore())}
        />
      </ScrollView>
    </View>
  );
}

const styles = StyleSheet.create({
  root: { flex: 1 },
  content: { padding: 24, gap: 12 },
  title: { fontSize: 24, fontWeight: '600' },
});
