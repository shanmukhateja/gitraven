cmake_minimum_required(VERSION 3.16)

qt_add_executable(gitraven_tests
        test_gitmanager.cpp
)

target_include_directories(gitraven_tests PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(gitraven_tests PRIVATE
        gitraven_core
        Qt6::Test
)

# Register test cases here
add_test(NAME GitRavenTests COMMAND gitraven_tests)