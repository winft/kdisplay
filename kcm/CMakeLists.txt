add_definitions(-DTRANSLATION_DOMAIN=\"kcm_kdisplay\")

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Kirigami2
)

add_subdirectory(app)
ki18n_install(po)

set(kcm_kdisplay_SRCS
    config_handler.cpp
    kcm.cpp
    output_identifier.cpp
    output_model.cpp
    ${CMAKE_SOURCE_DIR}/common/utils.cpp
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
    KF5::Kirigami2
    KF5::QuickAddons
    disman::lib
)

kcoreaddons_desktop_to_json(kcm_kdisplay "kcm_kdisplay.desktop")

install(TARGETS kcm_kdisplay DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kcm_kdisplay.desktop DESTINATION ${SERVICES_INSTALL_DIR})

kpackage_install_package(package kcm_kdisplay kcms)
