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