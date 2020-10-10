//
// Created by lewis on 2020/10/10.
//

#include "pdnet_parseoption.h"
#include<map>
std::map<std::string,optcount_t> opt_table = {
        {"-help",0},
//        {"-file",1},
        {"-showtree",0},
        {"-showcpn",0},
        {"-fnum",1},
        {"-noout",0},
        {"-directbuild",0},
        {"-slice",0},
        {"-compare",0}
};

bool cmdlinet::parse(int argc, char **argv) {
    for(int i=1;i<argc;i++){
        if(argv[i][0] == '-'){
            option_t option;
            option.name = argv[i];
            bool exist = opt_exist(option.name);
            if(exist == true)
                continue;
            int para_num = get_paranum(option.name);
            if(para_num != 0){
                for(unsigned int j=0;i<para_num;j++){
                    option.value.push_back(argv[++i]);
                }
            }
            options.push_back(option);
        }
        else
        {
            if(i!=argc-1)
            {
                std::cerr<<"All command must start with '-', you may want to use -"
                <<argv[i]<<" instead"<<std::endl;
            }
            else
                filename = argv[i];
        }
    }
    return true;
}

paracount_t cmdlinet::get_paranum(std::string optstring) {
    auto iter = opt_table.find(optstring);
    if(iter == opt_table.end()) {
        std::cerr<<"There is no command named:"<<optstring<<"! Please check the command."<<std::endl;
        exit(-1);
    }
    return iter->second;
}

bool cmdlinet::opt_exist(std::string optstring) {
    for(unsigned int i=0;i<options.size();i++){
        if(optstring == options[i].name)
            return true;
    }
    return false;
}

void cmdlinet::doit() {
    if(opt_exist("-help")){
        std::cout<<"\n"
                   "* *   PDNet_checker " PDNet_checker_VERSION " 2020-20** ";
        std::cout << "   * *\n";
    }
    if(opt_exist("-showtree")){

    }
    if(opt_exist("-noout")){

    }
    if(opt_exist("-fnum")){

    }
    if(opt_exist("-directbuild")){

    }
    else if(opt_exist("-slice")){

    }
    else if(opt_exist("-compare")){

    }

}

void cmdlinet::help() {
    std::cout<<"\n"
               "* *   PDNet_checker " PDNet_checker_VERSION " 2020-20** ";
    std::cout << "   * *\n";
    std::cout <<
              "* *                        Chen Cheng                       * *\n"
              "* *      Tongji University, Computer Science Department     * *\n"
              "* *                      ccisman@163.com                    * *\n"

              "\n"
              " cbmc [-?] [-h] [--help]      show help\n"
              " cbmc file.c ...              source file names\n"
              "\n"
              "Frontend options:\n"
              " -I path                      set include path (C/C++)\n"
              " -D macro                     define preprocessor macro (C/C++)\n"
              " --preprocess                 stop after preprocessing\n"
              " --16, --32, --64             set width of int\n"
              " --LP64, --ILP64, --LLP64,\n"
              "   --ILP32, --LP32            set width of int, long and pointers\n"
              " --little-endian              allow little-endian word-byte conversions\n"
              " --big-endian                 allow big-endian word-byte conversions\n"
              " --unsigned-char              make \"char\" unsigned by default\n"
              " --show-parse-tree            show parse tree\n"
              " --show-symbol-table          show symbol table\n"
              " --show-goto-functions        show goto program\n"
              " --mm model                   set memory model (default: sc)\n"
              " --arch                       set architecture (default: ";
}

