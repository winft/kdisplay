cmake_minimum_required(VERSION 3.0)

project(KDisplay)
set(PROJECT_VERSION "5.21.80")
set(KDISPLAY_VERSION ${PROJECT_VERSION})

add_definitions("-DKDISPLAY_VERSION=\"${KDISPLAY_VERSION}\"")

set(QT_MIN_VERSION "5.14.0")
set(KF5_MIN_VERSION "5.66.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test Sensors)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    DBusAddons
    Declarative
    I18n
)

set(MIN_DISMAN_VERSION "0.519.80")
find_package(Disman ${MIN_DISMAN_VERSION} REQUIRED)

add_subdirectory(kcm)
add_subdirectory(plasma-integration)
add_subdirectory(tests)

install(FILES kdisplay.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
