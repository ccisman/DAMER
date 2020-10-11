//
// Created by cc on 2020/10/10.
//

#include "cpn.h"
#include "v_table.h"
#include "base.h"
#include<cmath>

SortTable sorttable;
SORTID SortTable::psptr = 0;

vector<V_Table *> v_tables;//variable tables


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
string arr_suffix = "_arr",begin_suffix = " begin"
        ,end_suffix = " end",return_suffix = "_v",
        call_suffix = "()",para_suffix = "_para",
        call_statement_suffix = "_call",join_suffix = " join";
string executed_P_name = "executed_P";
vector<string> pthread_type = {"pthread_t","pthread_mutex_t","pthread_cond_t"};
vector<string> pthread_func_type = {"pthread_create","pthread_join","pthread_exit",
                                    "pthread_mutex_init","pthread_mutex_lock","pthread_mutex_unlock",
                                    "pthread_cond_init","pthread_cond_signal","pthread_cond_wait"};

extern int string_replace(std::string &s1, const std::string &s2, const std::string &s3);
vector<string> extract_paras(gtree *p);
void init_pthread_type()
{

    aka temp;
    for(unsigned int i=0;i<pthread_type.size();i++) {
        temp.origin_name = "";
        temp.aka_name = pthread_type[i];
        temp.level = 0;
        aka_type_array.push_back(temp);
    }
}
string gen_P()
{
    string temp = "";
    return temp + 'P' + to_string(gen_P_num++);

}
string gen_T()
{
    string temp = "";
    return temp + 'T' + to_string(gen_T_num++);
}
bool judge_bool(string s)
{
    bool ignore = false;
    for (unsigned int i = 0; i < s.length() - 1; i++)
    {
        if (s[i] == '(')
            ignore = true;
        else if (s[i] == ')')
            ignore = false;
        if (ignore == false && (s[i] == '>' || s[i] == '<'))
            return true;
        if (ignore == false && (s[i] == '=' || s[i] == '!'))
            if (s[i + 1] == '=')
                return true;
    }
    return false;
}
string opposite(string s)
{
    if (s.find(">=") != string::npos)
        string_replace(s, ">=", "<");
    else if (s.find("<=") != string::npos)
        string_replace(s, "<=", ">");
    else if (s.find("<") != string::npos)
        string_replace(s, "<", ">=");
    else if (s.find(">") != string::npos)
        string_replace(s, ">", "<=");
    else if (s.find("==") != string::npos)
        string_replace(s, "==", "!=");
    else if (s.find("!=") != string::npos)
        string_replace(s, "!=", "==");
    else//不含比较符
    {

    }
    if (s.find("&&") != string::npos)
        string_replace(s, "&&", "||");
    else if (s.find("||") != string::npos)
        string_replace(s, "||", "&&");

    return s;

}
string opposite_all(string s)
{
    if (!judge_bool(s))
        return "!(" + s + ")";
    vector<string> v;
    vector<string>v1;
    string temp_s;
    string result;
    for (unsigned int i = 0; i < s.size(); i++)
    {
        if (s[i] == '&')
        {
            v1.push_back("||");
            v.push_back(temp_s);
            temp_s.clear();
            i += 2;
        }
        else if (s[i] == '|')
        {
            v1.push_back("&&");
            v.push_back(temp_s);
            temp_s.clear();
            i += 2;
        }
        temp_s += s[i];
    }
    v.push_back(temp_s);
    for (unsigned int i = 0; i < v.size(); i++)
    {
        v[i] = opposite(v[i]);
    }
    result = v[0];
    for (unsigned int i = 0; i < v1.size(); i++)
    {
        result += v1[i] + v[i + 1];
    }
    return result;
}
void splitExpression(string &s, vector<string>& v)
{
    string s1;
    bool flag_shut = false;
    for (unsigned int i = 0; i < s.length(); i++)
    {
        if (s[i] == '(' || s[i] == ')')
            continue;
        else if (s[i] == '#')
            flag_shut = true;
        if (s[i] == '<' || s[i] == '>')
        {
            if (s[i + 1] == '=')
                i++;

            v.push_back(s1);
            s1.clear();
            flag_shut = false;
        }
        else if (s[i] == '=')
        {
            v.push_back(s1);
            s1.clear();
            flag_shut = false;
        }
        else if (s[i] == '|')
        {
            if (s[i + 1] == '|')
                i++;
            v.push_back(s1);
            s1.clear();
            flag_shut = false;
        }
        else if (s[i] == '&')
        {
            if (s[i + 1] == '&')
                i++;
            v.push_back(s1);
            s1.clear();
            flag_shut = false;
        }
        else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/')
        {
            v.push_back(s1);
            s1.clear();
            flag_shut = false;
        }
        else if (s[i] == '!' || s[i] == '=')
        {
            if (s[i + 1] == '=')
                i++;
            //else
            //cout << "�����쳣!=��==" << endl;
            v.push_back(s1);
            s1.clear();
            flag_shut = false;
        }
        else if (s[i] == '%')
        {
            v.push_back(s1);
            s1.clear();
            flag_shut = false;
        }
        else if (flag_shut == false)
            s1 = s1 + s[i];
    }

    v.push_back(s1);
}
string find_P_name(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->name == base)
        {
            return v_tables[i]->get_place(v_name);
        }
    }
    cout<<"can't find P name"<<endl;
    exit(-1);
}
void create_connect(CPN *petri, string T, string express, string base)
{
    string P2;
    vector<string> v;
    string V;
    bool sourceP;
    splitExpression(express, v);
    set<string> exist_V;
    for (unsigned int i = 0; i < v.size(); i++)
    {
        if (v[i][0] == '_' || (v[i][0] >= 'a'&&v[i][0] <= 'z') || (v[i][0] >= 'A'&&v[i][0] <= 'Z'))
        {
            int position = 0;
            for (unsigned int j = 0; j < v[i].length(); j++)
            {
                if (v[i][j] == '[')
                {
                    position = j;
                    break;
                }
            }
            if (position != 0)
                v[i] = v[i].substr(0, position);
            if(v[i].length()>return_suffix.length() &&
            v[i].substr(v[i].length()-return_suffix.length()) == return_suffix)
                continue;

            P2 = find_P_name(v[i],base);

            V = v[i];

            auto viter = exist_V.find(V);
            if(viter!=exist_V.end())
                continue;
            auto iter = petri->mapTransition.find(T);
            if(iter == petri->mapTransition.end())
            {
                cout<<"create_connect can't find T"<<endl;
                exit(-1);
            }
            petri->transition[iter->second].relvars.insert(V);
            exist_V.insert(V);
            petri->Add_Variable(V,P2);

            petri->Add_Arc(P2,T,V,true,data);
            petri->Add_Arc(T,P2,V,false,data);
//            sourceP = true;
//            petri.Add_Arc(P2, T, V, sourceP);//_v库所
//            sourceP = false;
//            //			V = "";
//            petri.Add_Arc(T, P2, V, sourceP);

        }
    }
}
void iter_extra(CPN *petri,string newP,string _P){
    vector<string> false_exit = petri->get_falseexit_T(_P);
    for (unsigned int i = 0; i < false_exit.size(); i++)
        petri->Add_Arc(false_exit[i], newP, "", false, executed);
}
void inside_block(CPN *petri, gtree *tree1, string T)//tree1 indicates a compound_statement node
{
    gtree *tr;
    int sum = 0;
    //bool flag = false;
    if(tree1->child->type == COMPOUND_STATEMENT)
        tree1 = tree1->child;
    if (tree1->child->next->type == STATEMENT_LIST)
    {
        tr = tree1->child->next;
    }
    else if(tree1->child->next->next == NULL)
    {
        return;
    }
    else if (tree1->child->next->next->type == STATEMENT_LIST)
    {
        tr = tree1->child->next->next;
    }
    else//只有定义
    {
        cout << "only have declaration!" << endl;
        return;
        //exit(-1);
    }
    while (tr->type != STATEMENT)
        tr = tr->child;

    vector<string> now;
    vector<string> last;
    bool mutex_flag=false;
    string mutex_P;
    string mutex_T;
//    string create_P;
    map<string,string> create_P_list;

    //init
    last.push_back(T);
    while (tr) {
        now.clear();
        /*if (tr->child->type == SELECTION_STATEMENT || tr->child->type == ITERATION_STATEMENT
            || judge_assign_statement(tr) || judge_call_statement(tr) || judge_return_statement(tr))*/
        if (!judge_statement(tr)) {
            tr = tr->parent->next;
            continue;
        }

        bool control_P, t;
        int n1 = 0;
        double d = 0.0;
        string tag;
        string _P = tr->matched_P;
        vector<string> call_P = petri->get_call_P(_P);

        if (tr->place == "pthread_mutex_unlock" + return_suffix) {
            mutex_flag = false;
            vector<string> cor_P;
//            cor_P.push_back(_P);
//            petri->set_correspond_P(mutex_P, cor_P);
//            cor_P.clear();
//            cor_P.push_back(mutex_P);
//            petri->set_correspond_P(_P, cor_P);
        } else if (tr->place == "pthread_join" + return_suffix) {
            vector<string> paras = extract_paras(tr->child->child->child->child);
            auto iter = create_P_list.find(paras[0]);
            if (iter == create_P_list.end()) {
                cout << "can't find matched pthread_create" << endl;
                exit(-1);
            }
            vector<string> cor_P;
            cor_P.push_back(_P);
            petri->set_correspond_P(iter->second, cor_P);
        }

        if (call_P.size() != 0) {
            //call_P[0]代表调用到的第一个函数
//            for (unsigned int i = 0; i < call_P.size(); i++) {
                if (mutex_flag == false)
                    petri->Add_Arc(T, call_P[0], "", false, control);
                else
                    petri->Add_Arc(mutex_T, call_P[0], "", false, control);
//            }
        } else {
            if (mutex_flag == false)
                petri->Add_Arc(T, _P, "", false, control);
            else
                petri->Add_Arc(mutex_T, _P, "", false, control);
        }
        //int flag = petri.get_call_flag(_P);


        if (call_P.size() == 0)
            now = petri->get_enter_T(_P);
        else
            now = petri->get_enter_T(call_P[0]);


//        if (sum == 0) {
//            sum++;
            string newP;

//            petri->Add_Place(newP, "", 0, true, executed_P_name);
//            if (mutex_flag == false)
//                petri->Add_Arc(T, newP, "", false, executed);
//            else
//                petri->Add_Arc(mutex_T, newP, "", false, executed);
//            for (unsigned int i = 0; i < now.size(); i++)
//                petri->Add_Arc(newP, now[i], "", true, control);//it's under consideration

            newP = petri->Add_executed_P(last,now);
            if (tr->child->type == ITERATION_STATEMENT)//while statement
                iter_extra(petri,newP,_P);
//        }
//        else {
//            string newP = gen_P();
//
//            petri->Add_Place(newP, "", 0, true, executed_P_name);
//
//            for (unsigned int i = 0; i < last.size(); i++)
//                petri->Add_Arc(last[i], newP, "", false, executed);
//
//            for (unsigned int i = 0; i < now.size(); i++)
//                petri->Add_Arc(newP, now[i], "", true, control);//it's under consideration
//            if (tr->child->type == ITERATION_STATEMENT)//while statement
//            {
//                vector<string> false_exit = petri->get_falseexit_T(_P);
//                for (unsigned int i = 0; i < false_exit.size(); i++)
//                    petri->Add_Arc(false_exit[i], newP, "", false, executed);
//            }
//        }


        last = petri->get_exit_T(_P);

        if (tr->place == "pthread_mutex_lock" + return_suffix) {
            mutex_flag = true;
            mutex_P = tr->matched_P;
            vector<string> lock_T = petri->get_enter_T(mutex_P);
            mutex_T = lock_T[0];
        } else if (tr->place == "pthread_create" + return_suffix) {
            //simplely process
            vector<string> paras = extract_paras(tr->child->child->child->child);
            create_P_list.insert(make_pair(paras[0].substr(1), tr->matched_P));
        }

        if (tr->parent->next->type == STATEMENT)
            tr = tr->parent->next;
        else
            break;

    }
}
vector<string> get_statement_exit(gtree *statement1, CPN *petri)
{
    //statement1->child is compound
    string statement_P;
    vector<string> temp_v1;
    gtree *last_statement = statement1;
    if (statement1->child->type == COMPOUND_STATEMENT)
    {
        if (statement1->child->child->next->type == STATEMENT_LIST)//only have sentence, no definition
        {
            if (statement1->child->child->next->child->type == STATEMENT)
            {
                last_statement = statement1->child->child->next->child;
                //last_statement = get_real_laststatement(last_statement);
            }
            else
            {
                last_statement = statement1->child->child->next->child->next;
                //last_statement = get_real_laststatement(last_statement);
                //temp_v1 = petri.get_exit(statement_P);
            }
        }
        else if(statement1->child->child->next->next == NULL)
        {
            return temp_v1;
        }
        else if (statement1->child->child->next->type == DECLARATION_LIST && statement1->child->child->next->next->type == STATEMENT_LIST)//有语句有定义
        {
            if (statement1->child->child->next->next->child->type == STATEMENT)
            {
                last_statement = statement1->child->child->next->next->child;
                //last_statement = get_real_laststatement(last_statement);
                //temp_v1 = petri.get_exit(statement_P);
            }
            else
            {
                last_statement = statement1->child->child->next->next->child->next;
                //last_statement = get_real_laststatement(last_statement);
                //temp_v1 = petri.get_exit(statement_P);
            }
        }
        else//只有定义
        {
            cout << "only have definition!" << endl;
            return temp_v1;
//            exit(-1);
        }
        if (last_statement != NULL)
        {
            statement_P = last_statement->matched_P;
            temp_v1 = petri->get_exit_T(statement_P);
        }
        else
        {
            cout<<"error in get_statement_exit!"<<endl;
            exit(-1);
        }
    }
    else if (statement1->child->type == ITERATION_STATEMENT || statement1->child->type == SELECTION_STATEMENT
             || statement1->child->type == JUMP_STATEMENT || statement1->child->type == LABELED_STATEMENT)//各类语句
    {
        while(last_statement->type!=STATEMENT)
            last_statement = last_statement->parent;
        statement_P = last_statement->matched_P;
        temp_v1 = petri->get_exit_T(statement_P);
    }
    else if (judge_assign_statement(statement1))
    {
        while(last_statement->type!=STATEMENT)
            last_statement = last_statement->parent;
        statement_P = last_statement->matched_P;
        temp_v1 = petri->get_exit_T(statement_P);
    }
    return temp_v1;
}

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
    else if(sort == dot)
        color = NULL;
    else
    {
        cout<<"error type!"<<endl;
//        exit(-1);
    }
}

