//
// Created by cc on 2020/10/10.
//

#ifndef PDNET_CHECKER_CPN_H
#define PDNET_CHECKER_CPN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <string.h>
#include "AST.h"
#include "y.tab.h"
#include "base.h"
//#include "condition_tree.h"
#include "expression_tree.h"
#include <gperftools/tcmalloc.h>
#include <gperftools/malloc_extension.h>
#define H1FACTOR 13//hash offset

using namespace std;

typedef unsigned short VARID;
typedef unsigned short SORTID;
//typedef unsigned short COLORID;
typedef unsigned int NUM_t;
typedef int Integer_t;
typedef double Real_t;
typedef string String_t;
typedef unsigned short MS_size_t;
typedef short token_count_t;
typedef unsigned int index_t;
typedef unsigned short ID_t;
typedef String_t TID_t;
typedef Integer_t INDEX_t;
typedef unsigned short SORTNUM_t;

class SortValue;
typedef SortValue** Product_t;
class CPN;
class SortTable;
struct mapsort_info;
extern SortTable sorttable;
//extern NUM_t placecount;
//extern NUM_t transitioncount;
//extern NUM_t varcount;
extern CPN *cpnet;

enum type{dot,finiteintrange,productsort,usersort,Integer,Real,String};
enum Arc_Type{executed,control,call_enter,call_exit,data,write,call_connect,remain};//data is equal to read


/*========================Sort==========================*/
class Sort
{
public:
    type mytype;
    virtual ~Sort(){};
};

class ProductSort:public Sort
{
public:
    string id;
    SORTNUM_t sortnum;
    vector<string> sortname;
    vector<string> membername;//used for 'struct,union' in program
    vector<mapsort_info> sortid;
    bool hastid;
    bool hasindex;
    ProductSort(){hastid=hasindex=false;}
};

//UserSort is a user declared enumerable sort
//class UserSort:public Sort
//{
//public:
//    string id;
//    SHORTNUM feconstnum;
//    vector<string> cyclicenumeration;
//    map<string,COLORID> mapValue;
//
//    COLORID getColorIdx(string color) {
//        map<string,COLORID>::iterator iter;
//        iter = mapValue.find(color);
//        if(iter!=mapValue.end()) {
//            return iter->second;
//        }
//        else {
//            cerr<<"Can not find color"<<color<<"in colorset"<<this->id<<endl;
//            exit(-1);
//        }
//    }
//};

//DotSort
//class DotSort:public Sort
//{
//public:
//    //empty
//};
//
////IntegerSORT
//class IntegerSort:public Sort
//{
//public:
//    //empty
//};
//
////StringSORT
//class StringSort:public Sort
//{
//public:
//    //empty
//};

//class FiniteIntRange:public Sort
//{
//public:
//    string id;
//    int start;
//    int end;
//};

/*==========================SortValue=========================*/
/*==================Abstract Base Class==================*/
class SortValue
{
public:
//    type mytype;
    virtual ~SortValue(){};
//    virtual void setColor(COLORID cid)=0;
//    virtual void getColor(COLORID &cid)=0;
    virtual void setColor(Product_t cid,SORTID sid)=0;
    virtual void setColor(Integer_t cid)=0;
    virtual void setColor(Real_t cid)=0;
    virtual void setColor(String_t cid)=0;
    virtual void getColor(Product_t cid,SORTID sid)=0;
    virtual void getColor(Integer_t &cid)=0;
    virtual void getColor(Real_t &cid)=0;
    virtual void getColor(String_t &cid)=0;
};

/*====================ProductSortValue================*/
class ProductSortValue:public SortValue
{
private:
    //this is a index;
    Product_t valueindex;
    unsigned int sortnum;
public:
    ProductSortValue(SORTID sid);
    ~ProductSortValue() {
        for(unsigned int i=0;i<sortnum;i++)
            delete valueindex[i];
        delete [] valueindex;
    }

//    void setColor(COLORID cid){};
//    void getColor(COLORID &cid){};
    virtual void setColor(Product_t cid,SORTID sid);
    virtual void getColor(Product_t cid,SORTID sid);
    virtual void setColor(Integer_t cid){};
    virtual void setColor(Real_t cid){};
    virtual void setColor(String_t cid){};
    virtual void getColor(Integer_t &cid){};
    virtual void getColor(Real_t &cid){};
    virtual void getColor(String_t &cid){};
};

