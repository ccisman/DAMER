//
// Created by lewis on 2020/10/10.
//

#include"v_table.h"

bool V_Table::find_P(std::string s){
    auto iter = v_map.begin();
    for(;iter!=v_map.end();iter++)
        if(iter->second == s)
            return true;
    return false;
}

std::string V_Table::get_place(std::string v) {
    map_1::iterator iter;
    iter = v_map.find(v);
    if (iter != v_map.end())
    {
        return iter->second;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_map.find(v);
            if (iter != temp_fa->v_map.end())
                return iter->second;
            else
                temp_fa = temp_fa->fa;
        }
        std::cout << "can't find variable " << v << std::endl;
        exit(-1);
    }
}

int V_Table::get_size(std::string v) {
    map_2::iterator iter;
    iter = v_sizemap.find(v);
    if (iter != v_sizemap.end())
    {
        return iter->second;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_sizemap.find(v);
            if (iter != temp_fa->v_sizemap.end())
                return iter->second;
            else
                temp_fa = temp_fa->fa;
        }
        std::cout << "can't find variable_size " << v << std::endl;
        exit(-1);
    }
    return 0;
}

bool V_Table::get_ispointer(std::string v) {
    map_3::iterator iter;
    iter = v_ispointer.find(v);
    if (iter != v_ispointer.end())
    {
        return iter->second;
    }
    else
    {
        V_Table *temp_fa = fa;
        while (temp_fa != NULL)
        {
            iter = temp_fa->v_ispointer.find(v);
            if (iter != temp_fa->v_ispointer.end())
                return iter->second;
            else
                temp_fa = temp_fa->fa;
        }
        std::cout << "can't find variable_size " << v << std::endl;
        exit(-1);
    }
    return false;
}

bool V_Table::get_isglobal(std::string v) {
    auto iter = v_map.find(v);
    if(iter != v_map.end()){
        if(name == global_table_name)
            return true;
        else
            return false;
    }
    else
//        if(fa)
            return fa->get_isglobal(v);
//        else
//            return false;

    return false;
}

//void V_Table::Add_pointerrelated(std::string v, std::string place) {
//    map_4::iterator iter;
//    iter = v_pointerrelated.find(v);
//    if (iter != v_pointerrelated.end())
//    {
//        std::vector<std::string> tmp;
//        tmp = iter->second;
//        tmp.push_back(place);
//        iter->second = tmp;
//    }
//    else
//    {
//        V_Table *temp_fa = fa;
//        while (temp_fa != NULL)
//        {
//            iter = temp_fa->v_pointerrelated.find(v);
//            if (iter != temp_fa->v_pointerrelated.end()) {
//                std::vector<std::string> tmp;
//                tmp = iter->second;
//                tmp.push_back(place);
//                iter->second = tmp;
//            }
//            else
//                temp_fa = temp_fa->fa;
//        }
//        std::cout << "can't find variable pointerrelated" << v << std::endl;
//        //exit(-1);
//    }
//}
//
//std::vector<std::string> V_Table::get_pointerrelated(std::string v) {
//    map_4::iterator iter;
//    iter = v_pointerrelated.find(v);
//    if (iter != v_pointerrelated.end())
//        return iter->second;
//    else
//    {
//        V_Table *temp_fa = fa;
//        while (temp_fa != NULL)
//        {
//            iter = temp_fa->v_pointerrelated.find(v);
//            if (iter != temp_fa->v_pointerrelated.end())
//                return iter->second;
//            else
//                temp_fa = temp_fa->fa;
//        }
//        std::cout << "can't find variable " << v << std::endl;
//        //exit(-1);
//    }
//}

void init_v_table()
{
    V_Table *table = new V_Table(global_table_name);
    table->fa == NULL;
    v_tables.push_back(table);
}

void release_v_table()
{
    for(unsigned int i=0;i<v_tables.size();i++)
        delete v_tables[i];
}