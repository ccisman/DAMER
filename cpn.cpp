//
// Created by cc on 2020/10/10.
//

#include "cpn.h"
#include "v_table.h"
#include<cmath>

SortTable sorttable;
SORTID SortTable::psptr = 0;

vector<V_Table *> v_tables;//variable tables
vector<Triple> readpointer;
vector<Triple> writepointer;
vector<Triple> strcpypointer;
vector<Triple> threadcreatearr;
map<string,string> join_create;
map<string,unsigned short> place_size;//first is place,second is size. place(1)——address(n)
map<string,bool> place_isglobal;//first is place,second is isglobal. isglobal and size should be together(can be improved)

int gen_P_num=0,gen_T_num=0;
ID_t id_ptr = 1000;
TID_t init_tid = "\"main\"";
TID_t pthread_P = "pthread";
type TID_colorset = String;
//TID_t tid_increment = 1000;
//string tid_store_P;
//string tid_store_type = "tid_store";
//string oldtid_str = "oldtid";
string alloc_store_P;
string alloc_store_type = "alloc_store";
string malloc_str = "malloc";
string calloc_str = "calloc";
ID_t init_alloc = 10000;
unsigned int malloc_maxsize = 100;
unsigned short thread_max = 20;

//map<string,string> map_address;//first is address(string),second is variable. address(1)——variable(1)


map<string,type> map_build_in_type {
        {"void",dot},
        {"char",String},
        {"short",Integer},
        {"int",Integer},
        {"long",Integer},
        {"float",Real},
        {"double",Real}
};
//map<string,MSI> usrdf_type;
string controlflag = "control";
string arr_suffix = "_arr",var_suffix = "_var",begin_suffix = " begin",
        id_suffix = "_id",tid_str = "__tid",
        end_suffix = " end",return_suffix = "_v",
        call_suffix = "()",global_suffix = "_global",
        call_statement_suffix = "_call",join_suffix = " join";
string executed_P_name = "executed_P";
vector<string> pthread_type = {"pthread_t","pthread_mutex_t","pthread_cond_t"};
vector<string> pthread_func_type = {"pthread_create","pthread_join","pthread_exit",
                                    "pthread_mutex_init","pthread_mutex_lock","pthread_mutex_unlock",
                                    "pthread_cond_init","pthread_cond_signal","pthread_cond_wait","pthread_mutex_destroy"};
vector<string> lib_func_type = {"printf","malloc","calloc","strcpy"};

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
    string tmp;
//    int arr_count = 0;
    bool single_quotation_flag = false,double_quotation_flag = false;
    for (unsigned int i = 0; i < s.length(); i++) {
//        if (s[i] == '(' || s[i] == ')')
//            continue;
//        if(s[i] == '[')
//            arr_count++;
//        else if(s[i] == ']')
//            arr_count--;
//        int count = is_operator(s,i);
//        if(count != 0 && arr_count == 0){
//            v.push_back(tmp);
//            tmp.clear();
//            i += count-1;
//        }
//        else
//            tmp = tmp + s[i];

//  Here we just extract all those variable occured in string:s. no matter where it is.
        if(s[i]=='\'')
            single_quotation_flag = ~single_quotation_flag;
        else if(s[i]=='"')
            double_quotation_flag = ~double_quotation_flag;
        if (!isdigit(s[i]) && !isalpha(s[i]) && s[i] != '_' && !single_quotation_flag && !double_quotation_flag) {
//            if (!isdigit(s[i + 1]) && !isalpha(s[i + 1]) && s[i + 1] != '_')
//                i++;
            if (!exist_in(v, tmp) && !tmp.empty())
                v.push_back(tmp);
            tmp.clear();
        } else
            tmp = tmp + s[i];
    }

    v.push_back(tmp);
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
int find_v_size(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->name == base)
        {
            return v_tables[i]->get_size(v_name);
        }
    }
    cout<<"can't find v size"<<endl;
    exit(-1);
}
bool find_v_ispointer(string v_name,string base)
{
    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->name == base)
        {
            return v_tables[i]->get_ispointer(v_name);
        }
    }
    cout<<"can't find v ispointer"<<endl;
    exit(-1);
}
bool find_v_isglobal(string v_name,string base){

    for(unsigned int i=0;i<v_tables.size();i++)
    {
        if(v_tables[i]->name == base)
        {
            return v_tables[i]->get_isglobal(v_name);
        }
    }
    cout<<"can't find v isglobal"<<endl;
    exit(-1);
}

//vector<string> find_v_reltab(string v_name,string base)
//{
//    for(unsigned int i=0;i<v_tables.size();i++)
//    {
//        if(v_tables[i]->name == base)
//        {
//            return v_tables[i]->get_pointerrelated(v_name);
//        }
//    }
//    cout<<"can't find v reltab!"<<endl;
//    exit(-1);
//}



string construct_arcexpstr(string value,string index,string id,string tid){
    string result;

    if(index == "" && id == "" && tid == "") {
        result = "1`" + value;
        return result;
    }

    result.append("1`{");
    result.append(value);
    if(!index.empty()){
        result.append(",");
        result.append(index);
    }
    if(!id.empty()) {
        result.append(",");
        result.append(id);
    }
    if(!tid.empty()) {
        result.append(",");
        result.append(tid);
    }
    result.append("}");

    return result;
}

//***************   create_connect   *************//
//Input: petri net, related transition, expression, base deal with homonymous variable
//Function: deal with right value. split the expression, and create connection between T and places splited in the expression
void create_connect(CPN *petri, string T, string express, string base)
{
    string P2;
    vector<string> v;
    string V;
    bool sourceP;
    bool ispointer,isglobal;
    int size;
    splitExpression(express, v);
//    set<string> exist_V;
    for (unsigned int i = 0; i < v.size(); i++)
    {
        if (v[i][0] == '_' || (v[i][0] >= 'a'&&v[i][0] <= 'z') || (v[i][0] >= 'A'&&v[i][0] <= 'Z'))
        {
            auto pos = v[i].find('[');
            string id,index;
            if(pos != string::npos){
                id = v[i].substr(0,pos);
                index = v[i].substr(pos+1,v[i].size()-pos-2);
            }
            else
                id = v[i];

            //_v place's arc construct in handle_call
            if(v[i].length()>return_suffix.length() &&
            v[i].substr(v[i].length()-return_suffix.length()) == return_suffix)
                continue;
            else if(v[i] == "case")
                continue;
            P2 = find_P_name(id,base);
            size = find_v_size(id,base);
            ispointer = find_v_ispointer(id,base);
            isglobal = find_v_isglobal(id,base);

            auto idpiter = petri->mapPlace.find(P2);
            id = petri->place[idpiter->second].expression;
            string tid_flag;
            if(isglobal)
                tid_flag = "";
            else
                tid_flag = tid_str;

            if(ispointer){
                //pointer
                Triple triple;
                triple.first = T;
                triple.second = id;
                readpointer.push_back(triple);
//                V = construct_arcexpstr(id,"",id+id_suffix,tid_flag);

//                vector<string> reltab = find_v_reltab(id,base);
//                for(auto iter = place_size.begin();iter!=place_size.end();iter++){
//
//                    auto giter = place_isglobal.find(iter->first);
//                    if(giter == place_isglobal.end()){
//                        cerr<<"ERROR!place_isglobal and place_size don't match!"<<endl;
//                        exit(-1);
//                    }
//                    string tid_flag1;
//                    if(giter->second)
//                        tid_flag1 = "";
//                    else
//                        tid_flag1 = tid_str;
//
//                    auto piter = petri->mapPlace.find(iter->first);
//                    if(piter == petri->mapPlace.end()){
//                        cerr<<"ERROR!can't find place!"<<endl;
//                        exit(-1);
//                    }
//                    string Exp ,sub_id = petri->place[piter->second].expression;
//                    if(iter->second>1) {
//                        for (unsigned short j = 0; j < iter->second; j++) {
//                            if(j!=0)
//                                Exp += "++";
//                            Exp += construct_arcexpstr(sub_id + to_string(j), to_string(j), sub_id + to_string(j) + id_suffix,
//                                                       tid_flag1);
//                        }
//                    }
//                    else
//                        Exp = construct_arcexpstr(sub_id, "", sub_id + id_suffix, tid_flag1);
//                    petri->Add_Arc_override(T,iter->first,Exp,false,remain,true);
//                    petri->Add_Arc_override(iter->first,T,Exp,true,remain,true);
//                }
            }
            if(size > 1){
                //array

                for(unsigned int j=0;j<size;j++) {
                    if(j!=0)
                        V.append("++");
                    V += construct_arcexpstr(id + to_string(j),to_string(j),id+to_string(j)+id_suffix,tid_flag);
                }
            }
            else
                V = construct_arcexpstr(id,"",id + id_suffix,tid_flag);


//            auto viter = exist_V.find(V);
//            if(viter!=exist_V.end())
//                continue;
            auto iter = petri->mapTransition.find(T);
            if(iter == petri->mapTransition.end())
            {
                cout<<"create_connect can't find T"<<endl;
                exit(-1);
            }
//            petri->transition[iter->second].relvars.insert(V);
//            exist_V.insert(V);

//            auto piter = petri->mapPlace.find(P2);

//            petri->Add_Variable(V,petri->place[piter->second].initMarking.tid,petri->place[piter->second].initMarking.sid);

            petri->Add_Arc_override(P2,T,V,true,data,false);
            petri->Add_Arc_override(T,P2,V,false,data,false);
//            sourceP = true;
//            petri.Add_Arc(P2, T, V, sourceP);//_v库所
//            sourceP = false;
//            //			V = "";
//            petri.Add_Arc(T, P2, V, sourceP);

        }
    }
}

//***************   create_assignment   *************//
//parameters explain:
//**left_P:left value's place
//**T:transition that connected with
//**index:the index for CPN variable(after translating)
//**tid_flag:tid or ""(global)
//**exp_tmp:right value (after translating)
//**base:base compound statement
//**itsef: true if assign itself, false if assign pointer or arr
//Function: deal with left value.create assignment connection and create connect(if necessary)
void create_assignment(CPN *cpn,string left_P,string T,string index,string tid_flag,string exp_tmp,bool itself){

    //itself is true if assign a pointer itself or a normal variable

    string Exp1,Exp2,id;
    auto iter = cpn->mapPlace.find(left_P);
    if(iter == cpn->mapPlace.end()){
        cerr <<"can't find place in create_assignment!"<<endl;
        exit(-1);
    }
    id = cpn->place[iter->second].expression;

    if(itself){
        int size;
        size = cpn->place[iter->second].size;
        if(size > 1)
            id = id + to_string(0) + id_suffix;
        Exp1 = construct_arcexpstr(exp_tmp,"",id + id_suffix,tid_flag);
        Exp2 = construct_arcexpstr(id,"",id + id_suffix,tid_flag);

        cpn->Add_Arc_override(T,left_P,Exp1,false,write,false);
        cpn->Add_Arc_override(left_P,T,Exp2,true,write,false);
    }
    else {
        bool ispointer = cpn->place[iter->second].is_pointer;
        if (!ispointer) {
            int size;
            size = cpn->place[iter->second].size;

            for (int i = 0; i < size; i++) {
                if (i != 0)
                    Exp2.append("++");
                string tmp;
                tmp = construct_arcexpstr(id + to_string(i), to_string(i), id + to_string(i) + id_suffix,
                                          tid_flag);
                Exp2.append(tmp);
            }
            Exp1 = Exp2;

            string readcase, writecase;
            writecase = readcase = CaseFlag + index;
            for (int i = 0; i < size; i++) {
                string tmp1, tmp2;
                tmp1 = construct_arcexpstr(id + to_string(i), to_string(i), id + to_string(i) + id_suffix,
                                           tid_flag);
                tmp2 = construct_arcexpstr(exp_tmp, to_string(i), id + to_string(i) + id_suffix,
                                           tid_flag);
                readcase.append(":");
                readcase.append(to_string(i));
                readcase.append("=>");
                readcase.append(tmp1);
                writecase.append(":");
                writecase.append(to_string(i));
                writecase.append("=>");
                writecase.append(tmp2);
            }
            readcase.append(":default;");
            writecase.append(":default;");
            Exp1.append("++");
            Exp1.append(writecase);
            Exp1.append("--");
            Exp1.append(readcase);
        } else {


            Exp1 = construct_arcexpstr(id, "", id + id_suffix, tid_flag);
            Exp2 = Exp1;
            Triple triple;
            triple.first = T;
            triple.second = id + "+" + index;
            triple.third = exp_tmp;
            writepointer.push_back(triple);
//            for (auto iter = place_size.begin(); iter != place_size.end(); iter++) {
//                auto giter = place_isglobal.find(iter->first);
//                if (giter == place_isglobal.end()) {
//                    cerr << "ERROR!place_isglobal and place_size don't match!" << endl;
//                    exit(-1);
//                }
//                string tid_flag1;
//                if (giter->second)
//                    tid_flag1 = "";
//                else
//                    tid_flag1 = tid_str;
//
//                auto piter = cpn->mapPlace.find(iter->first);
//                if (piter == cpn->mapPlace.end()) {
//                    cerr << "ERROR!can't find place!" << endl;
//                    exit(-1);
//                }
//                string Exp_tmp1, Exp_tmp2, tmp, sub_id = cpn->place[piter->second].expression;
//                string writecase, readcase;
//                readcase = writecase = CaseFlag + id + "+" + index;
//
//
//                if (iter->second > 1) {
//                    for (unsigned short j = 0; j < iter->second; j++) {
//                        writecase.append(":");
//                        writecase.append(sub_id + to_string(j) + id_suffix);
//                        writecase.append("=>");
//                        writecase.append(construct_arcexpstr(exp_tmp, to_string(j), sub_id + to_string(j) + id_suffix,
//                                                             tid_flag1));
//                        readcase.append(":");
//                        readcase.append(sub_id + to_string(j) + id_suffix);
//                        readcase.append("=>");
//                        readcase.append(construct_arcexpstr(sub_id + to_string(j), to_string(j),
//                                                            sub_id + to_string(j) + id_suffix,
//                                                            tid_flag1));
//                    }
//                    readcase.append(":default;");
//                    writecase.append(":default;");
//                    Exp_tmp1 = readcase;
//                    Exp_tmp2 = writecase;
//                } else {
//                    tmp = construct_arcexpstr(sub_id, "", sub_id + id_suffix, tid_flag1);
//                    Exp_tmp1 += tmp;
//                    writecase.append(":");
//                    writecase.append(sub_id + id_suffix);
//                    writecase.append("=>");
//                    writecase.append(construct_arcexpstr(exp_tmp, "", sub_id + id_suffix, tid_flag1));
//                    writecase.append(":");
//                    writecase.append(tmp);
//                    writecase.append(";");
//                    Exp_tmp2 = writecase;
//                }
//                cpn->Add_Arc_override(T, iter->first, Exp_tmp2, false, remain, true);
//                cpn->Add_Arc_override(iter->first, T, Exp_tmp1, true, remain, true);
//            }
        }
        Arc_Type arcType;
//        if(ispointer)
//            arcType = data;
//        else
            arcType = write;
        cpn->Add_Arc_override(T, left_P, Exp1, false, arcType, false);
        cpn->Add_Arc_override(left_P, T, Exp2, true, arcType, false);
    }
}

void create_readpointerarc(CPN *cpn,Triple triple) {
    string T = triple.first;
    string real_id = triple.second;
    for (auto iter = place_size.begin(); iter != place_size.end(); iter++) {

        auto giter = place_isglobal.find(iter->first);
        if (giter == place_isglobal.end()) {
            cerr << "ERROR!place_isglobal and place_size don't match!" << endl;
            exit(-1);
        }
        string tid_flag1;
        if (giter->second)
            tid_flag1 = "";
        else
            tid_flag1 = tid_str;

        auto piter = cpn->mapPlace.find(iter->first);
        if (piter == cpn->mapPlace.end()) {
            cerr << "ERROR!can't find place!" << endl;
            exit(-1);
        }
        string Exp, sub_id = cpn->place[piter->second].expression;
        Exp += CaseFlag + real_id + "!=0:1=>";
        if (iter->second > 1) {
            for (unsigned short j = 0; j < iter->second; j++) {
                if (j != 0)
                    Exp += "++";
                Exp += construct_arcexpstr(sub_id + to_string(j), to_string(j), sub_id + to_string(j) + id_suffix,
                                           tid_flag1);
            }
        } else
            Exp += construct_arcexpstr(sub_id, "", sub_id + id_suffix, tid_flag1);
        Exp += ":default;";
        cpn->Add_Arc_override(T, iter->first, Exp, false, remain, true);
        cpn->Add_Arc_override(iter->first, T, Exp, true, remain, true);
    }
}

