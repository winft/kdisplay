add_executable(kdisplay-console main.cpp console.cpp)

target_link_libraries(kdisplay-console
            Qt5::DBus
            Qt5::Gui
            KF5::CoreAddons
            KF5::I18n
            Disman)

install(TARGETS kdisplay-console ${INSTALL_TARGETS_DEFAULT_ARGS})
