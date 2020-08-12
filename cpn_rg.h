//
// Created by cc on 2020/8/8.
//

#ifndef PDNET_CHECKER_CPN_RG_H
#define PDNET_CHECKER_CPN_RG_H

#endif //PDNET_CHECKER_CPN_RG_H

#include "cpn.h"
#define CPNRGTABLE_SIZE sizeof(Integer_t)

class Marking
{
public:
    MultiSet *mss;
    NUM_t placecount;

    void init_marking(CPlace *place,NUM_t pc);
    void init_marking(Marking &marking);
    bool operator==(Marking &marking);

    ~Marking(){delete[] mss;};
};

class Binding
{
public:
    string variable;
    SortValue *value;
};

class FireTranQ
{
public:
    CTransition *transition;
    vector<Binding*> bindings;
    FireTranQ *next;
    void insert(CTransition *transition);
};


class RG_NODE
{
public:
    Marking marking;
    FireTranQ *tranQ;
    RG_NODE *next;
    NUM_t firenum;

    index_t Hash();
    RG_NODE(){tranQ = new FireTranQ;tranQ->next=NULL;firenum = 0;}
    void get_FireTranQ(CPN *cpn);
};

class RG
{
public:
    RG_NODE *init_node;
    RG_NODE **rgnodetable;
    vector<RG_NODE *> rgnodevec;
    NUM_t node_num;

    void init(CPN *cpn);
    void createNode(RG_NODE *node,CPN *cpn);
    void addRGNode(RG_NODE *node);
    bool nodeExist(RG_NODE *node);
    void GENERATE(CPN *cpn);
    void print_RG(string filename,CPN *cpn);
    RG(){node_num=0;}
};