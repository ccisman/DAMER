//
// Created by cc on 2020/10/10.
//

#ifndef CPN_PNML_PARSE_V_TABLE_H
#define CPN_PNML_PARSE_V_TABLE_H

#endif //CPN_PNML_PARSE_V_TABLE_H

#pragma once
#include<string>
#include<map>
#include<iostream>
#include<vector>
#define global_table_name "global"

//typedef std::map<std::string, std::string> map_1;
//typedef std::map<std::string,int> map_2;
//typedef std::map<std::string,bool> map_3;
//typedef std::map<std::string,std::vector<std::string>> map_4;

struct Item{
//    std::string name;
    std::string place;
    int size;
    bool is_pointer;
};

class V_Table
{
private:
    std::string name;
//    Item *table[20];
//    int count;
    std::map<std::string,Item> v_map;
//    map_1 v_map;
//    map_2 v_sizemap;
//    map_3 v_ispointer;
//    map_4 v_pointerrelated;
    V_Table *fa;

public:
    V_Table(std::string na) { name = na; fa = NULL;}
    V_Table(){fa = NULL;}
    void clear();
    void insert(std::string v, std::string place,int size,bool ispointer);
//    void Add_pointerrelated(std::string v,std::string place);
//    void erase(std::string v) {
//        v_map.erase(v);
//        v_sizemap.erase(v);
//        v_ispointer.erase(v);
//    }
    std::string get_name(){return name;}
    bool find_P(std::string s);
    void connect(V_Table *&father) { fa = father;}
    std::string get_place(std::string v);
    int get_size(std::string v);
    bool get_ispointer(std::string v);
    bool get_isglobal(std::string v);
//    std::vector<std::string> get_pointerrelated(std::string v);
};

extern std::vector<V_Table *> v_tables;//variable tables
void init_v_table();
void release_v_table();