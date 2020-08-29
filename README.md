# PDNet_checker

an aotumatic tool for modeling and slicing

This project is a tool for building a PDNet model of c program, and alleviate the state space with slicing.
The c program must satisfy ANSI-C grammar.

## Environment
- Deepin 15.11
- CLion


## Usage

- You can build the project in CLion through Cmake(make sure the version of Cmake is matched)
- I'm sorry that it has no user interface for now.

- In our program, you can put the checked program and corresponding LTL formula files(which is a .xml file) in the direction indicated by the variable "origin_dirname" in main.cpp, the corresponding LTL formula files' name is the program's name + "-V" or "-F". For example, a checked program named "program.c"
Its corresponding LTL formula file named "program-V.xml" or "program-F.xml". We have given some example programs and their corresponding formulas in test. 


## Program being checked must match the ANSI-C grammar
