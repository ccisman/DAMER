//
// Created by cc on 2020/8/4.
//

#include "cpn.h"
#include "v_table.h"

SortTable sorttable;
SORTID SortTable::psptr = 0;

SORTID SortTable::usptr = 0;

vector<V_Table *> v_tables;//variable tables
int v_tables_count = 0;

int gen_P_num=0,gen_T_num=0;

map<string,type> map_build_in_type {
        {"void",dot},
        {"char",String},
        {"short",Integer},
        {"int",Integer},
        {"long",Integer},
        {"float",Real},
        {"double",Real}
};
string arr_suffix = "_arr";

void Tokens::initiate(token_count_t tc, type sort, int PSnum) {
    tokencount = tc;
    if(sort == productsort)
        color = new ProductSortValue(PSnum);
    else if(sort == Integer)
        color = new IntegerSortValue;
    else if(sort == Real)
        color = new RealSortValue;
    else if(sort == String)
        color = new StringSortValue;
    else
    {
        cout<<"error type!"<<endl;
        exit(-1);
    }
}

int ps_cmp(type_union *ps1,type_union *ps2,vector<MSI> psinfo)
{
    for(unsigned int i=0;i<psinfo.size();i++)
    {
        if(psinfo[i].tid == Integer)
        {
            if(ps1[i].integer < ps2[i].integer)
               return -1;
            else if(ps1[i].integer == ps2[i].integer)
                continue;
            else
                return 1;
        }
        else if(psinfo[i].tid == Real)
        {
            if(ps1[i].real < ps2[i].real)
                return -1;
            else if(ps1[i].real == ps2[i].real)
                continue;
            else
                return 1;
        }
        else if(psinfo[i].tid == String)
        {
            int cmp = strncmp(ps1[i].str.s_str,ps2[i].str.s_str,ps1[i].str.s_size);
            if (cmp<0)
                return -1;
            else if(cmp==0)
                continue;
            else
                return 1;
        }
        else
        {
            cout<<"tid error!"<<endl;
            exit(-1);
        }
    }
    return 0;
}

void MultiSet::insert(Tokens *token) {

    Tokens *p = tokenQ->next , *q = tokenQ;

    if (tid == productsort) {
        type_union *cid1,*cid2;
        int psnum = sorttable.productsort[sid].sortnum;
        vector<MSI> psinfo = sorttable.productsort[sid].sortid;
        cid1 = new type_union[psnum];
        cid2 = new type_union[psnum];
        token->color->getColor(cid2,psnum);
        int cmp;
        while(p)
        {
            p->color->getColor(cid1,psnum);
            cmp = ps_cmp(cid1,cid2,psinfo);
            if(cmp>=0)
                break;
            q=p;
            p=p->next;
        }
        if(cmp == 0) {
            p->tokencount+=token->tokencount;
            delete token;
        }
        else
        {
            q->next = token;
            token->next=p;
            color_count++;
        }
        delete[] cid1;
        delete[] cid2;
    }
    else if (tid == Integer) {
        Integer_t cid1,cid2;
        token->color->getColor(cid2);
        while(p)
        {
            p->color->getColor(cid1);
            if(cid1>=cid2)
                break;
            q=p;
            p=p->next;
        }
        if(cid1 == cid2) {
            p->tokencount+=token->tokencount;
            delete token;
        }
        else
        {
            q->next = token;
            token->next=p;
            color_count++;
        }

    }
    else if (tid == Real) {
        Real_t cid1,cid2;
        token->color->getColor(cid2);
        while(p)
        {
            p->color->getColor(cid1);
            if(cid1>=cid2)
                break;
            q=p;
            p=p->next;
        }
        if(cid1 == cid2) {
            p->tokencount+=token->tokencount;
            delete token;
        }
        else
        {
            q->next = token;
            token->next=p;
            color_count++;
        }
    }
    else if (tid == String) {
        String_t cid1,cid2;
        token->color->getColor(cid2);
        while(p)
        {
            p->color->getColor(cid1);
         if(strncmp(cid1.s_str,cid2.s_str,cid1.s_size) >= 0)
                break;
            q=p;
            p=p->next;
        }
        if(strncmp(cid1.s_str,cid2.s_str,cid1.s_size) == 0) {
            p->tokencount+=token->tokencount;
            delete token;
        }
        else
        {
            q->next = token;
            token->next=p;
            color_count++;
        }
    }
    else if (tid == dot)
    {
        if(tokenQ->next==NULL)
            tokenQ->next=token;
        else {
            tokenQ->next->tokencount += token->tokencount;
            delete token;
        }
    }
}

