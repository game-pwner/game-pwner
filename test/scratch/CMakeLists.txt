# ┌──────────────────────────────────────────────────────────────────┐
#   SCRATCH_scanner_value
# └──────────────────────────────────────────────────────────────────┘
set(TARGET SCRATCH_scanner_value)
add_executable(${TARGET} scanner_value.cc)
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
#   SCRATCH_scanner_pointer_forward
# └──────────────────────────────────────────────────────────────────┘
set(TARGET SCRATCH_scanner_pointer_forward)
add_executable(${TARGET} scanner_pointer_forward.cc)
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
#   SCRATCH_scanner_pointer_backward
# └──────────────────────────────────────────────────────────────────┘
set(TARGET SCRATCH_scanner_pointer_backward)
add_executable(${TARGET} scanner_pointer_backward.cc)
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
