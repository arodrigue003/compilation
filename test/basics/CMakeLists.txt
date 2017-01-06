set(EXE_NAME "test1")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test2")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test3")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME} lol 42) #execution test

set(EXE_NAME "test4")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test5")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test6")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test7")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test8")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test9")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test10")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test11")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test12")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test13")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test14")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test15")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test

set(EXE_NAME "test16")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-clang COMMAND clang ${EXE_NAME}.ll -o ${EXE_NAME}) #compilation test with clang
add_test(NAME ${EXE_NAME}-exe COMMAND ./${EXE_NAME}) #execution test
