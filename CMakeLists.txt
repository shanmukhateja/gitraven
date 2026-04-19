cmake_minimum_required(VERSION 3.16)

project(gitraven VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(GITRAVEN_ENABLE_TESTING "Enable testing" ON)

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

# Used to run clang-format & clang-tidy on pre-commit Git hook 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core HttpServer WebEngineCore WebEngineWidgets Test)
find_package(libgit2 REQUIRED)

add_subdirectory(src)
if (GITRAVEN_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()

# main executable
qt_add_executable(gitraven_qt src/main.cpp)

# Include app's headers
target_include_directories(gitraven_qt PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Link to gitraven_core
target_link_libraries(gitraven_qt PRIVATE gitraven_core)


# Copy monaco editor to build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/editor ${CMAKE_BINARY_DIR}/editor
    COMMENT "Copying Monaco editor assets to build directory"
)

install(TARGETS gitraven_qt
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
