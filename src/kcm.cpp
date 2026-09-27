// SPDX-FileCopyrightText: 2024 kwin-3fingers4alttab contributors
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kcm.h"

#include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QFormLayout>
#include <QSpinBox>

K_PLUGIN_CLASS_WITH_JSON(SwipeConfigModule, "kcm_metadata.json")

SwipeConfigModule::SwipeConfigModule(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    auto *layout = new QFormLayout(widget());

    m_activationSpin = new QSpinBox(widget());
    m_activationSpin->setRange(10, 200);
    m_activationSpin->setSuffix(QStringLiteral(" px"));
    m_activationSpin->setKeyboardTracking(false);
    m_activationSpin->setToolTip(QStringLiteral("How far to swipe horizontally before the switcher opens"));
    layout->addRow(QStringLiteral("Activation threshold:"), m_activationSpin);

    m_cycleSpin = new QSpinBox(widget());
    m_cycleSpin->setRange(20, 500);
    m_cycleSpin->setSuffix(QStringLiteral(" px"));
    m_cycleSpin->setKeyboardTracking(false);
    m_cycleSpin->setToolTip(QStringLiteral("How far to swipe to advance one window in the switcher"));
    layout->addRow(QStringLiteral("Cycle threshold:"), m_cycleSpin);

    connect(m_activationSpin, &QSpinBox::valueChanged, this, [this] { setNeedsSave(true); });
    connect(m_cycleSpin,      &QSpinBox::valueChanged, this, [this] { setNeedsSave(true); });
}

void SwipeConfigModule::load()
{
    KConfigGroup cfg = KSharedConfig::openConfig(QStringLiteral("kwin-3fingers4alttabrc"))
                           ->group(QStringLiteral("General"));
    m_activationSpin->setValue(cfg.readEntry("ActivationThreshold", 40));
    m_cycleSpin->setValue(cfg.readEntry("CycleThreshold", 100));
    setNeedsSave(false);
}

void SwipeConfigModule::save()
{
    KConfigGroup cfg = KSharedConfig::openConfig(QStringLiteral("kwin-3fingers4alttabrc"))
                           ->group(QStringLiteral("General"));
    cfg.writeEntry("ActivationThreshold", m_activationSpin->value());
    cfg.writeEntry("CycleThreshold", m_cycleSpin->value());
    cfg.sync();
    setNeedsSave(false);

    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/Effects"),
        QStringLiteral("org.kde.kwin.Effects"),
        QStringLiteral("reconfigureEffect"));
    msg << QStringLiteral("kwin_effect_3fingers4alttab");
    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
}

void SwipeConfigModule::defaults()
{
    m_activationSpin->setValue(40);
    m_cycleSpin->setValue(100);
}

#include "kcm.moc"
