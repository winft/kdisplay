cmake_minimum_required(VERSION 3.0)

project(KDisplay)
set(PROJECT_VERSION "5.19.80")
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
include(KDEClangFormat)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test Sensors)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    DBusAddons
    Declarative
    GlobalAccel
    I18n
    IconThemes
    KCMUtils
    Plasma
    XmlGui
)
# WARNING PlasmaQuick provides unversioned CMake config
find_package(KF5 REQUIRED COMPONENTS PlasmaQuick)

set(MIN_DISMAN_VERSION "0.518.0")
find_package(Disman ${MIN_DISMAN_VERSION} REQUIRED)

add_subdirectory(kcm)
add_subdirectory(kded)
add_subdirectory(plasmoid)
add_subdirectory(tests)
add_subdirectory(console)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

install(FILES kdisplay.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
