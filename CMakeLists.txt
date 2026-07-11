cmake_minimum_required(VERSION 3.16)

project(project_florence_desktop VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appproject_florence_desktop
    main.cpp
)

qt_add_qml_module(appproject_florence_desktop
    URI project_florence_desktop
    QML_FILES
        Main.qml
        SettingsScreen.qml
    SOURCES
        src/api/ApiClient.h
        src/api/ApiClient.cpp
        src/i18n/TranslationManager.h
        src/i18n/TranslationManager.cpp
)

qt_add_resources(appproject_florence_desktop "i18n"
    PREFIX "/"
    FILES
        i18n/en.json
        i18n/tr.json
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appproject_florence_desktop PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appproject_florence_desktop
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appproject_florence_desktop PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i18n
)

target_link_libraries(appproject_florence_desktop
    PRIVATE Qt6::Quick Qt6::Network
)

install(TARGETS appproject_florence_desktop
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
