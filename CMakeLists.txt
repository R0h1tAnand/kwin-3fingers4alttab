cmake_minimum_required(VERSION 3.16)
project(kwin-3fingers4alttab VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(QT_MAJOR_VERSION 6)
set(QT_DEFAULT_MAJOR_VERSION 6)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs6)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 REQUIRED COMPONENTS Core DBus Widgets)
find_package(KF6 REQUIRED COMPONENTS Config KCMUtils)
find_package(KWin REQUIRED)

add_library(kwin_effect_3fingers4alttab MODULE
    src/main.cpp
    src/effect.cpp
)

target_link_libraries(kwin_effect_3fingers4alttab
    PRIVATE
    KWin::kwin
    KF6::ConfigCore
)

install(TARGETS kwin_effect_3fingers4alttab
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins
)

add_library(kwin_effect_3fingers4alttab_config MODULE
    src/kcm.cpp
)

target_link_libraries(kwin_effect_3fingers4alttab_config
    PRIVATE
    KF6::KCMUtils
    KF6::ConfigCore
    Qt6::DBus
    Qt6::Widgets
)

install(TARGETS kwin_effect_3fingers4alttab_config
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/configs
)
