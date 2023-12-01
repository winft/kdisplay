add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kwinft.kdisplay\")

set(kdisplayApplet_SRCS
    kdisplay_applet.cpp
    ../osd/osdaction.cpp
)

add_library(org.kwinft.kdisplay MODULE ${kdisplayApplet_SRCS})

target_link_libraries(org.kwinft.kdisplay
  Qt6::Qml
  Qt6::DBus
  KF6::I18n
  Plasma::Plasma
  disman::lib
)

install(TARGETS org.kwinft.kdisplay DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)

plasma_install_package(package org.kwinft.kdisplay)
