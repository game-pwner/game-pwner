
# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_SCRATCH
# └──────────────────────────────────────────────────────────────────┘
set(TARGET TOOL_SCRATCH)
add_executable(${TARGET} scratch.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 2)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        criu
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_SCRATCH2
# └──────────────────────────────────────────────────────────────────┘
set(TARGET TOOL_SCRATCH2)
add_executable(${TARGET} scratch2.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 2)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        -Wall -Wextra -Wpedantic
        )
target_link_libraries(${TARGET} PUBLIC
#        pwner
        stdc++fs
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   TOOL_ValueScannerByteSwath
# └──────────────────────────────────────────────────────────────────┘
set(TARGET TOOL_ValueScannerByteSwath)
add_executable(${TARGET} ValueScannerByteSwath.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 2)
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
add_executable(${TARGET} ValueScannerSequential.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 2)
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
target_use_hardening(${TARGET} 2)
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
target_use_hardening(${TARGET} 2)
target_use_warnings(${TARGET} 1)
target_use_openmp(${TARGET})
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        )
