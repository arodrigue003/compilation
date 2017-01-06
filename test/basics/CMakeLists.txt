configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test1.c ${CMAKE_CURRENT_BINARY_DIR}/test1.c @ONLY)
add_test(NAME test1 COMMAND parser test1.c) #compilation test with our compiler
add_test(NAME test1-clang COMMAND clang test1.ll -o test1) #compilation test with clang
add_test(NAME test1-exe COMMAND ./test1) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test2.c ${CMAKE_CURRENT_BINARY_DIR}/test2.c @ONLY)
add_test(NAME test2 COMMAND parser test2.c) #compilation test with our compiler
add_test(NAME test2-clang COMMAND clang test2.ll -o test2) #compilation test with clang
add_test(NAME test2-exe COMMAND ./test2) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test3.c ${CMAKE_CURRENT_BINARY_DIR}/test3.c @ONLY)
add_test(NAME test3 COMMAND parser test3.c) #compilation test with our compiler
add_test(NAME test3-clang COMMAND clang test3.ll -o test3) #compilation test with clang
add_test(NAME test3-exe COMMAND ./test3 arg1 arg2) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test4.c ${CMAKE_CURRENT_BINARY_DIR}/test4.c @ONLY)
add_test(NAME test4 COMMAND parser test4.c) #compilation test with our compiler
add_test(NAME test4-clang COMMAND clang test4.ll -o test4) #compilation test with clang
add_test(NAME test4-exe COMMAND ./test4) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test5.c ${CMAKE_CURRENT_BINARY_DIR}/test5.c @ONLY)
add_test(NAME test5 COMMAND parser test5.c) #compilation test with our compiler
add_test(NAME test5-clang COMMAND clang test5.ll -o test5) #compilation test with clang
add_test(NAME test5-exe COMMAND ./test5) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test6.c ${CMAKE_CURRENT_BINARY_DIR}/test6.c @ONLY)
add_test(NAME test6 COMMAND parser test6.c) #compilation test with our compiler
add_test(NAME test6-clang COMMAND clang test6.ll -o test6) #compilation test with clang
add_test(NAME test6-exe COMMAND ./test6) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test7.c ${CMAKE_CURRENT_BINARY_DIR}/test7.c @ONLY)
add_test(NAME test7 COMMAND parser test7.c) #compilation test with our compiler
add_test(NAME test7-clang COMMAND clang test7.ll -o test7) #compilation test with clang
add_test(NAME test7-exe COMMAND ./test7) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test8.c ${CMAKE_CURRENT_BINARY_DIR}/test8.c @ONLY)
add_test(NAME test8 COMMAND parser test8.c) #compilation test with our compiler
add_test(NAME test8-clang COMMAND clang test8.ll -o test8) #compilation test with clang
add_test(NAME test8-exe COMMAND ./test8) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test9.c ${CMAKE_CURRENT_BINARY_DIR}/test9.c @ONLY)
add_test(NAME test9 COMMAND parser test9.c) #compilation test with our compiler
add_test(NAME test9-clang COMMAND clang test9.ll -o test9) #compilation test with clang
add_test(NAME test9-exe COMMAND ./test9) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test10.c ${CMAKE_CURRENT_BINARY_DIR}/test10.c @ONLY)
add_test(NAME test10 COMMAND parser test10.c) #compilation test with our compiler
add_test(NAME test10-clang COMMAND clang test10.ll -o test10) #compilation test with clang
add_test(NAME test10-exe COMMAND ./test10) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test11.c ${CMAKE_CURRENT_BINARY_DIR}/test11.c @ONLY)
add_test(NAME test11 COMMAND parser test11.c) #compilation test with our compiler
add_test(NAME test11-clang COMMAND clang test11.ll -o test11) #compilation test with clang
add_test(NAME test11-exe COMMAND ./test11) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test12.c ${CMAKE_CURRENT_BINARY_DIR}/test12.c @ONLY)
add_test(NAME test12 COMMAND parser test12.c) #compilation test with our compiler
add_test(NAME test12-clang COMMAND clang test12.ll -o test12) #compilation test with clang
add_test(NAME test12-exe COMMAND ./test12) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test13.c ${CMAKE_CURRENT_BINARY_DIR}/test13.c @ONLY)
add_test(NAME test13 COMMAND parser test13.c) #compilation test with our compiler
add_test(NAME test13-clang COMMAND clang test13.ll -o test13) #compilation test with clang
add_test(NAME test13-exe COMMAND ./test13) #execution test

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test14.c ${CMAKE_CURRENT_BINARY_DIR}/test14.c @ONLY)
add_test(NAME test14 COMMAND parser test14.c) #compilation test with our compiler
add_test(NAME test14-clang COMMAND clang test14.ll -o test14) #compilation test with clang
add_test(NAME test14-exe COMMAND ./test14) #execution test