int ps_cmp(SortValue **ps1,SortValue **ps2,vector<MSI> psinfo)
{
    for(unsigned int i=0;i<psinfo.size();i++)
    {
        if(psinfo[i].tid == Integer)
        {
            Integer_t cid1,cid2;
            ps1[i]->getColor(cid1);
            ps2[i]->getColor(cid2);
            if(cid1 < cid2)
               return -1;
            else if(cid1 == cid2)
                continue;
            else
                return 1;
        }
        else if(psinfo[i].tid == Real)
        {
            Real_t cid1,cid2;
            ps1[i]->getColor(cid1);
            ps2[i]->getColor(cid2);
            if(cid1 < cid2)
                return -1;
            else if(cid1 == cid2)
                continue;
            else
                return 1;
        }
        else if(psinfo[i].tid == String)
        {
            String_t cid1,cid2;
            ps1[i]->getColor(cid1);
            ps2[i]->getColor(cid2);
            if(cid1 < cid2)
                return -1;
            else if(cid1 == cid2)
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
        SortValue **cid1,**cid2;
        int psnum = sorttable.productsort[sid].sortnum;
        vector<MSI> psinfo = sorttable.productsort[sid].sortid;
        cid1 = new SortValue*[psnum];
        cid2 = new SortValue*[psnum];
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
        if(cmp == 0&& p!=NULL) {
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
        if(cid1 == cid2 && p!=NULL) {
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
        if(cid1 == cid2&& p!=NULL) {
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
            if(cid1>=cid2)
                break;
            q=p;
            p=p->next;
        }
        if(cid1 == cid2&& p!=NULL) {
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

bool MultiSet::operator>=(const MultiSet &ms1) {
    Tokens *t1 = tokenQ->next,*t2=ms1.tokenQ->next;
    if(tid == productsort)
    {
        while(t2)
        {
            SortValue **cid1,**cid2;
            int psnum = sorttable.productsort[sid].sortnum;
            vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid1 = new SortValue*[psnum];
            cid2 = new SortValue*[psnum];
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
    else if(tid == dot)
    {
        if(t1 == NULL)
            return false;
        else if(t2 == NULL)
            return true;
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

void MultiSet::MINUS(MultiSet &ms) {
    Tokens *t1 = tokenQ->next,*t2=ms.tokenQ->next,*last = tokenQ;
    if(tid == productsort)
    {
        while(t2)
        {
            SortValue **cid1,**cid2;
            int psnum = sorttable.productsort[sid].sortnum;
            vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid1 = new SortValue*[psnum];
            cid2 = new SortValue*[psnum];
            t2->color->getColor(cid2,psnum);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1,psnum);
                cmp = ps_cmp(cid1,cid2,psinfo);
                if(cmp>0) {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else if(cmp == 0)
                    break;
                last = t1;
                t1=t1->next;
            }
            if(cmp == 0&& t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else {
                    t1->tokencount -= t2->tokencount;
                    if(t1->tokencount == 0) {
                        last->next = t1->next;
                        delete t1;
                    }
                }
                t1 = t1->next;
            }
            else
            {
                cout<<"MINUS error!"<<endl;
                exit(-1);
            }

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
                {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else if(cmp == 0)
                    break;
                last = t1;
                t1=t1->next;
            }
            if(cmp == 0 && t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else {
                    t1->tokencount -= t2->tokencount;
                    if(t1->tokencount == 0)
                    {
                        last->next = t1->next;
                        delete t1;
                    }
                }
                t1 = t1->next;
            }
            else
            {
                cout<<"MINUS error!"<<endl;
                exit(-1);
            }

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
                else if(abs(cid1-cid2)<1e-5)
                    cmp = 0;
                else
                    cmp = -1;
                if(cmp>0)
                {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else if(cmp == 0)
                    break;
                last = t1;
                t1=t1->next;
            }
            if(cmp == 0&& t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else {
                    t1->tokencount -= t2->tokencount;
                    if(t1->tokencount == 0)
                    {
                        last->next = t1->next;
                        delete t1;
                    }
                }
                t1 = t1->next;
            }
            else
            {
                cout<<"MINUS error!"<<endl;
                exit(-1);
            }

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
                if(cid1>cid2)
                    cmp = 1;
                else if(cid1 == cid2)
                    cmp = 0;
                else
                    cmp = -1;
                if(cmp>0)
                {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else if(cmp == 0)
                    break;
                last = t1;
                t1=t1->next;
            }
            if(cmp == 0&& t1!=NULL) {
                if(t1->tokencount < t2->tokencount)
                {
                    cout<<"MINUS error!"<<endl;
                    exit(-1);
                }
                else {
                    t1->tokencount -= t2->tokencount;
                    if(t1->tokencount == 0)
                    {
                        last->next = t1->next;
                        delete t1;
                    }
                }
                t1 = t1->next;
            }
            else
            {
                cout<<"MINUS error!"<<endl;
                exit(-1);
            }

            t2=t2->next;
        }
    }
    else if(tid == dot)
    {
        if(t1 == NULL)
        {
            cout<<"MINUS error!"<<endl;
            exit(-1);
        }
        if(t1->tokencount < t2->tokencount)
        {
            cout<<"MINUS error!"<<endl;
            exit(-1);
        }
        else {
            t1->tokencount -= t2->tokencount;
            if(t1->tokencount == 0) {
                last = tokenQ->next;
                tokenQ->next = NULL;
                delete last;
            }
        }
    }
    else
    {
        cout<<"error tid in MINUS"<<endl;
        exit(-1);
    }
//    Tokens *temp;
//    t2 = ms.tokenQ->next;
//    while(t2)
//    {
//        temp = t2;
//        t2 = t2->next;
//        delete temp;
//    }
//    ms.tokenQ->next = NULL;
}

void MultiSet::PLUS(MultiSet &ms) {

    Tokens *t1 = tokenQ->next,*t2=ms.tokenQ->next;
    while(t2)
    {
        insert(t2);
        t2=t2->next;
    }
    ms.tokenQ->next = NULL;
}

void MultiSet::operator=(const MultiSet &ms) {
    sid = ms.sid;
    tid = ms.tid;
    color_count = ms.color_count;
    hash_value = ms.hash_value;
    Tokens *t1=tokenQ,*t2 = ms.tokenQ->next;
    int psnum = 0;
    if(ms.tid == productsort)
        psnum = sorttable.productsort[ms.sid].sortnum;
    while(t2)
    {
        t1->next = new Tokens;
        t1->next->initiate(t2->tokencount,ms.tid,psnum);
        if(ms.tid == productsort)
        {
            SortValue **cid1,**cid2;
            t2->color->getColor(cid1,psnum);
            t1->next->color->setColor(cid1,psnum);
        }
        else if(ms.tid == Integer)
        {
            Integer_t cid1,cid2;
            t2->color->getColor(cid1);
            t1->next->color->setColor(cid1);
        }
        else if(ms.tid == Real)
        {
            Real_t cid1,cid2;
            t2->color->getColor(cid1);
            t1->next->color->setColor(cid1);
        }
        else if(ms.tid == String)
        {
            String_t cid1,cid2;
            t2->color->getColor(cid1);
            t1->next->color->setColor(cid1);
        }

        t2=t2->next;
    }
    if(t1->next)
        t1->next->next = NULL;
}

index_t MultiSet::Hash() {
    index_t hv = 0;
    if(tid == productsort)
    {
        hv = color_count*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p;
        int sortnum = sorttable.productsort[sid].sortnum;
        SortValue **cid = new SortValue*[sortnum];
        for(p=tokenQ->next;p!=NULL;p=p->next)
        {
            p->color->getColor(cid,sortnum);
            hv += p->tokencount*H1FACTOR*H1FACTOR;
            for(int j=0;j<sortnum;++j)
            {
                if (sorttable.productsort[sid].sortid[j].tid == Integer) {
                    Integer_t color;
                    cid[j]->getColor(color);
                    hv += (color + 1) * H1FACTOR;
                }
                else if (sorttable.productsort[sid].sortid[j].tid == Real) {
                    Real_t color;
                    cid[j]->getColor(color);
                    hv += (int(color)%sizeof(Integer_t) + 1) * H1FACTOR * H1FACTOR;
                }
                else if (sorttable.productsort[sid].sortid[j].tid == String){
                    String_t color;
                    cid[j]->getColor(color);
                    hv += ( 1) * H1FACTOR;//string's hash isn't sure
                }
            }
        }
        delete [] cid;
    }
    else if(tid == dot)
    {
        hv = color_count*H1FACTOR*H1FACTOR*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p = tokenQ->next;
        if(p!=NULL)
            hv += p->tokencount*H1FACTOR*H1FACTOR;
    }
    else if(tid == Integer)
    {
        hv = color_count*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p = tokenQ->next;

        for(p;p!=NULL;p=p->next)
        {

            Integer_t cid;
            p->color->getColor(cid);
            hv += p->tokencount*H1FACTOR*H1FACTOR+(cid+1)*H1FACTOR;
        }
    }
    else if(tid == Real)
    {
        hv = color_count*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p = tokenQ->next;
        for(p;p!=NULL;p=p->next)
        {
            Real_t cid;
            p->color->getColor(cid);
            hv += p->tokencount*H1FACTOR*H1FACTOR+(int(cid)%sizeof(Integer_t)+1)*H1FACTOR;
        }
    }
    else if(tid == String)
    {
        hv = color_count*H1FACTOR*H1FACTOR*H1FACTOR;
        Tokens *p = tokenQ->next;
        for(p;p!=NULL;p=p->next)
        {
            String_t cid;
            p->color->getColor(cid);
            hv += p->tokencount*H1FACTOR*H1FACTOR+(1)*H1FACTOR;
        }
    }
    hash_value = hv;
    return hv;
}

bool MultiSet::operator==(const MultiSet &ms) {
    Tokens *t1 = tokenQ->next,*t2=ms.tokenQ->next;
    if(tid == productsort)
    {
        while(t2)
        {
            SortValue **cid1,**cid2;
            int psnum = sorttable.productsort[sid].sortnum;
            vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid1 = new SortValue*[psnum];
            cid2 = new SortValue*[psnum];
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
                if(t1->tokencount != t2->tokencount)
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
                if(t1->tokencount != t2->tokencount)
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
                if(t1->tokencount != t2->tokencount)
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
                if(t1->tokencount != t2->tokencount)
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

        if(t1 == NULL && t2 == NULL)
            return true;
        else if(t1 == NULL || t2 == NULL)
            return false;
        if(t1->tokencount != t2->tokencount)
            return false;
    }
    return true;
}

void CPN::Add_Place(string id, string Type_name, int size,bool control_P,string exp) {
    CPlace *pp = &place[placecount++];

    int psnum = 0;
    pp->control_P = control_P;//pthread_mutex should be redifine
    pp->expression = exp;
    pp->id = id;
    pp->is_cond = false;
    pp->is_mutex = false;


    if(exp == executed_P_name)
        pp->is_executed = true;
    else if(exp.length()>4 && exp.substr(exp.length()-end_suffix.length()) == end_suffix)
        pp->is_executed = true;
    else
        pp->is_executed = false;

    //dot type
    if(Type_name == "") {
        Tokens *token = new Tokens;

        pp->initMarking.tid = dot;
        pp->initMarking.sid = 0;
        mapPlace.insert(make_pair(id,placecount-1));
        return ;
    }

    //pthread type
    if(exist_in(pthread_type,Type_name)) {

        Tokens *token = new Tokens;
        pp->control_P = true;
        pp->initMarking.tid = dot;
        pp->initMarking.sid = 0;
        if(Type_name == "pthread_mutex_t")
            pp->is_mutex = true;
        else if(Type_name == "pthread_cond_t")
            pp->is_cond = true;

        mapPlace.insert(make_pair(id, placecount - 1));
        return;
    }

    if(size>1) {
        Type_name = Type_name + arr_suffix;
    }


    auto siter = sorttable.mapSort.find(Type_name);
    if(siter == sorttable.mapSort.end())
    {
        cout<<"can't find Type_name"<<endl;
        exit(-1);
    }
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
                SortValue **cid;
                cid = new SortValue*[psnum];

                cid[psnum-1]->setColor(Integer_t(i));
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
            SortValue **cid;
            cid = new SortValue*[psnum];
            token->color->setColor(cid,psnum);
            pp->initMarking.insert(token);
            delete[] cid;
        }
        else {

            switch(pp->initMarking.tid)
            {
                case Integer:
                    token->color->setColor(int(0));
                    break;
                case Real:
                    token->color->setColor(double(0));
                    break;
                case String:
                    token->color->setColor("");
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

void CPN::Add_Transition(string id, string guard, string exp) {
    CTransition *tt = &transition[transitioncount++];
    tt->id = id;
    if(guard != "") {
        tt->guard.construct(guard);
        tt->hasguard = true;
    }
    else
        tt->hasguard = false;
    mapTransition.insert(make_pair(id,transitioncount-1));
}

void CPN::Add_Arc(string source, string target, string exp, bool sourceP,Arc_Type arcType) {
    CArc *aa = &arc[arccount++];
    aa->arcType = arcType;
    if(exp != "") {
        //aa->arc_exp.deconstruct();
        aa->arc_exp.construct(exp);
        aa->onlydot = false;
    }
    else
        aa->onlydot = true;
    aa->isp2t = sourceP;
    aa->source_id = source;
    aa->target_id = target;
}

void CPN::Add_Arc_override(string source, string target, string exp, bool sourceP,Arc_Type arcType) {
    CArc *aa;
    for(int i=arccount-1;i>=0;i--)
    {
        aa = &arc[i];
        if(aa->source_id == source && aa->target_id == target)
        {
            aa->arcType = arcType;
            if(exp != "") {
                aa->arc_exp.deconstruct();
                aa->arc_exp.construct(exp);
                aa->onlydot = false;
            }
            else
                aa->onlydot = true;
            aa->isp2t = sourceP;
            aa->source_id = source;
            aa->target_id = target;
            return;
        }
    }
    Add_Arc(source,target,exp,sourceP,arcType);


}

void CPN::Add_Variable(string id, string related_P) {
    auto iter = mapVariable.find(id);
    if(iter==mapVariable.end())
    {
        Variable *var = &vartable[varcount++];
        var->id = id;
        auto iter1 = mapPlace.find(related_P);
        var->sid = place[iter1->second].initMarking.sid;
        var->tid = place[iter1->second].initMarking.tid;
        int psnum = 0;
        if(var->tid == productsort) {
            psnum = sorttable.productsort[var->sid].sortnum;
            var->value = new ProductSortValue(psnum);
        }
        else if(var->tid == Integer)
            var->value = new IntegerSortValue;
        else if(var->tid == Real)
            var->value = new RealSortValue;
        else if(var->tid == String)
            var->value = new StringSortValue;
        mapVariable.insert(make_pair(id,varcount-1));
    }
}

void CPN::init() {


    sorttable.psptr = 0;
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
    if(iter == mapPlace.end())
    {
        cout<<"can't find id in init_Place."<<endl;
        exit(-1);
    }
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
        return v1 && v2;
    else if(Operator == "||")
        return v1 || v2;

}

void CPN::CTN_cal(condition_tree_node *CTN) {
    //recursion stops when node_type is variable or color
    if(CTN == NULL)
        return;
    if(CTN->node_type == RelationOperator)
    {
        string value1,value2;
        CTN_cal(CTN->left);
        CTN_cal(CTN->right);
        if(CTN->left->node_type == variable)
        {
            string var1 = CTN->left->node_name;
            auto iter = mapVariable.find(var1);
            if(iter == mapVariable.end())
            {
                cout<<"can't find var in CTN_cal"<<endl;
                exit(-1);
            }
            Variable *var  = &vartable[iter->second];
            if(var->tid == Integer) {
                Integer_t v;
                var->value->getColor(v);
                value1 = to_string(v);
            }
            else if(var->tid == Real) {
                Real_t v;
                var->value->getColor(v);
                value1 = to_string(v);
            }
        }
        else
            value1 = CTN->left->value;
        if(CTN->right->node_type == variable)
        {
            string var2 = CTN->right->node_name;
            auto iter = mapVariable.find(var2);
            if(iter == mapVariable.end())
            {
                cout<<"can't find var in CTN_cal"<<endl;
                exit(-1);
            }
            Variable *var  = &vartable[iter->second];
            if(var->tid == Integer) {
                Integer_t v;
                var->value->getColor(v);
                value1 = to_string(v);
            }
            else if(var->tid == Real) {
                Real_t v;
                var->value->getColor(v);
                value1 = to_string(v);
            }
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
        int count = is_Operator(CTN->node_name);
        if(count == 2)
        {
            string value1,value2;
            CTN_cal(CTN->left);
            CTN_cal(CTN->right);
            if(CTN->left->node_type == variable)
            {
                string var1 = CTN->left->node_name;
                auto iter = mapVariable.find(var1);
                if(iter == mapVariable.end())
                {
                    cout<<"can't find var in CTN_cal"<<endl;
                    exit(-1);
                }
                Variable *var  = &vartable[iter->second];
                if(var->tid == Integer) {
                    Integer_t v;
                    var->value->getColor(v);
                    value1 = to_string(v);
                }
                else if(var->tid == Real) {
                    Real_t v;
                    var->value->getColor(v);
                    value1 = to_string(v);
                }
            }
            else
                value1 = CTN->left->value;
            if(CTN->right->node_type == variable)
            {
                string var2 = CTN->right->node_name;
                auto iter = mapVariable.find(var2);
                if(iter == mapVariable.end())
                {
                    cout<<"can't find var in CTN_cal"<<endl;
                    exit(-1);
                }
                Variable *var  = &vartable[iter->second];
                if(var->tid == Integer) {
                    Integer_t v;
                    var->value->getColor(v);
                    value2 = to_string(v);
                }
                else if(var->tid == Real) {
                    Real_t v;
                    var->value->getColor(v);
                    value2 = to_string(v);
                }
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
                string var1 = CTN->left->node_name;
                auto iter = mapVariable.find(var1);
                if(iter == mapVariable.end())
                {
                    cout<<"can't find var in CTN_cal"<<endl;
                    exit(-1);
                }
                Variable *var  = &vartable[iter->second];
                if(var->tid == Integer) {
                    Integer_t v;
                    var->value->getColor(v);
                    value1 = to_string(v);
                }
                else if(var->tid == Real) {
                    Real_t v;
                    var->value->getColor(v);
                    value1 = to_string(v);
                }
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
        string var1 = CTN->node_name;
        auto iter = mapVariable.find(var1);
        if(iter == mapVariable.end())
        {
            cout<<"can't find var in CTN_cal"<<endl;
            exit(-1);
        }
        Variable *var  = &vartable[iter->second];
        if(var->tid == Integer) {
            Integer_t v;
            var->value->getColor(v);
            CTN->value = to_string(v);
        }
        else if(var->tid == Real) {
            Real_t v;
            var->value->getColor(v);
            CTN->value = to_string(v);
        }
    }
    else if(CTN->node_type == color)
    {

    }
}

void CPN::CT2MS(condition_tree ct, MultiSet &ms) {
    if(ct.root == NULL)
    {
        Tokens *token = new Tokens;
        token->initiate(1,dot,0);
        ms.insert(token);
    }
    CTN2MS(ct.root,ms);
}

void CPN::get_tuplecolor(condition_tree_node *ctn,vector<SortValue*> &color,vector<mapsort_info> sortid) {
//    if(ctn->node_type == TokenOperator && ctn->node_name == ",")
//    {
    if (ctn->left->node_type == TokenOperator && ctn->left->node_name == ",") {
        get_tuplecolor(ctn->left, color,sortid);
    }
    else{
        SortValue *u;
        CTN_cal(ctn->left);
        if(color.size()>=sortid.size())
        {
            cout<<"exp's dimension is larger than multiset!"<<endl;
            exit(-1);
        }
        if(sortid[color.size()].tid == Integer) {
            u = new IntegerSortValue;
            u->setColor(atoi(ctn->left->value.c_str()));
        }
        else if(sortid[color.size()].tid == Real) {
            u = new RealSortValue;
            u->setColor(atof(ctn->left->value.c_str()));
        }
        else if(sortid[color.size()].tid == String) {
            u = new StringSortValue;
            u->setColor(ctn->left->value);
        }
        color.push_back(u);
    }
    if (ctn->right->node_type == TokenOperator && ctn->right->node_name == ",") {
        get_tuplecolor(ctn->right, color,sortid);
    }
    else{
        SortValue *u;
        CTN_cal(ctn->right);
        if(color.size()>=sortid.size())
        {
            cout<<"exp's dimension is larger than multiset!"<<endl;
            exit(-1);
        }
        if(sortid[color.size()].tid == Integer) {
            u = new IntegerSortValue;
            u->setColor(atoi(ctn->right->value.c_str()));
        }
        else if(sortid[color.size()].tid == Real) {
            u = new RealSortValue;
            u->setColor(atof(ctn->right->value.c_str()));
        }
        else if(sortid[color.size()].tid == String) {
            u = new StringSortValue;
            u->setColor(ctn->right->value);
        }
        color.push_back(u);
    }
//    }
}

void CPN::CTN2COLOR(condition_tree_node *ctn,MultiSet &ms)
{
    if(ctn->node_type == TokenOperator && ctn->node_name == ",")
    {
        vector<SortValue *> color;
        get_tuplecolor(ctn,color,sorttable.productsort[ms.sid].sortid);
        Tokens *token = new Tokens;
        int psnum = color.size();
        SortValue **cid = new SortValue*[color.size()];
        for(unsigned int i=0;i<color.size();i++)
        {
            cid[i]=color[i];
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
            String_t cid = ctn->value;
            token->color->setColor(cid);
        }
        ms.insert(token);
    }
}

void CPN::CTN2MS(condition_tree_node *ctn, MultiSet &ms) {
    if(ctn == NULL)
        return;
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

void CPN::process_declarator(gtree *declarator, string tag, string base, bool para)
{
    bool call_declare_flag = false;//true when declarator is function declaration
    bool init_flag = false;//true when has init
    string init_value;
    gtree *identifier;
    string exp;
    bool ispoint = false;
    if (declarator->child->next == NULL)
        identifier = declarator->child;
    else
        identifier = declarator->child->next;
    if(identifier == NULL)
        exp = declarator->place;
    else {
        exp = identifier->place;
    }
    if (declarator->next == NULL || declarator->next->type != REMAIN || declarator->next->place != "=")//无初始化
        init_flag = false;
    else//有初始化,暂不支持数组初始化
    {
        init_flag = true;
        gtree *init = declarator->next->next;
        string temp_s;
        if (init->child->type == ASSIGNMENT_EXPRESSION)
            temp_s = init->child->place;
        else
        {
            cout << "暂不支持数组初始化!" << endl;
            exit(-1);
        }
        condition_tree temp_CT;
        temp_CT.construct(temp_s);
        CTN_cal(temp_CT.root);
        init_value = temp_CT.root->value;
    }
    if (declarator->child->type == POINTER)
        ispoint = true;
    else
        ispoint = false;
    bool array_flag = false;
    int array_size = 0;
    if (identifier->parent->next != NULL && identifier->parent->next->type == REMAIN && identifier->parent->next->place == "[")//数组
    {
        array_flag = true;
        if (identifier->parent->next->next->type == CONSTANT_EXPRESSION && para == false)
            array_size = atoi(identifier->parent->next->next->child->place.c_str());
        else if(identifier->parent->next->next->type == CONSTANT_EXPRESSION && para == true)
            array_size = 1;
        else
        {
            cout << "暂不支持数组大小不指定！" << endl;
            exit(-1);
        }
    }
    else if (identifier->parent->next != NULL && identifier->parent->next->type == REMAIN && identifier->parent->next->place == "(")//函数声明
        call_declare_flag = true;
    else//非数组非函数声明
    {
        array_size = 1;
    }
    if (!call_declare_flag)
    {
        if(tag != "pthread_t") {
            //Add variable place
            if (array_flag == true && para == true)
                ispoint = true;

            string _P = gen_P();
            Add_Place(_P, tag, array_size, false, exp);

            if (init_flag == true) {
                Tokens *token = new Tokens;
                auto iter = map_build_in_type.find(tag);
                if(iter == map_build_in_type.end())
                {
                    cout<<"can't find tag in map_build_in_type"<<endl;
                    exit(-1);
                }
                if (iter->second == Integer) {
                    token->init_a_token(iter->second, atoi(init_value.c_str()));
                } else if (iter->second == Real) {
                    token->init_a_token(iter->second, atof(init_value.c_str()));
                } else if (iter->second == String) {
                    token->init_a_token(iter->second, init_value);
                }
                init_Place(_P, token);
            }


            //it is for v_tables
            gtree *compound = declarator;
            if (declarator->parent->type == PARAMETER_DECLARATION) {
                while (compound->parent->type != FUNCTION_DEFINITION)
                    compound = compound->parent;
                while (compound->type != COMPOUND_STATEMENT)
                    compound = compound->next;
                compound->para.push_back(make_pair(identifier->place, _P));
            } else {
                string table_name;
                while (compound != NULL && compound->type != COMPOUND_STATEMENT)
                    compound = compound->parent;
                if (compound == NULL)
                    table_name = "global";
                else
                    table_name = compound->place;
                for (unsigned int i = 0; i < v_tables.size(); i++) {
                    if (table_name == v_tables[i]->name) {
                        v_tables[i]->insert(identifier->place, _P);
                        break;
                    }
                }
            }
        }
    }

}

void CPN::process_declaration(gtree *declaration,string base) {

    gtree *init_declarator = declaration->child->next;

    string tag = declaration->child->place;
    while (init_declarator->type != INIT_DECLARATOR)
        init_declarator = init_declarator->child;

    process_declarator(init_declarator->child, tag, base, false);
    gtree *temp;
    while (init_declarator->parent->next != NULL && init_declarator->parent->next->type == REMAIN &&
           init_declarator->parent->next->place == ",") {
        init_declarator = init_declarator->parent;
        temp = init_declarator->next->next;
        process_declarator(temp->child, tag, base, false);
    }

}

void CPN::process_para_type_list(gtree *para_type_list, string base_Vname)
{
    gtree *para_list;
    if (para_type_list->child->next != NULL)//ELLIPSIS暂不考虑
    {
        cout << "ELLIPSIS暂不考虑！" << endl;
        exit(-1);
    }
    else
        para_list = para_type_list->child;

    gtree *para_declaration = para_list->child;
    while (para_declaration->type != PARAMETER_DECLARATION)
        para_declaration = para_declaration->child;

    string tag = para_declaration->child->place;
    if (para_declaration->child->next == NULL || para_declaration->child->next->type == ABSTRACT_DECLARATOR)
    {
        if (para_declaration->child->place == "void")
            return;
        cout << "abstract_declarator doesn't consider!" << endl;
        exit(-1);
    }
    process_declarator(para_declaration->child->next,  tag, base_Vname, true);

    gtree *temp;
    while (para_declaration->parent->next != NULL && para_declaration->parent->next->type == REMAIN && para_declaration->parent->next->place == ",")
    {
        para_declaration = para_declaration->parent;
        temp = para_declaration->next->next;
        tag = temp->child->place;
        if (temp->child->next == NULL || temp->child->next->type == ABSTRACT_DECLARATOR)
        {
            if (temp->child->place == "void")
                return;
            cout << "abstract_declarator doesn't consider!" << endl;
            exit(-1);
        }
        process_declarator(temp->child->next, tag, base_Vname, true);
    }
}

void CPN::set_enter_T(string p_name, vector<string> enter_T) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        place[iter->second].enter = enter_T;
    else
    {
        cout<<"error in set_enter_T"<<endl;
        exit(-1);
    }
}

void CPN::set_exit_T(string p_name, vector<string> exit_T) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        place[iter->second].exit = exit_T;
    else
    {
        cout<<"error in set_exit_T"<<endl;
        exit(-1);
    }
}

void CPN::set_falseexit_T(string p_name, vector<string> false_exit_T) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        place[iter->second].false_exit = false_exit_T;
    else
    {
        cout<<"error in set_falseexit_T"<<endl;
        exit(-1);
    }
}

void CPN::set_call_P(string p_name, vector<string> call_P) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        place[iter->second].call_P = call_P;
    else
    {
        cout<<"error in set_call_P"<<endl;
        exit(-1);
    }
}

void CPN::set_correspond_P(string p_name, vector<string> correspond_P) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        place[iter->second].correspond_P = correspond_P;
    else
    {
        cout<<"error in set_correspond_P"<<endl;
        exit(-1);
    }
}

void CPN::Add_returns(string p_name, string return_T, string exp) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        place[iter->second].returns.push_back(make_pair(return_T,exp));
    else
    {
        cout<<"error in Add_returns"<<endl;
        exit(-1);
    }
}

vector<string> CPN::get_enter_T(string p_name) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        return place[iter->second].enter;
    else
    {
        cout<<"error in get_enter_T"<<endl;
        exit(-1);
    }
}

vector<string> CPN::get_exit_T(string p_name) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        return place[iter->second].exit;
    else
    {
        cout<<"error in get_exit_T"<<endl;
        exit(-1);
    }
}

vector<string> CPN::get_falseexit_T(string p_name) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        return place[iter->second].false_exit;
    else
    {
        cout<<"error in get_falseexit_T"<<endl;
        exit(-1);
    }
}

vector<string> CPN::get_call_P(string p_name) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        return place[iter->second].call_P;
    else
    {
        cout<<"error in get_call_P"<<endl;
        exit(-1);
    }
}

vector<string> CPN::get_correspond_P(string p_name) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        return place[iter->second].correspond_P;
    else
    {
        cout<<"error in get_correspond_P"<<endl;
        exit(-1);
    }
}

vector<pair<string, string>> CPN::get_returns(string p_name) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        return place[iter->second].returns;
    else
    {
        cout<<"error in get_returns"<<endl;
        exit(-1);
    }
}

vector<string> extract_paras(gtree *p)
{
    //p must be a postfix_expression
    vector<string> res;
    if(p->type!=POSTFIX_EXPRESSION || p->child->type != POSTFIX_EXPRESSION || p->child->next->place!="(")
    {
        cout<<"extract_paras's para must be a call_postfix!"<<endl;
        exit(-1);
    }
    gtree *argument_list = p->child->next->next,*assignment;
    if(argument_list->place == ")")
        return res;
    while(argument_list->type!=ASSIGNMENT_EXPRESSION)
        argument_list = argument_list->child;
    assignment = argument_list;
    while(assignment && assignment->type==ASSIGNMENT_EXPRESSION)
    {
        res.push_back(assignment->place);
        assignment = assignment->parent->next->next;
    }
}

void CPN::create_v_table(gtree *p){
    V_Table *table = new V_Table(p->place);
    v_tables.push_back(table);
    gtree *up = p->parent;

    while (up != NULL && up->type != COMPOUND_STATEMENT)
        up = up->parent;
    if(p->para.size()!=0) {
        for (unsigned int i = 0; i < p->para.size(); i++)
            table->insert(p->para[i].first, p->para[i].second);
        string identifier;
        gtree *func = p->parent;
        if (func->child->next->child->type == POINTER)
            identifier = func->child->next->child->next->child->child->place;
        else
            identifier = func->child->next->child->child->child->place;
        auto iter = mapFunction.find(identifier + begin_suffix);
        if(iter == mapFunction.end())
        {
            cout<<"can't find begin_Place in create_PDNet"<<endl;
            exit(-1);
        }
        string func_P = iter->second;
        auto iter1 = mapPlace.find(func_P);
        if(iter1 == mapPlace.end())
        {
            cout<<"can't find func_P in create_PDNet"<<endl;
            exit(-1);
        }
        for(unsigned int i=0;i<p->para.size();i++)
            place[iter1->second].para_list.push_back(p->para[i]);
    }
    if (up == NULL)
    {
        v_tables[v_tables.size()-1]->connect(v_tables[0]);
    }
    else
    {
        for(int i=0;i<v_tables.size();i++)
            if (v_tables[i]->name == up->place)
            {
                v_tables[v_tables.size()-1]->connect(v_tables[i]);
                break;
            }
    }
}

void CPN::visit_declaration(gtree *p)
{
    gtree *p1 = p->parent;
    string func = "";
    while (p1 != NULL && p1->type != COMPOUND_STATEMENT)
        p1 = p1->parent;
    if (p1 != NULL)
        func = p1->place;
    process_declaration(p,func);
}

void CPN::visit_statement(gtree *p)
{

    if(p->child->type == SELECTION_STATEMENT || p->child->type == ITERATION_STATEMENT)
    {
        //construct  P、T
        string control_P = gen_P();
        gtree *statement = p;
//        while(statement->type != STATEMENT)
//            statement = statement->parent;
        statement->matched_P = control_P;
        Add_Place(control_P,"",0,true,p->place);
        string control_T1 = gen_T(),control_T2 = gen_T();
        string condition = p->child->child->next->next->place;
        Add_Transition(control_T1,condition,condition);
        Add_Arc(control_P,control_T1,"",true,control);
        string condition_op = opposite_all(condition);
        Add_Transition(control_T2,condition_op,condition_op);
        Add_Arc(control_P,control_T2,"",true,control);

        //set enter,enter_P
        vector<string> enter,enter_P;
        enter.push_back(control_T1);
        enter.push_back(control_T2);
        enter_P.push_back(control_P);
        set_enter_T(control_P,enter);
    }
    else
    {
        //construct  P、T
        string control_P = gen_P();
        gtree *statement = p;
//        while(statement->type != STATEMENT)
//            statement = statement->parent;
        statement->matched_P = control_P;
        Add_Place(control_P,"",0,true,p->place);
        string control_T = gen_T();
        Add_Transition(control_T,"",p->place);
        Add_Arc(control_P,control_T,"",true,control);

        //set exit,enter
        vector<string> enter,exit_T;
        enter.push_back(control_T);
        exit_T.push_back(control_T);
        //enter_P.push_back(control_P);
        set_enter_T(control_P,enter);
        set_exit_T(control_P,exit_T);

    }
}

void CPN::visit_function(gtree *p){
    string ret_tag;
    if (p->child->type == DECLARATION_SPECIFIERS)
        ret_tag = p->child->place;
    else
    {
        cout << "暂不支持定义函数未定义返回类型!" << endl;
        exit(-1);
    }
    string func;
    bool return_pointer_flag = false;
    if (p->child->next->child->type == POINTER) {
        func = p->child->next->child->next->child->child->place;
        return_pointer_flag = true;
        ret_tag = "int";//unsolve pointer
    }
    else
        func = p->child->next->child->child->child->place;

    //construct begin_P,end_P,begin_T

    string begin_P = gen_P();
    p->matched_P = begin_P;
    Add_Place(begin_P,"",0,true,func + begin_suffix);
    mapFunction.insert(make_pair(func + begin_suffix,begin_P));
    string begin_T = gen_T();
    Add_Transition(begin_T,"",func + begin_suffix);
    vector<string> enter;
    enter.push_back(begin_T);
    set_enter_T(begin_P,enter);

    Add_Arc(begin_P,begin_T,"",true,control);

    string end_P = gen_P();
    Add_Place(end_P,"",0,true,func+ end_suffix);
    mapFunction.insert(make_pair(func + end_suffix,end_P));
//    vector<string> correspond_P;
//    correspond_P.push_back(end_P);
//    set_correspond_P(begin_P,correspond_P);

    //set ret_tag
//    if(ret_tag!="void")
        mapFunction.insert(make_pair(begin_P,ret_tag));

    //construct parameter_P

    if (p->child->child->type == POINTER || p->child->next->child->type == POINTER)
    {
        //do not support pointer now
    }
    else
    {
        gtree *direct_declarator = p->child->next->child;
        if (direct_declarator->child->next->type == REMAIN && direct_declarator->child->next->place == "(")
        {
            if (direct_declarator->child->next->next->type == PARAMETER_TYPE_LIST)
            {
                gtree *para_type_list = direct_declarator->child->next->next;

                process_para_type_list(para_type_list,func);
            }
            else//no para
            {

            }
        }
        else
        {
            cout << "there is no '('!" << endl;
            exit(-1);
        }
    }

    //construct return_P
//    if(ret_tag != "void" || return_pointer_flag == true)
//    {
//        string return_P = gen_P();
//        Add_Place(return_P,ret_tag,1,false,func + return_suffix);
//        mapFunction.insert(make_pair(func + return_suffix,return_P));
//        string V = func + return_suffix;
//        Add_Variable(V,return_P);
//        v_tables[0]->insert(V,return_P);
//        //to be continue
//    }
}

void handle_unary_expression(CPN *cpn,gtree *p,string T,string base,string exp){
    string left = p->place;
    string left_P = find_P_name(left,base);
    cpn->Add_Arc_override(T,left_P,exp,false,write);
    cpn->Add_Arc_override(left_P,T,left,true,write);
    string V = left;
    cpn->Add_Variable(left,left_P);
    auto iter = cpn->mapTransition.find(T);
    if(iter == cpn->mapTransition.end())
    {
        cout<<"can't find control_T in create_PDNet"<<endl;
        exit(-1);
    }
    cpn->transition[iter->second].relvars.insert(V);
}

void handle_conditional_expression(CPN *cpn,gtree *p,string T,string base){
    if(p->type != CONDITIONAL_EXPRESSION)
        create_connect(cpn, T, p->parent->place ,base);
    else
        create_connect(cpn, T, p->place ,base);
}

void handle_assignment_expression(CPN *cpn,gtree *p,string T){
//    if(p->child->type == CONDITIONAL_EXPRESSION)
    if(0)
        ;
    else
    {

        gtree *unary_expression = p->child,*conditional_expression,*com = p;
        while(com->type!=COMPOUND_STATEMENT)
            com = com->parent;
        string base = com->place;
        while(unary_expression->type == UNARY_EXPRESSION)
//        {
//            handle_unary_expression(cpn,unary_expression,T);
            unary_expression = unary_expression->next->next->child;
//        }
        conditional_expression = unary_expression;
        string exp = conditional_expression->parent->place;
        handle_conditional_expression(cpn,conditional_expression,T,base);
        unary_expression = p->child;
        while(unary_expression->type == UNARY_EXPRESSION)
        {
            handle_unary_expression(cpn,unary_expression,T,base,exp);
            unary_expression = unary_expression->next->next->child;
        }
    }
}

void CPN::handle_expression(gtree *p) {
    gtree *expression=p,*assignment_expression,*statement=p;
    while(statement->type != STATEMENT)
        statement = statement->parent;
    string statement_P = statement->matched_P;
    vector<string> statement_T = get_enter_T(statement_P);
//    while(expression->type == EXPRESSION)
//        expression = expression->child;
    if(expression->child->type == ASSIGNMENT_EXPRESSION)
        assignment_expression = expression->child;
    else
        assignment_expression = expression->child->next->next;
//    while(assignment_expression && assignment_expression->parent->next) {
        if(statement_T.size()==0){cout<<"error in handle_expression!"<<endl;exit(-1);}
        else if(statement_T.size()==1)
            handle_assignment_expression(this,assignment_expression,statement_T[0]);
        else if(statement_T.size()==2){
            handle_assignment_expression(this,assignment_expression,statement_T[0]);
            handle_assignment_expression(this,assignment_expression,statement_T[1]);
        }
//        assignment_expression = assignment_expression->parent->next->next;
//    }
}

void CPN::handle_call(gtree *p) {
    if(exist_in(pthread_func_type,p->child->place))
    {
        // get statement_P and paras
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;

        vector<string> paras;
        paras = extract_paras(p);
        if(p->child->place == "pthread_create") {
            string thread_v = paras[0], thread_func = paras[2];
            if (thread_v[0] == '&')
                thread_v = thread_v.substr(1);

            auto iter = mapFunction.find(thread_func + begin_suffix);
            if(iter == mapFunction.end())
            {
                cout<<"can't find thread_begin_P"<<endl;
                exit(-1);
            }
            string func_begin_P = iter->second;
            auto iter1 = mapFunction.find(thread_func + end_suffix);
            if(iter1 == mapFunction.end())
            {
                cout<<"can't find thread_end_P"<<endl;
                exit(-1);
            }
            string func_end_P = iter1->second;

            //add thread map
            mapPthread.insert(make_pair(thread_v + begin_suffix, func_begin_P));
            mapPthread.insert(make_pair(thread_v + end_suffix, func_end_P));


            //add thread arc
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0], func_begin_P, "", false, control);
        }
        else if(p->child->place == "pthread_join")
        {
            string thread_v = paras[0];
            if (thread_v[0] == '&')
                thread_v = thread_v.substr(1);

            auto iter = mapPthread.find(thread_v + end_suffix);
            if(iter == mapPthread.end())
            {
                cout<<"can't find pthread_end in mapPthread"<<endl;
                exit(-1);
            }
            string func_end_P = iter->second;

            //add map
            auto biter = mapPthread.find(thread_v + begin_suffix);
            if(biter == mapPthread.end())
            {
                cout<<"can't find pthread_begin when pthread_join"<<endl;
                exit(-1);
            }
            mapJoin.insert(make_pair(biter->second,statement_P));

            //add thread arc
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(func_end_P,enter_T[0], "", true, control);
        }
        else if(p->child->place == "pthread_exit")
        {
            gtree *find_func = p;

            while (find_func->type != FUNCTION_DEFINITION)
                find_func = find_func->parent;
            string identifier = find_func->place;

            auto iter1 = mapFunction.find(identifier + end_suffix);
            if(iter1 == mapFunction.end())
            {
                cout<<"can't find end_P in create_CPN"<<endl;
                exit(-1);
            }
            string last_func_end = iter1->second;
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0], last_func_end, "", false,executed);
        }
        else if(p->child->place == "pthread_mutex_init")
        {
            string mutex_v = paras[0];
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_name(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,"",false,control);
        }
        else if(p->child->place == "pthread_mutex_lock")
        {
            string mutex_v = paras[0];
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_name(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(mutex_P,enter_T[0],"",true,control);

            gtree *tr = statement->parent->next;

        }
        else if(p->child->place == "pthread_mutex_unlock")
        {
            string mutex_v = paras[0];
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_name(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,"",false,control);
        }
        else if(p->child->place == "pthread_cond_signal")
        {
            string cond_v = paras[0];
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string cond_P = find_P_name(cond_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],cond_P,"",false,control);
        }
        else if(p->child->place == "pthread_cond_wait")
        {
            string cond_v = paras[0],mutex_v = paras[1];
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            if(mutex_v[0] == '&')
                mutex_v = mutex_v.substr(1);
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string cond_P = find_P_name(cond_v,base);
            string mutex_P = find_P_name(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);


            //construct P、T
            string P1,P2,P3,T1,T2;
            P1 = gen_P();
            P2 = gen_P();
            P3 = gen_P();
            T1 = gen_T();
            T2 = gen_T();

            Add_Place(P1,"",0,true,"signalP1");
            Add_Place(P2,"",0,true,"signalP2");
            Add_Place(P3,"",0,true,"signalP3");
            Add_Transition(T1,"","signalT1");
            Add_Transition(T2,"","signalT2");

            //construct arcs
            Add_Arc(enter_T[0],P1,"",false,control);
            Add_Arc(enter_T[0],P3,"",false,executed);
            Add_Arc(enter_T[0],mutex_P,"",false,control);
            Add_Arc(cond_P,T1,"",true,control);
            Add_Arc(P1,T1,"",true,control);
            Add_Arc(T1,P2,"",false,control);
            Add_Arc(mutex_P,T2,"",true,control);
            Add_Arc(P2,T2,"",true,control);
            Add_Arc(P3,T2,"",true,control);

            //set exit
            vector<string> exit_T;
            exit_T.push_back(T2);
            set_exit_T(statement_P,exit_T);

//            vector<string> cor_P;
//            cor_P.push_back(P1);
//            cor_P.push_back(P2);
//            cor_P.push_back(P3);
//            set_correspond_P(statement_P,cor_P);
        }
    }
    else {
        gtree *com = p;
        while (com->type != COMPOUND_STATEMENT)
            com = com->parent;
        string base = com->place;
        string call_func_id = p->child->place, call_func_P_begin, call_func_P_end;

        //construct call structure
        string call_P = gen_P();
        string call_T = gen_T();
        p->matched_P = call_P;
        Add_Place(call_P, "", 0, true, p->child->place + call_suffix);
        Add_Transition(call_T, "", p->child->place + call_suffix);
        Add_Arc(call_P, call_T, "", true, control);

        vector<string> enter;
        enter.push_back(call_T);
        set_enter_T(call_P, enter);


        // set call_P
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;
        vector<string> statement_call_P = get_call_P(statement_P);
        statement_call_P.push_back(call_P);
        set_call_P(statement_P, statement_call_P);

        vector<string> statement_correspond_P = get_correspond_P(statement_P);
        statement_correspond_P.push_back(call_P);
        set_correspond_P(statement_P,statement_correspond_P);
        vector<string> cor_P;
        cor_P.push_back(statement_P);
        set_correspond_P(call_P,cor_P);

        auto iter_begin = mapFunction.find(call_func_id + begin_suffix);
        if(iter_begin == mapFunction.end())
        {
            cout<<"can't find call_begin_P in create_PDNet"<<endl;
            exit(-1);
        }
        call_func_P_begin = iter_begin->second;

        //passing parameter
        gtree *temp_tree = p->child->next->next;
        if (temp_tree->type == ARGUMENT_EXPRESSION_LIST) {
            vector<string> v;
            gtree *temp_assignment_expression = temp_tree;
            while (temp_assignment_expression->type != ASSIGNMENT_EXPRESSION)
                temp_assignment_expression = temp_assignment_expression->child;

            while (1) {
                string value = temp_assignment_expression->place;
                v.push_back(value);
                if (temp_assignment_expression->parent->next->next != NULL
                    && temp_assignment_expression->parent->next->next->type == ASSIGNMENT_EXPRESSION)
                    temp_assignment_expression = temp_assignment_expression->parent->next->next;
                else
                    break;
            }

            auto iter1 = mapPlace.find(call_func_P_begin);
            if(iter1 == mapPlace.end())
            {
                cout<<"can't find call_func_P_begin"<<endl;
                exit(-1);
            }
            CPlace *begin_place = &place[iter1->second];
            for (unsigned int i = 0; i < begin_place->para_list.size(); i++) {
                string para = begin_place->para_list[i].first;

                para += para_suffix;
                Add_Variable(para, begin_place->para_list[i].second);

                Add_Arc(call_T, begin_place->para_list[i].second, v[i], false, write);
                Add_Arc(begin_place->para_list[i].second, call_T, para, true, write);
                create_connect(this, call_T, v[i], base);
            }
        }
        //construct return places
        auto iter = mapFunction.find(call_func_P_begin);
        string ret_tag = iter->second;
        if(ret_tag!="void") {
            string return_P = gen_P();
            string return_v = p->child->place + return_suffix;

            Add_Place(return_P, ret_tag, 1, false, return_v);
            vector<pair<string, string>> returns = get_returns(call_func_P_begin);
            for (unsigned int i = 0; i < returns.size(); i++) {
                Add_Arc(returns[i].first, return_P, returns[i].second, false, write);
                Add_Arc(return_P, returns[i].first, return_v, true, write);
            }


            string last_P;
            vector<string> last_T;
            gtree *last_call = p->parent;
            while (!judge_call_postfix_expression(last_call) && last_call->type != STATEMENT) {
                last_P = last_call->matched_P;
                last_T = get_enter_T(last_P);
                for (unsigned int j = 0; j < last_T.size(); j++) {
                    Add_Arc(last_T[j], return_P, return_v, false, data);
                    Add_Arc(return_P, last_T[j], return_v, true, data);
                    Add_Variable(return_v, return_P);

                }
                last_call = last_call->parent;
            }
        }

        //construct enter (not return) arcs
        vector<string> enter_T = get_enter_T(statement_P);
        string called_identifier = p->child->place;
//        auto iter_begin = mapFunction.find(called_identifier + begin_suffix);
//        if(iter_begin == mapFunction.end())
//        {
//            cout<<"can't find call_begin_P when construct enter arcs"<<endl;
//            exit(-1);
//        }
//        string called_begin_P = iter_begin->second;
        Add_Arc(call_T, call_func_P_begin, "", false, call_enter);
//        auto iter1 = mapFunction.find(called_identifier + end_suffix);
//        if(iter1 == mapFunction.end())
//        {
//            cout<<"can't find call_end_P when construct enter arcs"<<endl;
//            exit(-1);
//        }
//        string called_end_P = iter1->second;

//        for (unsigned int i = 0; i < enter_T.size(); i++)
//            Add_Arc(called_end_P, enter_T[i], "", true, control);
//
//        //construct executed_control arc
////        string newP = gen_P();
////        Add_Place(newP,"",0,true,executed_P_name);
//        Add_Arc(call_T, statement_P, "", false, call_connect);
////        for(unsigned int i=0;i<enter_T.size();i++)
////            Add_Arc(newP,enter_T[i],"",true,control);
    }
}

