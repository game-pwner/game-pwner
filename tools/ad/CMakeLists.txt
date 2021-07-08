
set(TARGET TOOL_AC)
add_executable(${TARGET} main.cc)
# todo[high] better to replace set_property with set_target_properties everywhere
#set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 20
        )
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 0)
target_use_warnings(${TARGET} 1)
target_compile_options(${TARGET} PUBLIC
        -O3
#        -Wl,--no-as-needed
        )
target_link_libraries(${TARGET} PUBLIC
        -ldl
        pwner
)
