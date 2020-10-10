#include <iostream>
#include"pdnet_parseoption.h"

int main(int argc,char **argv) {

    cmdlinet cmd;
    cmd.parse(argc,argv);
    cmd.doit();

/********    Testing.....    ***********/
//    vector<string> files;
//    get filename in origin_dirname
//    so we can check all the .c file in origin_dirname
//    GetFileNames(origin_dirname,files);
//    for(unsigned int i=0;i<files.size();i++)
//        for(unsigned int num=1;num<=3;num++)
//            construct_and_slice(files[i],ltltype,num,gen_picture);
    return 0;
}
