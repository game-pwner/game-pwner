cmake_minimum_required(VERSION 3.13)
#include(ExternalProject)
include(FetchContent) # https://cmake.org/cmake/help/v3.14/module/FetchContent.html
find_package(PkgConfig)


if (NOT DEFINED EXTERNAL_PREFIX_DIR)
    set(EXTERNAL_PREFIX_DIR ${CMAKE_BINARY_DIR}/install CACHE PATH "external libraries build path" FORCE)
endif()

if (NOT DEFINED EXTERNAL_HEADER_DIR)
    set(EXTERNAL_HEADER_DIR ${EXTERNAL_PREFIX_DIR}/include CACHE INTERNAL "" FORCE)
endif()

if (NOT DEFINED EXTERNAL_LINKING_DIR)
    set(EXTERNAL_LINKING_DIR "${EXTERNAL_PREFIX_DIR}/lib;${EXTERNAL_PREFIX_DIR}/bin" CACHE INTERNAL "" FORCE)
endif()

include_directories(${EXTERNAL_HEADER_DIR})
link_directories(${EXTERNAL_LINKING_DIR})

# ┌──────────────────────────────────────────────────────────────────┐
#   External: bitmask
# └──────────────────────────────────────────────────────────────────┘
FetchContent_Declare(bitmask
        GIT_REPOSITORY https://github.com/oliora/bitmask.git
        GIT_SHALLOW TRUE
        )

FetchContent_GetProperties(bitmask)
if(NOT bitmask_POPULATED)
    FetchContent_Populate(bitmask)
    add_subdirectory(${bitmask_SOURCE_DIR} ${bitmask_BINARY_DIR})
endif()

# ┌──────────────────────────────────────────────────────────────────┐
#   External: pybind11
# └──────────────────────────────────────────────────────────────────┘
FetchContent_Declare(pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_SHALLOW TRUE
        )

FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()

# ┌──────────────────────────────────────────────────────────────────┐
#   External: radare2
# └──────────────────────────────────────────────────────────────────┘
#ExternalProject_Add(ex_radare2
#        GIT_REPOSITORY      https://github.com/radare/radare2.git
#        GIT_TAG             master
#        INSTALL_DIR         ${EXTERNAL_PREFIX_DIR}
#        CONFIGURE_COMMAND   sh -c "cd <SOURCE_DIR>; ./configure --with-syscapstone --with-openssl --prefix=/usr"
#        BUILD_COMMAND       sh -c "cd <SOURCE_DIR>; make"  # -j12 causes broken library
#        INSTALL_COMMAND     sh -c "cd <SOURCE_DIR>; make 'DESTDIR=<INSTALL_DIR>' install"
#        )
##[[set(R2_PACKAGES r_anal r_asm r_bin r_bp r_config
#        r_cons r_core r_crypto r_debug r_egg r_flag r_fs r_hash
#        r_io r_lang r_parse r_reg r_search r_socket
#        r_syscall r_util)]]
#set(R2_PACKAGES r_bin)
#pkg_check_modules(R2 QUIET ${R2_PACKAGES})
#if (NOT R2_FOUND)
#    set(CMAKE_PREFIX_PATH "${EXTERNAL_PREFIX_DIR}")
#    pkg_check_modules(R2 QUIET ${R2_PACKAGES})
#    if (NOT R2_FOUND)
#        message(WARNING "Build ex_radare2 first")
#    endif()
#endif()


#FetchContent_Declare(radare2
#        GIT_REPOSITORY https://github.com/oliora/bitmask.git
#        GIT_SHALLOW TRUE
#        )
#
#FetchContent_GetProperties(radare2)
#if(NOT radare2_POPULATED)
#    FetchContent_Populate(radare2)
#    add_subdirectory(${radare2_SOURCE_DIR} ${radare2_BINARY_DIR})
#endif()

# ┌──────────────────────────────────────────────────────────────────┐
#   Deps
# └──────────────────────────────────────────────────────────────────┘
#add_custom_target(ex_all
#        DEPENDS
#        $<$<NOT:$<STREQUAL:${bitmask_FOUND},1>>:ex_bitmask>
#        $<$<NOT:$<STREQUAL:${R2_FOUND},1>>:ex_radare2>
#        )

