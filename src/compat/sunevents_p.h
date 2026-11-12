/*
    SPDX-FileCopyrightText: 2012 Allen Winter <winter@kde.org>
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

// Private fallback for KHolidays::SunEvents, which only exists in KHolidays >= 6.15.
// Used when building against older KF6 (e.g. Debian trixie / GXDE 25 ships 6.13).
// Ported from kholidays/src/sunevents.cpp; keep in sync with upstream.

#pragma once

#include <QDateTime>
#include <QtMath>

#include <algorithm>
#include <cmath>

namespace KNightTimeCompat
{

namespace SunEventsDetail
{

// Formulas are taken from https://gml.noaa.gov/grad/solcalc/calcdetails.html

inline constexpr double secondsPerDay = 86400.0;
inline constexpr double j2000 = 2451545.0;
inline constexpr double julianUnixEpoch = 2440587.5;
inline constexpr double julianYear = 365.25;
inline constexpr double julianCentury = julianYear * 100;

inline constexpr double astronomicalTwilightElevation = -18.0;
inline constexpr double nauticalTwilightElevation = -12.0;
inline constexpr double civilTwilightElevation = -6.0;
inline constexpr double sunriseOrSunsetElevation = -0.833;

inline QDateTime julianDateToDateTime(double jd)
{
    return QDateTime::fromSecsSinceEpoch(std::round((jd - julianUnixEpoch) * secondsPerDay));
}

inline double dateTimeToJulianDate(const QDateTime &dateTime)
{
    return dateTime.toSecsSinceEpoch() / secondsPerDay + julianUnixEpoch;
}

inline double julianDateToJulianCenturies(double jd)
{
    return (jd - j2000) / julianCentury;
}

inline double sunGeometricMeanLongitude(double jcent)
{
    return qDegreesToRadians(std::fmod(280.46646 + jcent * (36000.76983 + jcent * 0.0003032), 360));
}

inline double sunGeometricMeanAnomaly(double jcent)
{
    return qDegreesToRadians(357.52911 + jcent * (35999.05029 - 0.0001537 * jcent));
}

inline double earthOrbitEccentricity(double jcent)
{
    return 0.016708634 - jcent * (0.000042037 + 0.0000001267 * jcent);
}

inline double equationOfCenter(double jcent)
{
    const double anomaly = sunGeometricMeanAnomaly(jcent);
    return qDegreesToRadians(std::sin(anomaly) * (1.914602 - jcent * (0.004817 + 0.000014 * jcent)) + std::sin(2 * anomaly) * (0.019993 - 0.000101 * jcent)
                             + std::sin(3 * anomaly) * 0.000289);
}

inline double sunTrueLongitude(double jcent)
{
    return sunGeometricMeanLongitude(jcent) + equationOfCenter(jcent);
}

inline double sunApparentLongitude(double jcent)
{
    return qDegreesToRadians(qRadiansToDegrees(sunTrueLongitude(jcent)) - 0.00569 - 0.00478 * std::sin(qDegreesToRadians(125.04 - 1934.136 * jcent)));
}

inline double obliquityCorrection(double jcent)
{
    const double meanEclipticObliquity = 23 + (26 + ((21.448 - jcent * (46.815 + jcent * (0.00059 - jcent * 0.001813)))) / 60) / 60;
    return qDegreesToRadians(meanEclipticObliquity + 0.00256 * std::cos(qDegreesToRadians(125.04 - 1934.136 * jcent)));
}

inline double sunDeclination(double jcent)
{
    return std::asin(std::sin(obliquityCorrection(jcent)) * std::sin(sunApparentLongitude(jcent)));
}

inline double sunHourAngle(double jcent, double elevation, double latitude)
{
    const double declination = sunDeclination(jcent);
    const double angle = std::acos(std::cos(elevation) / (std::cos(latitude) * std::cos(declination)) - std::tan(latitude) * std::tan(declination));
    return std::copysign(angle, -elevation);
}

inline double equationOfTime(double jcent)
{
    const double eccentricity = earthOrbitEccentricity(jcent);
    const double meanLongitude = sunGeometricMeanLongitude(jcent);
    const double meanAnomaly = sunGeometricMeanAnomaly(jcent);
    const double varY = std::pow(std::tan(obliquityCorrection(jcent) / 2), 2);

    return varY * std::sin(2 * meanLongitude) - 2 * eccentricity * std::sin(meanAnomaly)
        + 4 * eccentricity * varY * std::sin(meanAnomaly) * std::cos(2 * meanLongitude) - 0.5 * varY * varY * std::sin(4 * meanLongitude)
        - 1.25 * eccentricity * eccentricity * std::sin(2 * meanAnomaly);
}

inline double timeOfNoon(double jd, double longitude)
{
    const double offset = longitude / (2 * M_PI);
    double noon = std::round(jd + offset) - offset;
    noon -= equationOfTime(julianDateToJulianCenturies(noon)) / (2 * M_PI);
    return noon;
}

} // namespace SunEventsDetail

class SunEvents
{
public:
    SunEvents(const QDateTime &dateTime, double latitude, double longitude)
        : m_latitude(qDegreesToRadians(std::clamp(latitude, -90.0, 90.0)))
        , m_longitude(qDegreesToRadians(std::clamp(longitude, -180.0, 180.0)))
        , m_solarNoon(SunEventsDetail::timeOfNoon(SunEventsDetail::dateTimeToJulianDate(dateTime), m_longitude))
    {
    }

    QDateTime astronomicalDawn() const
    {
        return dateTimeByElevationAngle(90 - SunEventsDetail::astronomicalTwilightElevation);
    }

    QDateTime nauticalDawn() const
    {
        return dateTimeByElevationAngle(90 - SunEventsDetail::nauticalTwilightElevation);
    }

    QDateTime civilDawn() const
    {
        return dateTimeByElevationAngle(90 - SunEventsDetail::civilTwilightElevation);
    }

    QDateTime sunrise() const
    {
        return dateTimeByElevationAngle(90 - SunEventsDetail::sunriseOrSunsetElevation);
    }

    QDateTime solarNoon() const
    {
        return SunEventsDetail::julianDateToDateTime(m_solarNoon);
    }

    QDateTime sunset() const
    {
        return dateTimeByElevationAngle(-90 + SunEventsDetail::sunriseOrSunsetElevation);
    }

    QDateTime civilDusk() const
    {
        return dateTimeByElevationAngle(-90 + SunEventsDetail::civilTwilightElevation);
    }

    QDateTime nauticalDusk() const
    {
        return dateTimeByElevationAngle(-90 + SunEventsDetail::nauticalTwilightElevation);
    }

    QDateTime astronomicalDusk() const
    {
        return dateTimeByElevationAngle(-90 + SunEventsDetail::astronomicalTwilightElevation);
    }

private:
    QDateTime dateTimeByElevationAngle(double elevationAngle) const
    {
        using namespace SunEventsDetail;
        const double hourAngle = sunHourAngle(julianDateToJulianCenturies(m_solarNoon), qDegreesToRadians(elevationAngle), m_latitude) / (2 * M_PI);
        if (std::isnan(hourAngle)) {
            return QDateTime();
        }

        // With extreme hour angles, the timings are less predictable.
        const double absoluteHourAngle = std::abs(hourAngle);
        if (absoluteHourAngle < 0.05 || absoluteHourAngle > 0.45) {
            return QDateTime();
        }

        return julianDateToDateTime(m_solarNoon + hourAngle);
    }

    double m_latitude;
    double m_longitude;
    double m_solarNoon;
};

} // namespace KNightTimeCompat
