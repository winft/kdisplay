add_definitions(-DTRANSLATION_DOMAIN=\"kcm_kdisplay\")

add_subdirectory(app)

set(kcm_kdisplay_SRCS
    config_handler.cpp
    kcm.cpp
    output_identifier.cpp
    output_model.cpp
    ${CMAKE_SOURCE_DIR}/common/utils.cpp
    ${CMAKE_SOURCE_DIR}/common/globals.cpp
    ${CMAKE_SOURCE_DIR}/common/orientation_sensor.cpp
)

ecm_qt_declare_logging_category(kcm_kdisplay_SRCS
    HEADER
        kcm_kdisplay_debug.h
    IDENTIFIER
        KDISPLAY_KCM
    CATEGORY_NAME
        kdisplay.kcm
)

add_library(kcm_kdisplay MODULE ${kcm_kdisplay_SRCS})

target_link_libraries(kcm_kdisplay
    Qt5::Sensors
    KF5::Declarative
    KF5::I18n
    KF5::PlasmaQuick
    KF5::QuickAddons
    Disman::Disman
)

kcoreaddons_desktop_to_json(kcm_kdisplay "kcm_kdisplay.desktop" SERVICE_TYPES kcmodule.desktop)

#this desktop file is installed only for retrocompatibility with sycoca
install(FILES kcm_kdisplay.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

install(TARGETS kcm_kdisplay DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)

kpackage_install_package(package kcm_kdisplay kcms)