string CPN::Add_executed_P(vector<string> source_T,vector<string> target_T){

    string executed_P = gen_P();
    Add_Place(executed_P,"",0,true,executed_P_name);
    for (unsigned int i = 0; i < source_T.size(); i++)
        Add_Arc(source_T[i], executed_P, "", false, executed);
    for (unsigned int i = 0; i < target_T.size(); i++)
        Add_Arc(executed_P, target_T[i], "", true, control);
    return executed_P;
}

void CPN::handle_iter_sel(gtree *p) {

    gtree *statement = p->parent;
    string control_P = statement->matched_P;
    string control_T1 = get_enter_T(control_P)[0];
    string control_T2 = get_enter_T(control_P)[1];

    //set exit
    vector<string> exit_T, falseexit;

    vector<string> temp1, temp2;
    gtree *statement1 = p->child->next->next->next->next;
    gtree *statement2 = NULL;
    temp1 = get_statement_exit(statement1, this);
    if (p->child->next->next->next->next->next) {
        statement2 = p->child->next->next->next->next->next->next;
        temp2 = get_statement_exit(statement2, this);
    }
    else
        temp2.push_back(control_T2);
    if (p->type == SELECTION_STATEMENT) {
        if(temp1.size()!=0)
            exit_T.insert(exit_T.end(),temp1.begin(),temp1.end());
        else
            exit_T.push_back(control_T1);
        if(temp2.size()!=0)
            exit_T.insert(exit_T.end(),temp2.begin(),temp2.end());
        else
            exit_T.push_back(control_T2);
    }
    else
    {
        if(temp1.size()!=0)
            falseexit.insert(exit_T.end(),temp1.begin(),temp1.end());
        else
            falseexit.push_back(control_T1);
        exit_T.push_back(control_T2);
    }
    set_exit_T(control_P,exit_T);
    if(falseexit.size()!=0)
        set_falseexit_T(control_P,falseexit);

    //connect with child statement
    string child_P1,child_P2;

    child_P1 = statement1->matched_P;
    Add_Arc(control_T1,child_P1,"",false,control);
    vector<string> temp;
    temp.push_back(control_T1);
    Add_executed_P(temp,get_enter_T(child_P1));
    if(statement2) {
        child_P2 = statement2->matched_P;
        Add_Arc(control_T2,child_P2,"",false,control);
        temp.clear();
        temp.push_back(control_T2);
        Add_executed_P(temp,get_enter_T(child_P2));
    }

    //iter must supply executed flow
    if(p->type == ITERATION_STATEMENT)
    {
        //false_exit
        for(unsigned int i=0;i<falseexit.size();i++)
        {
            Add_Arc(falseexit[i],control_P,"",false,executed);
            string enter_P = statement->enter_P;//just consider call one time now
            if(enter_P != "")
                Add_Arc(falseexit[i],enter_P,"",false,executed);
        }
    }
}

