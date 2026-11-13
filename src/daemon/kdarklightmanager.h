/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "kdarklightscheduler.h"

#include <KConfigWatcher>
// Provided by KCoreAddons >= 6.15, or by this library on older KF6. The
// fallback is a using-declaration, so it cannot be forward declared.
#include <KSystemClockSkewNotifier>

#include <QGeoPositionInfoSource>
#include <QTimer>

class KDarkLightManagerInterface;
class KDarkLightSettings;
class KDarkLightState;

class KDarkLightManager : public QObject
{
    Q_OBJECT

public:
    explicit KDarkLightManager(QObject *parent = nullptr);
    ~KDarkLightManager() override;

    KDarkLightSchedule schedule() const;

    void reconfigure();
    void reschedule();

Q_SIGNALS:
    void scheduleChanged();

private:
    KConfigWatcher::Ptr m_configWatcher;
    std::unique_ptr<KDarkLightManagerInterface> m_dbusInterface;
    std::unique_ptr<KDarkLightSettings> m_settings;
    std::unique_ptr<KDarkLightState> m_state;
    std::unique_ptr<KDarkLightScheduler> m_scheduler;
    std::unique_ptr<KSystemClockSkewNotifier> m_skewNotifier;
    std::unique_ptr<QGeoPositionInfoSource> m_positionInfoSource;
    std::unique_ptr<QTimer> m_scheduleTimer;
    KDarkLightSchedule m_schedule;
};