bool MultiSet::operator>=(MultiSet ms1) {
    Tokens *t1 = tokenQ->next,*t2=ms1.tokenQ->next;
    if(tid == productsort)
    {
        while(t2)
        {
            type_union *cid1,*cid2;
            int psnum = sorttable.productsort[sid].sortnum;
            vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid1 = new type_union[psnum];
            cid2 = new type_union[psnum];
            t2->color->getColor(cid2,psnum);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1,psnum);
                cmp = ps_cmp(cid1,cid2,psinfo);
                if(cmp>0)
                    return false;
                else if(cmp == 0)
                    break;
                t1=t1->next;
            }
            if(cmp == 0&& t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                    return false;
                t1 = t1->next;
            }
            else
                return false;

            delete[] cid1;
            delete[] cid2;
            t2=t2->next;
        }
    }
    else if(tid == Integer)
    {
        while(t2)
        {
            Integer_t cid1,cid2;
            t2->color->getColor(cid2);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1);
                if(cid1>cid2)
                    cmp = 1;
                else if(cid1 == cid2)
                    cmp = 0;
                else
                    cmp = -1;
                if(cmp>0)
                    return false;
                else if(cmp == 0)
                    break;
                t1=t1->next;
            }
            if(cmp == 0 && t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                    return false;
                t1 = t1->next;
            }
            else
                return false;

            t2=t2->next;
        }
    }
    else if(tid == Real)
    {
        while(t2)
        {
            Real_t cid1,cid2;
            t2->color->getColor(cid2);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1);
                if(cid1>cid2)
                    cmp = 1;
                else if(cid1 == cid2)
                    cmp = 0;
                else
                    cmp = -1;
                if(cmp>0)
                    return false;
                else if(cmp == 0)
                    break;
                t1=t1->next;
            }
            if(cmp == 0&& t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                    return false;
                t1 = t1->next;
            }
            else
                return false;

            t2=t2->next;
        }
    }
    else if(tid == String)
    {
        while(t2)
        {
            String_t cid1,cid2;
            t2->color->getColor(cid2);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1);
                cmp = strncmp(cid1.s_str,cid2.s_str,cid1.s_size);
                if(cmp>0)
                    return false;
                else if(cmp == 0)
                    break;
                t1=t1->next;
            }
            if(cmp == 0&& t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                    return false;
                t1 = t1->next;
            }
            else
                return false;

            t2=t2->next;
        }
    }
    else if(tid == dot)
    {
        if(t1 == NULL)
            return false;
        if(t1->tokencount < t2->tokencount)
            return false;
    }
    return true;
}

void MultiSet::clear() {
    color_count = 0;
    Tokens *token = tokenQ->next,*temp;
    tokenQ->next = NULL;
    while(token)
    {
        temp = token;
        token = token->next;
        delete temp;
    }
}