void CPN::supply_func(gtree *p) {
    string identifier, func;
    identifier = p->place;

    func = identifier + begin_suffix;
    auto iter = mapFunction.find(func);
    if(iter == mapFunction.end())
    {
        cout<<"can't find func in create_PDNet"<<endl;
        exit(-1);
    }
    string begin_P = iter->second;
    vector<string> enter = get_enter_T(begin_P);
    string begin_T = enter[0];

    gtree *compound_statement = p->child;
    while (compound_statement->type != COMPOUND_STATEMENT)
        compound_statement = compound_statement->next;
//    inside_block(this, compound_statement, begin_T);


    vector<string> v;
    auto iter1 = mapFunction.find(identifier + begin_suffix);
    if(iter1 == mapFunction.end())
    {
        cout<<"can't find begin_Place in create_PDNet"<<endl;
        exit(-1);
    }
    string begin_place = iter1->second;
    //v = petri.get_exit(begin_place);
    //这里实现隐式return
    //找第一层语句列中最后一句语句，并将其出口加入函数出口

    gtree *statement_list = compound_statement->child;
    while (statement_list != NULL && statement_list->type != STATEMENT_LIST)
        statement_list = statement_list->next;
    if (statement_list == NULL) {

        string identifier = p->place;
        auto iter1 = mapFunction.find(identifier + begin_suffix);
        vector<string> enter_T = get_enter_T(iter1->second);
        auto iter2 = mapFunction.find(identifier + end_suffix);

        Add_Arc(enter_T[0], iter2->second, "", false, executed);
        return;
    }
    gtree *statement = statement_list;
    while (statement->type != STATEMENT)
        statement = statement->child;

    bool already_return = false;

    while (statement->parent != statement_list)
    {
        if (judge_return(statement))
        {
            already_return = true;
            break;
        }
        statement = statement->parent->next;
    }
    if (already_return == false)
    {
        if (judge_return(statement))
            already_return = true;
        else
        {
            vector<string> v1 = get_exit_T(statement->matched_P);
            v.insert(v.end(), v1.begin(), v1.end());
        }
    }

    if (already_return == false)
    {
        auto iter1 = mapFunction.find(identifier + end_suffix);
        if(iter1 == mapFunction.end())
        {
            cout<<"can't find end_P in create_PDNet"<<endl;
            exit(-1);
        }
        string func_end = iter1->second;
        string func_v = "";
        if (p->child->type == DECLARATION_SPECIFIERS && p->child->child->type == TYPE_SPECIFIER && p->child->child->place == "void")
            ;
        else//no return
            ;


        for (unsigned int i = 0; i < v.size(); i++)
        {

            Add_Arc_override(v[i], func_end, "", false,call_exit);
//                if (func_v != "")
//                {
//                    petri.Add_Arc(v[i], func_v, "0", false);
//                    petri.Add_Arc(func_v, v[i], "write", true);//_v库所弧全部为写弧
//                }
        }

    }
}

