//
// Created by cc on 2020/8/8.
//

#include "cpn_rg.h"
extern CPN *cpnet;
extern string arr_suffix ,begin_suffix,end_suffix ,return_suffix ,call_suffix ;
void Marking::init_marking(CPlace *place,NUM_t pc) {
    placecount = pc;
    mss = new MultiSet[pc];
    for(int i=0;i<pc;i++)
        mss[i]=place[i].initMarking;
}

void Marking::init_marking(Marking &marking) {
    placecount = marking.placecount;
    mss = new MultiSet[placecount];
    for(int i=0;i<placecount;i++)
        mss[i]=marking.mss[i];
}

bool Marking::operator==(Marking &marking) {
    if(placecount != marking.placecount)
        return false;
    for(int i=0;i<placecount;i++)
    {
        if(mss[i]==marking.mss[i])
            ;
        else
            return false;
    }
    return true;
}

bool is_Fireable(CTransition *transition,CPN *cpn,vector<Binding *>&bindings,const Marking &marking)
{
    //1.binding
    Binding *binding;
    for(int i=0;i<transition->producer.size();i++)
    {
        index_t idx = transition->producer[i].idx;

        if(cpn->place[idx].control_P == false) {
            //now just for normal variable
            string var = transition->producer[i].arc_exp.exp;
            auto iter = cpn->mapVariable.find(var);
            if(iter!=cpn->mapVariable.end())
            {
                if(marking.mss[idx].tid == productsort) {
                    int psnum = sorttable.productsort[marking.mss[idx].sid].sortnum;
                    SortValue **cid;
                    marking.mss[idx].tokenQ->next->color->getColor(cid,psnum);
                    binding = new Binding;
                    binding->variable = var;
                    binding->value = new ProductSortValue(psnum);
                    binding->value->setColor(cid,psnum);
                    bindings.push_back(binding);
                    cpn->vartable[iter->second].value->setColor(cid,psnum);
                }
                else if(marking.mss[idx].tid == Integer)
                {
                    Integer_t cid;
                    marking.mss[idx].tokenQ->next->color->getColor(cid);
                    binding = new Binding;
                    binding->variable = var;
                    binding->value = new IntegerSortValue;
                    binding->value->setColor(cid);
                    bindings.push_back(binding);
                    cpn->vartable[iter->second].value->setColor(cid);
                }
                else if(marking.mss[idx].tid == Real)
                {
                    Real_t cid;
                    marking.mss[idx].tokenQ->next->color->getColor(cid);
                    binding = new Binding;
                    binding->variable = var;
                    binding->value = new RealSortValue;
                    binding->value->setColor(cid);
                    bindings.push_back(binding);
                    cpn->vartable[iter->second].value->setColor(cid);
                }
                else if(marking.mss[idx].tid == String)
                {
                    String_t cid;
                    marking.mss[idx].tokenQ->next->color->getColor(cid);
                    binding = new Binding;
                    binding->variable = var;
                    binding->value = new StringSortValue;
                    binding->value->setColor(cid);
                    bindings.push_back(binding);
                    cpn->vartable[iter->second].value->setColor(cid);
                }
                else if(marking.mss[idx].tid == dot)
                {;}
            }
            else
            {
                cout<<"cpn_rg.cpp can't find variable"<<endl;
                exit(-1);
            }
        }
    }
    //2.judge guard
    if(transition->hasguard) {
        cpn->CTN_cal(transition->guard.root);
        if (transition->guard.root->value == "0")
            return false;
    }
    //3.judge producers
    for(int i=0;i<transition->producer.size();i++)
    {
        index_t idx = transition->producer[i].idx;
        if(!transition->producer[i].onlydot) {
            MultiSet ms;
            ms.sid = marking.mss[idx].sid;
            ms.tid = marking.mss[idx].tid;
            cpn->CT2MS(transition->producer[i].arc_exp, ms);
            if (cpn->place[idx].control_P == false)
                continue;
            if (marking.mss[idx] >= ms);
            else
                return false;
        }
        else
        {
            if(marking.mss[idx].tokenQ->next==NULL)
                return false;
            else if(marking.mss[idx].tokenQ->next->tokencount==0)
                return false;
        }
    }


    return true;
}

void RG_NODE::get_FireTranQ(CPN *cpn) {

    for(int i=0;i<cpn->transitioncount;i++)
    {
        vector<Binding *>bindings;
        if(is_Fireable(&cpn->transition[i],cpn,bindings,marking)) {
            firenum++;
            tranQ->insert(&cpn->transition[i]);
            tranQ->next->bindings = bindings;
        }
    }
}

index_t RG_NODE::Hash() {
    index_t hv=0;
    for(int i=0;i<marking.placecount;i++)
    {
        marking.mss[i].Hash();
        hv += marking.mss[i].hash_value;
    }
    return hv;
}


void FireTranQ::insert(CTransition *transition) {
    FireTranQ *temp = next,*newnode;
    newnode = new FireTranQ;
    newnode->transition = transition;
    next = newnode;
    newnode->next = temp;
}

void RG::init(CPN *cpn) {

    auto iter = cpn->mapFunction.find("main" + begin_suffix);
    string main_P = iter->second;
    auto iter2 = cpn->mapPlace.find(main_P);
    Tokens *token = new Tokens;
    token->tokencount = 1;
    cpn->place[iter2->second].initMarking.insert(token);

    init_node = new RG_NODE;
    init_node->marking.init_marking(cpn->place,cpn->placecount);
    rgnodetable = new RG_NODE*[CPNRGTABLE_SIZE]();
    addRGNode(init_node);
}

