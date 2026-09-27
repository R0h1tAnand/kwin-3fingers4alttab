# kwin-3fingers4alttab

A KWin C++ effect for KDE Plasma 6 that maps 3-finger touchpad swipes to the Alt+Tab window switcher.

- Swipe **right** → next window (Alt+Tab)
- Swipe **left** → previous window (Alt+Shift+Tab)
- Continue swiping to cycle through more windows; the switcher closes and activates the selected window when you lift your fingers

## Requirements

- KDE Plasma 6 (KWin 6)
- Wayland session
- KF6 development packages: `kf6-kconfig-dev`, `kf6-kcmutils-dev`
- KWin development package: `kwin-dev`
- ECM: `extra-cmake-modules`

On Ubuntu/Debian:

```sh
sudo apt install kwin-dev extra-cmake-modules \
    libkf6config-dev libkf6kcmutils-dev \
    qt6-base-dev
```

## Build & Install

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Prepare install directories
mkdir -p ~/.local/lib/x86_64-linux-gnu/plugins/kwin/effects/plugins
mkdir -p ~/.local/lib/x86_64-linux-gnu/plugins/kwin/effects/configs

# Install effect (must use mv — cp a loaded .so crashes the desktop)
mv -f build/kwin_effect_3fingers4alttab.so \
      ~/.local/lib/x86_64-linux-gnu/plugins/kwin/effects/plugins/

# Install config UI
cp -f build/kwin_effect_3fingers4alttab_config.so \
      ~/.local/lib/x86_64-linux-gnu/plugins/kwin/effects/configs/
```

Then restart KWin:

```sh
kwin_wayland --replace &
```

Or log out and back in.

## Enable the effect

Open **System Settings → Desktop Effects**, find **3-Finger Swipe for Alt+Tab**, and enable it.

## Configuration

Click the settings icon next to the effect in Desktop Effects:

| Setting | Default | Description |
|---|---|---|
| Activation threshold | 40 px | Horizontal distance before the switcher opens |
| Cycle threshold | 100 px | Distance per window step while the switcher is open |

Changes apply immediately (no re-login required).

## License

GPL-2.0-or-later