void CPN::supply_jump_statement(gtree *p){
    if(p->child->child->type == RETURN) {
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string control_P = statement->matched_P;
        string control_T = get_enter_T(control_P)[0];

        gtree *find_func = p;

        while (find_func->type != FUNCTION_DEFINITION)
            find_func = find_func->parent;

        string identifier = find_func->place;

        string expression;
        if (p->child->child->next->type == EXPRESSION)
            expression = p->child->child->next->place;
        else
            expression = "";



        auto iter_end = mapFunction.find(identifier + end_suffix);
        if (iter_end == mapFunction.end()) {
            cout << "can't find end_P in create_CPN" << endl;
            exit(-1);
        }
        string last_func_end = iter_end->second;
        Add_Arc(control_T, last_func_end, "", false, call_exit);

        auto iter_begin = mapFunction.find(identifier + begin_suffix);
        if (iter_begin == mapFunction.end()) {
            cout << "can't find end_P in create_CPN" << endl;
            exit(-1);
        }
        string last_func_begin = iter_begin->second;
        Add_returns(last_func_begin,control_T,expression);

//    if (expression != "") {
//
//        auto iter2 = mapFunction.find(identifier + return_suffix);
//        if(iter2 == mapFunction.end())
//        {
//            cout<<"can't find return_P in create_PDNet"<<endl;
//            exit(-1);
//        }
//        string last_func_v = iter2->second;
//        Add_Arc(control_T, last_func_v, expression, false, write);
//        Add_Arc(last_func_v, control_T, identifier + return_suffix, true, write);
//        gtree *com = p;
//        while (com->type != COMPOUND_STATEMENT)
//            com = com->parent;
//        string base = com->place;
//        create_connect(this, control_T, expression, base);
//        auto iter = mapTransition.find(control_T);
//        if(iter == mapTransition.end())
//        {
//            cout<<"can't find control_T in create_PDNet"<<endl;
//            exit(-1);
//        }
//        transition[iter->second].relvars.insert(identifier + return_suffix);
//
//    }
    }
}

