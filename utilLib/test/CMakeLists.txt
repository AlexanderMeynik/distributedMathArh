file(GLOB common CONFIGURE_DEPENDS
        "*common/*.h"
        "*common/*.cpp"
)
add_executable(common_Tests ${common})
target_link_libraries(common_Tests allUtils gtest gtest_main)
enable_coverage_for_target(common_Tests)
gtest_discover_tests(common_Tests NO_PRETTY_TYPES NO_PRETTY_VALUES)

file(GLOB parallelUtils CONFIGURE_DEPENDS
        "parallelUtils/*.cpp"
)
add_executable(par_Tests ${parallelUtils})
target_link_libraries(par_Tests gtest gmock gtest_main)
target_link_libraries(par_Tests parallelLib)
enable_coverage_for_target(par_Tests)
gtest_discover_tests(par_Tests NO_PRETTY_TYPES NO_PRETTY_VALUES)


file(GLOB_RECURSE testingUtils CONFIGURE_DEPENDS
        "testingUtils/*.cpp"
)

add_executable(testingUtilsTests ${testingUtils})
target_link_libraries(testingUtilsTests testingUtilsLib gtest gtest_main)
enable_coverage_for_target(testingUtilsTests)
gtest_discover_tests(testingUtilsTests NO_PRETTY_TYPES NO_PRETTY_VALUES)

file(GLOB plotUtils CONFIGURE_DEPENDS
        "plotUtils/*.cpp"
)

add_executable(plot_tests ${plotUtils})
target_link_libraries(plot_tests commonLib gtest gtest_main)
enable_coverage_for_target(plot_tests)
gtest_discover_tests(plot_tests NO_PRETTY_TYPES NO_PRETTY_VALUES)

#[[add_executable(utilsAll ${common} ${parallelUtils} ${plotUtils} ${testingUtils})
target_link_libraries(utilsAll allUtils testingUtilsLib gtest gtest_main)
enable_coverage_for_target(utilsAll)
gtest_discover_tests(utilsAll NO_PRETTY_TYPES NO_PRETTY_VALUES)]]