void create_writepointerarc(CPN *cpn,string T,string address,string exp_tmp){
    //mark transition for slicing

    auto titer = cpn->mapTransition.find(T);
    if(titer == cpn->mapTransition.end()){
        cerr<< "ERROR!can't find transition!"<<endl;
        exit(-1);
    }
    cpn->transition[titer->second].is_writepointer = true;

    for (auto iter = place_size.begin(); iter != place_size.end(); iter++) {
        auto giter = place_isglobal.find(iter->first);
        if (giter == place_isglobal.end()) {
            cerr << "ERROR!place_isglobal and place_size don't match!" << endl;
            exit(-1);
        }
        string tid_flag1;
        if (giter->second)
            tid_flag1 = "";
        else
            tid_flag1 = tid_str;

        auto piter = cpn->mapPlace.find(iter->first);
        if (piter == cpn->mapPlace.end()) {
            cerr << "ERROR!can't find place!" << endl;
            exit(-1);
        }
        string Exp_tmp1, Exp_tmp2, tmp, sub_id = cpn->place[piter->second].expression;
        string writecase, readcase;
        readcase = writecase = CaseFlag + address;


        if (iter->second > 1) {
            for (unsigned short j = 0; j < iter->second; j++) {
                writecase.append(":");
                writecase.append(sub_id + to_string(j) + id_suffix);
                writecase.append("=>");
                writecase.append(construct_arcexpstr(exp_tmp, to_string(j), sub_id + to_string(j) + id_suffix,
                                                     tid_flag1));
                readcase.append(":");
                readcase.append(sub_id + to_string(j) + id_suffix);
                readcase.append("=>");
                readcase.append(construct_arcexpstr(sub_id + to_string(j), to_string(j),
                                                    sub_id + to_string(j) + id_suffix,
                                                    tid_flag1));
            }
            readcase.append(":default;");
            writecase.append(":default;");
            Exp_tmp1 = readcase;
            Exp_tmp2 = writecase;
        } else {
            tmp = construct_arcexpstr(sub_id, "", sub_id + id_suffix, tid_flag1);
            Exp_tmp1 += tmp;
            writecase.append(":");
            writecase.append(sub_id + id_suffix);
            writecase.append("=>");
            writecase.append(construct_arcexpstr(exp_tmp, "", sub_id + id_suffix, tid_flag1));
            writecase.append(":");
            writecase.append(tmp);
            writecase.append(";");
            Exp_tmp2 = writecase;
        }
        cpn->Add_Arc_override(T, iter->first, Exp_tmp2, false, remain, true);
        cpn->Add_Arc_override(iter->first, T, Exp_tmp1, true, remain, true);
    }
}

void create_strcpypointerarc(CPN *cpn,Triple triple){
    string T = triple.first;
    string str = triple.second;
    string address = triple.third;

    for (auto iter = place_size.begin(); iter != place_size.end(); iter++) {
        if(iter->second < str.length())
            continue;
        auto giter = place_isglobal.find(iter->first);
        if (giter == place_isglobal.end()) {
            cerr << "ERROR!place_isglobal and place_size don't match!" << endl;
            exit(-1);
        }
        string tid_flag1;
        if (giter->second)
            tid_flag1 = "";
        else
            tid_flag1 = tid_str;

        auto piter = cpn->mapPlace.find(iter->first);
        if (piter == cpn->mapPlace.end()) {
            cerr << "ERROR!can't find place!" << endl;
            exit(-1);
        }
        string Exp_tmp1, Exp_tmp2, tmp_value, sub_id = cpn->place[piter->second].expression;
        string writecase, readcase;
        readcase = writecase = CaseFlag + address;

        if (iter->second > 1) {
            string real_id = sub_id;
            writecase.append(":");
            writecase.append(sub_id + to_string(0) + id_suffix);
            writecase.append("=>");
            readcase = writecase;
            for(unsigned int i=0;i<str.length();i++){
                tmp_value.clear();
                tmp_value.append("'");
                tmp_value.append(1,str[i]);
                tmp_value.append("'");
                if(i>0){
                    writecase += "++";
                    readcase += "++";
                }
                writecase += construct_arcexpstr(tmp_value,to_string(i),real_id + to_string(i) + id_suffix,tid_flag1);;
                readcase += construct_arcexpstr(real_id + to_string(i),to_string(i),real_id + to_string(i) + id_suffix,tid_flag1);
            }

            writecase.append(":default;");
            readcase.append(":default;");

            Exp_tmp1 = readcase;
            Exp_tmp2 = writecase;
        } else {
            cerr << "ERROR!strcpy's source can not be empty!"<<endl;
            exit(-1);
        }
        cpn->Add_Arc_override(T, iter->first, Exp_tmp2, false, remain, false);
        cpn->Add_Arc_override(iter->first, T, Exp_tmp1, true, remain, false);
    }
}

void create_threadcreate(CPN *cpn,Triple triple){
    string T = triple.first;
    string newtid = triple.second;
    string third = triple.third;
    string para,exp_tmp,init;
//    bool init_flag = false;
    auto pos = third.find(":");
    if(pos == string::npos){
        exp_tmp = "0";
    }
    para = third.substr(0,pos);
    exp_tmp = third.substr(pos+1);
    int tmp_id_ptr = 2000;
    for(auto iter = place_size.begin();iter!=place_size.end();iter++) {
        if(iter->first == para)
            init = exp_tmp;
        else
            init = "0";
        auto giter = place_isglobal.find(iter->first);
        if (giter == place_isglobal.end()) {
            cerr << "ERROR!place_isglobal and place_size don't match!" << endl;
            exit(-1);
        }
        string newtid_flag1;
        if (giter->second)
            continue;
        else {
//                    tid_flag1 = tid_str;
            newtid_flag1 = newtid;
        }
        auto piter = cpn->mapPlace.find(iter->first);
        if (piter == cpn->mapPlace.end()) {
            cerr << "ERROR!can't find place!" << endl;
            exit(-1);
        }
        string Exp, newExp, sub_id = cpn->place[piter->second].expression;
        if (iter->second > 1) {
            for (unsigned short j = 0; j < iter->second; j++) {
                if (j != 0)
                    newExp += "++";
                newExp +=
                        construct_arcexpstr(init, to_string(j),
                                            to_string(tmp_id_ptr++),
                                            newtid_flag1);
            }
        } else {
            newExp =
                    construct_arcexpstr(init, "", to_string(tmp_id_ptr++),
                                        newtid_flag1);
        }
        cpn->Add_Arc_addition(T, iter->first, newExp, false, remain);
    }
}

void iter_extra(CPN *petri,string newP,string _P){
    vector<string> false_exit = petri->get_falseexit_T(_P);
    for (unsigned int i = 0; i < false_exit.size(); i++)
        petri->Add_Arc(false_exit[i], newP, construct_arcexpstr(tid_str,"","",""), false, executed);
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
        }
