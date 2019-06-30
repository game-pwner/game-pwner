find_package(Boost 1.69 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

set(TARGET openmp_cc)
add_executable(${TARGET} openmp.cc)
set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 20
        PWN_OPTIMIZATION 3
        PWN_DEBUG        True
        PWN_WARNINGS     3
        PWN_OPENMP       True
        )
pwn_target_properties(${TARGET})
target_compile_definitions(${TARGET} PUBLIC
        -DBOOST_TEST_DYN_LINK
        -DBOOST_TEST_MODULE=${TARGET}
        )
target_link_libraries(${TARGET} PUBLIC
        Boost::unit_test_framework
        )


set(TARGET process_cc)
add_executable(${TARGET} process.cc)
set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 20
        PWN_OPTIMIZATION 3
        PWN_DEBUG        True
        PWN_WARNINGS     3
        PWN_OPENMP       True
        )
pwn_target_properties(${TARGET})
target_compile_definitions(${TARGET} PUBLIC
        -DBOOST_TEST_DYN_LINK
        -DBOOST_TEST_MODULE=${TARGET}
        )
target_link_libraries(${TARGET} PUBLIC
        Boost::unit_test_framework
        pwner
        )


set(TARGET scanner_cc)
add_executable(${TARGET} scanner.cc)
set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 20
        PWN_OPTIMIZATION 3
        PWN_DEBUG        True
        PWN_WARNINGS     2
        PWN_OPENMP       True
        )
pwn_target_properties(${TARGET})
target_compile_definitions(${TARGET} PUBLIC
        -DBOOST_TEST_DYN_LINK
        -DBOOST_TEST_MODULE=${TARGET}
        )
target_link_libraries(${TARGET} PUBLIC
        Boost::unit_test_framework
        pwner
        )

# if (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL PGI)
#set(TARGET TEST_openacc_cc)
#add_executable(${TARGET} openacc.cc)
#set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
##target_use_openacc(${TARGET})
#target_compile_options(${TARGET} PUBLIC ${OpenACC_CXX_FLAGS})
#target_link_libraries(${TARGET} PUBLIC -fopenacc)
#target_use_hardening(${TARGET} 0)
#target_compile_options(${TARGET} PUBLIC
#        -O3
#        )
#
#if (CMAKE_CXX_COMPILER_ID STREQUAL PGI)
#    set(TARGET TEST_openacc_device_cc)
#    add_executable(${TARGET} openacc.cc)
#    set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
#    target_use_openacc(${TARGET})
#    target_use_hardening(${TARGET} 0)
#    target_compile_options(${TARGET} PUBLIC
#            -O3
#            )
#endif()
#
#set(TARGET Process__get_region_address)
#add_executable(${TARGET} get_region_address.cc)
#set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
#target_use_openmp(${TARGET})
##target_use_hardening(${TARGET} 0)
#target_compile_options(${TARGET} PUBLIC
#        -O3
#        )
#target_link_libraries(${TARGET} PUBLIC
#        stdc++fs
#        pwner
#        )
