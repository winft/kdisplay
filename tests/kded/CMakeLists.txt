include_directories(${CMAKE_BINARY_DIR})

add_definitions(-DKDED_UNIT_TEST)

macro(ADD_KDED_TEST testname)
    set(test_SRCS
        ${testname}.cpp
        ${CMAKE_SOURCE_DIR}/plasma-integration/kded/generator.cpp
        ${CMAKE_SOURCE_DIR}/plasma-integration/kded/config.cpp
        #${CMAKE_SOURCE_DIR}/kded/daemon.cpp
    )
    ecm_qt_declare_logging_category(test_SRCS HEADER kdisplay_daemon_debug.h IDENTIFIER KDISPLAY_KDED CATEGORY_NAME kdisplay.kded)

    qt6_add_dbus_interface(test_SRCS
        ${CMAKE_SOURCE_DIR}/plasma-integration/kded/org.freedesktop.DBus.Properties.xml
        freedesktop_interface
    )

    add_executable(${testname} ${test_SRCS})
    add_dependencies(${testname} kdisplayd) # make sure the dbus interfaces are generated
    target_compile_definitions(${testname} PRIVATE "-DTEST_DATA=\"${CMAKE_CURRENT_SOURCE_DIR}/\"")
    target_link_libraries(${testname} Qt6::Test Qt6::DBus Qt6::Gui Qt6::Sensors disman::lib)
    add_test(NAME kdisplay-kded-${testname} COMMAND ${testname})
    ecm_mark_as_test(${testname})
endmacro()

add_kded_test(testgenerator)
#add_kded_test(testdaemon)
