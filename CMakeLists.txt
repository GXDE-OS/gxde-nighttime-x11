# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "6.8.4")
project(gxde-nighttime-x11 VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_DEP_VERSION "6.8.0")
set(QT_MIN_VERSION "6.8.0")
set(KF6_MIN_VERSION "6.13.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(CMakePackageConfigHelpers)
include(CheckCXXCompilerFlag)
include(FeatureSummary)
include(GenerateExportHeader)

include(ECMConfiguredInstall)
include(ECMGenerateDBusServiceFile)
include(ECMGenerateHeaders)
include(ECMGenerateQDoc)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    DBus
    Positioning
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    DBusAddons
    Holidays
    I18n
)

ecm_set_disabled_deprecation_versions(QT 6.8.0
    KF 6.13.0
)

check_cxx_compiler_flag(-Wno-unused-parameter COMPILER_UNUSED_PARAMETER_SUPPORTED)
if (COMPILER_UNUSED_PARAMETER_SUPPORTED)
    add_compile_options(-Wno-unused-parameter)
endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(examples)
endif()

add_subdirectory(src)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/gxde-nighttime-x11")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/gxde-nighttime-x11ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/gxde-nighttime-x11Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gxde-nighttime-x11Config.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/gxde-nighttime-x11Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/gxde-nighttime-x11ConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}" COMPONENT Devel
)

install(EXPORT gxde-nighttime-x11Targets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE gxde-nighttime-x11Targets.cmake NAMESPACE gxde-nighttime-x11::)

ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
