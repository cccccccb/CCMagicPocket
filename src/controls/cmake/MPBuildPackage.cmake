include(CMakeParseArguments)

set(_mp_tmp_build_source_path ${CMAKE_BINARY_DIR}/temp)
set(_mp_current_cmake_source_path ${CMAKE_CURRENT_LIST_DIR})

function(_mp_starts_with_prefix var prefix result)
    string(LENGTH ${prefix} prefix_length)
    string(SUBSTRING ${var} 0 ${prefix_length} prefix_part)
    if(prefix_part STREQUAL ${prefix})
        set(${result} TRUE PARENT_SCOPE)
    else()
        set(${result} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(_mp_internal_collect_dependencies target output_var)
    get_target_property(dependencies ${target} LINK_LIBRARIES)
    get_target_property(interface_dependencies ${target} INTERFACE_LINK_LIBRARIES)
    list(APPEND dependencies ${interface_dependencies})

    foreach(dep IN LISTS dependencies)
        if(TARGET ${dep})
            get_target_property(dep_type ${dep} TYPE)
            _mp_starts_with_prefix("${dep}" "Qt6" HAS_QT_DEPS)
            if (NOT ${HAS_QT_DEPS})
                _mp_starts_with_prefix("${dep}" "CCQuickStartup" HAS_QT_DEPS)
            endif()

            if(dep_type STREQUAL "SHARED_LIBRARY" AND NOT ${HAS_QT_DEPS})
                list(APPEND ${output_var} $<TARGET_FILE:${dep}>)
                _mp_internal_collect_dependencies(${dep} ${output_var})
            endif()
        else()
            message(DEBUG "Target ${dep} not found. Skipping...")
        endif()
    endforeach()

    set(${output_var} "${${output_var}}" PARENT_SCOPE)
endfunction()

function(_mp_internal_create_target ACTIVITY_NAME)
    set(oneValueArgs ICON_PATH OUTPUT_DIRECTORY PRELOAD ENTITY)
    set(multiValueArgs PRELOAD_LIBRARIES ENTITY_LIBRARIES)

    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${oneValueArgs}" "${multiValueArgs}")

    set(tmp_build_source_path ${_mp_tmp_build_source_path}/${ACTIVITY_NAME})
    set(output_target_file_path ${arg_OUTPUT_DIRECTORY}/${ACTIVITY_NAME}.mpk)

    add_custom_command(
        OUTPUT ${tmp_build_source_path}/Activity.json
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${output_target_file_path}
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/temp
        COMMAND ${CMAKE_COMMAND} -E make_directory ${tmp_build_source_path}
        COMMAND ${CMAKE_COMMAND} -E copy ${arg_ICON_PATH} ${tmp_build_source_path}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${tmp_build_source_path}/preload
        COMMAND ${CMAKE_COMMAND} -E copy ${arg_PRELOAD_LIBRARIES} ${tmp_build_source_path}/preload
        COMMAND ${CMAKE_COMMAND} -E make_directory ${tmp_build_source_path}/entity
        COMMAND ${CMAKE_COMMAND} -E copy ${arg_ENTITY_LIBRARIES} ${tmp_build_source_path}/entity
        COMMAND ${CMAKE_COMMAND} -D MP_SOURCE_PRELOADPATH=$<TARGET_FILE:${arg_PRELOAD}> -D MP_SOURCE_ENTITYPATH=$<TARGET_FILE:${arg_ENTITY}> -P ${CMAKE_CURRENT_BINARY_DIR}/${ACTIVITY_NAME}_configure_activity_json.cmake
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${ACTIVITY_NAME}_configure_activity_json.cmake ${arg_PRELOAD} ${arg_ENTITY}
    )

    add_custom_target(
        ${ACTIVITY_NAME}_copy_configure ALL
        DEPENDS ${tmp_build_source_path}/Activity.json
        COMMENT "Copy dynamic libraries and configuring Activity.json"
    )

    get_filename_component(ICON_FILE_NAME ${arg_ICON_PATH} NAME)
    add_custom_command(
        OUTPUT ${output_target_file_path}
        COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${output_target_file_path} --format=zip
                ${tmp_build_source_path}/preload ${tmp_build_source_path}/entity
                ${tmp_build_source_path}/Activity.json
                ${tmp_build_source_path}/${ICON_FILE_NAME}
        WORKING_DIRECTORY ${tmp_build_source_path}/
        DEPENDS ${ACTIVITY_NAME}_copy_configure
        COMMENT "Packaging dynamic libraries and their dependencies into ${output_target_file_path}"
    )

    add_custom_target(
        ${ACTIVITY_NAME}_package_libraries ALL
        DEPENDS ${output_target_file_path}
        COMMENT "Packaging dynamic libraries target"
    )
endfunction()

function(mp_build_package ACTIVITY_NAME)
    set(needSpecifyArgs NAME ICON PRELOAD ENTITY)
    set(oneValueArgs VERSION DESCRIPTION OUTPUT_DIRECTORY ${needSpecifyArgs})
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${oneValueArgs}" "")

    foreach(specify_arg IN LISTS ${needSpecifyArgs})
        if (NOT arg_${specify_arg})
            message(FATAL_ERROR
                "Need specify the NAME argument."
            )
        endif()
    endforeach()

    if (NOT arg_VERSION)
        set(arg_VERSION "v0.0.1")
    endif()

    if (NOT arg_DESCRIPTION)
        set(arg_VERSION "")
    endif()

    if (NOT arg_OUTPUT_DIRECTORY)
        set(arg_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    endif()

    get_filename_component(ICON_FILE_NAME ${arg_ICON} NAME)

    set(MP_ACTIVITY_NAME ${ACTIVITY_NAME})
    set(MP_ACTIVITY_REAL_NAME ${arg_NAME})
    set(MP_ACTIVITY_VERSION ${arg_VERSION})
    set(MP_DESCRIPTION ${arg_DESCRIPTION})
    set(MP_ICONPATH ${ICON_FILE_NAME})
    set(MP_BUILD_ACTIVITY_PATH ${_mp_tmp_build_source_path}/${ACTIVITY_NAME})
    set(MP_CURRENT_CMAKE_SOURCE_PATH ${_mp_current_cmake_source_path})

    configure_file(
        ${_mp_current_cmake_source_path}/../template/configure_activity_json.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${ACTIVITY_NAME}_configure_activity_json.cmake
        @ONLY
    )

    set(preload_library_files "")
    set(entity_library_files "")

    get_target_property(target_type ${arg_PRELOAD} TYPE)
    if(target_type STREQUAL "MODULE_LIBRARY")
        list(APPEND preload_library_files $<TARGET_FILE:${arg_PRELOAD}>)
        _mp_internal_collect_dependencies(${arg_PRELOAD} preload_library_files)
    endif()

    get_target_property(target_type ${arg_ENTITY} TYPE)
    if(target_type STREQUAL "MODULE_LIBRARY")
        list(APPEND entity_library_files $<TARGET_FILE:${arg_ENTITY}>)
        _mp_internal_collect_dependencies(${arg_ENTITY} entity_library_files)
    endif()

    _mp_internal_create_target(${ACTIVITY_NAME}
        ICON_PATH ${arg_ICON}
        PRELOAD_LIBRARIES ${preload_library_files}
        ENTITY_LIBRARIES ${entity_library_files}
        PRELOAD ${arg_PRELOAD}
        ENTITY ${arg_ENTITY}
        OUTPUT_DIRECTORY ${arg_OUTPUT_DIRECTORY})

endfunction()
