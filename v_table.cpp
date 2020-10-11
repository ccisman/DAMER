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
        //exit(-1);
    }
}

void init_v_table()
{
    V_Table *table = new V_Table("global");
    table->fa == NULL;
    v_tables.push_back(table);
}

void release_v_table()
{
    for(unsigned int i=0;i<v_tables.size();i++)
        delete v_tables[i];
}