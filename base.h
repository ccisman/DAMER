//
// Created by cc on 2020/8/4.
//

#ifndef PDNET_CHECKER_BASE_H
#define PDNET_CHECKER_BASE_H

#include<string>

extern int string_replace(std::string &s1, const std::string &s2, const std::string &s3);
extern void readGraph(std::string input, std::string output);
extern void makeGraph(std::string inputname, std::string outputname);
template <class T>
extern bool exist_in(vector<T> vec,T element);

extern vector<string> pthread_type;
extern vector<string> pthread_func_type;

#endif //PDNET_CHECKER_BASE_H