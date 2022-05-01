include_directories(.)

add_subdirectory(core)
add_subdirectory(prosur)

if (ENABLE_TESTS)
    add_subdirectory(test)
endif()
