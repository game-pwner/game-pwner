# ┌──────────────────────────────────────────────────────────────────┐
#   This is main library
# └──────────────────────────────────────────────────────────────────┘

find_package(Boost 1.45.0 COMPONENTS iostreams serialization)

# ┌──────────────────────────────────────────────────────────────────┐
#   Target declaration
# └──────────────────────────────────────────────────────────────────┘
version_write(${CMAKE_PROJECT_NAME} HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/pwner/version.hh")
version_write(${CMAKE_PROJECT_NAME} SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cc")

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh")
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
add_library(pwner SHARED ${${PROJECT_NAME}_SOURCES_LIST})
target_include_directories(pwner
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}-${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}>
        )

# ┌──────────────────────────────────────────────────────────────────┐
#   Compilation flags
# └──────────────────────────────────────────────────────────────────┘
set_target_properties(pwner PROPERTIES
        CXX_STANDARD 20
        VERSION   ${${CMAKE_PROJECT_NAME}_VERSION}
        SOVERSION ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}
        PWN_OPTIMIZATION 3
        PWN_TUNE         False
        PWN_DEBUG        True
        PWN_SHARED_LIBS  False
        PWN_WARNINGS     1
        PWN_HARDENING    0
        PWN_OPENMP       True
        )
pwn_target_properties(pwner)
target_compile_options(pwner PUBLIC
        -fsplit-stack
        )
target_link_libraries(pwner PUBLIC
        stdc++fs
        rt
        bitmask
        Boost::iostreams
        Boost::serialization
        )
add_dependencies(pwner bitmask)

#pvs_studio_add_target(TARGET pwner.PVS ALL
#        OUTPUT FORMAT errorfile
#        ANALYZE pwner
#        MODE GA:1,2,3 OP
#        LOG pwner.err
#        LICENSE ${CMAKE_SOURCE_DIR}/utils/PVS-Studio.lic
#        )


# ┌──────────────────────────────────────────────────────────────────┐
#   Install target
# └──────────────────────────────────────────────────────────────────┘
#include(GNUInstallDirs) # todo[low]: is this needless?
# todo[critical]: CMakeLists.txt: VERSIONING like mylibConfigVersion.cmake!
#include(CMakePackageConfigHelpers)

# Add namespaces to match exported target
#add_library(${CMAKE_PROJECT_NAME}::pwner ALIAS pwner) # todo[low]: needless?

# Installation
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}-${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR})
install(TARGETS pwner
        EXPORT ${CMAKE_PROJECT_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
install(EXPORT ${CMAKE_PROJECT_NAME}-targets
        NAMESPACE ${CMAKE_PROJECT_NAME}::
        FILE ${CMAKE_PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
        )

# TODO[cricital]: CMakeLists.txt: 1. Windows capability; 2. ldconfig should be called
file(WRITE "/etc/ld.so.conf.d/pwner.conf" "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# todo[critical]: CMakeLists.txt: should we remove export?
#export(TARGETS pwner FILE pwner-Config.cmake)
#export(PACKAGE pwner)


# ┌──────────────────────────────────────────────────────────────────┐
#   Uninstall target
# └──────────────────────────────────────────────────────────────────┘
#[[ if prefix is /usr
rm -rf /usr/include/pwner*
rm -f /usr/lib*/libpwner.so*
rm -rf /usr/lib*/cmake/pwner*
rm -f /usr/lib*/python3.7/site-packages/pwner.so*
]]

#[[ if prefix is /usr/local
rm -rf /usr/local/include/pwner*
rm -f /usr/local/lib*/libpwner.so*
rm -rf /usr/local/lib*/cmake/pwner*
rm -f /usr/lib*/python3.7/site-packages/pwner.so*
]]


# ┌──────────────────────────────────────────────────────────────────┐
#   Subdirectories
# └──────────────────────────────────────────────────────────────────┘
add_subdirectory(bindings)
