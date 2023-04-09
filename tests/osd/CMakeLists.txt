include_directories(
    ${CMAKE_BINARY_DIR}/plasma-integration/kded
)

qt_add_dbus_interface(OsdInterface
  ../../plasma-integration/osd/org.kwinft.kdisplay.osdService.xml
  osdservice_interface
)

add_executable(osdtest
  main.cpp
  ../../plasma-integration/osd/osdaction.cpp
  ${OsdInterface}
)

target_link_libraries(osdtest
  KF5::I18n
  Qt5::Core
  Qt5::DBus
)

add_test(NAME kdisplay-kded-osdtest COMMAND osdtest)
ecm_mark_as_test(osdtest)