//        else if (tr->place == "pthread_join" + return_suffix) {
//            gtree *postexp = tr;
//
//            vector<string> paras = extract_paras(tr->child);
//            auto iter = create_P_list.find(paras[0]);
//            if (iter == create_P_list.end()) {
//                cout << "can't find matched pthread_create" << endl;
//                exit(-1);
//            }
//            vector<string> cor_P;
//            cor_P.push_back(_P);
//            petri->set_correspond_P(iter->second, cor_P);
//        }

        if (call_P.size() != 0) {
            //call_P[0]代表调用到的第一个函数
//            for (unsigned int i = 0; i < call_P.size(); i++) {
                if (mutex_flag == false)
                    petri->Add_Arc(T, call_P[0], construct_arcexpstr(tid_str,"","",""), false, control);
                else
                    petri->Add_Arc(mutex_T, call_P[0], construct_arcexpstr(tid_str,"","",""), false, control);
//            }
        } else {
            if (mutex_flag == false)
                petri->Add_Arc(T, _P, construct_arcexpstr(tid_str,"","",""), false, control);
            else
                petri->Add_Arc(mutex_T, _P, construct_arcexpstr(tid_str,"","",""), false, control);
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


        last = petri->get_exit_T(_P);

        if (tr->place == "pthread_mutex_lock" + return_suffix) {
            mutex_flag = true;
            mutex_P = tr->matched_P;
            vector<string> lock_T = petri->get_enter_T(mutex_P);
            mutex_T = lock_T[0];
        }
//        else if (tr->place == "pthread_create" + return_suffix) {
//            //simplely process
//
//            vector<string> paras = extract_paras(tr->child->child->child->child);
//            create_P_list.insert(make_pair(paras[0].substr(1), tr->matched_P));
//        }

        if (tr->parent->next->type == STATEMENT)
            tr = tr->parent->next;
        else
            break;

    }
}
Product_t new_ProductColor(SORTID sid){
    int psnum = sorttable.productsort[sid].sortnum;
    Product_t cid = new SortValue*[psnum];
    for(unsigned int j=0;j<psnum;j++)
        if(sorttable.productsort[sid].sortid[j].tid == Integer)
            cid[j] = new IntegerSortValue;
        else if(sorttable.productsort[sid].sortid[j].tid == Real)
            cid[j] = new RealSortValue;
        else if(sorttable.productsort[sid].sortid[j].tid == String)
            cid[j] = new StringSortValue;

    return cid;
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

void Tokens::initiate(token_count_t tc, type sort,SORTID sid) {
    tokencount = tc;
    if(sort == productsort)
        color = new ProductSortValue(sid);
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

int ps_cmp(Product_t ps1,Product_t ps2,vector<MSI> psinfo)
{
    for(int i=psinfo.size()-1;i>=0;i--)
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
        Product_t cid1,cid2;
//        int psnum = sorttable.productsort[sid].sortnum;
//        vector<MSI> psinfo = sorttable.productsort[sid].sortid;
        cid1 = new_ProductColor(sid);
        cid2 = new_ProductColor(sid);
        token->color->getColor(cid2,sid);
        int cmp;
        while(p)
        {
            p->color->getColor(cid1,sid);
            cmp = ps_cmp(cid1,cid2,sorttable.productsort[sid].sortid);
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
            Product_t cid1,cid2;
//            int psnum = sorttable.productsort[sid].sortnum;
            vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid1 = new_ProductColor(sid);
            cid2 = new_ProductColor(sid);
            t2->color->getColor(cid2,sid);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1,sid);
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
    if(!t2)
        return;
    if(tid == productsort)
    {
        while(t2)
        {
            Product_t cid1,cid2;
//            int psnum = sorttable.productsort[sid].sortnum;
            vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid1 = new_ProductColor(sid);
            cid2 = new_ProductColor(sid);
            t2->color->getColor(cid2,sid);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1,sid);
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
}

void MultiSet::PLUS(MultiSet &ms) {

    Tokens *t1 = tokenQ->next,*t2=ms.tokenQ->next,*tmp;
    while(t2) {
        tmp = new Tokens;
        tmp->tokencount = t2->tokencount;
        if (tid == productsort) {
            Product_t cid;
            int psnum = sorttable.productsort[sid].sortnum;
            //vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid = new_ProductColor(sid);
            t2->color->getColor(cid, sid);
            tmp->color = new ProductSortValue(sid);
            tmp->color->setColor(cid, sid);
            delete[] cid;
        } else if (tid == Integer) {
            Integer_t cid;
            t2->color->getColor(cid);
            tmp->color = new IntegerSortValue;
            tmp->color->setColor(cid);
        } else if (tid == Real) {
            Real_t cid;
            t2->color->getColor(cid);
            tmp->color = new RealSortValue;
            tmp->color->setColor(cid);
        } else if (tid == String) {
            String_t cid;
            t2->color->getColor(cid);
            tmp->color = new StringSortValue;
            tmp->color->setColor(cid);
        } else if (tid == dot);

        insert(tmp);
        t2 = t2->next;
    }
//    ms.tokenQ->next = NULL;
}

void MultiSet::operator=(const MultiSet &ms) {
    sid = ms.sid;
    tid = ms.tid;
    color_count = ms.color_count;
    hash_value = ms.hash_value;
    Tokens *t1=tokenQ,*t2 = ms.tokenQ->next;
//    int psnum = 0;
//    if(ms.tid == productsort)
//        psnum = sorttable.productsort[ms.sid].sortnum;
    while(t2)
    {
        t1->next = new Tokens;
        t1->next->initiate(t2->tokencount,ms.tid,sid);
        if(ms.tid == productsort)
        {
            Product_t cid;
            cid = new_ProductColor(sid);
            t2->color->getColor(cid,sid);
            t1->next->color->setColor(cid,sid);
            delete[] cid;
        }
        else if(ms.tid == Integer)
        {
            Integer_t cid;
            t2->color->getColor(cid);
            t1->next->color->setColor(cid);
        }
        else if(ms.tid == Real)
        {
            Real_t cid;
            t2->color->getColor(cid);
            t1->next->color->setColor(cid);
        }
        else if(ms.tid == String)
        {
            String_t cid;
            t2->color->getColor(cid);
            t1->next->color->setColor(cid);
        }

        t2=t2->next;
        t1=t1->next;
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
        Product_t cid = new_ProductColor(sid);
        for(p=tokenQ->next;p!=NULL;p=p->next)
        {
            p->color->getColor(cid,sid);
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
            Product_t cid1,cid2;
            int psnum = sorttable.productsort[sid].sortnum;
            vector<MSI> psinfo = sorttable.productsort[sid].sortid;
            cid1 = new_ProductColor(sid);
            cid2 = new_ProductColor(sid);
            t2->color->getColor(cid2,sid);
            int cmp;
            while(t1)
            {
                t1->color->getColor(cid1,sid);
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

string uniqueexp(string exp,CPN *cpn){

    bool found = false;
    while(1) {
        found = false;
        for (unsigned int i = 0; i < cpn->placecount; i++) {
            if (cpn->place[i].expression == exp) {
                if(exp.size() > return_suffix.size() &&
                exp.substr(exp.size()-return_suffix.size()) == return_suffix)
                    exp = exp.substr(0,exp.size()-return_suffix.size()) + "_1" + return_suffix;
                else
                    exp = exp + "_1";
                found = true;
                break;
            }
        }
        if (!found)
            break;

    }
    return exp;
}

void CPN::Add_Place(string id, string Type_name, int size,bool control_P,string exp,bool isglobal,bool ispointer) {
    CPlace *pp = &place[placecount++];
    pp->is_pointer = ispointer;
    pp->size = size;

    bool alloc_flag = false;
    if(exp == malloc_str || exp == calloc_str)
        alloc_flag = true;
    //alloc_store_type
    if(Type_name == alloc_store_type){
        pp->control_P = false;
        pp->id = id;
        pp->is_cond = false;
        pp->is_mutex = false;
        pp->expression = exp;
        pp->is_executed = false;
        Tokens *token = new Tokens;
        token->initiate(1,Integer,0);
        token->color->setColor(init_alloc);
        pp->initMarking.tid = Integer;
        pp->initMarking.sid = 0;
        pp->initMarking.insert(token);
        mapPlace.insert(make_pair(id,placecount-1));
        return;
    }
    if(exist_in(pthread_type,Type_name))
        ;
    else if(size > 1)
        Type_name = Type_name + arr_suffix;
    else if(size == 1)
        Type_name = Type_name + var_suffix;


    int psnum = 0;
    pp->control_P = control_P;//pthread_mutex should be redifine
    if(!control_P)
        exp = uniqueexp(exp,this);
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
    if(Type_name == "dot"){
        pp->initMarking.tid = dot;
        pp->initMarking.sid = 0;
        mapPlace.insert(make_pair(id,placecount-1));
        return ;
    }

    //control type
    if(Type_name == controlflag) {
        Tokens *token = new Tokens;
        auto siter = sorttable.mapSort.find(controlflag);
        pp->initMarking.tid = siter->second.tid;
        pp->initMarking.sid = siter->second.sid;
        mapPlace.insert(make_pair(id,placecount-1));
        return ;
    }

    //pthread type
    if(exist_in(pthread_type,Type_name)) {

        Tokens *token = new Tokens;

        pp->control_P = true;
        pp->initMarking.tid = Integer;
        pp->initMarking.sid = 0;

        if (Type_name == "pthread_mutex_t")
            pp->is_mutex = true;
        else if (Type_name == "pthread_cond_t")
            pp->is_cond = true;

        mapPlace.insert(make_pair(id, placecount - 1));
        return;
    }

    //variable type,including pointer array
    if(isglobal)
        Type_name += global_suffix;
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

    place_size.insert(pair<string,unsigned short>(id,size));
    place_isglobal.insert(pair<string,bool>(id,isglobal));

    if(size >1 && !alloc_flag)
    {

        for(int i=0;i<size;i++)
        {
//            map_address.insert(pair<string,string>(to_string(id_ptr),exp + to_string(i)));
            token = new Tokens;
            token->initiate(1,pp->initMarking.tid,pp->initMarking.sid);
            if(pp->initMarking.tid == productsort) {
                Product_t cid;
                cid = new_ProductColor(pp->initMarking.sid);
                if(!isglobal) {
                    cid[psnum - 3]->setColor(Integer_t(i));//INDEX in cid[psnum-3]
                    cid[psnum - 2]->setColor(Integer_t(id_ptr++));
                    cid[psnum - 1]->setColor(init_tid);
                }
                else{
                    cid[psnum-2]->setColor(Integer_t(i));
                    cid[psnum-1]->setColor(Integer_t(id_ptr++));
                }
                token->color->setColor(cid,pp->initMarking.sid);
                pp->initMarking.insert(token);
                delete[] cid;
            }
            else {
                cout<<"error!array type must be product type!"<<endl;
                exit(-1);
            }
        }
    }
    else if(size == 1 && !alloc_flag)
    {
//        map_address.insert(pair<string,string>(to_string(id_ptr),exp));
        token = new Tokens;
        token->initiate(1,pp->initMarking.tid,pp->initMarking.sid);

        if(pp->initMarking.tid == productsort) {
            Product_t cid;
            cid = new_ProductColor(pp->initMarking.sid);
            if(!isglobal) {
                cid[psnum - 2]->setColor(Integer_t(id_ptr++));
                cid[psnum - 1]->setColor(init_tid);
            }
            else
                cid[psnum - 1]->setColor(Integer_t(id_ptr++));
            token->color->setColor(cid,pp->initMarking.sid);
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
    else if(alloc_flag)
        ;
    else{
        cerr << "invalid size!"<<endl;
        exit(-1);
    }

    mapPlace.insert(make_pair(id,placecount-1));
}

void CPN::Add_Transition(string id, string guard, string base) {
    CTransition *tt = &transition[transitioncount++];
    tt->id = id;
    if(guard != "") {
        guard = "1`" + translate_exp2arcexp(this,guard,base);
        tt->guard.construct(guard);
        tt->hasguard = true;
    }
    else
        tt->hasguard = false;
    mapTransition.insert(make_pair(id,transitioncount-1));
}

void CPN::Set_Guard(string id,string guard,string base){
    auto titer = mapTransition.find(id);
    if(titer == mapTransition.end()){
        cerr << "ERROR!CAN'T FIND T!"<<endl;
        exit(-1);
    }
    CTransition *tt = &transition[titer->second];
    tt->id = id;
    tt->guard.deconstruct();
    if(guard != "") {
        guard = "1`" + translate_exp2arcexp(this,guard,base);
        tt->guard.construct(guard);
        tt->hasguard = true;
    }
    else
        tt->hasguard = false;
}

void CPN::Add_Arc(string source, string target, string exp, bool sourceP,Arc_Type arcType) {
    CArc *aa = &arc[arccount++];
    aa->arcType = arcType;

    if(exp != "") {
        //aa->arc_exp.deconstruct();
        aa->arc_exp.construct(exp);

        //Add_variable
        string related_P;
        SORTID sid;
        type tid;
        SORTNUM_t psnum;
        if(sourceP)
            related_P = source;
        else
            related_P = target;
        auto piter = mapPlace.find(related_P);
        if(piter == mapPlace.end()){
            cerr << "ERROR!Can't find Place!"<<endl;
            exit(-1);
        }
        sid = place[piter->second].initMarking.sid;
        tid = place[piter->second].initMarking.tid;
        if(tid == productsort)
            psnum = sorttable.productsort[sid].sortnum;
        else
            psnum = 1;
        if(sourceP)
            Add_Variable(aa->arc_exp.root,tid,sid,0,psnum-1);

        aa->onlydot = false;
    }
    else {
        aa->onlydot = true;
        aa->arc_exp.exp = "";
        aa->arc_exp.root = NULL;
    }
    aa->isp2t = sourceP;
    aa->source_id = source;
    aa->target_id = target;
}

void CPN::Add_Arc_override(string source, string target, string exp, bool sourceP,Arc_Type arcType,bool be_overrided) {
    CArc *aa;
    for(int i=arccount-1;i>=0;i--)
    {
        aa = &arc[i];
        if(aa->source_id == source && aa->target_id == target)
        {
            if(be_overrided)
                return;
            aa->arcType = arcType;
            if(exp != "") {
                aa->arc_exp.deconstruct();
                aa->arc_exp.construct(exp);
                aa->onlydot = false;
            }
            else {
                aa->onlydot = true;
                aa->arc_exp.exp = "";
                aa->arc_exp.root = NULL;
            }
            aa->isp2t = sourceP;
            aa->source_id = source;
            aa->target_id = target;
            return;
        }
    }
    Add_Arc(source,target,exp,sourceP,arcType);

}

void CPN::Add_Arc_addition(string source, string target, string exp, bool sourceP,Arc_Type arcType) {
    CArc *aa;
    for(int i=arccount-1;i>=0;i--)
    {
        aa = &arc[i];
        if(aa->source_id == source && aa->target_id == target)
        {
            aa->arc_exp.deconstruct();
            aa->arc_exp.construct(exp + "++" + aa->arc_exp.exp);
            aa->onlydot = false;

            aa->isp2t = sourceP;
            aa->source_id = source;
            aa->target_id = target;
            return;
        }
    }
    Add_Arc(source,target,exp,sourceP,arcType);

}

string CPN::Add_Variable(string id, type tid,SORTID sid) {
    auto iter = mapVariable.find(id);
    if(iter==mapVariable.end())
    {
        Variable *var = &vartable[varcount++];
        var->id = id;

        var->sid = sid;
        var->tid = tid;
        int psnum = 0;
        if(var->tid == productsort) {
            psnum = sorttable.productsort[var->sid].sortnum;
            var->value = new ProductSortValue(sid);
        }
        else if(var->tid == Integer)
            var->value = new IntegerSortValue;
        else if(var->tid == Real)
            var->value = new RealSortValue;
        else if(var->tid == String)
            var->value = new StringSortValue;
        mapVariable.insert(make_pair(var->id,varcount-1));
        return id;
    }
    else{

        if(vartable[iter->second].tid != tid
        || vartable[iter->second].sid != sid)
        {
            Variable *var = &vartable[varcount++];
            var->id = id + "@" + to_string(tid) + to_string(sid);
            auto iter2 = mapVariable.find(var->id);
            if(iter2 != mapVariable.end()) {
                varcount--;
                return var->id;
            }
            var->sid = sid;
            var->tid = tid;
            int psnum = 0;
            if(var->tid == productsort) {
                psnum = sorttable.productsort[var->sid].sortnum;
                var->value = new ProductSortValue(sid);
            }
            else if(var->tid == Integer)
                var->value = new IntegerSortValue;
            else if(var->tid == Real)
                var->value = new RealSortValue;
            else if(var->tid == String)
                var->value = new StringSortValue;
            mapVariable.insert(make_pair(var->id,varcount-1));
            return var->id;
        }
        else
            return id;
    }
    cerr<<"ERROR!In Add_Variable!"<<endl;
    exit(-1);
}

void CPN::init() {

    sorttable.psptr = 0;
    gen_P_num = gen_T_num = 0;
    placecount = transitioncount = arccount = varcount = 0;

    place = new CPlace[200];
    transition = new CTransition[200];
    arc = new CArc[1000];

    vartable = new Variable[1000];

}

void CPN::init_alloc_func(){
    string P = gen_P();
    alloc_store_P = P;
    Add_Place(P,alloc_store_type,1,false,"alloc_store",true,false);
    //initializing in Add_Place
}

void CPN::initDecl() {

    map<string,type>::iterator miter;

    MSI control,m_int,m_str;

    control.tid = String;
    control.sid = 0;
    sorttable.mapSort.insert(pair<string,MSI>(controlflag,control));

    m_int.tid = Integer;
    m_int.sid = 0;
    m_str.tid = String;
    m_str.sid = 0;

    for(miter=map_build_in_type.begin();miter!=map_build_in_type.end();miter++)
    {
        //normal build_in type
        MSI m,m1;
        m.tid = miter->second;
        m.sid = 0;

        sorttable.mapSort.insert(pair<string,MSI>(miter->first,m));

        //build_in type array
        ProductSort pp;
        pp.id = miter->first + arr_suffix;
        pp.sortnum = 4;
        pp.mytype = productsort;
        pp.sortname.push_back(miter->first);
        pp.hasindex = true;
        pp.hastid = true;
        pp.sortname.push_back("index");
        pp.sortname.push_back("id");
        pp.sortname.push_back("tid");

        pp.sortid.push_back(m);
        pp.sortid.push_back(m_int);
        pp.sortid.push_back(m_int);
        pp.sortid.push_back(m_str);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp.id,m1));
        sorttable.productsort.push_back(pp);

        //build_in type var
        ProductSort pp1;
        pp1.id = miter->first + var_suffix;
        pp1.sortnum = 3;
        pp1.mytype = productsort;
        pp1.sortname.push_back(miter->first);
        pp1.hastid = true;
        pp1.sortname.push_back("id");
        pp1.sortname.push_back("tid");

        pp1.sortid.push_back(m);
        pp1.sortid.push_back(m_int);
        pp1.sortid.push_back(m_str);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp1.id,m1));
        sorttable.productsort.push_back(pp1);

        //build_in type array global
        ProductSort pp2;
        pp2.id = miter->first + arr_suffix + global_suffix;
        pp2.sortnum = 3;
        pp2.mytype = productsort;
        pp2.sortname.push_back(miter->first);
        pp2.hasindex = true;
        pp2.sortname.push_back("index");
        pp2.sortname.push_back("id");
//        pp.sortname.push_back("tid");

        pp2.sortid.push_back(m);
        pp2.sortid.push_back(m_int);
        pp2.sortid.push_back(m_int);
//        pp2.sortid.push_back(m_int);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp2.id,m1));
        sorttable.productsort.push_back(pp2);

        //build_in type var global
        ProductSort pp3;
        pp3.id = miter->first + var_suffix + global_suffix;
        pp3.sortnum = 2;
        pp3.mytype = productsort;
        pp3.sortname.push_back(miter->first);
        pp3.sortname.push_back("id");
//        pp3.sortname.push_back("tid");

        pp3.sortid.push_back(m);
        pp3.sortid.push_back(m_int);
//        pp1.sortid.push_back(m_str);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp3.id,m1));
        sorttable.productsort.push_back(pp3);
    }
}

void getinfofromdeclarator(gtree *declarator,bool &pointer_flag,int &arr_size,string &id){
    if(declarator->type != DECLARATOR){
        cerr << "ERROR!getinfofromdeclarator should be a declarator node!"<<endl;
        exit(-1);
    }

    gtree *direct_declarator;
    pointer_flag = false;
    arr_size = 0;
    string identifier;
    if(declarator->child->type == POINTER){
        direct_declarator = declarator->child->next;
        pointer_flag = true;
    }
    else
        direct_declarator = declarator->child;
    if(direct_declarator->child && direct_declarator->child->type == DIRECT_DECLARATOR
    && direct_declarator->child->next && direct_declarator->child->next->place == "("){
        cerr << "ERROR!We don't support function in struct_declarator for now!"<<endl;
        exit(-1);
    }
    else if(direct_declarator->child && direct_declarator->child->type == DIRECT_DECLARATOR){
        //we just support 1 dimension array for now
        //multiple dimension just need some address translation
        if(direct_declarator->child->next->next->type == CONSTANT_EXPRESSION){
            string size = direct_declarator->child->next->next->place;
            if(isdigit(size[0]))
                arr_size = atoi(size.c_str());
            else{
                cerr << "We don't support non-determined array for now!"<<endl;
                exit(-1);
            }
        }
        else{
            cerr << "We don't support non-determined array for now!"<<endl;
            exit(-1);
        }
        while(direct_declarator->type==DIRECT_DECLARATOR)
            direct_declarator = direct_declarator->child;
        if(direct_declarator->type == IDENTIFIER)
            id = direct_declarator->place;
        else{
            cerr << "We don't support declarator like:int (*a) for now!"<<endl;
            exit(-1);
        }
    }
    else if(direct_declarator){
        // due to cut_tree, there may just have one IDENTIFIER below DIRECT_DECLARATOR
        id = direct_declarator->place;
    }
}

vector<var_type*> process_struct_declaration_list(gtree *p)
{
    vector<var_type*> var_list;
    gtree *struct_declaration_list = p;
    while(struct_declaration_list->type==STRUCT_DECLARATION_LIST)
        struct_declaration_list = struct_declaration_list->child;
    gtree *struct_declaration = struct_declaration_list;
    while(struct_declaration->type==STRUCT_DECLARATION)
    {
        gtree *specifier_qualifier_list = struct_declaration->child;
        gtree *struct_declarator_list = struct_declaration->child->next;
        string specifier = "";
        vector<var_type> declarators;

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
                bool pointer_flag;
                int arr_size;
                string id;
                getinfofromdeclarator(struct_declarator->child,pointer_flag,arr_size,id);
                var_type *v_type;
                v_type = new var_type;
                //now we just support dimension 1 or 0
                if(arr_size > 0)
                    v_type->init(specifier,id,pointer_flag,1,&arr_size);
                else
                    v_type->init(specifier,id,pointer_flag,0,&arr_size);
                var_list.push_back(v_type);
//                if(struct_declarator->child->child->type == POINTER){
//                    cout<<"error! pointer doesn't not support for now."<<endl;
//                    exit(-1);
//                }
//                else{
//                    declarators.push_back(struct_declarator->child->child->child->place);
//                }
            }
            else{
                cerr << "ERROR!struct_declarator should have declarator for now!"<<endl;
                exit(-1);
            }
            struct_declarator = struct_declarator->parent->next->next;
        }
//        for(unsigned int i=0;i<declarators.size();i++) {
//            if(declarators[i].get_pointer_flag())
//                sortname.push_back(make_pair(specifier, declarators[i]));
//        }
        struct_declaration = struct_declaration->parent->next;
    }
    return var_list;
}

void CPN::getDecl(gtree *tree){
    if (tree == NULL)return;
    else if(tree->type == STRUCT_OR_UNION_SPECIFIER)
    {
        if(tree->child->next->type == REMAIN || (tree->child->next->next != NULL &&
                                                 tree->child->next->next->type == REMAIN))
        {
            MSI m,m1,m2,m_int;
            m_int.tid = Integer;
            m_int.sid = 0;

            ProductSort pp,pp1,pp2,pp3,pp_base;
            gtree *struct_declare_list;
            string usrdefine_typename;
            vector<string> base_sortname,base_membername;
            vector<MSI> base_sortid;
            int base_sortnum;
            usrdefine_typename = tree->place;
            if(tree->child->next->type == REMAIN)
            {
//                usrdefine_typename = "";
                struct_declare_list = tree->child->next->next->child;
            }
            else {
//                usrdefine_typename = tree->child->next->place;
                struct_declare_list = tree->child->next->next->next->child;
            }

            vector<var_type*> var_list = process_struct_declaration_list(struct_declare_list);
            var_list.size();
            for(SORTID i;i<var_list.size();i++) {
                if(var_list[i]->get_pointer_flag())
                    base_sortid.push_back(m_int);
                else {

                    auto iter = map_build_in_type.find(var_list[i]->get_tag());
                    MSI tmp_msi;
                    if (iter == map_build_in_type.end()) {
                        auto uiter = sorttable.mapSort.find(var_list[i]->get_tag());
                        if (uiter == sorttable.mapSort.end()) {
                            cerr << "ERROR variable type when getDecl!" << endl;
                            exit(-1);
                        }
                        tmp_msi = uiter->second;
                    } else {
                        tmp_msi.tid = iter->second;
                        tmp_msi.sid = 0;
                    }

                    if (var_list[i]->get_dimension() == 0) {
                        base_sortid.push_back(tmp_msi);
                        base_membername.push_back(var_list[i]->get_name());
                        base_sortname.push_back(var_list[i]->get_tag());
                    }
                    else {
                        //now just support dimension 1
                        for(int j=0;j<var_list[i]->get_size_table()[0];j++) {
                            base_sortid.push_back(tmp_msi);
                            base_membername.push_back(var_list[i]->get_name() + to_string(j));
                            base_sortname.push_back(var_list[i]->get_tag());
                        }
                    }
                }
            }
            base_sortnum = base_sortid.size();

            //base_type var
            pp_base.id = usrdefine_typename;
            pp_base.mytype = productsort;
            pp_base.sortnum = base_sortnum;
            pp_base.sortid = base_sortid;
            pp_base.membername = base_membername;
            pp_base.sortname = base_sortname;

            sorttable.productsort.push_back(pp_base);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp_base.id,m));

            //usrdefine_type var
            pp.id = usrdefine_typename + var_suffix;
            pp.mytype = productsort;
            pp.sortnum = base_sortnum + 2;
            pp.sortid = base_sortid;
            pp.sortid.push_back(m_int);
            pp.sortid.push_back(m_int);
            pp.sortname = base_sortname;
            pp.sortname.push_back("id");
            pp.sortname.push_back("tid");
            pp.membername = base_membername;

            sorttable.productsort.push_back(pp);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp.id,m));

            //usrdefine_type arr
            pp1.id = usrdefine_typename + arr_suffix;
            pp1.mytype = productsort;
            pp1.sortnum = base_sortnum + 3;
            pp1.sortid = base_sortid;
            pp1.sortid.push_back(m_int);
            pp1.sortid.push_back(m_int);
            pp1.sortid.push_back(m_int);
            pp1.sortname = base_sortname;
            pp1.sortname.push_back("id");
            pp1.sortname.push_back("index");
            pp1.sortname.push_back("tid");
            pp1.membername = base_membername;

            sorttable.productsort.push_back(pp1);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp1.id,m));

            //usrdefine_type var global
            pp2.id = usrdefine_typename + var_suffix + global_suffix;
            pp2.mytype = productsort;
            pp2.sortnum = base_sortnum + 1;
            pp2.sortid = base_sortid;
            pp2.sortid.push_back(m_int);
//            pp.sortid.push_back(m_int);
            pp2.sortname = base_sortname;
            pp2.sortname.push_back("id");
//            pp.sortname.push_back("tid");
            pp2.membername = base_membername;

            sorttable.productsort.push_back(pp2);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp2.id,m));

            //usrdefine_type var
            pp3.id = usrdefine_typename + arr_suffix + global_suffix;
            pp3.mytype = productsort;
            pp3.sortnum = base_sortnum + 2;
            pp3.sortid = base_sortid;
            pp3.sortid.push_back(m_int);
            pp3.sortid.push_back(m_int);
            pp3.sortname = base_sortname;
            pp3.sortname.push_back("id");
            pp3.sortname.push_back("index");
            pp3.membername = base_membername;

            sorttable.productsort.push_back(pp3);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp3.id,m));

            for(unsigned int i=0;i<var_list.size();i++){
                delete var_list[i];
            }
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

