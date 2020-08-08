//
// Created by cc on 2020/8/8.
//

#include "cpn_rg.h"

void Marking::init_marking(CPlace *place,NUM_t pc) {
    placecount = pc;
    markings = new MultiSet[pc];
    for(int i=0;i<pc;i++)
        markings[i]=place[i].initMarking;
}

void Marking::init_marking(Marking marking) {
    placecount = marking.placecount;
    markings = new MultiSet[placecount];
    for(int i=0;i<placecount;i++)
        markings[i]=marking.markings[i];
}

void RG_NODE::get_FireTranQ(CPN *cpn) {
    for(int i=0;i<cpn->transitioncount;i++)
    {
        for(int j=0;j<cpn->transition[i].producer.size();j++)
        {
            index_t idx = cpn->transition[i].producer[j].idx;
            MultiSet ms;
            ms.sid = cpn->place[idx].initMarking.sid;
            ms.tid = cpn->place[idx].initMarking.tid;
            cpnet->CT2MS(cpn->transition[i].producer[j].arc_exp,ms);
            
        }
    }
}