/*===================UserSortValue=================*/
//class UserSortValue:public SortValue
//{
//private:
//    //this is a index;
//    COLORID colorindex;
//public:
//    virtual void setColor(COLORID cid) {colorindex = cid;}
//    virtual void getColor(COLORID &cid) {cid = colorindex;}
//    virtual void setColor(type_union *cid,int size){};
//    virtual void getColor(type_union *cid,int size){};
//    virtual void setColor(int cid){};
//    virtual void setColor(double cid){};
//    virtual void setColor(char *cid){};
//    virtual void getColor(int &cid){};
//    virtual void getColor(double &cid){};
//    virtual void getColor(char *cid){};
//    ~UserSortValue(){};
//};

/*==================FIRSortValue==================*/
//class FIRSortValue:public SortValue
//{
//public:
//    int value;
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
//    virtual void setColor(type_union *cid,int size){};
//    virtual void getColor(type_union *cid,int size){};
//    virtual void setColor(int cid){};
//    virtual void setColor(double cid){};
//    virtual void setColor(char *cid){};
//    virtual void getColor(int &cid){};
//    virtual void getColor(double &cid){};
//    virtual void getColor(char *cid){};
//    ~FIRSortValue(){};
//};

/*==================DotSortValue==================*/
//class DotSortValue:public SortValue
//{
//private:
//    COLORID value;
//public:
//    void setColor(COLORID cid){value = cid;};
//    void getColor(COLORID &cid){cid = value;};
//    virtual void setColor(COLORID *cid,int size){};
//    virtual void getColor(COLORID *cid,int size){};
//};

/*==================Integer SortValue==================*/
class IntegerSortValue:public SortValue
{
private:
    Integer_t value;
public:
    IntegerSortValue(){}
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
    virtual void setColor(Product_t cid,SORTID sid){};
    virtual void getColor(Product_t cid,SORTID sid){};
    virtual void setColor(Integer_t cid){value = cid;};
    virtual void setColor(Real_t cid){};
    virtual void setColor(String_t cid){};
    virtual void getColor(Integer_t &cid){cid = value;};
    virtual void getColor(Real_t &cid){};
    virtual void getColor(String_t &cid){};
};

/*==================Real SortValue==================*/
class RealSortValue:public SortValue
{
private:
    Real_t value;
public:
    RealSortValue(){}
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
    virtual void setColor(Product_t cid,SORTID sid){};
    virtual void getColor(Product_t cid,SORTID sid){};
    virtual void setColor(Integer_t cid){};
    virtual void setColor(Real_t cid){value = cid;};
    virtual void setColor(String_t cid){};
    virtual void getColor(Integer_t &cid){};
    virtual void getColor(Real_t &cid){cid = value;};
    virtual void getColor(String_t &cid){};
};

/*==================String SortValue==================*/
class StringSortValue:public SortValue
{
private:
    String_t value;
public:
    StringSortValue(){}
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
    virtual void setColor(Product_t cid,SORTID sid){};
    virtual void getColor(Product_t cid,SORTID sid){};
    virtual void setColor(Integer_t cid){};
    virtual void setColor(Real_t cid){};
    virtual void setColor(String_t cid){value = cid;};
    virtual void getColor(Integer_t &cid){};
    virtual void getColor(Real_t &cid){};
    virtual void getColor(String_t &cid){cid = value;};
    ~StringSortValue(){};
};


class Tokens
{
public:
    SortValue *color;
    token_count_t tokencount;
    Tokens *next;

    Tokens() {color = NULL;tokencount=0;next=NULL;}
    ~Tokens() {
        if(color!=NULL)
            delete color;
    }
    void initiate(token_count_t tc,type sort,SORTID sid);
    void init_a_token(type tid,Integer_t value);
    void init_a_token(type tid,Real_t value);
    void init_a_token(type tid,String_t value);
};

//**Multiset contains tokens with the same type**//
class MultiSet
{
public:
    type tid;
    SORTID sid;
    Tokens *tokenQ;//queue contains head node
    MS_size_t color_count;
    index_t hash_value;

    MultiSet(){tokenQ = new Tokens;tokenQ->next=NULL;color_count = 0;}
    ~MultiSet(){
        Tokens *temp = tokenQ->next,*temp1;
        while(temp)
        {
            temp1 = temp->next;
            delete temp;
            temp = temp1;
        }
        delete tokenQ;
    }
    index_t Hash();
    void clear();
    void insert(Tokens *token);
    bool operator>=(const MultiSet &ms1);
    bool operator==(const MultiSet &ms);
    void operator=(const MultiSet &ms);
    void MINUS(MultiSet &ms);
    void PLUS(MultiSet &ms);

