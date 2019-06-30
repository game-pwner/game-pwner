include_guard(GLOBAL)
find_package(OpenACC QUIET)

define_property(GLOBAL PROPERTY PWN_OPTIMIZATION
        BRIEF_DOCS "Compile-time optimizations"
        FULL_DOCS  "Compile-time optimizations. -O<value>"
        )

define_property(GLOBAL PROPERTY PWN_TUNE
        BRIEF_DOCS "Host machine optimizations"
        FULL_DOCS  "Host machine optimizations. -xHOST -mtune and other"
        )

define_property(GLOBAL PROPERTY PWN_DEBUG
        BRIEF_DOCS "Include debug info"
        FULL_DOCS  "Include debug info. -ggdb<value> -g<value>"
        )

define_property(GLOBAL PROPERTY PWN_SHARED_LIBS
        BRIEF_DOCS "Force shared c++ libs"
        FULL_DOCS  "Force shared c++ libs"
        )

define_property(GLOBAL PROPERTY PWN_WARNINGS
        BRIEF_DOCS "Set warnings"
        FULL_DOCS  "Set warnings. Sets warnings as error if value 1 or greater provided"
        )

define_property(GLOBAL PROPERTY PWN_HARDENING
        BRIEF_DOCS "Set hardening flags"
        FULL_DOCS  "Set hardening flags"
        )

define_property(GLOBAL PROPERTY PWN_OPENMP
        BRIEF_DOCS "Include OpenMP"
        FULL_DOCS  "Include OpenMP"
        )

define_property(GLOBAL PROPERTY PWN_OPENACC
        BRIEF_DOCS "Include OpenACC"
        FULL_DOCS  "Include OpenACC"
        )


