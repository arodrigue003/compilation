add_subdirectory(lib)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sketch.js ${CMAKE_CURRENT_BINARY_DIR}/sketch.js @ONLY)


set(EXE_NAME "ronds")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.html @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-emcc COMMAND emcc ${EXE_NAME}.ll --js-library lib/p5-wrap.js -s EXPORTED_FUNCTIONS=['_my_draw','_my_setup'] -o ${EXE_NAME}.js) #compilation test with emcc
add_custom_target(run-${EXE_NAME} COMMAND firefox ${EXE_NAME}.html)

set(EXE_NAME "mandelbrot")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.html @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-emcc COMMAND emcc ${EXE_NAME}.ll --js-library lib/p5-wrap.js -s EXPORTED_FUNCTIONS=['_my_draw','_my_setup'] -o ${EXE_NAME}.js) #compilation test with emcc
add_custom_target(run-${EXE_NAME} COMMAND firefox ${EXE_NAME}.html)

set(EXE_NAME "line")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.html @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-emcc COMMAND emcc ${EXE_NAME}.ll --js-library lib/p5-wrap.js -s EXPORTED_FUNCTIONS=['_my_draw','_my_setup'] -o ${EXE_NAME}.js) #compilation test with emcc
add_custom_target(run-${EXE_NAME} COMMAND firefox ${EXE_NAME}.html)

set(EXE_NAME "ellipses")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.html @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-emcc COMMAND emcc ${EXE_NAME}.ll --js-library lib/p5-wrap.js -s EXPORTED_FUNCTIONS=['_my_draw','_my_setup'] -o ${EXE_NAME}.js) #compilation test with emcc
add_custom_target(run-${EXE_NAME} COMMAND firefox ${EXE_NAME}.html)

set(EXE_NAME "grid")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.html @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-emcc COMMAND emcc ${EXE_NAME}.ll --js-library lib/p5-wrap.js -s EXPORTED_FUNCTIONS=['_my_draw','_my_setup'] -o ${EXE_NAME}.js) #compilation test with emcc
add_custom_target(run-${EXE_NAME} COMMAND firefox ${EXE_NAME}.html)

set(EXE_NAME "rec")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.html @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.c ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.c @ONLY)
add_test(NAME ${EXE_NAME} COMMAND compiler ${EXE_NAME}.c) #compilation test with our compiler
add_test(NAME ${EXE_NAME}-emcc COMMAND emcc ${EXE_NAME}.ll --js-library lib/p5-wrap.js -s EXPORTED_FUNCTIONS=['_my_draw','_my_setup'] -o ${EXE_NAME}.js) #compilation test with emcc
add_custom_target(run-${EXE_NAME} COMMAND firefox ${EXE_NAME}.html)
