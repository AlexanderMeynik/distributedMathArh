


file(GLOB common CONFIGURE_DEPENDS
        "*common/*.h"
        "*common/*.cpp"
)
add_executable(common_Tests ${common})
target_link_libraries(common_Tests  allUtils gtest gtest_main)


file(GLOB parallelUtils CONFIGURE_DEPENDS
        "parallelUtils/*.cpp"
)


add_executable(par_Tests ${parallelUtils})
target_link_libraries(par_Tests gtest gmock gtest_main)
target_link_libraries(par_Tests parallelLib)



file(GLOB plotUtils CONFIGURE_DEPENDS
        "plotUtils/*.cpp"
)

add_executable(plot_tests ${plotUtils})
target_link_libraries(plot_tests gtest gmock gtest_main)
target_link_libraries(plot_tests plotUtilsLib)

add_executable(utilsAll ${common_Tests} ${parallelUtils} ${plotUtils})
target_link_libraries(utilsAll gtest gmock gtest_main)
target_link_libraries(utilsAll allUtils)
