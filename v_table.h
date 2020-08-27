//
// Created by cc on 2020/7/30.
//

#ifndef CPN_PNML_PARSE_V_TABLE_H
#define CPN_PNML_PARSE_V_TABLE_H

#endif //CPN_PNML_PARSE_V_TABLE_H

#pragma once
#include<string>
#include<map>
#include<iostream>
#include<vector>

typedef std::map<std::string, std::string> map_1;

class V_Table
{
public:
    std::string name;
    map_1 v_map;
    V_Table *fa;

public:
    V_Table(std::string na) { name = na; }
    void clear() { v_map.clear(); fa = NULL; }
    void insert(std::string v, std::string place) {
        v_map.insert(std::pair<std::string, std::string>(v, place));
    }
    void erase(std::string v) {
        v_map.erase(v);
    }
    bool find_P(std::string s);
    void connect(V_Table *&father) { fa = father;}
    std::string get_place(std::string v);
};

extern std::vector<V_Table *> v_tables;//variable tables
void init_v_table();
void release_v_table();