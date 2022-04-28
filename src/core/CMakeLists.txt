add_library(core
    bios.cpp
    bios.h
    cpu.cpp
    cpu.h
    mips.h
)

target_link_libraries(core fmt)
