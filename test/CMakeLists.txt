add_executable ( test_01 main.c)
target_link_libraries ( test_01 cyan cyan_hwcam )

add_test(NAME test_01 COMMAND ./test )

