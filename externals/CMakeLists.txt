# Definitions for all external bundled libraries

# Suppress warnings from external libraries
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W0)
endif()

# fmt
add_subdirectory(fmt)

# Glad
add_subdirectory(glad)

# glfw
add_subdirectory(glfw)

# getopt
if (MSVC)
    add_subdirectory(getopt)
endif()
