/*
    SPDX-FileCopyrightText: 2019 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

// Private fallback for KSystemClockSkewNotifier, which only exists in KCoreAddons >= 6.15.
// Used when building against older KF6 (e.g. Debian trixie / GXDE 25 ships 6.13).
// Linux-only port of the timerfd engine from kcoreaddons/src/lib/util.

#pragma once

#include <QObject>

class QSocketNotifier;

class KSystemClockSkewNotifier : public QObject
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