void CPN::Add_Place(string id, string Type_name, int size,bool control_P,string exp) {
    CPlace *pp = &place[placecount++];

    int psnum = 0;
    pp->control_P = control_P;
    pp->expression = exp;
    if(size>1) {
        id = id + arr_suffix;
    }
    pp->id = id;
    map<string, MSI>::iterator siter;
    siter = sorttable.mapSort.find(Type_name);
    pp->initMarking.sid = siter->second.sid;
    pp->initMarking.tid = siter->second.tid;

    if(pp->initMarking.tid == productsort)
        psnum = sorttable.productsort[pp->initMarking.sid].sortnum;

    Tokens *token;
    if(size >1)
    {
        for(int i=0;i<size;i++)
        {
            token = new Tokens;
            token->initiate(1,pp->initMarking.tid,psnum);
            if(pp->initMarking.tid == productsort) {
                type_union *cid;
                cid = new type_union[psnum];

                cid[psnum-1].integer = i;
                token->color->setColor(cid,psnum);
                pp->initMarking.insert(token);
                delete[] cid;
            }
            else {
                cout<<"error!array type must be product type!"<<endl;
                exit(-1);
            }
        }
    }
    else if(size == 1)
    {
        token = new Tokens;
        token->initiate(1,pp->initMarking.tid,psnum);

        if(pp->initMarking.tid == productsort) {
            type_union *cid;
            cid = new type_union[psnum];
            token->color->setColor(cid,psnum);
            pp->initMarking.insert(token);
            delete[] cid;
        }
        else {
            String_t str;
            str.s_str = NULL;
            str.s_size = 0;
            switch(pp->initMarking.tid)
            {
                case Integer:
                    token->color->setColor(int(0));
                    break;
                case Real:
                    token->color->setColor(double(0));
                    break;
                case String:
                    token->color->setColor(str);
                    break;
                case usersort:
                    cout<<"usersort doesn't match a type in PDNet!"<<endl;
                    exit(-1);
                default:
                    cout<<"error tid!"<<endl;
                    exit(-1);
            }
            pp->initMarking.insert(token);
        }
    }
    else
    {
        cout<<"invalid size!"<<endl;
        exit(-1);
    }
    mapPlace.insert(make_pair(id,placecount-1));
}

void CPN::init() {

    V_Table *table = new V_Table("global");
    v_tables.push_back(table);

    sorttable.psptr = 0;
    sorttable.usptr = 0;
    gen_P_num = gen_T_num = 0;
    placecount = transitioncount = arccount = varcount = 0;

    place = new CPlace[200];
    transition = new CTransition[200];
    arc = new CArc[1000];

    vartable = new Variable[100];

}

void CPN::initDecl() {

    map<string,type>::iterator miter;
    for(miter=map_build_in_type.begin();miter!=map_build_in_type.end();miter++)
    {
        //normal build_in type
        MSI m;
        m.tid = miter->second;
        m.sid = 0;
        sorttable.mapSort.insert(pair<string,MSI>(miter->first,m));

        //build_in type array
        ProductSort pp;
        pp.id = miter->first + arr_suffix;
        pp.sortnum = 2;
        pp.mytype = productsort;
        pp.sortname.push_back(miter->first);
        pp.sortname.push_back("int");

        pp.sortid.push_back(m);
        m.tid = Integer;
        pp.sortid.push_back(m);


        m.sid = SortTable::psptr++;
        m.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp.id,m));
        sorttable.productsort.push_back(pp);
    }
}

vector<pair<string,string>> process_struct_declaration_list(gtree *p)
{
    vector<pair<string,string>> sortname;
    gtree *struct_declaration_list = p;
    while(struct_declaration_list->type==STRUCT_DECLARATION_LIST)
        struct_declaration_list = struct_declaration_list->child;
    gtree *struct_declaration = struct_declaration_list;
    while(struct_declaration->type==STRUCT_DECLARATION)
    {
        gtree *specifier_qualifier_list = struct_declaration->child;
        gtree *struct_declarator_list = struct_declaration->child->next;
        string specifier = "";
        vector<string> declarators;

        while(specifier_qualifier_list != NULL)
        {
            if(specifier_qualifier_list->child->type == TYPE_SPECIFIER)
            {
                specifier = specifier_qualifier_list->child->child->place;
                break;
            }
            specifier_qualifier_list = specifier_qualifier_list->child->next;
        }

        if(specifier == "")
        {
            cout<<"error! declarator doesn't have a type!"<<endl;
            exit(-1);
        }

        while(struct_declarator_list->type == STRUCT_DECLARATOR_LIST)
            struct_declarator_list = struct_declarator_list->child;

        gtree *struct_declarator = struct_declarator_list;
        while(struct_declarator != NULL)
        {
            if(struct_declarator->child->type == DECLARATOR)
            {
                if(struct_declarator->child->child->type == POINTER)
                {
                    cout<<"error! pointer doesn't not support for now."<<endl;
                    exit(-1);
                } else
                {
                    declarators.push_back(struct_declarator->child->child->child->place);
                }
            }
            struct_declarator = struct_declarator->parent->next->next;
        }
        for(unsigned int i=0;i<declarators.size();i++)
            sortname.push_back(make_pair(specifier,declarators[i]));

        struct_declaration = struct_declaration->parent->next;
    }
    return sortname;
}

