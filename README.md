# kwin-3fingers4alttab

A KWin C++ effect for KDE Plasma 6 that maps 3-finger touchpad swipes to the Alt+Tab window switcher.

This is a fork of [MrSnowball-dev/kwin-3fingers4alttab](https://github.com/MrSnowball-dev/kwin-3fingers4alttab)
with fixes for a conflict with KWin's built-in touchpad gestures, plus a
no-wraparound option for gesture-driven cycling. See [Changes from upstream](#changes-from-upstream).

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

> **Note:** KWin only loads plugins from directories on its `QT_PLUGIN_PATH`.
> On most distros that includes the arch-triplet path above, but some (e.g.
> Kali's Plasma packaging) instead set `QT_PLUGIN_PATH` to
> `~/.local/lib/plugins` (no arch triplet). Check yours with:
> `systemctl --user show-environment | grep QT_PLUGIN_PATH`, and install to
> `~/.local/lib/plugins/kwin/effects/{plugins,configs}` instead if that's
> what it shows. If the effect doesn't appear in Desktop Effects after
> restarting KWin, this is the first thing to check.

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
| Stop at first/last window instead of wrapping around | On | Gesture-driven cycling stays at the last/first window instead of looping back around when you keep swiping past the end. Keyboard Alt+Tab is unaffected and always wraps. |

Changes apply immediately (no re-login required).

## Changes from upstream

- **Fixed a conflict with KWin's built-in touchpad gestures.** KWin core
  unconditionally registers *both* 3-finger and 4-finger horizontal swipes
  for virtual desktop switching. Upstream only installed a passive
  `InputEventSpy`, so a 3-finger swipe would switch desktops *and* cycle
  windows at the same time. This fork converts the effect to an
  `InputEventFilter` ordered ahead of `GlobalShortcutFilter` and consumes
  the 3-finger gesture, so only 4-finger swipes reach the built-in
  desktop-switch action.
- **Stop at the ends of the window list instead of wrapping around** when
  cycling via the gesture (configurable, see above). Keyboard Alt+Tab
  still wraps normally.

## License

GPL-2.0-or-later
