find_package(OpenACC QUIET)
get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)

set(WARNED_ACC_IMPL FALSE CACHE INTERNAL "")

function(target_use_openmp TARGET)
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(FATAL_ERROR "target_use_openmp() for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
    endif()
    target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-qopenmp>")
    target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-fopenmp>")
    target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Clang>:-fopenmp>")
    target_link_libraries(${TARGET} PUBLIC -fopenmp)
endfunction()

function(target_use_openacc TARGET)
#    message(WARNING "target_use_openacc() is not tested")  # TODO[low]: test cmake target_use_openacc and add unit tests
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()
#    todo also try "get_property(ResultVariable TARGET TargetName PROPERTY CXX_STANDARD)"
    if (NOT OpenACC_CXX_FOUND AND NOT WARNED_ACC_IMPL)
        message(WARNING "target_use_openacc() for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        set(WARNED_ACC_IMPL TRUE CACHE INTERNAL "")
        target_compile_options(${TARGET} PUBLIC -fopenacc)
    else()
        if("C" IN_LIST languages)
            target_compile_options(${TARGET} PUBLIC ${OpenACC_C_FLAGS})
        elseif("CXX" IN_LIST languages)
            target_compile_options(${TARGET} PUBLIC ${OpenACC_CXX_FLAGS})
        endif()
    endif()
    target_link_libraries(${TARGET} PUBLIC -fopenacc)
endfunction()


function(target_use_shared_libs TARGET)
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()
    add_compile_options(${RE_LIBRARY_NAME} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-shared-libgcc>")
    add_compile_options(${RE_LIBRARY_NAME} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-shared-intel;-Wl,-rpath=/opt/intel/compilers_and_libraries/linux/lib/intel64>")  # TODO[low] rpath?
endfunction()


function(target_use_debug TARGET)
    # HH=opt && diff -y -t <(gcc -O0 -Q --help=${HH}) <(gcc -O1 -Q --help=${HH}) | grep '| '
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()
    target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-g;-debug;all>")
    target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-qopenmp-link;dynamic>")
    # target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-Wl,-rpath=/opt/intel/compilers_and_libraries/linux/lib/intel64>")
    target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-g3;-ggdb3>")
    target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Clang>:-g3>")

    # target_compile_definitions(${TARGET} PUBLIC -DOMPTARGET_DEBUG)
endfunction()


function(target_use_hardening TARGET LEVEL)
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()
    # Use "-fPIC" / "-fPIE" for all targets by default, including static libs
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    # CMake doesn't add "-pie" by default for executables (CMake issue #14983)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")
    target_compile_definitions(${TARGET} PUBLIC
            -D_FORTIFY_SOURCE=2
            )

    if (LEVEL GREATER_EQUAL 1)
        target_link_options(${TARGET} PUBLIC
                "LINKER:SHELL:-z defs"
                "LINKER:SHELL:-z relro -z now"
                )
    endif()
    # Further: https://security.stackexchange.com/questions/24444/what-is-the-most-hardened-set-of-options-for-gcc-compiling-c-c
    if (LEVEL GREATER_EQUAL 2)
        target_compile_options(${TARGET} PUBLIC
                # Attempt to compile code without unintended return addresses, making ROP just a little harder.
                #[[GNU]] "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-mmitigate-rop>"
                # Enables retpoline (return trampolines) to mitigate some variants of Spectre V2. The second flag is necessary on Skylake+ due to the fact that the branch target buffer is vulnerable.
                #[[GNU]] "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-mindirect-branch=thunk;-mfunction-return=thunk>"
                # Defeats a class of attacks called stack clashing
                #[[GNU]] "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-fstack-clash-protection>"
                # Generates traps for signed overflow
                #[[GNU]] "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-ftrapv>"
                )
    endif()

    if (LEVEL EQUAL 1)
        target_compile_options(${TARGET} PUBLIC -fstack-protector)
        target_compile_definitions(${TARGET} PUBLIC -D_FORTIFY_SOURCE=1)
    elseif (LEVEL GREATER_EQUAL 2)
        target_compile_options(${TARGET} PUBLIC -fstack-protector-strong)
        target_compile_definitions(${TARGET} PUBLIC -D_FORTIFY_SOURCE=2)
    endif()
endfunction()


function(target_use_warnings TARGET LEVEL)
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()
    target_compile_options(${TARGET} PUBLIC
            -Wall -Wextra
            -Wno-unused
            -Wconversion -Wsign-conversion
            -Wformat
            #[[GNU,Clang]] "$<$<NOT:$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>>:-Wpedantic>"
            #[[GNU,Clang]] "$<$<NOT:$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>>:-Wstack-protector>"
            )
    if (LEVEL GREATER_EQUAL 1)
        target_compile_options(${TARGET} PUBLIC
                -Werror=format-security
                -Werror=return-type
                )
    endif()
    if (LEVEL GREATER_EQUAL 3)
        target_compile_options(${TARGET} PUBLIC
                -Werror
                )
    endif()
endfunction()

function(target_use_optimizations_host TARGET)
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
        message(FATAL_ERROR "\${ENABLE_ARCH_SPECIFIC_INSTRUCTIONS} for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        # target_compile_options(pwner PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},MSVC>:-mtune=native>")
    endif()
    target_compile_options(pwner PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-mtune=native;-march=native;-xHOST>")
    target_compile_options(pwner PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-mtune=native;-march=native>")
    target_compile_options(pwner PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Clang>:-mtune=native;-march=native>")  # TODO[med] is mtune implemented in Clang?
endfunction()