void CPN::getDecl(gtree *tree){
    if (tree == NULL)return;
    else if(tree->type == STRUCT_OR_UNION_SPECIFIER)
    {
        if(tree->child->next->type == REMAIN || (tree->child->next->next != NULL &&
                                                 tree->child->next->next->type == REMAIN))
        {
            ProductSort pp;
            gtree *struct_declare_list;
            if(tree->child->next->type == REMAIN)
            {
                pp.id = "";
                struct_declare_list = tree->child->next->next->child;
            }
            else {
                pp.id = tree->child->next->place;
                struct_declare_list = tree->child->next->next->next->child;
            }

            vector<pair<string,string>> sortname = process_struct_declaration_list(struct_declare_list);

            pp.mytype = productsort;
            SORTID sortcount = sortname.size();

            pp.sortnum = sortcount;
            for(SORTID i;i<sortcount;i++) {
                pp.sortname.push_back(sortname[i].first);
                pp.membername.push_back(sortname[i].second);
            }
            sorttable.productsort.push_back(pp);

            MSI m;
            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp.id,m));
        }
    }
    getDecl(tree->child);
    getDecl(tree->next);

}

CPN::CPN() {
    placecount = 0;
    transitioncount = 0;
    arccount = 0;
    varcount = 0;
}

CPN::~CPN() {
    delete [] place;
    delete [] transition;
    delete [] arc;
    delete [] vartable;
}

void CPN::init_Place(string id, Tokens *token) {
    map<string,index_t>::iterator iter = mapPlace.find(id);
    CPlace *pp = &place[iter->second];
    pp->initMarking.clear();
    pp->initMarking.insert(token);
}

int is_Operator(string s)
{
    if (s == "+" || s == "-" || s == "*" || s == "/"
    || s == "&" || s == "|" || s == "^" || s == "%")
        return 2;
    else if(s == "!" || s == "@")
        return 1;
    return 0;
}

Real_t operate(string s1,string s2,string Operator)
{
    Real_t v1,v2;
    v1 = atof(s1.c_str());
    if(s2!="")
        v2 = atof(s2.c_str());
    if(Operator == "+")
        return v1 + v2;
    else if(Operator == "-")
        return v1 - v2;
    else if(Operator == "*")
        return v1 * v2;
    else if(Operator == "/")
        return v1 / v2;
    else if(Operator == "&")
        return int(v1) & int(v2);
    else if(Operator == "|")
        return int(v1) | int(v2);
    else if(Operator == "^")
        return int(v1) ^ int(v2);
    else if(Operator == "%")
        return int(v1) % int(v2);
    else if(Operator == "!")
        return !(int(v1));
    else if(Operator == "@")
        return -v1;
}

bool operate_reloperator(string s1,string s2,string Operator)
{
    Real_t v1,v2;
    v1 = atof(s1.c_str());
    v2 = atof(s2.c_str());
    if(Operator == ">")
        return v1>v2;
    else if(Operator == "<")
        return v1<v2;
    else if(Operator == ">=")
        return v1>=v2;
    else if(Operator == "<=")
        return v1<=v2;
    else if(Operator == "==")
        return v1==v2;
    else if(Operator == "!=")
        return v1!=v2;
    else if(Operator == "&&")
        return v1&&v2;
    else if(Operator == "||")
        return v1||v2;
}

