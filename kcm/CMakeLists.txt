add_definitions(-DTRANSLATION_DOMAIN=\"kcm_kdisplay\")

kcoreaddons_add_plugin(kcm_kdisplay INSTALL_NAMESPACE "plasma/kcms/systemsettings")

add_subdirectory(app)
ki18n_install(po)

target_sources(kcm_kdisplay
  PRIVATE
    config_handler.cpp
    kcm.cpp
    output_identifier.cpp
    output_model.cpp
    ${CMAKE_SOURCE_DIR}/common/utils.cpp
    ${CMAKE_SOURCE_DIR}/common/orientation_sensor.cpp
)

ecm_qt_declare_logging_category(kcm_kdisplay
    HEADER
        kcm_kdisplay_debug.h
    IDENTIFIER
        KDISPLAY_KCM
    CATEGORY_NAME
        kdisplay.kcm
    DESCRIPTION "kdisplay kcm (kdisplay)" EXPORT KDISPLAY
)

target_link_libraries(kcm_kdisplay
  disman::lib
  KF6::I18n
  KF6::KCMUtils
  KF6::KCMUtilsQuick
  KF6::PlasmaQuick
  Qt6::Sensors
)

kcmutils_generate_desktop_file(kcm_kdisplay)
kpackage_install_package(package kcm_kdisplay kcms)
