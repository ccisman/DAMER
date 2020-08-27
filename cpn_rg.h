//
// Created by cc on 2020/8/8.
//

#ifndef PDNET_CHECKER_CPN_RG_H
#define PDNET_CHECKER_CPN_RG_H

#include "cpn.h"
#define CPNRGTABLE_SIZE 10000
class Marking;
class Binding;
void Marking_after_fire(Marking &marking,CTransition *transition,vector<Binding *>bindings,CPN *cpn);
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
    string last_tran;
    NUM_t num;
    NUM_t fathernum;
    NUM_t firenum;

    bool tranQ_obtained;
    index_t Hash();
    RG_NODE(){tranQ = new FireTranQ;tranQ->next=NULL;firenum = 0;tranQ_obtained = false;}
    void get_FireTranQ(CPN *cpn);
    bool fireable(string transname);
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
    RG_NODE* nodeExist(RG_NODE *node);
    void GENERATE(CPN *cpn);
    void print_RG(string filename,CPN *cpn);
    RG(){node_num=0;}
    ~RG(){if(init_node)delete init_node;if(rgnodetable)delete rgnodetable;}
};
#endif //PDNET_CHECKER_CPN_RG_H