void CPN::CTN_cal(condition_tree_node *CTN) {
    //recursion stops when node_type is variable or color
    if(CTN->node_type == RelationOperator)
    {
        string value1,value2;
        CTN_cal(CTN->left);
        CTN_cal(CTN->right);
        if(CTN->left->node_type == variable)
        {
            string var1 = CTN->left->value;
            map<string,index_t>::iterator iter = mapVariable.find(var1);
            Variable *var  = &vartable[iter->second];
            if(var->tid == Integer)
                value1 = to_string(var->value.integer);
            else if(var->tid == Real)
                value1 = to_string(var->value.real);
        }
        else
            value1 = CTN->left->value;
        if(CTN->right->node_type == variable)
        {
            string var2 = CTN->left->value;
            map<string,index_t>::iterator iter = mapVariable.find(var2);
            Variable *var  = &vartable[iter->second];
            if(var->tid == Integer)
                value1 = to_string(var->value.integer);
            else if(var->tid == Real)
                value1 = to_string(var->value.real);
        }
        else
            value2 = CTN->right->value;
        bool res = operate_reloperator(value1,value2,CTN->node_name);
        if(res == true)
            CTN->value = "1";
        else
            CTN->value = "0";
    }
    else if(CTN->node_type == Operator)
    {
        //**Operator is used for Integer and Real
        int count = is_Operator(CTN->value);
        if(count == 2)
        {
            string value1,value2;
            CTN_cal(CTN->left);
            CTN_cal(CTN->right);
            if(CTN->left->node_type == variable)
            {
                string var1 = CTN->left->value;
                map<string,index_t>::iterator iter = mapVariable.find(var1);
                Variable *var  = &vartable[iter->second];
                if(var->tid == Integer)
                    value1 = to_string(var->value.integer);
                else if(var->tid == Real)
                    value1 = to_string(var->value.real);
            }
            else
                value1 = CTN->left->value;
            if(CTN->right->node_type == variable)
            {
                string var2 = CTN->left->value;
                map<string,index_t>::iterator iter = mapVariable.find(var2);
                Variable *var  = &vartable[iter->second];
                if(var->tid == Integer)
                    value1 = to_string(var->value.integer);
                else if(var->tid == Real)
                    value1 = to_string(var->value.real);
            }
            else
                value2 = CTN->right->value;
            Real_t res = operate(value1,value2,CTN->node_name);
            CTN->value = to_string(res);
        }
        else if(count == 1)
        {
            string value1;
            CTN_cal(CTN->left);
            if(CTN->left->node_type == variable)
            {
                string var1 = CTN->left->value;
                map<string,index_t>::iterator iter = mapVariable.find(var1);
                Variable *var  = &vartable[iter->second];
                if(var->tid == Integer)
                    value1 = to_string(var->value.integer);
                else if(var->tid == Real)
                    value1 = to_string(var->value.real);
            }
            else
                value1 = CTN->left->value;
            Real_t res = operate(value1,"",CTN->node_name);
            CTN->value = to_string(res);
        }
        else
        {
            cout<<"error in CTN value!"<<endl;
            exit(-1);
        }
    }
    else if(CTN->node_type == variable)
    {
        string var1 = CTN->value;
        map<string,index_t>::iterator iter = mapVariable.find(var1);
        Variable *var  = &vartable[iter->second];
        if(var->tid == Integer)
            CTN->value = to_string(var->value.integer);
        else if(var->tid == Real)
            CTN->value = to_string(var->value.real);
    }
    else if(CTN->node_type == color)
    {

    }
}

void CPN::CT2MS(condition_tree ct, MultiSet &ms) {
    CTN2MS(ct.root,ms);
}

