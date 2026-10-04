find_package(Catch2 3 REQUIRED)
add_executable(test_functions string_functions.cpp
    ../string_functions.cpp
    ../string_functions.h
)

target_link_libraries(test_functions PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(test_functions)