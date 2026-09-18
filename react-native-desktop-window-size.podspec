require 'json'

package = JSON.parse(File.read(File.join(__dir__, 'package.json')))

Pod::Spec.new do |s|
  s.name = 'react-native-desktop-window-size'
  s.version = package['version']
  s.summary = package['description']
  s.homepage = package['homepage']
  s.license = package['license']
  s.authors = { 'Natsuneko' => 'https://github.com/mika-f' }
  s.source = { :git => package['repository']['url'], :tag => "v#{s.version}" }
  s.platform = :osx, '14.0'
  s.source_files = 'macos/**/*.{h,m,mm}'
  s.frameworks = 'AppKit'
  install_modules_dependencies(s)
end