void CPN::init_Place(string id, Tokens *token,type tid,SORTID sid) {
    map<string,index_t>::iterator iter = mapPlace.find(id);
    if(iter == mapPlace.end())
    {
        cout<<"can't find id in init_Place."<<endl;
        exit(-1);
    }
    CPlace *pp = &place[iter->second];
    pp->initMarking.clear();
    pp->initMarking.tid = tid;
    pp->initMarking.sid = sid;
    pp->initMarking.insert(token);
}

/*init_Place just support non-array normal type for now */
void CPN::init_Place(string id, string init_str){
    map<string,index_t>::iterator iter = mapPlace.find(id);
    if(iter == mapPlace.end())
    {
        cout<<"can't find id in init_Place."<<endl;
        exit(-1);
    }
    CPlace *pp = &place[iter->second];
//    pp->initMarking.clear();

    condition_tree tree;
    init_str = "1`" + init_str;
    tree.construct(init_str);
    MultiSet ms;


    if(pp->initMarking.tid == productsort){
        Product_t cid;
        cid = new_ProductColor(pp->initMarking.sid);
        type subtid = sorttable.productsort[pp->initMarking.sid].sortid[0].tid;
        if(subtid == Integer){
            ms.tid = Integer;
            ms.Exp2MS(this,tree.root,0,0,false);
            Integer_t subcid;
            ms.tokenQ->next->color->getColor(subcid);
            pp->initMarking.tokenQ->next->color->getColor(cid,pp->initMarking.sid);
            cid[0]->setColor(subcid);
            pp->initMarking.tokenQ->next->color->setColor(cid,pp->initMarking.sid);
        }
        else if(subtid == Real){
            ms.tid = Real;
            ms.Exp2MS(this,tree.root,0,0,false);
            Real_t subcid;
            ms.tokenQ->next->color->getColor(subcid);
            pp->initMarking.tokenQ->next->color->getColor(cid,pp->initMarking.sid);
            cid[0]->setColor(subcid);
            pp->initMarking.tokenQ->next->color->setColor(cid,pp->initMarking.sid);
        }
        else if(subtid == String){
            ms.tid = String;
            ms.Exp2MS(this,tree.root,0,0,false);
            String_t subcid;
            ms.tokenQ->next->color->getColor(subcid);
            pp->initMarking.tokenQ->next->color->getColor(cid,pp->initMarking.sid);
            cid[0]->setColor(subcid);
            pp->initMarking.tokenQ->next->color->setColor(cid,pp->initMarking.sid);
        }
        delete[] cid;
    }
    else{
        cerr<<"ERROR!Variable Places' Sort must be productsort!"<<endl;
        exit(-1);
    }

//    pp->initMarking.Exp2MS(this,tree.root,0,0);
}

bool is_Operator(string s)
{
    if (s == "+" || s == "-" || s == "*" || s == "/"
    || s == "&" || s == "|" || s == "^" || s == "%"
    || s == "!" || s == "@")
        return true;
    return false;
}

bool is_BoolOperator(string s)
{
    if (s == ">" || s == "<" || s == ">=" || s == "<=" || s == "==" || s == "!="||s == "&&")
        return true;
    else if(s == "||")
        return true;
    return false;
}

template<class T>
T operate(T s1,T s2,string Operator) {
    T v1, v2;
    v1 = s1;
    v2 = s2;
    if (Operator == "+")
        return v1 + v2;
    else if (Operator == "-")
        return v1 - v2;
    else if (Operator == "*")
        return v1 * v2;
    else if (Operator == "/") {
        if (v2 != 0)
            return v1 / v2;
        else {
            cerr << "divide 0 ERROR!" << endl;
            exit(-1);
        }
    } else if (Operator == "&")
        return int(v1) & int(v2);
    else if (Operator == "|")
        return int(v1) | int(v2);
    else if (Operator == "^")
        return int(v1) ^ int(v2);
    else if (Operator == "%")
        return int(v1) % int(v2);
    else if (Operator == "!")
        return !(int(v1));
    else if (Operator == "@")
        return -v1;
}

template<class T>
bool booloperate(T s1,T s2,string Operator) {
    T v1, v2;
    v1 = s1;
    v2 = s2;
    if (Operator == ">")
        return v1 > v2;
    else if (Operator == ">=")
        return v1 >= v2;
    else if (Operator == "<")
        return v1 < v2;
    else if (Operator == "<=")
        return v1 <= v2;
    else if (Operator == "==")
        return v1 == v2;
    else if (Operator == "!=")
        return v1 != v2;
    else if (Operator == "||")
        return v1 || v2;
    else if (Operator == "&&")
        return v1 && v2;

}

bool booloperate(string s1,string s2,string Operator){
    string v1=s1,v2=s2;
    if (Operator == ">")
        return v1 > v2;
    else if (Operator == ">=")
        return v1 >= v2;
    else if (Operator == "<")
        return v1 < v2;
    else if (Operator == "<=")
        return v1 <= v2;
    else if (Operator == "==")
        return v1 == v2;
    else if (Operator == "!=")
        return v1 != v2;
}

void color_copy(type tid,SORTID sid,SortValue *src,SortValue *des){
    if(!src)
        return;
    if(tid == productsort){
        Product_t cid;
        cid = new_ProductColor(sid);
//        int psnum = sorttable.productsort[sid].sortnum;
//        cid = new SortValue*[psnum];
        src->getColor(cid,sid);
        des->setColor(cid,sid);
        delete[] cid;
    }
    else if(tid == Integer){
        Integer_t cid;
        src->getColor(cid);
        des->setColor(cid);
    }
    else if(tid == Real){
        Real_t cid;
        src->getColor(cid);
        des->setColor(cid);
    }
    else if(tid == String){
        String_t cid;
        src->getColor(cid);
        des->setColor(cid);
    }
}

//*******translate_exp2arcexp*****************************//
//*******function:trans new tid automatically*************//
//*******now just support *,&,[]*************************//
string translate_exp2arcexp(CPN *cpn,string s,string base){
    int count;
    string tmp,result = "";
    int array_count = 0,parenthesis_count = 0;
    for(unsigned int i=0;i<s.size();i++){

        count = is_operator(s,i);
        if(s[i]=='[')
            array_count++;
        if(s[i]==']')
            array_count--;
        if(s[i]=='(')
            parenthesis_count++;

        if(count != 0 && array_count == 0 && parenthesis_count == 0 && !tmp.empty()){
            result.append(translate_exp2arcexp(cpn,tmp,base));
            result.append(s.substr(i,count));
            tmp.clear();
            i += count-1;
        }
        else if(s[i] == '$' || s[i] == ':'){
            result.append(tmp);
            result = result + s[i];
            tmp.clear();
        }
        else
            tmp = tmp + s[i];

        if(s[i]==')')
            parenthesis_count--;
    }
    if(tmp[0]=='(' && tmp[tmp.size()-1]==')') {
        tmp = tmp.substr(1, tmp.size() - 2);
        result.append(translate_exp2arcexp(cpn,tmp,base));
        return result;
    }

    if(tmp[0] == '*'){
        // pointer gets value
        string pointer = tmp.substr(1);
        result = result + CaseFlag + translate_exp2arcexp(cpn,pointer,base);

//        for(auto iter = map_address.begin();iter!=map_address.end();iter++){
//            result.append(":");
//            result.append(iter->first);
//            result.append("=>");
//            result.append(iter->second);
//        }
        for(auto iter = place_size.begin();iter != place_size.end();iter++){
            auto piter = cpn->mapPlace.find(iter->first);
            if(piter == cpn->mapPlace.end()){
                cerr << "ERROR!can't find place!"<<endl;
                exit(-1);
            }
            if(iter->second > 1){
                for(unsigned int i=0;i<iter->second;i++){
                    result.append(":");
                    result.append(cpn->place[piter->second].expression + to_string(i) + id_suffix);
                    result.append("=>");
                    result.append(cpn->place[piter->second].expression + to_string(i));
                }
            }
            else{
                result.append(":");
                result.append(cpn->place[piter->second].expression + id_suffix);
                result.append("=>");
                result.append(cpn->place[piter->second].expression);
            }
        }
        result.append(":default;");
    }
    else if(tmp[0] == '&'){
        //assume & just for variable or arr[exp]

        string var = tmp.substr(1);
        int arr_pos = var.find('[');
        if(arr_pos != string::npos){
            string id,index;
            int id_start;
            for (id_start = arr_pos - 1; id_start >= 0; id_start--)
                if (isdigit(tmp[id_start]) || isalpha(tmp[id_start]) || tmp[id_start] == '_');
                else
                    break;

            if(id_start!=-1){
                cerr<<"translate_exp error occur!"<<endl;
                exit(-1);
            }
            id = var.substr(id_start + 1, arr_pos - id_start - 1);
            string tmp_P = find_P_name(id,base);
            string real_id = id;
            if(tmp_P != pthread_P) {
                auto iter = cpn->mapPlace.find(tmp_P);
                if (iter == cpn->mapPlace.end()) {
                    cerr << "can't find tmp_P in trans_exp2arcexp!" << endl;
                    exit(-1);
                }
                real_id = cpn->place[iter->second].expression;
            }
            index = var.substr(arr_pos + 1, var.size() - arr_pos - 2);
            result += real_id + to_string(0) + id_suffix + "+" + translate_exp2arcexp(cpn,index,base);
        }
        else{
            result += var + id_suffix;
        }
    }
    else {
        int arr_pos = tmp.find('[');
        string id, index;

        if (arr_pos != string::npos) {
            int id_start;
            for (id_start = arr_pos - 1; id_start >= 0; id_start--)
                if (isdigit(tmp[id_start]) || isalpha(tmp[id_start]) || tmp[id_start] == '_');
                else
                    break;

            if(id_start!=-1){
                cerr<<"translate_exp error occur!"<<endl;
                exit(-1);
            }
            id = tmp.substr(id_start + 1, arr_pos - id_start - 1);
            index = tmp.substr(arr_pos + 1, tmp.size() - arr_pos - 2);
            bool is_pointer = find_v_ispointer(id, base);
            int size = find_v_size(id, base);

            string tmp_P = find_P_name(id,base);


            if (!is_pointer) {
                string real_id = id;
                if(tmp_P != pthread_P) {
                    auto iter = cpn->mapPlace.find(tmp_P);
                    if (iter == cpn->mapPlace.end()) {
                        cerr << "can't find tmp_P in trans_exp2arcexp!" << endl;
                        exit(-1);
                    }
                    real_id = cpn->place[iter->second].expression;
                }
                result = result + CaseFlag + translate_exp2arcexp(cpn,index, base);
                for (int i = 0; i < size; i++) {
                    result.append(":");
                    result.append(to_string(i));
                    result.append("=>");
                    result.append(real_id + to_string(i));
                }
                result = result + ":default;";
            } else {
                //pointer

                result.append(translate_exp2arcexp(cpn,"*(" + id + "+" + index + ")",base));
            }

        } else {
            string id = tmp;
            if(tmp[0] == '_' || isalpha(tmp[0])) {
                int size;
                string real_id = id;
                if(tmp.length()>return_suffix.length() &&
                   tmp.substr(tmp.length()-return_suffix.length()) == return_suffix) {
                    //Here we assume that function call's related place's will not repeat;
                    //remain improving
                    size = 1;
                }
                else {
                    if(id == "case") {
                        real_id = id;
                        size = 0;
                    }
                    else {
                        size = find_v_size(id, base);
                        string tmp_P = find_P_name(id, base);

                        if (tmp_P != pthread_P) {
                            auto iter = cpn->mapPlace.find(tmp_P);
                            if (iter == cpn->mapPlace.end()) {
                                cerr << "can't find tmp_P in trans_exp2arcexp!" << endl;
                                exit(-1);
                            }
                            real_id = cpn->place[iter->second].expression;
                        }
                    }
                }
                if (size > 1)
                    result += real_id + to_string(0) + id_suffix;
                else
                    result += real_id;
            }
            else
                result += id;
        }
    }
    return result;
}

//*******translate_exp2tid******************//
//*******function:trans new tid*************//
//*******now just support tid or tid[index]*//
string translate_exp2tid(CPN *cpn,string s, string base){
    string id,index,result;
    int arr_pos = s.find('[');
    if(arr_pos != string::npos) {
        id = s.substr(0, arr_pos);
        index = s.substr(arr_pos + 1, s.size() - arr_pos - 2);
        int size = find_v_size(id, base);
        string tmp_P = find_P_name(id,base);
        if(tmp_P != pthread_P) {
            auto iter = cpn->mapPlace.find(tmp_P);
            if (iter == cpn->mapPlace.end()) {
                cerr << "can't find tmp_P in trans_exp2tid!" << endl;
                exit(-1);
            }
            id = cpn->place[iter->second].expression;
        }
        string tmp_P1 = find_P_name(index,base);
        if(tmp_P1 != pthread_P) {
            auto iter = cpn->mapPlace.find(tmp_P1);
            if (iter == cpn->mapPlace.end()) {
                cerr << "can't find tmp_P in trans_exp2tid!" << endl;
                exit(-1);
            }
            index = cpn->place[iter->second].expression;
        }
        result = result + CaseFlag + index;
        for (int i = 0; i < size; i++) {
            result.append(":");
            result.append(to_string(i));
            result.append("=>");
            result.append("\"" + id + "_" + to_string(i) + "\"");
        }
        result = result + ":default;";
    }
    else {
        id = s;
        string tmp_P = find_P_name(id,base);
        if(tmp_P != pthread_P) {
            auto iter = cpn->mapPlace.find(tmp_P);
            if (iter == cpn->mapPlace.end()) {
                cerr << "can't find tmp_P in trans_exp2tid!" << endl;
                exit(-1);
            }
            id = cpn->place[iter->second].expression;
        }
        result = "\"" + id + "\"";
    }
    return result;
}

//string translate_exp2renameexp(CPN *cpn,string s,string base){
//    string tmp,tmp_P,result;
//    for(unsigned int i=0;i<s.size();i++){
//        if(isdigit(s[i]) || isalpha(s[i]) || s[i]=='_')
//            tmp = tmp + s[i];
//        else{
//            tmp_P = find_P_name(tmp,base);
//            tmp.clear();
//            auto iter = cpn->mapPlace.find(tmp_P);
//            result += cpn->place[iter->second].expression;
//            result = result + s[i];
//        }
//    }
//    result += tmp;
//    return result;
//}

