add_definitions(-DTRANSLATION_DOMAIN="kdisplay")

include_directories(${CMAKE_BINARY_DIR})

add_executable(kdisplay main.cpp)
target_link_libraries(kdisplay KF5::I18n KF5::KCMUtils)

install(TARGETS kdisplay ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kwinft.kdisplay.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
