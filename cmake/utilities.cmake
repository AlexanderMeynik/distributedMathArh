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
    message(STATUS "Start test ${TARGET_NAME} configuration "
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