void CPN::supply_compound(gtree *p) {
    string statement_P = p->parent->matched_P;
    string statement_T = get_enter_T(statement_P)[0];//compound_statement just have one enter_T

    inside_block(this,p,statement_T);

    //set exit
    vector<string> exit_T;
    exit_T = get_statement_exit(p->parent, this);
    if(exit_T.size()!=0)
        set_exit_T(statement_P,exit_T);
}

void organize_call(CPN *cpn,gtree *p){
    string statement_P = p->matched_P;
    vector<string> statement_T = cpn->get_enter_T(statement_P);
    vector<string> call_P = cpn->get_call_P(statement_P);
    if(call_P.size()==0)
        return;
    unsigned int i=0;
    string last_P,next_P,last_T,next_T;//call just has one control_T
    string last_end_P;
    for(;i<call_P.size()-1;i++)
    {
        last_P = call_P[i];
        next_P = call_P[i+1];
        last_T = cpn->get_enter_T(last_P)[0];
        next_T = cpn->get_enter_T(next_P)[0];
        cpn->Add_Arc(last_T,next_P,"",false,call_connect);

        auto iter = cpn->mapPlace.find(last_P);
        string call_exp = cpn->place[iter->second].expression;
        string_replace(call_exp,call_suffix,end_suffix);
        string end_exp = call_exp;
        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = iter_end->second;
        cpn->Add_Arc(last_end_P,next_T,"",true,control);
    }
    last_P = call_P[i];
    next_P = statement_P;
    last_T = cpn->get_enter_T(last_P)[0];
    cpn->Add_Arc(last_T,next_P,"",false,call_connect);
    for(unsigned int j=0;j<statement_T.size();j++)
    {
        next_T = statement_T[j];
        auto iter = cpn->mapPlace.find(last_P);
        string call_exp = cpn->place[iter->second].expression;
        string_replace(call_exp,call_suffix,end_suffix);
        string end_exp = call_exp;
        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = iter_end->second;
        cpn->Add_Arc(last_end_P,next_T,"",true,control);
    }
}

