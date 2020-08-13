cmake_minimum_required(VERSION 3.17)
project(PDNet_checker)

set(CMAKE_CXX_STANDARD 14)

add_executable(PDNet_checker main.cpp AST.cpp y.tab.cpp lex.y.cpp y.tab.h AST.h v_table.h pre_process.cpp base.h base.cpp cpn.h cpn.cpp condition_tree.h condition_tree.cpp cpn_rg.h cpn_rg.cpp slice.cpp
        BA/tinystr.cpp
        BA/tinystr.h
        BA/tinyxml.cpp
        BA/tinyxml.h
        BA/tinyxmlerror.cpp
        BA/tinyxmlparser.cpp

        BA/Data_Structures.h
        BA/Data_Structures.cpp
        BA/Lexer.h
        BA/Lexer.cpp
        BA/SBA.h
        BA/SBA.cpp
        BA/Syntax_Tree.h
        BA/Syntax_Tree.cpp
        BA/TBA.h
        BA/TBA.cpp
        BA/TGBA.h
        BA/TGBA.cpp
        BA/xml2ltl.h
        BA/xml2ltl.cpp)