void CPN::get_tuplecolor(condition_tree_node *ctn,vector<type_union*> &color,vector<mapsort_info> sortid) {
//    if(ctn->node_type == TokenOperator && ctn->node_name == ",")
//    {
    if (ctn->left->node_type == TokenOperator && ctn->left->node_name == ",") {
        get_tuplecolor(ctn->left, color,sortid);
    }
    else{
        type_union *u = new type_union;
        CTN_cal(ctn->left);
        if(color.size()>=sortid.size())
        {
            cout<<"exp's dimension is larger than multiset!"<<endl;
            exit(-1);
        }
        if(sortid[color.size()].tid == Integer)
            u->integer = atoi(ctn->left->value.c_str());
        else if(sortid[color.size()].tid == Real)
            u->real = atof(ctn->left->value.c_str());
        else if(sortid[color.size()].tid == String)
        {
            u->str.s_size = ctn->left->value.size();
            u->str.s_str = new char[u->str.s_size];
            strncpy(u->str.s_str,ctn->left->value.c_str(),u->str.s_size);
        }
        color.push_back(u);
    }
    if (ctn->right->node_type == TokenOperator && ctn->right->node_name == ",") {
        get_tuplecolor(ctn->right, color,sortid);
    }
    else{
        type_union *u = new type_union;
        CTN_cal(ctn->right);
        if(color.size()>=sortid.size())
        {
            cout<<"exp's dimension is larger than multiset!"<<endl;
            exit(-1);
        }
        if(sortid[color.size()].tid == Integer)
            u->integer = atoi(ctn->right->value.c_str());
        else if(sortid[color.size()].tid == Real)
            u->real = atof(ctn->right->value.c_str());
        else if(sortid[color.size()].tid == String)
        {
            u->str.s_size = ctn->right->value.size();
            u->str.s_str = new char[u->str.s_size];
            strncpy(u->str.s_str,ctn->right->value.c_str(),u->str.s_size);
        }
        color.push_back(u);
    }
//    }
}

void CPN::CTN2COLOR(condition_tree_node *ctn,MultiSet &ms)
{
    if(ctn->node_type == TokenOperator && ctn->node_name == ",")
    {
        vector<type_union*> color;
        get_tuplecolor(ctn,color,sorttable.productsort[ms.sid].sortid);
        Tokens *token = new Tokens;
        int psnum = color.size();
        type_union *cid = new type_union[color.size()];
        for(unsigned int i=0;i<color.size();i++)
        {
            memcpy(cid+i,color[i],sizeof(type_union));
        }
        token->initiate(1,productsort,psnum);
        token->color->setColor(cid,psnum);
        ms.insert(token);
    }
    else
    {
        CTN_cal(ctn);
        Tokens *token = new Tokens;
        token->initiate(1,ms.tid,0);
        if(ms.tid == Integer) {
            Integer_t cid = atoi(ctn->value.c_str());
            token->color->setColor(cid);
        }
        else if(ms.tid == Real)
        {
            Real_t cid = atof(ctn->value.c_str());
            token->color->setColor(cid);
        }
        else if(ms.tid == String)
        {
            String_t cid;
            cid.s_size = ctn->value.size();
            cid.s_str = new char[cid.s_size];
            strncpy(cid.s_str,ctn->value.c_str(),cid.s_size);
            token->color->setColor(cid);
            delete[] cid.s_str;
        }
        ms.insert(token);
    }
}

void CPN::CTN2MS(condition_tree_node *ctn, MultiSet &ms) {
    if(ctn->node_type==TokenOperator && ctn->node_name == "++")
    {
        if(ctn->left->node_type == TokenOperator && ctn->left->node_name == "++")
        {
            CTN2MS(ctn->left,ms);
        }
        else
        {
            CTN2COLOR(ctn->left,ms);
        }
        if(ctn->right->node_type == TokenOperator && ctn->right->node_name == "++")
        {
            CTN2MS(ctn->right,ms);
        }
        else
        {
            CTN2COLOR(ctn->right,ms);
        }
    }
    else
    {
        CTN2COLOR(ctn,ms);
    }
}