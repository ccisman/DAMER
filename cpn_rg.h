//
// Created by cc on 2020/10/10.
//

#ifndef PDNET_CHECKER_CPN_RG_H
#define PDNET_CHECKER_CPN_RG_H

#include "cpn.h"
#define CPNRGTABLE_SIZE 100000
class Marking;
class Binding;
void Marking_after_fire(Marking &marking,CTransition *transition,Binding *bindings,CPN *cpn);
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
    Binding *next;
    Binding(){value=NULL;}
};

class FireTranQ
{
public:
    CTransition *transition;
    vector<Binding*> bindings;
    FireTranQ *next;

    ~FireTranQ(){for(unsigned int i=0;i<bindings.size();i++)delete bindings[i];}
//    void insert(CTransition *transition);
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
    RG_NODE(){tranQ = new FireTranQ;tranQ->next=NULL;firenum = 0;tranQ_obtained = false;next=NULL;}
    ~RG_NODE() {
        while (tranQ) {
            FireTranQ *tmp = tranQ;
            tranQ = tranQ->next;
            delete tmp;
        }
        if (next)
            delete next;
    }
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
    ~RG(){if(init_node)delete init_node;if(rgnodetable)delete[] rgnodetable;}
};
#endif //PDNET_CHECKER_CPN_RG_H