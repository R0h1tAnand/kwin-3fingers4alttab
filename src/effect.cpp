// SPDX-FileCopyrightText: 2024 kwin-3fingers4alttab contributors
// SPDX-License-Identifier: GPL-2.0-or-later

#include "effect.h"

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
{
    input()->installInputEventSpy(this);
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

void SwipeForAltTabEffect::swipeGestureBegin(PointerSwipeGestureBeginEvent *event)
{
    if (event->fingerCount != 3 || m_state != State::Idle)
        return;
    m_state = State::Tracking;
    m_delta = {};
    m_lastCycleX = 0;
}

void SwipeForAltTabEffect::swipeGestureUpdate(PointerSwipeGestureUpdateEvent *event)
{
    if (m_state == State::Idle)
        return;

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
}

void SwipeForAltTabEffect::swipeGestureEnd(PointerSwipeGestureEndEvent *event)
{
    Q_UNUSED(event)
    if (m_state == State::Switching)
        acceptSwitching();
    m_state = State::Idle;
}

void SwipeForAltTabEffect::swipeGestureCancelled(PointerSwipeGestureCancelEvent *event)
{
    Q_UNUSED(event)
    if (m_state == State::Switching)
        cancelSwitching();
    m_state = State::Idle;
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
void SwipeForAltTabEffect::injectCycleKey(bool forward)
{
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