    void Exp2MS(CPN *cpn,condition_tree_node *tree,unsigned short down,unsigned short up,bool in_bool_op);

    NUM_t tokennum();

};

/*===========================================================*/
typedef struct mapsort_info
{
    //its index in corresponding vector
    SORTID sid;
    type tid;
} MSI;

//typedef struct mapcolor_info  //only used for cyclicenumeration
//{
//    SORTID sid;
//    COLORID cid;
//} MCI;

class SortTable
{
public:
    vector<ProductSort> productsort;
//    vector<UserSort> usersort;
//    vector<FiniteIntRange> finitintrange;
    bool hasdot;

    static SORTID psptr;

public:
    SortTable(){hasdot = false;}
    //first string is the sortname,the second MSI is this sort's information;
    map<string,MSI> mapSort;
//    map<string,MCI> mapColor;
//    SORTID getPSid(string PSname);
//    SORTID getUSid(string USname);
//    SORTID getFIRid(string FIRname);
    friend class CPN;
};


struct Variable
{
    string id;
    SORTID sid;
    SortValue *value;//assume variable is not a productsort
    type tid;

    bool operator == (const Variable &var) const {

    }
    bool operator < (const Variable &var) const {
//        if(this->vid < var.vid)
//            return true;
//        else
//            return false;
    }
};

/*========================Net Element========================*/
typedef struct CPN_Small_Arc
{
    index_t idx;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool onlydot;
} CSArc;

struct Triple{
    string first;
    string second;
    string third;
};

typedef struct CPN_Place
{
    string id;
//    type tid;
//    SORTID sid;
    MultiSet initMarking;
    vector<CSArc>producer;
    vector<CSArc>consumer;

    //***PDNet added start***/
    bool control_P;
    bool is_mutex;
    bool is_cond;
    bool is_executed;
    bool is_pointer;
    string expression;
    vector<string> enter;
    vector<string> exit;
    vector<string> false_exit;
    vector<string> call_P;
    vector<string> correspond_P;
    vector<Triple> returns;//for func begin P
    vector<pair<string,string>> para_list;
    //***PDNet added end***/

    CPN_Place(){expression="";}
    void operator=(CPN_Place &plc){
        //copy except producer and consumer
        id = plc.id;
        initMarking = plc.initMarking;
        expression=plc.expression;
        enter = plc.enter;
        exit = plc.exit;
        false_exit = plc.false_exit;
        call_P = plc.call_P;
        correspond_P = plc.correspond_P;
        returns = plc.returns;
        para_list = plc.para_list;
        is_executed = plc.is_executed;
        control_P = plc.control_P;
        is_mutex = plc.is_mutex;
        is_cond = plc.is_cond;
    }

    void printTokens(string &str);
//    ~CPN_Place(){
//        delete initMarking;
//    }
} CPlace;

typedef struct CPN_Transition
{
    string id;
    condition_tree guard;
    bool hasguard;
    vector<CSArc> producer;
    vector<CSArc> consumer;
    void operator=(CPN_Transition &trans){
        //copy except produce and consumer
        id = trans.id;
        guard = trans.guard;
        hasguard = trans.hasguard;
    }
//    set<string> relvars;
    //vector<Variable> relvararray;

    //***PDNet added start***/

    //***PDNet added end***/
} CTransition;

typedef struct CPN_Arc
{
    string id;
    bool isp2t;
    string source_id;
    string target_id;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool onlydot;

    void operator=(CPN_Arc &arc){
        id = arc.id;
        isp2t = arc.isp2t;
        source_id = arc.source_id;
        target_id = arc.target_id;
        arc_exp = arc.arc_exp;
        arcType = arc.arcType;
        onlydot = arc.onlydot;
    }
//    MultiSet arc_MS;
//    ~CPN_Arc() {
//        arc_exp.destructor(arc_exp.root);
//    }
} CArc;

/*========================Color_Petri_Net=======================*/
class CPN
{
public:
    CPlace *place;
    CTransition *transition;
    CArc *arc;

    Variable *vartable;
    NUM_t placecount;
    NUM_t transitioncount;
    NUM_t arccount;
    NUM_t varcount;
    map<string,index_t> mapPlace;
    map<string,index_t> mapTransition;
    map<string,VARID> mapVariable;
    map<string,string> mapFunction;//map some unique function expression with their matched places , for example begin,end  and especially ret_tag
    map<string,string> mapPthread;//map some pthread information with their matched places
    map<string,string> mapJoin;//map pthread_create with join