//*create PDNet by traverse AST tree*//

void CPN::create_PDNet(gtree *p) {
    if(p == NULL)return;

    //inherited attribute
    if(p->type == COMPOUND_STATEMENT)
        create_v_table(p);
    else if(p->type == DECLARATION)
        visit_declaration(p);
    else if(p->type == FUNCTION_DEFINITION)
        visit_function(p);
    else if(judge_statement(p))
        visit_statement(p);
    else if(judge_call_postfix_expression(p))
        handle_call(p);

    create_PDNet(p->child);

    //comprehensive attribute
    if(p->type == COMPOUND_STATEMENT)
        supply_compound(p);
    else if(p->type == EXPRESSION)
        handle_expression(p);
    else if(p->type == ITERATION_STATEMENT || p->type == SELECTION_STATEMENT)
        handle_iter_sel(p);
    else if(p->type == FUNCTION_DEFINITION)
        supply_func(p);
    else if(judge_jump_statement(p))//equal to jump_statement,the same level with SELECTION\ITERATION statement above
        supply_jump_statement(p);
    if(judge_statement(p))
        organize_call(this,p);

    create_PDNet(p->next);
}

void Tokens::init_a_token(type tid, Integer_t value) {
    tokencount = 1;
    if(tid == Integer)
        color = new IntegerSortValue;
    else
    {
        cout<<"init_a_token must have matched tid and value!"<<endl;
        exit(-1);
    }
    color->setColor(value);
}

