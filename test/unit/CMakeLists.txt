

set(TARGET TEST_openmp_cc)
add_executable(${TARGET} openmp.cc)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
target_use_openmp(${TARGET})
target_use_hardening(${TARGET} 2)
target_compile_options(${TARGET} PUBLIC
        -O3
        )

# if (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL PGI)
#set(TARGET TEST_openacc_cc)
#add_executable(${TARGET} openacc.cc)
#set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
##target_use_openacc(${TARGET})
#target_compile_options(${TARGET} PUBLIC ${OpenACC_CXX_FLAGS})
#target_link_libraries(${TARGET} PUBLIC -fopenacc)
#target_use_hardening(${TARGET} 2)
#target_compile_options(${TARGET} PUBLIC
#        -O3
#        )
#
#if (CMAKE_CXX_COMPILER_ID STREQUAL PGI)
#    set(TARGET TEST_openacc_device_cc)
#    add_executable(${TARGET} openacc.cc)
#    set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
#    target_use_openacc(${TARGET})
#    target_use_hardening(${TARGET} 2)
#    target_compile_options(${TARGET} PUBLIC
#            -O3
#            )
#endif()
#
#set(TARGET Process__get_region_address)
#add_executable(${TARGET} get_region_address.cc)
#set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
#target_use_openmp(${TARGET})
##target_use_hardening(${TARGET} 2)
#target_compile_options(${TARGET} PUBLIC
#        -O3
#        )
#target_link_libraries(${TARGET} PUBLIC
#        stdc++fs
#        pwner
#        )
