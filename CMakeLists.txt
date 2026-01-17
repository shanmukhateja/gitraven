cmake_minimum_required(VERSION 3.16)

project(gitraven-qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Set a env variable based on build type
#  for C++ code simplicity.

# Define preprocessor macros for build types
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DBUILD_TYPE_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DBUILD_TYPE_RELEASE)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_definitions(-DBUILD_TYPE_RELWITHDEBINFO)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_definitions(-DBUILD_TYPE_MINSIZEREL)
endif()

# Qt6 only
set(QT_VERSION_MAJOR 6)

# Use local prefix for install()
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS HttpServer)

# WebEngine
find_package(Qt6 REQUIRED COMPONENTS WebEngineCore)
find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)

find_package(libgit2 REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        raveneditor.h
        raveneditor.cpp
        raventree.h
        raventree.cpp
        raventreemodel.h
        raventreemodel.cpp
        gitmanager.h
        gitmanager.cpp
        ravenlhsview.h
        ravenlhsview.cpp
        ravenrhsview.h
        ravenrhsview.cpp
        ravenfile.h
        ravenfile.cpp
        raventreeitem.h
        raventreeitem.cpp
        raventreedelegate.h
        raventreedelegate.cpp
        ravenmonacopage.h
        ravenmonacopage.cpp
        ravenmonaco.h
        ravenmonaco.cpp
        ravenmonacohttpserver.h
        ravenmonacohttpserver.cpp
        ravenmonacobridge.h
        ravenmonacobridge.cpp
        ravenutils.h
        ravenstatusbar.h
        ravenstatusbar.cpp
        ravengitcheckoutdialog.h
        ravengitcheckoutdialog.cpp
        ravenstatusmessagedispatcher.h
        ravenstatusmessagedispatcher.cpp
)

qt_add_executable(gitraven-qt
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(gitraven-qt PRIVATE Qt6::Widgets)
target_link_libraries(gitraven-qt PRIVATE Qt6::Core)
target_link_libraries(gitraven-qt PRIVATE Qt6::Gui)
target_link_libraries(gitraven-qt PRIVATE Qt6::HttpServer)
target_link_libraries(gitraven-qt PRIVATE Qt6::WebEngineCore)
target_link_libraries(gitraven-qt PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(gitraven-qt PRIVATE libgit2::libgit2package)
target_link_libraries(gitraven-qt PRIVATE magic)

# Copy monaco editor to build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/editor ${CMAKE_BINARY_DIR}/editor
    COMMENT "Copying Monaco editor assets to build directory"
)

install(TARGETS gitraven-qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Copy editor directory
install(
    DIRECTORY   editor
    DESTINATION  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)

# Copy .desktop file
install(FILES com.github.shanmukhateja.gitraven.desktop DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications)

qt_finalize_executable(gitraven-qt)
