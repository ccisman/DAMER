//
// Created by lewis on 2020/10/10.
//

#ifndef PDNET_CHECKER_PDNET_PARSEOPTION_H
#define PDNET_CHECKER_PDNET_PARSEOPTION_H

#define PDNet_checker_VERSION "0.0.1"
#include<iostream>
#include<string>
#include<vector>
typedef std::string optname_t;
typedef std::string optvalue_t;
typedef unsigned short optcount_t;
typedef unsigned short paracount_t;

struct option_t{
public:
    optname_t name;
    std::vector<optvalue_t> value;
};

class cmdlinet{
public:
    bool parse(int argc,char **argv);
    paracount_t get_paranum(std::string optstring);
    bool opt_exist(std::string optstring);
    void help();
    void doit();
protected:
    std::vector<option_t> options;
    std::string filename;
};


#endif //PDNET_CHECKER_PDNET_PARSEOPTION_H
