include_directories(${CMAKE_BINARY_DIR})

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    KCMUtils
)

add_executable(kdisplay main.cpp)
target_link_libraries(kdisplay KF5::I18n KF5::KCMUtils)

install(TARGETS kdisplay ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kwinft.kdisplay.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
