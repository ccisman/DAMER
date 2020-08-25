cmake_minimum_required(VERSION 3.15.3)
project(PDNet_checker)

set(CMAKE_CXX_STANDARD 14)
set(LINK_DIR /usr/local/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
link_directories(${LINK_DIR})
link_libraries(tcmalloc)

add_executable(PDNet_checker main.cpp AST.cpp y.tab.cpp lex.y.cpp y.tab.h AST.h v_table.h pre_process.cpp base.h base.cpp cpn.h cpn.cpp condition_tree.h condition_tree.cpp cpn_rg.h cpn_rg.cpp slice.cpp product.cpp product.h
        BA/tinystr.cpp
        BA/tinystr.h
        BA/tinyxml.cpp
        BA/tinyxml.h
        BA/tinyxmlerror.cpp
        BA/tinyxmlparser.cpp
        BA/Syntax_Tree.h
        BA/Syntax_Tree.cpp
        BA/VWAA.h
        BA/VWAA.cpp
        BA/buchi.h
        BA/buchi.cpp
        BA/set.h
        LTL2PT.cpp)
target_link_libraries(PDNet_checker tcmalloc)