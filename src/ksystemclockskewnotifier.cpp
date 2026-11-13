/*
    SPDX-FileCopyrightText: 2019 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "ksystemclockskewnotifier.h"

#include <QSocketNotifier>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <sys/timerfd.h>
#include <unistd.h>

#ifndef TFD_TIMER_CANCEL_ON_SET // only available in newer glibc
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)
#endif

namespace GxdeNightTime
{

KSystemClockSkewNotifier::KSystemClockSkewNotifier(QObject *parent)
    : QObject(parent)
{
}

KSystemClockSkewNotifier::~KSystemClockSkewNotifier()
{
    unload();
}

bool KSystemClockSkewNotifier::isActive() const
{
    return m_isActive;
}

void KSystemClockSkewNotifier::setActive(bool active)
{
    if (m_isActive == active) {
        return;
    }

    m_isActive = active;
    if (m_isActive) {
        load();
    } else {
        unload();
    }

    Q_EMIT activeChanged();
}

void KSystemClockSkewNotifier::load()
{
    const int fd = timerfd_create(CLOCK_REALTIME, O_CLOEXEC | O_NONBLOCK);
    if (fd == -1) {
        qWarning("Couldn't create clock skew notifier engine: %s", strerror(errno));
        return;
    }

    const itimerspec spec = {};
    if (timerfd_settime(fd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &spec, nullptr) == -1) {
        qWarning("Couldn't create clock skew notifier engine: %s", strerror(errno));
        close(fd);
        return;
    }

    m_fd = fd;
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &KSystemClockSkewNotifier::handleTimerCancelled);
}

void KSystemClockSkewNotifier::unload()
{
    delete m_notifier;
    m_notifier = nullptr;

    if (m_fd != -1) {
        close(m_fd);
        m_fd = -1;
    }
}

void KSystemClockSkewNotifier::handleTimerCancelled()
{
    uint64_t expirationCount;
    [[maybe_unused]] const ssize_t ret = read(m_fd, &expirationCount, sizeof(expirationCount));

    Q_EMIT skewed();
}

} // namespace GxdeNightTime

#include "moc_ksystemclockskewnotifier.cpp"
