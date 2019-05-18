project(pwner_gui VERSION ${CMAKE_PROJECT_VERSION})



function(ui2py _file_ui _file_py)
    execute_process(
            COMMAND pyuic5 ${_file_ui}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_FILE ${_file_py}
            ERROR_QUIET
            )
    message(STATUS "OUTPUT_FILE ${_file_py}")
endfunction()

file(GLOB_RECURSE ${PROJECT_NAME}_UI_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")
foreach(FILE_UI ${${PROJECT_NAME}_UI_LIST})
    get_filename_component(FILE_PATH ${FILE_UI} DIRECTORY)
    get_filename_component(FILE_BASENAME ${FILE_UI} NAME_WLE)
    ui2py("${FILE_UI}" "${FILE_PATH}/Ui_${FILE_BASENAME}.py")
endforeach()


# TODO[high]: CMakeLists.txt: build python
#add_custom_target(pwner_gui
#        COMMAND python main.py
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        SOURCES main.py
#        )
