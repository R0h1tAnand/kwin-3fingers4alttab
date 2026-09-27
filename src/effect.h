// SPDX-FileCopyrightText: 2024 kwin-3fingers4alttab contributors
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <effect/effect.h>
#include <input.h>
#include <input_event_spy.h>

#include <QPointF>

namespace KWin
{

class SwipeForAltTabEffect : public Effect, public InputEventSpy
{
    Q_OBJECT

public:
    SwipeForAltTabEffect();
    ~SwipeForAltTabEffect() override;

    void swipeGestureBegin(PointerSwipeGestureBeginEvent *event) override;
    void swipeGestureUpdate(PointerSwipeGestureUpdateEvent *event) override;
    void swipeGestureEnd(PointerSwipeGestureEndEvent *event) override;
    void swipeGestureCancelled(PointerSwipeGestureCancelEvent *event) override;

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

    QPointF m_delta;
    double m_lastCycleX = 0;
    bool m_altHeld = false;

    double m_activationThreshold = 40.0;
    double m_cycleThreshold = 100.0;
};

} // namespace KWin