void MultiSet::Exp2MS(CPN *cpn,condition_tree_node *tree,unsigned short down,unsigned short up,bool in_bool_op) {
    MultiSet lms,rms;
    lms.sid=rms.sid=sid;
    lms.tid=rms.tid=tid;

    vector<mapsort_info> sortid;

    if(tree == NULL)
        return;
    if(tree->node_type == TokenOperator){
        lms.Exp2MS(cpn,tree->left,down,up,in_bool_op);
        rms.Exp2MS(cpn,tree->right,down,up,in_bool_op);
        if(tree->node_name == "++") {
            PLUS(lms);
            PLUS(rms);
        }
        else if(tree->node_name == "--"){
            PLUS(lms);

            //negative PLUS
            Tokens *tokens = rms.tokenQ->next;
            while(tokens){
                tokens->tokencount = - tokens->tokencount;
                tokens = tokens->next;
            }
            PLUS(rms);
        }
        else{
            cerr<<"ERROR!We just support '++' and '--' for now!"<<endl;
            exit(-1);
        }
        //to avoid 0 tokencount
        Tokens *tokens = tokenQ;
        while(tokens->next){
            if(tokens->next->tokencount == 0){
                Tokens *tmp = tokens->next;
                tokens->next = tokens->next->next;

                delete tmp;
            }
            tokens = tokens->next;
            if(!tokens)
                break;
        }
    }
    else if(tree->node_type == Token){
        lms.Exp2MS(cpn,tree->left,down,up,in_bool_op);
        lms.tokenQ->next->tokencount = tree->num;
        PLUS(lms);

    }
    else if(tree->node_type == Tuple){
        lms.Exp2MS(cpn,tree->left,down,down,in_bool_op);
        rms.Exp2MS(cpn,tree->right,down+1,up,in_bool_op);

        Tokens *lToken = lms.tokenQ->next,*rToken = rms.tokenQ->next;
        if(tid == productsort){
            Product_t cid1,cid2,cid;
//            int psnum = sorttable.productsort[sid].sortnum;
            cid1 = new_ProductColor(sid);
            cid2 = new_ProductColor(sid);
            cid = new_ProductColor(sid);

//            lToken->color = new ProductSortValue(sid);
//            rToken->color = new ProductSortValue(sid);
            lToken->color->getColor(cid1,sid);
            rToken->color->getColor(cid2,sid);

            color_copy(sorttable.productsort[sid].sortid[down].tid,sorttable.productsort[sid].sortid[down].sid,cid1[down],cid[down]);
            for(unsigned int i=down+1;i<=up;i++)
                color_copy(sorttable.productsort[sid].sortid[i].tid,sorttable.productsort[sid].sortid[down].sid,cid2[i],cid[i]);

            Tokens *tokens = new Tokens;
            tokens->color = new ProductSortValue(sid);
            tokens->color->setColor(cid,sid);
            insert(tokens);

            delete[] cid;
            delete[] cid1;
            delete[] cid2;
        }
        else{
            cerr<<"ERROR!Tuple can not occur in tid that is not productsort!"<<endl;
            exit(-1);
        }
    }
    else if(tree->node_type == CaseOperator){
        MultiSet cms;
        lms.Exp2MS(cpn,tree->left,down,up,in_bool_op);
        if(tree->right)
            rms.Exp2MS(cpn,tree->right,down,up,in_bool_op);
        cms.tid = Integer;
        cms.sid = 0;
        cms.Exp2MS(cpn,tree->condition,0,0,false);

        Integer_t result;
        cms.tokenQ->next->color->getColor(result);

        if(result != 0) {
            if(in_bool_op){
                tid = lms.tid;
                sid = lms.sid;
            }
            PLUS(lms);
        }
        else {
            if(in_bool_op){
                tid = rms.tid;
                sid = rms.tid;
            }
            PLUS(rms);
        }
        cms.clear();
    }
    else if(tree->node_type == Operator){
        if(down!=up){
            cerr<<"ERROR!Operator's down and up must be consistent!"<<endl;
            exit(-1);
        }
        type tmp_tid;
        if(tid == productsort)
            tmp_tid = sorttable.productsort[sid].sortid[down].tid;
        else
            tmp_tid = tid;

        if(is_BoolOperator(tree->node_name) && tmp_tid == Integer){
            lms.Exp2MS(cpn,tree->left,down,up,true);
            if(tree->right)
                rms.Exp2MS(cpn,tree->right,down,up,true);
            Integer_t sub_cid;
            if(lms.tid == String){
                String_t cid1,cid2;

                lms.tokenQ->next->color->getColor(cid1);
                rms.tokenQ->next->color->getColor(cid2);
                if (booloperate(cid1, cid2, tree->node_name))
                    sub_cid = 1;
                else
                    sub_cid = 0;
            }
            else if(lms.tid == Real){
                Real_t cid1,cid2;

                lms.tokenQ->next->color->getColor(cid1);
                rms.tokenQ->next->color->getColor(cid2);
                if (booloperate(cid1, cid2, tree->node_name))
                    sub_cid = 1;
                else
                    sub_cid = 0;
            }
            else if(lms.tid == Integer){
                Integer_t cid1,cid2;

                lms.tokenQ->next->color->getColor(cid1);
                rms.tokenQ->next->color->getColor(cid2);
                if (booloperate(cid1, cid2, tree->node_name))
                    sub_cid = 1;
                else
                    sub_cid = 0;
            }
            else{
                Product_t cid1,cid2;
                cid1 = new_ProductColor(sid);
                cid2 = new_ProductColor(sid);

                lms.tokenQ->next->color->getColor(cid1,sid);
                rms.tokenQ->next->color->getColor(cid2,sid);

                Integer_t ccid1,ccid2;
                cid1[down]->getColor(ccid1);
                cid2[down]->getColor(ccid2);
                if (booloperate(ccid1, ccid2, tree->node_name))
                    sub_cid = 1;
                else
                    sub_cid = 0;
            }
            if(tid == productsort && !in_bool_op){
                Product_t cid;
                cid = new_ProductColor(sid);
                cid[down]->setColor(sub_cid);
                Tokens *token = new Tokens;
                token->color = new ProductSortValue(sid);
                token->color->setColor(cid, sid);
                if(in_bool_op && !is_BoolOperator(tree->node_name)){
                    tid = lms.tid;
                    sid = lms.sid;
                }
                else if(in_bool_op){
                    tid = Integer;
                    sid = 0;
                }
                insert(token);
                delete[] cid;
            }
            else{
                Integer_t cid;
                cid = sub_cid;
                Tokens *token = new Tokens;
                token->color = new IntegerSortValue;
                token->color->setColor(cid);
                if(in_bool_op && !is_BoolOperator(tree->node_name)){
                    tid = lms.tid;
                    sid = lms.sid;
                }
                else if(in_bool_op){
                    tid = Integer;
                    sid = 0;
                }
                insert(token);
            }

        }
        else if(is_BoolOperator(tree->node_name)){
            cerr << "ERROR!we assume boolop just appears in Integer exp!";
            exit(-1);
        }
        else {
            if(in_bool_op){
                lms.Exp2MS(cpn,tree->left,down,up,in_bool_op);
                if(tree->right)
                    rms.Exp2MS(cpn,tree->right,down,up,in_bool_op);
                if(lms.tid == Integer){
                    Integer_t cid1, cid2, cid;
                    Tokens *ltoken = lms.tokenQ->next, *rtoken;
                    if (tree->right)
                        rtoken = rms.tokenQ->next;

                    ltoken->color->getColor(cid1);
                    if (tree->right)
                        rtoken->color->getColor(cid2);

                    if (is_Operator(tree->node_name))
                        cid = operate(cid1, cid2, tree->node_name);
                    Tokens *token = new Tokens;
                    token->color = new IntegerSortValue;
                    token->color->setColor(cid);
                    if(in_bool_op && !is_BoolOperator(tree->node_name)){
                        tid = lms.tid;
                        sid = lms.sid;
                    }
                    else if(in_bool_op){
                        tid = Integer;
                        sid = 0;
                    }
                    insert(token);
                }
                else if(lms.tid == Real){
                    Real_t cid1, cid2, cid;
                    Tokens *ltoken = lms.tokenQ->next, *rtoken;
                    if (tree->right)
                        rtoken = rms.tokenQ->next;

                    ltoken->color->getColor(cid1);
                    if (tree->right)
                        rtoken->color->getColor(cid2);

                    if (is_Operator(tree->node_name))
                        cid = operate(cid1, cid2, tree->node_name);
                    Tokens *token = new Tokens;
                    token->color = new RealSortValue;
                    token->color->setColor(cid);
                    if(in_bool_op && !is_BoolOperator(tree->node_name)){
                        tid = lms.tid;
                        sid = lms.sid;
                    }
                    else if(in_bool_op){
                        tid = Integer;
                        sid = 0;
                    }
                    insert(token);
                }
            }
            else {
                if (tid == productsort) {
                    lms.Exp2MS(cpn, tree->left, down, up, in_bool_op);
                    if (tree->right)
                        rms.Exp2MS(cpn, tree->right, down, up, in_bool_op);
                    Product_t cid1, cid2, cid;
//            int psnum = sorttable.productsort[sid].sortnum;
                    cid1 = new_ProductColor(sid);
                    if (tree->right)
                        cid2 = new_ProductColor(sid);
                    cid = new_ProductColor(sid);

                    Tokens *ltoken = lms.tokenQ->next, *rtoken;
                    if (tree->right)
                        rtoken = rms.tokenQ->next;

                    ltoken->color->getColor(cid1, sid);
                    if (tree->right)
                        rtoken->color->getColor(cid2, sid);

                    type t = sorttable.productsort[sid].sortid[down].tid;
                    if (t == Integer) {
                        Integer_t sub_col1, sub_col2, sub_col;
                        cid1[down]->getColor(sub_col1);
                        if (tree->right)
                            cid2[down]->getColor(sub_col2);
                        if (is_Operator(tree->node_name))
                            sub_col = operate(sub_col1, sub_col2, tree->node_name);
//                else if(is_BoolOperator(tree->node_name)) {
//                    if (booloperate(sub_col1, sub_col2, tree->node_name))
//                        sub_col = 1;
//                    else
//                        sub_col = 0;
//                }
                        cid[down]->setColor(sub_col);
                    } else if (t == Real) {
                        Real_t sub_col1, sub_col2, sub_col;
                        cid1[down]->getColor(sub_col1);
                        if (tree->right)
                            cid2[down]->getColor(sub_col2);
                        if (is_Operator(tree->node_name))
                            sub_col = operate(sub_col1, sub_col2, tree->node_name);
//                else if(is_BoolOperator(tree->node_name)) {
//                    if (booloperate(sub_col1, sub_col2, tree->node_name))
//                        sub_col = 1;
//                    else
//                        sub_col = 0;
//                }
                        cid[down]->setColor(sub_col);
                    } else if (t == String) {
//                if(is_Operator(tree->node_name)) {
                        cerr << "ERROR!String can not operate!" << endl;
                        exit(-1);
//                }
//                else if(is_BoolOperator(tree->node_name)){
//                    String_t sub_col1,sub_col2;
//
//                    cid1[down]->getColor(sub_col1);
//                    if(tree->right)
//                        cid2[down]->getColor(sub_col2);
//                    if (booloperate(sub_col1, sub_col2, tree->node_name))
//                        sub_col = 1;
//                    else
//                        sub_col = 0;
//                }
                    }
                    Tokens *token = new Tokens;
                    token->color = new ProductSortValue(sid);
                    token->color->setColor(cid, sid);
                    insert(token);

                    delete[] cid1;
                    if (tree->right)
                        delete[] cid2;
                    delete[] cid;
                } else if (tid == Integer) {
                    lms.Exp2MS(cpn, tree->left, down, up, in_bool_op);
                    if (tree->right)
                        rms.Exp2MS(cpn, tree->right, down, up, in_bool_op);

                    Integer_t cid1, cid2, cid;
                    Tokens *ltoken = lms.tokenQ->next, *rtoken;
                    if (tree->right)
                        rtoken = rms.tokenQ->next;

                    ltoken->color->getColor(cid1);
                    if (tree->right)
                        rtoken->color->getColor(cid2);

                    if (is_Operator(tree->node_name))
                        cid = operate(cid1, cid2, tree->node_name);
//            else if(is_BoolOperator(tree->node_name)) {
//                if (booloperate(cid1, cid2, tree->node_name))
//                    cid = 1;
//                else
//                    cid = 0;
//            }
                    Tokens *token = new Tokens;
                    token->color = new IntegerSortValue;
                    token->color->setColor(cid);
                    insert(token);
                } else if (tid == Real) {
                    lms.Exp2MS(cpn, tree->left, down, up, in_bool_op);
                    if (tree->right)
                        rms.Exp2MS(cpn, tree->right, down, up, in_bool_op);
                    Real_t cid1, cid2, cid;
                    Tokens *ltoken = lms.tokenQ->next, *rtoken;
                    if (tree->right)
                        rtoken = rms.tokenQ->next;

                    ltoken->color->getColor(cid1);
                    if (tree->right)
                        rtoken->color->getColor(cid2);

                    if (is_Operator(tree->node_name))
                        cid = operate(cid1, cid2, tree->node_name);
//            else if(is_BoolOperator(tree->node_name)) {
//                if (booloperate(cid1, cid2, tree->node_name))
//                    cid = 1;
//                else
//                    cid = 0;
//            }
                    Tokens *token = new Tokens;
                    token->color = new RealSortValue;
                    token->color->setColor(cid);
                    insert(token);
                } else if (tid == String) {
                    cerr << "ERROR!String can not operate!" << endl;
                    exit(-1);
                } else {
                    cerr << "TYPE ERROR!" << endl;
                    exit(-1);
                }
            }
        }

    }
    else if(tree->node_type == color){
        if(down!=up){
            cerr<<"ERROR!color's down and up must consistent!"<<endl;
            exit(-1);
        }
        if(in_bool_op){
            if(tree->value[0]=='\'') {
                String_t cid;
                cid = tree->value;
                Tokens *token = new Tokens;
                token->color = new StringSortValue;
                token->color->setColor(cid);
                tid = String;
                sid = 0;
                insert(token);

            }
            else if(tree->value.find('.')==string::npos) {
                Integer_t cid;
                cid = atoi(tree->value.c_str());
                Tokens *token = new Tokens;
                token->color = new IntegerSortValue;
                token->color->setColor(cid);
                tid = Integer;
                sid = 0;
                insert(token);

            }
            else {
                Real_t cid;
                cid = atof(tree->value.c_str());
                Tokens *token = new Tokens;
                token->color = new RealSortValue;
                token->color->setColor(cid);
                tid = Real;
                sid = 0;
                insert(token);

            }
        }
        else {
            if (tid == productsort) {

                Product_t cid;
//            int psnum = sorttable.productsort[sid].sortnum;
                cid = new_ProductColor(sid);
                if (sorttable.productsort[sid].sortid[down].tid == Integer) {
                    Integer_t subcolor;
                    subcolor = atoi(tree->value.c_str());
                    cid[down]->setColor(subcolor);
                    Tokens *token = new Tokens;
                    token->color = new ProductSortValue(sid);
                    token->color->setColor(cid, sid);
                    insert(token);
                } else if (sorttable.productsort[sid].sortid[down].tid == Real) {
                    Real_t subcolor;
                    subcolor = atof(tree->value.c_str());
                    cid[down]->setColor(subcolor);
                    Tokens *token = new Tokens;
                    token->color = new ProductSortValue(sid);
                    token->color->setColor(cid, sid);
                    insert(token);
                } else if (sorttable.productsort[sid].sortid[down].tid == String) {
                    String_t subcolor;
                    subcolor = tree->value;
                    cid[down]->setColor(subcolor);
                    Tokens *token = new Tokens;
                    token->color = new ProductSortValue(sid);
                    token->color->setColor(cid, sid);
                    insert(token);
                } else {
                    cerr << "TYPE ERROR!" << endl;
                    exit(-1);
                }
                delete[] cid;
            } else if (tid == Integer) {
                //Integer is more complicated ,because it includes bool type

                if (tree->value[0] == '\'') {
                    String_t cid;
                    cid = tree->value;
                    Tokens *token = new Tokens;
                    token->color = new StringSortValue;
                    token->color->setColor(cid);
                    insert(token);
                } else if (tree->value.find('.') == string::npos) {
                    Integer_t cid;
                    cid = atoi(tree->value.c_str());
                    Tokens *token = new Tokens;
                    token->color = new IntegerSortValue;
                    token->color->setColor(cid);
                    insert(token);
                } else {
                    Real_t cid;
                    cid = atof(tree->value.c_str());
                    Tokens *token = new Tokens;
                    token->color = new RealSortValue;
                    token->color->setColor(cid);
                    insert(token);
                }
            } else if (tid == Real) {
                Real_t cid;
                cid = atof(tree->value.c_str());
                Tokens *token = new Tokens;
                token->color = new RealSortValue;
                token->color->setColor(cid);
                insert(token);
            } else if (tid == String) {
                String_t cid;
                cid = tree->value;
                Tokens *token = new Tokens;
                token->color = new StringSortValue;
                token->color->setColor(cid);
                insert(token);
            } else if (tid == dot) {
                Tokens *token = new Tokens;
                insert(token);
            } else {
                cerr << "TYPE ERROR!" << endl;
                exit(-1);
            }
        }
    }
    else if(tree->node_type == variable){
        if(down!=up){
            cerr<<"ERROR!color's down and up must consistent!"<<endl;
            exit(-1);
        }
        auto iter = cpn->mapVariable.find(tree->node_name);
        if(iter == cpn->mapVariable.end()){
            cerr << "ERROR!can't find variable!"<<endl;
            exit(-1);
        }
        Variable *v = &cpn->vartable[iter->second];
        if(in_bool_op){
            if(v->tid == String) {
                String_t cid;
                v->value->getColor(cid);
                Tokens *token = new Tokens;
                token->color = new StringSortValue;
                token->color->setColor(cid);
                tid = String;
                sid = 0;
                insert(token);

            }
            else if(v->tid == Integer) {
                Integer_t cid;
                v->value->getColor(cid);
                Tokens *token = new Tokens;
                token->color = new IntegerSortValue;
                token->color->setColor(cid);
                tid = Integer;
                sid = 0;
                insert(token);

            }
            else {
                Real_t cid;
                v->value->getColor(cid);
                Tokens *token = new Tokens;
                token->color = new RealSortValue;
                token->color->setColor(cid);
                tid = Real;
                sid = 0;
                insert(token);

            }
        }
        else {
            if (tid == productsort) {
                Product_t cid;
//            int psnum = sorttable.productsort[sid].sortnum;
                cid = new_ProductColor(sid);
                if (sorttable.productsort[sid].sortid[down].tid == Integer) {
                    if (v->tid == Integer) {
                        Integer_t subcolor;
                        v->value->getColor(subcolor);
                        cid[down]->setColor(subcolor);
                        Tokens *token = new Tokens;
                        token->color = new ProductSortValue(sid);
                        token->color->setColor(cid, sid);
                        insert(token);
                    } else if (v->tid == Real) {
                        Real_t subcolor;
                        v->value->getColor(subcolor);
                        cid[down]->setColor(subcolor);
                        Tokens *token = new Tokens;
                        token->color = new ProductSortValue(sid);
                        token->color->setColor(cid, sid);
                        insert(token);
                    } else {
                        String_t subcolor;
                        v->value->getColor(subcolor);
                        cid[down]->setColor(subcolor);
                        Tokens *token = new Tokens;
                        token->color = new ProductSortValue(sid);
                        token->color->setColor(cid, sid);
                        insert(token);
                    }
                } else if (sorttable.productsort[sid].sortid[down].tid == Real) {
                    Real_t subcolor;
                    v->value->getColor(subcolor);
                    cid[down]->setColor(subcolor);
                    Tokens *token = new Tokens;
                    token->color = new ProductSortValue(sid);
                    token->color->setColor(cid, sid);
                    insert(token);
                } else if (sorttable.productsort[sid].sortid[down].tid == String) {
                    String_t subcolor;
                    v->value->getColor(subcolor);
                    cid[down]->setColor(subcolor);
                    Tokens *token = new Tokens;
                    token->color = new ProductSortValue(sid);
                    token->color->setColor(cid, sid);
                    insert(token);
                } else {
                    cerr << "TYPE ERROR!" << endl;
                    exit(-1);
                }
                delete[] cid;
            } else if (tid == Integer) {
                Integer_t cid;
                v->value->getColor(cid);
                Tokens *token = new Tokens;
                token->color = new IntegerSortValue;
                token->color->setColor(cid);
                insert(token);
            } else if (tid == Real) {
                Real_t cid;
                v->value->getColor(cid);
                Tokens *token = new Tokens;
                token->color = new RealSortValue;
                token->color->setColor(cid);
                insert(token);
            } else if (tid == String) {
                String_t cid;
                v->value->getColor(cid);
                Tokens *token = new Tokens;
                token->color = new StringSortValue;
                token->color->setColor(cid);
                insert(token);
            } else if (tid == dot) {
                Tokens *token = new Tokens;
                insert(token);
            } else {
                cerr << "TYPE ERROR!" << endl;
                exit(-1);
            }
        }
    }

    //release memory
    if(tree->left)
        lms.clear();
    if(tree->right)
        lms.clear();
}

