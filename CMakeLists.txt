cmake_minimum_required(VERSION 3.5)

project(dshell LANGUAGES C)

set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}")

find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.0 REQUIRED)

set(LEXER_OUT "${PARSER_DIR}/lexer.c")
set(PARSER_OUT "${PARSER_DIR}/parser.c")

flex_target(LEXER "lexer.l" "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/lexer.h")
bison_target(PARSER "parser.y" "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parser.h")
add_flex_bison_dependency(LEXER PARSER)

add_executable(dshell main.c "${LEXER_OUT}" "${PARSER_OUT}")
target_link_libraries(dshell m  ${CMAKE_DL_LIBS})
target_include_directories(dshell PRIVATE "${PARSER_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
