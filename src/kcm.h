// SPDX-FileCopyrightText: 2024 kwin-3fingerswipe4alttab contributors
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <KCModule>

class QSpinBox;

class SwipeConfigModule : public KCModule
{
    Q_OBJECT

public:
    SwipeConfigModule(QObject *parent, const KPluginMetaData &data);

    void load() override;
    void save() override;
    void defaults() override;

private:
    QSpinBox *m_activationSpin = nullptr;
    QSpinBox *m_cycleSpin = nullptr;
};
