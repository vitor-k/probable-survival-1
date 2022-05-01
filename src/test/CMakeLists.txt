add_executable(tests
    bit_tests.cpp
)

find_package(Catch2 3)

target_link_libraries(tests PRIVATE core)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
