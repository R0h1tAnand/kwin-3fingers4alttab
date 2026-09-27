// SPDX-FileCopyrightText: 2024 kwin-3fingers4alttab contributors
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <effect/effect.h>
#include <input.h>

#include <QPointF>

namespace KWin
{

// InputEventFilter (not InputEventSpy) so we can consume the 3-finger swipe
// and stop it from also reaching GlobalShortcutFilter, which owns KWin's
// built-in 3/4-finger horizontal swipe -> desktop switch gesture.
class SwipeForAltTabEffect : public Effect, public InputEventFilter
{
    Q_OBJECT

public:
    SwipeForAltTabEffect();
    ~SwipeForAltTabEffect() override;

    bool swipeGestureBegin(PointerSwipeGestureBeginEvent *event) override;
    bool swipeGestureUpdate(PointerSwipeGestureUpdateEvent *event) override;
    bool swipeGestureEnd(PointerSwipeGestureEndEvent *event) override;
    bool swipeGestureCancelled(PointerSwipeGestureCancelEvent *event) override;

    void reconfigure(ReconfigureFlags flags) override;

private:
    void startSwitching(bool forward);
    void injectCycleKey(bool forward);
    void acceptSwitching();
    void cancelSwitching();
    void injectKeyDown(uint32_t key);
    void injectKeyUp(uint32_t key);

    enum class State { Idle, Tracking, Switching };
    State m_state = State::Idle;
    bool m_owning = false;

    QPointF m_delta;
    double m_lastCycleX = 0;
    bool m_altHeld = false;

    double m_activationThreshold = 40.0;
    double m_cycleThreshold = 100.0;
};

} // namespace KWin
