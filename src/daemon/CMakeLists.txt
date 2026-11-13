# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_executable(gxde-nighttime-x11d)

kconfig_target_kcfg_file(gxde-nighttime-x11d
    FILE kdarklightsettings.kcfg
    CLASS_NAME KDarkLightSettings
    GENERATE_MOC
    GENERATE_PROPERTIES
    MUTATORS
)

kconfig_target_kcfg_file(gxde-nighttime-x11d
    FILE kdarklightstate.kcfg
    CLASS_NAME KDarkLightState
    GENERATE_MOC
    GENERATE_PROPERTIES
    MUTATORS
)

target_compile_definitions(gxde-nighttime-x11d PRIVATE
    -DTRANSLATION_DOMAIN=\"gxde-nighttime-x11d\"
)

target_sources(gxde-nighttime-x11d PRIVATE
    kdarklightmanager.cpp
    kdarklightmanagerinterface.cpp
    kdarklightscheduler.cpp
    ksolardarklightscheduler.cpp
    ktimeddarklightscheduler.cpp
    main.cpp
)

target_link_libraries(gxde-nighttime-x11d PRIVATE
    gxde-nighttime-x11

    Qt6::DBus
    Qt6::Positioning

    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::DBusAddons
)

install(FILES com.gxde.NightTimeX11.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(TARGETS gxde-nighttime-x11d DESTINATION ${KDE_INSTALL_LIBEXECDIR})

ecm_generate_dbus_service_file(
    NAME com.gxde.NightTimeX11
    EXECUTABLE "${KDE_INSTALL_FULL_LIBEXECDIR}/gxde-nighttime-x11d"
    SYSTEMD_SERVICE gxde-nighttime-x11d.service
    DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR}
)

ecm_install_configured_files(
   INPUT gxde-nighttime-x11d.service.in
   DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR}
)

ecm_install_configured_files(
    INPUT com.gxde.nighttime-x11d.desktop.in
    DESTINATION ${KDE_INSTALL_APPDIR}
)
