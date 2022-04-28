add_executable(ProSur1
    main.cpp
)

target_link_libraries(ProSur1 PRIVATE core fmt)

if (MSVC)
    target_link_libraries(ProSur1 PRIVATE getopt)
endif()