void Tokens::init_a_token(type tid, Real_t value) {
    tokencount = 1;
    if(tid == Real)
        color = new RealSortValue;
    else
    {
        cout<<"init_a_token must have matched tid and value!"<<endl;
        exit(-1);
    }
    color->setColor(value);
}

void Tokens::init_a_token(type tid, String_t value) {
    tokencount = 1;
    if(tid == String)
        color = new StringSortValue;
    else
    {
        cout<<"init_a_token must have matched tid and value!"<<endl;
        exit(-1);
    }
    color->setColor(value);
}

void CPN::print_CPN(string filename) {
    ofstream out;
    out.open(filename, ios::out);
    //out << "Place:" << endl;
    //out << "-----------------------------------" << endl;

    int P_width, T_width,T_height,font_size=30;
    P_width=T_width=T_height=1;

    string fillcolor = "chartreuse";
    string fillcolor1 = "lightblue";


    for (int i = 0; i < placecount; i++) {
        if (place[i].control_P == false)
            out << "subgraph cluster_" << place[i].id << "{"<<"fontsize = "<<to_string(font_size)<< ";label=\"" <<
                place[i].expression << "\";color=\"white\"" << place[i].id <<
                "[shape=circle"<<",fontsize = "<<to_string(font_size)<<",width="<<to_string(P_width)<<",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor << "\"]}" << endl;
        else {
//            out << place[i].id << "[shape=circle," << "label=\"" << place[i].expression << "\"]" << endl;
            out << "subgraph cluster_" << place[i].id << "{"<<"fontsize = "<<to_string(font_size)<< ";label=\"" <<
                place[i].expression << "\";color=\"white\"" << place[i].id <<
                "[shape=circle"<<",fontsize = "<<to_string(font_size)<<",width="<<to_string(P_width)<<",style=\"filled\",color=\"black\",fillcolor=\"" << fillcolor1 << "\"]}" << endl;
        }
    }
    //out << "-----------------------------------" << endl;
    //out << "Transition:" << endl;
    //out << "-----------------------------------" << endl;
    for (int i = 0; i < transitioncount; i++) {
        out << transition[i].id << "[shape=box"<<",fontsize = "<<to_string(font_size)<<",width="<<to_string(T_width)<<",height="<<to_string(T_height)<<"]" << endl;
    }
    //out << "-----------------------------------" << endl;
    //out << "Arc:" << endl;
    //out << "-----------------------------------" << endl;

    for (int i = 0; i < arccount; i++) {
        if (arc[i].arcType == executed)
            out << "{" << arc[i].source_id << "," << arc[i].target_id << "[color=\"red\"]}" << endl;
        else if (arc[i].arcType == write)
            out << "{" << arc[i].source_id << "," << arc[i].target_id << "[color=\"blue\"]}" << endl;
        else if (arc[i].arcType == data)
            out << "{" << arc[i].source_id << "," << arc[i].target_id << "[color=\"blue\"]}" << endl;
        else
            out << "{" << arc[i].source_id << "," << arc[i].target_id << "}" << endl;
    }
    out.close();

}

void CPN::set_producer_consumer() {
    string source,target;
    CSArc csArc1,csArc2;
    for(int i=0;i<arccount;i++)
    {
        source = arc[i].source_id;
        target = arc[i].target_id;
        csArc1.arcType = arc[i].arcType;
        csArc1.arc_exp = arc[i].arc_exp;
        csArc1.onlydot = arc[i].onlydot;
        csArc2.arcType = arc[i].arcType;
        csArc2.arc_exp = arc[i].arc_exp;
        csArc2.onlydot = arc[i].onlydot;
        if(arc[i].isp2t) {
            auto siter = mapPlace.find(source);
            if(siter == mapPlace.end())
            {
                cout<<"can't find source in set_pc"<<endl;
                exit(-1);
            }
            auto titer = mapTransition.find(target);
            if(titer == mapTransition.end())
            {
                cout<<"can't find target in set_pc"<<endl;
                exit(-1);
            }
            csArc1.idx = titer->second;
            csArc2.idx = siter->second;
            place[siter->second].consumer.push_back(csArc1);
            transition[titer->second].producer.push_back(csArc2);
        }
        else
        {
            auto titer = mapPlace.find(target);
            if(titer == mapPlace.end())
            {
                cout<<"can't find target in set_pc"<<endl;
                exit(-1);
            }
            auto siter = mapTransition.find(source);
            if(siter == mapTransition.end())
            {
                cout<<"can't sourcce var in set_pc"<<endl;
                exit(-1);
            }
            csArc1.idx = siter->second;
            csArc2.idx = titer->second;
            place[titer->second].producer.push_back(csArc1);
            transition[siter->second].consumer.push_back(csArc2);
        }

    }
}

void CPN::copy_childNet(CPN *cpnet,vector<string> places,vector<string> transitions) {

    init();
//    mapPlace = cpnet->mapPlace;
    mapVariable = cpnet->mapVariable;
//    mapTransition = cpnet->mapTransition;
    mapFunction = cpnet->mapFunction;
    vartable = cpnet->vartable;
    for(unsigned int i=0;i<places.size();i++)
    {
        auto iter = cpnet->mapPlace.find(places[i]);
        if(iter == cpnet->mapPlace.end())
        {
            cout<<"can't find place in copy_childtree"<<endl;
            exit(-1);
        }
        memcpy(&place[placecount++],&cpnet->place[iter->second],sizeof(CPlace));
//        place[placecount++] = cpnet->place[iter->second];
//        for(auto iter=place[placecount-1].producer.begin();iter!=place[placecount-1].producer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = place[placecount-1].producer.erase(iter);
//            else
//                iter++;
//        }
//        for(auto iter=place[placecount-1].consumer.begin();iter!=place[placecount-1].consumer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = place[placecount-1].consumer.erase(iter);
//            else
//                iter++;
//        }
        place[placecount-1].producer.clear();
        place[placecount-1].consumer.clear();
        mapPlace.insert(make_pair(places[i],placecount-1));
    }
    for(unsigned int i=0;i<transitions.size();i++)
    {
        auto iter = cpnet->mapTransition.find(transitions[i]);
        if(iter == cpnet->mapTransition.end())
        {
            cout<<"can't find transition in copy_childtree"<<endl;
            exit(-1);
        }
        memcpy(&transition[transitioncount++],&cpnet->transition[iter->second],sizeof(CTransition));
//        transition[transitioncount++] = cpnet->transition[iter->second];
//        for(auto iter=transition[transitioncount-1].producer.begin();iter!=transition[transitioncount-1].producer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = transition[transitioncount-1].producer.erase(iter);
//            else
//                iter++;
//        }
//        for(auto iter=transition[transitioncount-1].consumer.begin();iter!=transition[transitioncount-1].consumer.end();)
//        {
//            if(!exist_in(transitions,cpnet->transition[iter->idx].id))
//                iter = transition[transitioncount-1].consumer.erase(iter);
//            else
//                iter++;
//        }
        transition[transitioncount-1].producer.clear();
        transition[transitioncount-1].consumer.clear();
        mapTransition.insert(make_pair(transitions[i],transitioncount-1));
    }
    for(unsigned int i=0;i<cpnet->arccount;i++)
    {
        if(cpnet->arc[i].isp2t)
        {
            if(exist_in(places,cpnet->arc[i].source_id) && exist_in(transitions,cpnet->arc[i].target_id))
                arc[arccount++] = cpnet->arc[i];
        }
        else
        {
            if(exist_in(transitions,cpnet->arc[i].source_id) && exist_in(places,cpnet->arc[i].target_id))
                arc[arccount++] = cpnet->arc[i];
        }
    }
    set_producer_consumer();
}

void delete_arc(CPN *cpn,string source,string target){
    for(unsigned int i=0;i<cpn->arccount;i++)
        if((cpn->arc[i].source_id == source || source == "" )
        && (cpn->arc[i].target_id == target || target == "" )){
            for(unsigned int j=i;j<cpn->arccount-1;j++){
                cpn->arc[j] = cpn->arc[j+1];
            }
            cpn->arccount--;
        }
//    cout<<"can't find delete_arc!"<<endl;
//    exit(-1);
}

void CPN::delete_compound(gtree *p) {
    if(p==NULL)return;
    if(p->type == COMPOUND_STATEMENT && p->parent->type == STATEMENT)
    {
        string statement_P = p->parent->matched_P;
        string statement_T = get_enter_T(statement_P)[0];//compound just has one enter_T
        string pre_exe_P,after_exe_P,father_T;
        vector<string> after_P;

        //find pre_exe_P,after_exe_P,after_P
        //father_T pre_exe_P and after_exe_P are the only so we can just define one father_P pre_exe_P and after_exe_P
        for(unsigned int i=0;i<arccount;i++){
            if(arc[i].target_id == statement_T){
                string temp = arc[i].source_id;
                auto iter = mapPlace.find(temp);
                if(place[iter->second].expression == executed_P_name){
                    pre_exe_P = temp;
                }
            }
            if(arc[i].source_id == statement_T){
                string temp = arc[i].target_id;
                auto iter = mapPlace.find(temp);
                if(place[iter->second].expression == executed_P_name){
                    after_exe_P = temp;
                }
                else{
                    after_P.push_back(temp);
                }
            }
            if(arc[i].target_id == statement_P){
                father_T = arc[i].source_id;
            }
        }
        bool iter_flag=false;
        for(unsigned int i=0;i<arccount;i++)
            if(arc[i].target_id == after_exe_P && arc[i].source_id != statement_T){
                iter_flag=true;
                break;
            }

        delete_arc(this,father_T,statement_P);
        delete_arc(this,after_exe_P,"");
        delete_arc(this,"",after_exe_P);
        delete_arc(this,pre_exe_P,statement_T);
        for(unsigned int i=0;i<after_P.size();i++) {
            delete_arc(this, statement_T, after_P[i]);
            Add_Arc(father_T,after_P[i],"",false,control);
        }
        vector<string> first_after_T = get_enter_T(after_P[0]);
        for(unsigned int i=0;i<first_after_T.size();i++)
            Add_Arc(pre_exe_P,first_after_T[i],"",true,control);

        if(iter_flag == true){
            vector<string> last_T = get_enter_T(after_P[after_P.size()-1]);
            for(unsigned int i=0;i<last_T.size();i++)
                Add_Arc(last_T[i],pre_exe_P,"",false,executed);
        }
    }
    delete_compound(p->child);
    delete_compound(p->next);
}


NUM_t MultiSet::tokennum() {
    int sum = 0;
    Tokens *p=this->tokenQ->next;
    while(p) {
        sum += p->tokencount;
        p=p->next;
    }
}