add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kwinft.kdisplay\")

# WARNING PlasmaQuick provides unversioned CMake config
find_package(KF5 REQUIRED COMPONENTS PlasmaQuick)

set(kdisplayApplet_SRCS
    kdisplay_applet.cpp
    ../osd/osdaction.cpp
)

add_library(plasma_applet_kdisplay MODULE ${kdisplayApplet_SRCS})

kcoreaddons_desktop_to_json(plasma_applet_kdisplay package/metadata.desktop)

target_link_libraries(plasma_applet_kdisplay
                      Qt5::Qml
                      Qt5::DBus
                      KF5::I18n
                      KF5::Plasma
                      disman::lib
)

install(TARGETS plasma_applet_kdisplay DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)

plasma_install_package(package org.kwinft.kdisplay)
