# DAMER

an aotumatic tool for modeling and slicing multiple-thread c programs(PThread programs).

This project is a tool for building a PDNet model of c program, and alleviate the state space with slicing.
The c program must satisfy ANSI-C grammar.


## Environment
- Deepin 15.11
- CLion
- tcmalloc
- Graphviz

## installation 
### install Graphviz.

if you have not installed Graphviz yet, use the following command-line to install Graphviz

$ sudo apt-get install graphviz

### install tcmalloc.

if you have not installed tcmalloc yet, use the following command-lines to install tcmalloc

First, you should install m4

$ wget http://mirrors.kernel.org/gnu/m4/m4-1.4.13.tar.gz \
&& tar -xzvf m4-1.4.13.tar.gz \
&& cd m4-1.4.13 \
&& ./configure --prefix=/usr/local

$ make && make install
$ cd ..

if there is an error occured, use the following commands:

#$ sed -i 's/IO_ftrylockfile/IO_EOF_SEEN/' lib/*.c

#$ echo "#define _IO_IN_BACKUP 0x100" >> lib/stdio-impl.h

#$ sudo make install

Second, you should install autoconf

$ wget http://mirrors.kernel.org/gnu/autoconf/autoconf-2.65.tar.gz \
&& tar -xzvf autoconf-2.65.tar.gz \
&& cd autoconf-2.65 \
&& ./configure --prefix=/usr/local

$ sudo make && make install
$ cd ..

Third, you should install automake

$ wget http://mirrors.kernel.org/gnu/automake/automake-1.11.1tar.gz \
&& tar xzvf automake-1.11.1tar.gz \
&& cd automake-1.11.1 \
&& ./configure --prefix=/usr/local

$ sudo make && make install
$ cd ..

Forth, you should install libtool

$ wget http://mirrors.kernel.org/gnu/libtool/libtool-2.2.6b.tar.gz \
&& tar xzvf libtool-2.2.6b.tar.gz \
&& cd libtool-2.2.6b \
&& ./configure --prefix=/usr/local

$ sudo make && make install
$ cd ..

Fifth, you should install libunwind

$ wget http://download.savannah.gnu.org/releases/libunwind/libunwind-1.1.tar.gz
&& tar -xf libunwind-1.1.tar.gz
&& cd libunwind-1.1
&& ./configure --prefix=/usr/local 

$ sudo make && make install
$ cd ..

Finally, you can install google-perftools

get the newest google-perftools-*.tar.gz from https://github.com/gperftools/gperftools/releases

$ tar zxvf gperftools-*.tar.gz
&& cd gperftools-*
&& ./configure --prefix=/usr/local

$ sudo make && make install

$ echo "/usr/local/lib" > /etc/ld.so.conf 

## Usage

- You can build the project through Cmake(make sure the version of Cmake is matched)
- The 'test' folder is where you put the test files, including *.c files and *.xml files. Those *.c files is the original program files, while those *.xml files indicate the checked properties using LTL form. To be noticed, those *.xml files' name must match the *.c files. For example, you want to check a program named 'test.c', the correspond xml file must name as 'test-V.xml'. And make sure the .c file and the .xml file are in the same directory.


To run DAMER, use the following command-line from this directory:

$ cmake-build-debug/PDNet_checker [-showtree] [-showcpn] [-fnum 1] (-compare|-directbuild|-slice) (filename)

for example:

$ cmake-build-debug/PDNet_checker -fnum 1 -compare test/fib_bench-1.c

More command can be seen with:

$ cmake-build-debug/PDNet_checker -help

The help interface may be like that if running correctly.

![Image text](https://github.com/ccisman/DAMER/blob/master/pic/help.png)

The model check interface may be like that if running correctly.(-compare can be replaced by -directbuild or -slice, more details can be seen in -help)

![Image text](https://github.com/ccisman/DAMER/blob/master/pic/compare.png)
