add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kwinft.kdisplay\")

kcoreaddons_add_plugin(kdisplayd INSTALL_NAMESPACE "kf6/kded")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../)

target_sources(kdisplayd
  PRIVATE
    daemon.cpp
    config.cpp
    generator.cpp
    ../osd/osdaction.cpp
    ${CMAKE_SOURCE_DIR}/common/orientation_sensor.cpp
    ${CMAKE_SOURCE_DIR}/common/utils.cpp
)

ecm_qt_declare_logging_category(kdisplayd
    HEADER kdisplay_daemon_debug.h
    IDENTIFIER KDISPLAY_KDED
    CATEGORY_NAME kdisplay.kded
)

qt_add_dbus_interface(dbus_SRCS
    org.freedesktop.DBus.Properties.xml
    freedesktop_interface)
qt_add_dbus_adaptor(dbus_SRCS
    org.kwinft.kdisplay.xml
    daemon.h
    KDisplayDaemon
)
qt_add_dbus_interface(dbus_SRCS
  ../osd/org.kwinft.kdisplay.osdService.xml
  osdservice_interface
)

target_sources(kdisplayd PRIVATE ${dbus_SRCS})

target_link_libraries(kdisplayd
  disman::lib
  KF6::CoreAddons
  KF6::DBusAddons
  KF6::I18n
  KF6::XmlGui
  KF6::GlobalAccel
  Qt6::Sensors
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kdisplayd.json.in
               ${CMAKE_CURRENT_BINARY_DIR}/kdisplayd.json
               @ONLY)
