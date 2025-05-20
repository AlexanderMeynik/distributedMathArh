# -----------------------------------------------------------------------------
# enable_coverage_for_target(<target>)
# adds coverage flags for the target if build type contains Coverage
# - <target> : name of the test‐executable
# -----------------------------------------------------------------------------
function(enable_coverage_for_target TARGET_NAME)
    set(options PUBLIC PRIVATE INTERFACE)
    set(oneValueArgs SCOPE)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT ARG_SCOPE)
        set(ARG_SCOPE PUBLIC)
    endif()

    string(REGEX MATCH "-Coverage$" IS_COVERAGE_BUILD "${CMAKE_BUILD_TYPE}")
    if(IS_COVERAGE_BUILD AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Enabling coverage for target: ${TARGET_NAME}")

        target_compile_options(${TARGET_NAME} ${ARG_SCOPE}
                $<$<COMPILE_LANGUAGE:C>:-fprofile-arcs -ftest-coverage>
                $<$<COMPILE_LANGUAGE:CXX>:-fprofile-arcs -ftest-coverage>
        )

        target_link_options(${TARGET_NAME} ${ARG_SCOPE}
                -fprofile-arcs -ftest-coverage
        )
    endif()
endfunction()

# -----------------------------------------------------------------------------
# add_env_test(<target> <source> <ARGLIST>)
# Checks required ENV variable names from the list
# and appends found arguments({}) to the arguments of the test exe
# - <target> : name of the test‐executable
# - <source> : path to the .cpp source file
# - <ARGLIST>: Required env names list NAME
# -----------------------------------------------------------------------------
macro(add_env_test TARGET_NAME SOURCE_FILE ARGN)
    set(_missing "")
    set(_args     "")
    message(STATUS "Start test ${TARGET_NAME} configuration  with"
            "required ENVIROMENT variables: ${${ARGN}}")
    foreach(_var IN LISTS ${ARGN})
        if (NOT DEFINED ENV{${_var}})
            list(APPEND _missing ${_var})
        else()
            if (_args)
                string(APPEND _args " $ENV{${_var}}")
            else()
                set(_args "$ENV{${_var}}")
            endif()
        endif()
    endforeach()

    if (_missing)
        string(REPLACE ";" ", " _miss_joined "${_missing}")
        message(WARNING
                "Skipping target ${TARGET_NAME}: "
                "missing environment variable(s): ${_miss_joined}"
        )
    else ()
        separate_arguments(_args_list NATIVE_COMMAND "${_args}")

        add_executable(${TARGET_NAME} ${SOURCE_FILE})
        target_link_libraries(${TARGET_NAME} PUBLIC network_shared_lib gtest gmock)
        enable_coverage_for_target(${TARGET_NAME})

        gtest_add_tests(
                TARGET ${TARGET_NAME}
                EXTRA_ARGS ${_args_list}
        )

        message(STATUS "Configured test ${TARGET_NAME} with args: ${_args_list}")
    endif()
endmacro()

# -----------------------------------------------------------------------------
# add_subdirectory_with_deps(<subdir> <DEP_LIST_VAR> <DIR_LIST_VAR>)
#     <PKG_LIST_VAR>     # a CMake variable name holding a list of package names
#     <DIR_LIST_VAR>     # a CMake variable name holding a list of subdir names
# )
#
# Checks:
#  - for each pkg in ${${PKG_LIST_VAR}}, that <pkg>_FOUND is true
#  - for each dir in ${${DIR_LIST_VAR}}, that it’s been added earlier
#
# If *all* checks pass, calls add_subdirectory(<subdir>) and records it in
# ADDED_SUBDIRECTORIES.  Otherwise emits a WARNING and skips.
# -----------------------------------------------------------------------------
macro(add_subdirectory_with_deps SUBDIR DEP_LIST_VAR DIR_LIST_VAR)
    set(_deps ${${DEP_LIST_VAR}})

    set(_dirs ${${DIR_LIST_VAR}})

    set(_missing "")
    message(STATUS "Start adding '${SUBDIR}' with required "
            "package(s): ${_deps} and subdir(s): ${_dirs}")

    if (NOT DEFINED ADDED_SUBDIRECTORIES)
        set(ADDED_SUBDIRECTORIES "")
    endif()

    foreach(_pkg IN LISTS _deps)
        if (NOT ${_pkg}_FOUND)
            list(APPEND _missing ${_pkg})
        endif()
    endforeach()

    set(_missing_dirs "")
    foreach(_req_dir IN LISTS _dirs)
        list(FIND ADDED_SUBDIRECTORIES ${_req_dir} _idx)
        if (_idx EQUAL -1)
            list(APPEND _missing_dirs ${_req_dir})
        endif()
    endforeach()


    if (_missing OR _missing_dirs)
        if (_missing_pkgs)
        list(JOIN _missing ", " _miss_joined)
        set(_msg_pkg " missing package(s): ${_mp};")
        endif()
        if (_missing_dirs)
            list(JOIN _missing_dirs ", " _md)
            set(_msg_dir " missing subdir(s): ${_md};")
        endif()

        message(WARNING
                "Skipping add_subdirectory(${SUBDIR}):"
                "${_msg_pkg}${_msg_dir}"
        )
    else()
        message(STATUS "Adding '${SUBDIR}'.")
        add_subdirectory(${SUBDIR})
        list(APPEND ADDED_SUBDIRECTORIES ${SUBDIR})
    endif()
endmacro()
