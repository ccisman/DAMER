//
// Created by cc on 2020/8/8.
//

#ifndef PDNET_CHECKER_CPN_RG_H
#define PDNET_CHECKER_CPN_RG_H

#endif //PDNET_CHECKER_CPN_RG_H

#include "cpn.h"

class Marking
{
public:
    MultiSet *markings;
    NUM_t placecount;

    void init_marking(CPlace *place,NUM_t pc);
    void init_marking(Marking marking);
};

class FireTranQ
{
public:
    CTransition *transition;
    FireTranQ *next;
};


class RG_NODE
{
public:
    Marking marking;
    FireTranQ *tranQ;

    RG_NODE(){tranQ = new FireTranQ;tranQ->next=NULL;}
    void get_FireTranQ(CPN *cpn);
};

class RG
{
public:
    RG_NODE init_node;
    vector<RG_NODE> rgnode;
};