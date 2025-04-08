# How to install Flutter and its dependencies

Note, that Flutter's installation process does not automatically install Android Studio, but it does recommend installing it as part of the setup.

Installing Flutter on Windows, macOS, and Linux follows a similar process, with slight variations depending on the operating system. Here's a general guide:

## Windows
1. Download Flutter SDK – Get the Flutter SDK from the [official Flutter website](https://docs.flutter.dev/get-started/install).
1. Extract the ZIP file – Place it in a suitable location (e.g., C:\flutter).
1. Update your system PATH – Add C:\flutter\bin to your environment variables.
1. Run Flutter Doctor – Open Command Prompt and run flutter doctor to check dependencies.
1. Install Android Studio – Required for Android development.
1. Set up an editor – VS Code or Android Studio are good choices.

## macOS
1. Download Flutter SDK – Get it from the [official Flutter website](https://docs.flutter.dev/get-started/install).
1. Extract the file – Move it to /Users/YourUsername/flutter.
1. Update your PATH – Run export PATH="$PATH:/Users/YourUsername/flutter/bin" in Terminal.
1. Run Flutter Doctor – Check dependencies with flutter doctor.
1. Install Xcode – Required for iOS development.
1. Install Android Studio – Needed for Android development.
1. Set up an editor – VS Code or Android Studio work well.

## Linux
1. Download Flutter SDK – Get it from the [official Flutter website](https://docs.flutter.dev/get-started/install).
1. Extract the file – Place it in a suitable directory.
1. Update your PATH – Add Flutter’s bin directory to your system PATH.
1. Run Flutter Doctor – Check dependencies with flutter doctor.
1. Install Android Studio – Required for Android development.
1. Set up an editor – VS Code or Android Studio are good choices.

## Build a debugable Flutter app to an Emulator

Optional: To build the Flutter app:
```bash
flutter build apk --debug
```

If you suspect something is complete wrong with version, etc, do a clean of the build environment:
```bash
flutter clean
```

## Debug the Flutter app on an Emulator

1. Using CLI (emulator tool):
List available AVDs:
```bash
emulator -list-avds
# or
flutter devices
```
This will show all preconfigured emulator devices.

2. Start a specific emulator:
```bash
emulator -avd <your_avd_name>
# or
flutter emulators --launch <your_avd_name>
```
Replace <your_avd_name> with the name of the emulator from the list.

3. Debug/Start app:
```bash
flutter run -d <emulator-id>
```

## Debug the Flutter app on an Android device

To enable development testing on your Android device, follow these steps:

1. Enable Developer Mode
Open Settings on your Android device.

Go to About phone.

Find Build Number and tap it seven times until you see a message saying, "You are now a developer!".

2. Enable USB Debugging
Go to Settings -> Developer options.

Scroll down and turn on USB debugging.

Connect your device to your computer via USB.

If prompted, allow debugging access.

3. Open a terminal or command prompt and run:
```bash
adb devices
```

4. This will list connected devices.
If your device appears, you’re ready to test apps:
```bash
flutter run -d <device-id>
```

# Getting Started in general for Flutter apps

A few resources to get you started if this is your first Flutter project:

- [Lab: Write your first Flutter app](https://docs.flutter.dev/get-started/codelab)
- [Cookbook: Useful Flutter samples](https://docs.flutter.dev/cookbook)

For help getting started with Flutter development, view the
[online documentation](https://docs.flutter.dev/), which offers tutorials,
samples, guidance on mobile development, and a full API reference.
