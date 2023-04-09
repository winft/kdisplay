add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kwinft.kdisplay\")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    GlobalAccel
    XmlGui
)

set(kdisplay_daemon_SRCS
    daemon.cpp
    config.cpp
    generator.cpp
    ../osd/osdaction.cpp
    ${CMAKE_SOURCE_DIR}/common/orientation_sensor.cpp
    ${CMAKE_SOURCE_DIR}/common/utils.cpp
)

ecm_qt_declare_logging_category(kdisplay_daemon_SRCS
    HEADER kdisplay_daemon_debug.h
    IDENTIFIER KDISPLAY_KDED
    CATEGORY_NAME kdisplay.kded
)

qt5_add_dbus_interface(kdisplay_daemon_SRCS
    org.freedesktop.DBus.Properties.xml
    freedesktop_interface)
qt5_add_dbus_adaptor(kdisplay_daemon_SRCS
    org.kwinft.kdisplay.xml
    daemon.h
    KDisplayDaemon
)
qt5_add_dbus_interface(kdisplay_daemon_SRCS
  ../osd/org.kwinft.kdisplay.osdService.xml
  osdservice_interface
)

add_library(kdisplayd MODULE ${kdisplay_daemon_SRCS})

target_link_libraries(kdisplayd
                              Qt5::Sensors
                              KF5::Declarative
                              disman::lib
                              KF5::DBusAddons
                              KF5::I18n
                              KF5::XmlGui
                              KF5::GlobalAccel)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kdisplayd.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/kdisplayd.desktop
               @ONLY)

kcoreaddons_desktop_to_json(kdisplayd ${CMAKE_CURRENT_BINARY_DIR}/kdisplayd.desktop)

install(TARGETS kdisplayd DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kded)
