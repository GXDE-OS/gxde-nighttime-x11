/*
    SPDX-FileCopyrightText: 2019 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

// Fallback for KSystemClockSkewNotifier, which only exists in KCoreAddons >= 6.15.
// It is built and installed only when gxde-nighttime-x11 is built against an
// older KF6 (e.g. Debian trixie / GXDE 25 ships 6.13), so that users of this
// library (gxde-nighttime-x11d, gxde-kwin_x11) can rely on it there.
//
// The class lives in its own namespace so that its exported symbols never clash
// with the real class should a newer KCoreAddons be loaded into the same
// process. The using-declaration below keeps the KCoreAddons spelling working.
// Consequence: include this header instead of forward declaring
// "class KSystemClockSkewNotifier;".
//
// Linux-only port of the timerfd engine from kcoreaddons/src/lib/util.

#pragma once

#include "knighttime_export.h"

#include <QObject>

class QSocketNotifier;

namespace GxdeNightTime
{

/*!
 * \class KSystemClockSkewNotifier
 * \inmodule KNightTime
 *
 * \brief Notifies when the system clock jumps, e.g. because it was set
 * manually or the system resumed from suspend.
 *
 * API compatible with the class of the same name in KCoreAddons 6.15.
 */
class KNIGHTTIME_EXPORT KSystemClockSkewNotifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

public:
    explicit KSystemClockSkewNotifier(QObject *parent = nullptr);
    ~KSystemClockSkewNotifier() override;

    bool isActive() const;
    void setActive(bool active);

Q_SIGNALS:
    void activeChanged();
    void skewed();

private:
    void load();
    void unload();
    void handleTimerCancelled();

    QSocketNotifier *m_notifier = nullptr;
    int m_fd = -1;
    bool m_isActive = false;
};

} // namespace GxdeNightTime

using GxdeNightTime::KSystemClockSkewNotifier;
