include_directories(${CMAKE_BINARY_DIR})

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    KCMUtils
)

add_executable(kdisplay main.cpp)
target_link_libraries(kdisplay
  KF6::I18n
  KF6::KCMUtils
)

install(TARGETS kdisplay ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kwinft.kdisplay.desktop DESTINATION ${KDE_INSTALL_APPDIR})
