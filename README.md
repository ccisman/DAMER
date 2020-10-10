# DAMER

an aotumatic tool for modeling and slicing

This project is a tool for building a PDNet model of c program, and alleviate the state space with slicing.
The c program must satisfy ANSI-C grammar.

## Environment
- Deepin 15.11
- CLion

## Usage

- You can build the project through Cmake(make sure the version of Cmake is matched)
- The 'test' folder is where you put the test files, including *.c files and *.xml files. Those *.c files is the original program files, while those *.xml files indicate the checked properties using LTL form. To be noticed, those *.xml files' name must match the *.c files. For example, you want to check a program named 'test.c', the correspond xml file must name as 'test-V.xml'.


To run DAMER, use the following command-line from this directory:

$ cmake-build-debug/PDNet_checker [-showtree] [-showcpn] [-fnum 1] -compare test/triangular-2.c

More command can be seen with $ cmake-build-debug/PDNet_checker -help

## The picture of syntax tree and cpn can be found in this directory, using Graphviz to generate it.

## Program being checked must match the ANSI-C grammar
