# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_library(gxde-nighttime-x11)
add_library(gxde-nighttime-x11::NightTime ALIAS gxde-nighttime-x11)

generate_export_header(gxde-nighttime-x11 BASE_NAME KNightTime EXPORT_FILE_NAME knighttime_export.h)

set_target_properties(gxde-nighttime-x11 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME NightTime
)

target_compile_definitions(gxde-nighttime-x11 PRIVATE
    -DTRANSLATION_DOMAIN=\"gxde-nighttime-x11\"
)

target_include_directories(gxde-nighttime-x11
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/gxde-nighttime-x11/>"
)

target_sources(gxde-nighttime-x11 PRIVATE
    kdarklightschedule.cpp
    kdarklightscheduleprovider.cpp
    kdarklightschedulesubscription.cpp
)

# KSystemClockSkewNotifier was added in KCoreAddons 6.15. On older KF6, ship a
# fallback in this library so that the daemon and gxde-kwin_x11 can use it.
if (KF6CoreAddons_VERSION VERSION_LESS 6.15.0)
    set(GXDE_NIGHTTIME_X11_SKEW_NOTIFIER ON)
else()
    set(GXDE_NIGHTTIME_X11_SKEW_NOTIFIER OFF)
endif()
set(GXDE_NIGHTTIME_X11_SKEW_NOTIFIER ${GXDE_NIGHTTIME_X11_SKEW_NOTIFIER} PARENT_SCOPE)
add_feature_info("SkewNotifierFallback" GXDE_NIGHTTIME_X11_SKEW_NOTIFIER
    "Bundled KSystemClockSkewNotifier for KCoreAddons < 6.15")

set(KNightTime_HEADER_NAMES
    KDarkLightSchedule
    KDarkLightScheduleProvider
)
set(KNightTime_LOWERCASE_HEADERS
    kdarklightschedule.h
    kdarklightscheduleprovider.h
)
if (GXDE_NIGHTTIME_X11_SKEW_NOTIFIER)
    target_sources(gxde-nighttime-x11 PRIVATE ksystemclockskewnotifier.cpp)
    list(APPEND KNightTime_HEADER_NAMES KSystemClockSkewNotifier)
    list(APPEND KNightTime_LOWERCASE_HEADERS ksystemclockskewnotifier.h)
endif()

target_link_libraries(gxde-nighttime-x11
    PUBLIC
        Qt6::Core
    PRIVATE
        Qt6::DBus
        KF6::Holidays
)

install(TARGETS gxde-nighttime-x11 EXPORT gxde-nighttime-x11Targets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_qt_declare_logging_category(gxde-nighttime-x11
    HEADER knighttimelogging.h
    IDENTIFIER KNIGHTTIME
    CATEGORY_NAME gxde-nighttime-x11
    DESCRIPTION "Helpers for scheduling the dark-light cycle"
    EXPORT gxde-nighttime-x11
)

ecm_qt_install_logging_categories(
    EXPORT gxde-nighttime-x11
    FILE gxde-nighttime-x11.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

ecm_generate_qdoc(gxde-nighttime-x11 gxde-nighttime-x11.qdocconf)

ecm_generate_headers(KNightTime_HEADERS
    HEADER_NAMES
        ${KNightTime_HEADER_NAMES}
    REQUIRED_HEADERS KCoreAddons_HEADERS
)

install(
    FILES
        ${KNightTime_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/knighttime_export.h
        ${KNightTime_LOWERCASE_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/gxde-nighttime-x11 COMPONENT Devel
)

add_subdirectory(daemon)
