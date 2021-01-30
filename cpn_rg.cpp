//
// Created by cc on 2020/10/10.
//

#include "cpn_rg.h"

extern CPN *cpnet;
extern string arr_suffix ,begin_suffix,end_suffix ,return_suffix ,call_suffix,tid_str,init_tid;


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

vector<String_t > merge_vec(vector<String_t> vec1,vector<String_t> vec2){
    if(vec1.empty())
        return vec2;
    if(vec2.empty()){
        vec1.clear();
        return vec1;
    }
//    bool finded_flag = false;
    for(auto iter = vec1.begin();iter != vec1.end();){
        if(exist_in(vec2,*iter))
            iter++;
        else
            iter = vec1.erase(iter);
    }
    return vec1;
}


Binding* bindingcid(Product_t cid,SORTID sid,condition_tree_node *tokennode){
    condition_tree_node *node = tokennode->left;
    int offset = 0;
    Binding *newbinding,*result;
    result = new Binding;
    result->next = NULL;

    auto sortid = sorttable.productsort[sid].sortid;
    while(node->right){
        if(node->left->node_type == variable){
            if(node->node_name != tid_str) {
                newbinding = new Binding;
                newbinding->variable = node->left->value;
                if (sortid[offset].tid == Integer)
                    newbinding->value = new IntegerSortValue;
                else if (sortid[offset].tid == Real)
                    newbinding->value = new RealSortValue;
                else if (sortid[offset].tid == String)
                    newbinding->value = new StringSortValue;
                else {
                    cerr << "ERROR!variable can not be a productsort!" << endl;
                    exit(-1);
                }

                color_copy(sortid[offset].tid, sortid[offset].sid, cid[offset], newbinding->value);
                newbinding->next = result->next;
                result->next = newbinding;
            }
        }
        offset++;
        node = node->right;
    }
    if(node->node_type == variable){
        if(node->node_name != tid_str) {
            newbinding = new Binding;
            newbinding->variable = node->value;
            if (sortid[offset].tid == Integer)
                newbinding->value = new IntegerSortValue;
            else if (sortid[offset].tid == Real)
                newbinding->value = new RealSortValue;
            else if (sortid[offset].tid == String)
                newbinding->value = new StringSortValue;
            else {
                cerr << "ERROR!variable can not be a productsort!" << endl;
                exit(-1);
            }

            color_copy(sortid[offset].tid, sortid[offset].sid, cid[offset], newbinding->value);
            newbinding->next = result->next;
            result->next = newbinding;
        }
    }
    return result;
}

