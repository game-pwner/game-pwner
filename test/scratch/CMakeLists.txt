find_package(Boost REQUIRED COMPONENTS iostreams filesystem)

# ┌──────────────────────────────────────────────────────────────────┐
#   libscratch
# └──────────────────────────────────────────────────────────────────┘
set(TARGET libscratch)
add_library(${TARGET} SHARED libscratch.cc)
target_include_directories(${TARGET}
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
        )
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 0)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O0
        -g3
        -ggdb3
        -Wall -Wextra -Wpedantic
        )
target_link_libraries(${TARGET} PUBLIC
        stdc++fs
        Boost::iostreams
        Boost::filesystem
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_SCRATCH
# └──────────────────────────────────────────────────────────────────┘
set(TARGET scratch)
add_executable(${TARGET} scratch.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 0)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        libscratch
        pwner
        criu
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_PredicateScanner
# └──────────────────────────────────────────────────────────────────┘
set(TARGET TOOL_PredicateScanner)
add_executable(${TARGET} MemoryIterator.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 0)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_ValueScannerSequential
# └──────────────────────────────────────────────────────────────────┘
set(TARGET TOOL_ValueScannerSequential)
add_executable(${TARGET} ValueScanner.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 0)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_PointerScannerForward
# └──────────────────────────────────────────────────────────────────┘
set(TARGET TOOL_PointerScannerForward)
add_executable(${TARGET} PointerScannerForward.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 0)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_PointerScannerBackward
# └──────────────────────────────────────────────────────────────────┘
set(TARGET TOOL_PointerScannerBackward)
add_executable(${TARGET} PointerScannerBackward.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 0)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        )
