# kwin-3fingers4alttab

> A KWin effect for KDE Plasma 6 that maps 3-finger touchpad swipes to the Alt+Tab window switcher.

Fork of [MrSnowball-dev/kwin-3fingers4alttab](https://github.com/MrSnowball-dev/kwin-3fingers4alttab) — fixes gesture conflicts with KWin's built-in touchpad handling and adds a no-wraparound option.

| Gesture | Action |
|---|---|
| Swipe right | Next window (Alt+Tab) |
| Swipe left | Previous window (Alt+Shift+Tab) |
| Lift fingers | Activate selected window |

---

## Requirements

- KDE Plasma 6 (Wayland)
- `kwin-dev`, `extra-cmake-modules`, `libkf6config-dev`, `libkf6kcmutils-dev`, `qt6-base-dev`

```sh
sudo apt install kwin-dev extra-cmake-modules libkf6config-dev libkf6kcmutils-dev qt6-base-dev
```

---

## Build & Install

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel

mkdir -p ~/.local/lib/x86_64-linux-gnu/plugins/kwin/effects/{plugins,configs}

mv -f build/kwin_effect_3fingers4alttab.so \
      ~/.local/lib/x86_64-linux-gnu/plugins/kwin/effects/plugins/

cp -f build/kwin_effect_3fingers4alttab_config.so \
      ~/.local/lib/x86_64-linux-gnu/plugins/kwin/effects/configs/
```

> **Note:** KWin loads plugins from `QT_PLUGIN_PATH`. Verify yours with:
> `systemctl --user show-environment | grep QT_PLUGIN_PATH`
> Some distros use `~/.local/lib/plugins` (no arch triplet) — install there instead if the effect doesn't appear in Desktop Effects.

Restart KWin: `kwin_wayland --replace &` or log out and back in.

---

## Usage

1. **Enable** — *System Settings → Desktop Effects → 3-Finger Swipe for Alt+Tab*
2. **Configure** — click the ⚙️ icon next to the effect

| Setting | Default | Description |
|---|---|---|
| Activation threshold | 40 px | Distance before the switcher opens |
| Cycle threshold | 100 px | Distance per window step |
| No wraparound | On | Stop at first/last window instead of looping |

---

## Changes from Upstream

- **Gesture conflict fix** — Converts the effect from a passive `InputEventSpy` to an `InputEventFilter`, consuming 3-finger swipes before they reach KWin's built-in desktop-switch handler. Prevents simultaneous desktop switching and window cycling.
- **No-wraparound mode** — Gesture-driven cycling stops at the list ends; keyboard Alt+Tab still wraps normally.

---

## License

GPL-2.0-or-later
