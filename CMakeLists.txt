cmake_minimum_required(VERSION 3.16)

project(KDisplay VERSION 5.27.80)

add_definitions("-DKDISPLAY_VERSION=\"${PROJECT_VERSION}\"")

set(QT_MIN_VERSION "6.4.0")
set(KF6_MIN_VERSION "5.240.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.84")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMConfiguredInstall)
include(ECMGenerateDBusServiceFile)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test Sensors)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
  Config
  DBusAddons
  GlobalAccel
  I18n
  IconThemes
  KCMUtils
  WindowSystem
  XmlGui
)

find_package(Plasma ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} REQUIRED)
find_package(PlasmaQuick ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} REQUIRED)

find_package(LayerShellQt)
set_package_properties(LayerShellQt PROPERTIES
    DESCRIPTION "Layer shell Qt bindings"
    URL "https://invent.kde.org/plasma/layer-shell-qt"
    PURPOSE "Required for the screen selection osd"
    TYPE REQUIRED
)

set(MIN_DISMAN_VERSION "0.527.80")
find_package(disman ${MIN_DISMAN_VERSION} REQUIRED)

add_subdirectory(kcm)
add_subdirectory(plasma-integration)
add_subdirectory(tests)

install(FILES kdisplay.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