void CPN::process_declarator(gtree *declarator, string tag, string base, bool para)
{
    bool call_declare_flag = false;//true when declarator is function declaration
    bool init_flag = false;//true when has init
    bool array_flag = false;
    int array_size = 0;
    bool point_flag = false;
    string init_str;
//    string init_value;
    gtree *direct_declarator,*identifier;
    string exp;
    MultiSet init_ms;


    if (declarator->child->next == NULL)
        direct_declarator = declarator->child;
    else
        direct_declarator = declarator->child->next;

    while(direct_declarator->type == DIRECT_DECLARATOR){
        if(direct_declarator->child)
            direct_declarator = direct_declarator->child;
        else
            break;
    }
    if(direct_declarator && direct_declarator->place == "("){
        cerr<<"ERROR!we don't support direct_declaration begin with '('"<<endl;
        exit(-1);
    }
    identifier = direct_declarator;
    if(identifier == NULL)
        exp = declarator->place;
    else {
        exp = identifier->place;
    }
    if (declarator->next == NULL || declarator->next->type != REMAIN || declarator->next->place != "=")//无初始化
        init_flag = false;
    else//now we just support initializing for non-array normal type
    {
        init_flag = true;
        gtree *init = declarator->next->next;

        if (init->child->type == ASSIGNMENT_EXPRESSION)
            init_str = init->child->place;
        else
        {
            cout << "暂不支持数组初始化!" << endl;
            exit(-1);
        }

    }
    if (declarator->child->type == POINTER)
        point_flag = true;
    else
        point_flag = false;

    if (identifier->parent->next != NULL && identifier->parent->next->type == REMAIN && identifier->parent->next->place == "[")//数组
    {
        array_flag = true;
        if (identifier->parent->next->next->type == CONSTANT_EXPRESSION && !para) {
            MultiSet ms;
            ms.tid = Integer;
            ms.sid = 0;
            condition_tree *tmp_tree = new condition_tree;
            tmp_tree->construct("1`" + identifier->parent->next->next->child->place);
            ms.Exp2MS(this,tmp_tree->root,0,0,false);
            ms.tokenQ->next->color->getColor(array_size);
        }
        else if(identifier->parent->next->next->type == CONSTANT_EXPRESSION && para)
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
    if (!call_declare_flag) {
        bool isglobal;
        if (!para) {
            if (base == global_table_name)
                isglobal = true;
            else
                isglobal = false;
        } else
            isglobal = false;

        string _P;
        if (tag != "pthread_t" || point_flag) {
            //Add variable place
            if(tag == "pthread_t")
                array_size = thread_max;
            if (array_flag && para)
                point_flag = true;

            _P = gen_P();
            if (point_flag)
                tag = "int";//ID is a type of int

            Add_Place(_P, tag, array_size, false, exp, isglobal,point_flag);

            if (init_flag) {
//                Tokens *token = new Tokens;
//                if (iter->second == Integer) {
//                    token->init_a_token(iter->second, atoi(init_value.c_str()));
//                } else if (iter->second == Real) {
//                    token->init_a_token(iter->second, atof(init_value.c_str()));
//                } else if (iter->second == String) {
//                    token->init_a_token(iter->second, init_value);
//                }
                auto siter = sorttable.mapSort.find(tag);
                if (siter == sorttable.mapSort.end()) {
                    cout << "can't find tag in mapSort" << endl;
                    exit(-1);
                }
//                token->color->setColor()
                if (array_flag) {
                    cerr << "ERROR!Do not support initializing for array for now!" << endl;
                    exit(-1);
                }

                init_Place(_P, init_str);
            }
        }
        else
            _P = pthread_P;

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
                table_name = global_table_name;
            else
                table_name = compound->place;
            for (unsigned int i = 0; i < v_tables.size(); i++) {
                if (table_name == v_tables[i]->name) {
                    v_tables[i]->insert(identifier->place, _P, array_size, point_flag);
                    break;
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

void CPN::Add_returns(string p_name, string return_T, string exp,string base) {
    auto iter = mapPlace.find(p_name);
    Triple tmp;
    tmp.first = return_T;
    tmp.second = exp;
    tmp.third = base;
    if(iter!=mapPlace.end())
        place[iter->second].returns.push_back(tmp);
    else
    {
        cout<<"error in Add_returns"<<endl;
        exit(-1);
    }
}

//void CPN::set_ispointer(string p_name,bool ispointer) {
//    auto iter = mapPlace.find(p_name);
//    if(iter!=mapPlace.end())
//        place[iter->second].is_pointer = ispointer;
//    else
//    {
//        cout<<"error in set_ispointer"<<endl;
//        exit(-1);
//    }
//}

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

vector<Triple> CPN::get_returns(string p_name) {
    auto iter = mapPlace.find(p_name);
    if(iter!=mapPlace.end())
        return place[iter->second].returns;
    else
    {
        cout<<"error in get_returns"<<endl;
        exit(-1);
    }
}

//bool CPN::get_ispointer(string p_name) {
//    auto iter = mapPlace.find(p_name);
//    if(iter!=mapPlace.end())
//        return place[iter->second].is_pointer;
//    else
//    {
//        cout<<"error in get_ispointer"<<endl;
//        exit(-1);
//    }
//}

vector<string> extract_paras(gtree *p)
{
    //p must be a postfix_expression
    vector<string> res;
    gtree *tmp = p;
    while(tmp && tmp->type!=PRIMARY_EXPRESSION)
        tmp = tmp->child;
    if(!tmp->parent->next || tmp->parent->next->place!="(")
    {
        cout<<"extract_paras's para must be a call_postfix!"<<endl;
        exit(-1);
    }
    gtree *argument_list = tmp->parent->next->next,*assignment;
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
    return res;
}

void CPN::create_v_table(gtree *p){
    V_Table *table = new V_Table(p->place);
    v_tables.push_back(table);
    gtree *up = p->parent;

    while (up != NULL && up->type != COMPOUND_STATEMENT)
        up = up->parent;
    if(p->para.size()!=0) {
        for (unsigned int i = 0; i < p->para.size(); i++)
            table->insert(p->para[i].first, p->para[i].second,1,true);
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
    else
        func = global_table_name;
    if(p->child->type == DECLARATION_SPECIFIERS && p->child->child->type == STORAGE_CLASS_SPECIFIER && p->child->child->child->type == TYPEDEF)
        return;
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
        gtree *com = p;
        while(com->type!=COMPOUND_STATEMENT)
            com = com->parent;
        string base = com->place;
        statement->matched_P = control_P;
        Add_Place(control_P,controlflag,0,true,p->place,true,false);
        string control_T1 = gen_T(),control_T2 = gen_T();

        //guard is added in backward traverse
        Add_Transition(control_T1,"",base);
        Add_Transition(control_T2,"",base);
        Add_Arc(control_P,control_T1,construct_arcexpstr(tid_str,"","",""),true,control);
        Add_Arc(control_P,control_T2,construct_arcexpstr(tid_str,"","",""),true,control);

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
        Add_Place(control_P,controlflag,0,true,p->place,true,false);
        string control_T = gen_T();
        Add_Transition(control_T,"",p->place);
        Add_Arc(control_P,control_T,construct_arcexpstr(tid_str,"","",""),true,control);

        //set exit,enter
        vector<string> enter,exit_T;
        enter.push_back(control_T);
        exit_T.push_back(control_T);//compound_statement's exit_T will clear this
        //enter_P.push_back(control_P);
        set_enter_T(control_P,enter);
        set_exit_T(control_P,exit_T);

        if(judge_return(statement)){
            auto tier = mapTransition.find(control_T);
            if(tier == mapTransition.end()){
                cerr << "ERROR!can't find transition!"<<endl;
                exit(-1);
            }
            transition[tier->second].isreturn = true;
        }
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
        ret_tag = "*int";//unsolve pointer
    }
    else
        func = p->child->next->child->child->child->place;

    //construct begin_P,end_P,begin_T

    string begin_P = gen_P();
    p->matched_P = begin_P;
    Add_Place(begin_P,controlflag,0,true,func + begin_suffix,true,false);
    mapFunction.insert(make_pair(func + begin_suffix,begin_P));
    string begin_T = gen_T();
    Add_Transition(begin_T,"",func + begin_suffix);
    vector<string> enter;
    enter.push_back(begin_T);
    set_enter_T(begin_P,enter);

    Add_Arc(begin_P,begin_T,construct_arcexpstr(tid_str,"","",""),true,control);

    string end_P = gen_P();
    Add_Place(end_P,controlflag,0,true,func+ end_suffix,true,false);
    mapFunction.insert(make_pair(func + end_suffix,end_P));
//    vector<string> correspond_P;
//    correspond_P.push_back(end_P);
//    set_correspond_P(begin_P,correspond_P);

    //set ret_tag
//    if(ret_tag!="void")
        mapFunction.insert(make_pair(begin_P,ret_tag));

    //construct parameter_P

//    if (p->child->child->type == POINTER || p->child->next->child->type == POINTER)
//    {
//        //do not support pointer now
//    }
//    else
//    {

        gtree *direct_declarator;
        if(p->child->type == DECLARATION_SPECIFIERS && p->child->next->type == DECLARATOR
            && p->child->next->child->type == POINTER)
            direct_declarator = p->child->next->child->next;
        else if(p->child->type == DECLARATION_SPECIFIERS && p->child->next->type == DECLARATOR
        && p->child->next->child->type == DIRECT_DECLARATOR)
            direct_declarator = p->child->next->child;
        else{
            cerr << "ERROR!There is something wrong with function:"<<p->place<<endl;
            exit(-1);
        }
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
//    }

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
//    string left = p->place;
//    string left_P = find_P_name(left,base);
    string Exp1,Exp2,id,exp_tmp,left_P;

    exp_tmp = translate_exp2arcexp(cpn,exp,base);

    if(p->child->type == POSTFIX_EXPRESSION && p->child->child && p->child->child->type == POSTFIX_EXPRESSION
    && p->child->child->next && p->child->child->next->place == "["){
        //array or pointer

        string index;
        bool ispointer,isglobal;
        int size;
        id = p->child->child->place;
        left_P = find_P_name(id,base);
        ispointer = find_v_ispointer(id,base);
        isglobal = find_v_isglobal(id,base);
        index = p->child->child->next->next->place;
        index = translate_exp2arcexp(cpn,index,base);

        auto idpiter = cpn->mapPlace.find(left_P);
        id = cpn->place[idpiter->second].expression;

        string tid_flag;
        if(isglobal)
            tid_flag = "";
        else
            tid_flag = tid_str;

        create_assignment(cpn,left_P,T,index,tid_flag,exp_tmp,false);
//        if(!ispointer) {
//            size = find_v_size(id, base);
//
//            for (int i = 0; i < size; i++) {
//                if (i != 0)
//                    Exp2.append("++");
//                string tmp;
//                tmp = construct_arcexpstr(id + to_string(i), to_string(i), id + to_string(i) + id_suffix,
//                                          tid_flag);
//                Exp2.append(tmp);
//            }
//            Exp1 = Exp2;
//
//            string readcase, writecase;
//            writecase = readcase = CaseFlag + index;
//            for (int i = 0; i < size; i++) {
//                string tmp1, tmp2;
//                tmp1 = construct_arcexpstr(id + to_string(i), to_string(i), id + to_string(i) + id_suffix,
//                                           tid_flag);
//                tmp2 = construct_arcexpstr(exp_tmp, to_string(i), id + to_string(i) + id_suffix,
//                                           tid_flag);
//                readcase.append(":");
//                readcase.append(to_string(i));
//                readcase.append("=>");
//                readcase.append(tmp1);
//                writecase.append(":");
//                writecase.append(to_string(i));
//                writecase.append("=>");
//                writecase.append(tmp2);
//            }
//            readcase.append(":default;");
//            writecase.append(":default;");
//            Exp1.append("++");
//            Exp1.append(writecase);
//            Exp1.append("--");
//            Exp1.append(readcase);
//        }
//        else{
//            Exp1 = construct_arcexpstr(id,"",id+id_suffix,tid_flag);
//            Exp2 = Exp1;
//            for(auto iter = place_size.begin();iter != place_size.end();iter++){
//                auto giter = place_isglobal.find(iter->first);
//                if(giter == place_isglobal.end()){
//                    cerr<<"ERROR!place_isglobal and place_size don't match!"<<endl;
//                    exit(-1);
//                }
//                string tid_flag1;
//                if(giter->second)
//                    tid_flag1 = "";
//                else
//                    tid_flag1 = tid_str;
//
//                auto piter = cpn->mapPlace.find(iter->first);
//                if(piter == cpn->mapPlace.end()){
//                    cerr<<"ERROR!can't find place!"<<endl;
//                    exit(-1);
//                }
//                string Exp_tmp1,Exp_tmp2,tmp,sub_id = cpn->place[piter->second].expression;
//                string writecase,readcase;
//                readcase = writecase = CaseFlag + id + "+" + index;
//
//
//                if(iter->second>1) {
//                    for(unsigned short j = 0;j < iter->second;j++){
//                        writecase.append(":");
//                        writecase.append(sub_id + to_string(j) + id_suffix);
//                        writecase.append("=>");
//                        writecase.append(construct_arcexpstr(exp_tmp,to_string(j),sub_id + to_string(j) + id_suffix,tid_flag1));
//                        readcase.append(":");
//                        readcase.append(sub_id + to_string(j) + id_suffix);
//                        readcase.append("=>");
//                        readcase.append(construct_arcexpstr(sub_id + to_string(j), to_string(j), sub_id + to_string(j) + id_suffix,
//                                                        tid_flag1));
//                    }
//                    readcase.append(":default;");
//                    writecase.append(":default;");
//                    Exp_tmp1 = readcase;
//                    Exp_tmp2 = writecase;
//                }
//                else{
//                    tmp = construct_arcexpstr(sub_id, "", sub_id + id_suffix, tid_flag1);
//                    Exp_tmp1 += tmp;
//                    writecase.append(":");
//                    writecase.append(sub_id + id_suffix);
//                    writecase.append("=>");
//                    writecase.append(construct_arcexpstr(exp_tmp,"",sub_id + id_suffix,tid_flag1));
//                    writecase.append(":");
//                    writecase.append(tmp);
//                    writecase.append(";");
//                    Exp_tmp2 = writecase;
//                }
//                cpn->Add_Arc_override(T,iter->first,Exp_tmp2,false,remain,true);
//                cpn->Add_Arc_override(iter->first,T,Exp_tmp1,true,remain,true);
//            }
//        }
//        cpn->Add_Arc_override(T,left_P,Exp1,false,write,false);
//        cpn->Add_Arc_override(left_P,T,Exp2,true,write,false);
    }
    else{
        //not array

        id = p->place;
        left_P = find_P_name(id,base);
        bool ispointer,isglobal;
        ispointer = find_v_ispointer(id,base);
        isglobal = find_v_isglobal(id,base);
        auto idpiter = cpn->mapPlace.find(left_P);
        id = cpn->place[idpiter->second].expression;

        string tid_flag;
        if(isglobal)
            tid_flag = "";
        else
            tid_flag = tid_str;

        create_assignment(cpn,left_P,T,"",tid_flag,exp_tmp,true);
//        Exp1 = construct_arcexpstr(exp_tmp,"",id + id_suffix,tid_flag);
//        Exp2 = construct_arcexpstr(id,"",id + id_suffix,tid_flag);
//
//        cpn->Add_Arc_override(T,left_P,Exp1,false,write,false);
//        cpn->Add_Arc_override(left_P,T,Exp2,true,write,false);
    }


//    cpn->Add_Arc_override(T,left_P,,false,write);
//    cpn->Add_Arc_override(left_P,T,,true,write);
//    string V = left;
//    auto piter = cpn->mapPlace.find(left_P);
//    cpn->Add_Variable(left,cpn->place[piter->second].initMarking.tid,cpn->place[piter->second].initMarking.sid);
//    auto iter = cpn->mapTransition.find(T);
//    if(iter == cpn->mapTransition.end())
//    {
//        cout<<"can't find control_T in create_PDNet"<<endl;
//        exit(-1);
//    }
//    cpn->transition[iter->second].relvars.insert(id);
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
    while(statement && statement->type != STATEMENT)
        statement = statement->parent;
    if(!statement || statement->is_processed)
        return;
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
    gtree *com = p;
    while (com->type != COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;
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
            string thread_v = paras[0], thread_func = paras[2],para_pass = paras[3];
            if (thread_v[0] == '&')
                thread_v = thread_v.substr(1);
            if(para_pass[0] == '&')
                para_pass = para_pass.substr(1);
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
            string newtid = translate_exp2tid(this,thread_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0], func_begin_P, construct_arcexpstr(newtid,"","",""), false, control);

            //add copy

//            Add_Arc(tid_store_P,enter_T[0],oldtid,true,data);
//            Add_Arc(enter_T[0],tid_store_P,oldtid + "+" +to_string(tid_increment),false,data);

            string statement_T = get_enter_T(statement_P)[0];
            Triple triple;
            triple.first = statement_T;
            triple.second = newtid;



            //passing paras
            if(para_pass != "NULL") {

                create_connect(this, enter_T[0], para_pass, base);
                auto piter = mapPlace.find(func_begin_P);
                if(piter == mapPlace.end()){
                    cerr << "ERROR!can't find Place!"<<endl;
                    exit(-1);
                }
                if(place[piter->second].para_list.size()!=1){
                    cerr << "ERROR!pthread_create's target function should have 1 parameter!but now have:"<<place[piter->second].para_list.size()<<endl;
                    exit(-1);
                }
                string exp_tmp = translate_exp2arcexp(this,para_pass,base);
                triple.third = place[piter->second].para_list[0].second + ":" + exp_tmp;
//                create_assignment(this,place[piter->second].para_list[0].second,enter_T[0],"",tid_str,exp_tmp,true);
            }

            threadcreatearr.push_back(triple);

            //cor_P
            string thread_idexp;
            auto pos = thread_v.find("[");
            if(pos != string::npos)
                thread_idexp = thread_v.substr(0,pos);
            else
                thread_idexp = thread_v;
            if(join_create.find(thread_idexp) == join_create.end())
                join_create.insert(make_pair(thread_idexp,statement_P));

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

            string jointid = translate_exp2tid(this,thread_v,base);
            Add_Arc(func_end_P,enter_T[0], construct_arcexpstr(jointid,"","",""), true, control);

            //cor_P
            string thread_idexp;
            auto pos = thread_v.find("[");
            if(pos != string::npos)
                thread_idexp = thread_v.substr(0,pos);
            else
                thread_idexp = thread_v;
            auto mapiter = join_create.find(thread_idexp);
            if(mapiter == join_create.end()){
                cerr << "ERROR!pthread_join doesn't have correspond create!"<<endl;
                exit(-1);
            }
            vector<string> cor_P = get_correspond_P(mapiter->second);
            cor_P.push_back(statement_P);
            set_correspond_P(mapiter->second,cor_P);
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
            Add_Arc(enter_T[0], last_func_end, construct_arcexpstr(tid_str,"","",""), false,executed);
        }
        else if(p->child->place == "pthread_mutex_init")
        {
            string mutex_v = paras[0];
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = construct_arcexpstr(mutex_v.substr(pos+1,mutex_v.length()-pos-2),"","","");
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = construct_arcexpstr("0","","","");
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_name(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,Exp,false,control);
        }
        else if(p->child->place == "pthread_mutex_lock")
        {
            string mutex_v = paras[0];
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = construct_arcexpstr(mutex_v.substr(pos+1,mutex_v.length()-pos-2),"","","");
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = construct_arcexpstr("0","","","");
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_name(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(mutex_P,enter_T[0],Exp,true,control);

            gtree *tr = statement->parent->next;

        }
        else if(p->child->place == "pthread_mutex_unlock")
        {
            string mutex_v = paras[0];
            if(mutex_v[0]=='&')
                mutex_v = mutex_v.substr(1);
            string Exp;
            auto pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp = construct_arcexpstr(mutex_v.substr(pos+1,mutex_v.length()-pos-2),"","","");
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp = construct_arcexpstr("0","","","");
            }
            gtree *com = p;
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string mutex_P = find_P_name(mutex_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],mutex_P,Exp,false,control);
        }
        else if(p->child->place == "pthread_cond_signal")
        {
            string cond_v = paras[0];
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            gtree *com = p;
            string Exp;
            auto pos = cond_v.find("[");
            if(pos != string::npos){
                Exp = construct_arcexpstr(cond_v.substr(pos+1,cond_v.length()-pos-2),"","","");
                cond_v = cond_v.substr(0,pos);
            }
            else{
                Exp = construct_arcexpstr("0","","","");
            }
            while(com->type!=COMPOUND_STATEMENT)
                com = com->parent;
            string base = com->place;

            string cond_P = find_P_name(cond_v,base);
            vector<string> enter_T = get_enter_T(statement_P);
            Add_Arc(enter_T[0],cond_P,Exp,false,control);
        }
        else if(p->child->place == "pthread_cond_wait")
        {
            string cond_v = paras[0],mutex_v = paras[1];
            if (cond_v[0] == '&')
                cond_v = cond_v.substr(1);
            if(mutex_v[0] == '&')
                mutex_v = mutex_v.substr(1);
            string Exp_mutex,Exp_cond;
            auto pos = cond_v.find("[");
            if(pos != string::npos){
                Exp_cond = construct_arcexpstr(cond_v.substr(pos+1,cond_v.length()-pos-2),"","","");
                cond_v = cond_v.substr(0,pos);
            }
            else{
                Exp_cond = construct_arcexpstr("0","","","");
            }
            pos = mutex_v.find("[");
            if(pos != string::npos){
                Exp_mutex = construct_arcexpstr(mutex_v.substr(pos+1,mutex_v.length()-pos-2),"","","");
                mutex_v = mutex_v.substr(0,pos);
            }
            else{
                Exp_mutex = construct_arcexpstr("0","","","");
            }
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

            Add_Place(P1,controlflag,0,true,"signalP1",true,false);
            Add_Place(P2,controlflag,0,true,"signalP2",true,false);
            Add_Place(P3,controlflag,0,true,"signalP3",true,false);
            Add_Transition(T1,"","signalT1");
            Add_Transition(T2,"","signalT2");

            //construct arcs
            Add_Arc(enter_T[0],P1,construct_arcexpstr(tid_str,"","",""),false,control);
            Add_Arc(enter_T[0],P3,construct_arcexpstr(tid_str,"","",""),false,executed);
            Add_Arc(enter_T[0],mutex_P,Exp_mutex,false,control);
            Add_Arc(cond_P,T1,Exp_cond,true,control);
            Add_Arc(P1,T1,construct_arcexpstr(tid_str,"","",""),true,control);
            Add_Arc(T1,P2,construct_arcexpstr(tid_str,"","",""),false,control);
            Add_Arc(mutex_P,T2,Exp_mutex,true,control);
            Add_Arc(P2,T2,construct_arcexpstr(tid_str,"","",""),true,control);
            Add_Arc(P3,T2,construct_arcexpstr(tid_str,"","",""),true,control);

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
        else if(p->child->place == "pthread_mutex_destroy")
            ;
    }
    else if(exist_in(lib_func_type,p->child->place)){
        // get statement_P and paras
        gtree *statement = p;
        while (statement->type != STATEMENT)
            statement = statement->parent;
        string statement_P = statement->matched_P;

        statement->is_processed = true;//have been processed

        vector<string> paras;
        paras = extract_paras(p);
        if(p->child->place == "printf")
            ;
        else if(p->child->place == "malloc"){

            string tag,tmp_size;
            int size;
            string para = paras[0];
            auto pos_start = para.find("sizeof(");
            if(pos_start == string::npos){
                cerr << "malloc should be the standard form:malloc(sizeof(type)*num)!"<<endl;
                exit(-1);
            }
            auto pos_end = para.find(")",pos_start);
            tag = para.substr(pos_start + 7,pos_end - pos_start - 7);

            if(tag == "pthread_t")
                return;
            auto mul_pos = para.find("*");
            if(mul_pos == string::npos)
                size = 1;
            else {
                if (mul_pos < pos_start)
                    tmp_size = para.substr(0, mul_pos);
                else
                    tmp_size = para.substr(mul_pos + 1);
                if(isdigit(tmp_size[0]))
                    size = atoi(tmp_size.c_str());
                else
                    size = malloc_maxsize;
            }
            string malloc_P = gen_P();
            Add_Place(malloc_P,tag,size,false,"malloc",true,false);

//            string malloc_v_P = gen_P();
//            Add_Place(malloc_v_P,"int",1,false,"malloc_v",)

            string statement_T = get_enter_T(statement_P)[0];
//            create_assignment(this,alloc_store_P,statement_T,"","",)

            //Here arcexp is solid, remain update
            string allocid_str = "allocid";
            string allocid_Exp1 = construct_arcexpstr(allocid_str,"","","");
            string allocid_Exp2 = construct_arcexpstr(allocid_str + "+" + to_string(size),"","","");
            Add_Arc(alloc_store_P,statement_T,allocid_Exp1,true,write);
            Add_Arc(statement_T,alloc_store_P,allocid_Exp2,false,write);
            string Exp;
            if(size > 1) {
                for (unsigned int i = 0; i < size; i++) {
                    if (i != 0)
                        Exp += "++";
                    Exp += construct_arcexpstr("0", to_string(i), allocid_str + "+" + to_string(i), "");
                }
            }
            else
                Exp += construct_arcexpstr("0", "", allocid_str, "");
            Add_Arc(statement_T, malloc_P, Exp, false, allocwrite);
            gtree *assignment = statement;
            string left_id;
            while(assignment->type != ASSIGNMENT_EXPRESSION)
                assignment = assignment->child;
            if(assignment->child->type == UNARY_EXPRESSION)
                left_id = assignment->child->place;
            else{
                cerr << "malloc should be the standard form:malloc(sizeof(type)*num)!"<<endl;
                exit(-1);
            }
            string left_P = find_P_name(left_id,base);
            bool isglobal = find_v_isglobal(left_id,base);
            string tid_flag;
            if(isglobal)
                tid_flag = "";
            else
                tid_flag = tid_str;
            create_assignment(this,left_P,statement_T,"",tid_flag,allocid_str,true);
        }
        else if(p->child->place == "calloc"){
            string tag,tmp_size;
            int size;
            string para1 = paras[0],para2 = paras[1];
            auto pos_start = para2.find("sizeof(");
            if(pos_start == string::npos){
                cerr << "calloc should be the standard form:calloc(num,sizeof(type))!"<<endl;
                exit(-1);
            }
            auto pos_end = para2.find(")",pos_start);
            tag = para2.substr(pos_start + 7,pos_end - pos_start - 7);
            if(tag == "pthread_t")
                return;
            size = atoi(para1.c_str());
            string calloc_P = gen_P();
            Add_Place(calloc_P,tag,size,false,"calloc",true,false);

//            string malloc_v_P = gen_P();
//            Add_Place(malloc_v_P,"int",1,false,"malloc_v",)

            string statement_T = get_enter_T(statement_P)[0];
//            create_assignment(this,alloc_store_P,statement_T,"","",)

            //Here arcexp is solid, remain update
            string allocid_str = "allocid";
            string allocid_Exp1 = construct_arcexpstr(allocid_str,"","","");
            string allocid_Exp2 = construct_arcexpstr(allocid_str + "+" + to_string(size),"","","");
            Add_Arc(alloc_store_P,statement_T,allocid_Exp1,true,write);
            Add_Arc(statement_T,alloc_store_P,allocid_Exp2,false,write);
            string Exp;
            if(size > 1) {
                for (unsigned int i = 0; i < size; i++) {
                    if (i != 0)
                        Exp += "++";
                    Exp += construct_arcexpstr("0", to_string(i), allocid_str + "+" + to_string(i), "");
                }
            }
            else
                Exp += construct_arcexpstr("0", "", allocid_str, "");
            Add_Arc(statement_T, calloc_P, Exp, false, allocwrite);
            gtree *assignment = statement;
            string left_id;
            while(assignment->type != ASSIGNMENT_EXPRESSION)
                assignment = assignment->child;
            if(assignment->child->type == UNARY_EXPRESSION)
                left_id = assignment->child->place;
            else{
                cerr << "calloc should be the standard form:calloc(num,sizeof(type))!"<<endl;
                exit(-1);
            }
            string left_P = find_P_name(left_id,base);
            bool isglobal = find_v_isglobal(left_id,base);
            string tid_flag;
            if(isglobal)
                tid_flag = "";
            else
                tid_flag = tid_str;
            create_assignment(this,left_P,statement_T,"",tid_flag,allocid_str,true);
        }
        else if(p->child->place == "strcpy"){
            string para1 = paras[0],para2 = paras[1];
            if(para2[0] != '"' || para2[para2.size()-1] != '"'){
                cerr << "ERROR!strcpy must match the standard form:strcpy(variable,\"str\""<<endl;
                exit(-1);
            }
            para2 = para2.substr(1,para2.length()-2);

            string id = para1;
            string left_P = find_P_name(id,base);


            string statement_T = get_enter_T(statement_P)[0];
            string tmp_value;
            string tid_flag;
            bool isglobal = find_v_isglobal(id,base);
            if(isglobal)
                tid_flag = "";
            else
                tid_flag = tid_str;

            bool ispointer = find_v_ispointer(id,base);

            auto piter = mapPlace.find(left_P);
            if(piter == mapPlace.end()){
                cerr << "ERROR!can't find place!"<<endl;
                exit(-1);
            }
            string real_id = place[piter->second].expression;
            string Exp_read,Exp_write;

            if(ispointer){
                Triple triple;
                triple.first = statement_T;
                triple.second = para2;
                triple.third = real_id;
                strcpypointer.push_back(triple);

                string Exp = construct_arcexpstr(real_id,"",real_id + id_suffix,tid_flag);
                Add_Arc(statement_T,left_P,Exp,false,write);
                Add_Arc(left_P,statement_T,Exp,true,write);
            }
            else{

                for(unsigned int i=0;i<para2.length();i++){
                    tmp_value = "'" + to_string(para2[i]) + "'";
                    if(i>0){
                        Exp_read += "++";
                        Exp_write += "++";
                    }
                    Exp_write += construct_arcexpstr(tmp_value,to_string(i),real_id + id_suffix,tid_flag);;
                    Exp_read += construct_arcexpstr(real_id,to_string(i),real_id + id_suffix,tid_flag);
                }
                Add_Arc(statement_T,left_P,Exp_write,false,write);
                Add_Arc(left_P,statement_T,Exp_read,true,write);
            }
        }
    }
    else {
//process all the variable as local variable
//        bool isglobal = find_v_isglobal(base);
        string tid_flag;
//        if(isglobal)
//            tid_flag = "";
//        else
            tid_flag = tid_str;

        string call_func_id = p->child->place, call_func_P_begin, call_func_P_end;

        //construct call structure
        string call_P = gen_P();
        string call_T = gen_T();
        p->matched_P = call_P;
        Add_Place(call_P, controlflag, 0, true, p->child->place + call_suffix,true,false);
        Add_Transition(call_T, "", p->child->place + call_suffix);
        Add_Arc(call_P, call_T, construct_arcexpstr(tid_str,"","",""), true, control);

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

//                para += para_suffix;
                auto piter = mapPlace.find(begin_place->para_list[i].second);
                if(piter == mapPlace.end()){
                    cerr<<"ERROR!can't find Place!"<<endl;
                    exit(-1);
                }

                //now just consider normal type
                if(place[piter->second].initMarking.tid != productsort){
                    cerr<<"ERROR!variable place's sort must be productsort!"<<endl;
                    exit(-1);
                }

//                type v_tid = sorttable.productsort[place[piter->second].initMarking.sid].sortid[0].tid;
//                SORTID v_sid = sorttable.productsort[place[piter->second].initMarking.sid].sortid[0].sid;


//                string exp1,exp2;
//                exp1 = construct_arcexpstr(translate_exp2arcexp(this,v[i],base), "", para + id_suffix, tid_flag);
//                exp2 = construct_arcexpstr(para, "", para + id_suffix, tid_flag);
//                Add_Arc(call_T, begin_place->para_list[i].second, exp1, false, write);
//                Add_Arc(begin_place->para_list[i].second, call_T, exp2, true, write);
                create_connect(this, call_T, v[i], base);
                string exp_tmp = translate_exp2arcexp(this,v[i],base);
                create_assignment(this,begin_place->para_list[i].second,call_T,"",tid_flag,exp_tmp,true);
            }
        }

        //construct return places
        auto iter = mapFunction.find(call_func_P_begin);
        if(iter == mapFunction.end()){
            cerr << "can't find call_func_P_begin!"<<endl;
            exit(-1);
        }
        string ret_tag = iter->second;
        bool ret_ispointer = false;
        if(ret_tag[0]=='*')
            ret_ispointer = true;
        if(ret_tag!="void") {


            string return_P = gen_P();
            string return_v;
//            gtree *primary_expression = p;
//            while(primary_expression->type != PRIMARY_EXPRESSION)
//                primary_expression = primary_expression->child;
            return_v = p->place;

            // return place is a local place

            Add_Place(return_P, ret_tag, 1, false, return_v,false,ret_ispointer);
            auto piter = mapPlace.find(return_P);
            if(piter == mapPlace.end()){
                cerr << "ERROR!<can't find place!"<<endl;
                exit(-1);
            }
            return_v = place[piter->second].expression;

            // return place is a little unique, because it may appear in one statement with two place
            // so we should identify them
            p->place = return_v;
            gtree *tmp_pointer = p->parent,*tmp_child;

            while(tmp_pointer->type != EXPRESSION_STATEMENT
            && tmp_pointer->type != ITERATION_STATEMENT
            && tmp_pointer->type != SELECTION_STATEMENT
            && tmp_pointer->type != JUMP_STATEMENT){
                tmp_child = tmp_pointer->child;
                string tmp_str = "";
                while(tmp_child){
                    tmp_str.append(tmp_child->place);
                    tmp_child = tmp_child->next;
                }
                tmp_pointer->place = tmp_str;
                tmp_pointer = tmp_pointer->parent;
            }
//            tmp_child = tmp_pointer->child;
//            string tmp_str = "";
//            while(tmp_child){
//                tmp_str.append(tmp_child->place);
//                tmp_child = tmp_child->next;
//            }
//            tmp_pointer->place = tmp_str;


            // connect return_P with all return statements
            vector<Triple> returns = get_returns(call_func_P_begin);
            for (unsigned int i = 0; i < returns.size(); i++) {
                string Exp1,Exp2,exp_tmp;

                exp_tmp = translate_exp2arcexp(this,returns[i].second,returns[i].third);

                create_assignment(this,return_P,returns[i].first,exp_tmp,tid_flag,exp_tmp,true);
//                Exp1 = construct_arcexpstr(exp_tmp,"",return_v + id_suffix,tid_flag);
//                Exp2 = construct_arcexpstr(return_v,"",return_v + id_suffix,tid_flag);
//                Add_Arc(returns[i].first, return_P, Exp1, false, write);
//                Add_Arc(return_P, returns[i].first, Exp2, true, write);
            }


            string last_P;
            vector<string> last_T;
            gtree *last_call = p->parent;
            while (!judge_call_postfix_expression(last_call) && last_call->type != STATEMENT) {

                last_call = last_call->parent;
            }
            if(!last_call->matched_P.empty()) {
                last_P = last_call->matched_P;
                last_T = get_enter_T(last_P);
                for (unsigned int j = 0; j < last_T.size(); j++) {
                    string Exp;

                    //Here cannot create_connection, because create_connection will not connect return_v
//                    create_connect(this, last_T[j], return_v, base);
                    Exp = construct_arcexpstr(return_v,"",return_v + id_suffix,tid_flag);
                    Add_Arc(last_T[j], return_P, Exp, false, data);
                    Add_Arc(return_P, last_T[j], Exp, true, data);

//                    auto piter = mapPlace.find(return_P);
//                    Add_Variable(return_v, place[piter->second].initMarking.tid,place[piter->second].initMarking.sid);

                }
            }
            else{
                cerr << "ERROR!can't find last_call!"<<endl;
                exit(-1);
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
        Add_Arc(call_T, call_func_P_begin, construct_arcexpstr(tid_str,"","",""), false, call_enter);
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
    Add_Place(executed_P,controlflag,0,true,executed_P_name,true,false);
    for (unsigned int i = 0; i < source_T.size(); i++) {
        auto tier = mapTransition.find(source_T[i]);
        if(tier == mapTransition.end()){
            cerr << "ERROR!can't find trans!"<<endl;
            exit(-1);
        }
        if(!transition[tier->second].isreturn)
            Add_Arc(source_T[i], executed_P, construct_arcexpstr(tid_str,"","",""), false, executed);
    }
    for (unsigned int i = 0; i < target_T.size(); i++)
        Add_Arc(executed_P, target_T[i], construct_arcexpstr(tid_str,"","",""), true, control);
    return executed_P;
}

void CPN::handle_iter_sel(gtree *p) {

    gtree *statement = p->parent;
    string control_P = statement->matched_P;
    string control_T1 = get_enter_T(control_P)[0];
    string control_T2 = get_enter_T(control_P)[1];

    //set guard , must be set in backward traverse, because the expression might change(in case of return_v)

    gtree *com = p;
    while(com->type != COMPOUND_STATEMENT)
        com = com->parent;
    string base = com->place;
    string condition = p->child->next->next->place;
    Set_Guard(control_T1,condition,base);
    string condition_op = opposite_all(condition);
    Set_Guard(control_T2,condition_op,base);

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
    vector<string> call_P1 = get_call_P(child_P1);
    if(!call_P1.empty())
        child_P1 = call_P1[0];
    vector<string> temp;
    temp.push_back(control_T1);
    Add_Arc(control_T1,child_P1,construct_arcexpstr(tid_str,"","",""),false,control);
    Add_executed_P(temp,get_enter_T(child_P1));
    if(statement2) {
        child_P2 = statement2->matched_P;
        vector<string> call_P2 = get_call_P(child_P2);
        if(!call_P2.empty())
            child_P2 = call_P2[0];
        Add_Arc(control_T2,child_P2,construct_arcexpstr(tid_str,"","",""),false,control);
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
            Add_Arc(falseexit[i],control_P,construct_arcexpstr(tid_str,"","",""),false,executed);
            string enter_P = statement->enter_P;//just consider call one time now
            if(enter_P != "")
                Add_Arc(falseexit[i],enter_P,construct_arcexpstr(tid_str,"","",""),false,executed);
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

        Add_Arc(enter_T[0], iter2->second, construct_arcexpstr(tid_str,"","",""), false, executed);
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
            Add_Arc_override(v[i], func_end, construct_arcexpstr(tid_str,"","",""), false,call_exit,false);
    }

    // func_T to para
    string Exp;
    Exp = "NULL";

    auto piter = mapPlace.find(begin_P);
    if(piter == mapPlace.end()){
        cerr << "ERROR!can't find begin_P!"<<endl;
        exit(-1);
    }
    for(unsigned int i=0;i<place[piter->second].para_list.size();i++) {
        Add_Arc(begin_T, place[piter->second].para_list[i].second, Exp, false, write);
        Add_Arc(place[piter->second].para_list[i].second,begin_T,  Exp, true, write);
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
        Add_Arc(control_T, last_func_end, construct_arcexpstr(tid_str,"","",""), false, call_exit);

        auto iter_begin = mapFunction.find(identifier + begin_suffix);
        if (iter_begin == mapFunction.end()) {
            cout << "can't find end_P in create_CPN" << endl;
            exit(-1);
        }
        string last_func_begin = iter_begin->second;
        gtree *com = p;
        while(com->type!=COMPOUND_STATEMENT)
            com = com->parent;
        string base = com->place;
        Add_returns(last_func_begin,control_T,expression,base);

        create_connect(this, control_T, expression, base);
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
        cpn->Add_Arc(last_T,next_P,construct_arcexpstr(tid_str,"","",""),false,call_connect);

        auto iter = cpn->mapPlace.find(last_P);
        string call_exp = cpn->place[iter->second].expression;
        string_replace(call_exp,call_suffix,end_suffix);
        string end_exp = call_exp;
        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = iter_end->second;
        cpn->Add_Arc(last_end_P,next_T,construct_arcexpstr(tid_str,"","",""),true,control);
    }
    last_P = call_P[i];
    next_P = statement_P;
    last_T = cpn->get_enter_T(last_P)[0];
    cpn->Add_Arc(last_T,next_P,construct_arcexpstr(tid_str,"","",""),false,call_connect);
    for(unsigned int j=0;j<statement_T.size();j++)
    {
        next_T = statement_T[j];
        auto iter = cpn->mapPlace.find(last_P);
        string call_exp = cpn->place[iter->second].expression;
        string_replace(call_exp,call_suffix,end_suffix);
        string end_exp = call_exp;
        auto iter_end = cpn->mapFunction.find(end_exp);
        last_end_P = iter_end->second;
        cpn->Add_Arc(last_end_P,next_T,construct_arcexpstr(tid_str,"","",""),true,control);
    }
}

void CPN::visit_condition(gtree *p){
    string logic_or_exp,exp1,exp2;
    logic_or_exp = p->child->place;
    exp1 = p->child->next->next->place;
    exp2 = p->child->next->next->next->next->place;
    p->place = CaseFlag + logic_or_exp + ":" + to_string(0) + "=>" + exp2 + ":" + exp1 + ";";

    gtree *tmp_pointer = p->parent,*tmp_child;
    while(tmp_pointer->type != EXPRESSION){
        tmp_child = tmp_pointer->child;
        string tmp_str = "";
        while(tmp_child){
            tmp_str.append(tmp_child->place);
            tmp_child = tmp_child->next;
        }
        tmp_pointer->place = tmp_str;
        tmp_pointer = tmp_pointer->parent;
    }
    tmp_child = tmp_pointer->child;
    string tmp_str = "";
    while(tmp_child){
        tmp_str.append(tmp_child->place);
        tmp_child = tmp_child->next;
    }
    tmp_pointer->place = tmp_str;
}

void CPN::Traverse_ST(gtree *p){
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
    else if(p->type == REMAIN && p->place == "?")
        visit_condition(p->parent);//handle(condition?exp1:exp2) , because of cuttree cannot judge condition_exp simplely

    Traverse_ST(p->child);

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

    Traverse_ST(p->next);
}

//*create PDNet by traverse ST *//

void CPN::create_PDNet(gtree *p) {
    Traverse_ST(p);


    for(unsigned int i=0;i<strcpypointer.size();i++){
        create_strcpypointerarc(this,strcpypointer[i]);
    }
    for(unsigned int i=0;i<threadcreatearr.size();i++){
        create_threadcreate(this,threadcreatearr[i]);
    }
    for(unsigned int i=0;i<writepointer.size();i++){
        create_writepointerarc(this,writepointer[i].first,writepointer[i].second,writepointer[i].third);
    }
    //deal with writepointer first, read can be override
    for(unsigned int i=0;i<readpointer.size();i++){
        create_readpointerarc(this,readpointer[i]);
    }
    readpointer.clear();
    writepointer.clear();
    strcpypointer.clear();
    threadcreatearr.clear();
    join_create.clear();
    place_size.clear();
    place_isglobal.clear();

    SortTable::psptr = 0;
    gen_P_num=0;
    gen_T_num=0;
    id_ptr = 1000;
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
        string tmp_exp = arc[i].arc_exp.exp;
        int pos = 0;
        pos = tmp_exp.find('\"');
        while(pos != string::npos){
            tmp_exp = tmp_exp.substr(0,pos) + "\\" + tmp_exp.substr(pos);
            pos = tmp_exp.find('\"',pos + 2);
        }
        if (arc[i].arcType == executed)
            out << arc[i].source_id << "->" << arc[i].target_id << "[color=\"red\",label=\""<< tmp_exp<<"\"]" << endl;
        else if (arc[i].arcType == write)
            out << arc[i].source_id << "->" << arc[i].target_id << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        else if (arc[i].arcType == data)
            out << arc[i].source_id << "->" << arc[i].target_id << "[color=\"blue\",label=\""<< tmp_exp<<"\"]" << endl;
        else if (arc[i].arcType == remain)
            ;
        else
            out << arc[i].source_id << "->" << arc[i].target_id << "[label=\""<< tmp_exp<<"\"]" << endl;
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
        place[placecount++]=cpnet->place[iter->second];
//        memcpy(&place[placecount++],&cpnet->place[iter->second],sizeof(CPlace));
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
        transition[transitioncount++] = cpnet->transition[iter->second];
//        memcpy(&transition[transitioncount++],&cpnet->transition[iter->second],sizeof(CTransition));
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
            i--;
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
            Add_Arc(father_T,after_P[i],construct_arcexpstr(tid_str,"","",""),false,control);
        }
        vector<string> first_after_T = get_enter_T(after_P[0]);
        for(unsigned int i=0;i<first_after_T.size();i++)
            Add_Arc(pre_exe_P,first_after_T[i],construct_arcexpstr(tid_str,"","",""),true,control);

        if(iter_flag == true){
            vector<string> last_T = get_enter_T(after_P[after_P.size()-1]);
            for(unsigned int i=0;i<last_T.size();i++)
                Add_Arc(last_T[i],pre_exe_P,construct_arcexpstr(tid_str,"","",""),false,executed);
        }
    }
    delete_compound(p->child);
    delete_compound(p->next);
}

void CPN::Add_Variable(condition_tree_node *tree, type tid, SORTID sid, unsigned short down, unsigned short up) {
    if(!tree)
        return;
    if(tree->node_type == Tuple){
        Add_Variable(tree->left,tid,sid,down,down);
        Add_Variable(tree->right,tid,sid,down+1,up);
        if(tree->condition)
            Add_Variable(tree->condition,Integer,0,0,0);
    }
    else{
        Add_Variable(tree->left,tid,sid,down,up);
        Add_Variable(tree->right,tid,sid,down,up);
        if(tree->condition)
            Add_Variable(tree->condition,Integer,0,0,0);
    }

    if(tree->node_type == variable){
        if(down != up){
            cerr << "ERROR!in Add_Variable,variable's up and down don't consistent!"<<endl;
            exit(-1);
        }
        type sub_tid ;
        SORTID sub_sid;

        if(tid == productsort){
            sub_tid = sorttable.productsort[sid].sortid[down].tid;
            sub_sid = sorttable.productsort[sid].sortid[down].sid;
        }
        else{
            sub_tid = tid;
            sub_sid = sid;
        }
        tree->node_name = tree->value = Add_Variable(tree->node_name,sub_tid,sub_sid);
    }

}

void CPN::setmaintoken() {
    auto iter = mapFunction.find("main" + begin_suffix);
    if(iter == mapFunction.end())
    {
        cout<<"can't find main func"<<endl;
        exit(-1);
    }
    string main_P = iter->second;
    auto iter2 = mapPlace.find(main_P);
    if(iter2 == mapPlace.end())
    {
        cout<<"can't find main place"<<endl;
        exit(-1);
    }
    if(place[iter2->second].initMarking.tokenQ->next == NULL) {
        Tokens *token = new Tokens;
        if(TID_colorset == String)
            token->color = new StringSortValue;
        else if(TID_colorset == Integer)
            token->color = new IntegerSortValue;
        else if(TID_colorset == Real)
            token->color = new RealSortValue;
        token->color->setColor(init_tid);
        token->tokencount = 1;
        place[iter2->second].initMarking.insert(token);
    }
}

NUM_t MultiSet::tokennum() {
    int sum = 0;
    Tokens *p=this->tokenQ->next;
    while(p) {
        sum += p->tokencount;
        p=p->next;
    }
}

void ProductSortValue::setColor(Product_t cid, SORTID sid) {
//    vector<mapsort_info> sortid = sorttable.productsort[sid].sortid;
    int size = sorttable.productsort[sid].sortnum;
    for(unsigned int i=0;i<size;i++){
        //actually productsort can't be a subsort of productsort
        if(sorttable.productsort[sid].sortid[i].tid == productsort){
//            int psnum = sorttable.productsort[sortid[i].sid].sortnum;
//            Product_t tmp;
//            tmp = new_ProductColor(sortid[i].sid);
//            cid[i]->getColor(tmp,sortid[i].sid);
//            valueindex[i]->setColor(tmp,sortid[i].sid);
//            delete[] tmp;
            cerr<<"ERROR!product sort can't be a subsort of productsort!"<<endl;
            exit(-1);
        }
        else if(sorttable.productsort[sid].sortid[i].tid == Integer){
            Integer_t tmp;
            cid[i]->getColor(tmp);
            valueindex[i]->setColor(tmp);
        }
        else if(sorttable.productsort[sid].sortid[i].tid == Real){
            Real_t tmp;
            cid[i]->getColor(tmp);
            valueindex[i]->setColor(tmp);
        }
        else if(sorttable.productsort[sid].sortid[i].tid == String){
            String_t tmp;
            cid[i]->getColor(tmp);
            valueindex[i]->setColor(tmp);
        }
    }
}

void ProductSortValue::getColor(Product_t cid, SORTID sid) {
//    vector<mapsort_info> sortid = sorttable.productsort[sid].sortid;
    int size = sorttable.productsort[sid].sortnum;
    for(unsigned int i=0;i<size;i++){
        if(sorttable.productsort[sid].sortid[i].tid == productsort){
            Product_t tmp;
            int psnum = sorttable.productsort[sid].sortnum;
            tmp = new SortValue*[psnum];
            valueindex[i]->getColor(tmp,sorttable.productsort[sid].sortid[i].sid);
            cid[i]->setColor(tmp,sorttable.productsort[sid].sortid[i].sid);
            delete[] tmp;
            cerr<<"ProductSort can't consist of productsort!"<<endl;
            exit(-1);
        }
        else if(sorttable.productsort[sid].sortid[i].tid == Integer){
            Integer_t tmp;
            valueindex[i]->getColor(tmp);
            cid[i]->setColor(tmp);
        }
        else if(sorttable.productsort[sid].sortid[i].tid == Real){
            Real_t tmp;
            valueindex[i]->getColor(tmp);
            cid[i]->setColor(tmp);
        }
        else if(sorttable.productsort[sid].sortid[i].tid == String){
            String_t tmp;
            valueindex[i]->getColor(tmp);
            cid[i]->setColor(tmp);
        }
    }
}

ProductSortValue::ProductSortValue(SORTID sid) {
    sortnum = sorttable.productsort[sid].sortnum;
    valueindex = new SortValue*[sortnum];
    for(unsigned int i=0;i<sortnum;i++)
        if(sorttable.productsort[sid].sortid[i].tid == Integer)
            valueindex[i] = new IntegerSortValue;
        else if(sorttable.productsort[sid].sortid[i].tid == Real)
            valueindex[i] = new RealSortValue;
        else if(sorttable.productsort[sid].sortid[i].tid == String)
            valueindex[i] = new StringSortValue;
}