void Marking_after_fire(Marking &marking,CTransition *transition,vector<Binding *>bindings,CPN *cpn)
{
    string variable;
    for(unsigned int i=0;i<bindings.size();i++)
    {
        variable = bindings[i]->variable;
        auto iter = cpn->mapVariable.find(variable);
        type tid = cpn->vartable[iter->second].tid;
        SORTID sid = cpn->vartable[iter->second].sid;
        int psnum = 0;
        if(tid == productsort) {
            SortValue **cid;
            psnum = sorttable.productsort[sid].sortnum;
            bindings[i]->value->getColor(cid,psnum);
            cpn->vartable[iter->second].value->setColor(cid,psnum);
        }
        else if(tid == Integer)
        {
            Integer_t cid;
            bindings[i]->value->getColor(cid);
            cpn->vartable[iter->second].value->setColor(cid);
        }
        else if(tid == Real)
        {
            Real_t cid;
            bindings[i]->value->getColor(cid);
            cpn->vartable[iter->second].value->setColor(cid);
        }
        else if(tid == String)
        {
            String_t cid;
            bindings[i]->value->getColor(cid);
            cpn->vartable[iter->second].value->setColor(cid);
        }

    }
    //1.producer
    for(unsigned int i=0;i<transition->producer.size();i++)
    {
        MultiSet ms;
        index_t idx=transition->producer[i].idx;
        ms.tid = cpn->place[idx].initMarking.tid;
        ms.sid = cpn->place[idx].initMarking.sid;
        cpn->CT2MS(transition->producer[i].arc_exp,ms);
        marking.mss[idx].MINUS(ms);
    }

    //2.consumer
    for(unsigned int i=0;i<transition->consumer.size();i++)
    {
        MultiSet *ms = new MultiSet;//Here we use pointer,cause PLUS function use insert
        index_t idx=transition->consumer[i].idx;
        ms->tid = cpn->place[idx].initMarking.tid;
        ms->sid = cpn->place[idx].initMarking.sid;
        cpn->CT2MS(transition->consumer[i].arc_exp,*ms);
        marking.mss[idx].PLUS(*ms);
    }
}

void RG::createNode(RG_NODE *node,CPN *cpn) {
    node->get_FireTranQ(cpn);
    FireTranQ *tranQ = node->tranQ->next;
    while(tranQ)
    {
        if(tranQ->transition->id == "T16")
        {
            int a=a+1;
        }
        RG_NODE *newnode = new RG_NODE;
        newnode->marking.init_marking(node->marking);
        Marking_after_fire(newnode->marking,tranQ->transition,tranQ->bindings,cpn);
        //if(nodeExist(newnode))
        //    ;
        //else {
            addRGNode(newnode);
            createNode(newnode, cpn);
        //}
        tranQ = tranQ->next;
    }
}

bool RG::nodeExist(RG_NODE *node) {
    index_t hv = node->Hash() % CPNRGTABLE_SIZE;
    RG_NODE *exist = rgnodetable[hv];
    while(exist)
    {
        if(node->marking == exist->marking)
            return true;
        exist = exist->next;
    }
    return false;
}

void RG::addRGNode(RG_NODE *node) {

    index_t hv = node->Hash() % CPNRGTABLE_SIZE;
    if(rgnodetable[hv] == NULL) {
        rgnodetable[hv] = new RG_NODE;
        rgnodetable[hv] = node;
        rgnodetable[hv]->next = NULL;
        rgnodevec.push_back(node);
        node_num++;
        return;
    }
    node->next = rgnodetable[hv];
    rgnodetable[hv] = node;
    rgnodevec.push_back(node);
    node_num++;
}

void RG::GENERATE(CPN *cpn) {
    createNode(init_node,cpn);
}

void RG::print_RG(string filename,CPN *cpn) {
    ofstream fout;
    RG_NODE *node;
    type tid;
    Tokens *tokenQ;
    fout.open(filename, ios::out);
    fout << "total node:" << node_num << " nodes" << endl;
    fout << "show them below" << endl;
    fout << endl;
    for (int i = 0; i < node_num; i++)
    {

        node = rgnodevec[i];
        fout << "node:" << i << endl;
        cout << "node:" << i << endl;
        for (int j = 0; j < node->marking.placecount; j++)
        {
            tokenQ = node->marking.mss[j].tokenQ->next;
            tid = node->marking.mss[j].tid;
            fout << "(" << cpn->place[j].expression << ",";
            cout << "(" << cpn->place[j].expression << ",";
            while(tokenQ)
            {
                fout << "[" << tokenQ->tokencount << ",";
                cout << "[" << tokenQ->tokencount << ",";
                if(tid == productsort)
               {

               }
               else if(tid == Integer)
               {
                   Integer_t cid;
                   tokenQ->color->getColor(cid);
                   fout << cid ;
                   cout << cid ;
               }
               else if(tid == Real)
               {
                   Real_t cid;
                   tokenQ->color->getColor(cid);
                   fout << cid ;
                   cout << cid ;
               }
               else if(tid == String)
               {
                   String_t cid;
                   tokenQ->color->getColor(cid);
                   fout << cid ;
                   cout << cid ;
               }
               else if(tid == dot)
               {
//                   String_t cid;
//                   tokenQ->color->getColor(cid);
                   fout << "dot";
                   cout << "dot" ;
               }
                fout << "] ";
                cout << "] " ;
                tokenQ = tokenQ->next;
            }

            fout << ")    ";
            cout << ")    " ;
        }

        fout << endl;
        cout << endl;

        fout << "successor node:";
        cout << endl;
        FireTranQ *tranQ = node->tranQ->next;
        while(tranQ)
        {

            fout << tranQ->transition->id << " , " ;
            cout << tranQ->transition->id << " , " ;
            tranQ = tranQ->next;
        }
        cout << endl;
        fout << endl;
    }

    fout.close();
}