    //***PDNet added start***/
//    void CTN_cal(condition_tree_node *CTN);
//    void CT2MS(condition_tree ct,MultiSet &ms);
//    void CTN2MS(condition_tree_node *ctn,MultiSet &ms);
//    void CTN2COLOR(condition_tree_node *ctn,MultiSet &ms);
//    void get_tuplecolor(condition_tree_node *ctn,vector<SortValue *> &color,vector<mapsort_info> sortid);
    //***PDNet added end***/

    CPN();


    //***PDNet added start***//
    void getDecl(gtree *tree);
    void initDecl();
    void init();
    void Add_Place(string id,string Type_name,int size,bool control_P,string exp,bool isglobal,bool is_pointer);
    void Add_Transition(string id,string guard,string exp);
    void Add_Arc(string source,string target,string exp,bool sourceP,Arc_Type arcType);
    void Add_Arc_override(string source,string target,string exp,bool sourceP,Arc_Type arcType,bool be_overrided);
    string Add_Variable(string id,type tid,SORTID sid);
    void Add_Variable(condition_tree_node *tree,type tid,SORTID sid,unsigned short down,unsigned short up);
    void init_Place(string id,Tokens *token,type tid,SORTID sid);
    void init_Place(string id, string init_str);
//    void set_Global(string id);
    void process_declaration(gtree *declaration,string base);
    void process_declarator(gtree *declarator, string tag, string base, bool para);
    void process_para_type_list(gtree *para_type_list, string base_Vname);
    void set_producer_consumer();
    void set_call_P(string p_name,vector<string> enter_P);
    vector<string> get_call_P(string p_name);
    void set_correspond_P(string p_name,vector<string> correspond_P);
    vector<string> get_correspond_P(string p_name);
    void set_exit_T(string p_name,vector<string> exit_T);
    vector<string> get_exit_T(string p_name);
    void set_falseexit_T(string p_name,vector<string> exit_T);
    vector<string> get_falseexit_T(string p_name);
    void set_enter_T(string p_name,vector<string> enter_T);
    vector<string> get_enter_T(string p_name);
    void Add_returns(string p_name,string return_T,string exp,string base);
    vector<Triple> get_returns(string p_name);
//    void set_ispointer(string p_name,bool ispointer);
//    bool get_ispointer(string p_name);

    //***PDNet added end***//
    ~CPN();
    void print_CPN(string filename);
    string Add_executed_P(vector<string> source_T,vector<string> target_T);
    void copy_childNet(CPN *cpn,vector<string> places,vector<string> transitions);
    void create_PDNet(gtree *tree);

    //visit function
    void create_v_table(gtree *p);
    void visit_declaration(gtree *p);
    void visit_statement(gtree *p);
    void visit_function(gtree *p);
    void supply_compound(gtree *p);
    //void build_control_flow(gtree *p);
    void handle_expression(gtree *p);
    void handle_call(gtree *p);
    void handle_iter_sel(gtree *p);
    void supply_func(gtree *p);
    void supply_jump_statement(gtree *p);
    void delete_compound(gtree *p);
private:
};



class var_type{
private:
    string tag;
    string name;
    bool pointer_flag;
    int dimension;
    int *size_table;
public:
    var_type(){size_table = NULL;}
    ~var_type(){if(size_table)delete size_table;}
    void init(string t,string id,bool pointer_f,int dim,int size[]){
        tag = t;
        name = id;
        pointer_flag = pointer_f;
        dimension = dim;
        size_table = new int[dim];
        for(int i=0;i<dim;i++)
            size_table[i] = size[i];
    }
    bool get_pointer_flag(){return pointer_flag;}
    int *get_size_table(){return size_table;}
    int get_dimension(){return dimension;}
    string get_tag(){return tag;}
    string get_name(){return name;}
};

//void Tokenscopy(Tokens &t1,const Tokens &t2,type tid,int PSnum=0);
extern string executed_P_name;
extern string arr_suffix;
extern map<string,type> map_build_in_type;
extern vector<string> pthread_type,pthread_func_type;
void two_phrase_slicing(CPN *cpn, vector<string> place, vector<string> &final_P, vector<string>&final_T);
void post_process(CPN *cpn,CPN *cpn_slice,vector<string> transitions);
void init_pthread_type();
extern Product_t new_ProductColor(SORTID sid);
extern string translate_exp2arcexp(CPN *cpn,string s,string base);
extern void color_copy(type tid,SORTID sid,SortValue *src,SortValue *des);
extern type TID_colorset;
#endif //PDNET_CHECKER_CPN_H