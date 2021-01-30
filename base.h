//
// Created by cc on 2020/8/4.
//

#ifndef PDNET_CHECKER_BASE_H
#define PDNET_CHECKER_BASE_H


extern int string_replace(std::string &s1, const std::string &s2, const std::string &s3);
extern void readGraph(std::string input, std::string output);
extern void makeGraph(std::string inputname, std::string outputname);

extern vector<string> pthread_type;
extern vector<string> pthread_func_type;

template <class T>
bool exist_in(vector<T> &vec,T element)
{
    for(unsigned int i=0;i<vec.size();i++)
        if(element == vec[i])
            return true;

    return false;
}

#endif //PDNET_CHECKER_BASE_H