cmake_minimum_required(VERSION 3.16)

qt_add_library(gitraven_core
        mainwindow.cpp
        raveneditor.cpp
        raventree.cpp
        raventreemodel.cpp
        gitmanager.cpp
        ravenlhsview.cpp
        ravenrhsview.cpp
        ravenfile.cpp
        raventreeitem.cpp
        raventreedelegate.cpp
        ravenmonacopage.cpp
        ravenmonaco.cpp
        ravenmonacohttpserver.cpp
        ravenmonacobridge.cpp
        ravenstatusbar.cpp
        ravengitcheckoutdialog.cpp
        ravenstatusmessagedispatcher.cpp
        ravengitstatusthread.cpp

        # headers
        ${CMAKE_SOURCE_DIR}/include/mainwindow.h
        ${CMAKE_SOURCE_DIR}/include/raveneditor.h
        ${CMAKE_SOURCE_DIR}/include/raventree.h
        ${CMAKE_SOURCE_DIR}/include/raventreemodel.h
        ${CMAKE_SOURCE_DIR}/include/gitmanager.h
        ${CMAKE_SOURCE_DIR}/include/ravenlhsview.h
        ${CMAKE_SOURCE_DIR}/include/ravenrhsview.h
        ${CMAKE_SOURCE_DIR}/include/ravenfile.h
        ${CMAKE_SOURCE_DIR}/include/raventreeitem.h
        ${CMAKE_SOURCE_DIR}/include/raventreedelegate.h
        ${CMAKE_SOURCE_DIR}/include/ravenmonacopage.h
        ${CMAKE_SOURCE_DIR}/include/ravenmonaco.h
        ${CMAKE_SOURCE_DIR}/include/ravenmonacohttpserver.h
        ${CMAKE_SOURCE_DIR}/include/ravenmonacobridge.h
        ${CMAKE_SOURCE_DIR}/include/ravenutils.h
        ${CMAKE_SOURCE_DIR}/include/ravenstatusbar.h
        ${CMAKE_SOURCE_DIR}/include/ravengitcheckoutdialog.h
        ${CMAKE_SOURCE_DIR}/include/ravenstatusmessagedispatcher.h
        ${CMAKE_SOURCE_DIR}/include/ravengitstatusthread.h
)

# Include own headers
target_include_directories(gitraven_core
        PUBLIC ${CMAKE_SOURCE_DIR}/include
)


# ASAN stuff
# Note: This is disabled by default because it crashes QWebEngine EVERY time.
#       If you need to debug QWebEngine stuff, do not enable this code
#if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#    # 1. Compile flags (The "Sensors")
#    target_compile_options(gitraven_core PRIVATE -fsanitize=address -fno-omit-frame-pointer)
#
#    # 2. Linker flags (The "Engine")
#    # Using target_link_libraries with the flag is the most reliable way
#    # to force the compiler driver to link the ASan runtime.
#    target_link_libraries(gitraven_core PRIVATE -fsanitize=address)
#endif()

# Uncomment for profiling app. (callgrind)
# set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")


target_link_libraries(gitraven_core PUBLIC Qt6::Widgets Qt6::Core Qt6::Gui Qt6::HttpServer Qt6::WebEngineCore Qt6::WebEngineWidgets)
target_link_libraries(gitraven_core PUBLIC libgit2::libgit2package)
target_link_libraries(gitraven_core PUBLIC magic)