//binding variables through pattern matching
Binding* bindingToken(condition_tree_node *node,MultiSet *multiset,TID_t tid){


    SORTID sid;
    bool hasindex,hastid;
    Binding *result,*tmpbinding;
    Tokens *tokens = multiset->tokenQ->next;

    result = new Binding;
    result->next = NULL;
    if(!tokens)
        return result;

    //binding Integer, for alloc and mutex cond
    if(multiset->tid == Integer){
        if(node->left->node_name[0] == '_' || isalpha(node->left->node_name[0])) {
            result->next = new Binding;
            result->next->next = NULL;
            result->next->variable = node->left->node_name;
            result->next->value = new IntegerSortValue;
            color_copy(Integer, 0, tokens->color, result->next->value);
            return result;
        }
        else
            return result;
    }

    sid = multiset->sid;
    hasindex = sorttable.productsort[sid].hasindex;
    hastid = sorttable.productsort[sid].hastid;

    int offset = 0;
    Product_t cid;
    cid = new_ProductColor(sid);
    INDEX_t index;
    condition_tree_node *indexnode,*tidnode;
    indexnode = tidnode = node->left;
    if(hasindex && hastid){
        tidnode = tidnode->right->right;
        while(tidnode->right){
            indexnode = indexnode->right;
            tidnode = tidnode->right;
            offset++;
        }
        index = atoi(indexnode->left->value.c_str());
        while(tokens){

            tokens->color->getColor(cid,sid);
            TID_t sub_tid;
            INDEX_t sub_index;
            cid[offset-1+1]->getColor(sub_index);
            cid[offset-1+3]->getColor(sub_tid);
            if(sub_index == index && tid == sub_tid)
            {
                tmpbinding = bindingcid(cid,sid,node);
                Binding *end = tmpbinding;
                while(end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            tokens = tokens->next;
        }
        if(!tokens){
            cerr<<"ERROR!can't binding correctly!"<<endl;
            exit(-1);
        }
    }
    else if(hasindex){
        tidnode = tidnode->right->right;
        while(tidnode){
            indexnode = indexnode->right;
            tidnode = tidnode->right;
            offset++;
        }
        index = atoi(indexnode->left->value.c_str());
        while(tokens){

            tokens->color->getColor(cid,sid);
            Integer_t sub_index;
            cid[offset-1+1]->getColor(sub_index);
            if(sub_index == index)
            {
                tmpbinding = bindingcid(cid,sid,node);
                Binding *end = tmpbinding;
                while(end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            tokens = tokens->next;
        }
        if(!tokens){
            cerr<<"ERROR!can't binding correctly!"<<endl;
            exit(-1);
        }
    }
    else if(hastid){
        tidnode = tidnode->right;
        while(tidnode->right){
            tidnode = tidnode->right;
            offset++;
        }
        while(tokens){

            tokens->color->getColor(cid,sid);
            TID_t sub_tid;
            cid[offset-1+2]->getColor(sub_tid);
            if(sub_tid == tid)
            {
                tmpbinding = bindingcid(cid,sid,node);
                Binding *end = tmpbinding;
                while(end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            tokens = tokens->next;
        }
        if(!tokens){
            cerr<<"ERROR!can't binding correctly!"<<endl;
            exit(-1);
        }
    }
    else{
        tidnode = tidnode->right;
        while(tidnode){
            tidnode = tidnode->right;
            offset++;
        }
        while(tokens){
            tokens->color->getColor(cid,sid);
            {
                tmpbinding = bindingcid(cid, sid, node);
                Binding *end = tmpbinding;
                while (end->next)
                    end = end->next;
                end->next = result->next;
                result->next = tmpbinding->next;
                delete tmpbinding;
                break;
            }
            tokens = tokens->next;
        }
        if(!tokens){
            cerr<<"ERROR!can't binding correctly!"<<endl;
            exit(-1);
        }
    }
    delete[] cid;
    return result;
}

vector<Binding *>get_bindings(CPN *cpn,CTransition *transition,const Marking &marking){
    //Binding control places first
    //transition's happen or not depends on control places

    vector<Binding *>bindings;
    Binding *binding,*tmpbinding;

    vector<TID_t > possible,tmp_vec;
    for(int i=0;i<transition->producer.size();i++){
        index_t idx = transition->producer[i].idx;
        condition_tree_node *root = transition->producer[i].arc_exp.root;
        if(cpn->place[idx].control_P == true) {
            if(cpn->place[idx].initMarking.tid == TID_colorset){
                if(root->node_type == Token){
                    if(root->left->node_type == variable){
                        // arc_exp is 1'tid
                        string var = root->left->value;
                        auto iter = cpn->mapVariable.find(var);
                        if(iter != cpn->mapVariable.end()){
                            TID_t cid;
                            Tokens *token = marking.mss[idx].tokenQ->next;
                            tmp_vec.clear();
                            while(token) {
                                token->color->getColor(cid);
                                tmp_vec.push_back(cid);
                                token = token->next;
                            }
                            possible = merge_vec(possible,tmp_vec);
                            if(possible.empty())
                                return bindings;//empty return
                        }
                        else {
                            cout << "cpn_rg.cpp can't find variable" << endl;
                            exit(-1);
                        }
                    }
                    else{
                        // arc_exp is color or case
                        TID_t cid1,cid2;
                        Tokens *token = marking.mss[idx].tokenQ->next;
                        MultiSet tmp_ms;
                        bool finded = false;
                        tmp_ms.tid = TID_colorset;
                        tmp_ms.sid = 0;
                        while(token) {
                            token->color->getColor(cid1);
                            tmp_ms.Exp2MS(cpn,root,0,0,false);
                            tmp_ms.tokenQ->next->color->getColor(cid2);
                            if(cid1 == cid2) {
                                finded = true;
                                break;
                            }
                            token = token->next;
                        }
                        if(finded)
                            continue;
                        else
                            return bindings;
                    }
                }
                else{
                    cerr << "ERROR!control places' read arc just have one token!"<<endl;
                    exit(-1);
                }
            }
            else if(cpn->place[idx].initMarking.tid == Integer){
                //mutex cond
                Tokens *token = marking.mss[idx].tokenQ->next;
                Integer_t cid_arc = atoi(root->left->node_name.c_str());
                while(token){
                    Integer_t cid_place;
                    token->color->getColor(cid_place);
                    if(cid_arc == cid_place)
                        break;
                    token = token->next;
                }
                if(!token)
                    return bindings;//empty return
            }
        }
    }

    for(int i=0;i<possible.size();i++){
        String_t tid = possible[i];
        binding = new Binding;
        binding->next = new Binding;
        binding->next->variable = tid_str;
        binding->next->value = new StringSortValue;
        binding->next->value->setColor(tid);
        binding->next->next = NULL;
        for(unsigned int j=0;j<transition->producer.size();j++) {
            index_t idx = transition->producer[j].idx;
            if (!cpn->place[idx].control_P) {
                condition_tree_node *root = transition->producer[j].arc_exp.root;
                while (root) {
                    if(root->node_type == CaseOperator && !root->right){
                        root = root->left;
                        continue;
                    }
                    if (root->node_type == Token) {

                        tmpbinding = bindingToken(root, &marking.mss[idx], tid);
                        if(tmpbinding->next) {
                            Binding *end = tmpbinding->next;
                            while (end->next)
                                end = end->next;
                            end->next = binding->next;
                            binding->next = tmpbinding->next;
                            delete tmpbinding;
                        }
                    }
                    if (root->left->node_type == Token) {
                        tmpbinding = bindingToken(root->left, &marking.mss[idx], tid);
                        if(tmpbinding->next) {
                            Binding *end = tmpbinding->next;
                            while (end->next)
                                end = end->next;
                            end->next = binding->next;
                            binding->next = tmpbinding->next;
                            delete tmpbinding;
                        }
                    }
                    root = root->right;
                }
            }
        }
        bindings.push_back(binding);

    }

    return bindings;
}

void BindingVariable(const Binding *binding,CPN *cpn){
    Binding *tmp = binding->next;
    while(tmp){
        auto viter = cpn->mapVariable.find(tmp->variable);
        if(viter == cpn->mapVariable.end()){
            cerr<<"ERROR!BindingVariable failed!"<<endl;
            exit(-1);
        }
        color_copy(cpn->vartable[viter->second].tid,cpn->vartable[viter->second].sid,tmp->value,cpn->vartable[viter->second].value);
        tmp = tmp->next;
    }
}

bool is_Fireable(CTransition *transition,CPN *cpn,vector<Binding *>&bindings,const Marking &marking)
{
    //1.get bindings

    bindings = get_bindings(cpn,transition,marking);
    if(bindings.empty())
        return false;

    //2.judge guard
    if (transition->hasguard) {

        for (auto iter = bindings.begin(); iter != bindings.end();) {
            Integer_t res;
            MultiSet ms;

            ms.tid = Integer;
            ms.sid = 0;
            BindingVariable(*iter,cpn);
            ms.Exp2MS(cpn,transition->guard.root,0,0,false);
            ms.tokenQ->next->color->getColor(res);
            if(res == 0)
                iter = bindings.erase(iter);
            else
                iter++;
        }
    }

    //3.judge producers
//    for(int i=0;i<transition->producer.size();i++)
//    {
//        index_t idx = transition->producer[i].idx;
//        if(!transition->producer[i].onlydot) {
////            MultiSet ms;
////            ms.sid = marking.mss[idx].sid;
////            ms.tid = marking.mss[idx].tid;
////            cpn->CT2MS(transition->producer[i].arc_exp, ms);
//            if (cpn->place[idx].control_P == false)
//                continue;
//            if (marking.mss[idx] >= ms);
//            else
//                return false;
//        }
//        else
//        {
//            if(marking.mss[idx].tokenQ->next==NULL)
//                return false;
//            else if(marking.mss[idx].tokenQ->next->tokencount==0)
//                return false;
//        }
//    }

    if(bindings.empty())
        return false;
    return true;
}

void RG_NODE::get_FireTranQ(CPN *cpn) {

    if(tranQ_obtained)
        return;
    tranQ_obtained = true;
    for(int i=0;i<cpn->transitioncount;i++)
    {
        vector<Binding *>bindings;
        if(is_Fireable(&cpn->transition[i],cpn,bindings,marking)) {
            firenum++;
//            tranQ->insert(&cpn->transition[i]);
            FireTranQ *tmptranQ = new FireTranQ;
            tmptranQ->bindings = bindings;
            tmptranQ->transition = &cpn->transition[i];
            tmptranQ->next = tranQ->next;
            tranQ->next = tmptranQ;

        }
    }
}

index_t RG_NODE::Hash() {
    index_t hv=0;
    for(int i=0;i<marking.placecount;i++)
    {
        marking.mss[i].Hash();
        hv += i * marking.mss[i].hash_value;
    }
    return hv;
}

bool RG_NODE::fireable(string transname) {
    FireTranQ *p = tranQ->next;
    while (p) {
        if(p->transition->id == transname)
            return true;
        p=p->next;
    }
    return false;
}

//void FireTranQ::insert(CTransition *transition) {
//    FireTranQ *temp = next,*newnode;
//    newnode = new FireTranQ;
//    newnode->transition = transition;
//    next = newnode;
//    newnode->next = temp;
//}

void RG::init(CPN *cpn) {


    init_node = new RG_NODE;
    init_node->marking.init_marking(cpn->place,cpn->placecount);
    rgnodetable = new RG_NODE*[CPNRGTABLE_SIZE]();
    init_node->get_FireTranQ(cpn);
    addRGNode(init_node);

}

void Marking_after_fire(Marking &marking,CTransition *transition,Binding *binding,CPN *cpn)
{
    BindingVariable(binding,cpn);
    if(transition->id == "T25"){
        int a;
        a = a+1;
    }
    //1.producer
    for(unsigned int i=0;i<transition->producer.size();i++) {
        MultiSet ms;
        int up;
        index_t idx = transition->producer[i].idx;
        ms.tid = cpn->place[idx].initMarking.tid;
        ms.sid = cpn->place[idx].initMarking.sid;
//        cpn->CT2MS(transition->producer[i].arc_exp,ms);
        if (ms.tid == dot) {
            Tokens *token = new Tokens;
            token->tokencount = 1;
            ms.insert(token);
            marking.mss[idx].MINUS(ms);
        } else {
            if (ms.tid == productsort)
                up = sorttable.productsort[ms.sid].sortnum - 1;
            else
                up = 0;
            ms.Exp2MS(cpn, transition->producer[i].arc_exp.root, 0, up,false);
            marking.mss[idx].MINUS(ms);
        }
    }

    //2.consumer
    for(unsigned int i=0;i<transition->consumer.size();i++) {
        MultiSet ms;
        int up;
        index_t idx = transition->consumer[i].idx;
        ms.tid = cpn->place[idx].initMarking.tid;
        ms.sid = cpn->place[idx].initMarking.sid;
//        cpn->CT2MS(transition->consumer[i].arc_exp,*ms);
        if (ms.tid == dot) {
            Tokens *token = new Tokens;
            token->tokencount = 1;
            marking.mss[idx].insert(token);
        } else {
            if (ms.tid == productsort)
                up = sorttable.productsort[ms.sid].sortnum - 1;
            else
                up = 0;
            ms.Exp2MS(cpn, transition->consumer[i].arc_exp.root, 0, up,false);
            marking.mss[idx].PLUS(ms);
        }
    }
}
unsigned long count=0;
void RG::createNode(RG_NODE *node,CPN *cpn) {

    FireTranQ *tranQ = node->tranQ->next;
    while(tranQ)
    {

        for(unsigned int i=0;i<tranQ->bindings.size();i++) {
            RG_NODE *newnode = new RG_NODE;
            newnode->fathernum = node->num;
            newnode->last_tran = tranQ->transition->id;
            newnode->marking.init_marking(node->marking);
            newnode->next = NULL;
            Marking_after_fire(newnode->marking, tranQ->transition, tranQ->bindings[i], cpn);

//            if(count == 10000)
//                return;
            if (nodeExist(newnode))
                delete newnode;
            else {
                newnode->get_FireTranQ(cpn);
                cout << "count = " << count++ << endl;
                addRGNode(newnode);
                createNode(newnode, cpn);
            }
        }
        tranQ = tranQ->next;
    }
}

RG_NODE* RG::nodeExist(RG_NODE *node) {
    index_t hv = node->Hash() % CPNRGTABLE_SIZE;
    RG_NODE *exist = rgnodetable[hv];
    while(exist)
    {
        if(node->marking == exist->marking)
            return exist;
        exist = exist->next;
    }
    return NULL;
}

void RG::addRGNode(RG_NODE *node) {

    index_t hv = node->Hash() % CPNRGTABLE_SIZE;
    if(rgnodetable[hv] == NULL) {
        rgnodetable[hv] = new RG_NODE;
        rgnodetable[hv] = node;
        rgnodetable[hv]->next = NULL;
        rgnodevec.push_back(node);
        node->num = node_num;
        node_num++;
        return;
    }
    node->next = rgnodetable[hv];
    rgnodetable[hv] = node;
    rgnodevec.push_back(node);
    node->num = node_num;
    node_num++;
//    cout<<node_num<<endl;
}

void RG::GENERATE(CPN *cpn) {
    createNode(init_node,cpn);
}

void RG::print_RG(string filename,CPN *cpn) {
    ofstream fout;
    RG_NODE *node;
    type tid;
    SORTID sid;
    Tokens *tokenQ;
    fout.open(filename, ios::out);
    fout << "total node:" << node_num << " nodes" << endl;
    fout << "show them below" << endl;
    fout << endl;
    for (int i = 0; i < node_num; i++)
    {

        node = rgnodevec[i];
        fout << "node:" << node->num << endl;
        cout << "node:" << node->num << endl;
        for (int j = 0; j < node->marking.placecount; j++)
        {
            fout << cpn->place[j].id << ":";
            cout << cpn->place[j].id << ":";
            tokenQ = node->marking.mss[j].tokenQ->next;
            tid = node->marking.mss[j].tid;
            sid = node->marking.mss[j].sid;
            fout << "(" << cpn->place[j].expression << ",";
            cout << "(" << cpn->place[j].expression << ",";
            while(tokenQ)
            {
                fout << "[" << tokenQ->tokencount << ",";
                cout << "[" << tokenQ->tokencount << ",";
                if(tid == productsort) {
                    int psnum = sorttable.productsort[sid].sortnum;
                    auto sortid = sorttable.productsort[sid].sortid;
                    Product_t cid;
                    cid = new_ProductColor(sid);
                    tokenQ->color->getColor(cid, sid);
                    cout << "{";
                    fout << "{";
                    for (unsigned int k = 0; k < psnum; k++) {
                        if (sortid[k].tid == Integer) {
                            Integer_t subcid;
                            cid[k]->getColor(subcid);
                            cout << subcid << ",";
                            fout << subcid << ",";
                        } else if (sortid[k].tid == Real) {
                            Real_t subcid;
                            cid[k]->getColor(subcid);
                            cout << subcid << ",";
                            fout << subcid << ",";
                        } else if (sortid[k].tid == String) {
                            String_t subcid;
                            cid[k]->getColor(subcid);
                            cout << subcid << ",";
                            fout << subcid << ",";
                        }
                    }
                    cout << "}";
                    fout << "}";
                    delete[]cid;
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
        cout << "successor node:";


        FireTranQ *tranQ = node->tranQ->next;
        if(tranQ) {
            while (tranQ) {

                fout << tranQ->transition->id << " , ";
                cout << tranQ->transition->id << " , ";
                tranQ = tranQ->next;
            }
        }
        else{
            fout << "NULL";
            cout << "NULL";
        }
        cout << endl;
        fout << endl;

        fout <<"father node:"<<node->fathernum<<"    last Tran:"<<node->last_tran;
        cout <<"father node:"<<node->fathernum<<"    last Tran:"<<node->last_tran;

        cout << endl<<endl;
        fout << endl<<endl;
    }

    fout.close();
}