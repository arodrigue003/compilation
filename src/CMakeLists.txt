add_subdirectory(test)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.l @ONLY)
add_custom_command(OUTPUT lex.yy.c
                    COMMAND lex scanner.l
                    DEPENDS scanner.l)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/grammar.y ${CMAKE_CURRENT_BINARY_DIR}/grammar.y @ONLY)
add_custom_command(OUTPUT grammar.cpp grammar.tab.h
                    COMMAND yacc --verbose --debug -d grammar.y -o grammar.cpp --defines=grammar.tab.h
                    DEPENDS grammar.y)

add_library(lexer STATIC lex.yy.c grammar.tab.h)

add_executable(parser grammar.cpp expression_symbols.cpp utilityFunctions.cpp expression.cpp)
target_link_libraries(parser lexer)
