# DAMER

an aotumatic tool for modeling and slicing multiple-thread c programs(PThread programs).

This project is a tool for building a PDNet model of c program, and alleviate the state space with slicing.
The c program must satisfy ANSI-C grammar.


## Environment
- Deepin 15.11
- CLion


## Usage

- You can build the project through Cmake(make sure the version of Cmake is matched)
- The 'test' folder is where you put the test files, including *.c files and *.xml files. Those *.c files is the original program files, while those *.xml files indicate the checked properties using LTL form. To be noticed, those *.xml files' name must match the *.c files. For example, you want to check a program named 'test.c', the correspond xml file must name as 'test-V.xml'. And make sure the .c file and the .xml file are in the same directory.


To run DAMER, use the following command-line from this directory:

$ cmake-build-debug/PDNet_checker [-showtree] [-showcpn] [-fnum 1] (-compare|-directbuild|-slice) (filename)

for example:
$ cmake-build-debug/PDNet_checker -fnum 1 -compare test/fib_bench-1.c

More command can be seen with:
$ cmake-build-debug/PDNet_checker -help

## The picture of syntax tree and cpn can be found in this directory, using Graphviz to generate it, so make sure you have installed Graphviz.
you can use the following command-line to install Graphviz

$ sudo apt-get install graphviz

The help interface may be like that if running correctly.

![Image text](https://github.com/ccisman/DAMER/blob/master/pic/help.png)

The model check interface may be like that if running correctly.(-compare can be replaced by -directbuild or -slice, more details can be seen in -help)

![Image text](https://github.com/ccisman/DAMER/blob/master/pic/compare.png)
