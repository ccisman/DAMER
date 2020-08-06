cmake_minimum_required(VERSION 3.17)
project(PDNet_checker)

set(CMAKE_CXX_STANDARD 14)

add_executable(PDNet_checker main.cpp AST.cpp y.tab.cpp lex.y.cpp y.tab.h AST.h v_table.h pre_process.cpp base.h base.cpp cpn.h cpn.cpp condition_tree.h condition_tree.cpp)