function(pwn_target_properties TARGET)
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "${TARGET} is not a target")
    endif()

    get_target_property(PWN_OPTIMIZATION ${TARGET} PWN_OPTIMIZATION)
    get_target_property(PWN_TUNE         ${TARGET} PWN_TUNE)
    get_target_property(PWN_DEBUG        ${TARGET} PWN_DEBUG)
    get_target_property(PWN_SHARED_LIBS  ${TARGET} PWN_SHARED_LIBS)
    get_target_property(PWN_WARNINGS     ${TARGET} PWN_WARNINGS)
    get_target_property(PWN_HARDENING    ${TARGET} PWN_HARDENING)
    get_target_property(PWN_OPENMP       ${TARGET} PWN_OPENMP)
    get_target_property(PWN_OPENACC      ${TARGET} PWN_OPENACC)

    if (PWN_SHARED_LIBS)
        if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
            message(WARNING "pwn_target_properties: PWN_SHARED_LIBS for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        endif()
        add_compile_options(${RE_LIBRARY_NAME} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-shared-libgcc>")
        add_compile_options(${RE_LIBRARY_NAME} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-shared-intel>")   # TODO[low] rpath? ;-Wl,-rpath=/opt/intel/compilers_and_libraries/linux/lib/intel64>")
    endif()

    if (PWN_DEBUG)
        if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
            message(WARNING "pwn_target_properties: PWN_DEBUG for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        endif()
        target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-g;-debug;all>")
        # target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-Wl,-rpath=/opt/intel/compilers_and_libraries/linux/lib/intel64>")
        target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-g3;-ggdb3>")
        target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Clang>:-g3>")
    endif()

    if (PWN_OPTIMIZATION OR PWN_OPTIMIZATION STREQUAL 0)
        if (PWN_OPTIMIZATION MATCHES [0-9]+|g|s|fast)
            target_compile_options(${TARGET} PUBLIC -O${PWN_OPTIMIZATION})
        else()
            message(FATAL_ERROR "pwn_target_properties(): Target '${TARGET}' have invalid value '${PWN_OPTIMIZATION}' for property PWN_OPTIMIZATION.")
        endif()
    endif()

    if (PWN_TUNE)
        if (NOT OpenACC_CXX_FOUND)
            message(WARNING "pwn_target_properties: PWN_OPENACC for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        endif()
        target_compile_options(pwner PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-mtune=native;-march=native;-xHOST>")
        target_compile_options(pwner PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-mtune=native;-march=native>")
        target_compile_options(pwner PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Clang>:-mtune=native;-march=native>")
    endif()

    if (PWN_WARNINGS)
        if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
            message(WARNING "pwn_target_properties: PWN_WARNINGS for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        endif()
        target_compile_options(${TARGET} PUBLIC
                -Wall -Wextra
                -Wno-unused
                -Wconversion
                -Wformat
                -Wmissing-declarations
                #[[GNU,Clang]] "$<$<NOT:$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>>:-Wpedantic>"
                #[[GNU,Clang]] "$<$<NOT:$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>>:-Wstack-protector>"
                )
        set(WERROR_1
                -Werror=format-security
                -Werror=return-type
                )
        set(WERROR_2
                -Werror=empty-body
                -Werror=non-virtual-dtor
                -Werror=address
                -Werror=sequence-point
                )
        if (PWN_WARNINGS EQUAL 1)
            target_compile_options(${TARGET} PUBLIC ${WERROR_1})
        endif()
        if (PWN_WARNINGS EQUAL 2)
            target_compile_options(${TARGET} PUBLIC ${WERROR_1} ${WERROR_2})
        endif()
        if (PWN_WARNINGS GREATER_EQUAL 3)
            target_compile_options(${TARGET} PUBLIC -Werror)
        endif()
    endif()

    if (PWN_HARDENING)
        if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
            message(WARNING "pwn_target_properties: PWN_HARDENING for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        endif()
        set_target_properties(${TARGET} PROPERTIES
                POSITION_INDEPENDENT_CODE True
                )
        if (PWN_HARDENING GREATER_EQUAL 1)
            target_link_options(${TARGET} PUBLIC
                    "LINKER:SHELL:-z defs"
                    "LINKER:SHELL:-z relro -z now"
                    )
        endif()
        if (PWN_HARDENING GREATER_EQUAL 3)
            # Further: https://security.stackexchange.com/questions/24444/what-is-the-most-hardened-set-of-options-for-gcc-compiling-c-c
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

        if (PWN_HARDENING EQUAL 1)
            target_compile_options(${TARGET} PUBLIC -fstack-protector)
            target_compile_definitions(${TARGET} PUBLIC -D_FORTIFY_SOURCE=1)
        elseif (PWN_HARDENING GREATER_EQUAL 2)
            target_compile_options(${TARGET} PUBLIC -fstack-protector-strong)
            target_compile_definitions(${TARGET} PUBLIC -D_FORTIFY_SOURCE=2)
        endif()
    endif()

    if (PWN_OPENMP)
        if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
            message(WARNING "pwn_target_properties: PWN_OPENMP for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        endif()
        target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-qopenmp>")
        target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},GNU>:-fopenmp>")
        target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Clang>:-fopenmp>")
        target_link_libraries(${TARGET} PUBLIC -fopenmp)
        if (PWN_DEBUG)
            target_compile_definitions(${TARGET} PUBLIC -DOMPTARGET_DEBUG)
        endif()
        if (PWN_SHARED_LIBS)
            target_compile_options(${TARGET} PUBLIC "$<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},Intel>:-qopenmp-link;dynamic>")
        endif()
    endif()

    if (PWN_OPENACC)
        if (NOT OpenACC_CXX_FOUND)
            message(WARNING "pwn_target_properties: PWN_OPENACC for ${CMAKE_CXX_COMPILER_ID} compiler is not implemented")
        endif()
        target_compile_options(${TARGET} PUBLIC ${OpenACC_CXX_FLAGS})
    endif()

    #TODO: if (CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES Debug|RelWithDebInfo)
endfunction()
