// SPDX-FileCopyrightText: 2024 kwin-3fingers4alttab contributors
// SPDX-License-Identifier: GPL-2.0-or-later

#include "effect.h"

#include <effect/effecthandler.h>
#include <input.h>
#include <input_event.h>
#include <keyboard_input.h>
#include <core/inputdevice.h>

#include <KConfig>
#include <KConfigGroup>
#include <QTimer>
#include <cmath>
#include <linux/input-event-codes.h>

namespace KWin
{

SwipeForAltTabEffect::SwipeForAltTabEffect()
    : InputEventFilter(InputFilterOrder::TabBox)
{
    input()->installInputEventFilter(this);
    reconfigure(ReconfigureAll);
}

void SwipeForAltTabEffect::reconfigure(ReconfigureFlags)
{
    KConfig config(QStringLiteral("kwin-3fingers4alttabrc"), KConfig::SimpleConfig);
    KConfigGroup cfg = config.group(QStringLiteral("General"));
    m_activationThreshold = cfg.readEntry("ActivationThreshold", 40.0);
    m_cycleThreshold      = cfg.readEntry("CycleThreshold",      100.0);
}

SwipeForAltTabEffect::~SwipeForAltTabEffect()
{
    if (m_altHeld)
        cancelSwitching();
}

bool SwipeForAltTabEffect::swipeGestureBegin(PointerSwipeGestureBeginEvent *event)
{
    if (event->fingerCount != 3 || m_state != State::Idle)
        return false;
    m_state = State::Tracking;
    m_owning = true;
    m_delta = {};
    m_lastCycleX = 0;
    return true;
}

bool SwipeForAltTabEffect::swipeGestureUpdate(PointerSwipeGestureUpdateEvent *event)
{
    // Once we've claimed a 3-finger gesture at Begin, keep consuming its
    // events for its whole lifetime (even if we internally give up on
    // recognizing it as a horizontal swipe) so it's never partially leaked
    // to GlobalShortcutFilter downstream.
    if (!m_owning)
        return false;

    m_delta += event->delta;

    const double ax = std::abs(m_delta.x());
    const double ay = std::abs(m_delta.y());

    if (m_state == State::Tracking) {
        if (ax > m_activationThreshold && ax > ay * 1.2) {
            startSwitching(m_delta.x() > 0);
            m_state = State::Switching;
            m_lastCycleX = m_delta.x();
        } else if (ay > m_activationThreshold && ay > ax * 1.2) {
            m_state = State::Idle;
        }
    } else if (m_state == State::Switching) {
        const double traveled = m_delta.x() - m_lastCycleX;
        if (std::abs(traveled) >= m_cycleThreshold) {
            injectCycleKey(traveled > 0);
            m_lastCycleX = m_delta.x();
        }
    }
    return true;
}

bool SwipeForAltTabEffect::swipeGestureEnd(PointerSwipeGestureEndEvent *event)
{
    Q_UNUSED(event)
    if (!m_owning)
        return false;
    if (m_state == State::Switching)
        acceptSwitching();
    m_state = State::Idle;
    m_owning = false;
    return true;
}

bool SwipeForAltTabEffect::swipeGestureCancelled(PointerSwipeGestureCancelEvent *event)
{
    Q_UNUSED(event)
    if (!m_owning)
        return false;
    if (m_state == State::Switching)
        cancelSwitching();
    m_state = State::Idle;
    m_owning = false;
    return true;
}

// Inject Alt + [Shift +] Tab to open the task switcher and take one step.
// Deferred: processKey() must not be called re-entrantly from within a spy callback.
void SwipeForAltTabEffect::startSwitching(bool forward)
{
    m_altHeld = true;
    QTimer::singleShot(0, this, [this, forward]() {
        injectKeyDown(KEY_LEFTALT);
        if (!forward)
            injectKeyDown(KEY_LEFTSHIFT);
        injectKeyDown(KEY_TAB);
        injectKeyUp(KEY_TAB);
        if (!forward)
            injectKeyUp(KEY_LEFTSHIFT);
    });
}

// While the switcher is open (Alt held), inject [Shift +] Tab to cycle one step.
// Stops at the last/first window instead of wrapping around when the swipe
// keeps going past the end of the list (keyboard Alt+Tab still wraps normally
// since this clamp only applies to gesture-driven cycling).
void SwipeForAltTabEffect::injectCycleKey(bool forward)
{
    const QList<EffectWindow *> windows = effects->currentTabBoxWindowList();
    EffectWindow *current = effects->currentTabBoxWindow();
    if (!windows.isEmpty() && current) {
        const int idx = windows.indexOf(current);
        if (idx >= 0) {
            if (forward && idx == windows.size() - 1)
                return;
            if (!forward && idx == 0)
                return;
        }
    }

    QTimer::singleShot(0, this, [this, forward]() {
        if (!forward)
            injectKeyDown(KEY_LEFTSHIFT);
        injectKeyDown(KEY_TAB);
        injectKeyUp(KEY_TAB);
        if (!forward)
            injectKeyUp(KEY_LEFTSHIFT);
    });
}

// Release Alt — the switcher accepts the highlighted window and closes.
void SwipeForAltTabEffect::acceptSwitching()
{
    if (!m_altHeld)
        return;
    m_altHeld = false;
    QTimer::singleShot(0, this, [this]() {
        injectKeyUp(KEY_LEFTALT);
    });
}

// Cancel via Escape then release Alt — reverts to original window.
void SwipeForAltTabEffect::cancelSwitching()
{
    if (!m_altHeld)
        return;
    m_altHeld = false;
    QTimer::singleShot(0, this, [this]() {
        injectKeyDown(KEY_ESC);
        injectKeyUp(KEY_ESC);
        injectKeyUp(KEY_LEFTALT);
    });
}

void SwipeForAltTabEffect::injectKeyDown(uint32_t key)
{
    input()->keyboard()->processKey(key, KeyboardKeyState::Pressed, std::chrono::microseconds(0));
}

void SwipeForAltTabEffect::injectKeyUp(uint32_t key)
{
    input()->keyboard()->processKey(key, KeyboardKeyState::Released, std::chrono::microseconds(0));
}

} // namespace KWin
