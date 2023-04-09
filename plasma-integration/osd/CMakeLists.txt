add_executable(kdisplay_osd_service
  main.cpp
  osdaction.cpp
  osdmanager.cpp
  osd.cpp
  qml.qrc
)

qt_add_dbus_adaptor(dbus_SRCS
  org.kwinft.kdisplay.osdService.xml
  osdmanager.h
  KDisplay::OsdManager
)

target_sources(kdisplay_osd_service PRIVATE ${dbus_SRCS})

target_link_libraries(kdisplay_osd_service PRIVATE
  disman::lib
  KF5::I18n
  KF5::WindowSystem
  LayerShellQt::Interface
  Qt::DBus
  Qt::Quick
)

install(TARGETS kdisplay_osd_service DESTINATION ${KDE_INSTALL_LIBEXECDIR})

ecm_generate_dbus_service_file(
  NAME org.kwinft.kdisplay.osdService
  EXECUTABLE ${KDE_INSTALL_FULL_LIBEXECDIR}/kdisplay_osd_service
  DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR}
  SYSTEMD_SERVICE plasma-kdisplay-osd.service
)

ecm_install_configured_files(INPUT plasma-kdisplay-osd.service @ONLY
  DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR}
)
