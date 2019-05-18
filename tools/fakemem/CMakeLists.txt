

set(TARGET TOOL_FAKEMEM_LIB)
add_library(${TARGET} SHARED lib/fakemem.cc)
target_include_directories(${TARGET}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
        PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
        )
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 2)
target_use_warnings(${TARGET} 1)
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        )



set(TARGET TOOL_FAKEMEM_EXE)
add_executable(${TARGET} main.cc)
target_include_directories(${TARGET}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> #TOOL_FAKEMEM_LIB
                $<INSTALL_INTERFACE:include>
        )
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_debug(${TARGET})
target_use_hardening(${TARGET} 2)
target_use_warnings(${TARGET} 1)
target_compile_options(${TARGET} PUBLIC
        -O3
        )
target_link_libraries(${TARGET} PUBLIC
        pwner
        TOOL_FAKEMEM_LIB
        )