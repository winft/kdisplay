include_directories(
    ${CMAKE_BINARY_DIR}/plasma-integration/kded
)

add_executable(osdtest main.cpp
                       osdtest.cpp
                       ../../plasma-integration/kded/osd.cpp
                       ../../plasma-integration/kded/osdmanager.cpp
                       ../../plasma-integration/kded/osdaction.cpp
                       ../../common/utils.cpp
)

target_link_libraries(osdtest Qt5::Core
                              Qt5::DBus
                              Qt5::Quick
                              Qt5::Qml
                              Disman::Disman
                              KF5::I18n
                              KF5::Declarative
)

add_test(NAME kdisplay-kded-osdtest COMMAND osdtest)
ecm_mark_as_test(osdtest)
