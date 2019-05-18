#fixme[high]: versioning.cmake: related https://github.com/yrnkrn/zapcc/blob/master/utils/git/find-rev why do they uses python????
#todo[med]: versioning.cmake: do caching
string(TIMESTAMP VERSION_TIMESTAMP "%Y-%m-%d %H:%M" UTC)

function(get_git_revision _vcs_revision)
    if(NOT GIT_FOUND)
        find_package(Git QUIET)
    endif()
    execute_process(
#             COMMAND echo 1234567
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE SHORT_HASH
            RESULT_VARIABLE SHORT_HASH_RESULT
            ERROR_QUIET
            )
    set(${_vcs_revision} "${SHORT_HASH}" PARENT_SCOPE)
endfunction()


function(get_vcs_revision _vcs_type _vcs_revision)
    get_git_revision(VCS_REVISION)
    if (VCS_REVISION)
        set(${_vcs_type} "git" PARENT_SCOPE)
        set(${_vcs_revision} ${VCS_REVISION} PARENT_SCOPE)
        return()
    endif()
#    get_hg_revision(VCS_REVISION)
#    if (VCS_REVISION)
#        set(${_vcs_type} "git" PARENT_SCOPE)
#        set(${_vcs_revision} ${VCS_REVISION} PARENT_SCOPE)
#        return()
#    endif()
endfunction()


function(get_version_vcs _project _version_vcs)
    get_vcs_revision(VCS_NAME VERSION_PATCH)
    if (VCS_NAME)
        set(${_version_vcs} "${${_project}_VERSION}+${VCS_NAME}.${VERSION_PATCH}" PARENT_SCOPE)
    else()
        set(${_version_vcs} "${${_project}_VERSION}" PARENT_SCOPE)
    endif()
endfunction()


function(get_version_full _project _version_full)
    get_version_vcs(${_project} VERSION_VCS)
    set(${_version_full} "${VERSION_VCS} ${VERSION_TIMESTAMP}" PARENT_SCOPE)
endfunction()


function(version_print _project)
    get_version_full(${_project} VERSION_FULL)
    message(STATUS "${_project} v${VERSION_FULL}")
endfunction()


function(version_write _project _type _file)
    get_vcs_revision(${_project} VCS_TYPE VCS_REVISION)
    get_version_vcs(${_project} VERSION_VCS)
    string(TOUPPER ${_project} PROJECT)
    if (${_type} STREQUAL HEADER)
        # TODO[critical]: CMakeLists.txt: WRITE at once or multiple APPEND (makefiles generating issue (is it CMake-side?))
        file(WRITE ${_file} "// DO NOT EDIT: Generated with CMake, function `version_write`
#pragma once
#include <cstddef>
const size_t ${PROJECT}_VERSION_MAJOR = ${${_project}_VERSION_MAJOR};
const size_t ${PROJECT}_VERSION_MINOR = ${${_project}_VERSION_MINOR};
const size_t ${PROJECT}_VERSION_PATCH = ${${_project}_VERSION_PATCH}+0;
const char *${PROJECT}_VERSION = \"${${_project}_VERSION}\";
const char *${PROJECT}_VERSION_VCS_TYPE = \"${VCS_TYPE}\";
const char *${PROJECT}_VERSION_VCS_REVISION = \"${VCS_REVISION}\";
const char *${PROJECT}_VERSION_TIMESTAMP = \"${VERSION_TIMESTAMP}\";
const char *${PROJECT}_VERSION_WATERMARK = \"This is the ${_project} library version ${VERSION_VCS}\";
")
    elseif(${_type} STREQUAL SOURCE)
        file(WRITE ${_file} "// DO NOT EDIT: Generated with CMake, function `version_write`
const char *${PROJECT}_VERSION_WATERMARK = \"This is the ${_project} library version ${VERSION_VCS}\";
")
    endif()
endfunction()
